/* Copyright (C) 2011-2020 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: For non commercial use only.
* Source code: https://github.com/DoubangoTelecom/ultimateMRZ-SDK
* WebSite: https://www.doubango.org/webapps/mrz/
*/

/*
	Usage: 
		runtimeKey \
			[--json <json-output:bool>] \
			[--assets <path-to-assets-folder>]
			[--type <host-type>]

	Example:
		runtimeKey \
			--json false \
			--assets C:/Projects/GitHub/ultimate/ultimateMRZ/SDK_dist/assets
		
*/

#include <ultimateMRZ-SDK-API-PUBLIC.h>
#include "../mrz_utils.h"

using namespace ultimateMrzSdk;

static void printUsage(const std::string& message = "");

/*
* Entry point
*/
int main(int argc, char *argv[])
{
	// Parsing args
	std::map<std::string, std::string > args;
	if (!mrzParseArgs(argc, argv, args)) {
		printUsage();
		return -1;
	}
	bool rawInsteadOfJSON = false;
	if (args.find("--json") != args.end()) {
		rawInsteadOfJSON = (args["--json"].compare("true") != 0);
	}
	
	// Build JSON string
	std::string jsonConfig;
	if (args.find("--assets") != args.end()) {
		jsonConfig += std::string("\"assets_folder\": \"") + args["--assets"] + std::string("\""); 
	}
	if (args.find("--type") != args.end()) {
		jsonConfig += (jsonConfig.empty() ? "" : ",") 
			+ std::string("\"host_type\": \"") + args["--type"] + std::string("\"");
	}
	jsonConfig = "{" + jsonConfig + "}";

	// Initialize the engine
	ULTMRZ_SDK_ASSERT(UltMrzSdkEngine::init(jsonConfig.c_str()).isOK());

	// Request runtime license key
	const UltMrzSdkResult result = UltMrzSdkEngine::requestRuntimeLicenseKey(rawInsteadOfJSON);
	if (result.isOK()) {
		ULTMRZ_SDK_PRINT_INFO("\n\n%s\n\n",
			result.json()
		);
	}
	else {
		ULTMRZ_SDK_PRINT_ERROR("\n\n*** Failed: code -> %d, phrase -> %s ***\n\n",
			result.code(),
			result.phrase()
		);
	}
	
	ULTMRZ_SDK_PRINT_INFO("Press any key to terminate !!");
	getchar();

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
		"runtimeKey [--json <json-output:bool>] \n"
		"\n"
		"Options surrounded with [] are optional.\n"
		"\n"
		"--json: Whether to output the runtime license key as JSON string intead of raw string. Default: true.\n"
		"--assets: Path to the assets folder containing the configuration files and models. Default value is the current folder.\n"
		"--type: Defines how the license is attached to the machine/host. Possible values are 'aws-instance' or 'aws-byol'. Default: null.\n"
		"********************************************************************************\n"
	);
}
