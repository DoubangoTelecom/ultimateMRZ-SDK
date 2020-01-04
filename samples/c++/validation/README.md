- [Building](#building)
  - [Android](#building-android)
  - [iOS](#building-ios)
  - [Windows](#building-windows)
  - [Generic GCC](#building-generic-gcc)
  - [Raspberry Pi (Raspbian OS)](#building-rpi)
- [Testing](#testing)
  - [Usage](#testing-usage)
  - [Examples](#testing-examples)

This sample application validates MRZ fields (document number, birth date, expiry date...). The algorithm is explained at https://www.doubango.org/SDKs/mrz/docs/Data_validation.html.

This sample is standalone application with no dependencies.

<a name="building"></a>
# Building #

This sample contains [a single C++ source file](main.cxx) and is easy to build.

<a name="building-android"></a>
## Android ##
Please check [android](../../android) folder for Android samples.

<a name="building-ios"></a>
## iOS ##
Please check [iOS](../../ios) folder for iOS samples.

<a name="building-windows"></a>
## Windows ##
You'll need Visual Studio and the project is at [validation.vcxproj](validation.vcxproj).

<a name="building-generic-gcc"></a>
## Generic GCC ##
Next command is a generic GCC command:
```
cd ultimateMRZ-SDK/samples/c++/validation

g++ main.cxx -O3 -o validation
```
- If you're cross compiling then, you'll have to change `g++` with the correct triplet. For example, on Android ARM64 the triplet would be equal to `aarch64-linux-android-g++`.

<a name="building-rpi"></a>
## Raspberry Pi (Raspbian OS) ##

To build the sample for Raspberry Pi you can either do it on the device itself or cross compile it on [Windows](#cross-compilation-rpi-install-windows), [Linux](#cross-compilation-rpi-install-ubunt) or OSX machines. 
For more information on how to install the toolchain for cross compilation please check [here](../README.md#cross-compilation-rpi).

```
cd ultimateMRZ-SDK/samples/c++/validation

arm-linux-gnueabihf-g++ main.cxx -O3 -o validation
```
- On Windows: replace `arm-linux-gnueabihf-g++` with `arm-linux-gnueabihf-g++.exe`
- If you're building on the device itself: replace `arm-linux-gnueabihf-g++` with `g++` to use the default GCC

<a name="testing"></a>
# Testing #
After [building](#building) the application you can test it on your local machine.

<a name="testing-usage"></a>
## Usage ##

`validation` is a command line application with the following usage:
```
validation <path-to-file-containing mrz-lines>
```
- `<path-to-file-containing mrz-lines>` Path to txt file containing MRZ lines to parse. You can use default files at [../../../assets/samples/*.txt](../../../assets/samples).

<a name="testing-examples"></a>
## Examples ##

For example, on **Raspberry Pi** you may call the validation application using the following command:
```
chmod +x ./validation
./validation ../../../assets/samples/td1.txt
```


