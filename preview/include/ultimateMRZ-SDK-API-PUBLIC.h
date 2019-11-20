/* Copyright (C) 2016-2019 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: For non commercial use only.
* Source code: https://github.com/DoubangoTelecom/ultimateMRZ-SDK
* WebSite: https://www.doubango.org/webapps/mrz/
*/
/**
\file ultimateMRZ-SDK-API-PUBLIC.h
ultimateMRZ-SDK public header
*/
#ifndef _ULTIMATE_MRZ_SDK_API_PUBLIC_H_
#define _ULTIMATE_MRZ_SDK_API_PUBLIC_H_

#include <string>

#define ULTMRZ_SDK_VERSION_MAJOR		2
#define ULTMRZ_SDK_VERSION_MINOR		0
#define ULTMRZ_SDK_VERSION_MICRO		0

// Windows's symbols export
#if defined(SWIG)
# 	define ULTIMATE_MRZ_SDK_PUBLIC_API
#else
#	if (defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE) || defined(_WIN16) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)) && !defined(ULTMRZ_SDK_STATIC)
#		if defined(ULTIMATE_MRZ_SDK_PUBLIC_EXPORTS)
# 			define ULTIMATE_MRZ_SDK_PUBLIC_API		__declspec(dllexport)
#		else
# 			define ULTIMATE_MRZ_SDK_PUBLIC_API		__declspec(dllimport)
#		endif
#	else
# 		define ULTIMATE_MRZ_SDK_PUBLIC_API			__attribute__((visibility("default")))
#	endif /* WIN32 */
#endif /* SWIG */

// Android OS detection
#if (defined(__ANDROID__) || defined(ANDROID)) && !defined(SWIG)
#	define ULTMRZ_SDK_OS_ANDROID	1
#endif /* ULTMRZ_SDK_OS_ANDROID */

// Macros to print logs to the console
#if ULTMRZ_SDK_OS_ANDROID
#	if !defined(SWIG)
#		include <android/log.h>
#		include <android/asset_manager.h>
#		include <jni.h>
#	endif
#	define ULTMRZ_SDK_PRINT_VERBOSE(FMT, ...) __android_log_print(ANDROID_LOG_VERBOSE, "org.doubango.ultimateMrz.Sdk", "*[ULTMRZ_SDK VERBOSE]: " FMT "\n", ##__VA_ARGS__)
#	define ULTMRZ_SDK_PRINT_INFO(FMT, ...) __android_log_print(ANDROID_LOG_INFO, "org.doubango.ultimateMrz.Sdk", "*[ULTMRZ_SDK INFO]: " FMT "\n", ##__VA_ARGS__)
#	define ULTMRZ_SDK_PRINT_WARN(FMT, ...) __android_log_print(ANDROID_LOG_WARN, "org.doubango.ultimateMrz.Sdk", "**[ULTMRZ_SDK WARN]: function: \"%s()\" \nfile: \"%s\" \nline: \"%u\" \nmessage: " FMT "\n", __FUNCTION__,  __FILE__, __LINE__, ##__VA_ARGS__)
#	define ULTMRZ_SDK_PRINT_ERROR(FMT, ...) __android_log_print(ANDROID_LOG_ERROR, "org.doubango.ultimateMrz.Sdk", "***[ULTMRZ_SDK ERROR]: function: \"%s()\" \nfile: \"%s\" \nline: \"%u\" \nmessage: " FMT "\n", __FUNCTION__,  __FILE__, __LINE__, ##__VA_ARGS__)
#	define ULTMRZ_SDK_PRINT_FATAL(FMT, ...) __android_log_print(ANDROID_LOG_FATAL, "org.doubango.ultimateMrz.Sdk", "****[ULTMRZ_SDK FATAL]: function: \"%s()\" \nfile: \"%s\" \nline: \"%u\" \nmessage: " FMT "\n", __FUNCTION__,  __FILE__, __LINE__, ##__VA_ARGS__)
#else
#	define ULTMRZ_SDK_PRINT_VERBOSE(FMT, ...) fprintf(stderr, "*[ULTMRZ_SDK VERBOSE]: " FMT "\n", ##__VA_ARGS__)
#	define ULTMRZ_SDK_PRINT_INFO(FMT, ...) fprintf(stderr, "*[ULTMRZ_SDK INFO]: " FMT "\n", ##__VA_ARGS__)
#	define ULTMRZ_SDK_PRINT_WARN(FMT, ...) fprintf(stderr, "**[ULTMRZ_SDK WARN]: function: \"%s()\" \nfile: \"%s\" \nline: \"%u\" \nmessage: " FMT "\n", __FUNCTION__,  __FILE__, __LINE__, ##__VA_ARGS__)
#	define ULTMRZ_SDK_PRINT_ERROR(FMT, ...) fprintf(stderr, "***[ULTMRZ_SDK ERROR]: function: \"%s()\" \nfile: \"%s\" \nline: \"%u\" \nmessage: " FMT "\n", __FUNCTION__,  __FILE__, __LINE__, ##__VA_ARGS__)
#	define ULTMRZ_SDK_PRINT_FATAL(FMT, ...) fprintf(stderr, "****[ULTMRZ_SDK FATAL]: function: \"%s()\" \nfile: \"%s\" \nline: \"%u\" \nmessage: " FMT "\n", __FUNCTION__,  __FILE__, __LINE__, ##__VA_ARGS__)
#endif /* !ULTMRZ_SDK_OS_ANDROID */

