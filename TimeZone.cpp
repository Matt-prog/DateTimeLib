//#include "DateTimeBase.h"
#include "TimeZone.h"

#if defined(ARDUINO)
#if defined(ESP8266) || defined(ESP32)
#include <climits>
#else
#include <limits.h>
#endif // defined(ESP8266) || defined(ESP32)

#else
#include <climits>

#if DT_UNDER_OS == DT_WIN //Windows specific code
#include <Windows.h>
#include <timezoneapi.h> //Time zone api for windows
#endif // DT_UNDER_OS == DT_WIN

#endif // defined(ARDUINO)


#if DT_UNDER_OS == DT_LINUX ||  DT_UNDER_OS == DT_MAC
//TODO those functions are HACK for Linux and Mac time zone and DST offset - there are used temporarily,
//     remove them adter proper implementation for reading system TimeZone is done

#include <ctime>
#include <iomanip>
#include <sstream>

/**
* @brief Gets UTC offset at specified time.
* @param when Time, which UTC offset will be get.
* @param[out] isDST True if DST is applyied to offset.
* @return Returns UTC offset in minutes.
*/
static int getUTCOffset(time_t when, bool& isDST) {

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
* @brief Gets system time zone offset and DST offset.
* and DST offset.
* @param[out] TZ_mins Time zone offset without DST in minutes.
* @param[out] DST_mins DST offset in minutes. If system does not use DST, it will be set to 0.
*/
static void getSysTZandDST(int& TZ_mins, int& DST_mins) {
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
static TimeZone getSysTZ(bool includingDST) {
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
#endif // DT_UNDER_OS == DT_LINUX



TimeZone TimeZone::getSystemTZ() {
#if DT_UNDER_OS == DT_WIN       //Windows specific code
    DYNAMIC_TIME_ZONE_INFORMATION timeZoneInformation;
    GetDynamicTimeZoneInformation(&timeZoneInformation);
    return TimeZone::fromTotalMinutesOffset(-timeZoneInformation.Bias);

#elif DT_UNDER_OS == DT_LINUX   //Linux specific code
    //TODO implement propertly
    bool isDST;
    getUTCOffset(time(NULL), isDST);
    return getSysTZ(isDST);
#elif DT_UNDER_OS == DT_MAC     //Mac OS specific code
    //TODO implement propertly
    bool isDST;
    getUTCOffset(time(NULL), isDST);
    return getSysTZ(isDST);
#elif defined(ESP32) || defined(ESP8266)    //ESP8266 and ESP32 specific code
    const char* tz_env = getenv("TZ");
    if (tz_env != NULL) {
        int pos;
        TimeZoneInfo result = TimeZoneInfo::fromPOSIX(tz_env, strlen(tz_env), pos);
        return result.timeZone;
    }
    else {
        return TimeZone();
    }
#else   //Arduino specific code
    return TimeZone();
#endif
}

#if defined(ESP32) || defined(ESP8266)
void TimeZone::setSystemTZ(TimeZone tz, DSTAdjustment dst) {
    TimeZoneInfo tzinfo = TimeZoneInfo::getSystemTZInfo();
    tzinfo.timeZone = tz;
    tzinfo.DST = dst;
    TimeZoneInfo::setSystemTZInfo(tzinfo);
}

void TimeZone::setSystemTZ(TimeZone tz) {
    TimeZoneInfo tzinfo = TimeZoneInfo::getSystemTZInfo();
    tzinfo.timeZone = tz;
    TimeZoneInfo::setSystemTZInfo(tzinfo);
}
#endif // defined(ESP32) || defined(ESP8266)

DSTAdjustment DSTAdjustment::getSystemDST() {
#if DT_UNDER_OS == DT_WIN       //Windows specific code
    //For more info see TimeZoneInfo::getSystemTZInfo()
    DYNAMIC_TIME_ZONE_INFORMATION timeZoneInformation;
    GetDynamicTimeZoneInformation(&timeZoneInformation);
    bool supportsDst = (timeZoneInformation.StandardDate.wMonth != 0);
    if (supportsDst) {
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
    return DSTAdjustment();

#elif DT_UNDER_OS == DT_LINUX   //Linux specific code
    return DSTAdjustment(); //TODO implement
#elif DT_UNDER_OS == DT_MAC     //Mac OS specific code
    return DSTAdjustment(); //TODO implement
#elif defined(ESP32) || defined(ESP8266)    //ESP8266 and ESP32 specific code
    const char* tz_env = getenv("TZ");
    if (tz_env != NULL) {
        int pos;
        TimeZoneInfo result = TimeZoneInfo::fromPOSIX(tz_env, strlen(tz_env), pos);
        return result.DST;
    }
    else {
        return DSTAdjustment();
    }
#else   //Arduino specific code
    return DSTAdjustment();
#endif
}


#if defined(ESP32) || defined(ESP8266)
void DSTAdjustment::setSystemDST(DSTAdjustment dst) {
    TimeZoneInfo tzinfo = TimeZoneInfo::getSystemTZInfo();
    tzinfo.DST = dst;
    TimeZoneInfo::setSystemTZInfo(tzinfo);
}
#endif // defined(ESP32) || defined(ESP8266)



void DSTTransitionRule::setFixed(hour_t transitionHour, uint16_t dayOfYear) {
    transitionHour.convertTo24();
    rawValue = (DSTTRT_Fixed & 0x03U) | ((dayOfYear & 0x03FFU) << 2) | (((uint32_t)(transitionHour & 0x3FU)) << 16);
}

void DSTTransitionRule::setDate(hour_t transitionHour, Month month, uint8_t dayOfMonth, int8_t dayOffset) {
    transitionHour.convertTo24();
    bool negativeOffset = dayOffset < 0;
    uint8_t dayOffsetABS = (negativeOffset) ? (-1 - dayOffset) : dayOffset;
    rawValue = (DSTTRT_Date & 0x03U) | ((month & 0x0FU) << 2) | (((uint32_t)(dayOfMonth & 0x3FU)) << 6) | (((uint32_t)(dayOffsetABS & 0x07U)) << 12) | (((uint32_t)negativeOffset) << 15) | (((uint32_t)(transitionHour & 0x3FU)) << 16);
}

void DSTTransitionRule::setFloating(hour_t transitionHour, Month month, DayOfWeek dayOfWeek, WeekOfMonth weekOfMonth, int8_t dayOffset) {
    transitionHour.convertTo24();
    bool negativeOffset = dayOffset < 0;
    uint8_t dayOffsetABS = (negativeOffset) ? (-1 - dayOffset) : dayOffset;
    rawValue = (DSTTRT_Floating & 0x03U) | ((month & 0x0FU) << 2) | ((dayOfWeek & 0x07U) << 6) | (((uint32_t)(weekOfMonth & 0x07U)) << 9) | (((uint32_t)(dayOffsetABS & 0x07U)) << 12) | (((uint32_t)negativeOffset) << 15) | (((uint32_t)(transitionHour & 0x3FU)) << 16);
}

int8_t DSTTransitionRule::getDaysOffset() const {
    int8_t dayOffset = (rawValue >> 12) & 0x07U;
    if (rawValue & 0x8000U) {
        //Negative
        return -dayOffset - 1;
    }
    else {
        //Positive
        return dayOffset;
    }
}

void DSTTransitionRule::setDaysOffset(int8_t val) {
    bool negativeOffset = val < 0;
    uint8_t dayOffsetABS = (negativeOffset) ? (-1 - val) : val;
    rawValue = (rawValue & (~(0x0FUL << 12UL))) | (((uint32_t)(dayOffsetABS & 0x07UL)) << 12) | (((uint32_t)negativeOffset) << 15);
}


uint16_t DSTTransitionRule::getDayOfYearOfTransition(int32_t year) const {
    if (noDST()) return 0;

    DSTTransitionRuleType type = getType();
    if (type == DSTTRT_Fixed) {
        return getDayOfYear();
    }
    else {
        bool isLeap = dtlib::isLeapYear(year);
        int32_t daysUntilFirstDayOfYear = dtlib::getDaysUntilYear(year);

        return getDayOfYearOfTransitionHelper(daysUntilFirstDayOfYear, isLeap);
        //Unknown type returns 0
    }
}


int64_t DSTTransitionRule::getDateOfTransitionRaw(int32_t year) const {
    if (noDST()) return 0LL;
    int32_t daysUntilFirstDayOfYear = dtlib::getDaysUntilYear(year);

    int32_t dayOfYearTrans = 0;
    DSTTransitionRuleType type = getType();
    if (type == DSTTRT_Fixed) {
        dayOfYearTrans = getDayOfYear();
    }
    else {
        bool isLeap = dtlib::isLeapYear(year);

        dayOfYearTrans = getDayOfYearOfTransitionHelper(daysUntilFirstDayOfYear, isLeap);
        //Unknown type returns 0
    }

    int32_t days = dayOfYearTrans + daysUntilFirstDayOfYear;
    int64_t transitionTime = getTransitionTime();
    int64_t rawTime = ((int64_t)days) * DAY + transitionTime * HOUR;
    return rawTime;
}

uint16_t DSTTransitionRule::getDayOfYearOfTransitionHelper(int32_t daysUntilFirstDayOfYear, bool isLeap) const {
    DSTTransitionRuleType type = getType();
    uint16_t dayOfYearTrans = 0;
    Month month = getMonth();
    int32_t dayOfYearUntilMonth = dtlib::getDayOfYearFromMonth(month, isLeap);
    int32_t daysUntilFirstDayOfMonth = daysUntilFirstDayOfYear + dayOfYearUntilMonth;

    uint8_t dayOfWeekOffset = 0; //Day of week offset at first day of month
    if (daysUntilFirstDayOfMonth < 0) {
        //BC
        dayOfWeekOffset = (7 + ((daysUntilFirstDayOfMonth - 5) % 7));
    }
    else {
        //AD
        dayOfWeekOffset = (((daysUntilFirstDayOfMonth + 1) % 7) + 1);
    }

    if (type == DSTTRT_Floating) {
        dayOfYearTrans = dtlib::getDayOfYearFromMonth(month, isLeap); //Getting count of days from month
        uint8_t wom = (uint8_t)getWeekOfMonth() - 1;
        uint8_t dow = getDayOfWeek();
        uint8_t DoWoffset;
        //dayOfWeekOffset is day of week of first day in month
        if (dayOfWeekOffset <= dow) {
            DoWoffset = dow - dayOfWeekOffset;
        }
        else {
            DoWoffset = 7 - dayOfWeekOffset + dow;
        }
        uint8_t monthDaysAdd = wom * 7 + DoWoffset;
        if (monthDaysAdd < month.getMonthLength(isLeap)) {
            dayOfYearTrans += monthDaysAdd;
        }
        else {
            dayOfYearTrans += 3 * 7 + DoWoffset;
        }
        dayOfYearTrans += getDaysOffset(); //Adding days offset
    }
    else if (type == DSTTRT_Date) {
        dayOfYearTrans = dtlib::getDayOfYearFromMonth(month, isLeap); //Getting count of days from month
        dayOfYearTrans += (getDayOfMonth() - 1); //Adding days of month
        dayOfYearTrans += getDaysOffset(); //Adding days offset
    }
    return dayOfYearTrans;
}

const DSTTransitionRule DSTTransitionRule::NoDST = DSTTransitionRule();


bool DSTAdjustment::checkDSTRegion(int64_t rawDT) const {
    if (noDST()) return false;

    //Current date info
    int32_t year = dtlib::getYearFromRaw(rawDT);
    bool isLeap = dtlib::isLeapYear(year);
    int32_t daysUntilFirstDayOfYear = dtlib::getDaysUntilYear(year);

    //Current day of year
    int64_t DSTOffset = ((int64_t)getDSTOffsetTotalMinutes()) * MINUTE;
    uint16_t dayOfYear = dtlib::getDaysFromRaw(rawDT) - daysUntilFirstDayOfYear;
    uint16_t dayOfYearDST = dtlib::getDaysFromRaw(rawDT + DSTOffset) - daysUntilFirstDayOfYear;

    //Current hours
    hour_t hours = dtlib::getHoursFromRaw(rawDT);
    hour_t hoursDST = dtlib::getHoursFromRaw(rawDT + DSTOffset);

    //DST start
    uint16_t dayOfYearTransStart = DaylightTransitionStart.getDayOfYearOfTransitionHelper(daysUntilFirstDayOfYear, isLeap);
    bool overStart = dayOfYear > dayOfYearTransStart || (dayOfYear == dayOfYearTransStart && hours >= DaylightTransitionStart.getTransitionTime());

    //DST end
    uint16_t dayOfYearTransEnd = DaylightTransitionEnd.getDayOfYearOfTransitionHelper(daysUntilFirstDayOfYear, isLeap);
    bool overEnd = dayOfYearDST > dayOfYearTransEnd || (dayOfYearDST == dayOfYearTransEnd && hoursDST >= DaylightTransitionEnd.getTransitionTime());

    if (dayOfYearTransStart < dayOfYearTransEnd) {
        //North hemisphere
        return overStart != overEnd; //XOR
    }
    else {
        //South hemisphere
        return overStart == overEnd; //XNOR
    }
}

int64_t DSTAdjustment::getNextTransitionDateRaw(int64_t rawDT, bool& nextTransIsDST) const {
    nextTransIsDST = false;
    if (noDST()) return 0LL;

    //Current date info
    int32_t year = dtlib::getYearFromRaw(rawDT);
    bool isLeap = dtlib::isLeapYear(year);
    int32_t daysUntilFirstDayOfYear = dtlib::getDaysUntilYear(year);

    //Current day of year
    int64_t DSTOffset = ((int64_t)getDSTOffsetTotalMinutes()) * MINUTE;
    uint16_t dayOfYear = dtlib::getDaysFromRaw(rawDT) - daysUntilFirstDayOfYear;
    uint16_t dayOfYearDST = dtlib::getDaysFromRaw(rawDT + DSTOffset) - daysUntilFirstDayOfYear;

    //Current hours
    hour_t hours = dtlib::getHoursFromRaw(rawDT);
    hour_t hoursDST = dtlib::getHoursFromRaw(rawDT + DSTOffset);

    //DST start
    uint16_t dayOfYearTransStart = DaylightTransitionStart.getDayOfYearOfTransitionHelper(daysUntilFirstDayOfYear, isLeap);
    bool overStart = dayOfYear > dayOfYearTransStart || (dayOfYear == dayOfYearTransStart && hours >= DaylightTransitionStart.getTransitionTime());

    //DST end
    uint16_t dayOfYearTransEnd = DaylightTransitionEnd.getDayOfYearOfTransitionHelper(daysUntilFirstDayOfYear, isLeap);
    bool overEnd = dayOfYearDST > dayOfYearTransEnd || (dayOfYearDST == dayOfYearTransEnd && hoursDST >= DaylightTransitionEnd.getTransitionTime());

    int16_t hourOfYearTransStart = dayOfYearTransStart * 24 + DaylightTransitionStart.getTransitionTime();
    int16_t hourOfYearTransEnd = dayOfYearTransEnd * 24 + DaylightTransitionEnd.getTransitionTime();
    int16_t hourOfYearTransEndNoDST = hourOfYearTransEnd - getDSTOffsetMinutes() / 60;
    int16_t hoursPerYear = 365 * 24;
    if(isLeap) hoursPerYear = 366 * 24;

    int64_t rv;
    if (dayOfYearTransStart < dayOfYearTransEnd) {
        //North hemisphere

        if (hourOfYearTransEndNoDST - hourOfYearTransStart >= hoursPerYear) {
            //DST is always applyied
            return 0LL;
        }

        if (!overStart) {
            rv = ((int64_t)(daysUntilFirstDayOfYear + dayOfYearTransStart)) * DAY + ((int64_t)DaylightTransitionStart.getTransitionTime()) * HOUR;
            nextTransIsDST = true;
        }
        else if (!overEnd) {
            rv = ((int64_t)(daysUntilFirstDayOfYear + dayOfYearTransEnd)) * DAY + ((int64_t)DaylightTransitionEnd.getTransitionTime()) * HOUR;
            nextTransIsDST = false;
        }
        else {
            year++;
            if (year == 0) year++;
            nextTransIsDST = true;
            return DaylightTransitionStart.getDateOfTransitionRaw(year);
        }
    }
    else {
        //South hemisphere

        if (hourOfYearTransStart - hourOfYearTransEndNoDST >= hoursPerYear) {
            //DST is always applyied
            return 0LL;
        }

        if (!overEnd) {
            rv = ((int64_t)(daysUntilFirstDayOfYear + dayOfYearTransEnd)) * DAY + ((int64_t)DaylightTransitionEnd.getTransitionTime()) * HOUR;
            nextTransIsDST = false;
        }
        else if (!overStart) {
            rv = ((int64_t)(daysUntilFirstDayOfYear + dayOfYearTransStart)) * DAY + ((int64_t)DaylightTransitionStart.getTransitionTime()) * HOUR;
            nextTransIsDST = true;
        }
        else {
            year++;
            if (year == 0) year++;
            nextTransIsDST = false;
            return DaylightTransitionEnd.getDateOfTransitionRaw(year);
        }
    }
    return rv;
}


const DSTAdjustment DSTAdjustment::NoDST = DSTAdjustment();
const DSTAdjustment DSTAdjustment::NorthAmerica = DSTAdjustment(DSTTransitionRule(2, Month::March, DayOfWeek::Sunday, WeekOfMonth::Second, 0), DSTTransitionRule(2, Month::November, DayOfWeek::Sunday, WeekOfMonth::First, 0), 1);
const DSTAdjustment DSTAdjustment::Cuba = DSTAdjustment(DSTTransitionRule(0, Month::March, DayOfWeek::Sunday, WeekOfMonth::Second, 0), DSTTransitionRule(1, Month::November, DayOfWeek::Sunday, WeekOfMonth::First, 0), 1);
const DSTAdjustment DSTAdjustment::Mexico = DSTAdjustment(DSTTransitionRule(2, Month::April, DayOfWeek::Sunday, WeekOfMonth::First, 0), DSTTransitionRule(2, Month::October, DayOfWeek::Sunday, WeekOfMonth::Last, 0), 1);
const DSTAdjustment DSTAdjustment::WesternEurope = DSTAdjustment(DSTTransitionRule(1, Month::March, DayOfWeek::Sunday, WeekOfMonth::Last, 0), DSTTransitionRule(2, Month::October, DayOfWeek::Sunday, WeekOfMonth::Last, 0), 1);
const DSTAdjustment DSTAdjustment::CentralEurope = DSTAdjustment(DSTTransitionRule(2, Month::March, DayOfWeek::Sunday, WeekOfMonth::Last, 0), DSTTransitionRule(3, Month::October, DayOfWeek::Sunday, WeekOfMonth::Last, 0), 1);
const DSTAdjustment DSTAdjustment::EasternEurope = DSTAdjustment(DSTTransitionRule(3, Month::March, DayOfWeek::Sunday, WeekOfMonth::Last, 0), DSTTransitionRule(4, Month::October, DayOfWeek::Sunday, WeekOfMonth::Last, 0), 1);
const DSTAdjustment DSTAdjustment::Greenland = DSTAdjustment(DSTTransitionRule(22, Month::March, DayOfWeek::Sunday, WeekOfMonth::Last, -1), DSTTransitionRule(23, Month::October, DayOfWeek::Sunday, WeekOfMonth::Last, -1), 1);
const DSTAdjustment DSTAdjustment::Moldova = DSTAdjustment(DSTTransitionRule(2, Month::March, DayOfWeek::Sunday, WeekOfMonth::Last, 0), DSTTransitionRule(3, Month::October, DayOfWeek::Sunday, WeekOfMonth::Last, 0), 1);
const DSTAdjustment DSTAdjustment::Israel = DSTAdjustment(DSTTransitionRule(2, Month::March, DayOfWeek::Sunday, WeekOfMonth::Last, -2), DSTTransitionRule(2, Month::October, DayOfWeek::Sunday, WeekOfMonth::Last, 0), 1);
const DSTAdjustment DSTAdjustment::Lebanon = DSTAdjustment(DSTTransitionRule(0, Month::March, DayOfWeek::Sunday, WeekOfMonth::Last, -2), DSTTransitionRule(0, Month::October, DayOfWeek::Sunday, WeekOfMonth::Last, 0), 1);
const DSTAdjustment DSTAdjustment::Palestine = DSTAdjustment(DSTTransitionRule(0, Month::March, DayOfWeek::Sunday, WeekOfMonth::Last, -2), DSTTransitionRule(1, Month::October, DayOfWeek::Friday, WeekOfMonth::Last, 0), 1);
const DSTAdjustment DSTAdjustment::Syria = DSTAdjustment(DSTTransitionRule(0, Month::March, DayOfWeek::Friday, WeekOfMonth::Last, -2), DSTTransitionRule(0, Month::October, DayOfWeek::Friday, WeekOfMonth::Last, 0), 1);
const DSTAdjustment DSTAdjustment::Jordan = DSTAdjustment(DSTTransitionRule(0, Month::February, DayOfWeek::Thursday, WeekOfMonth::Last, -2), DSTTransitionRule(1, Month::October, DayOfWeek::Friday, WeekOfMonth::Last, 0), 1);
const DSTAdjustment DSTAdjustment::Chile = DSTAdjustment(DSTTransitionRule(0, Month::September, DayOfWeek::Saturday, WeekOfMonth::First, -2), DSTTransitionRule(0, Month::April, DayOfWeek::Saturday, WeekOfMonth::First, 0), 1);
const DSTAdjustment DSTAdjustment::Paraguay = DSTAdjustment(DSTTransitionRule(0, Month::October, DayOfWeek::Sunday, WeekOfMonth::First, -2), DSTTransitionRule(0, Month::March, DayOfWeek::Sunday, WeekOfMonth::Fourth, 0), 1);
const DSTAdjustment DSTAdjustment::Australia = DSTAdjustment(DSTTransitionRule(2, Month::October, DayOfWeek::Sunday, WeekOfMonth::First, -2), DSTTransitionRule(3, Month::April, DayOfWeek::Sunday, WeekOfMonth::First, 0), 1);
const DSTAdjustment DSTAdjustment::Australia_Lord_Howe_Island = DSTAdjustment(DSTTransitionRule(2, Month::October, DayOfWeek::Sunday, WeekOfMonth::First, -2), DSTTransitionRule(2, Month::April, DayOfWeek::Sunday, WeekOfMonth::First, 0), 0, 30, false);
const DSTAdjustment DSTAdjustment::NewZeland = DSTAdjustment(DSTTransitionRule(2, Month::September, DayOfWeek::Sunday, WeekOfMonth::Last, -2), DSTTransitionRule(2, Month::April, DayOfWeek::Sunday, WeekOfMonth::First, 0), 1);


const TimeZoneInfo TimeZoneInfo::Empty = TimeZoneInfo();

TimeZoneInfo TimeZoneInfo::fromPOSIX(const char* buffer, int bufferSize, int& pos) {
    pos = 0;
    if (bufferSize <= 0) return TimeZoneInfo::Empty;
    int oldBufSize = bufferSize;

    TimeZoneInfo ret;
    
    //Copying abbreviation name
    const char* newBuffer = copyABR(buffer, bufferSize, ret.standardABR);
    int diff = (int)(newBuffer - buffer);
    bufferSize -= diff;
    buffer = newBuffer;
    if (bufferSize < 1 || diff < 1) {
        //Parsing ABR name failed
        pos = bufferSize - oldBufSize;
        return ret;
    }

    int16_t offsValTZ; //In minutes
    if (!parseOffsTime(buffer, bufferSize, offsValTZ) || offsValTZ >= 900 || offsValTZ <= -900) {
        //Parsing offset failed
        pos = bufferSize - oldBufSize;
        return ret;
    }
    offsValTZ = -offsValTZ; //POSIX has offset specified as offset from local time to UTC, but current library specifies
                            //offset as value, that need tu be add to UTC time to get local time
    ret.timeZone = TimeZone::fromTotalMinutesOffset(offsValTZ);

    if (*buffer == '\0' || bufferSize < 1) {
        //Only time zone was specified
        pos = oldBufSize - bufferSize;
        return ret;
    }

    //Copying abbreviation summer time name
    newBuffer = copyABR(buffer, bufferSize, ret.daylightABR);
    diff = (int)(newBuffer - buffer);
    bufferSize -= diff;
    buffer = newBuffer;
    if (bufferSize < 1 || diff < 1) {
        //Parsing ABR name failed
        pos = bufferSize - oldBufSize;
        return ret;
    }

    int16_t offsValDST; //In minutes
    size_t oldBufferSize2 = bufferSize;
    if (!parseOffsTime(buffer, bufferSize, offsValDST) || offsValDST >= 900 || offsValDST <= -900) {
        if (oldBufferSize2 != bufferSize || offsValDST >= 900 || offsValDST <= -900) {
            //Parsing offset failed
            pos = bufferSize - oldBufSize;
            return ret;
        }
        else {
            //DST offset not specified, assuming 1 hour more than time zone
            offsValDST = -(offsValTZ + 60); //Set to negative -> see below
        }
    }
    //DST offset in POSIX is offset of local DST time from UTC time, but this library has DST offset specified as
    //offset of local time without DST from local time with DST.
    offsValDST = -offsValDST - offsValTZ;

    if (*buffer == '\0' || bufferSize < 4 || *buffer != ',') {
        //Summer time zone specified, but transition rules was not found
        pos = bufferSize - oldBufSize;
        return ret;
    }
    ++buffer; //Skip comma
    --bufferSize;
    if (bufferSize < 1) {
        //No text after comma
        pos = bufferSize - oldBufSize;
        return ret;
    }

    DSTTransitionRule tr1 = parseDSTTrans(buffer, bufferSize);
    if (tr1 == DSTTransitionRule::NoDST) {
        //Error
        pos = bufferSize - oldBufSize;
        return ret;
    }

    if (*buffer != ',' || bufferSize < 2) {
        //Seconds rule is missing
        pos = bufferSize - oldBufSize;
        return ret;
    }
    ++buffer; //Skip comma
    --bufferSize;

    DSTTransitionRule tr2 = parseDSTTrans(buffer, bufferSize);
    if (tr2 == DSTTransitionRule::NoDST) {
        //Error
        pos = bufferSize - oldBufSize;
        return ret;
    }

    ret.DST = DSTAdjustment(tr1, tr2, offsValDST / 60, offsValDST % 60, false);
    pos = oldBufSize - bufferSize;

    return ret;
}

TimeZoneInfo TimeZoneInfo::fromPOSIX(const char* buffer, int bufferSize) {
    int pos;
    return fromPOSIX(buffer, bufferSize, pos);
}

#ifdef ARDUINO
TimeZoneInfo TimeZoneInfo::fromPOSIX(String text, int& pos) {
    return TimeZoneInfo::fromPOSIX(text.c_str(), text.length() + 1, pos);
}
#else
TimeZoneInfo TimeZoneInfo::fromPOSIX(std::string text, int& pos) {
    return TimeZoneInfo::fromPOSIX(text.c_str(), text.length()+1, pos);
}
#endif // ARDUINO

#ifdef ARDUINO
TimeZoneInfo TimeZoneInfo::fromPOSIX(String text) {
    int pos;
    return TimeZoneInfo::fromPOSIX(text.c_str(), text.length() + 1, pos);
}
#else
TimeZoneInfo TimeZoneInfo::fromPOSIX(std::string text) {
    int pos;
    return TimeZoneInfo::fromPOSIX(text.c_str(), text.length() + 1, pos);
}
#endif // ARDUINO

char* TimeZoneInfo::toPOSIX(char* buffer, int bufferSize) const {
    if (bufferSize <= 0) return buffer;
    bufferSize--;

    int16_t offsValTZ = -timeZone.getTimeZoneOffsetTotalMinutes();
    if (standardABR[0] == '\0') {
        //ABR name not specified
        char* newBuff = getNumericABRFromOffset(buffer, bufferSize, offsValTZ);
        bufferSize -= newBuff - buffer;
        buffer = newBuff;
        if (bufferSize < 1) {
            //Too small buffer
            *buffer = '\0';
            return buffer;
        }
    }
    else {
        for (int i = 0; i < TIME_ZONE_INFO_TZ_ABR_NAME_SIZE - 1 && standardABR[i] != '\0' && bufferSize > 0; i++) {
            *buffer = standardABR[i];
            ++buffer;
            --bufferSize;
        }
    }
    if (bufferSize < 4) {
        //Too small buffer
        *buffer = '\0';
        return buffer;
    }

    char* newBuff = offsTimeToStr(buffer, bufferSize, offsValTZ);
    int diff = (int)(newBuff - buffer);
    bufferSize -= diff;
    buffer = newBuff;
    if (bufferSize < 1 || diff < 1) {
        //Too small buffer
        *buffer = '\0';
        return buffer;
    }

    if (!DST.noDST()) {
        int16_t offsValDST = -DST.getDSTOffsetTotalMinutes();
        if (daylightABR[0] == '\0') {
            //ABR name not specified
            char* newBuff = getNumericABRFromOffset(buffer, bufferSize, offsValDST);
            bufferSize -= newBuff - buffer;
            buffer = newBuff;
            if (bufferSize < 1) {
                //Too small buffer
                *buffer = '\0';
                return buffer;
            }
        }
        else {
            for (int i = 0; i < TIME_ZONE_INFO_TZ_ABR_NAME_SIZE - 1 && daylightABR[i] != '\0' && bufferSize > 0; i++) {
                *buffer = daylightABR[i];
                ++buffer;
                --bufferSize;
            }
        }
        if (bufferSize < 1) {
            //Too small buffer
            *buffer = '\0';
            return buffer;
        }

        if (offsValDST != -60) {
            offsValDST = offsValTZ + offsValDST;
            char* newBuff = offsTimeToStr(buffer, bufferSize, offsValDST);
            int diff = (int)(newBuff - buffer);
            bufferSize -= diff;
            buffer = newBuff;
            if (bufferSize < 1 || diff < 1) {
                //Too small buffer
                *buffer = '\0';
                return buffer;
            }
        }

        uint8_t isEnd = 0;
        while (isEnd < 2) {
            if (bufferSize < 3) {
                //Too small buffer
                *buffer = '\0';
                return buffer;
            }
            *buffer = ',';
            ++buffer;
            --bufferSize;
            
            DSTTransitionRule rule;
            if (isEnd) {
                rule = DST.DaylightTransitionEnd;
            }
            else {
                rule = DST.DaylightTransitionStart;
            }

            DSTTransitionRuleType type = rule.getType();
            if (type == DSTTransitionRuleType::DSTTRT_Date) {
                *buffer = 'J';
                ++buffer;
                --bufferSize;

                uint16_t doy = dtlib::getDayOfYearFromMonth(rule.getMonth(), false) + rule.getDayOfMonth() - 1;

                if (bufferSize < 1 || !fi3toaHelper(buffer, bufferSize, doy + 1)) {
                    return buffer;
                }

            }
            else if (type == DSTTransitionRuleType::DSTTRT_Floating) {
                *buffer = 'M';
                ++buffer;
                --bufferSize;

                if (bufferSize < 1 || !fi3toaHelper(buffer, bufferSize, rule.getMonth())) {
                    return buffer;
                }

                *buffer = '.';
                ++buffer;
                --bufferSize;

                if (bufferSize < 1 || !fi3toaHelper(buffer, bufferSize, (uint8_t)rule.getWeekOfMonth())) {
                    return buffer;
                }

                *buffer = '.';
                ++buffer;
                --bufferSize;

                if (bufferSize < 1 || !fi3toaHelper(buffer, bufferSize, ((uint8_t)rule.getDayOfWeek()) - 1)) {
                    return buffer;
                }
            }
            else {
                //Fixed
                if (bufferSize < 1 || !fi3toaHelper(buffer, bufferSize, rule.getDayOfYear())) {
                    return buffer;
                }
            }

            if (bufferSize < 3) {
                //Too low buffer
                return buffer;
            }
            *buffer = '/';
            ++buffer;
            --bufferSize;

            hour_t tt = rule.getTransitionTime();
            tt.convertTo24();
            int8_t doffs = rule.getDaysOffset();
            int16_t transTime = doffs * 24 + tt;
            if (transTime < 0 && bufferSize > 0) {
                *buffer = '-';
                ++buffer;
                --bufferSize;
                transTime = -transTime;
            }
            if (bufferSize < 1 || !fi3toaHelper(buffer, bufferSize, transTime)) {
                return buffer;
            }
            
            isEnd++;
        }
    }

    *buffer = '\0';
    return buffer;
}

#ifdef ARDUINO
String TimeZoneInfo::toPOSIX() const {
    String ret;
#else
std::string TimeZoneInfo::toPOSIX() const {
    std::string ret;
#endif // ARDUINO

    bool hasDST = !DST.noDST();
    ret.reserve((hasDST) ? 58 : 14);

    char buffer[12];
    constexpr static size_t bufferSize = sizeof(buffer) / sizeof(char);

    int16_t offsValTZ = -timeZone.getTimeZoneOffsetTotalMinutes();
    if (standardABR[0] == '\0') {
        //ABR name not specified
        getNumericABRFromOffset(buffer, bufferSize, offsValTZ);
        ret += buffer;
    }
    else {
        for (int i = 0; i < TIME_ZONE_INFO_TZ_ABR_NAME_SIZE - 1 && standardABR[i] != '\0'; i++) {
            ret += standardABR[i];
        }
    }

    offsTimeToStr(buffer, bufferSize, offsValTZ);
    ret += buffer;
    if (hasDST) {
        int16_t offsValDST = -DST.getDSTOffsetTotalMinutes();
        if (daylightABR[0] == '\0') {
            //ABR name not specified
            getNumericABRFromOffset(buffer, bufferSize, offsValTZ);
            ret += buffer;
        }
        else {
            for (int i = 0; i < TIME_ZONE_INFO_TZ_ABR_NAME_SIZE - 1 && daylightABR[i] != '\0'; i++) {
                ret += daylightABR[i];
            }
        }

        if (offsValDST != -60) {
            offsValDST = offsValTZ + offsValDST;
            offsTimeToStr(buffer, bufferSize, offsValDST);
            ret += buffer;
        }

        uint8_t isEnd = 0;
        while (isEnd < 2) {
            ret += ',';

            DSTTransitionRule rule;
            if (isEnd) {
                rule = DST.DaylightTransitionEnd;
            }
            else {
                rule = DST.DaylightTransitionStart;
            }

            DSTTransitionRuleType type = rule.getType();
            if (type == DSTTransitionRuleType::DSTTRT_Date) {
                ret += 'J';

                uint16_t doy = dtlib::getDayOfYearFromMonth(rule.getMonth(), false) + rule.getDayOfMonth() - 1;
                
                char* bufferVar = buffer;
                int bufferSizeVar = bufferSize;
                fi3toaHelper(bufferVar, bufferSizeVar, doy + 1);
                ret += buffer;
            }
            else if (type == DSTTransitionRuleType::DSTTRT_Floating) {
                ret += 'M';

                char* bufferVar = buffer;
                int bufferSizeVar = bufferSize;
                fi3toaHelper(bufferVar, bufferSizeVar, rule.getMonth());
                ret += buffer;

                ret += '.';

                bufferVar = buffer;
                bufferSizeVar = bufferSize;
                fi3toaHelper(bufferVar, bufferSizeVar, (uint8_t)rule.getWeekOfMonth());
                ret += buffer;

                ret += '.';

                bufferVar = buffer;
                bufferSizeVar = bufferSize;
                fi3toaHelper(bufferVar, bufferSizeVar, ((uint8_t)rule.getDayOfWeek()) - 1);
                ret += buffer;
            }
            else {
                //Fixed
                char* bufferVar = buffer;
                int bufferSizeVar = bufferSize;
                fi3toaHelper(bufferVar, bufferSizeVar, rule.getDayOfYear());
                ret += buffer;
            }

            ret += '/';

            hour_t tt = rule.getTransitionTime();
            tt.convertTo24();
            int8_t doffs = rule.getDaysOffset();
            int16_t transTime = doffs * 24 + tt;
            if (transTime < 0 && bufferSize > 0) {
                ret += '-';
                transTime = -transTime;
            }
            char* bufferVar = buffer;
            int bufferSizeVar = bufferSize;
            fi3toaHelper(bufferVar, bufferSizeVar, transTime);
            ret += buffer;

            isEnd++;
        }
    }

#ifdef ARDUINO
    ret.reserve(ret.length() + 1); //Shring to fit by arduino
#else
    ret.shrink_to_fit();
#endif // ARDUINO

    return ret;
}



const char* TimeZoneInfo::copyABR(const char* bufferFrom, size_t bufferFromSize, char* bufferTo) {
    
    //Resolving maximum size
    if (bufferFromSize > TIME_ZONE_INFO_TZ_ABR_NAME_SIZE) {
        bufferFromSize = TIME_ZONE_INFO_TZ_ABR_NAME_SIZE - 1;
    }

    if (bufferFromSize < 1) return bufferFrom; //bufferFrom is empty

    if (*bufferFrom == '<') { //for example: <+01>,<-04>
        //Custom time zone abreviation
        for (; bufferFromSize > 0 && *bufferFrom != '\0'; --bufferFromSize, ++bufferFrom, ++bufferTo) {
            *bufferTo = *bufferFrom; //Copying characters
            if (*bufferFrom == '>') {
                ++bufferFrom;
                ++bufferTo;
                break;
            }
        }
    }
    else { //for example: CET, CEST
        //Standardized time zone abreviation
        for (; bufferFromSize > 0 && *bufferFrom != '\0' && ((*bufferFrom >= 'a' && *bufferFrom <= 'z') || (*bufferFrom >= 'A' && *bufferFrom <= 'Z') || *bufferFrom == '_'); --bufferFromSize, ++bufferFrom, ++bufferTo) {
            *bufferTo = *bufferFrom; //Copying characters
        }
        
    }
    *bufferTo = 0; //Null terminator
    return bufferFrom;
}

const char* TimeZoneInfo::atoiTZ(const char* bufferFrom, int bufferFromSize, int& value, uint8_t digitsLimit) {
    if (bufferFromSize < 1) {
        value = INT_MIN;
        return bufferFrom;
    }
    
    bool negative = *bufferFrom == '-';
    if (negative || *bufferFrom == '+') {
        ++bufferFrom;
        --bufferFromSize;
    }

    if (bufferFromSize > digitsLimit) { //Limit
        bufferFromSize = digitsLimit;
    }

    if (bufferFromSize < 1) {
        //Parsing failed, no digit found
        value = INT_MIN;
        return bufferFrom;
    }

    value = 0;

    while (bufferFromSize > 0 && *bufferFrom >= '0' && *bufferFrom <= '9') {
        value *= 10;
        value += *bufferFrom - '0';
        ++bufferFrom;
        --bufferFromSize;
    }

    if (negative) {
        value = -value;
    }

    return bufferFrom;
}

bool TimeZoneInfo::parseOffsTime(const char*& buffer, int& bufferSize, int16_t& parsed_min) {
    //Parsing TZ offset (hours)
    int val;
    const char* newBuffer = atoiTZ(buffer, bufferSize, val, 3);
    int diff = (int)(newBuffer - buffer);
    bufferSize -= diff;
    parsed_min = val * 60; //Hours
    bool negative = buffer[0] == '-';
    bool posSign = buffer[0] == '+';
    buffer = newBuffer;
    bool err = val == INT_MIN || diff < (1 + (negative || posSign));
    if (bufferSize < 1 || err) {
        return !err;
    }
    

    if (*buffer == ':') {
        //Parsing offset (minutes)
        --bufferSize;
        ++buffer;
        if (bufferSize < 1) {
            return false; //Offset in minutes is expected
        }

        int val2;
        newBuffer = atoiTZ(buffer, bufferSize, val2, 2);
        diff = (int)(newBuffer - buffer);
        bufferSize -= diff;
        bool negativeSig = buffer[0] == '-';
        posSign = buffer[0] == '+';
        buffer = newBuffer;
        if (negative) {
            parsed_min -= val2;
        }
        else {
            parsed_min += val2;
        }
        err = val2 == INT_MIN || diff < 1 || negativeSig || posSign;
        if (bufferSize < 2 || err) {
            return !err;
        }
        if (*buffer == ':') {
            //Skipping offset in seconds
            uint8_t i = 0;
            do {
                --bufferSize;
                ++buffer;
                if (bufferSize < 1 || i >= 3) {
                    return i >= 1 && i < 3;
                }
                i++;
            } while (*buffer >= '0' && *buffer <= '9');
            if (((*buffer >= '0' && *buffer <= '9') || *buffer == '+' || *buffer == '-') || !(i > 1 && i <= 3)) {
                //Number found, but not expected
                return false; //Error
            }
        }
        else if (((*buffer >= '0' && *buffer <= '9') || *buffer == '+' || *buffer == '-')) {
            //Number found, but not expected
            return false; //Error
        }
    }
    else if (((*buffer >= '0' && *buffer <= '9') || *buffer == '+' || *buffer == '-')) {
        //Number found, but not expected
        return false; //Error
    }
    return true;
}

DSTTransitionRule TimeZoneInfo::parseDSTTrans(const char*& buffer, int& bufferSize) {
    if (bufferSize < 2) {
        return DSTTransitionRule::NoDST;
    }

    DSTTransitionRule ret;

    if (*buffer == 'J') {
        //In this form, n counts from 1 to 365, and February 29 is not counted even if it is present.
        //(Thus, a transition occurring on February 29 could not be specified this way. However, days
        //after February have the same numbers whether it's a leap year or not, so that this form is
        //usually more useful than the plain-integer form for transitions on fixed dates.)

        ++buffer; //Skip J
        --bufferSize;

        int val;
        const char* newBuffer = atoiTZ(buffer, bufferSize, val, 3);
        int diff = (int)(newBuffer - buffer);
        bufferSize -= diff;
        bool negative = buffer[0] == '-';
        buffer = newBuffer;
        if (val < 1 || val > 365 || negative) {
            //Parsing failed
            return ret;
        }
        else {
            month_day_tuple md = dtlib::getMonthFromDayOfYear(val - 1, false);
            if (*buffer == '\0' || *buffer != '/' || bufferSize < 1) {
                //Transition time is not specified, so 02:00:00 is assumed.
                return DSTTransitionRule::Date(2, md.month, md.day);
            }
            else {
                month_day_tuple md = dtlib::getMonthFromDayOfYear(val - 1, false);
                ret = DSTTransitionRule::Date(0, md.month, md.day);
            }
        }
    }
    else if (*buffer == 'M') {
        //This specifies day d of week w of month m. The day d must be between 0 (Sunday) and 6.
        //The week w must be between 1 and 5; week 1 is the first week in which day d occurs,
        //and week 5 specifies the last d day in the month. The month m should be between 1 and 12.

        ++buffer; //Skip M
        --bufferSize;

        int val;
        const char* newBuffer = atoiTZ(buffer, bufferSize, val, 2);
        int diff = (int)(newBuffer - buffer);
        bufferSize -= diff;
        bool negative = buffer[0] == '-';
        buffer = newBuffer;
        if (val < 1 || val > 12 || negative || *buffer != '.') {
            //Parsing failed
            return ret;
        }
        
        ++buffer; //Skip dot
        --bufferSize;
        uint8_t val2 = 0;
        if (bufferSize < 2 || !(*buffer >= '1' && *buffer <= '5') ) {
            //Parsing failed, one digit between 1 to 5 was expected
            return ret;
        }
        val2 = *buffer - '0';
        ++buffer; //Skip digit
        --bufferSize;

        if (*buffer != '.') {
            //Parsing failed - dot expected
            return ret;
        }
        ++buffer; //Skip dot
        --bufferSize;
        uint8_t val3 = 0;
        if (bufferSize < 2 || !(*buffer >= '0' && *buffer <= '6')) {
            //Parsing failed, one digit between 0 to 6 was expected
            return ret;
        }
        val3 = *buffer - '0';
        ++buffer; //Skip digit
        --bufferSize;

        if (bufferSize > 0 && (*buffer >= '0' && *buffer <= '6')) {
            //Parsing failed, another digit was found, but was not expected
            return ret;
        }
        else if (*buffer == '\0' || *buffer != '/' || bufferSize < 1) {
            return DSTTransitionRule::Floating(2, val, val3 + 1, (WeekOfMonth)val2);
        }
        else {
            ret = DSTTransitionRule::Floating(0, val, val3 + 1, (WeekOfMonth)val2);
        }

    }
    else {
        //Just number - A plain integer denotes a day of the year, counting from zero to 364, or to 365 in leap years.
        int val;
        const char* newBuffer = atoiTZ(buffer, bufferSize, val, 3);
        int diff = (int)(newBuffer - buffer);
        bufferSize -= diff;
        buffer = newBuffer;
        if (val < 0 || val > 365) {
            //Parsing failed
            return ret;
        }
        else if (*buffer == '\0' || *buffer != '/' || bufferSize < 1) {
            return DSTTransitionRule::Fixed(2, val);
        }
        else {
            ret = DSTTransitionRule::Fixed(0, val);
        }
    }

    if (*buffer == '/') {
        ++buffer; //Skip slash
        --bufferSize;
        if (bufferSize < 1) {
            //Parsing failed
            return DSTTransitionRule::NoDST;
        }
        int16_t parsMin;
        if (!parseOffsTime(buffer, bufferSize, parsMin) || parsMin >= 10080 || parsMin <= -10080) {
            //Parsing failed
            return DSTTransitionRule::NoDST;
        }
        else {
            bool isNegative = parsMin < 0;
            int16_t parsMinABS = isNegative ? -parsMin : parsMin;
            parsMinABS /= 60; //To get hours
            int16_t hourPart = parsMinABS % 24;
            int16_t daysPart = parsMinABS / 24;

            if (isNegative) {
                daysPart = -daysPart;
                if (hourPart > 0) {
                    daysPart--;
                }
                hourPart = 24 - hourPart; //flip
                if (daysPart < -9) {
                    //Parsing failed - Out of range
                    return DSTTransitionRule::NoDST;
                }
            }
            else if (daysPart > 8) {
                //Parsing failed - Out of range
                return DSTTransitionRule::NoDST;
            }

            if (daysPart != 0) {
                ret.setDaysOffset((int8_t)daysPart);
            }
            if (hourPart != 0) {
                ret.setTransitionTime((uint8_t)hourPart);
            }
            return ret;
        }
    }
    return DSTTransitionRule::NoDST;
}

bool TimeZoneInfo::fi3toaHelper(char*& buffer, int& bufferSize, uint16_t val) {
    if (val >= 100) { //Three digits
        if (bufferSize < 3) {
            //Too low buffer
            *buffer = '\0';
            return false;
        }
        *buffer = val / 100 + '0';
        ++buffer;
        val = val % 100;
        *buffer = val / 10 + '0';
        ++buffer;
        *buffer = val % 10 + '0';
        ++buffer;
        bufferSize -= 3;
    }
    else if (val >= 10) { //Two digits
        if (bufferSize < 2) {
            //Too low buffer
            *buffer = '\0';
            return false;
        }
        *buffer = val / 10 + '0';
        ++buffer;
        *buffer = val % 10 + '0';
        ++buffer;
        bufferSize -= 2;
    }
    else {
        //One digit
        *buffer = val + '0';
        ++buffer;
        --bufferSize;
    }
    *buffer = '\0';
    return true;
}

char* TimeZoneInfo::offsTimeToStr(char* buffer, int bufferSize, int offset) {
    bufferSize--;
    if (bufferSize < 3) {
        //Too low buffer
        *buffer = '\0';
        return buffer;
    }

    *buffer = (offset < 0) ? '-' : '+';
    ++buffer;
    --bufferSize;
    if (offset < 0) {
        offset = -offset;
    }
    int hOffs = offset / 60;
    int mOffs = offset % 60;
    if (!fi3toaHelper(buffer, bufferSize, hOffs)) {
        return buffer;
    }

    if (mOffs > 0) {
        if (bufferSize < 4) {
            //Too low buffer
            *buffer = '\0';
            return buffer;
        }
        *buffer = ':';
        ++buffer;
        if (!fi3toaHelper(buffer, bufferSize, mOffs)) {
            return buffer;
        }
    }
    *buffer = '\0';
    return buffer;
}

char* TimeZoneInfo::getNumericABRFromOffset(char* buffer, int bufferSize, int16_t offset) {
    if (bufferSize < 6) {
        //Too low buffer
        *buffer = '\0';
        return buffer;
    }

    bool isNegative = offset < 0;
    if (isNegative) offset = -offset;
    int8_t hrs = offset / 60;
    int8_t mins = offset % 60;

    *buffer = '<';
    ++buffer;
    *buffer = (isNegative) ? '-' : '+';
    ++buffer;
    *buffer = (hrs / 10) - '0';
    ++buffer;
    *buffer = (hrs % 10) - '0';
    ++buffer;
    if (mins > 0) {
        if (bufferSize < 8) {
            //Too low buffer
            *buffer = '\0';
            return buffer;
        }
        *buffer = (mins / 10) - '0';
        ++buffer;
        *buffer = (mins % 10) - '0';
        ++buffer;
    }
    *buffer = '>';
    ++buffer;
    *buffer = '\0';
    return buffer;
}


#if DT_UNDER_OS == DT_WIN //Windows specific code
TimeZoneInfo TimeZoneInfo::getSystemTZInfo() {
    DYNAMIC_TIME_ZONE_INFORMATION timeZoneInformation;
    GetDynamicTimeZoneInformation(&timeZoneInformation);

    TimeZoneInfo ret;

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
    if (supportsDst) {
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

        ret.DST = DSTAdjustment(startRule, endRule, DST_offset_h, DST_offset_m, false);
    }

    ret.timeZone = TimeZone::fromTotalMinutesOffset(-timeZoneInformation.Bias);

    std::wstring tmpKey = timeZoneInformation.TimeZoneKeyName;
    ret.keyName = std::string((const char*)&tmpKey[0], sizeof(wchar_t) / sizeof(char) * tmpKey.size());
    ret.standardName = timeZoneInformation.StandardName;
    ret.daylightName = timeZoneInformation.DaylightName;

    return ret;
}
#elif DT_UNDER_OS == DT_LINUX //Linux specific code
TimeZoneInfo TimeZoneInfo::getSystemTZInfo() {
    return TimeZoneInfo(); //TODO implement
}
#elif DT_UNDER_OS == DT_MAC //Mac OS specific code
TimeZoneInfo TimeZoneInfo::getSystemTZInfo() {
    return TimeZoneInfo(); //TODO implement
}
#elif defined(ESP32) || defined(ESP8266) //ESP32, ESP8266 specific code
TimeZoneInfo TimeZoneInfo::getSystemTZInfo() {
    const char* tz_env = getenv("TZ");
    if (tz_env != NULL) {
        int pos;
        return TimeZoneInfo::fromPOSIX(tz_env, strlen(tz_env), pos);
    }
    else {
        return TimeZoneInfo();
    }
}
#else //Arduino specific code
TimeZoneInfo TimeZoneInfo::getSystemTZInfo() {
    //TODO maybe create some weak function to handle this
    return TimeZoneInfo(); //Not implemented on Arduino now
}
#endif


#if defined(ESP32) || defined(ESP8266)
void TimeZoneInfo::setSystemTZInfo(const TimeZoneInfo& tzinfo) {
    char buffer[58];
    tzinfo.toPOSIX(buffer, sizeof(buffer) / sizeof(char));
    setenv("TZ", buffer, 1/*overwrite*/);
    tzset();
}
#endif // defined(ESP32) || defined(ESP8266)