/* Copyright (C) 2016-2019 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: For non commercial use only.
* Source code: https://github.com/DoubangoTelecom/ultimateMRZ-SDK
* WebSite: https://www.doubango.org/webapps/mrz/
*/

#if !defined(_ULTIMATE_MRZ_SDK_SAMPLES_MRZ_PARSER_H_)
#define _ULTIMATE_MRZ_SDK_SAMPLES_MRZ_PARSER_H_

#include <assert.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <regex>
#include <sstream>

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

#define MRZ_REGEX_ANYCHAR					"[A-Z0-9<]"
#define MRZ_REGEX_09						"[0-9]"
#define MRZ_REGEX_AZ						"[A-Z]"
#define MRZ_REGEX_SEX						"[M|F|X|<]"
#define MRZ_REGEX_ANYCHAR_GROUPMATCH(nn)	"(" MRZ_REGEX_ANYCHAR "{" #nn "})"
#define MRZ_REGEX_09_GROUPMATCH(nn)			"(" MRZ_REGEX_09 "{" #nn "})"
#define MRZ_REGEX_AZ_GROUPMATCH(nn)			"(" MRZ_REGEX_AZ "{" #nn "})"
#define MRZ_REGEX_SEX_GROUPMATCH(nn)		"(" MRZ_REGEX_SEX "{" #nn "})"

enum MRZ_DOCUMENT_TYPE {
	MRZ_DOCUMENT_TYPE_UNKNOWN,
	MRZ_DOCUMENT_TYPE_TD1,
	MRZ_DOCUMENT_TYPE_TD2,
	MRZ_DOCUMENT_TYPE_TD3,
	MRZ_DOCUMENT_TYPE_MRVA,
	MRZ_DOCUMENT_TYPE_MRVB
};

typedef std::pair<std::string, std::string> MrzField;
typedef std::vector<MrzField> MrzFieldsVector;

struct MrzData {
	MRZ_DOCUMENT_TYPE type;
	MrzFieldsVector fields;
};

static bool __mrz_parser_processTD1(const std::vector<std::string>& lines, MrzFieldsVector& fields);
static bool __mrz_parser_processTD2(const std::vector<std::string>& lines, MrzFieldsVector& fields);
static bool __mrz_parser_processTD3(const std::vector<std::string>& lines, MrzFieldsVector& fields);
static bool __mrz_parser_processMRVA(const std::vector<std::string>& lines, MrzFieldsVector& fields);
static bool __mrz_parser_processMRVB(const std::vector<std::string>& lines, MrzFieldsVector& fields);

static bool(*__mrz_parser_processXXXX[])(const std::vector<std::string>& lines, MrzFieldsVector& fields) = {
	nullptr, // UNKNOWN
	__mrz_parser_processTD1,
	__mrz_parser_processTD2,
	__mrz_parser_processTD3,
	__mrz_parser_processMRVA,
	__mrz_parser_processMRVB
};

static bool __mrz_parser_process(const std::vector<std::string>& lines, MrzData& data)
{
	// Make sure we have 2 or 3 lines
	if (lines.size() != 2 && lines.size() != 3) {
		ULTMRZ_SDK_PRINT_FATAL("%zu not a valid number of lines. Expecting 2 or 3 lines", lines.size());
		return false;
	}

	// Make sure all lines have same length
	for (size_t i = 1; i < lines.size(); ++i) {
		if (lines.front().size() != lines[i].size()) {
			ULTMRZ_SDK_PRINT_FATAL("All lines must have same length: %zu != %zu", lines.front().size(), lines[i].size());
			return false;
		}
	}

	// Get type
	data.type =
		(lines.size() == 3 && lines.front().size() == 30) ? MRZ_DOCUMENT_TYPE_TD1
		: ((lines.front().size() == 44 && lines.size() == 2) ? (lines.front()[0] == 'P' ? MRZ_DOCUMENT_TYPE_TD3 : MRZ_DOCUMENT_TYPE_MRVA)
			: ((lines.front().size() == 36 && lines.size() == 2) ? (lines.front()[0] == 'V' ? MRZ_DOCUMENT_TYPE_MRVB : MRZ_DOCUMENT_TYPE_TD2) : MRZ_DOCUMENT_TYPE_UNKNOWN));
	if (data.type == MRZ_DOCUMENT_TYPE_UNKNOWN) {
		ULTMRZ_SDK_PRINT_FATAL("Invalid type");
		return false;
	}

	return (__mrz_parser_processXXXX[static_cast<size_t>(data.type)])(lines, data.fields);
}

