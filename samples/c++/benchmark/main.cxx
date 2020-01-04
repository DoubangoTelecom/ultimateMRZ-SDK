/* Copyright (C) 2011-2019 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: For non commercial use only.
* Source code: https://github.com/DoubangoTelecom/ultimateMRZ-SDK
* WebSite: https://www.doubango.org/webapps/mrz/
*/

// More info about benchmark application: https://www.doubango.org/SDKs/mrz/docs/Benchmark.html
/*
	https://github.com/DoubangoTelecom/ultimateMRZ-SDK/blob/master/samples/c++/benchmark/README.md
	Usage: 
		benchmark \
			--positive <path-to-image-with-mrzlines> \
			--negative <path-to-image-without-mrzlines> \
			[--assets <path-to-assets-folder>] \
			[--loops <number-of-times-to-run-the-loop:[1, inf]>] \
			[--rate <positive-rate:[0.0, 1.0]>] \
			[--tokenfile <path-to-license-token-file>] \
			[--tokendata <base64-license-token-data>]

	Example:
		benchmark \
			--positive C:/Projects/GitHub/ultimate/ultimateMRZ/SDK_dist/assets/images/Passport-Australia_1280x720.jpg \
			--negative C:/Projects/GitHub/ultimate/ultimateMRZ/SDK_dist/assets/images/Passport-France_1200x864.jpg \
			--loops 100 \
			--rate 0.2 \
			--assets C:/Projects/GitHub/ultimate/ultimateMRZ/SDK_dist/assets \
			--tokenfile C:/Projects/GitHub/ultimate/ultimateMRZ/SDK_dev/tokens/windows-iMac.lic
		
*/

#include <ultimateMRZ-SDK-API-PUBLIC.h>
#include "../mrz_utils.h"
#include <chrono>
#include <vector>
#include <algorithm>
#include <random>
#if defined(_WIN32)
#include <algorithm> // std::replace
#endif

using namespace ultimateMrzSdk;

// Configuration for MRZ deep learning engine
// More info about JSON configuration entries: https://www.doubango.org/SDKs/mrz/docs/Configuration_options.html
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
;

// Asset manager used on Android to files in "assets" folder
#if ULTMRZ_SDK_OS_ANDROID 
#	define ASSET_MGR_PARAM() __sdk_android_assetmgr, 
#else
#	define ASSET_MGR_PARAM() 
#endif /* ULTMRZ_SDK_OS_ANDROID */

static void printUsage(const std::string& message = "");

