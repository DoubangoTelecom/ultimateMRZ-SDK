/* Copyright (C) 2016-2019 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: For non commercial use only.
* Source code: https://github.com/DoubangoTelecom/ultimateMRZ-SDK
* WebSite: https://www.doubango.org/webapps/mrz/
*/
// More info about parser application: https://www.doubango.org/SDKs/mrz/docs/MRZ_parser.html
/*
	https://github.com/DoubangoTelecom/ultimateMRZ-SDK/blob/master/samples/c++/parser/README.md
	Usage: 
		parser <path-to-file-containing mrz-lines>

	Example:
		parser C:/Projects/GitHub/ultimate/ultimateMRZ/SDK_dist/assets/samples/td1.txt
		
*/
#include "../mrz_parser.h"

#include <map>
#include <vector>
#include <sstream>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
	if (argc != 2) {
		ULTMRZ_SDK_PRINT_INFO(
			"\nUsage:\n"
			"parser <path-to-file-containing mrz-lines>\n"
			"\n"
		);
		return -1;
	}
	
	// Build lines using data from the file
	const std::string filePath = argv[1];
	FILE* fp = fopen(filePath.c_str(), "rb");
	if (!fp) {
		ULTMRZ_SDK_PRINT_FATAL("Cannot open file: %s", filePath.c_str());
		return -1;
	}

	struct stat st_;
	ULTMRZ_SDK_ASSERT(stat(filePath.c_str(), &st_) == 0);
	const size_t size = static_cast<size_t>(st_.st_size);
	ULTMRZ_SDK_ASSERT(size > 0);

	void* data = malloc(size + 1);
	ULTMRZ_SDK_ASSERT(data != nullptr);
	ULTMRZ_SDK_ASSERT(size == fread(data, 1, size, fp));
	fclose(fp);
	reinterpret_cast<char*>(data)[size] = '\0';

	std::stringstream stream(reinterpret_cast<const char*>(data));
	free(data);
	std::vector<std::string> lines;
	for (std::string line; std::getline(stream, line); ) {
		line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
		ULTMRZ_SDK_PRINT_INFO("Line #%zu: %s [%zu]", lines.size(), line.c_str(), line.size());
		lines.push_back(line);
	}
	if (lines.size() != 2 && lines.size() != 3) {
		ULTMRZ_SDK_PRINT_FATAL("%zu not a valid number of lines. Expecting 2 or 3 lines", lines.size());
		return -1;
	}

	// Make sure all lines have same length
	for (size_t i = 1; i < lines.size(); ++i) {
		if (lines.front().size() != lines[i].size()) {
			ULTMRZ_SDK_PRINT_FATAL("All lines must have same length: %zu != %zu", lines.front().size(), lines[i].size());
			return false;
		}
	}

	// Process
	MrzData mrz_data;
	if (!__mrz_parser_process(lines, mrz_data)) {
		ULTMRZ_SDK_PRINT_FATAL("Processing failed");
		return -1;
	}

	// Print result
	ULTMRZ_SDK_PRINT_INFO("=========================");
	switch (mrz_data.type) {
		case MRZ_DOCUMENT_TYPE_TD1: ULTMRZ_SDK_PRINT_INFO("=== Document Type: TD1 ==="); break;
		case MRZ_DOCUMENT_TYPE_TD2: ULTMRZ_SDK_PRINT_INFO("=== Document Type: TD2 ==="); break;
		case MRZ_DOCUMENT_TYPE_TD3: ULTMRZ_SDK_PRINT_INFO("=== Document Type: TD3 ==="); break;
		case MRZ_DOCUMENT_TYPE_MRVA: ULTMRZ_SDK_PRINT_INFO("=== Document Type: MRVA ==="); break;
		case MRZ_DOCUMENT_TYPE_MRVB: ULTMRZ_SDK_PRINT_INFO("=== Document Type: MRVB ==="); break;
		default:  ULTMRZ_SDK_ASSERT(false); break;
	}
	for (const auto& field : mrz_data.fields) {
		ULTMRZ_SDK_PRINT_INFO("%s:\t%s", field.first.c_str(), field.second.c_str());
	}
	ULTMRZ_SDK_PRINT_INFO("=========================");

	// Press any key to terminate
	ULTMRZ_SDK_PRINT_INFO("Press any key to terminate !!");
	getchar();

	return 0;
}


