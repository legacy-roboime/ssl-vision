> This software is free: you can redistribute it and/or modify
> it under the terms of the GNU General Public License Version 3,
> as published by the Free Software Foundation.
> 
> This software is distributed in the hope that it will be useful,
> but WITHOUT ANY WARRANTY; without even the implied warranty of
> MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
> GNU General Public License for more details.
> 
> You should have received a copy of the GNU General Public License
> Version 3 in the file COPYING that came with this distribution.
> If not, see &lt;<http://www.gnu.org/licenses/>&gt;.

[ssl-vision](http://github.com/roboime/ssl-vision/)
===================================================

RoboCup Small Size League Shared Vision System
----------------------------------------------

This is a fork which intends to add compatibility for Windows, and
possibly Mac OSX. The official repo is available [here](http://code.google.com/p/ssl-vision/).


Online Documentation
--------------------

To find more in-depth and up-to-date information about SSL-Vision
(including installation and configuration), please visit the [Wiki
Documentation Page](http://code.google.com/p/ssl-vision/wiki/Manual).
 

Software Requirements
---------------------

- cmake >= 2.8
- g++, clang++ or vc++
- QT >= 4.8 with opengl and networking support
- Eigen2 or Eigen3
- Google protocol buffers (protoc)
- OpenGL
- GLU
- libdc1394 >= 2.0 for using IEEE1394 camera, currently only works on linux
- libjpeg
- libpng

To get all of these packages in ubuntu/debian variants, run:

``sudo apt-get install g++ libqt4-dev libeigen2-dev protobuf-compiler libprotobuf-dev libdc1394-22 libdc1394-22-dev cmake``

For gentoo users the following should suffice:

``sudo emerge cmake qt-core qt-gui qt-opengl eigen protobuf opengl libdc1394``


Hardware Requirements
---------------------

The system supports 1394B / Firewire 800, but it's also backward compatible
with 1394A.
Though it's not mandatory and the software can be compiled without support
for camera.


Compilation
-----------

On linux or Mac, build the code by running:

``make``

The project *should* build without errors or warnings.

Alternativley on any system:

```
cd build
cmake .. -i #for interactive mode
```

On Windows it's possible to use:

``make_vcproj``

that script will call cmake-gui.

Also on Mac and Windows one can also use cmake-gui directly, choosing
the project root as the source directory, and build folder as
build directory.


Running
-------

1. Depending on your OS, you might need to ensure that you have
   full access to the firewire devices /dev/fw* This **might**
   require logging in as root.

2. Run the software using the following command:

   ```
   cd bin
   ./vision
   ```

   Note that it's better to go to the ./bin dir first since some .xml's
   will be generated and for organization sake they shouldn't be on
   the root of the project not to mix with the source nor build system.

   Alternatively open it from your file manager, this works for all OS's,
   though on linux you won't see the stdout this way.


Starting to Capture and Setting Parameters
------------------------------------------

Once the software is running, you should see two empty capture frames
on the right, and a data-tree structure on the left.

In this data-structure you can setup your camera parameters,
such as resolution, capture mode, etc.

A quick hint: the text-field below the data-tree allows for
fast search through the data-tree.

See the section of DC1394 parameters below to get an idea of what the
parameters do.

Once you have them set up, you can start capturing by clicking
"Image Capture/Capture Control/Start"
in the data-tree.


DC1394 Parameters
-----------------

If you expand the tree then the capture parameters are in
"Image Capture/DC1394/Capture Settings"
   
Furthermore, conversion settings are in
"Image Capture/DC1394/Conversion Settings"

"convert to mode" should currently be "yuv422"

for best performance, "capture mode" should also be "yuv422",
but can also be a different format such as "yuv411" or "rgb"

Alternatively, if you desire, you can do de-bayering in software,
but this will take extra CPU cycles. To do so, you would set the
capture mode to e.g. "raw8" and the convert mode to "rgb". Finally,
you will need to set "de-bayer" to true, and select the correct
de-bayer pattern and desired method.

Capturing supports both DCAM native modes and Format7 modes.
This is selected in the "capture format" field. Leaving it on
"auto" will attempt native mode first, then format7_0.

By default, ISO800 support is disabled. To enable it, mark the
field "use ISO800" as true.

### ONLINE PARAMETERS ###

Once you start capturing, you should see the realtime video image
on the right.
Furthermore, in the data tree, you should be able to go to the
"Camera Parameters" node which will then be expandable and show
all of your DCAM parameters.

You can adjust all of these parameters in real-time.

Note that the read-out of these parameters from the camera
only happens automatically if "auto refresh params" in the
Capture Control is set to true. Otherwise, you can use
the "re-read params" "Refresh" button to refresh them manually.
For performance reasons, it might make sense to set auto refresh
to false, so the bus is not being flooded with too much control
data and has full bandwidth available for the video streaming.


Storage of Settings and Parameters 
----------------------------------

When you quit the application normally (by closing the window
not thru Ctrl-C) then all settings and parameters will
automatically be written to a set of xml files.

All settings should be automatically restored during the next
program start.

In case the files should ever become corrupted, or the
program refuses to start completely when parsing the XML files
(this should normally never occur) then simply delete all
XML files and restart. The program should restore its default
settings.

