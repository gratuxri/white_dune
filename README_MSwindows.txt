This file is free software; the Free Software Foundation    
gives unlimited permission to copy, distribute and modify it.

white_dune do not have a installation procedure. It should run on all 32 bit
windows versions from scratch (except early versions of Windows 95, which 
requires the installation of OpenGL first). 

white_dune usually stores some configuration information in the user section 
of the registry. It is possible to remove this information with
"path\to\white_dune*.exe -uninstall"

If you want to store the configuration information in a file instead of the
windows registry, you can set the environment variable DUNERC.
This makes it possible to run white_dune from a USB-Stick without touching 
the windows registry with a additional batchfile like

set DUNERC=%~d0:\dune.rc 
%~d0:\white_dune*.exe

To compile with Visual C++, execute batch/nt.bat (or batch/jpg_png_zlib.bat
if you have the matching libraries/SDKs (see INSTALL)). Then go to the src
directory and click to the dune.dsw files for the older Visual C++ compiler
or dune.vcproj for the newer Visual C++ (Express) compilers.
 
After successfull compilation, search for a M$Windows executable dune.exe 
in the subdirectory "src/Debug"


