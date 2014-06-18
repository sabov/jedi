#Libraries
##################################################

Some libraries must be installed prior to configuring and compiling:

*Assimp Library:
http://assimp.sourceforge.net/
On ubuntu 14, type the following in a terminal to install:
sudo apt-get install libassimp-dev libassimp-doc libassimp3
(on ubuntu 12 the last one is libassimp2, I think... or check with "sudo apt-cache search libassimp")

*Boost Library (current version is 1.55, but older versions should work too)
http://www.boost.org/
http://sourceforge.net/projects/boost/files/boost-binaries/ (for windows installer)
On ubuntu 14, type the following in a terminal to install:
sudo apt-get install libboost1.55-all-dev libboost1.55-dbg libboost1.55-doc
or try
sudo apt-get install libboost-all-dev
(or find out what the latest version for your system is with "apt-cache search libboost")

*Qt Library
If you are QtCreator for compiling the project, then Qt should already be installed, if not check out:
http://qt-project.org/downloads
or on ubuntu:
sudo apt-get install qt4-default qt5-default

*GLEW Libray (GL Extension Wrangler)
(NOTE: should be installed already, if the basic project-framework compiles for you, but I'm not totally sure if it is needed for running OpenGL-Applications. I'm just used to using it for programming, that is why it is included and because it is needed in the source code for loading and displaying objects.)
http://glew.sourceforge.net/
On ubuntu:
sudo apt-get install libglew1.10 libglewmx1.10 libglew-dev libglew-dbg libglewmx-dev libglewmx-dbg glew-utils
(or check again with "apt-cache search glew" to find which libraries are available for you).

#CMake - Notes
##############################################
1)
When setting up the projet with cmake you probably have to set the include and library paths for Assimp by hand (at least I never manage to get it work automatically), you can use the same library (libassimp.so / assimp.lib) for both release and debug. To set the values for the paths, you can use the cmake-gui (on Windows you simply open Cmake, on Linux you type "cmake-gui .." in a terminal), then press the "Configure"-Button until no red entries remain in the list. If red entries remain, you have to fix them by hand (apart from Assimp this should not be the case). Then close
the cmake-gui and open the CMakeLists.txt with QtCreator, like you are probably used to do now.

( on ubuntu you have to install cmake-gui via "sudo apt-get install cmake-qt-gui". )

2)
When configuring with cmake-gui, there is one entry for the Boost-Library that is never found: Boost_DIR. This is a bug from findBoost.cmake, and can be ignored, the only important thing is that Boost_INCLUDE_DIR is set correctly, and that there are no red entries in the list.
