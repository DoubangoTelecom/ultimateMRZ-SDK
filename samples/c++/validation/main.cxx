/* Copyright (C) 2011-2020 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: For non commercial use only.
* Source code: https://github.com/DoubangoTelecom/ultimateMRZ-SDK
* WebSite: https://www.doubango.org/webapps/mrz/
*/
// More info about validation application: https://www.doubango.org/SDKs/mrz/docs/Data_validation.html
/*
	https://github.com/DoubangoTelecom/ultimateMRZ-SDK/blob/master/samples/c++/validation/README.md
	Usage: 
		validation <path-to-file-containing mrz-lines>

	Example:
		validation C:/Projects/GitHub/ultimate/ultimateMRZ/SDK_dist/assets/samples/td1.txt
		
*/
#include <assert.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <string>
#include <regex>
#include <sstream>
#include <sys/stat.h>

// Assertion function
#if !defined(ULTMRZ_SDK_ASSERT)
#	if defined(NDEBUG)
#		define ULTMRZ_SDK_ASSERT(x) do { bool __ULTMRZ_SDK_b_ret = (x); if (!__ULTMRZ_SDK_b_ret) { ULTMRZ_SDK_PRINT_FATAL("Assertion failed!"); abort(); } } while(0)
#	else
#		define ULTMRZ_SDK_ASSERT(x) do { bool __ULTMRZ_SDK_b_ret = (x); assert(__ULTMRZ_SDK_b_ret); } while(0)
#	endif /* !NDEBUG */
#endif

// Functions for Logging
#if !defined (ULTMRZ_SDK_PRINT_VERBOSE)
#	if (defined(__ANDROID__) || defined(ANDROID))
#		include <android/log.h>
#		define ULTMRZ_SDK_PRINT_VERBOSE(FMT, ...) __android_log_print(ANDROID_LOG_VERBOSE, "org.doubango.ultimateMrz.Sdk.parser", "*[ULTMRZ_SDK VERBOSE]: " FMT "\n", ##__VA_ARGS__)
#		define ULTMRZ_SDK_PRINT_INFO(FMT, ...) __android_log_print(ANDROID_LOG_INFO, "org.doubango.ultimateMrz.Sdk.parser", "*[ULTMRZ_SDK INFO]: " FMT "\n", ##__VA_ARGS__)
#		define ULTMRZ_SDK_PRINT_WARN(FMT, ...) __android_log_print(ANDROID_LOG_WARN, "org.doubango.ultimateMrz.Sdk.parser", "**[ULTMRZ_SDK WARN]: function: \"%s()\" \nfile: \"%s\" \nline: \"%u\" \nmessage: " FMT "\n", __FUNCTION__,  __FILE__, __LINE__, ##__VA_ARGS__)
#		define ULTMRZ_SDK_PRINT_ERROR(FMT, ...) __android_log_print(ANDROID_LOG_ERROR, "org.doubango.ultimateMrz.Sdk.parser", "***[ULTMRZ_SDK ERROR]: function: \"%s()\" \nfile: \"%s\" \nline: \"%u\" \nmessage: " FMT "\n", __FUNCTION__,  __FILE__, __LINE__, ##__VA_ARGS__)
#		define ULTMRZ_SDK_PRINT_FATAL(FMT, ...) __android_log_print(ANDROID_LOG_FATAL, "org.doubango.ultimateMrz.Sdk.parser", "****[ULTMRZ_SDK FATAL]: function: \"%s()\" \nfile: \"%s\" \nline: \"%u\" \nmessage: " FMT "\n", __FUNCTION__,  __FILE__, __LINE__, ##__VA_ARGS__)
#	else
#		define ULTMRZ_SDK_PRINT_VERBOSE(FMT, ...) fprintf(stderr, "*[ULTMRZ_SDK VERBOSE]: " FMT "\n", ##__VA_ARGS__)
#		define ULTMRZ_SDK_PRINT_INFO(FMT, ...) fprintf(stderr, "*[ULTMRZ_SDK INFO]: " FMT "\n", ##__VA_ARGS__)
#		define ULTMRZ_SDK_PRINT_WARN(FMT, ...) fprintf(stderr, "**[ULTMRZ_SDK WARN]: function: \"%s()\" \nfile: \"%s\" \nline: \"%u\" \nmessage: " FMT "\n", __FUNCTION__,  __FILE__, __LINE__, ##__VA_ARGS__)
#		define ULTMRZ_SDK_PRINT_ERROR(FMT, ...) fprintf(stderr, "***[ULTMRZ_SDK ERROR]: function: \"%s()\" \nfile: \"%s\" \nline: \"%u\" \nmessage: " FMT "\n", __FUNCTION__,  __FILE__, __LINE__, ##__VA_ARGS__)
#		define ULTMRZ_SDK_PRINT_FATAL(FMT, ...) fprintf(stderr, "****[ULTMRZ_SDK FATAL]: function: \"%s()\" \nfile: \"%s\" \nline: \"%u\" \nmessage: " FMT "\n", __FUNCTION__,  __FILE__, __LINE__, ##__VA_ARGS__)
#	endif /* !ANDROID */
#endif

