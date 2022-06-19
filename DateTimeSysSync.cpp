#include "DateTimeSysSync.h"

int64_t DateTimeSysSync::getRawTimeSync() const {
	DT_SYNC_TYPE currentTime = DT_SYNC_FUNC();
	return DateTimeBase::getRawTimeNoSync() + (int64_t)((currentTime - syncTime)) * DT_SYNC_RESOLUTION;
}

void DateTimeSysSync::preSetSync() {
	syncTime = DT_SYNC_FUNC();
}

int64_t DateTimeSysSync::getAndPreSetSync() {
	DT_SYNC_TYPE currentTime = DT_SYNC_FUNC();
	int64_t retVal = DateTimeBase::getRawTimeNoSync() + (int64_t)((currentTime - syncTime)) * DT_SYNC_RESOLUTION;
	syncTime = currentTime;
	return retVal;
}

#if DT_UNDER_OS > 0
DateTimeSysSync DateTimeSysSync::getSysTimeUTC() {
	DateTimeSysSync dt;

	//Getting local time
	auto now = std::chrono::system_clock::now();
	auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
	auto fraction = now - seconds;
	time_t cnow = std::chrono::system_clock::to_time_t(now);
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(fraction).count();
	auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(fraction).count() - milliseconds * MILLISECOND;

	//Converting to date_time_s
	struct tm timeinfo;
#if _MSC_VER && !__INTEL_COMPILER
	//Specific code for MVSC
	gmtime_s(&timeinfo, &cnow);

#else
	timeinfo = *gmtime(&cnow);
#endif

	date_time_s dts = date_time_s(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, (uint16_t)milliseconds, (uint16_t)microseconds);

	//Setting value without synchronizing
	dt.setRawTime(dtlib::dateTimeToRaw(dts));
	return dt;
}

DateTimeSysSync DateTimeSysSync::getSysTime(bool& isDST) {
	DateTimeSysSync dt;

	//Getting local time
	auto now = std::chrono::system_clock::now();
	auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
	auto fraction = now - seconds;
	time_t cnow = std::chrono::system_clock::to_time_t(now);
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(fraction).count();
	auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(fraction).count() - milliseconds * MILLISECOND;

	//Converting to date_time_s
	struct tm timeinfo;
#if _MSC_VER && !__INTEL_COMPILER
	//Specific code for MVSC
	localtime_s(&timeinfo, &cnow);
	
#else
	timeinfo = *localtime(&cnow);
#endif

	isDST = timeinfo.tm_isdst;
	date_time_s dts = date_time_s(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, (uint16_t)milliseconds, (uint16_t)microseconds);
	
	//Setting value without synchronizing
	dt.setRawTime(dtlib::dateTimeToRaw(dts));
	return dt;
}

#elif defined(ESP32) || defined(ESP8266)

DateTimeSysSync DateTimeSysSync::getSysTimeUTC() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return DateTimeSysSync::from_timeval(tv);
}

DateTimeSysSync DateTimeSysSync::getSysTime(bool& isDST) {
	DateTimeSysSync ret = getSysTimeUTC();

	TimeZoneInfo tzinfo;
	const char* tz_env = getenv("TZ");
	if (tz_env != NULL) {
		int pos;
		tzinfo = TimeZoneInfo::fromPOSIX(tz_env, strlen(tz_env), pos);
	}

	ret += tzinfo.timeZone.getTimeZoneOffset();
	isDST = tzinfo.DST.checkDSTRegion(ret);
	if (isDST) {
		ret += tzinfo.DST.getDSTOffset();
	}
	return ret;
}

#endif // !DT_UNDER_OS > 0