static bool __mrz_parser_processNames(const std::string& namesField, MrzFieldsVector& fields);

static bool __mrz_parser_processTD1(const std::vector<std::string>& lines, MrzFieldsVector& fields)
{
	ULTMRZ_SDK_ASSERT(
		lines.size() == 3
		&& lines[0].size() == 30
		&& lines[1].size() == 30
		&& lines[2].size() == 30
	);

	static const std::regex __TD1_line0(
		"([A|C|I][A-Z0-9<]{1})" /*1: Doc*/ \
		MRZ_REGEX_AZ_GROUPMATCH(3) /*2: country*/ \
		MRZ_REGEX_ANYCHAR_GROUPMATCH(9) /*3: Document number*/ \
		MRZ_REGEX_09_GROUPMATCH(1) /*4: H */ \
		MRZ_REGEX_ANYCHAR_GROUPMATCH(15) /*5: Optional Data 1*/ \
	);
	static const std::regex __TD1_line1(
		MRZ_REGEX_09_GROUPMATCH(6) /*1: Birth date*/ \
		MRZ_REGEX_09_GROUPMATCH(1) /*2: H*/ \
		MRZ_REGEX_SEX_GROUPMATCH(1) /*3: S*/ \
		MRZ_REGEX_09_GROUPMATCH(6) /*4: Expiry date*/ \
		MRZ_REGEX_09_GROUPMATCH(1) /*5: H*/ \
		MRZ_REGEX_AZ_GROUPMATCH(3) /*6: nationality*/ \
		MRZ_REGEX_ANYCHAR_GROUPMATCH(11) /*7: Optional Data 2*/ \
		MRZ_REGEX_09_GROUPMATCH(1) /*8: FH*/
	);
	static const std::regex __TD1_line2(
		MRZ_REGEX_ANYCHAR_GROUPMATCH(30) /*3:Surname << Given names*/
	);

	// line-0
	try {
		std::smatch matches;
		if (std::regex_search(lines[0], matches, __TD1_line0) && matches.size() == 6) {
			fields.push_back(std::make_pair("doc", matches.str(1)));
			fields.push_back(std::make_pair("country", matches.str(2)));
			fields.push_back(std::make_pair("doc_number", matches.str(3)));
			fields.push_back(std::make_pair("hash", matches.str(4)));
			fields.push_back(std::make_pair("optional_data1", matches.str(5)));
		}
		else {
			ULTMRZ_SDK_PRINT_ERROR("Invalid number of groups: %zu", matches.size());
			return false;
		}
	}
	catch (std::regex_error& e) {
		ULTMRZ_SDK_PRINT_ERROR("regex error: %s", e.what());
		return false;
	}

	// line-1
	try {
		std::smatch matches;
		if (std::regex_search(lines[1], matches, __TD1_line1) && matches.size() == 9) {
			fields.push_back(std::make_pair("birth_date", matches.str(1)));
			fields.push_back(std::make_pair("hash", matches.str(2)));
			fields.push_back(std::make_pair("sex", matches.str(3)));
			fields.push_back(std::make_pair("expiry_date", matches.str(4)));
			fields.push_back(std::make_pair("hash", matches.str(5)));
			fields.push_back(std::make_pair("nationality", matches.str(6)));
			fields.push_back(std::make_pair("optional_data2", matches.str(7)));
			fields.push_back(std::make_pair("final_hash", matches.str(8)));
		}
		else {
			ULTMRZ_SDK_PRINT_ERROR("Invalid number of groups: %zu", matches.size());
			return false;
		}
	}
	catch (std::regex_error& e) {
		ULTMRZ_SDK_PRINT_ERROR("regex error: %s", e.what());
		return false;
	}

	// line-2
	try {
		std::smatch matches;
		if (std::regex_search(lines[2], matches, __TD1_line2) && matches.size() == 2) {
			ULTMRZ_SDK_ASSERT(__mrz_parser_processNames(matches.str(1), fields));
		}
		else {
			ULTMRZ_SDK_PRINT_ERROR("Invalid number of groups: %zu", matches.size());
			return false;
		}
	}
	catch (std::regex_error& e) {
		ULTMRZ_SDK_PRINT_ERROR("regex error: %s", e.what());
		return false;
	}

	return true;
}