/*
* Entry point
*/
int main(int argc, char *argv[])
{
	// local variables
	UltMrzSdkResult result(0, "OK", "{}");
	std::string assetsFolder, licenseTokenData, licenseTokenFile;
	size_t loopCount = 100;
	double percentPositives = .2; // 20%
	std::string pathFilePositive;
	std::string pathFileNegative;

	// Parsing args
	std::map<std::string, std::string > args;
	if (!mrzParseArgs(argc, argv, args)) {
		printUsage();
		return -1;
	}
	if (args.find("--positive") == args.end()) {
		printUsage("--positive required");
		return -1;
	}
	if (args.find("--negative") == args.end()) {
		printUsage("--negative required");
		return -1;
	}
	pathFilePositive = args["--positive"];
	pathFileNegative = args["--negative"];
	if (args.find("--rate") != args.end()) {
		const double rate = std::atof(args["--rate"].c_str());
		if (rate > 1.0 || rate < 0.0) {
			printUsage("--rate must be within [0.0, 1.0]");
			return -1;
		}
		percentPositives = rate;
	}
	if (args.find("--loops") != args.end()) {
		const int loops = std::atoi(args["--loops"].c_str());
		if (loops < 1) {
			printUsage("--loops must be within [1, inf]");
			return -1;
		}
		loopCount = static_cast<size_t>(loops);
	}

	if (args.find("--assets") != args.end()) {
		assetsFolder = args["--assets"];
#if defined(_WIN32)
		std::replace(assetsFolder.begin(), assetsFolder.end(), '\\', '/');
#endif
	}
	if (args.find("--tokenfile") != args.end()) {
		licenseTokenFile = args["--tokenfile"];
#if defined(_WIN32)
		std::replace(licenseTokenFile.begin(), licenseTokenFile.end(), '\\', '/');
#endif
	}
	if (args.find("--tokendata") != args.end()) {
		licenseTokenData = args["--tokendata"];
	}

	// Update JSON config
	std::string jsonConfig = __jsonConfig;
	if (!assetsFolder.empty()) {
		jsonConfig += std::string(",\"assets_folder\": \"") + assetsFolder + std::string("\"");
	}
	if (!licenseTokenFile.empty()) {
		jsonConfig += std::string(",\"license_token_file\": \"") + licenseTokenFile + std::string("\"");
	}
	if (!licenseTokenData.empty()) {
		jsonConfig += std::string(",\"license_token_data\": \"") + licenseTokenData + std::string("\"");
	}
	
	jsonConfig += "}"; // end-of-config

	// Read files
	// Positive: the file contains at least one MRZ zone
	// Negative: the file doesn't contain any MRZ line
	// Change positive rates to evaluate the detector versus recognizer
	MrzFile filePositive, fileNegative;
	if (!mrzDecodeFile(pathFilePositive, filePositive)) {
		ULTMRZ_SDK_PRINT_INFO("Failed to read positive file: %s", pathFilePositive.c_str());
		return -1;
	}
	if (!mrzDecodeFile(pathFileNegative, fileNegative)) {
		ULTMRZ_SDK_PRINT_INFO("Failed to read positive file: %s", pathFilePositive.c_str());
		return -1;
	}

	// Create image indices
	std::vector<size_t> indices(loopCount, 0);
	const int numPositives = (int)std::max(loopCount * percentPositives, 1.);
	for (int i = 0; i < numPositives; ++i) {
		indices[i] = 1; // positive index
	}
	std::shuffle(std::begin(indices), std::end(indices), std::default_random_engine{}); // make the indices random

	// Init
	ULTMRZ_SDK_PRINT_INFO("Starting benchmark...");
	ULTMRZ_SDK_ASSERT((result = UltMrzSdkEngine::init(
		ASSET_MGR_PARAM()
		jsonConfig.c_str()
	)).isOK());

	// Recognize/Process
	const std::chrono::high_resolution_clock::time_point timeStart = std::chrono::high_resolution_clock::now();
	const MrzFile* files[2] = { &fileNegative, &filePositive };
	UltMrzSdkResult resultPositive(0, "OK", "{}");
	for (const auto& indice : indices) {
		const MrzFile* file = files[indice];
		UltMrzSdkResult* result_ = indice ? &resultPositive : &result;
		ULTMRZ_SDK_ASSERT((*result_ = UltMrzSdkEngine::process(
			file->type,
			file->uncompressedData,
			file->width,
			file->height
		)).isOK());
	}
	const std::chrono::high_resolution_clock::time_point timeEnd = std::chrono::high_resolution_clock::now();
	const double elapsedTimeInMillis = std::chrono::duration_cast<std::chrono::duration<double >>(timeEnd - timeStart).count() * 1000.0;
	ULTMRZ_SDK_PRINT_INFO("Elapsed time (MRZ) = [[[ %lf millis ]]]", elapsedTimeInMillis);

	// Print one of the positive results
	const std::string& json_ = resultPositive.json();
	if (!json_.empty()) {
		ULTMRZ_SDK_PRINT_INFO("result: %s", json_.c_str());
	}

	// Print estimated frame rate
	const double estimatedFps = 1000.f / (elapsedTimeInMillis / (double)loopCount);
	ULTMRZ_SDK_PRINT_INFO("*** elapsedTimeInMillis: %lf, estimatedFps: %lf ***", elapsedTimeInMillis, estimatedFps);

	ULTMRZ_SDK_PRINT_INFO("Press any key to terminate !!");
	getchar();

	// DeInit
	ULTMRZ_SDK_PRINT_INFO("Ending benchmark...");
	ULTMRZ_SDK_ASSERT((result = UltMrzSdkEngine::deInit()).isOK());

	return 0;
}

/*
* Print usage
*/
static void printUsage(const std::string& message /*= ""*/)
{
	if (!message.empty()) {
		ULTMRZ_SDK_PRINT_ERROR("%s", message.c_str());
	}

	ULTMRZ_SDK_PRINT_INFO(
		"\n********************************************************************************\n"
		"benchmark\n"
		"\t--positive <path-to-image-with-mrzlines> \n"
		"\t--negative <path-to-image-without-mrzlines> \n"
		"\t[--assets <path-to-assets-folder>] \n"
		"\t[--loops <number-of-times-to-run-the-loop:[1, inf]>] \n"
		"\t[--rate <positive-rate:[0.0, 1.0]>] \n"
		"\t[--tokenfile <path-to-license-token-file>] \n"
		"\t[--tokendata <base64-license-token-data>] \n"
		"\n"
		"Options surrounded with [] are optional.\n"
		"\n"
		"--positive: Path to an image(JPEG/PNG/BMP) with a MRZ zone. This image will be used to evaluate the recognizer. You can use default image at ../../../assets/images/Passport-Australia_1280x720.jpg.\n"
		"--negative: Path to an image(JPEG/PNG/BMP) without MRZ lines. This image will be used to evaluate the detector. You can use default image at ../../../assets/images/Passport-France_1200x864.jpg.\n"
		"--assets: Path to the assets folder containing the configuration files and models. Default value is the current folder.\n"
		"--loops: Number of times to run the processing pipeline.\n"
		"--rate: Percentage value within[0.0, 1.0] defining the positive rate. The positive rate defines the percentage of images with MRZ lines.\n"
		"--tokenfile: Path to the file containing the base64 license token if you have one. If not provided then, the application will act like a trial version. Default: null.\n"
		"--tokendata: Base64 license token if you have one. If not provided then, the application will act like a trial version. Default: null.\n"
		"********************************************************************************\n"
	);
}
