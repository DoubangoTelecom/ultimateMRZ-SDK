/* Copyright (C) 2011-2021 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: For non commercial use only.
* Source code: https://github.com/DoubangoTelecom/ultimateMRZ-SDK
* WebSite: https://www.doubango.org/webapps/mrz/
*/

/*
	https://github.com/DoubangoTelecom/ultimateMRZ/blob/master/SDK_dist/samples/c++/recognizer/README.md
	Usage: 
		recognizer \
			--image <path-to-image-with-mrzdata-to-recognize> \
			[--assets <path-to-assets-folder>] \
			[--backprop <whether-to-enable-backpropagation:true/false>] \
			[--vcheck <whether-to-enable-vertical-check:true/false>] \
			[--ielcd <whether-to-enable-IELCD:true/false>] \
			[--tokenfile <path-to-license-token-file>] \
			[--tokendata <base64-license-token-data>]
	Example:
		recognizer \
			--image C:/Projects/GitHub/ultimate/ultimateMRZ/SDK_dist/assets/images/Czech_passport_2005_MRZ_orient1_1300x1002.jpg \
			--assets C:/Projects/GitHub/ultimate/ultimateMRZ/SDK_dist/assets \
			--backprop true \
			--vcheck true \
			--tokenfile C:/Projects/GitHub/ultimate/ultimateMRZ/SDK_dev/tokens/windows-iMac.lic
		
*/

#include <ultimateMRZ-SDK-API-PUBLIC.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <map>
#include <algorithm>

// Not part of the SDK, used to decode images -> https://github.com/nothings/stb
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "../stb_image.h"

using namespace ultimateMrzSdk;

struct MrzFile {
	int width = 0, height = 0, channels = 0;
	stbi_uc* uncompressedDataPtr = nullptr;
	void* compressedDataPtr = nullptr;
	size_t compressedDataSize = 0;
	FILE* filePtr = nullptr;
	virtual ~MrzFile() {
		if (uncompressedDataPtr) free(uncompressedDataPtr), uncompressedDataPtr = nullptr;
		if (compressedDataPtr) free(compressedDataPtr), compressedDataPtr = nullptr;
		if (filePtr) fclose(filePtr), filePtr = nullptr;
	}
	bool isValid() const {
		return width > 0 && height > 0 && (channels == 1 || channels == 3 || channels == 4) && uncompressedDataPtr && compressedDataPtr && compressedDataSize > 0;
	}
};

static void printUsage(const std::string& message = "");
static bool parseArgs(int argc, char *argv[], std::map<std::string, std::string >& values);
static bool readFile(const std::string& path, MrzFile& file);

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
"\"gamma\": -1,"
"\"interpolation\": \"bilinear\","
"\"min_num_lines\": 2,"
"\"roi\": [0, 0, 0, 0],"
"\"min_score\": 0.0"
;

