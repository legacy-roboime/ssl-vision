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
  \file    capturemf.h
  \brief   C++ Interface: CaptureMF
  \author  Jan Segre, 2012
*/
//========================================================================
#ifdef USE_MMF
#ifndef CAPTUREMF_H
#define CAPTUREMF_H
#include "captureinterface.h"

#ifdef VDATA_NO_QT
class CaptureMF : public CaptureInterface
{
public:
    CaptureMF(VarList *settings);
#else
#include <QMutex>
class CaptureMF : public QObject, public CaptureInterface
{
    Q_OBJECT
protected:
    QMutex mutex;

public slots:
    void changed(VarType *group);

public:
    CaptureMF(VarList *settings, QObject *parent=0);
    void mvc_connect(VarList *group);
#endif
    ~CaptureMF();

    virtual RawImage getFrame();
    virtual bool     isCapturing();
    virtual void     releaseFrame();
    virtual bool     startCapture();
    virtual bool     stopCapture();
    virtual bool     resetBus();
    virtual bool     copyAndConvertFrame(const RawImage &, RawImage &);
    virtual string   getCaptureMethodName() const;

private:
    struct CaptureMFImpl *impl;
};

#endif
#endif
