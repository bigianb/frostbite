Building
========

For windows, I use vcpkg to install packages. See https://github.com/Microsoft/vcpkg

you will want to install the following packages as follows:

```
vcpkg install sdl2:x64-windows
vcpkg install glm:x64-windows
vcpkg install glew:x64-windows
vcpkg install boost:x64-windows
```

Then you can open the directory in visual studio and it will be recognised as a cmake project.
Whether you use the open folder functionality or call cmake on the command line you will need to
set the toolchain file to the vcpkg one. This lives in the vcpkg directory under `scripts\buildsystems\vcpkg.cmake`

https://github.com/microsoft/vcpkg/blob/master/docs/examples/installing-and-using-packages.md#cmake-toolchain-file

note that if you do `vcpkg integrate install` then you don't need to set the toolchain when using visual studio 2019

