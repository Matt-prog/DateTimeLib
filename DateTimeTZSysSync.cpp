#include "DateTimeTZSysSync.h"


DateTimeSysSync DateTimeTZSysSync::getSysSyncUTC() const {
	DateTimeSysSync ret(*this);
	int64_t tzOffset = (int64_t)getTimeZoneOffset();
	int64_t dstOffset = (int64_t)getCurrentDSTOffset();
	ret -= (tzOffset + dstOffset);
	return ret;
}

int64_t DateTimeTZSysSync::recalcDST() {
	DT_SYNC_TYPE currentTime = DT_SYNC_FUNC();
	int64_t rawTime = DateTimeBase::getRawTimeNoSync() + (int64_t)((currentTime - syncTime)) * DT_SYNC_RESOLUTION;
	if (DateTimeTZBase<DateTimeTZSysSync>::adj.noDST()) return rawTime;

	bool overTransition = (nextDSTTransitionDT <= rawTime); //Checking if synced time is over next DST transition time
	if (overTransition) {
		//We are now over next DST transition, so we need to calculate new next DST transition
		int64_t rawTimeNoDST = rawTime;
		bool oldDST = DateTimeTZBase<DateTimeTZSysSync>::adj.isDST();
		if (oldDST) {
			rawTimeNoDST -= (int64_t)DateTimeTZBase<DateTimeTZSysSync>::getDSTOffset(); //Getting time without DST
		}
		bool newIsDST;
		nextDSTTransitionDT = DateTimeTZBase<DateTimeTZSysSync>::adj.getNextTransitionDate(rawTimeNoDST, newIsDST);
		newIsDST = !newIsDST;
		if (oldDST != newIsDST) {
			int64_t offset;
			if (newIsDST) {
				//Applying DST offset
				offset = (int64_t)getDSTOffset();
			}
			else {
				//Removing DST offset
				offset = -(int64_t)getDSTOffset();
			}
			DateTimeTZBase<DateTimeTZSysSync>::addRawTimeNoTD(offset);
			adj.setDST(newIsDST);
			return rawTime + offset;
		}
	}
	return rawTime;
}

DateTime_DST_tuple DateTimeTZSysSync::getDateTimeEnh() const {
	DT_SYNC_TYPE currentTime = DT_SYNC_FUNC();
	int64_t raw = DateTimeBase::getRawTimeNoSync() + (int64_t)((currentTime - syncTime)) * DT_SYNC_RESOLUTION;
	bool isDST_ = DateTimeTZBase<DateTimeTZSysSync>::adj.isDST();
	bool overTransition = (nextDSTTransitionDT <= raw); //Checking if synced time is over next DST transition time
	if (overTransition) {
		if (isDST_) {
			//Was DST, but now is not
			raw -= (int64_t)getDSTOffset();
		}
		else {
			//Was not DST, but now is
			raw += (int64_t)getDSTOffset();
		}
		isDST_ = !isDST_;
	}
	return DateTime_DST_tuple(raw, isDST_);
}

int64_t DateTimeTZSysSync::getRawTimeSync() const {
	DT_SYNC_TYPE currentTime = DT_SYNC_FUNC();
	int64_t raw = DateTimeBase::getRawTimeNoSync() + (int64_t)((currentTime - syncTime)) * DT_SYNC_RESOLUTION;
	bool overTransition = (nextDSTTransitionDT <= raw); //Checking if synced time is over next DST transition time
	if (overTransition) {
		if (DateTimeTZBase<DateTimeTZSysSync>::adj.isDST()) {
			//Was DST, but now is not
			return raw - (int64_t)getDSTOffset();
		}
		else {
			//Was not DST, but now is
			return raw + (int64_t)getDSTOffset();
		}
	}
	else {
		//No change
		return raw;
	}
}

void DateTimeTZSysSync::preSetSync() {
	syncTime = DT_SYNC_FUNC();
}