/*
	Entry point.
*/
int main(int argc, char *argv[])
{
	// local variables
	UltMrzSdkResult result = UltMrzSdkResult::bodylessOK();
	std::string assetsFolder, licenseTokenData, licenseTokenFile;
	std::string pathFileImage;
#if defined(__arm__) || defined(__thumb__) || defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB) || defined(_ARM) || defined(_M_ARM) || defined(_M_ARMT) || defined(__arm) || defined(__aarch64__)
	bool backpropEnabled = false;
	bool ielcdEnabled = false;
	bool vcheckEnabled = false;
#else
	bool backpropEnabled = true;
	bool ielcdEnabled = true;
	bool vcheckEnabled = true;
#endif

	// Parsing args
	std::map<std::string, std::string > args;
	if (!parseArgs(argc, argv, args)) {
		printUsage();
		return -1;
	}
	if (args.find("--image") == args.end()) {
		printUsage("--image required");
		return -1;
	}
	pathFileImage = args["--image"];
	
	if (args.find("--assets") != args.end()) {
		assetsFolder = args["--assets"];
#if defined(_WIN32)
		std::replace(assetsFolder.begin(), assetsFolder.end(), '\\', '/');
#endif
	}
	if (args.find("--backprop") != args.end()) {
		backpropEnabled = (args["--backprop"] == "true");
	}
	if (args.find("--vcheck") != args.end()) {
		vcheckEnabled = (args["--vcheck"] == "true");
	}
	if (args.find("--ielcd") != args.end()) {
		ielcdEnabled = (args["--ielcd"] == "true");
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
	jsonConfig += std::string(",\"backpropagation_enabled\": ") + (backpropEnabled ? "true" : "false");
	jsonConfig += std::string(",\"vertical_check_enabled\": ") + (vcheckEnabled ? "true" : "false");
	jsonConfig += std::string(",\"ielcd_enabled\": ") + (ielcdEnabled ? "true" : "false");
	if (!licenseTokenFile.empty()) {
		jsonConfig += std::string(",\"license_token_file\": \"") + licenseTokenFile + std::string("\"");
	}
	if (!licenseTokenData.empty()) {
		jsonConfig += std::string(",\"license_token_data\": \"") + licenseTokenData + std::string("\"");
	}

	jsonConfig += "}"; // end-of-config

	// Decode the file
	MrzFile file;
	if (!readFile(pathFileImage, file)) {
		ULTMRZ_SDK_PRINT_ERROR("Can't process %s", pathFileImage.c_str());
		return -1;
	}
	ULTMRZ_SDK_ASSERT(file.isValid());

	// Init
	ULTMRZ_SDK_PRINT_INFO("Initialization...");
	ULTMRZ_SDK_ASSERT((result = UltMrzSdkEngine::init(
		jsonConfig.c_str()
	)).isOK());

	// Recognize/Process
	// Please note that the first time you call this function all deep learning models will be loaded 
	// and initialized which means it will be slow. In your application you've to initialize the engine
	// once and do all the recognitions you need then, deinitialize it.
	ULTMRZ_SDK_ASSERT((result = UltMrzSdkEngine::process(
		file.channels == 4 ? ULTMRZ_SDK_IMAGE_TYPE_RGBA32 : (file.channels == 1 ? ULTMRZ_SDK_IMAGE_TYPE_Y : ULTMRZ_SDK_IMAGE_TYPE_RGB24),
		file.uncompressedDataPtr,
		static_cast<size_t>(file.width),
		static_cast<size_t>(file.height),
		0, // stride
		UltMrzSdkEngine::exifOrientation(file.compressedDataPtr, file.compressedDataSize)
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

	// Press any key to terminate
	ULTMRZ_SDK_PRINT_INFO("Press any key to terminate !!");
	getchar();

	// DeInitinialize to free all allocated memories
	ULTMRZ_SDK_PRINT_INFO("Deinitializing the application...");
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
		"recognizer\n"
		"\t--image <path-to-image-with-mrzdata-to-recognize> \n"
		"\t[--assets <path-to-assets-folder>] \n"
		"\t[--backprop <whether-to-enable-backpropagation:true/false>] \n"
		"\t[--ielcd <whether-to-enable-IELCD:true/false>] \n"
		"\t[--tokenfile <path-to-license-token-file>] \n"
		"\t[--tokendata <base64-license-token-data>] \n"
		"\n"
		"Options surrounded with [] are optional.\n"
		"\n"
		"--image: Path to the image(JPEG/PNG/BMP) to process. You can use default image at ../../../assets/images/Czech_passport_2005_MRZ_orient1_1300x1002.jpg.\n"
		"--assets: Path to the assets folder containing the configuration files and models. Default value is the current folder.\n"
		"--backprop: Whether to enable backpropagation to detect the MICR lines. Only CMC-7 font uses this option. More information at https://www.doubango.org/SDKs/mrz/docs/Detection_techniques.html#backpropagation. Default: true for x86 CPUs and false for ARM CPUs.\n\n"
		"--vcheck: Whether to enable vertical check to detect +/-90deg rotated images. Default: true for x86 CPUs and false for ARM CPUs.\n\n"
		"--ielcd: Whether to enable Image Enhancement for Low Contrast Document (IELCD). More information at https://www.doubango.org/SDKs/mrz/docs/IELCD.html#ielcd. Default: true for x86 CPUs and false for ARM CPUs.\n\n"
		"--tokenfile: Path to the file containing the base64 license token if you have one. If not provided then, the application will act like a trial version. Default: null.\n"
		"--tokendata: Base64 license token if you have one. If not provided then, the application will act like a trial version. Default: null.\n"
		"********************************************************************************\n"
	);
}

static bool parseArgs(int argc, char *argv[], std::map<std::string, std::string >& values)
{
	ULTMRZ_SDK_ASSERT(argc > 0 && argv != nullptr);

	values.clear();

	// Make sure the number of arguments is even
	if ((argc - 1) & 1) {
		ULTMRZ_SDK_PRINT_ERROR("Number of args must be even");
		return false;
	}

	// Parsing
	for (int index = 1; index < argc; index += 2) {
		std::string key = argv[index];
		if (key.size() < 2 || key[0] != '-' || key[1] != '-') {
			ULTMRZ_SDK_PRINT_ERROR("Invalid key: %s", key.c_str());
			return false;
		}
		values[key] = argv[index + 1];
	}

	return true;
}

static bool readFile(const std::string& path, MrzFile& file)
{
	// Open the file
	if ((file.filePtr = fopen(path.c_str(), "rb")) == nullptr) {
		ULTMRZ_SDK_PRINT_ERROR("Can't open %s", path.c_str());
		return false;
	}

	// Retrieve file size
	struct stat st_;
	if (stat(path.c_str(), &st_) != 0) {
		ULTMRZ_SDK_PRINT_ERROR("File is empty %s", path.c_str());
	}
	file.compressedDataSize = static_cast<size_t>(st_.st_size);

	// Alloc memory and read data
	file.compressedDataPtr = ::malloc(file.compressedDataSize);
	if (!file.compressedDataPtr) {
		ULTMRZ_SDK_PRINT_ERROR("Failed to alloc mem with size = %zu", file.compressedDataSize);
		return false;
	}
	size_t read_;
	if (file.compressedDataSize != (read_ = fread(file.compressedDataPtr, 1, file.compressedDataSize, file.filePtr))) {
		ULTMRZ_SDK_PRINT_ERROR("fread(%s) returned %zu instead of %zu", path.c_str(), read_, file.compressedDataSize);
		return false;
	}

	// Decode image
	file.uncompressedDataPtr = stbi_load_from_memory(
		reinterpret_cast<stbi_uc const *>(file.compressedDataPtr), static_cast<int>(file.compressedDataSize), 
		&file.width, &file.height, &file.channels, 0
	);
	
	return file.isValid();
}
