## pxt.dll build for Visual Studio 2019

The native pxt runtime (pxt.dll) is traditionally built using a gcc toolchain. This project is an experiment to build it in Visual Studio. The motivation for this project is to make debugging easier on Windows machines. For example, this makes it possible to attach to Unity.

Please note most of the source files in this project were generated using the pxt CLI, located here: https://github.com/microsoft/pxt/. The codebase in this repo is a snapshot in time of generated source.

The source code for pxt.dll exists in this repo: https://github.com/microsoft/pxt-common-packages

TODO:
	- [ ] Include instructions to generate pxt source
	- [ ] Merge changes for MSVC back to pxt repo