// Assertion function
#if defined(NDEBUG)
#	define ULTMRZ_SDK_ASSERT(x) do { bool __ULTMRZ_SDK_b_ret = (x); if (!__ULTMRZ_SDK_b_ret) { ULTMRZ_SDK_PRINT_FATAL("Assertion failed!"); abort(); } } while(0)
#else
#	define ULTMRZ_SDK_ASSERT(x) do { bool __ULTMRZ_SDK_b_ret = (x); assert(__ULTMRZ_SDK_b_ret); } while(0)
#endif /* !NDEBUG */

namespace ultimateMrzSdk  
{
	/*! Defines the image types.
	*/
	enum ULTMRZ_SDK_IMAGE_TYPE {
		/*! Each pixel is stored on 3 bytes. Each channel (R, G, B) is stored with 8 bits (1 byte) of precision (256 possible values).
		* The R channel is stored at the lowest memory address followed by G then B channels.
		* Here is how the pixels are packed:
		* \code{.cpp}
		* const int pixel = (B & 0xff) << 16 | (G & 0xff) << 8 | (R & 0xff);
		* \endcode
		*/
		ULTMRZ_SDK_IMAGE_TYPE_RGB24,
		/*! Each pixel is stored on 4 bytes. Each channel (R, G, B, A) is stored with 8 bits (1 byte) of precision (256 possible values).
		* The R channel is stored at the lowest memory address followed by G, B then A channels. If you're using Android then,
		* this is the same as <a href="https://developer.android.com/reference/android/graphics/Bitmap.Config#ARGB_8888">ARGB_8888</a>.
		* Here is how the pixels are packed:
		* \code{.cpp}
		* const int pixel = (A & 0xff) << 24 | (B & 0xff) << 16 | (G & 0xff) << 8 | (R & 0xff);
		* \endcode
		*/
		ULTMRZ_SDK_IMAGE_TYPE_RGBA32,
		/*! YUV 4:2:0 image with a plane of 8 bit Y samples followed by an interleaved U/V plane containing 8 bit 2x2 subsampled colour difference samples.
		*	More information at https://www.fourcc.org/pixel-format/yuv-nv12/
		*/
		ULTMRZ_SDK_IMAGE_TYPE_NV12,
		/*! YUV 4:2:0 image with a plane of 8 bit Y samples followed by an interleaved V/U plane containing 8 bit 2x2 subsampled chroma samples. 
		* The same as \ref ULTMRZ_SDK_IMAGE_TYPE_NV12 except the interleave order of U and V is reversed.
		*	More information at https://www.fourcc.org/pixel-format/yuv-nv21/
		*/
		ULTMRZ_SDK_IMAGE_TYPE_NV21,
		/*! These formats are identical to YV12 except that the U and V plane order is reversed. 
		* They comprise an NxM Y plane followed by (N/2)x(M/2) U and V planes. 
		* This is the format of choice for many software MPEG codecs.
		* More information at https://www.fourcc.org/pixel-format/yuv-i420/
		*/
		ULTMRZ_SDK_IMAGE_TYPE_YUV420P,
		/*! Same as \ref ULTMRZ_SDK_IMAGE_TYPE_YUV420P except the order of U and V is reversed.
		*	More information at https://www.fourcc.org/pixel-format/yuv-yv12/
		*/
		ULTMRZ_SDK_IMAGE_TYPE_YVU420P,
		/*!  YUV 4:2:2 image with an NxM Y plane followed by (N/2)x(M) V and U planes.
		*/
		ULTMRZ_SDK_IMAGE_TYPE_YUV422P,
		/*!  YUV 4:4:4 image with an NxM Y plane followed by NxM V and U planes.
		*/
		ULTMRZ_SDK_IMAGE_TYPE_YUV444P,
	};

	/*! Result returned by the \ref UltMrzSdkEngine "engine" at initialization, deInitialization and processing stages.
	*/
	class UltMrzSdkResult {
	public:
#if !defined(SWIG)
		UltMrzSdkResult() = delete;
#endif /* SWIG */
		UltMrzSdkResult(const int code, std::string const& phrase, std::string const& json, const size_t numZones = 0)
		: code_(code), phrase_(phrase), json_(json), numZones_(numZones) {}
		
		virtual ~UltMrzSdkResult() {}

