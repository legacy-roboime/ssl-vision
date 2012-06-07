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

using namespace std;

#include "capturemf_helper.h"

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
    map<string, IMFActivate *> deviceMap;
    map<string, VarType> sizeMap;

    // Media Foundation classes
    IMFSourceReader *mediaReader;
    IMFMediaBuffer *mediaBuffer;
    IMFSample *currentSample;
    UINT32 count;
    WCHAR *symLink;
    UINT32 symLinkLength;

    // To use as a template when generating frames
    RawImage image;

    // Flags
    bool locked;
    // this flag is used to prevent releasing when already released.
    //FIXME: tearDown should be idempotent
    bool busy;

    CaptureMFImpl() :
        currentSample(0),
        mediaBuffer(NULL),
        locked(false),
        mediaReader(NULL),
        count(0),
        symLink(NULL),
        symLinkLength(0),
        busy(false)
    {
        conversionSettings = new VarList("Conversion Settings");
        captureSettings = new VarList("Capture Settings");
        deviceSetting = new VarStringEnum("Device");
        sizeSetting = new VarStringEnum("Size");
        captureSettings->addChild(deviceSetting);

        if(SUCCEEDED(enumerateDevices())) {
            if(deviceMap.size() == 0) {
                cerr << "No device found." << endl;
            }
        } else {
            cerr << "Couldn't enumerate devices." << endl;
        }

    }

    ~CaptureMFImpl() {
        tearDown();//really?
        delete conversionSettings;
        delete captureSettings;
        delete deviceSetting;
        delete sizeSetting;
        //TODO: check for memory leaks maybe?
    }

    HRESULT enumerateDevices() {
        HRESULT hr = S_OK;
        IMFAttributes *attributes = NULL;

        // Initialize an attribute store to specify enumeration parameters.
        hr = MFCreateAttributes(&attributes, 1);
        if(FAILED(hr)) goto done;

        // Ask for source type = video capture devices.
        hr = attributes->SetGUID(
            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
        );
        if(FAILED(hr)) goto done;

        // Enumerate devices.
        IMFActivate **devices;
        hr = MFEnumDeviceSources(attributes, &devices, &count);
        if(FAILED(hr)) goto done;

        // Save list of friendly names
        deviceSetting->setSize(count);
        for(UINT32 i = 0; i < count; i++) {
            WCHAR *friendlyName = NULL;

            hr = devices[i]->GetAllocatedString(
                MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,
                &friendlyName,
                NULL
            );
            if(FAILED(hr)) goto done;

            // Since we are not using Unicode, we must convert friendlyName to string
            string name = toString(friendlyName);

            // Push the device to settings and mapping.
            deviceSetting->setLabel(i, name);
            deviceMap[name] = devices[i];
        }
    done:
        return hr;
    }

    HRESULT setupDevice(IMFActivate *device) {
        if(busy) return E_NOT_VALID_STATE;

        HRESULT hr(S_OK);

        IMFMediaSource *source(NULL);
        IMFAttributes *attributes(NULL);
        IMFMediaType *mediaType(NULL);
        IMFSample *sample(NULL);

        //Release the current device, if any.
        hr = tearDown();
        if(FAILED(hr)) goto done;

        // Now we can be busy
        busy = true;

        // Create the media source for the device.
        hr = device->ActivateObject(
            __uuidof(IMFMediaSource),
            (void**)&source
        );
        if(FAILED(hr)) goto done;

        //hr = EnumerateCaptureFormats(source);
        //if(FAILED(hr)) goto end;

        // Get the symbolic link.
        hr = device->GetAllocatedString(
            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK,
            &symLink,
            &symLinkLength
        );
        if(FAILED(hr)) goto done;

        // Create an attribute store to hold initialization settings.
        hr = MFCreateAttributes(&attributes, 2);
        if(FAILED(hr)) goto done;
        hr = attributes->SetUINT32(MF_READWRITE_DISABLE_CONVERTERS, TRUE);
        if(FAILED(hr)) goto done;

        // Create the source reader.
        hr = MFCreateSourceReaderFromMediaSource(
            source,
            attributes,
            &mediaReader
        );
        if(FAILED(hr)) goto done;

        //TODO: Try to find a suitable output type.
        hr = mediaReader->GetCurrentMediaType(
            0,//is it ok to just try the first one?
            &mediaType
        );
        if(FAILED(hr)) goto done;

        // Get width and height from mediaType
        UINT32 width, height;
        hr = MFGetAttributeSize(
            mediaType,
            MF_MT_FRAME_SIZE,
            &width,
            &height
        );
        if(FAILED(hr)) goto done;
        image.setWidth(width);
        image.setHeight(height);

        // Indicate that we manage the image buffer
        image.setManaged(true);

        // Get color format from mediaType
        //TODO: set the right color format
        GUID subtype;
        hr = mediaType->GetGUID(
            MF_MT_SUBTYPE,
            &subtype
        );
        if(FAILED(hr)) goto done;
        ColorFormat cf = toColorFormat(subtype);
        image.setColorFormat(cf);

        // Ask for the first sample.
        DWORD streamFlags;
        hr = mediaReader->ReadSample(
            MF_SOURCE_READER_FIRST_VIDEO_STREAM,
            0,
            NULL,
            &streamFlags,
            NULL,
            &sample
        );

    done:
        if(FAILED(hr)) {
            if(source)
                source->Shutdown();
            tearDown();
            busy = false;
        }

        SafeRelease(&source);
        SafeRelease(&attributes);
        SafeRelease(&mediaType);
        SafeRelease(&sample);

        return hr;
    }

    HRESULT tearDown() {
        SafeRelease(&mediaReader);
        CoTaskMemFree(symLink);
        symLink = NULL;
        symLinkLength = 0;
        busy = false;
        image = RawImage();
        return S_OK;
    }

    void releaseFrame() {
        SafeRelease(&currentSample);
        if(locked && mediaBuffer) {
            mediaBuffer->Unlock();
            locked = false;
        }
        SafeRelease(&mediaBuffer);
    }

    RawImage getFrame() {
        if(!mediaReader) goto done;

        HRESULT hr = S_OK;

        // Read sample from reader.
        DWORD streamFlags;
        LONGLONG timeStamp;
        hr = mediaReader->ReadSample(
            MF_SOURCE_READER_FIRST_VIDEO_STREAM,
            0,
            NULL,
            &streamFlags,
            &timeStamp,
            &currentSample
        );
        if(FAILED(hr)) goto done;

        // Get the first buffer on the sample.
        hr = currentSample->GetBufferByIndex(0, &mediaBuffer);
        if(FAILED(hr)) goto done;

        // Acquire acces to the memory
        BYTE *buffer;
        DWORD maxLength;
        DWORD bufferLength;
        hr = mediaBuffer->Lock(
            &buffer,
            &maxLength,
            &bufferLength
        );

    done:
        if(SUCCEEDED(hr)) {
            // Build image from buffer
            locked = true;
            //TODO: properly convert timeStamp to double and set the time
            //image.setTime(...)
            image.setData(buffer);
            return image;
        } else {
            cerr << "An error occurred getting a frame." << endl;
            return RawImage();
        }
    }

    IMFActivate *selectedDevice() {
        return deviceMap[deviceSetting->getSelection()];
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
    return impl->getFrame();
}

bool CaptureMF::isCapturing()
{
    return impl->busy;
}

void CaptureMF::releaseFrame()
{
    impl->releaseFrame();
}

bool CaptureMF::startCapture()
{
    impl->enumerateDevices();
    IMFActivate *device = impl->selectedDevice();
    if(device)
        return SUCCEEDED(impl->setupDevice(device));
    else
        return false;
}

bool CaptureMF::stopCapture()
{
    return SUCCEEDED(impl->tearDown());
}

bool CaptureMF::resetBus()
{
    //TODO
    return false;
}

bool CaptureMF::copyAndConvertFrame(const RawImage &fromImage, RawImage &toImage)
{
    toImage.setHeight(fromImage.getHeight());
    toImage.setWidth(fromImage.getWidth());
    toImage.setTime(fromImage.getTime());
    //TODO: implement a conversion
    //note that only COLOR_YUV422_UYVY, COLOR_YUV422_YUYV and COLOR_RGB8 are supported as output
    toImage.setData(fromImage.getData());
    toImage.setColorFormat(fromImage.getColorFormat());
    toImage.setManaged(true);
    return true;
}

string CaptureMF::getCaptureMethodName() const
{
    return "Media Foundation";
}

#endif
