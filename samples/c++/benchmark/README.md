- [Peformance numbers](#peformance-numbers)
- [Building](#building)
  - [Android](#building-android)
  - [iOS](#building-ios)
  - [Windows](#building-windows)
  - [Generic GCC](#building-generic-gcc)
  - [Raspberry Pi (Raspbian OS)](#building-rpi)
- [Testing](#testing)
  - [Usage](#testing-usage)
  - [Examples](#testing-examples)


It's easy to assert that our implementation is fast without backing our claim with numbers and source code freely available to everyone to check.

Rules:
 - We're running the processing function within a loop for #100 times.
 - The **positive rate** defines the percentage of images with at least #2 MRZ lines. For example, 20% positives means we will have #80 **negative** images (no MRZ lines) and #20 positives (with MRZ lines) out of the #100 total images. This percentage is important as it allows timing both the detector and recognizer.
 - We're using **high** accuracy for the segmenter and **bilinear** interpolation.
 - All positive images contain a least #2 MRZ lines.
 - The initialization is done outside the loop.

The concept of **negative** and **positive** images is very important because in most use cases you'll:
 - 1. Start the application
 - 2. Move the application to the MRZ zone to recognize the data

**You only need a single "good frame" to recognize the MRZ lines**. 
But, between step #1 and step #2 the application has probably processed more than #200 frames (40fps * 5sec). 
So, in such scenario the application have to process #201 frames before reaching the "good frame": #200 negatives and #1 positive. 
If processing negative frame is very slow then, the application won't be able to catch this "good frame" at the right time. 
A slow application will do one of these strategies:

- 1. **Drop frames to keep the impression that the application is running at realtime**: In such scenario the positive frames will most likely be dropped (probability = 1/201 = 0.49%) which means reporting the time when this single "good frame" is caught.
- 2. **Enqueue the frames and process them at the application speed**: This is the worse solution because you could run out of memory and when the application is running slowly then, you can spend several minutes before reaching this single "good frame".

A fast application will run at 40fps and catch this "good frame" as soon as it's presented for processing. This offers a nice user experience.

<a name="peformance-numbers"></a>
# Peformance numbers #

Some performance numbers on high-end (**Core i7**) and low-end (**Raspberry Pi 4**) devices using **720p (1280x720)** images:

|  | 0.0 rate | 0.2 rate | 0.5 rate | 0.7 rate | 1.0 rate |
|-------- | --- | --- | --- | --- | --- |
| **Core i7-4790K (Windows 8)** | 877 millis<br/>**114 fps** | 1975 millis<br/>50.61 fps | 3736 millis<br/>26.76 fps | 4901 millis<br/>20.40 fps | 6526 millis<br/>15.32 fps |
| **iPhone7 (iOS 13)** | 1990 millis<br/>**50.23 fps** | 4325 millis<br/>23.11 fps | 7982 millis<br/>12.52 fps | 10595 millis<br/>9.43 fps | 14201 millis<br/>7.04 fps |
| **Galaxy S10+ (Android 10)** | 2825 millis<br/>**35.39 fps** | 7575 millis<br/>13.20 fps | 12960 millis<br/>7.71 fps | 17636 millis<br/>5.67 fps | 21069 millis<br/>4.74 fps |
| **Raspberry Pi 4 (Raspbian Buster)** | 4335 millis<br/>**23.06 fps** | 13555 millis<br/>7.37 fps | 27878 millis<br/>3.58 fps | 37399 millis<br/>2.67 fps | 47797 millis<br/>2.09 fps |

Some notes:
- **Please note that even if Raspberry Pi 4 has a 64-bit CPU [Raspbian OS](https://en.wikipedia.org/wiki/Raspbian>) uses a 32-bit kernel which means we're loosing many SIMD optimizations.**

<a name="building"></a>
# Building #

This sample contains [a single C++ source file](main.cxx) and is easy to build. The documentation about the C++ API is at [https://www.doubango.org/SDKs/mrz/docs/cpp-api.html](https://www.doubango.org/SDKs/mrz/docs/cpp-api.html).

<a name="building-android"></a>
## Android ##
Please check [android](../../android) folder for Android samples.

<a name="building-ios"></a>
## iOS ##
Please check [iOS](../../ios) folder for iOS samples.

<a name="building-windows"></a>
## Windows ##
You'll need Visual Studio and the project is at [benchmark.vcxproj](benchmark.vcxproj).

<a name="building-generic-gcc"></a>
## Generic GCC ##
Next command is a generic GCC command:
```
cd ultimateMRZ-SDK/samples/c++/benchmark

g++ main.cxx -O3 -I../../../c++ -L../../../binaries/<yourOS>/<yourArch> -lultimate_mrz-sdk -o benchmark
```
- You've to change `yourOS` and  `yourArch` with the correct values. For example, on Android ARM64 they would be equal to `android` and `jniLibs/arm64-v8a` respectively.
- If you're cross compiling then, you'll have to change `g++` with the correct triplet. For example, on Android ARM64 the triplet would be equal to `aarch64-linux-android-g++`.

<a name="building-rpi"></a>
## Raspberry Pi (Raspbian OS) ##

To build the sample for Raspberry Pi you can either do it on the device itself or cross compile it on [Windows](#cross-compilation-rpi-install-windows), [Linux](#cross-compilation-rpi-install-ubunt) or OSX machines. 
For more information on how to install the toolchain for cross compilation please check [here](../README.md#cross-compilation-rpi).

```
cd ultimateMRZ-SDK/samples/c++/benchmark

arm-linux-gnueabihf-g++ main.cxx -O3 -I../../../c++ -L../../../binaries/raspbian/armv7l -lultimate_mrz-sdk -o benchmark
```
- On Windows: replace `arm-linux-gnueabihf-g++` with `arm-linux-gnueabihf-g++.exe`
- If you're building on the device itself: replace `arm-linux-gnueabihf-g++` with `g++` to use the default GCC

<a name="testing"></a>
# Testing #
After [building](#building) the application you can test it on your local machine.

<a name="testing-usage"></a>
## Usage ##

`Benchmark` is a command line application with the following usage:
```
benchmark \
      --positive <path-to-image-with-a-plate> \
      --negative <path-to-image-without-a-plate> \
      [--assets <path-to-assets-folder>] \
      [--loops <number-of-times-to-run-the-loop:[1, inf]>] \
      [--rate <positive-rate:[0.0, 1.0]>] \
      [--tokenfile <path-to-license-token-file>] \
      [--tokendata <base64-license-token-data>]
```
Options surrounded with **[]** are optional.
- `--positive` Path to an image (JPEG/PNG/BMP) with a license plate. This image will be used to evaluate the recognizer. You can use default image at [../../../assets/images/Passport-Australia_1280x720.jpg](../../../assets/images/Passport-Australia_1280x720.jpg).
- `--negative` Path to an image (JPEG/PNG/BMP) without a license plate. This image will be used to evaluate the decoder. You can use default image at [../../../assets/images/Passport-France_1200x864.jpg](../../../assets/images/Passport-France_1200x864.jpg).
- `--assets` Path to the [assets](../../../assets) folder containing the configuration files and models. Default value is the current folder.
- `--loops` Number of times to run the processing pipeline.
- `--rate` Percentage value within[0.0, 1.0] defining the positive rate. The positive rate defines the percentage of images with MRZ lines.
- `--tokenfile` Path to the file containing the base64 license token if you have one. If not provided then, the application will act like a trial version. Default: *null*.
- `--tokendata` Base64 license token if you have one. If not provided then, the application will act like a trial version. Default: *null*.

The information about the maximum frame rate (**114fps** on core i7 and **50fps** on iPhone7) is obtained using `--rate 0.0` which means evaluating the negative (no MRZ lines) image only. The minimum frame rate could be obtained using `--rate 1.0` which means evaluating the positive image only (all images on the video stream have MRZ lines). In real life, you only need a single positive frame to recognize the MRZ data.

<a name="testing-examples"></a>
## Examples ##

For example, on **Raspberry Pi** you may call the benchmark application using the following command:
```
LD_LIBRARY_PATH=../../../binaries/raspbian/armv7l:$LD_LIBRARY_PATH ./benchmark \
    --positive ../../../assets/images/Passport-Australia_1280x720.jpg \
    --negative ../../../assets/images/Passport-France_1200x864.jpg \
    --assets ../../../assets \
    --loops 100 \
    --rate 0.2
```
On Android ARM64 you may use the next command:
```
LD_LIBRARY_PATH=../../../binaries/android/jniLibs/arm64-v8a:$LD_LIBRARY_PATH ./benchmark \
    --positive ../../../assets/images/Passport-Australia_1280x720.jpg \
    --negative ../../../assets/images/Passport-France_1200x864.jpg \
    --assets ../../../assets \
    --loops 100 \
    --rate 0.2
```

Please note that if you're cross compiling the application then you've to make sure to copy the application and both the [assets](../../../assets) and [binaries](../../../binaries) folders to the target device.


