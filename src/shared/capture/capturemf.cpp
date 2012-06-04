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

//for debugging:
#include <iostream>
using namespace std;

struct CaptureMFImpl
{
    VarList *conv_opts;
    VarList *capt_opts;
    VarStringEnum *dev_opts;
    IMFActivate **ppDevices;
    UINT32 count;
    //TODO

    CaptureMFImpl(VarList *settings) {
        settings->addChild(conv_opts = new VarList("Conversion Settings"));
        settings->addChild(capt_opts = new VarList("Capture Settings"));
        capt_opts->addChild(dev_opts = new VarStringEnum("Device"));
    }

    bool enumerateDevices() {
        HRESULT hr = S_OK;
        IMFAttributes *pAttributes = NULL;

        // Initialize an attribute store to specify enumeration parameters.
        hr = MFCreateAttributes(&pAttributes, 1);
        if(FAILED(hr)) return false;

        // Ask for source type = video capture devices.
        hr = pAttributes->SetGUID(
            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
        );
        if(FAILED(hr)) return false;

        // Enumerate devices.
        hr = MFEnumDeviceSources(pAttributes, &ppDevices, &count);
        if(FAILED(hr)) return false;
        else return true;
    }

    void populateSettings() {
        HRESULT hr = S_OK;

        for(UINT32 i = 0; i < count; i++) {
            WCHAR *szFriendlyName = NULL;

            hr = ppDevices[i]->GetAllocatedString(
                MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,
                &szFriendlyName,
                NULL
            );
            if(FAILED(hr)) break;

            // Since we are not using Unicode, we must convert szFriendlyName
            wstring wstr(szFriendlyName);
            dev_opts->addItem(string(wstr.begin(), wstr.end()));
        }
    }
};

CaptureMF::CaptureMF(VarList *_settings) :
    CaptureInterface(_settings),
    impl(new CaptureMFImpl(settings))
{
    if(impl->enumerateDevices())
        impl->populateSettings();
    else
        cerr << "Couldn't enumerate devices." << endl;
    //TODO
}

CaptureMF::~CaptureMF()
{
    delete impl;
}

RawImage CaptureMF::getFrame()
{
    //TODO
    return RawImage();
}

bool CaptureMF::isCapturing()
{
    //TODO
    return false;
}

void CaptureMF::releaseFrame()
{
    //TODO
}

bool CaptureMF::startCapture()
{
    return impl->enumerateDevices();
}

bool CaptureMF::stopCapture()
{
    //TODO
    return true;
}

bool CaptureMF::resetBus()
{
    //TODO
    return false;
}

void CaptureMF::readAllParameterValues()
{
    //TODO
}

string CaptureMF::getCaptureMethodName() const
{
    return "Media Foundation";
}

#endif
