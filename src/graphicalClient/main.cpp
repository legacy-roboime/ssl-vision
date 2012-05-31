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
\file    main.cpp
\brief   Main Entry point for the graphicalClient binary`
\author  Joydeep Biswas (C) 2011
*/
//========================================================================

#include <stdio.h>
#include <QtGui>
#include <QApplication>
#include "soccerview.h"
#include "timer.h"

GLSoccerView *view;

bool runApp = true;

class MyThread : public QThread
{  
protected:
  void run()
  {
    static const double minDuration = 0.01; //100FPS
    RoboCupSSLClient client;
    client.open(false);
    SSL_WrapperPacket packet;
    while(runApp) {
      if (client.receive(packet)) {
        if (packet.has_detection()) {
          SSL_DetectionFrame detection = packet.detection();
          view->updateDetection(detection);
        }
        if (packet.has_geometry()) {
          //TODO: update geometry
        }
      }
      Sleep(minDuration);
    }
  }
  
public:
  MyThread(QObject* parent = 0){}
  ~MyThread(){}
};

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  view = new GLSoccerView();
  view->show();
  MyThread thread;
  thread.start();
  int retVal = app.exec();
  runApp = false;
  thread.wait();
  return retVal;
}

#ifdef HAVE_WINDOWS
#include <QMainWindow>

int CALLBACK WinMain(
    __in  HINSTANCE hInstance,
    __in  HINSTANCE hPrevInstance,
    __in  LPSTR lpCmdLine,
    __in  int nCmdShow)
{
    wchar_t **wargv;//wargv is an array of LPWSTR (wide-char string)
    char **argv;
    int argc;
    // we have to convert each arg to a char*
    wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    // here and then we allocate one more cell to make it a NULL-end array
    argv = (char **)calloc(argc + 1, sizeof(char *));
    for(int i = 0; i < argc; i++) {
        size_t origSize = wcslen(wargv[i]) + 1;
        size_t converted = 0;
        argv[i] = (char *)calloc(origSize + 1, sizeof(char));
        // we use the truncate strategy, it won't handle non latin chars correctly
        // but hey we don't even use args, right?
        wcstombs_s(&converted, argv[i], origSize, wargv[i], _TRUNCATE);
    }
    return main(argc, argv);
}
#endif