static bool __mrz_parser_processTD2(const std::vector<std::string>& lines, MrzFieldsVector& fields)
{
	ULTMRZ_SDK_ASSERT(
		lines.size() == 2
		&& lines[0].size() == 36
		&& lines[1].size() == 36
	);
	static const std::regex __TD2_line0(
		"([A|C|I][A-Z0-9<]{1})" /*1: Doc*/ \
		MRZ_REGEX_AZ_GROUPMATCH(3) /*2: country*/ \
		MRZ_REGEX_ANYCHAR_GROUPMATCH(31) /*3:Surname << Given names*/
	);
	static const std::regex __TD2_line1(
		MRZ_REGEX_ANYCHAR_GROUPMATCH(9) /*1: Document number*/ \
		MRZ_REGEX_09_GROUPMATCH(1) /*2: H*/ \
		MRZ_REGEX_AZ_GROUPMATCH(3) /*3: nationality*/ \
		MRZ_REGEX_09_GROUPMATCH(6) /*4: Date of birth*/ \
		MRZ_REGEX_09_GROUPMATCH(1) /*5: H*/ \
		MRZ_REGEX_SEX_GROUPMATCH(1) /*6: S*/ \
		MRZ_REGEX_09_GROUPMATCH(6) /*7: Date of expiry*/ \
		MRZ_REGEX_09_GROUPMATCH(1) /*8: H*/ \
		MRZ_REGEX_ANYCHAR_GROUPMATCH(7) /*9: Optional Data 1*/ \
		MRZ_REGEX_09_GROUPMATCH(1) /*10: FH*/
	);

	// line-0
	try {
		std::smatch matches;
		if (std::regex_search(lines[0], matches, __TD2_line0) && matches.size() == 4) {
			fields.push_back(std::make_pair("doc", matches.str(1)));
			fields.push_back(std::make_pair("country", matches.str(2)));
			ULTMRZ_SDK_ASSERT(__mrz_parser_processNames(matches.str(3), fields));
		}
		else {
			ULTMRZ_SDK_PRINT_ERROR("Invalid number of groups: %zu", matches.size());
			return false;
		}
	}
	catch (std::regex_error& e) {
		ULTMRZ_SDK_PRINT_ERROR("regex error: %s", e.what());
		return false;
	}

	// line-1
	try {
		std::smatch matches;
		if (std::regex_search(lines[1], matches, __TD2_line1) && matches.size() == 11) {
			fields.push_back(std::make_pair("doc", matches.str(1)));
			fields.push_back(std::make_pair("hash", matches.str(2)));
			fields.push_back(std::make_pair("nationality", matches.str(3)));
			fields.push_back(std::make_pair("birth_date", matches.str(4)));
			fields.push_back(std::make_pair("hash", matches.str(5)));
			fields.push_back(std::make_pair("sex", matches.str(6)));
			fields.push_back(std::make_pair("expiry_date", matches.str(7)));
			fields.push_back(std::make_pair("hash", matches.str(8)));
			fields.push_back(std::make_pair("optional_data1", matches.str(9)));
			fields.push_back(std::make_pair("final_hash", matches.str(10)));			
		}
		else {
			ULTMRZ_SDK_PRINT_ERROR("Invalid number of groups: %zu", matches.size());
			return false;
		}
	}
	catch (std::regex_error& e) {
		ULTMRZ_SDK_PRINT_ERROR("regex error: %s", e.what());
		return false;
	}

	return true;
}

