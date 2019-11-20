/* Copyright (C) 2016-2019 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: For non commercial use only.
* Source code: https://github.com/DoubangoTelecom/ultimateMRZ-SDK
* WebSite: https://www.doubango.org/webapps/mrz/
*/

/*
This is a sample application showing how to use Doubango's MRZ deep learning engine.
This is a preview code with very limited documentation to provide early access to our engine. More
documentation will be released in the coming days.

More information: https://github.com/DoubangoTelecom/ultimateMRZ-SDK/tree/master/preview
*/

#include <ultimateMRZ-SDK-API-PUBLIC.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/stat.h>

// Not part of the SDK, used to decode images -> https://github.com/nothings/stb
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"

using namespace ultimateMrzSdk;

// Configuration for MRZ deep learning engine
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
"\"interpolation\": \"bilinear\","
"\"min_num_lines\": 2,"
"\"roi\": [0, 0, 0, 0],"
"\"min_score\": 0"
"}";

/*
Entry point.
usage: mrz_sample input_path

example: mrz_sample images/Czech_passport_2005_MRZ_data_1300x1002.jpg
More formats: https://github.com/DoubangoTelecom/data/tree/master/mrz
Sample image at https://github.com/DoubangoTelecom/ultimateMRZ-SDK/tree/master/preview/images/Czech_passport_2005_MRZ_data_1300x1002.jpg
*/
int main(int argc, char *argv[])
{
	// Parse arguments
	if (argc != 2) {
		ULTMRZ_SDK_PRINT_ERROR(
			"Invalid number of parameters.\n"
			"Usage: mrz_sample input_path\n"
			"More info: https://github.com/DoubangoTelecom/ultimateMRZ-SDK/tree/master/preview"
		);
		return -1;
	}

	// Decode the file
	FILE* file = nullptr;
	if (!file && (file = fopen(argv[1], "rb")) == nullptr) {
		ULTMRZ_SDK_PRINT_ERROR("Can't open %s", argv[1]);
		return -1;
	}

	int width, height, channels;
	stbi_uc* uncompressedData = stbi_load_from_file(file, &width, &height, &channels, 0);
	fclose(file);
	if (!uncompressedData || !width || !height || (channels != 3 && channels != 4)) {
		ULTMRZ_SDK_PRINT_ERROR("Invalid file(%s, %d, %d, %d)", argv[1], width, height, channels);
		return -1;
	}

	// Init
	ULTMRZ_SDK_PRINT_INFO("Initialization...");
	UltMrzSdkResult result = UltMrzSdkResult::bodylessOK();
	ULTMRZ_SDK_ASSERT((result = UltMrzSdkEngine::init(
		__jsonConfig
	)).isOK());

	// Recognize/Process
	// Please note that the first time you call this function all deep learning models will be loaded 
	// and initialized which means it will be slow. In your application you've to initialize the engine
	// once and do all the recognitions you need then, deinitialize it.
	ULTMRZ_SDK_ASSERT((result = UltMrzSdkEngine::process(
		channels == 4 ? ULTMRZ_SDK_IMAGE_TYPE_RGBA32 : ULTMRZ_SDK_IMAGE_TYPE_RGB24,
		uncompressedData,
		static_cast<size_t>(width),
		static_cast<size_t>(height)
	)).isOK());

	// Print result
	// The JSON content contains the MRZ lines with the coordinates and texts.
	// If you're using a trial version then, some characters may be obfuscated and replaced with a placeholder (*).
	// Also, in the trial version the warped boxes for individual lines would only contains -1.0 values.
	// This preview code is almost identical to what is used on the cloud (https://www.doubango.org/webapps/mrz/) and 
	// you should expect the same accuracy on the detection part. The small difference is that the cloud uses bicubic
	// interpolation while this one use bilinear one. You can change the configuration to use bicubic interpolation.
	const std::string& json_ = result.json();
	if (!json_.empty()) {
		ULTMRZ_SDK_PRINT_INFO("result: %s", json_.c_str());
	}

	// free memory
	stbi_image_free(uncompressedData);

	// Press any key to terminate
	ULTMRZ_SDK_PRINT_INFO("Press any key to terminate !!");
	getchar();

	// DeInitinialize to free all allocated memories
	ULTMRZ_SDK_PRINT_INFO("Deinitializing the application...");
	ULTMRZ_SDK_ASSERT((result = UltMrzSdkEngine::deInit()).isOK());

	return 0;
}