int64_t DateTimeTZSysSync::getAndPreSetSync() {
	DT_SYNC_TYPE currentTime = DT_SYNC_FUNC();
	int64_t raw = DateTimeBase::getRawTimeNoSync() + (int64_t)((currentTime - syncTime)) * DT_SYNC_RESOLUTION;
	bool overTransition = (nextDSTTransitionDT <= raw); //Checking if synced time is over next DST transition time
	if (overTransition) {
		if (DateTimeTZBase<DateTimeTZSysSync>::adj.isDST()) {
			//Was DST, but now is not
			raw -= (int64_t)getDSTOffset();
		}
		else {
			//Was not DST, but now is
			raw += (int64_t)getDSTOffset();
		}
	}
	syncTime = currentTime;
	return raw;
}

bool DateTimeTZSysSync::isSyncDST() const {
	bool negNeeded = isOverNextTrans(); //Checking if synced time is over next DST transition time
	return negNeeded != DateTimeTZBase<DateTimeTZSysSync>::adj.isDST();
}

void DateTimeTZSysSync::calcNextTransOnSet(int64_t newRaw, bool is_DST) {
	if (!DateTimeTZBase<DateTimeTZSysSync>::adj.noDST()) {
		//We are now over next DST transition, so we need to calculate new next DST transition
		if (is_DST) {
			newRaw -= (int64_t)DateTimeTZBase<DateTimeTZSysSync>::getDSTOffset(); //Getting time without DST
		}
		bool newIsDST;
		nextDSTTransitionDT = DateTimeTZBase<DateTimeTZSysSync>::adj.getNextTransitionDate(newRaw, newIsDST);
	}
	else {
		nextDSTTransitionDT = DateTime::MaxValue;
	}
}

/*void DateTimeTZSysSync::ctorBeforeCtor() {
	nextDSTTransitionDT = DateTime::MaxValue;
}*/

bool DateTimeTZSysSync::isOverNextTrans() const {
	DT_SYNC_TYPE currentTime = DT_SYNC_FUNC();
	int64_t raw = DateTimeBase::getRawTimeNoSync() + (int64_t)((currentTime - syncTime)) * DT_SYNC_RESOLUTION;
	return nextDSTTransitionDT <= raw;
}

int64_t DateTimeTZSysSync::unappliedOffset(DT_SYNC_TYPE currentTime) const {
	//DT_SYNC_TYPE currentTime = DT_SYNC_FUNC();
	int64_t raw = DateTimeBase::getRawTimeNoSync() + (int64_t)((currentTime - syncTime)) * DT_SYNC_RESOLUTION;
	bool isDST_ = DateTimeTZBase<DateTimeTZSysSync>::adj.isDST();
	bool overTransition = (nextDSTTransitionDT <= raw); //Checking if synced time is over next DST transition time
	if (overTransition) {
		if (isDST_) {
			//Was DST, but now is not
			return -(int64_t)getDSTOffset();
		}
		else {
			//Was not DST, but now is
			return (int64_t)getDSTOffset();
		}
	}
	return 0LL;
}

void DateTimeTZSysSync::setSyncedTime(int64_t baseRawValue, DT_SYNC_TYPE syncTime_new, bool DST_app) {
	syncTime = syncTime_new; //Copying sync time

	if (DateTimeTZBase<DateTimeTZSysSync>::adj.noDST()) {
		DateTimeTZBase<DateTimeTZSysSync>::setRawTimeNoTD(baseRawValue);
		adj.setDST(false);
		nextDSTTransitionDT = DateTime::MaxValue;
		return;
	}

	if (DST_app) {
		baseRawValue -= (int64_t)DateTimeTZBase<DateTimeTZSysSync>::getDSTOffset(); //Getting time without DST
	}
	DT_SYNC_TYPE currentTime = DT_SYNC_FUNC();
	int64_t currRawTime = baseRawValue + (int64_t)((currentTime - syncTime)) * DT_SYNC_RESOLUTION;

	bool newIsDST;
	nextDSTTransitionDT = DateTimeTZBase<DateTimeTZSysSync>::adj.getNextTransitionDate(currRawTime, newIsDST);
	newIsDST = !newIsDST; //getting DST of region before next transition

	if (newIsDST) {
		baseRawValue += (int64_t)DateTimeTZBase<DateTimeTZSysSync>::getDSTOffset(); //Applying DST offset
	}

	DateTimeTZBase<DateTimeTZSysSync>::setRawTimeNoTD(baseRawValue);
	adj.setDST(newIsDST);
}







