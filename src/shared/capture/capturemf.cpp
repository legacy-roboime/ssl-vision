//========================================================================
//  This software is free: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License Version 3,
//  as published by the Free Software Foundation.
//
//  This software is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  Version 3 in the file COPYING that came with this distribution.
//  If not, see <http://www.gnu.org/licenses/>.
//========================================================================
/*!
  \file    capturemf.cpp
  \brief   C++ Implementation: CaptureMF
  \author  Jan Segre, 2012
*/
//========================================================================
#ifdef USE_MMF
#include "capturemf.h"
#include <Windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <iostream>
using namespace std;

string toString(WCHAR *pWString)
{
    wstring wstr(pWString);
    return string(wstr.begin(), wstr.end());
}

template <class T> void sRelease(T **pp)
{
    if(*pp) (*pp)->Release();
    *pp = NULL;
}

/// Capture from Microsoft Media Foundation

// Opaque implementation:

struct CaptureMFImpl
{
    VarList *conversionSettings;
    VarList *captureSettings;
    VarStringEnum *deviceSetting;
    IMFActivate **devices;
    IMFSourceReader *reader;
    IMFSample *currentSample;
    vector<string> deviceNames;
    UINT32 count;
    WCHAR *symLink;
    UINT32 symLinkLength;
    // this flag is used to prevent releasing
    // when already released.
    //TODO: implement a better solution, tearDown should be idempotent
    bool busy;

    CaptureMFImpl() :
        currentSample(0),
        devices(NULL),
        reader(NULL),
        deviceNames(vector<string>()),
        count(0),
        symLink(NULL),
        symLinkLength(0),
        busy(false)
    {
        conversionSettings = new VarList("Conversion Settings");
        captureSettings = new VarList("Capture Settings");

        string defaultDevice = "";
        if(enumerateDevices()) {
            if(deviceNames.size() > 0)
                defaultDevice = deviceNames[0];
        } else {
            cerr << "Couldn't enumerate devices." << endl;
        }

        deviceSetting = new VarStringEnum("Device", defaultDevice);
        captureSettings->addChild(deviceSetting);
        populateSettings();
    }

    ~CaptureMFImpl() {
        tearDown();//really?
        delete conversionSettings;
        delete captureSettings;
        delete deviceSetting;
        //TODO: check for memory leaks maybe
    }

    bool enumerateDevices() {
        HRESULT hr = S_OK;
        IMFAttributes *attributes = NULL;

        // Initialize an attribute store to specify enumeration parameters.
        hr = MFCreateAttributes(&attributes, 1);
        if(FAILED(hr)) return false;

        // Ask for source type = video capture devices.
        hr = attributes->SetGUID(
            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
        );
        if(FAILED(hr)) return false;

        // Enumerate devices.
        hr = MFEnumDeviceSources(attributes, &devices, &count);
        if(FAILED(hr)) return false;

        // Save list of friendly names
        for(UINT32 i = 0; i < count; i++) {
            WCHAR *friendlyName = NULL;

            hr = devices[i]->GetAllocatedString(
                MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,
                &friendlyName,
                NULL
            );
            if(FAILED(hr)) break;

            // Since we are not using Unicode, we must convert friendlyName to string
            deviceNames.push_back(toString(friendlyName));
        }
        return true;
    }

    void populateSettings() {
        for(UINT32 i = 0; i < deviceNames.size(); i++)
            deviceSetting->addItem(string(deviceNames[i]));
    }

    bool setupDevice(IMFActivate *device) {
        if(busy) return false;
        busy = true;

        HRESULT hr = S_OK;

        IMFMediaSource  *source = NULL;
        IMFAttributes   *attributes = NULL;
        IMFMediaType    *type = NULL;

        //TODO: Release the current device, if any.

        // Create the media source for the device.
        hr = device->ActivateObject(
            __uuidof(IMFMediaSource),
            (void**)&source
        );
        if(FAILED(hr)) goto end;

        // Get the symbolic link.
        hr = device->GetAllocatedString(
            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK,
            &symLink,
            &symLinkLength
        );
        if(FAILED(hr)) goto end;

        // Create an attribute store to hold initialization settings.
        hr = MFCreateAttributes(&attributes, 2);
        if(FAILED(hr)) goto end;
        hr = attributes->SetUINT32(MF_READWRITE_DISABLE_CONVERTERS, TRUE);
        if(FAILED(hr)) goto end;

        // Create the source reader.
        hr = MFCreateSourceReaderFromMediaSource(
            source,
            attributes,
            &reader
        );
        if(FAILED(hr)) goto end;

        //TODO: Try to find a suitable output type.

        // Ask for the first sample.
        DWORD streamFlags;
        IMFSample *sample;
        hr = reader->ReadSample(
            MF_SOURCE_READER_FIRST_VIDEO_STREAM,
            0,
            NULL,
            &streamFlags,
            NULL,
            &sample
        );

    end:
        if(FAILED(hr)) {
            if(source)
                source->Shutdown();
            tearDown();
            busy = false;
        }

        sRelease(&source);
        sRelease(&attributes);
        sRelease(&type);

        return SUCCEEDED(hr);
    }

    void tearDown() {
        if(!busy) return;
        sRelease(&reader);
        CoTaskMemFree(symLink);
        symLink = NULL;
        symLinkLength = 0;
        busy = false;
    }

    RawImage getImage() {
        if(!reader) goto fail;

        HRESULT hr = S_OK;

        // read sample from reader
        DWORD streamFlags;
        LONGLONG timeStamp;
        hr = reader->ReadSample(
            MF_SOURCE_READER_FIRST_VIDEO_STREAM,
            0,
            NULL,
            &streamFlags,
            &timeStamp,
            &currentSample
        );
        if(FAILED(hr)) goto fail;
        if(!currentSample) goto fail;

        //TODO: RawImage from IMFSample
        return RawImage();

    fail:
        cerr << "Error retrieving sample." << endl;
        return RawImage();
    }

    void releaseImage() {
        if(currentSample) sRelease(&currentSample);
    }
};

// Public interface:

CaptureMF::CaptureMF(VarList *_settings) :
    CaptureInterface(_settings),
    impl(new CaptureMFImpl())
{
    settings->addChild(impl->conversionSettings);
    settings->addChild(impl->captureSettings);
}

CaptureMF::~CaptureMF()
{
    delete impl;
}

RawImage CaptureMF::getFrame()
{
    return impl->getImage();
}

bool CaptureMF::isCapturing()
{
    return impl->busy;
}

void CaptureMF::releaseFrame()
{
    impl->releaseImage();
}

bool CaptureMF::startCapture()
{
    impl->enumerateDevices();
    //TODO: call the selected device instead of the first one
    return impl->setupDevice(impl->devices[0]);
}

bool CaptureMF::stopCapture()
{
    impl->tearDown();
    return true;
}

bool CaptureMF::resetBus()
{
    //TODO
    return false;
}

string CaptureMF::getCaptureMethodName() const
{
    return "Media Foundation";
}

#endif
