 - [Sample applications](#Sample-applications)
    - [Video Recognizer](#Sample-applications-Video-Recognizer)
    - [Benchmark](#Sample-applications-Benchmark)
    - [Parser](#Sample-applications-Parser)
    - [Validation](#Sample-applications-Validation)
    - [Trying the samples](#Sample-applications-Trying-the-samples)
      - [Android](#Sample-applications-Trying-the-samples-Android)
      - [iOS](#Sample-applications-Trying-the-samples-iOS)
      - [Windows](#Sample-applications-Trying-the-samples-Windows)
      - [Raspberry Pi and Others](#Sample-applications-Trying-the-samples-Raspberry-Pi-and-Others)
- [Getting-started](#Getting-started)
  - [Adding the SDK to your project](#Getting-started-Adding-the-SDK-to-your-project)
    - [Android](#Getting-started-Adding-the-SDK-to-your-project-Android)
    - [iOS](#Getting-started-Adding-the-SDK-to-your-project-iOS)
    - [Raspberry Pi, Windows and Others](#Getting-started-Adding-the-SDK-to-your-project-Raspberry-Pi-Windows-and-Others)
   - [Using the API](#Getting-started-Using-the-API)
     - [Android (Java)](#Getting-started-Using-the-API-Android-Java)
     - [iOS, Windows, Raspberry Pi and Others (C++)](#Getting-started-Using-the-API-iOS-Windows-Raspberry-Pi-and-Others-Cpp)
 - [Getting help](#technical-questions)
  
 - Full documentation at https://www.doubango.org/SDKs/mrz/docs/
 - Online demo at https://www.doubango.org/webapps/mrz/
  
<hr />

This is state-of-the-art [Machine Readable Zone / Travel Documents (MRZ / MRTD)](https://en.wikipedia.org/wiki/Machine-readable_passport) dectector and recognizer using **deep learning**.

Unlike other solutions you can find on the web, you don't need to adjust the camera/image to define a Region Of Interest (ROI). 
We also don't try to use small ROI to decrease the processing time or false-positives. The whole image (**up to 4K supported**) is processed and every pixel is checked. 
No matter if the MRZ lines are **small**, **far away**, **blurred**, **partially occluded**, **skewed** or **slanted**, our implementation can accurately detect and recognize every character.

The detector is agnostic and doesn't decode (recognize/OCR) the text to check it against some pre-defined rules (regular expressions) which means **we support all MRZ types** (Travel Documents 1/2/3, MRVA, MRVB...) regardless the font, content, shape or country.
You can use our implementation to extract information from **local identity cards**, **passports**, **enhanced driver licenses**, **visas**, **resident cards**... 

In addition to being highly accurate our implementation is very fast and can run at up to **50fps on ARM devices** (iPhone7) and **114fps on x86_64** (Core i7-4790K) using HD images (**720p**). See [benchmark section](https://www.doubango.org/SDKs/mrz/docs/Benchmark.html) for more information.

Next [video](https://youtu.be/AO5XdbLK9Do) ([https://youtu.be/AO5XdbLK9Do](https://youtu.be/AO5XdbLK9Do)) shows [VideoRecognizer sample](#Sample-applications-Video-Recognizer) Running on Android: <br />
[![VideoRecognizer Running on Android](https://www.doubango.org/SDKs/mrz/docs/_images/Screenshot_20191231-160311_MrzVideoRecognizer.jpg)](https://www.youtube.com/watch?v=AO5XdbLK9Do)
   
<hr />

The Github repository contains the SDK binaries for **Android**, **iOS**, **Raspberry Pi** and **Windows**. You can contact us if you want binaries for other platforms.

Don't take our word for it, come check our implementation. **No registration, license key or internet connection is needed**, just clone the code and start coding/testing. 
Everything runs on the device, no data is leaving your computer. 
The code released on [Github](https://github.com/DoubangoTelecom/ultimateMRZ-SDK) comes with many ready-to-use samples to help you get started easily. 

You can also check our online cloud-based implementation (no registration required) at  https://www.doubango.org/webapps/mrz/ to check out the accuracy and precision before starting to play with the SDK.

 - Github repo: https://github.com/DoubangoTelecom/ultimateMRZ-SDK
 - Cloud-based implementation: https://www.doubango.org/webapps/mrz/
 
 
<a name="Sample-applications"></a>
# Sample applications # 
The source code comes with #4 sample applications: [Benchmark](samples/c++/Benchmark), [VideoRecognizer](samples/c++/recognizer), [Parser](samples/c++/parser) and [Validation](samples/c++/validation). All sample applications are open source and don’t require registration or license key.

The source code is at [samples](samples).

<a name="Sample-applications-Video-Recognizer"></a>
## Video Recognizer ##
This application should be used as reference implementation by any developer trying to add ultimateMRZ to their products. 
It shows how to detect and recognize MRZ lines at realtime using live video stream from the camera.
 - For **iOS**, a sample with user interface could be found at [samples/iOS/VideoRecognizer](samples/ios/VideoRecognizer)
 - For **Android**, a sample with user interface could be found at [samples/Android/VideoRecognizer](samples/android/videorecognizer)
 - For **Raspberry Pi, Windows and Others** a C++ command line application could be found at [samples/c++/recognizer](samples/c++/recognizer)
 
<a name="Sample-applications-Benchmark"></a>
## Benchmark ##
This application is used to check everything is ok and running as fast as expected. 
The information about the maximum frame rate (**50fps** on iPhone7 and **114fps** on core i7) could be checked using this application.
 - For **iOS**, a sample with user interface could be found at [samples/iOS/Benchmark](samples/ios/Benchmark)
 - For **Android**, a sample with user interface could be found at [samples/Android/Benchmark](samples/android/benchmark)
 - For **Raspberry Pi, Windows and Others** a C++ command line application could be found at [samples/c++/benchmark](samples/c++/benchmark)

<a name="Sample-applications-Parser"></a>
## Parser ##
This is a C++ sample application using regular expressions to extract the MRZ fields. If you want to extract the expiry date, document number, holder's name or birth date then, this is the right application. 
More info about the parser: https://www.doubango.org/SDKs/mrz/docs/MRZ_parser.html

<a name="Sample-applications-Validation"></a>
## Validation ##
MRZ lines contains check digits to validate the field. This C++ sample application shows how to validate the MRZ fields to make sure the OCR result from the recognizer is valid. 
Using the confidence score and validation process allows reaching **100% accuracy**.

More info about data validation: https://www.doubango.org/SDKs/mrz/docs/Data_validation.html

<a name="Sample-applications-Trying-the-samples"></a>
## Trying the samples ##

<a name="Sample-applications-Trying-the-samples-Android"></a>
### Android ###

To try the sample applications on Android:
- Open Android Studio and select **Open an existing Android Studio project**
    ![alt text](https://www.doubango.org/SDKs/mrz/docs/_images/android_studio_open_existing_project.jpg "Open an existing Android Studio project")
   
 - Navigate to [**<ultimateMRZ-SDK>/samples**](samples), select **android** folder and click **Open**
    ![alt text](https://www.doubango.org/SDKs/mrz/docs/_images/android_studio_select_samples_android.jpg "Select project")

- Select the sample you want to try (e.g. **videorecognize**), the device (e.g. **samsung SM-G975F**) and press **run**
    ![alt text](https://www.doubango.org/SDKs/mrz/docs/_images/android_studio_select_samples_videorecognizer.jpg "Select sample")

<a name="Sample-applications-Trying-the-samples-iOS"></a>
### iOS ###

To try the sample applications on iOS just open the corresponding Xcode project. 
For example, the Xcode project for the VideoRecognizer sample is at [ultimateMRZ-SDK/samples/ios/VideoRecognizer/VideoRecognizer.xcodeproj](samples/ios/VideoRecognizer/VideoRecognizer.xcodeproj)

<a name="Sample-applications-Trying-the-samples-Windows"></a>
### Windows ###
To try the sample applications on iOS just open the corresponding Visual Studio project. 
For example, the VS project for the VideoRecognizer sample is at [samples/c++/recognizer/recognizer.vcxproj](samples/c++/recognizer/recognizer.vcxproj).

<a name="Sample-applications-Trying-the-samples-Raspberry-Pi-and-Others"></a>
### Raspberry Pi and Others ###
For Raspberry Pi and other Linux systems you need to build the sample applications from source. More info at [samples/c++/README.md](samples/c++/README.md).



<a name="Getting-started"></a>
# Getting started # 
The SDK works on [many platforms](https://www.doubango.org/SDKs/mrz/docs/Architecture_overview.html#supportedoperatingsystems) and comes with support for many [programming languages](https://www.doubango.org/SDKs/mrz/docs/Architecture_overview.html#supportedprogramminglanguages). 

<a name="Getting-started-Adding-the-SDK-to-your-project"></a>
## Adding the SDK to your project ##
This Github repository contains [binaries](binaries) for **Android**, **iOS**, **Raspberry Pi** and **Windows**. The next sections explain how to add the SDK to an existing project.

<a name="Getting-started-Adding-the-SDK-to-your-project-Android"></a>
### Android ###
The SDK is distributed as an Android Studio module and you can add it as reference or you can also build it and add the AAR to your project. But, the easiest way to add the SDK to your project is by directly including the source.
In your **build.gradle** file add:
```python
android {

      # This is the block to add within "android { } " section
      sourceSets {
         main {
             jniLibs.srcDirs += ['path-to-your-ultimateMRZ-SDK/binaries/android/jniLibs']
             java.srcDirs += ['path-to-your-ultimateMRZ-SDK/java/android']
             assets.srcDirs += ['path-to-your-ultimateMRZ-SDK/assets/models']
         }
      }
}
```

<a name="Getting-started-Adding-the-SDK-to-your-project-iOS"></a>
### iOS ###
On iOS we provide a [FAT C++ framework](binaries/ios/ultimate_mrz-sdk.framework) for Xcode. 
The framework has a single C++ header file which means you can easily write an Obj-C wrapper around it if you want to use Swift language. 
Please contact us if you want to use the framework with Swift.

The framework is at [binaries/ios/ultimate_mrz-sdk.framework](binaries/ios/ultimate_mrz-sdk.framework).

Make sure you're using latest Xcode version. In the next sections we're using **Xcode Version 11.3 (11C29)**.

- Select your target (let's say **Test**), select **General** section then, **Framework, Libraries, and Embedded Content** and press **+** to browse to [binaries/ios/ultimate_mrz-sdk.framework](binaries/ios/ultimate_mrz-sdk.framework) to add the framework. 
 ![alt text](https://www.doubango.org/SDKs/mrz/docs/_images/ios_xcode_select_framework.jpg "Select framework")

- Select your target (let's say **Test**), select **Build Settings** section then, **Framework Search Paths** and press **+** to add path to the folder containing the framework (should be [**ultimateMRZ-SDK/binaries/ios**](binaries/ios))
 ![alt text](https://www.doubango.org/SDKs/mrz/docs/_images/ios_xcode_search_path.jpg "Add framework to search path")

- Right click on your project, select **Add Files to...** and browse to [**ultimateMRZ-SDK/assets**](assets)  to select [**models**](assets/models) the folder.
 ![alt text](https://www.doubango.org/SDKs/mrz/docs/_images/ios_add_files.jpg "Add files")

    The previous action should add the models and configuration files to the bundle resources:
       ![alt text](https://www.doubango.org/SDKs/mrz/docs/_images/ios_models.jpg "Add models")

That's it.

<a name="Getting-started-Adding-the-SDK-to-your-project-Raspberry-Pi-Windows-and-Others"></a>
### Raspberry Pi, Windows and Others ###

The shared libraries are under [**ultimateMRZ-SDK/binaries/<platform>**](binaries). The header file at [c++](c++). You can use any C++ compiler/linker.

<a name="Getting-started-Using-the-API"></a>
## Using the API ##

It's hard to be lost when you try to use the API as there are only 3 useful functions: init, process and deInit.

The C++ API is defined at https://www.doubango.org/SDKs/mrz/docs/cpp-api.html.

<a name="Getting-started-Using-the-API-Android-Java"></a>
### Android (Java) ###
```java
	import org.doubango.ultimateMrz.Sdk.ULTMRZ_SDK_IMAGE_TYPE;
	import org.doubango.ultimateMrz.Sdk.UltMrzSdkEngine;
	import org.doubango.ultimateMrz.Sdk.UltMrzSdkResult;

	// JSON configuration string
	// More info at https://www.doubango.org/SDKs/mrz/docs/Configuration_options.html
	final static String CONFIG = "{" +
		"\"debug_level\": \"info\"," + 
		"\"debug_write_input_image_enabled\": false," + 
		"\"debug_internal_data_path\": \".\"," + 
		"" + 
		"\"num_threads\": -1," + 
		"\"gpgpu_enabled\": true," + 
		"\"gpgpu_workload_balancing_enabled\": true," + 
		"" + 
		"\"segmenter_accuracy\": \"high\"," + 
		"\"interpolation\": \"bilinear\"," + 
		"\"min_num_lines\": 2," + 
		"\"roi\": [0, 0, 0, 0]," + 
		"\"min_score\": 0.0" + 
	"}";

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		

		// Initialize the engine
		assert UltMrzSdkEngine.init(
				getAssets(),
				CONFIG
		).isOK();
	}

	// Camera listener: https://developer.android.com/reference/android/media/ImageReader.OnImageAvailableListener
	final ImageReader.OnImageAvailableListener mOnImageAvailableListener = new ImageReader.OnImageAvailableListener() {

		@Override
		public void onImageAvailable(ImageReader reader) {
				try {
				    final Image image = reader.acquireLatestImage();
				    if (image == null) {
				        return;
				    }

				    // MRZ recognition
				    final int exifOrientation = 1; // Normal (landscape) - no rotation
				    final Image.Plane[] planes = image.getPlanes();
				    final UltMrzSdkResult result = UltMrzSdkEngine.process(
				        ULTMRZ_SDK_IMAGE_TYPE.ULTMRZ_SDK_IMAGE_TYPE_YUV420P,
				        planes[0].getBuffer(),
				        planes[1].getBuffer(),
				        planes[2].getBuffer(),
				        image.getWidth(),
				        image.getHeight(),
				        planes[0].getRowStride(),
				        planes[1].getRowStride(),
				        planes[2].getRowStride(),
				        planes[1].getPixelStride(),
				        exifOrientation
				    );
				    assert result.isOK();

				    image.close();

				} catch (final Exception e) {
				   e.printStackTrace();
				}
		}
	};

	@Override
	public void onDestroy() {
		// DeInitialize the engine
		assert UltMrzSdkEngine.deInit().isOK();

		super.onDestroy();
	}
 ```
 
 <a name="Getting-started-Using-the-API-iOS-Windows-Raspberry-Pi-and-Others-Cpp"></a>
 ### iOS, Windows, Raspberry Pi and Others (C++) ###

```cpp

	// JSON configuration string
	// More info at https://www.doubango.org/SDKs/mrz/docs/Configuration_options.html
	static const char* __jsonConfig =
	"{"
	"\"debug_level\": \"info\","
	"\"debug_write_input_image_enabled\": false,"
	"\"debug_internal_data_path\": \".\","
	""
	"\"num_threads\": -1,"
	"\"gpgpu_enabled\": true,"
	#if defined(__arm__) || defined(__thumb__) || defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB) || defined(_ARM) || defined(_M_ARM) || defined(_M_ARMT) || defined(__arm) || defined(__aarch64__)
	"\"gpgpu_workload_balancing_enabled\": true,"
	#else // Disable GPGPU/CPU work balancing on x86 and enable it on ARM -> https://devtalk.nvidia.com/default/topic/494659/execute-kernels-without-100-cpu-busy-wait-/
	"\"gpgpu_workload_balancing_enabled\": false,"
	#endif
	""
	"\"segmenter_accuracy\": \"high\","
	"\"interpolation\": \"bilinear\","
	"\"min_num_lines\": 2,"
	"\"roi\": [0, 0, 0, 0],"
	"\"min_score\": 0.0"
	"}";

	// Local variable
	UltMrzSdkResult result(0, "OK", "{}");

	// Initialize the engine (should be done once)
	ULTMRZ_SDK_ASSERT((result = UltMrzSdkEngine::init(
		__jsonConfig
	)).isOK());

	// Processing (detection + recognition)
	// Call this function for every video frame
	const void* imageData = nullptr;
	ULTMRZ_SDK_ASSERT((*result_ = UltMrzSdkEngine::process(
			ULTMRZ_SDK_IMAGE_TYPE_RGB24,
			imageData,
			imageWidth,
			imageHeight
		)).isOK());

	// DeInit
	// Call this function before exiting the app to free the allocate resources
	// You must not call process() after calling this function
	ULTMRZ_SDK_ASSERT((result = UltMrzSdkEngine::deInit()).isOK());
```

Again, please check the [sample applications](Sample-applications) for more information on how to use the API.


<a name="Technical-questions"></a>
 # Technical questions #
 Please check our [discussion group](https://groups.google.com/forum/#!forum/doubango-ai) or [twitter account](https://twitter.com/doubangotelecom?lang=en)