//OS specific functions
#if DT_UNDER_OS > 0

#include <ctime>
#include <iomanip>
#include <sstream>

#ifdef DT_GET_SYS_DST_FUNC
const DSTAdjustment DateTimeTZSysSync::systemDSTAdjustment = DateTimeTZSysSync::getSystemDSTAdjustment();
#endif // DT_GET_SYS_DST_FUNC

#ifdef DT_GET_SYS_TZ_FUNC

#ifdef DT_GET_SYS_DST_FUNC
const TimeZone DateTimeTZSysSync::systemTZ = DateTimeTZSysSync::getSystemTZ(); //Getting TZ offset, without DST offset
#else
const TimeZone DateTimeTZSysSync::systemTZ = DateTimeTZSysSync::getSystemTZ(true); //Getting TZ offset, which includes DST offset
#endif // DT_GET_SYS_DST_FUNC

#endif // DT_GET_SYS_TZ_FUNC

/**
* @brief Gets UTC offset at specified time.
* @param when Time, which UTC offset will be get.
* @param[out] isDST True if DST is applyied to offset.
* @return Returns UTC offset in minutes.
*/
int getUTCOffset(time_t when, bool& isDST) {

	struct tm timeinfo;
#if _MSC_VER && !__INTEL_COMPILER
	//Specific code for MVSC
	localtime_s(&timeinfo, &when);
#else
	timeinfo = *localtime(&when);
#endif

	isDST = timeinfo.tm_isdst;

	std::ostringstream os;
	os << std::put_time(&timeinfo, "%z");
	std::string s = os.str();
	// s is in ISO 8601 format: "±HHMM"
	int h = std::stoi(s.substr(0, 3), nullptr, 10);
	int m = std::stoi(s[0] + s.substr(3), nullptr, 10);
	if (s.length() > 0 && s[0] == '-') {
		//Negative offset
		return h * 60 - m;
	}
	else {
		//Positive offset
		return h * 60 + m;
	}
}

/**
* @brief Gets system time zone offset (without DST)
* and DST offset.
* @param[out] TZ_mins Time zone offset without DST in minutes.
* @param[out] DST_mins DST offset in minutes. If system does not use DST, it will be set to 0.
*/
void getSysTZandDST(int& TZ_mins, int& DST_mins) {
	int changeCnt = 0;

	bool DST_old = false;
	int tz_old = 0;
	//Checking 5 dates and reading it's UTC offset
	for (long long i = 0; i <= 86400LL * 365; i += 86400LL * 91) {
		bool DST_new;
		int tz_new = getUTCOffset(i, DST_new);
		if (DST_old != DST_new || tz_old != tz_new) {
			changeCnt++;
			if (changeCnt == 2) {
				if (DST_old != DST_new) {
					//DST flag works as expected
					if (DST_new) {
						DST_mins = tz_new - tz_old;
						TZ_mins = tz_old;
					}
					else {
						DST_mins = tz_old - tz_new;
						TZ_mins = tz_new;
					}
				}
				else {
					//DST flag does not works, but UTC offset has changed.
					//We are assumimg, that DST offset is positive
					if (tz_new > tz_old) {
						DST_mins = tz_new - tz_old;
						TZ_mins = tz_old;
					}
					else {
						DST_mins = tz_old - tz_new;
						TZ_mins = tz_new;
					}
				}
				return;
			}

			DST_old = DST_new;
			tz_old = tz_new;
		}
	}
	//No DST on current system time
	TZ_mins = tz_old;
	DST_mins = 0;
}

/**
* @brief Gets system time zone.
* @param includingDST True if time zone has to include DST offset.
* @return Returns system time zone.
*/
TimeZone getSysTZ(bool includingDST) {
	int TZ_offset, DST_offset;
	getSysTZandDST(TZ_offset, DST_offset);

	if (includingDST) {
		TimeZones tzs = (TZ_offset + DST_offset) / 15;
		return TimeZone(tzs);
	}
	else {
		TimeZones tzs = (TZ_offset) / 15;
		return TimeZone(tzs);
	}
}

