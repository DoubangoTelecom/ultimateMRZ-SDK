- [Cross compilation](#cross-compilation)
  - [Raspberry Pi](#cross-compilation-rpi)
    - [Installing the toolchain](#cross-compilation-rpi-install)
      - [Windows](#cross-compilation-rpi-install-windows)
      - [Ubuntu](#cross-compilation-rpi-install-ubuntu)
- [Known issues](#known-issues)
      

There are 5 C++ samples: [Benchmark](benchmark), [Parser](parser), [Recognizer](recognizer), [RuntimeKey](runtimeKey) and [Validation](validation). Check [here](benchmark/README.md), [here](parser/README.md), [here](recognizer/README.md), [here](runtimeKey/README.md) and Check [here](validation/README.md) on how to build and use them.

The C++ samples are common to all platforms: **Android**, **Windows**, **iOS**, **Raspberry Pi (3 or 4)**, **Linux**...

For **Android**, we recommend using the Java samples under [android](../android) folder. These are complete samples with support for reatime recognition from video stream (camera) and require Android Studio to build. More info on how to install these samples is available [here](../../README.md#trying-the-samples-android).

For **iOS**, we recommend using the Obj-C samples under [iOS](../iOS) folder. These are complete samples with support for reatime recognition from video stream (camera) and require Xcode to build. More info on how to install these samples is available [here](../../README.md#trying-the-samples-ios).

<a name="cross-compilation"></a>
# Cross compilation #

Every sample contain a single C++ source file and is easy to cross compile. 

<a name="cross-compilation-rpi"></a>
## Raspberry Pi (Raspbian OS) ##

This section explain how to install Raspberry Pi 4 (Raspbian OS) toolchain to cross compile the samples. These instructions can be easily adapted to another target platform (e.g Android).

<a name="cross-compilation-rpi-install"></a>
### Installing the toolchain ###

<a name="cross-compilation-rpi-install-windows"></a>
#### Windows ####
The toolchain for Raspberry Pi 4 could be found at [http://sysprogs.com/getfile/566/raspberry-gcc8.3.0.exe](http://sysprogs.com/getfile/566/raspberry-gcc8.3.0.exe) and more toolchain versions are at [https://gnutoolchains.com/raspberry/](https://gnutoolchains.com/raspberry/).

If you haven't changed the installation dir then, it should be installed at **C:\SysGCC**. To add the toolchain to the **%PATH**:
```
set PATH=%PATH%;C:\SysGCC\raspberry\bin
```

<a name="cross-compilation-rpi-install-ubuntu"></a>
#### Ubuntu ####
```
sudo apt-get update
sudo apt-get install crossbuild-essential-armhf
```

<a name="known-issues"></a>
# Known issues #
- On Linux you may get `[CompVSharedLib] Failed to load library with path=<...>libultimatePluginOpenVINO.so, Error: 0xffffffff`. Make sure to set `LD_LIBRARY_PATH` to add binaries folder to help the loader find all dependencies. You can also run `ldd libultimatePluginOpenVINO.so` to see which libraries are missing.
- On Linux you may get `'GLIBC_2.27' not found (required by <...>)`. This message means you're using an old glibc version. Update glibc or your OS to Ubuntu 18, Debian Buster... You can check your actual version by running `ldd --version`. 
