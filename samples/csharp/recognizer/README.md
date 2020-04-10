- [Building](#building)
- [Testing](#testing)
  - [Usage](#testing-usage)
  - [Examples](#testing-examples)


This application is a reference implementation for developers to show how to use the C# API and could
be used to easily check the accuracy. The C# API is a wrapper around the C++ API defined at [https://www.doubango.org/SDKs/mrz/docs/cpp-api.html](https://www.doubango.org/SDKs/mrz/docs/cpp-api.html).

The application accepts path to a JPEG/PNG/BMP file as input. This **is not the recommended** way to use the API. We recommend reading the data directly from the camera and feeding the SDK with the uncompressed **YUV data** without saving it to a file or converting it to RGB.

If you don't want to build this sample and is looking for a quick way to check the accuracy then, try
our cloud-based solution at [https://www.doubango.org/webapps/mrz/](https://www.doubango.org/webapps/mrz/).

This sample is open source and doesn't require registration or license key.

<a name="building"></a>
# Building #

This sample contains [a single C# source file](Program.cs) and is easy to build using Visual Studio. Open the [VS solution](recognizer.sln) and run the project.
The Visual Studio project is configured for Win64, you have to change the settings for other platforms.

<a name="testing-usage"></a>
## Usage ##

`recognizer` is a command line application with the following usage:
```
recognizer \
      --image <path-to-image-with-mrzdata-to-process> \
      [--assets <path-to-assets-folder>] \
      [--tokenfile <path-to-license-token-file>] \
      [--tokendata <base64-license-token-data>]
```
Options surrounded with **[]** are optional.
- `--image` Path to the image(JPEG/PNG/BMP) to process. You can use default image at [../../../assets/images/Czech_passport_2005_MRZ_orient1_1300x1002.jpg](../../../assets/images/Czech_passport_2005_MRZ_orient1_1300x1002.jpg).
- `--assets` Path to the [assets](../../../assets) folder containing the configuration files and models. Default value is the current folder.
- `--tokenfile` Path to the file containing the base64 license token if you have one. If not provided then, the application will act like a trial version. Default: *null*.
- `--tokendata` Base64 license token if you have one. If not provided then, the application will act like a trial version. Default: *null*.

<a name="testing-examples"></a>
## Examples ##
You'll need to change the Visual Studio properties to define the command line arguments.

```
recognizer.exe \
    --image ../../../assets/images/Czech_passport_2005_MRZ_orient1_1300x1002.jpg \
    --assets ../../../assets
```

Please note that if you're cross compiling the application then you've to make sure to copy the application and both the [assets](../../../assets) and [binaries](../../../binaries) folders to the target device.