static bool __mrz_parser_processTD3(const std::vector<std::string>& lines, MrzFieldsVector& fields)
{
	ULTMRZ_SDK_ASSERT(
		lines.size() == 2
		&& lines[0].size() == 44
		&& lines[1].size() == 44
	);

	static const std::regex __TD3_line0(
		"(P[A-Z0-9<]{1})" /*1: Doc*/ \
		MRZ_REGEX_AZ_GROUPMATCH(3) /*2: country*/ \
		MRZ_REGEX_ANYCHAR_GROUPMATCH(39) /*3:Surname << Given names*/
	);
	static const std::regex __TD3_line1(
		MRZ_REGEX_ANYCHAR_GROUPMATCH(9) /*1: Document number*/ \
		MRZ_REGEX_09_GROUPMATCH(1) /*2: H*/ \
		MRZ_REGEX_AZ_GROUPMATCH(3) /*3: nationality*/ \
		MRZ_REGEX_09_GROUPMATCH(6) /*4: Birth date*/ \
		MRZ_REGEX_09_GROUPMATCH(1) /*5: H*/ \
		MRZ_REGEX_SEX_GROUPMATCH(1) /*6: S*/ \
		MRZ_REGEX_09_GROUPMATCH(6) /*7: Expiry date*/ \
		MRZ_REGEX_09_GROUPMATCH(1) /*8: H*/ \
		MRZ_REGEX_ANYCHAR_GROUPMATCH(14) /*9: Optional data */ \
		MRZ_REGEX_09_GROUPMATCH(1) /*10: H*/\
		MRZ_REGEX_09_GROUPMATCH(1) /*11: FH*/
	);

	// line-0
	try {
		std::smatch matches;
		if (std::regex_search(lines[0], matches, __TD3_line0) && matches.size() == 4) {
			fields.push_back(std::make_pair("doc", matches.str(1)));
			fields.push_back(std::make_pair("country", matches.str(2)));
			ULTMRZ_SDK_ASSERT(__mrz_parser_processNames(matches.str(3), fields));
		}
		else {
			ULTMRZ_SDK_PRINT_ERROR("Invalid number of groups: %zu", matches.size());
			return false;
		}
	}
	catch (std::regex_error& e) {
		ULTMRZ_SDK_PRINT_ERROR("regex error: %s", e.what());
		return false;
	}

	// line-1
	try {
		std::smatch matches;
		if (std::regex_search(lines[1], matches, __TD3_line1) && matches.size() == 12) {
			fields.push_back(std::make_pair("doc_number", matches.str(1)));
			fields.push_back(std::make_pair("hash", matches.str(2)));
			fields.push_back(std::make_pair("nationality", matches.str(3)));
			fields.push_back(std::make_pair("birth_date", matches.str(4)));
			fields.push_back(std::make_pair("hash", matches.str(5)));
			fields.push_back(std::make_pair("sex", matches.str(6)));
			fields.push_back(std::make_pair("expiry_date", matches.str(7)));
			fields.push_back(std::make_pair("hash", matches.str(8)));
			fields.push_back(std::make_pair("personal_number", matches.str(9)));
			fields.push_back(std::make_pair("hash", matches.str(10)));
			fields.push_back(std::make_pair("final_hash", matches.str(11)));
		}
		else {
			ULTMRZ_SDK_PRINT_ERROR("Invalid number of groups: %zu", matches.size());
			return false;
		}
	}
	catch (std::regex_error& e) {
		ULTMRZ_SDK_PRINT_ERROR("regex error: %s", e.what());
		return false;
	}

	return true;
}

