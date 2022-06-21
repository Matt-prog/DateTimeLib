#ifndef TZFILE_PARSER_H_
#define TZFILE_PARSER_H_

#include "DateTimeHelpers.h"

#if DT_UNDER_OS > 0
#include <iostream>
#include <fstream>

#ifndef TZDIR
#define TZDIR	"/usr/share/zoneinfo" // Time zone object file directory, but this may not be true always
#endif // !TZDIR 

#ifndef TZDEFAULT
#define TZDEFAULT	"/etc/localtime"
#endif // !TZDEFAULT

#ifndef TZDEFRULES
#define TZDEFRULES	"posixrules"
#endif // !TZDEFRULES

/**
* @struct tzif_header
* @brief Structure, that represents time zone file header.
*/
struct tzif_header {
	/**
	* @brief Default constructor.
	*/
	tzif_header() {
		valid = false;
	}

	/**
	* @brief Construct header from input stream( file).
	* @param istr Stream to parse header from. Stream (file) has to be opened and is not closed after operation is done.
	* @note If conversion failed, valid field will be false.
	*/
	tzif_header(std::istream& istr);

	size_t getSizeOfData(int headerIndex = 0);

	/**
	* @brief This has to contain "TZif" string, which is **NOT** null terminated.
	*/
	char magic[4];

	/**
	* @bief Version of time zone file:
	* + version 1 = '\0'
	* + version 2 = '2'
	* + version 3 = '3'
	*/
	char version;

	/**
	* @brief Reserved for future use.
	*/
	char reserved[15];

	uint32_t tzh_ttisutcnt;  /**< The number of UT/local indicators stored in the file. (UT is Universal Time.) */
	uint32_t tzh_ttisstdcnt; /**< The number of standard/wall indicators stored in the file. */
	uint32_t tzh_leapcnt;    /**< The number of leap seconds for which data entries are stored in the file. */
	uint32_t tzh_timecnt;    /**< The number of transition times for which data entries are stored in the file. */
	uint32_t tzh_typecnt;    /**< The number of local time types for which data entries are stored in the file (must not be zero). */
	uint32_t tzh_charcnt;    /**< The number of bytes of time zone abbreviation strings stored in the file. */

	/**
	* @brief True if structure contains valid data.
	*/
	bool valid;
};

std::istream& operator >>(std::istream& is, tzif_header& bignum);

struct ttinfo {
	int32_t        tt_utoff;
	unsigned char  tt_isdst;
	unsigned char  tt_desigidx;
};

std::string tryGetPOSIXFrom_tzfile(const char* path);

#endif // DT_UNDER_OS > 0

#endif // !TZFILE_PARSER_H_