		/*! The result code. 0 if success, nonzero otherwise.
		*/
		inline int code()const { return code_; }
		/*! Short description for the \ref code.
		*/
		inline const std::string& phrase()const { return phrase_; }
		/*! The MRZ zones as JSON content string. May be null if no zone found.
		*/
		inline const std::string& json()const { return json_; }
		/*! Number of zones in \ref json string. This is a helper function to quickly check whether the result contains zones
			without parsing the \ref json string.
		*/
		inline const size_t numZones()const { return numZones_; }
		/*! Whether the result is success. true if success, false otherwise.
		*/
		inline bool isOK()const { return (code_ == 0); }
#if !defined(SWIG)
		static UltMrzSdkResult bodyless(const int code, const std::string& phrase) { return UltMrzSdkResult(code, phrase, ""); }
		static UltMrzSdkResult bodylessOK() { return UltMrzSdkResult(0, "OK", ""); }
#endif /* SWIG */
	private:
		int code_;
		std::string phrase_;
		std::string json_;
		size_t numZones_;
	};

	/*! The Machine-readable zone/passport (MRZ/MRP) engine.
	*/
	class ULTIMATE_MRZ_SDK_PUBLIC_API UltMrzSdkEngine
	{
#if !defined(SWIG)
	protected:
		UltMrzSdkEngine() = delete;
#endif /* SWIG */
	public:

#if ULTMRZ_SDK_OS_ANDROID
		/*! Initializes the engine. This function must be the first one to call.
			This function is only available on Android.
			\param assetManager AssetManager to use to read the content of the "assets" folder containing the models and configuration files.
			\param jsonConfig JSON string containing configuration entries. May be null. More info at https://www.doubango.org/SDKs/mrz/docs/Configuration_options.html
			\returns a result
		*/
		static UltMrzSdkResult init(jobject assetManager, const std::string& jsonConfig = "");
#else
		/*! Initializes the engine. This function must be the first one to call.
			\param jsonConfig JSON string containing configuration entries. May be null. More info at https://www.doubango.org/SDKs/mrz/docs/Configuration_options.html
			\returns a \ref UltMrzSdkResult "result"
		*/
		static UltMrzSdkResult init(const std::string& jsonConfig = "");
#endif /* ULTMRZ_SDK_OS_ANDROID */

		/*! DeInitialize the engine. This function must be the last one to be call.
			Deallocate all the resources allocated using \ref init function.
			\returns a \ref UltMrzSdkResult "result"
		*/
		static UltMrzSdkResult deInit();

		/*! Performs ANPR detection and recognition operations.
			\param imageType The image type.
			\param imageData Pointer to the image data.
			\param imageWidthInSamples Image width in samples.
			\param imageHeightInSamples Image height in samples.
			\param imageStrideInSamples Image stride in samples. Should be zero unless your the data is strided.
			\param imageExifOrientation Image EXIF/JPEG orientation. Must be within [1, 8]. More information at https://www.impulseadventure.com/photo/exif-orientation.html
		*/
		static UltMrzSdkResult process(
			const ULTMRZ_SDK_IMAGE_TYPE imageType, 
			const void* imageData, 
			const size_t imageWidthInSamples,
			const size_t imageHeightInSamples,
			const size_t imageStrideInSamples = 0,
			const int imageExifOrientation = 1
		);

		/*! Performs ANPR detection and recognition operations.
			\param imageType The image type.
			\param yPtr Pointer to the start of the Y (luma) samples.
			\param uPtr Pointer to the start of the U (chroma) samples.
			\param vPtr Pointer to the start of the V (chroma) samples.
			\param widthInSamples Image width in samples.
			\param heightInSamples Image height in samples.
			\param yStrideInBytes Stride in bytes for the Y (luma) samples.
			\param uStrideInBytes Stride in bytes for the U (chroma) samples.
			\param vStrideInBytes Stride in bytes for the V (chroma) samples.
			\param uvPixelStrideInBytes Pixel stride in bytes for the UV (chroma) samples. Should be 1 for planar and 2 for semi-planar formats. Set to 0 for auto-detect.
			\param exifOrientation Image EXIF/JPEG orientation. Must be within [1, 8]. More information at https://www.impulseadventure.com/photo/exif-orientation.html
		*/
		static UltMrzSdkResult process(
			const ULTMRZ_SDK_IMAGE_TYPE imageType,
			const void* yPtr,
			const void* uPtr,
			const void* vPtr,
			const size_t widthInSamples,
			const size_t heightInSamples,
			const size_t yStrideInBytes,
			const size_t uStrideInBytes,
			const size_t vStrideInBytes,
			const size_t uvPixelStrideInBytes = 0,
			const int exifOrientation = 1
		);

#if ULTMRZ_SDK_OS_ANDROID && !defined(SWIG)
		static void setAssetManager(AAssetManager* assetManager);
#endif /* ULTMRZ_SDK_OS_ANDROID */
	};

} // namespace ultimateMrzSdk 

#endif /* _ULTIMATE_MRZ_SDK_API_PUBLIC_H_ */