// e.g. "visa usa"
static bool __mrz_parser_processMRVA(const std::vector<std::string>& lines, MrzFieldsVector& fields)
{
	ULTMRZ_SDK_ASSERT(
		lines.size() == 2
		&& lines[0].size() == 44
		&& lines[1].size() == 44
	);

	static const std::regex __MRVA_line0(
		"(V[A-Z0-9<]{1})" /*1: Doc*/ \
		MRZ_REGEX_AZ_GROUPMATCH(3) /*2: country*/ \
		MRZ_REGEX_ANYCHAR_GROUPMATCH(39) /*3:Surname << Given names*/
	);
	static const std::regex __MRVA_line1(
		MRZ_REGEX_ANYCHAR_GROUPMATCH(9) /*1: Document number*/ \
		MRZ_REGEX_09_GROUPMATCH(1) /*2: H*/ \
		MRZ_REGEX_AZ_GROUPMATCH(3) /*3: Nationality*/ \
		MRZ_REGEX_09_GROUPMATCH(6) /*4: Birth date*/ \
		MRZ_REGEX_09_GROUPMATCH(1) /*5: H*/ \
		MRZ_REGEX_SEX_GROUPMATCH(1) /*6: S*/ \
		MRZ_REGEX_09_GROUPMATCH(6) /*7: Expiry date*/ \
		MRZ_REGEX_09_GROUPMATCH(1) /*8: H*/ \
		MRZ_REGEX_ANYCHAR_GROUPMATCH(16) /*9: Optional data*/
	);

	// line-0
	try {
		std::smatch matches;
		if (std::regex_search(lines[0], matches, __MRVA_line0) && matches.size() == 4) {
			fields.push_back(std::make_pair("doc", matches.str(1)));
			fields.push_back(std::make_pair("country", matches.str(2)));
			ULTMRZ_SDK_ASSERT(__mrz_parser_processNames(matches.str(3), fields));
		}
		else {
			ULTMRZ_SDK_PRINT_ERROR("Invalid number of groups: %zu", matches.size());
			return false;
		}
	}
	catch (std::regex_error& e) {
		ULTMRZ_SDK_PRINT_ERROR("regex error: %s", e.what());
		return false;
	}

	// line-1
	try {
		std::smatch matches;
		if (std::regex_search(lines[1], matches, __MRVA_line1) && matches.size() == 10) {
			fields.push_back(std::make_pair("doc_number", matches.str(1)));
			fields.push_back(std::make_pair("hash", matches.str(2)));
			fields.push_back(std::make_pair("nationality", matches.str(3)));
			fields.push_back(std::make_pair("birth_date", matches.str(4)));
			fields.push_back(std::make_pair("hash", matches.str(5)));
			fields.push_back(std::make_pair("sex", matches.str(6)));
			fields.push_back(std::make_pair("expiry_date", matches.str(7)));
			fields.push_back(std::make_pair("hash", matches.str(8)));
			fields.push_back(std::make_pair("optional_data", matches.str(9)));
		}
		else {
			ULTMRZ_SDK_PRINT_ERROR("Invalid number of groups: %zu", matches.size());
			return false;
		}
	}
	catch (std::regex_error& e) {
		ULTMRZ_SDK_PRINT_ERROR("regex error: %s", e.what());
		return false;
	}

	return true;
}

