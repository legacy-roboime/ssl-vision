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

struct CaptureMFImpl
{
    //TODO
};

CaptureMF::CaptureMF(VarList *_settings) :
    CaptureInterface(_settings),
    impl(new CaptureMFImpl())
{
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
    //TODO
    return false;
}

bool CaptureMF::stopCapture()
{
    //TODO
    return false;
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