enum MRZ_DOCUMENT_TYPE {
	MRZ_DOCUMENT_TYPE_UNKNOWN,
	MRZ_DOCUMENT_TYPE_TD1,
	MRZ_DOCUMENT_TYPE_TD2,
	MRZ_DOCUMENT_TYPE_TD3,
	MRZ_DOCUMENT_TYPE_MRVA,
	MRZ_DOCUMENT_TYPE_MRVB
};

#define MRZ_BOOL_TO_STRING(val) ((val) ? "OK" : "**NOK**")

#define MRZ_COMPUTE_WEIGHTED_SUM(_line_, _start_, _end_, w) { \
		for (size_t i = _start_, j = w; i <= _end_; ++i, ++j) { \
			sum += __MappedValues[(_line_)[i]] * __Weights[j % 3]; \
		} \
	}

#define MRZ_CHECK_VALIDITY(_line_, _start_, _end_, _check_, _ret_) { \
		const std::string __line__ = (_line_); \
		int sum = 0; \
		MRZ_COMPUTE_WEIGHTED_SUM(__line__, _start_, _end_, 0); \
		_ret_ = ((sum % 10) == __MappedValues[__line__[_check_]]); \
	}

static const int __Weights[] = { 7, 3, 1 };

static std::map<char, int> __MappedValues;