// e.g. "visa schengen"
static bool __mrz_parser_processMRVB(const std::vector<std::string>& lines, MrzFieldsVector& fields)
{
	ULTMRZ_SDK_ASSERT(
		lines.size() == 2
		&& lines[0].size() == 36
		&& lines[1].size() == 36
	);

	static const std::regex __MRVB_line0(
		"(V[A-Z0-9<]{1})" /*1: Doc*/ \
		MRZ_REGEX_AZ_GROUPMATCH(3) /*2: country*/ \
		MRZ_REGEX_ANYCHAR_GROUPMATCH(31) /*3:Surname << Given names*/
	);
	static const std::regex __MRVB_line1(
		MRZ_REGEX_ANYCHAR_GROUPMATCH(9) /*1: Document number*/ \
		MRZ_REGEX_09_GROUPMATCH(1) /*2: H*/ \
		MRZ_REGEX_AZ_GROUPMATCH(3) /*3: Nationality*/ \
		MRZ_REGEX_09_GROUPMATCH(6) /*4: Birth date*/ \
		MRZ_REGEX_09_GROUPMATCH(1) /*5: H*/ \
		MRZ_REGEX_SEX_GROUPMATCH(1) /*6: S*/ \
		MRZ_REGEX_09_GROUPMATCH(6) /*7: Expiry date*/ \
		MRZ_REGEX_09_GROUPMATCH(1) /*8: H*/ \
		MRZ_REGEX_ANYCHAR_GROUPMATCH(8) /*9: Optional data*/
	);

	// line-0
	try {
		std::smatch matches;
		if (std::regex_search(lines[0], matches, __MRVB_line0) && matches.size() == 4) {
			fields.push_back(std::make_pair("doc", matches.str(1)));
			fields.push_back(std::make_pair("country", matches.str(2)));
			ULTMRZ_SDK_ASSERT(__mrz_parser_processNames(matches.str(3), fields));
		}
		else {
			ULTMRZ_SDK_PRINT_ERROR("Invalid number of groups: %zu", matches.size());
			return false;
		}
	}
	catch (std::regex_error& e) {
		ULTMRZ_SDK_PRINT_ERROR("regex error: %s", e.what());
		return false;
	}

	// line-1
	try {
		std::smatch matches;
		if (std::regex_search(lines[1], matches, __MRVB_line1) && matches.size() == 10) {
			fields.push_back(std::make_pair("doc_number", matches.str(1)));
			fields.push_back(std::make_pair("hash", matches.str(2)));
			fields.push_back(std::make_pair("nationality", matches.str(3)));
			fields.push_back(std::make_pair("birth_date", matches.str(4)));
			fields.push_back(std::make_pair("hash", matches.str(5)));
			fields.push_back(std::make_pair("sex", matches.str(6)));
			fields.push_back(std::make_pair("expiry_date", matches.str(7)));
			fields.push_back(std::make_pair("hash", matches.str(8)));
			fields.push_back(std::make_pair("optional_data", matches.str(9)));
		}
		else {
			ULTMRZ_SDK_PRINT_ERROR("Invalid number of groups: %zu", matches.size());
			return false;
		}
	}
	catch (std::regex_error& e) {
		ULTMRZ_SDK_PRINT_ERROR("regex error: %s", e.what());
		return false;
	}

	return true;
}

static bool __mrz_parser_processNames(const std::string& namesField, MrzFieldsVector& fields)
{
	if (namesField.empty()) {
		return true;
	}

	std::string namesField_ = namesField;
	std::replace(namesField_.begin(), namesField_.end(), '<', ' ');
	std::istringstream iss(namesField_);
	std::vector<std::string> names(std::istream_iterator<std::string>{iss},
		std::istream_iterator<std::string>());
	if (!names.empty()) {
		fields.push_back(std::make_pair("surname", names[0]));
		for (size_t j = 1; j < names.size(); ++j) {
			fields.push_back(std::make_pair("given_name_" + std::to_string(j - 1), names[j]));
		}
	}

	return true;
}

#endif /* _ULTIMATE_MRZ_SDK_SAMPLES_MRZ_PARSER_H_ */
