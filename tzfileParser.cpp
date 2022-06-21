#include "tzfileParser.h"


#if DT_UNDER_OS > 0

static bool isLittleEndian()
{
	static const uint16_t m_endianCheck(0x00ff);
	return (*((const uint8_t*)&m_endianCheck) != 0x0);
}

template <typename T, typename = dtlib::enable_if<sizeof(T) == 4>>
static void swapEndianness(T& val) {
	T tmp = val;
	uint8_t* in_ptr = reinterpret_cast<uint8_t*>(&tmp);
	uint8_t* out_ptr = reinterpret_cast<uint8_t*>(&val);
	out_ptr[0] = in_ptr[3];
	out_ptr[1] = in_ptr[2];
	out_ptr[2] = in_ptr[1];
	out_ptr[3] = in_ptr[0];
}

tzif_header::tzif_header(std::istream& istr) {
	if (istr) {
		istr.read(reinterpret_cast<char*>(this), 44);
		if (istr) {
			valid = magic[0] == 'T' && magic[1] == 'Z' && magic[2] == 'i' && magic[3] == 'f';
			if (isLittleEndian()) {
				swapEndianness(tzh_ttisutcnt);
				swapEndianness(tzh_ttisstdcnt);
				swapEndianness(tzh_leapcnt);
				swapEndianness(tzh_timecnt);
				swapEndianness(tzh_typecnt);
				swapEndianness(tzh_charcnt);
			}
			return;
		}
	}
	valid = false;
}

size_t tzif_header::getSizeOfData(int headerIndex) {
	/*
	* +---------------------------------------------------------+
	* |  transition times          (timecnt x TIME_SIZE)        |
	* +---------------------------------------------------------+
	* |  transition types          (timecnt)                    |
	* +---------------------------------------------------------+
	* |  local time type records   (typecnt x 6)                |
	* +---------------------------------------------------------+
	* |  time zone designations    (charcnt)                    |
	* +---------------------------------------------------------+
	* |  leap-second records       (leapcnt x (TIME_SIZE + 4))  |
	* +---------------------------------------------------------+
	* |  standard/wall indicators  (isstdcnt)                   |
	* +---------------------------------------------------------+
	* |  UT/local indicators       (isutcnt)                    |
	* +---------------------------------------------------------+
	*/
	if (headerIndex == 1 && version >= '2') {
		/* For version-2-format timezone files, the above header and data
		* are followed by a second header and data, identical in format
		* except that eight bytes are used for each transition time or leap
		* second time. (Leap second counts remain four bytes.)
		*/
		return tzh_timecnt * 8 + tzh_timecnt + tzh_typecnt * 6 + tzh_charcnt + tzh_leapcnt * 12 + tzh_ttisstdcnt + tzh_ttisutcnt;
	}
	else {
		return tzh_timecnt * 4 + tzh_timecnt + tzh_typecnt * 6 + tzh_charcnt + tzh_leapcnt * 8 + tzh_ttisstdcnt + tzh_ttisutcnt;
	}
}

std::istream& operator >>(std::istream& is, tzif_header& bignum)
{
	bignum = tzif_header(is);
	return is;
}


std::string tryGetPOSIXFrom_tzfile(const char* path) {
	std::string posixTZ = "";
	
	// Read from the text file
	std::ifstream tzfile(path, std::ios::binary);

	if (tzfile) {
		tzif_header header;
		tzfile >> header;

		if (header.valid) {
			size_t s1 = header.getSizeOfData(0);

			if (header.version == 0) {
				//Version 1 does not have any POSIX time zone
			}
			else {
				tzfile.seekg(s1, tzfile.cur); //Skip data

				if (tzfile) {
					//Parse seconds header
					tzif_header header2;
					tzfile >> header2;
					if (header2.valid) {
						size_t s2 = header2.getSizeOfData(1);

						tzfile.seekg(s2 + 1, std::ios_base::cur); //Skip data
						//+1 to skip new line

						std::getline(tzfile, posixTZ);
					}
				}
			}
		}

		// Close the file
		tzfile.close();
	}

	return posixTZ;
}


#endif // DT_UNDER_OS > 0