#endif // DT_UNDER_OS > 0








//Windows specific functions
#if defined(_WIN64) || defined(_WIN32)
#include <Windows.h>
#include <timezoneapi.h> //Time zone api for windows

/**
* @brief Gets system DSTAdjustment on Windows.
* @return Returns DSTAdjustment.
*/
DSTAdjustment getSysDSTAdjustmentWindows() {
	DYNAMIC_TIME_ZONE_INFORMATION timeZoneInformation;
	GetDynamicTimeZoneInformation(&timeZoneInformation);

	//
	// SYSTEMTIME - 
	//
	// If the time zone does not support daylight saving time or if the caller needs
	// to disable daylight saving time, the wMonth member in the SYSTEMTIME structure
	// must be zero. If this date is specified, the DaylightDate value in the 
	// TIME_ZONE_INFORMATION structure must also be specified. Otherwise, the system 
	// assumes the time zone data is invalid and no changes will be applied.
	//
	bool supportsDst = (timeZoneInformation.StandardDate.wMonth != 0);
	if (!supportsDst) {
		return DSTAdjustment::NoDST;
	}

	//
	// SYSTEMTIME -
	//
	// * FixedDateRule -
	//   If the Year member is not zero, the transition date is absolute; it will only occur one time
	//
	// * FloatingDateRule -
	//   To select the correct day in the month, set the Year member to zero, the Hour and Minute 
	//   members to the transition time, the DayOfWeek member to the appropriate weekday, and the
	//   Day member to indicate the occurence of the day of the week within the month (first through fifth).
	//
	//   Using this notation, specify the 2:00a.m. on the first Sunday in April as follows: 
	//   Hour      = 2, 
	//   Month     = 4,
	//   DayOfWeek = 0,
	//   Day       = 1.
	//
	//   Specify 2:00a.m. on the last Thursday in October as follows:
	//   Hour      = 2,
	//   Month     = 10,
	//   DayOfWeek = 4,
	//   Day       = 5.
	//
	//
	// 
	// read the "daylightTransitionStart"
	//
	DSTTransitionRule startRule;
	if (timeZoneInformation.DaylightDate.wYear == 0) {
		//Floating rule
		startRule = DSTTransitionRule((int8_t)timeZoneInformation.DaylightDate.wHour,
			(int8_t)timeZoneInformation.DaylightDate.wMonth,
			((DayOfWeek)(timeZoneInformation.DaylightDate.wDayOfWeek + 1)),
			(WeekOfMonth)(timeZoneInformation.DaylightDate.wDay - 1),
			0);
	}
	else {
		//Fixed rule
		startRule = DSTTransitionRule((int8_t)timeZoneInformation.DaylightDate.wHour,
			(int8_t)timeZoneInformation.DaylightDate.wMonth,
			(int8_t)timeZoneInformation.DaylightDate.wDay,
			0);
	}

	DSTTransitionRule endRule;
	if (timeZoneInformation.StandardDate.wYear == 0) {
		//Floating rule
		endRule = DSTTransitionRule((int8_t)timeZoneInformation.StandardDate.wHour,
			(int8_t)timeZoneInformation.StandardDate.wMonth,
			((DayOfWeek)(timeZoneInformation.StandardDate.wDayOfWeek + 1)),
			(WeekOfMonth)(timeZoneInformation.StandardDate.wDay - 1),
			0);
	}
	else {
		//Fixed rule
		endRule = DSTTransitionRule((int8_t)timeZoneInformation.StandardDate.wHour,
			(int8_t)timeZoneInformation.StandardDate.wMonth,
			(int8_t)timeZoneInformation.StandardDate.wDay,
			0);
	}

	int DST_offset = -timeZoneInformation.DaylightBias;
	int DST_offset_h = DST_offset / 60;
	int DST_offset_m = DST_offset % 60;

	return DSTAdjustment(startRule, endRule, DST_offset_h, DST_offset_m, false);
}

#endif // defined(_WIN64) || defined(_WIN32)