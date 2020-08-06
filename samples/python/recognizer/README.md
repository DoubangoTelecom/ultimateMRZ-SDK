- [Prerequisite](#prerequisite)
- [Usage](#testing-usage)
- [Examples](#testing-examples)


This application is used as reference code for developers to show how to use the Python bindings for the [C++ API](https://www.doubango.org/SDKs/mrz/docs/cpp-api.html) and could
be used to easily check the accuracy. The application accepts path to a JPEG/PNG/BMP file as input. This **is not the recommended** way to use the API. We recommend reading the data directly from the camera and feeding the SDK with the uncompressed **YUV data** without saving it to a file or converting it to RGB.

If you don't want to build this sample and is looking for a quick way to check the accuracy then, try
our cloud-based solution at [https://www.doubango.org/webapps/mrz/](https://www.doubango.org/webapps/mrz/).

This sample is open source and doesn't require registration or license key.

<a name="prerequisite"></a>
# Prerequisite #

[**You must build the Python extension**](../../../python/README.md) before trying to run this sample. More information on how to build the extenstion could be found [here](../../../python/README.md)

<a name="testing-usage"></a>
# Usage #

`recognizer.py` is a Python command line application with the following usage:
```
recognizer.py \
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
# Examples #

You should navigate to the current folder (`ultimateMRZ-SDK/samples/python/recognizer` ) before trying the next commands:
```
cd ultimateMRZ-SDK/samples/python/recognizer
```

- For example, on **Raspberry Pi** you may call the recognizer application using the following command:
```
PYTHONPATH=../../../binaries/raspbian/armv7l:../../../python \
LD_LIBRARY_PATH=../../../binaries/raspbian/armv7l:$LD_LIBRARY_PATH \
python recognizer.py --image ../../../assets/images/Czech_passport_2005_MRZ_orient1_1300x1002.jpg --assets ../../../assets 
```
- On **Linux x86_64**, you may use the next command:
```
PYTHONPATH=../../../binaries/linux/x86_64:../../../python \
LD_LIBRARY_PATH=../../../binaries/linux/x86_64:$LD_LIBRARY_PATH \
python recognizer.py --image ../../../assets/images/Czech_passport_2005_MRZ_orient1_1300x1002.jpg --assets ../../../assets 
```
- On **Windows x86_64**, you may use the next command:
```
set PYTHONPATH=../../../binaries/windows/x86_64;../../../python
python recognizer.py --image ../../../assets/images/Czech_passport_2005_MRZ_orient1_1300x1002.jpg --assets ../../../assets
```

Please note that if you're cross compiling the application then you've to make sure to copy the application and both the [assets](../../../assets) and [binaries](../../../binaries) folders to the target device.


