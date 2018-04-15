# Requirements
* Windows10 64bits
* Visual Studio 2015 (community edition is ok)
* Premake5 (https://premake.github.io/download.html)
* Git for windows (https://git-scm.com/download/win)
* 7-zip installed at default location

# Build
* Open a Git console and go to ArioCppMiner folder
* Launch ./setup.sh, this will install & compile dependencies
* Open ./prj/ArioCppMiner.sln with Visual Studio 2015
* Select either "Rel", "RelAVX" or "RelAVX2" configuration
* Press F7 to build
* Executable and required DLLs are put in ./bin/ folder