int main(int argc, char *argv[])
{
	if (argc != 2) {
		ULTMRZ_SDK_PRINT_INFO(
			"\nUsage:\n"
			"validation <path-to-file-containing mrz-lines>\n"
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

	// Get MRZ document type
	const MRZ_DOCUMENT_TYPE docType =
		(lines.size() == 3 && lines.front().size() == 30) ? MRZ_DOCUMENT_TYPE_TD1
		: ((lines.front().size() == 44 && lines.size() == 2) ? (lines.front()[0] == 'P' ? MRZ_DOCUMENT_TYPE_TD3 : MRZ_DOCUMENT_TYPE_MRVA)
			: ((lines.front().size() == 36 && lines.size() == 2) ? (lines.front()[0] == 'V' ? MRZ_DOCUMENT_TYPE_MRVB : MRZ_DOCUMENT_TYPE_TD2) : MRZ_DOCUMENT_TYPE_UNKNOWN));
	
	if (docType == MRZ_DOCUMENT_TYPE_UNKNOWN) {
		ULTMRZ_SDK_PRINT_ERROR("Invalid MRZ format");
		return -1;
	}

	// Build map values
	if (__MappedValues.empty()) {
		const std::string charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		for (int i = 0; i < static_cast<int>(charset.size()); ++i) {
			__MappedValues[charset[i]] = i;
		}
		__MappedValues['<'] = 0;
	}

	// Checking
	ULTMRZ_SDK_PRINT_INFO("=========================");
	switch (docType) {
		case MRZ_DOCUMENT_TYPE_TD1: {
			// 9303_p5_cons_en.pdf - 4.2.4 Check digits in the MRZ
			bool documentNumber, dateOfBirth, dateOfExpiry, upperAndMiddleLines;
			MRZ_CHECK_VALIDITY(lines[0], 5, 13, 14, documentNumber);
			MRZ_CHECK_VALIDITY(lines[1], 0, 5, 6, dateOfBirth);
			MRZ_CHECK_VALIDITY(lines[1], 8, 13, 14, dateOfExpiry);
			int sum = 0;
			MRZ_COMPUTE_WEIGHTED_SUM(lines[0], 5, 29, 0);
			MRZ_COMPUTE_WEIGHTED_SUM(lines[1], 0, 6, 25);
			MRZ_COMPUTE_WEIGHTED_SUM(lines[1], 8, 14, 32);
			MRZ_COMPUTE_WEIGHTED_SUM(lines[1], 18, 28, 43);
			upperAndMiddleLines = (sum % 10) == __MappedValues[lines[1][29]];
			ULTMRZ_SDK_PRINT_INFO("Document number: %s", MRZ_BOOL_TO_STRING(documentNumber));
			ULTMRZ_SDK_PRINT_INFO("Date of birth: %s", MRZ_BOOL_TO_STRING(dateOfBirth));
			ULTMRZ_SDK_PRINT_INFO("Date of expiry: %s", MRZ_BOOL_TO_STRING(dateOfExpiry));
			ULTMRZ_SDK_PRINT_INFO("Upper and middle lines: %s", MRZ_BOOL_TO_STRING(upperAndMiddleLines));
			break;
		}
		case MRZ_DOCUMENT_TYPE_TD2: {
			// 9303_p6_cons_en.pdf - 4.2.4 Check digits in the MRZ
			bool documentNumber, dateOfBirth, dateOfExpiry, composite;
			MRZ_CHECK_VALIDITY(lines[1], 0, 8, 9, documentNumber);
			MRZ_CHECK_VALIDITY(lines[1], 13, 18, 19, dateOfBirth);
			MRZ_CHECK_VALIDITY(lines[1], 21, 26, 27, dateOfExpiry);
			int sum = 0;
			MRZ_COMPUTE_WEIGHTED_SUM(lines[1], 0, 9, 0);
			MRZ_COMPUTE_WEIGHTED_SUM(lines[1], 13, 19, 10);
			MRZ_COMPUTE_WEIGHTED_SUM(lines[1], 21, 34, 17);
			composite = (sum % 10) == __MappedValues[lines[1][35]];
			ULTMRZ_SDK_PRINT_INFO("Document number: %s", MRZ_BOOL_TO_STRING(documentNumber));
			ULTMRZ_SDK_PRINT_INFO("Date of birth: %s", MRZ_BOOL_TO_STRING(dateOfBirth));
			ULTMRZ_SDK_PRINT_INFO("Date of expiry: %s", MRZ_BOOL_TO_STRING(dateOfExpiry));
			ULTMRZ_SDK_PRINT_INFO("Composite: %s", MRZ_BOOL_TO_STRING(composite));
			break;
		}
		case MRZ_DOCUMENT_TYPE_TD3: {
			// 9303_p4_cons_en.pdf - 4.2.4 Check digits in the MRZ
			bool passportNumber, dateOfBirth, dateOfExpiry, personalNumber, composite;
			MRZ_CHECK_VALIDITY(lines[1], 0, 8, 9, passportNumber);
			MRZ_CHECK_VALIDITY(lines[1], 13, 18, 19, dateOfBirth);
			MRZ_CHECK_VALIDITY(lines[1], 21, 26, 27, dateOfExpiry);
			MRZ_CHECK_VALIDITY(lines[1], 28, 41, 42, personalNumber);
			int sum = 0;
			MRZ_COMPUTE_WEIGHTED_SUM(lines[1], 0, 9, 0);
			MRZ_COMPUTE_WEIGHTED_SUM(lines[1], 13, 19, 10);
			MRZ_COMPUTE_WEIGHTED_SUM(lines[1], 21, 42, 17);
			composite = (sum % 10) == __MappedValues[lines[1][43]];
			ULTMRZ_SDK_PRINT_INFO("Passport number: %s", MRZ_BOOL_TO_STRING(passportNumber));
			ULTMRZ_SDK_PRINT_INFO("Date of birth: %s", MRZ_BOOL_TO_STRING(dateOfBirth));
			ULTMRZ_SDK_PRINT_INFO("Date of expiry: %s", MRZ_BOOL_TO_STRING(dateOfExpiry));
			ULTMRZ_SDK_PRINT_INFO("Personal number: %s", MRZ_BOOL_TO_STRING(personalNumber));
			ULTMRZ_SDK_PRINT_INFO("Composite: %s", MRZ_BOOL_TO_STRING(composite));
			break;
		}
		case MRZ_DOCUMENT_TYPE_MRVA:
		case MRZ_DOCUMENT_TYPE_MRVB: {
			// 9303_p7_cons_en.pdf
			bool documentNumber, dateOfBirth, dateOfExpiry;
			MRZ_CHECK_VALIDITY(lines[1], 0, 8, 9, documentNumber);
			MRZ_CHECK_VALIDITY(lines[1], 13, 18, 19, dateOfBirth);
			MRZ_CHECK_VALIDITY(lines[1], 21, 26, 27, dateOfExpiry);
			ULTMRZ_SDK_PRINT_INFO("Document number: %s", MRZ_BOOL_TO_STRING(documentNumber));
			ULTMRZ_SDK_PRINT_INFO("Date of birth: %s", MRZ_BOOL_TO_STRING(dateOfBirth));
			ULTMRZ_SDK_PRINT_INFO("Date of expiry: %s", MRZ_BOOL_TO_STRING(dateOfExpiry));
			break;
		}
		default: {
			ULTMRZ_SDK_PRINT_ERROR("Invalid MRZ format");
			return -1;
		}
	}
	ULTMRZ_SDK_PRINT_INFO("=========================");

	// Press any key to terminate
	ULTMRZ_SDK_PRINT_INFO("Press any key to terminate !!");
	getchar();

	return 0;
}


