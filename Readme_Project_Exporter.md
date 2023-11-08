# CODE::BLOCKS PROJECT/WORKSPACE EXPORTER PLUGIN

THIS PLUGIN IS A WORK IN PROGRESS, BUT IS USABLE AND WILL BE UPDATED TO FIX BUGS AND ENHANCED AS NEEDED!!!!

## Description

The ProjectExporter plugin allows you to export the active project/workspace to one of the following:

* Autotools
* Premake 4 script
* Bakefile
* CMakeFileLists.txt

Premake has the ability to generate multiple IDE project files and makefiles, so if proper Premake support is achieved, Code::Blocks will in essence be able to export to all of them.
Cmake has the ability to generate multiple IDE project files and makefiles, so if proper Premake support is achieved, Code::Blocks will in essence be able to export to all of them.

Currently, the ProjectExporter plugin generates all of the basic components, drastically reducing the time required to create a new build system, however, some modification of the output is often required.

## Testing/Coding/Feature Check List

|                   Item                                      |   Date    |   Result   |
|-------------------------------------------------------------| :-------: | :--------: |
| Autotools                                                   |           |            |
|   - Export simple project                                   |           |            |
|   - Can build simple exported project                       |           |            |
|   - Export complex project                                  |           |            |
|   - Can build complex exported project                      |           |            |
|                                                             |           |            |
| Premake 4 script                                            |           |            |
|   * Export simple project                                   |           |            |
|   * Can build simple exported project                       |           |            |
|   * Export complex project                                  |           |            |
|   * Can build complex exported project                      |           |            |
|                                                             |           |            |
| Bakefile                                                    |           |            |
|   * Export simple project                                   |           |            |
|   * Can build simple exported project                       |           |            |
|   * Export complex project                                  |           |            |
|   * Can build complex exported project                      |           |            |
|                                                             |           |            |
| CMake - CMakeFileLists.txt                                  |           |            |
|   * Export simple project                                   |           |            |
|   * Can build simple exported project                       |           |            |
|   * Export complex project                                  |           |            |
|   * Can build complex exported project                      |           |            |
|   * Export C::B project                                     |   Yes     |  14AUG2022 |
|   * Can build C::B exported project                         |  Failed   |  14AUG2022 |
|   * Export C::B workspace                                   |           |            |
|   * Can build C::B exported workspace                       |           |            |
|                                                             |           |            |
| Build/Test                                                  |           |            |
|   * Plugin loads, runs and works on Windows                 |   Yes     |  14AUG2022 |
|   * Plugin loads, runs and works on Linux                   |           |            |
|   * Plugin loads, runs and works on MacOS                   |           |            |
|   * Builds on Windows via workspace                         |           |            |
|   * Builds on Windows via MSYS2 makefile                    |           |            |
|   * Builds on Linux via workspace                           |           |            |
|   * Builds on Linux via bootstrap/configure/make            |           |            |
|   * Builds on Linux via Debian bootstrap/configure/make     |           |            |
|   * Builds on MacOS via workspace                           |           |            |
|   * Builds on MacOS via bootstrap/configure/make            |           |            |


## Known problems/To-Do's/Issues

### Common Items

**Note:** that the Project Exporter plugin will overwrite files (in the case of a conflict) without notification.

* Continue adding detection of more properties of Code::Blocks projects (especially dependencies)
* Add workspace recognition

### Autotools Items

* Preliminary Autotools code (not yet functional)!
* Using a small project, for autotools Miguel got the following, which is not correct:

    ~~~ autotools
    AM_OPTIONS_WXCONFIG
    AM_PATH_WXCONFIG(3.1.0, wxWin=1)
        if test "$wxWin" != 1; then
            AC_MSG_ERROR([
                wxWidgets must be installed on your system.

                Please check that wx-config is in path, the directory
                where wxWidgets libraries are installed (returned by
                'wx-config --libs' or 'wx-config --static --libs' command)
                is in LD_LIBRARY_PATH or equivalent variable and
                wxWindows version is 3.1.0 or above.
            ])
        fi
    Autotools build system created
    ~~~

### Premake 4 script Items

* C language projects are detected, but Premake4 generated makefiles do not compile - Does anyone know why?  I looked at the makefiles and they looked like they should work.
* Premake shows two messages:

    ~~~ text
    Premake script exported
    Premake script exported
    ~~~

### Premake 5 script Items

* WIP - Add Premake5:
    https://forums.codeblocks.org/index.php/topic,24581.msg167777.html#msg167777
    https://github.com/arnholm/premake5cb

### Bakefile  Items

* Prebuild steps are ignored during Bakefile export
* Parsing of global variables during Bakefile export sometimes yields invalid Bakefiles
* Bakefile I get 4 identical asserts:

    ~~~ text
    arrstr(558) iIndex != -1 failed in Remove()
    ~~~

### CMakeFileLists.txt Items

* Issue with building codeblcoks.exe, but other targets before this build.
* Need to be able to build the C::B main workspace with no manual file changes before the export is deemed usable!!!
* Add back in support for Windows compilation of rc files once compilation is fixed. Google "cmake windres CMAKE_RC_FLAGS" and read

## References

**Original Links**
This plugin is taken from Alpha's Project exporter as described in the following thread, but with a CMake exporter added:
<http://forums.codeblocks.org/index.php/topic,15201.0.html>

Original source:
<https://onedrive.live.com/?authkey=%21&id=5183AC017415BF92%21123&cid=5183AC017415BF92>

**Other References**
<https://forums.codeblocks.org/index.php/topic,6241.0.html>
<https://github.com/kisoft/cbmakefilegen>
<https://sourceforge.net/projects/cbp2make/files>
<https://github.com/mirai-computing/cbp2make>
<http://forums.codeblocks.org/index.php/topic,13675.0.html>
