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
#include <mferror.h>
#include <iostream>
#include "capturemf_helper.h"
using namespace std;

string toString(WCHAR *pWString)
{
    wstring wstr(pWString);
    return string(wstr.begin(), wstr.end());
}

/// Capture from Microsoft Media Foundation

// Opaque implementation:

struct CaptureMFImpl
{
    // VarType settings, displayed to the user
    VarList *conversionSettings;
    VarList *captureSettings;
    VarStringEnum *deviceSetting;
    VarStringEnum *sizeSetting;

    // Maps to ease retriving values from settings
    map<string, VarType> deviceMap;
    map<string, VarType> sizeMap;

    // Media Foundation classes
    IMFActivate **devices;
    IMFSourceReader *mediaReader;
    IMFMediaType *mediaType;
    IMFMediaBuffer *mediaBuffer;
    IMFSample *currentSample;
    LONGLONG timeStamp;
    bool locked;
    vector<string> deviceNames;
    UINT32 count;
    WCHAR *symLink;
    UINT32 symLinkLength;
    UINT32 width;
    UINT32 height;
    // this flag is used to prevent releasing
    // when already released.
    //TODO: implement a better solution, tearDown should be idempotent
    bool busy;

    CaptureMFImpl() :
        currentSample(0),
        mediaBuffer(NULL),
        timeStamp(0),
        locked(false),
        devices(NULL),
        mediaReader(NULL),
        mediaType(NULL),
        deviceNames(vector<string>()),
        count(0),
        symLink(NULL),
        symLinkLength(0),
        width(0),
        height(0),
        busy(false)
    {
        conversionSettings = new VarList("Conversion Settings");
        captureSettings = new VarList("Capture Settings");

        string defaultDevice = "";
        if(enumerateDevices()) {
            if(deviceNames.size() > 0) {
                defaultDevice = deviceNames[0];
            }
        } else {
            cerr << "Couldn't enumerate devices." << endl;
        }

        deviceSetting = new VarStringEnum("Device", defaultDevice);
        sizeSetting = new VarStringEnum("Size", "");
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

        //TODO: Release the current device, if any.

        // Create the media source for the device.
        hr = device->ActivateObject(
            __uuidof(IMFMediaSource),
            (void**)&source
        );
        if(FAILED(hr)) goto end;

        //hr = EnumerateCaptureFormats(source);
        //if(FAILED(hr)) goto end;

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
            &mediaReader
        );
        if(FAILED(hr)) goto end;

        //TODO: Try to find a suitable output type.
        hr = mediaReader->GetCurrentMediaType(
            0,//is it ok to just try the first one?
            &mediaType
        );
        if(FAILED(hr)) goto end;

        // Get width and height from mediaType
        hr = MFGetAttributeSize(
            mediaType,
            MF_MT_FRAME_SIZE,
            &width,
            &height
        );
        if(FAILED(hr)) goto end;

        // Ask for the first sample.
        DWORD streamFlags;
        IMFSample *sample;
        hr = mediaReader->ReadSample(
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

        SafeRelease(&source);
        SafeRelease(&attributes);

        return SUCCEEDED(hr);
    }

    void tearDown() {
        if(!busy) return;
        SafeRelease(&mediaReader);
        SafeRelease(&mediaType);
        CoTaskMemFree(symLink);
        symLink = NULL;
        symLinkLength = 0;
        width = 0;
        height = 0;
        busy = false;
    }

    bool getSample() {
        if(!mediaReader) goto fail;

        HRESULT hr = S_OK;

        // Read sample from reader.
        DWORD streamFlags;
        hr = mediaReader->ReadSample(
            MF_SOURCE_READER_FIRST_VIDEO_STREAM,
            0,
            NULL,
            &streamFlags,
            &timeStamp,
            &currentSample
        );
        if(FAILED(hr)) goto fail;
        if(!currentSample) goto fail;

        return true;

    fail:
        cerr << "Error retrieving sample." << endl;
        return false;
    }

    void releaseSample() {
        SafeRelease(&currentSample);
        if(locked && mediaBuffer) {
            mediaBuffer->Unlock();
            locked = false;
        }
        SafeRelease(&mediaBuffer);
    }

    RawImage sampleToRawImage() {
        if(!currentSample) goto fail;

        HRESULT hr = S_OK;

        // Get the first buffer on the sample.
        hr = currentSample->GetBufferByIndex(0, &mediaBuffer);
        if(FAILED(hr)) goto fail;

        // Acquire acces to the memory
        BYTE *buffer;
        DWORD maxLength;
        DWORD bufferLength;
        hr = mediaBuffer->Lock(
            &buffer,
            &maxLength,
            &bufferLength
        );

        if(FAILED(hr)) goto fail;

        // Build image from buffer
        else {
            locked = true;
            RawImage image;
            image.setWidth(width);
            image.setHeight(height);
            //TODO: properly convert timeStamp to double and set the time
            //image.setTime(...)
            image.setData(buffer);
            //TODO: set the right color format
            image.setColorFormat(COLOR_YUV422_YUYV);
            return image;
        }

    fail:
        cerr << "Failed to build RawImage()" << endl;
        return RawImage();
    }

};

// Public interface:

#ifdef VDATA_NO_QT
CaptureMF::CaptureMF(VarList *_settings) :
#else
CaptureMF::CaptureMF(VarList *_settings, QObject *parent) :
    QObject(parent),
#endif
    CaptureInterface(_settings),
    impl(new CaptureMFImpl())
{
    settings->addChild(impl->conversionSettings);
    settings->addChild(impl->captureSettings);
}

#ifndef VDATA_NO_QT
void CaptureMF::mvc_connect(VarList *group)
{
    vector<VarType *> v = group->getChildren();
    for (uint i = 0; i < v.size(); i++) {
        connect(
            v[i],
            SIGNAL(wasEdited(VarType *)),
            group,
            SLOT(mvcEditCompleted())
        );
    }
    connect(
        group,
        SIGNAL(wasEdited(VarType *)),
        this,
        SLOT(changed(VarType *))
    );
}

void CaptureMF::changed(VarType *group)
{
    if(group->getType() == VARTYPE_ID_LIST) {
        //TODO
        //writeParameterValues((VarList *)group);
        //readParameterValues((VarList *)group);
    }
}
#endif

CaptureMF::~CaptureMF()
{
    delete impl;
}

RawImage CaptureMF::getFrame()
{
    if(impl->getSample())
        return impl->sampleToRawImage();
    else
        return RawImage();
}

bool CaptureMF::isCapturing()
{
    return impl->busy;
}

void CaptureMF::releaseFrame()
{
    impl->releaseSample();
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

bool CaptureMF::copyAndConvertFrame(const RawImage &fromImage, RawImage &toImage)
{
    //TODO: actually implement something
    //note that only COLOR_YUV422_UYVY, COLOR_YUV422_YUYV and COLOR_RGB8 are supported as output
    /*toImage.setHeight(fromImage.getHeight());
    toImage.setWidth(fromImage.getWidth());
    toImage.setTime(fromImage.getTime());
    toImage.setData(fromImage.getData());
    toImage.setColorFormat(fromImage.getColorFormat());*/
    toImage.deepCopyFromRawImage(fromImage, true);
    return true;
}

string CaptureMF::getCaptureMethodName() const
{
    return "Media Foundation";
}

#endif
