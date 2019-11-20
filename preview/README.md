- [SDK](#SDK)
  - [GPGPU acceleration](#SDK.GPGPU_acceleration)
- [Sample application](#Sample_application)
  - [Building](#Sample_application.Building)
  - [Testing](#Sample_application.Testing)
- [Technical questions](#Technical_questions)

<hr />

This folder contains a preview version of our MRZ deep learning engine **for Windows**. We're working to release the full documentation and add some speed optimizations. We're releasing this preview because we've received many emails from companies requesting to have access to our cloud API hosted at https://www.doubango.org/webapps/mrz/.

We're working to release the SDK for the coming days. The initial version will support **Android**, **Windows** and **Raspberry pi**. The SDK will have the same API for all platforms which means you can start testing on Windows and easily switch to another platform (e.g. Android or iOS) when we release the code.

This preview is a trial version and may come with some restrictions (some characters obfuscated with a '\*' placeholder).


# SDK #
<a name="SDK"> </a>
The root directory contains the following folders:
- [assets](assets):
  - [mrz.classifier.params.json.doubango](assets/mrz.classifier.params.json.doubango): configuration file for the Doubango deep learning model
  - [mrz.classifier.strong.model.flat](assets/mrz.classifier.strong.model.flat): Doubango deep learning model for the detection
  - [mrz.classifier.strong.pca.json](assets/mrz.classifier.strong.pca.json): pca (Principal component analysis) data for the deep learning model
  - [mrz.traineddata](assets/mrz.traineddata): Tesseract traineddata
  
  These files must be redistributed with your application and installed in the sample folder (final release will allow specifying the assets folder)
  
- [images](images): sample images
- [bin](bin): dll to redistribute with your application
- [lib](lib): shared libraries to link to
- [include](include): contains single file ([ultimateMRZ-SDK-API-PUBLIC.h](include/ultimateMRZ-SDK-API-PUBLIC.h)) you have to include in your c++ code
- [c++](c++): sample code and visual studio project to build it

<a name="SDK.GPGPU_acceleration"></a>
## GPGPU acceleration ##
Make sure the update your GPU drivers to have GPGPU accelerations (OpenCL 1.2+) enabled. 

<a name="Sample_application"></a>
# Sample application #
The sample application is under [c++](c++) folder and contains a visual studio project to build it. It's difficult to be lost when you look at the [sample code](c++/main.cxx). The MRZ API only have #3 functions: **init**, **process** and **deInit**. 

The next sections explain how to build the sample application. If you just want to test the application without building it yourself then, download the zip file at [c++/mrz_sample.zip](c++/mrz_sample.zip) and it contains all required files (mrz_sample.exe, models, dlls...).

<a name="Sample_application.Building"></a>
## Building ##
You just need to open [c++/mrz_sample.sln](c++/mrz_sample.sln) with Visual Studio (we're using VS 2015 Community) and build the project.

Once you've built the sample application you'll need to copy the files in the [assets](assets) and [bin](bin) folders to the same folder as [ultimateMRZ-SDK/preview/c++/x64/Release/mrz_sample.exe](c++/x64/Release/mrz_sample.exe).

<a name="Sample_application.Testing"></a>
## Testing ##
[ultimateMRZ-SDK/preview/c++/x64/Release/mrz_sample.exe](c++/x64/Release/mrz_sample.exe) is a command line application.

Usage:
```
mrz_sample.exe input_path
```
- **input_path**: path to the input file

Example:
```
mrz_sample.exe images/Czech_passport_2005_MRZ_data_1300x1002_yuv420p.jpg
```

[images](images) folder contains sample images like the next one:

<img src="images/Czech_passport_2005_MRZ_data_1300x1002.jpg">

<a name="Technical_questions"></a>
 # Technical questions #
 Please check our [discussion group](https://groups.google.com/forum/#!forum/doubango-ai) or [twitter account](https://twitter.com/doubangotelecom?lang=en)


