/**
 * @file DateTimeHelpers.h
 * @brief This file contains helper classes, structures, definitions and functions for DateTime.
 * 
 * @todo Add isDST flag to date_s, time_s and date_time_s
 *
 * # Credits
 * @author Matej Fitoš
 * @date Jun 4, 2022
 */

#ifndef _DATE_TIME_HELPERS_H
#define _DATE_TIME_HELPERS_H

#define DT_NO_OS    (0)
#define DT_WIN      (1)
#define DT_LINUX    (2)
#define DT_MAC      (3)

#if defined(_WIN64) || defined(_WIN32)
#define DT_UNDER_OS	(DT_WIN) //DateTime is used in app, that will run under some OS
#include <winsock.h>
#elif defined(__APPLE__)
#define DT_UNDER_OS	(DT_MAC) //DateTime is used in app, that will run under some OS
#elif defined(__linux) || defined(__unix) || defined(__posix)
#define DT_UNDER_OS	(DT_LINUX) //DateTime is used in app, that will run under some OS
#else
#define DT_UNDER_OS	(DT_NO_OS) //DateTime is used in app, that will run on some microcontroller without OS
#endif

#include <time.h>
#include <stdlib.h>

#if __cplusplus >= 201703L
#define CONSTEXPR_IF if constexpr
#else
#define CONSTEXPR_IF if constexpr //TODO shows warning
#endif

#if DT_UNDER_OS > 0
#include <chrono>
#include <string>

#define DT_SYNC_TYPE			int64_t
#define DT_SYNC_FUNC()			getSysTicks()
#define DT_SYNC_RESOLUTION		(MICROSECOND)
#define DT_SUPPORTS_NOW         (1U)    //now() and nowUTC() functions are defined in DateTimeSysSync and DateTimeTZSysSync

/**
* @brief Gets system ticks in microseconds (same as micros64() on Arduino), the time will be synchronized with in classes
* DateTimeSysSync and DateTimeTZSysSync on Windows, Linux and Mac OS.
*/
int64_t getSysTicks();

#else

#if defined(ARDUINO)
#include "Arduino.h"
//#include <cstdint>

#if defined(ESP8266)
//ESP8266 uses micros64() for synchronizing time
#define DT_SYNC_TYPE			int64_t
#define DT_SYNC_FUNC()			micros64()
#define DT_SYNC_RESOLUTION		(MICROSECOND)
#define DT_SUPPORTS_NOW         (1U)    //now() and nowUTC() functions are defined in DateTimeSysSync and DateTimeTZSysSync
#elif defined(ESP32)
//ESP32 uses esp_timer_get_time() for synchronizing time
#include "esp_timer.h"
#define DT_SYNC_TYPE			uint64_t
#define DT_SYNC_FUNC()			esp_timer_get_time()
#define DT_SYNC_RESOLUTION		(MICROSECOND)
#define DT_SUPPORTS_NOW         (1U)    //now() and nowUTC() functions are defined in DateTimeSysSync and DateTimeTZSysSync
#else
//Other arduinos uses millis() for synchronizing time
#define DT_SYNC_TYPE			int32_t
#define DT_SYNC_FUNC()			millis()
#define DT_SYNC_RESOLUTION		(MILLISECOND)
#define DT_SUPPORTS_NOW         (0U)    //now() and nowUTC() functions are unsupported on Arduino, because it has no system clock
//TODO use microseconds on arduino
#endif // defined(ESP8266) || defined(ESP32)

#endif //ARDUINO

#endif

#include "DT_type_traits_helper.h"


#ifndef F
#define F()
#endif // !F



#ifndef MTYPE_CHECKER
/// Checker for member with given name and convertible type
#define MTYPE_CHECKER(checker, name) \
template<class C, typename T, typename = void> struct checker : dtlib::false_type {}; \
template<class C, typename T> struct checker<C, T, typename dtlib::enable_if<dtlib::is_convertible<decltype(C::name), T>::value>::type> : dtlib::true_type {}
#endif // !MTYPE_CHECKER

#ifndef MTYPE_CHECKER_STRICT
/// Checker for member with given name and exact type
#define MTYPE_CHECKER_STRICT(checker, name) \
template<class C, typename T, typename = void> struct checker : dtlib::false_type {}; \
template<class C, typename T> struct checker<C, T, typename dtlib::enable_if<dtlib::is_same<decltype(C::name), T>::value>::type> : dtlib::true_type {}
#endif // !MTYPE_CHECKER_STRICT

#ifndef MTYPE_CHECKER_ANY
/// Checker for member with given name and any type
#define MTYPE_CHECKER_ANY(checker, name) \
template<class C, typename = void> struct checker : dtlib::false_type {}; \
template<class C> struct checker<C, typename dtlib::enable_if<!dtlib::is_same<decltype(C::name)*, void>::value>::type> : dtlib::true_type {}
#endif // !MTYPE_CHECKER_ANY

#ifndef METHOD_CHECKER
/// Checker for member function with convertible return type and accepting given arguments
#define METHOD_CHECKER(checker, name, ret, args) \
template<class C, typename=void> struct checker : dtlib::false_type {}; \
template<class C> struct checker<C, typename dtlib::enable_if<dtlib::is_convertible<decltype(dtlib::declval<C>().name args), ret>::value>::type> : dtlib::true_type {};
#endif // !METHOD_CHECKER

#ifndef METHOD_CHECKER_STRICT_RET
/// Checker for member function with exact return type and accepting given arguments
#define METHOD_CHECKER_STRICT_RET(name, fn, ret, args) \
template<class C, typename=void> struct name : dtlib::false_type {}; \
template<class C> struct name<C, typename dtlib::enable_if<dtlib::is_same<decltype(dtlib::declval<C>().fn args), ret>::value>::type> : dtlib::true_type {};
#endif // !METHOD_CHECKER_STRICT_RET

#ifndef METHOD_CHECKER_ANY
/// Checker for member function accepting given arguments
#define METHOD_CHECKER_ANY(name, fn, args) \
template<class C, typename=void> struct name : dtlib::false_type {}; \
template<class C> struct name<C, typename dtlib::enable_if<!dtlib::is_same<decltype(dtlib::declval<C>().fn args)*, void>::value>::type> : dtlib::true_type {};
#endif // !METHOD_CHECKER_ANY

#define DAYS_IN_400_YRS	(365L * 400L + 97L) //with 97 leap days
#define DAYS_IN_100_YRS	(365L * 100L + 24L) //without last leap day
#define DAYS_IN_4_YRS	(365L * 4L   + 1L)  //with 1 leap day
#define DAYS_IN_YEAR	(365L)				//without leap day

#define MICROSECOND   (1)
#define MILLISECOND   (1000L * MICROSECOND)
#define SECOND        (1000L * MILLISECOND)
#define MINUTE        (60LL * SECOND)
#define HOUR          (60LL * MINUTE)
#define DAY           (24LL * HOUR)

#define MICROS_PER_YEAR			(DAY*365) //Count of microseconds per year
#define MICROS_PER_LEAP_YEAR	(DAY*366) //Count of microseconds per leap year

//Limits
/*
#define MAX_RAW_MILLIS	(185542570511940999LL)
#define MIN_RAW_MILLIS (-185542570598400000LL)

#define MAX_DAYS (2147483454LL)		//Maximum valid days elapsed from year 1
#define MIN_DAYS (-2147483456LL)	//Minimum valid days elapsed from year 1
*/

#define MAX_YEAR ( 292277L)		//Maximum valid year
#define MIN_YEAR (-292277L)		//Minimum valid year

#define MAX_DAYS ( 106751981L)	//Maximum valid days value
#define MIN_DAYS (-106751983L)	//Minimum valid days value


const char* const dt_day_names[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
const char* const dt_month_names[] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };

/**
* @enum WeekOfMonth
* @brief Enumeration of week of month.
* @see DSTTransitionRule
*/
typedef enum {
    First = 0,
    Second = 1,
    Third = 2,
    Fourth = 3,
    Last = 4,
    Fifth = 4
}WeekOfMonth;

/**
* @struct DayOfWeek
* @brief Structure, that acts as day of week enumeration with size of 1 byte.
*/
struct DayOfWeek {
    DayOfWeek() {}

    DayOfWeek(int8_t val) :value(val) {	}

    operator uint8_t() {
        return value;
    }

    operator uint8_t() const {
        return value;
    }

    DayOfWeek& operator=(const uint8_t val) {
        value = val;
        return *this;
    }

    /**
    * @brief Gets pointer to month name or NULL when month has invalid value.
    */
    inline const char* const getName() const {
        uint8_t val = value - 1;
        if (val < 7) {
            return dt_day_names[val];
        }
        return NULL;
    }

    /**
    * @brief Returns true if month is valid.
    */
    inline bool isValid() const {
        return value >= 1 && value <= 7;
    }

    enum Day{
        Unknown = 0,
        Sunday = 1,
        Monday = 2,
        Tuesday = 3,
        Wednesday = 4,
        Thursday = 5,
        Friday = 6,
        Saturday = 7
    };

protected:
    uint8_t value = Unknown;
};

/**
* @struct Month
* @brief Structure, that acts as month enumeration with size of 1 byte.
*/
struct Month {
    Month() {}

    Month(int8_t val) :value(val) {	}

    operator uint8_t() {
        return value;
    }

    operator uint8_t() const {
        return value;
    }

    Month& operator=(const uint8_t val) {
        value = val;
        return *this;
    }

    Month& operator++() {
        ++value;
        return *this;
    }

    Month& operator--() {
        --value;
        return *this;
    }

    Month operator++(int) {
        Month tmp = *this;
        value++;
        return tmp;
    }

    Month operator--(int) {
        Month tmp = *this;
        value--;
        return tmp;
    }

    /**
    * @brief Gets pointer to month name or NULL when month has invalid value.
    */
    inline const char* const getName() const {
        uint8_t val = value - 1;
        if (val < 12) {
            return dt_month_names[val];
        }
        return NULL;
    }

    /**
    * @brief Returns true if month is valid.
    */
    inline bool isValid() const {
        return value >= 1 && value <= 12;
    }

    /**
    * @brief Gets length of current month.
    * @param leapYear True if year contains leap day.
    * @return Returns length of current month.
    */
    uint8_t getMonthLength(bool leapYear) const;

    /**
    * @brief Gets length of month.
    * @param month Month, which length need to be known. Value can be from range 1-12.
    * @return Returns count of days in specified month. Value is in range 1-28/29/30/31;
    */
    static uint8_t getMonthLength(Month month, bool leapYear);

    enum MonthE{
        Unknown = 0,
        January = 1,
        February = 2,
        March = 3,
        April = 4,
        May = 5,
        June = 6,
        July = 7,
        August = 8,
        September = 9,
        October = 10,
        November = 11,
        December = 12
    };

protected:
    uint8_t value = Unknown;
};

struct DateTime_DST_tuple {
    DateTime_DST_tuple(const int64_t& dt_, bool isDST_) :
        value(dt_),
        isDST(isDST_)
    {}

    int64_t value;
    bool isDST;
};

struct year_day_tuple {
    year_day_tuple() {}

    year_day_tuple(int32_t year_, uint16_t dayOfYear_) :
        year(year_),
        dayOfYear(dayOfYear_)
    {}
    int32_t year = 0; //Year range from -5879609 to +5879610 except 0.
    uint16_t dayOfYear = 0; //Day of year from range 0-364 and 0-365 for leap year
};

struct month_day_tuple {
    month_day_tuple(uint8_t month_, uint8_t day_) :
        month(month_),
        day(day_)
    {}
    Month month; //Month in range 1-12
    uint8_t day; //Day of month in range 1-31
};


/**
* @struct hour_t
* @brief Structure, which represents hour field. It supports 24 hour and 12 hour format.
* It can remember it's format. This strucure can act as 6-bit uint8_t variable.
*/
struct hour_t {
    hour_t() :
        hours(0),
        is24_(1),
        isAM_(1)
    {	}

    /**
    * @brief Constructor.
    * @param hours_ Hours value in specified format.
    * @param is24 True if hour format is 24 hour format.
    * @param isAM When 12 hour format is used, this parameter specifies if hour is AM.
    */
    hour_t(uint8_t hours_, bool is24 = true, bool isAM = true) {
        if (is24) setHours24(hours_);
        else setHours12(hours_, isAM);
    }

    /**
    * @brief True if current hour is AM.
    */
    inline bool isAM() {
        return isAM_;
    }

    /**
    * @brief True if current hour is PM.
    */
    inline bool isPM() {
        return !isAM_;
    }

    /**
    * @brief True if current hour format is 24 hour format.
    */
    inline bool is24() {
        return is24_;
    }

    /**
    * @brief True if current hour format is 12 hour format.
    */
    inline bool is12() {
        return !is24_;
    }

    /**
    * @brief Sets hours in 12 hour format. is24() will return false.
    * @param hours_ Hours from 1 to 12.
    */
    inline void setHours12(uint8_t hours_, bool isAM) {
        hours = ((hours_ + 11) % 12) + 1;
        is24_ = false;
        isAM_ = isAM;
    }

    /**
    * @brief Sets hours in 24 hour format. is24() will return true.
    * @param hours_ Hours from 0 to 23.
    */
    inline void setHours24(uint8_t hours_) {
        isAM_ = hours_ < 12;
        hours = hours_ % 24;
        is24_ = true;
    }

    /**
    * @brief Gets hours in specified format.
    * @return Returns hours in specified format.
    */
    inline uint8_t getHours() {
        return hours;
    }

    operator uint8_t() {
        return hours; //Return only hours
    }

    operator uint8_t() const {
        return hours; //Return only hours
    }

    hour_t& operator=(uint8_t val) {
        if (is24_) {
            hours = val % 24;
        }
        else {
            hours = ((hours + 11) % 12) + 1;
        }
        return *this;
    }

    /**
    * @brief Converts to 24 hour format.
    */
    inline void convertTo24() {
        if (!is24_) {
            hours = (hours % 12) + 12 * (!isAM_);
            is24_ = false;
        }
    }

    /**
    * @brief Converts to 12 hour format.
    */
    inline void convertTo12() {
        if (is24_) {
            isAM_ = hours < 12;
            hours = ((hours + 11) % 12) + 1;
            is24_ = true;
        }
    }

private:
    uint8_t hours : 6;
    bool is24_ : 1;		//24 hours format
    bool isAM_ : 1;		//True if hours are AM
};

/**
* @struct date_time_s
* @brief Structure, which contains all date and time fields.
*/
struct date_time_s {
    date_time_s(){}

    date_time_s(int32_t year_, Month month_, uint8_t day_) :
        year(year_),
        month(month_),
        day(day_)
    {}

    date_time_s(int32_t year_, Month month_, uint8_t day_, hour_t hours_, uint8_t minutes_ = 0, uint8_t seconds_ = 0, uint16_t milliseconds_ = 0, uint16_t microseconds_ = 0, DayOfWeek dayOfWeek_ = DayOfWeek::Unknown) :
        year(year_),
        month(month_),
        day(day_),
        hours(hours_),
        minutes(minutes_),
        seconds(seconds_),
        milliseconds(milliseconds_),
        microseconds(microseconds_)
    {}

    int32_t year = 1;
    Month month = 1;
    uint8_t day = 1;
    DayOfWeek dayOfWeek = DayOfWeek::Unknown;

    hour_t hours = hour_t();
    uint8_t minutes = 0;
    uint8_t seconds = 0;
    uint16_t milliseconds = 0;
    uint16_t microseconds = 0;
};

/**
* @struct date_s
* @brief Structure, which contains only date fields.
*/
struct date_s {
    date_s(){}

    date_s(int32_t year_, Month month_, uint8_t day_) :
        year(year_),
        month(month_),
        day(day_)
    {}

    date_s(int32_t year_, Month month_, uint8_t day_, DayOfWeek dayOfWeek_) :
        year(year_),
        month(month_),
        day(day_),
        dayOfWeek(dayOfWeek_)
    {}

    int32_t year = 1;
    Month month = 1;
    uint8_t day = 1;
    DayOfWeek dayOfWeek = DayOfWeek::Unknown;
};

/**
* @struct time_s
* @brief Structure, which contains only time fields.
*/
struct time_s {
    time_s() {}

    time_s(hour_t hours_, uint8_t minutes_, uint8_t seconds_, uint16_t milliseconds_, uint16_t microseconds_) :
        hours(hours_),
        minutes(minutes_),
        seconds(seconds_),
        milliseconds(milliseconds_),
        microseconds(microseconds_)
    {}

    hour_t hours = hour_t();
    uint8_t minutes = 0;
    uint8_t seconds = 0;
    uint16_t milliseconds = 0;
    uint16_t microseconds = 0;
};

namespace dtlib {

    /**
    * @brief Checks if year is leap year.
    * @param year Year to check. Value can be from -5879609 to +5879610 except 0.
    * @return Returns true if year is leap year.
    */
    inline bool isLeapYear(int32_t year) {
        if (year < 0) year++; //skip year 0 because it does not exist
        return (((year % 4) == 0 && ((year % 100) != 0)) || ((year % 400) == 0));
    }

    /**
    * @brief Gets count of days from the 1st Jan 0000 until specified year.
    * @param year Year, until which count of days will be calculated. Year can be in range from -5879609 to +5879610 except 0.
    * @return Returns count of days from the 1st Jan 0000 until specified year.
    */
    int32_t getDaysUntilYear(int32_t year);

    /**
    * @brief Gets year and days of year from elapsed days from first year (1st of January 0001).
    *
    * ## Calculation of days count
    *
    * Leap days are every fourth year, but with one exception.
    * If year is divisible by 100 and not divisible by 400 it is not leap year.
    * For example year 1900 was not leap year, but year 2000 was leap year.<br>
    * <br>
    * **So how to check if year is leap year?**
    * @verbatim
    * isLeap = divisible by 4 **AND** ( not divisible by 100 **OR** divisible by 400 )
    * isLeap = (year % 4 == 0)   &&   (   (year % 100 != 0)    ||   (year % 400 == 0))
    * @endverbatim
    * Now we need to split year to 400 years parts, 100 years parts, 4 years parts and remaining years.
    *
    * @verbatim
    * years400 => count of 400 years parts => has 97 leap years(days) (400/4 = 100; 100 - 3 = 97)
    * years100 => count of 100 years parts => has 24 leap years(days) when not divisibly by 400, else has 25 leap years(days)
    * years4   => count of 4   years parts => has 1 leap year(day)
    * years1   => count of remaining years => has no leap year(day)
    * @endverbatim
    *
    * For example, if we want to know how many days elapsed until year 911, we have to first get count of leap years
    * until first day of this year. Then we have to substract 1, because there is no year 0 and we starts from year 1.
    * Then we need to split year 910 to those parts:
    * @verbatim
    * Year 910 = 2*400 + 1*100 + 2*4 + 2*1
    * |------|---------------------------|---------------------------|-----------------|-----------|-----------|-------|-------|
    * |Years |          400 yrs          |          400 yrs          |     100 yrs     |   4 yrs   |   4 yrs   | 1 yr  | 1 yr  | Here starts year 910
    * |------|---------------------------|---------------------------|-----------------|-----------|-----------|-------|-------|
    * |Days  |      365 * 400 + 97       |      365 * 400 + 97       | 365 * 100 + 24  | 365*4 + 1 | 365*4 + 1 | 365*1 | 365*1 |
    * |------|---------------------------|---------------------------|-----------------|-----------|-----------|-------|-------|
    *
    * years400 = 2
    * years100 = 1
    * years4   = 2
    * years1   = 2
    * @endverbatim
    *
    * Now, if we know count of each part, we can easy calculate count of leaps days by this formula:
    * @verbatim
    * leapDaysCount = years400 * 97 + years100 * 24 + years4 * 1 + years1 * 0;
    * leapDaysCount =        2 * 97 +        1 * 24 +      2 * 1 +      1 * 0 = 220 leap days until year 910
    * @endverbatim
    *
    * We know count of leap days and we can easy calculate count of non leap days:
    * @verbatim
    * nonLeapDays = 910 * 365 = 332150
    * @endverbatim
    * If we calculate 332150 + 220 = 332370, we have count of days, which elapsed from year 1 to first day of year 911.
    *
    * @note Year 1BC (-1) was leap year and next year was 1AD, there were no year 0. That's why day 0 is the 1st of January 0001.
    *
    * ## Calculation of year from days count
    * The whole process can be reversed to calculate year from elapsed days. First, we need to know count of days until nearest year rounded down.
    * So we need to calculate those values: years400, years100, years4, years1. But before calculation, we need to add value 365 to days, because
    * the first year would be marked as leap year, but first leap year is year 4. This is correct only for positive values. To see how it is calculated
    * with negative values, see code.
    * @verbatim
    * days += 365
    *
    * years400 = days / (365 * 400 + 97)
    * remainingDays = days % (365 * 400 + 97)
    *
    * years100 = remainingDays / (365 * 100 + 24)
    * remainingDays = remainingDays % (365 * 100 + 24)
    *
    * years4 = remainingDays / (365 * 4 + 1)
    * remainingDays = remainingDays % (365 * 100 + 1)
    *
    * years1 = remainingDays / 365
    * remainingDays = remainingDays % 365 //This is count of days of year
    *
    * years = years400 * 400 + years10 * 100 + years4 * 4 + years + 1
    * @endverbatim
    * One is added to year because there is no year 0
    *
    * @param days Days elapsed from first year (1st of January 0001).
    * @return Returns year and days of year.
    */
    year_day_tuple getYearFromDays(int32_t days);

    /**
    * @brief Gets month and day of month from day of year. For example 43 => 13th of February.
    * @note This version uses binary search, which is much more faster than linear search using loop.
    * @param day Day of year from range 0-364 and 0-365 for leap year. The 1st of January is day 0.
    * @param leapYear True if year is leap year.
    * @return Returns structure, which contains month from rage 1-12 and day of month from range 1-31/30/28.
    */
    month_day_tuple getMonthFromDayOfYear(uint16_t day, bool leapYear);

    /**
    * @brief Gets day of year of first month day.
    * @param month Month, which day of year need to be known. Value can be from range 1-12.
    * @param leapYear True if year is leap year.
    * @return Returns day of year of first month day. Value can be in range 0-364 and 0-365 for leap years. The 1st of January is day 0.
    */
    uint16_t getDayOfYearFromMonth(Month month, bool leapYear);




    /**
    * @brief Converts raw value to date time structure.
    * @note This function is the fastest way, how to convert raw value to all date and time fields.
    * @param raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @return Returns date time structure.
    */
    date_time_s rawToDateTime(int64_t raw);

    /**
    * @brief Converts date time structure to raw value.
    * @param dateTime Date time structure with valid values. dayOfWeek field is ignored.
    * @return Returns raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    */
    int64_t dateTimeToRaw(date_time_s dateTime);

    /**
    * @brief Converts raw value to date structure.
    * @note This function is the fastest way, how to convert raw value to all date fields.
    * @param raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @return Returns date structure.
    */
    date_s rawToDate(int64_t raw);

    /**
    * @brief Converts date structure to raw value.
    * @note Note that time fields will be restarted to 0.
    * @param date Date structure with valid values. dayOfWeek field is ignored.
    * @return Returns raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    */
    int64_t dateToRaw(date_s date);

    /**
    * @brief Converts date structure to raw value. Time fields are kept.
    * @param date Date structure with valid values. dayOfWeek field is ignored.
    * @return Returns raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    */
    void setDateToRaw(int64_t& raw, date_s date);

    /**
    * @brief Converts raw value to time structure.
    * @note This function is the fastest way, how to convert raw value to all time fields.
    * @param raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @return Returns time structure.
    */
    time_s rawToTime(int64_t raw);

    /**
    * @brief Converts time structure to raw value.
    * @note Note that date fields will be restarted to date 1/1/0001.
    * @param time Time structure with valid values. dayOfWeek field is ignored.
    * @return Returns raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    */
    int64_t timeToRaw(time_s time);

    /**
    * @brief Converts time structure to raw value. Date fields are kept.
    * @param time Time structure with valid values. dayOfWeek field is ignored.
    * @return Returns raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    */
    void setTimeToRaw(int64_t& raw, time_s time);





    /**
    * @brief Gets hours field value from raw value.
    * @note This function should be used only if hours need to be known. If you want to get all time fields, use rawToTime()
    * and if you want to get all date time fields, use rawToDateTime().
    * @param raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @return Returns hours field value in range from 0 to 23.
    */
    hour_t getHoursFromRaw(int64_t raw);

    /**
    * @brief Sets new hours field value to raw value.
    * @note This function should be used only if hours need to be set. If you want to set all time fields, use timeToRaw()
    * and if you want to set all date time fields, use dateTimeToRaw().
    * @param[in,out] raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @param val New value to set. Value can be in range from 0 to 23.
    */
    void setHoursToRaw(int64_t& raw, hour_t val);

    /**
    * @brief Adds hours to raw value.
    * @note This function has super performance, because only basic arithmetic is done here.
    * @param[in,out] raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @param val Value to add. This value can be also negative, which means, that it will be subtracted.
    */
    inline void addHoursToRaw(int64_t& raw, int32_t val) {
        raw += (int64_t)val * HOUR;
    }

    /**
    * @brief Adds hours to raw value.
    * @note This function has super performance, because only basic arithmetic is done here.
    * @param[in,out] raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @param val Value to add. This value can be also negative, which means, that it will be subtracted.
    */
    inline void addHoursToRaw(int64_t& raw, int64_t val) {
        raw += val * HOUR;
    }

    /**
    * @brief Gets minutes field value from raw value.
    * @note This function should be used only if minutes need to be known. If you want to get all time fields, use rawToTime()
    * and if you want to get all date time fields, use rawToDateTime().
    * @param raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @return Returns minutes field value in range from 0 to 59.
    */
    uint8_t getMinutesFromRaw(int64_t raw);

    /**
    * @brief Sets minute field value to raw value.
    * @note This function should be used only if minutes need to be set. If you want to set all time fields, use timeToRaw()
    * and if you want to set all date time fields, use dateTimeToRaw().
    * @param[in,out] raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @param val New value to set. Value can be in range from 0 to 59.
    */
    void setMinutesToRaw(int64_t& raw, uint8_t val);

    /**
    * @brief Adds minutes to raw value.
    * @note This function has super performance, because only basic arithmetic is done here.
    * @param[in,out] raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @param val Value to add. This value can be also negative, which means, that it will be subtracted.
    */
    inline void addMinutesToRaw(int64_t& raw, int32_t val) {
        raw += (int64_t)val * MINUTE;
    }

    /**
    * @brief Adds minutes to raw value.
    * @note This function has super performance, because only basic arithmetic is done here.
    * @param[in,out] raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @param val Value to add. This value can be also negative, which means, that it will be subtracted.
    */
    inline void addMinutesToRaw(int64_t& raw, int64_t val) {
        raw += val * MINUTE;
    }

    /**
    * @brief Gets seconds field value from raw value.
    * @note This function should be used only if seconds need to be known. If you want to get all time fields, use rawToTime()
    * and if you want to get all date time fields, use rawToDateTime().
    * @param raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @return Returns seconds field value in range from 0 to 59.
    */
    uint8_t getSecondsFromRaw(int64_t raw);

    /**
    * @brief Sets seconds field value to raw value.
    * @note This function should be used only if seconds need to be set. If you want to set all time fields, use timeToRaw()
    * and if you want to set all date time fields, use dateTimeToRaw().
    * @param[in,out] raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @param val New value to set. Value can be in range from 0 to 59.
    */
    void setSecondsToRaw(int64_t& raw, uint8_t val);

    /**
    * @brief Adds seconds to raw value.
    * @note This function has super performance, because only basic arithmetic is done here.
    * @param[in,out] raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @param val Value to add. This value can be also negative, which means, that it will be subtracted.
    */
    inline void addSecondsToRaw(int64_t& raw, int32_t val) {
        raw += (int64_t)val * SECOND;
    }

    /**
    * @brief Adds seconds to raw value.
    * @note This function has super performance, because only basic arithmetic is done here.
    * @param[in,out] raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @param val Value to add. This value can be also negative, which means, that it will be subtracted.
    */
    inline void addSecondsToRaw(int64_t& raw, int64_t val) {
        raw += val * SECOND;
    }

    /**
    * @brief Gets milliseconds field value from raw value.
    * @note This function should be used only if milliseconds need to be known. If you want to get all time fields, use rawToTime()
    * and if you want to get all date time fields, use rawToDateTime().
    * @param raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @return Returns milliseconds field value in range from 0 to 999.
    */
    uint16_t getMillisFromRaw(int64_t raw);

    /**
    * @brief Sets milliseconds field value to raw value.
    * @note This function should be used only if milliseconds need to be set. If you want to set all time fields, use timeToRaw()
    * and if you want to set all date time fields, use dateTimeToRaw().
    * @param[in,out] raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @param val New value to set. Value can be in range from 0 to 999.
    */
    void setMillisToRaw(int64_t& raw, uint16_t val);

    /**
    * @brief Adds milliseconds to raw value.
    * @note This function has super performance, because only basic arithmetic is done here.
    * @param[in,out] raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @param val Value to add. This value can be also negative, which means, that it will be subtracted.
    */
    inline void addMillisToRaw(int64_t& raw, int32_t val) {
        raw += (int64_t)val * MILLISECOND;
    }

    /**
    * @brief Adds milliseconds to raw value.
    * @note This function has super performance, because only basic arithmetic is done here.
    * @param[in,out] raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @param val Value to add. This value can be also negative, which means, that it will be subtracted.
    */
    inline void addMillisToRaw(int64_t& raw, int64_t val) {
        raw += val * MILLISECOND;
    }

    /**
    * @brief Gets microseconds field value from raw value.
    * @note This function should be used only if microseconds need to be known. If you want to get all time fields, use rawToTime()
    * and if you want to get all date time fields, use rawToDateTime().
    * @param raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @return Returns microseconds field value in range from 0 to 999.
    */
    uint16_t getMicrosFromRaw(int64_t raw);

    /**
    * @brief Sets microseconds field value to raw value.
    * @note This function should be used only if microseconds need to be set. If you want to set all time fields, use timeToRaw()
    * and if you want to set all date time fields, use dateTimeToRaw().
    * @param[in,out] raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @param val New value to set. Value can be in range from 0 to 999.
    */
    void setMicrosToRaw(int64_t& raw, uint16_t val);

    /**
    * @brief Adds microseconds to raw value.
    * @note This function has super performance, because only basic arithmetic is done here.
    * @param[in,out] raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @param val Value to add. This value can be also negative, which means, that it will be subtracted.
    */
    inline void addMicrosToRaw(int64_t& raw, int32_t val) {
        raw += val;
    }

    /**
    * @brief Adds microseconds to raw value.
    * @note This function has super performance, because only basic arithmetic is done here.
    * @param[in,out] raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @param val Value to add. This value can be also negative, which means, that it will be subtracted.
    */
    inline void addMicrosToRaw(int64_t& raw, int64_t val) {
        raw += val * MICROSECOND;
    }

    /**
    * @brief Gets days, that elapsed from start of the epoch.
    * @param raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @return Returns count of days that elapsed from year 1 (calendar start). Value can be in range from MIN_DAYS to MAX_DAYS. It
    * can be also negative, which means, that current date is B.C.
    */
    int32_t getDaysFromRaw(int64_t raw);

    /**
    * @brief Sets days, that elapsed from start of the epoch..
    * @param raw[in,out] raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @param val Count of days, that elapsed from from year 1 (calendar start). This value can be in range from MIN_DAYS to MAX_DAYS. It
    * can be also negative, which means, that current date is B.C.
    */
    void setDaysToRaw(int64_t& raw, int32_t val);

    /**
    * @brief Adds days to raw value.
    * @note This function has super performance, because only basic arithmetic is done here.
    * @param[in,out] raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @param val Value to add. This value can be also negative, which means, that it will be subtracted.
    */
    inline void addDaysToRaw(int64_t& raw, int32_t val) {
        raw += (int64_t)val * DAY;
    }

    /**
    * @brief Gets count of microseconds, that elapsed since start of current day.
    * @param raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @return Returns count of microseconds, that elapsed since start of current year. This value can be in range from 0 to (DAY - 1).
    */
    int64_t getMicrosOfDayFromRaw(int64_t raw);

    /**
    * @brief Sets count of microseconds, that elapsed since start of current day.
    * @param[in,out] raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @param val Count of microseconds, that elapsed since start of current year. This value can be in range from 0 to (DAY - 1).
    */
    void setMicrosOfDayToRaw(int64_t& raw, int64_t val);

    /**
    * @brief Gets day of month or basically day.
    * @note This function should be used only if only this one parameter need to be get. If you want to get whole date, use rawToDate()
    * which has better performance for getting all 3 parameters. If you want to get whole date and time, use rawToDateTime().
    * which has better performance for getting all date time parameters.
    * @param raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @return Returns day in range from 1 to 28/29/30/31.
    */
    uint8_t getMonthDayFromRaw(int64_t raw);

    /**
    * @brief Sets day of month or basically day.
    * @note This function should be used only if only this one parameter need to be set. If you want to set whole date, use dateToRaw()
    * which has better performance for setting all 3 parameters. If you want to set whole date and time, use dateTimeToRaw().
    * which has better performance for setting all date time parameters.
    * @param[in,out] raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @param val New value of day to be set. Value can be in range from 1 to 28/29/30/31. To get maximum value that can be set,
    * use getMonthLength() function.
    */
    void setMonthDayToRaw(int64_t& raw, uint8_t val);

    /**
    * @brief Gets month.
    * @note This function should be used only if only this one parameter need to be get. If you want to get whole date, use rawToDate()
    * which has better performance for getting all 3 parameters. If you want to get whole date and time, use rawToDateTime().
    * which has better performance for getting all date time parameters
    * @param raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @return Returns month from raw value. Value can be in range from 1 to 12.
    */
    Month getMonthFromRaw(int64_t raw);

    /**
    * @brief Sets month.
    * @note This function should be used only if only this one parameter need to be set. If you want to set whole date, use dateToRaw()
    * which has better performance for setting all 3 parameters. If you want to set whole date and time, use dateTimeToRaw().
    * which has better performance for setting all date time parameters.
    * @param[in,out] raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @param val New value of month to be set. Value can be in range from 1 to 12.
    */
    void setMonthToRaw(int64_t& raw, Month val);

    /**
    * @brief Adds count of months to raw value. This function calculates the resulting month and year, taking into account leap years and
    * the number of days in a month, then adjusts the day part of the result.
    * @note This function has worse performance than addYearsToRaw(), try to avoid it.
    * @param[in,out] raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @param val Value to add. This value can be also negative, which means, that it will be subtracted.
    */
    void addMonthsToRaw(int64_t& raw, int32_t val);

    /**
    * @brief Gets year.
    * @note This function should be used only if only this one parameter need to be get. If you want to get whole date, use rawToDate()
    * which has better performance for getting all 3 parameters. If you want to get whole date and time, use rawToDateTime().
    * which has better performance for getting all date time parameters
    * @param raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @return Returns year from raw value. Value can be in range from MIN_YEAR to MAX_YEAR except 0, because year 0 does not exists. Value
    * can be also negative, which means, that this is year BC.
    */
    inline int32_t getYearFromRaw(int64_t raw) {
        int32_t days = getDaysFromRaw(raw);
        year_day_tuple yd = getYearFromDays(days);
        return yd.year;
    }

    /**
    * @brief Sets year.
    * @note This function should be used only if only this one parameter need to be set. If you want to set whole date, use dateToRaw()
    * which has better performance for setting all 3 parameters. If you want to set whole date and time, use dateTimeToRaw(),
    * which has better performance for setting all date time parameters.
    * @param[in,out] raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @param val New value of year to be set. Value can be in range from MIN_YEAR to MAX_YEAR except 0, because year 0 does not exists. Value
    * can be also negative, which means, that this is year BC.
    */
    void setYearToRaw(int64_t& raw, int32_t val);

    /**
    * @brief Adds amount of years to raw value. Only year field is affected, others stays unchanged.
    * @note This function has slightly worst performance than setYearToRaw().
    * @param[in,out] raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @param val Value to add. This value can be also negative, which means, that it will be subtracted.
    */
    void addYearsToRaw(int64_t& raw, int32_t val);

    /**
    * @brief Gets day of week from raw value.
    * @param raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @return Returns day of week, which is in range from 1 to 7. See enum DayOfWeek.
    */
    inline DayOfWeek getDayOfWeekFromRaw(int64_t raw) {
        int32_t days = getDaysFromRaw(raw);
        if (days < 0) {
            //BC
            return (DayOfWeek)(7 + ((days - 5) % 7));
        }
        else {
            //AD
            return (DayOfWeek)(((days + 1) % 7) + 1);
        }
    }

    /**
    * @brief Gets week of the year.
    * @param raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @param firstDayOfWeek First day of week. In some countries week starts with Sunday, some with Monday.
    * @returns Week of the year from 1 to 52.
    */
    uint8_t getWeekOfYearFromRaw(int64_t raw, DayOfWeek firstDayOfWeek);

    /**
    * @brief Gets week number of the month.
    * Example, when first week day of month is wednesday and *firstDayOfWeek* is sunday (columns: day of month | day of week | week of month):
    * @verbatim
    * 1st wed -> 1
    * 2nd thu -> 1
    * 3rt fri -> 1
    * 4th sat -> 1
    * 5th sun -> 2
    * 5th mon -> 2
    * 5th tue -> 2
    * 5th wed -> 2
    * ...
    * #endverbatim
    * @param raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @param firstDayOfWeek First day of week. In some countries week starts with Sunday, some with Monday.
    * @returns Week of the month from 1(first) to 6(sixth).
    */
    uint8_t getWeekOfMonthFromRaw(int64_t raw, DayOfWeek firstDayOfWeek);

    /**
    * @brief Gets week day of the month. So, it gets count of how many times was same day of week as today in current month.
    * Example, when first week day of month is wednesday (columns: day of month | day of week | week day of month):
    * @verbatim
    * 1st wed -> 1 (first)
    * 2nd thu -> 1 (first)
    * 3rt fri -> 1 (first)
    * 4th sat -> 1 (first)
    * 5th sun -> 1 (first)
    * 5th mon -> 1 (first)
    * 5th tue -> 1 (first)
    * 5th wed -> 2 (second)
    * ...
    * #endverbatim
    * @param raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @returns Week day of the month from 1(first) to 5(fifth).
    */
    uint8_t getWeekDayOfMonthFromRaw(int64_t raw);


    /**
    * @brief Gets count of days, that elapsed from first day in current year.
    * @param raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @return Returns count of days, that elapsed from first day in current year. This value can be in range from 0 to 364 for basic years
    * and in range from 0 to 365 for leap years.
    */
    inline uint16_t getDayOfYearFromRaw(int64_t raw) {
        int32_t days = getDaysFromRaw(raw);
        year_day_tuple yd = getYearFromDays(days);
        return yd.dayOfYear;
    }

    /**
    * @brief Sets count of days, that elapsed from first day in current year.
    * @param[in,out] raw Raw value of microseconds since year 1 (start of calendar). This value can be also negative, which means years BC.
    * @param val Count of days, that elapsed from first day in current year. This value can be in range from 0 to 364 for basic years
    * and in range from 0 to 365 for leap years.
    */
    void setDayOfYearToRaw(int64_t& raw, uint32_t val);

    /**
    * @brief Converts positive integer with 1 or 2 digits to char array.
    * @warning Null terminator is not inserted to the end of string.
    * @param buffer Buffer, where string will be written.
    * @param bufferSize Size of buffer without null terminator.
    * @param val Value to convert from range 0 to 99.
    * @param strictWidth If true, number will always have 0 digits, for example 1 => 01.
    * @return Returns pointer in buffer to new position after last digit.
    */
    char* intToStr2(char* buffer, size_t bufferSize, uint32_t val, bool strictWidth = false);

    /**
    * @brief Converts positive integer to string.
    * @warning Null terminator is not inserted to the end of string.
    * @param buffer Buffer, where string will be written.
    * @param bufferSize Size of buffer without null terminator.
    * @param val Value to convert.
    * @param width Width of final string. Blank space before number is filled with zeros.
    * If set to 0, variable width mode is set.
    * @return Returns pointer in buffer to new position after last digit.
    */
    char* intToStr(char* buffer, size_t bufferSize, uint32_t val, int8_t width = 0);

    /**
    * @brief Converts seconds fraction (which consists of microseconds and milliseconds)
    * to string.
    * @warning Null terminator is not inserted to the end of string.
    * @param buffer Buffer, where string will be written.
    * @param bufferSize Size of buffer without null terminator.
    * @param microseconds Fraction value in microseconds from -999999 to 999999.
    * @param width Width of final string. Value is rounded.
    * @param strictWidth True, to fill with zeros until width parameter.
    * @return Returns pointer in buffer to new position after last digit.
    */
    char* ms_usFractToStr(char* buffer, size_t bufferSize, int32_t microseconds, int8_t width, bool strictWidth);

    /**
    * @brief Converts DateTime to string.
    * @param buffer Buffer, where string will be written.
    * @param bufferSize Size of buffer including null terminator.
    * @param format Custom date and time format.
    * ## Format specifiers
    * The following table describes the custom date and time format specifiers and
    * displays a result string produced by each format specifier.
    * 
    * | Format specifier     | Description                                                                               | Example                                                                                       |
    * |----------------------|-------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------|
    * | "d"                  | The day of the month, from 1 through 31.                                                  | 2009-06-01T13:45:30 -\> 1<br>2009-06-15T13:45:30 -\> 15                                       |
    * | "dd"                 | The day of the month, from 01 through 31.                                                 | 2009-06-01T13:45:30 -\> 01<br>2009-06-15T13:45:30 -\> 15                                      |
    * | "ddd"                | The abbreviated name of the day of the week.                                              | 2009-06-15T13:45:30 -\> Mon                                                                   |
    * | "dddd"               | The full name of the day of the week.                                                     | 2009-06-15T13:45:30 -\> Monday                                                                |
    * | "f"                  | The tenths of a second in a date and time value.                                          | 2009-06-15T13:45:30.6170000 -\> 6<br>2009-06-15T13:45:30.04 -\> 0                             |
    * | "ff"                 | The hundredths of a second in a date and time value.                                      | 2009-06-15T13:45:30.6170000 -\> 62<br>2009-06-15T13:45:30.0040000 -\> 00                      |
    * | "fff"                | The milliseconds in a date and time value.                                                | 2009-06-15T13:45:30.6170000 -\> 617<br>2009-06-15T13:45:30.0004000 -\> 000                    |
    * | "ffff"               | The ten thousandths of a second in a date and time value.                                 | 2009-06-15T13:45:30.6175000 -\> 6175<br>2009-06-15T13:45:30.0000400 -\> 0000                  |
    * | "fffff"              | The hundred thousandths of a second in a date and time value.                             | 2009-06-15T13:45:30.6175400 -\> 61754<br>2009-06-15T13:45:30.0000040 -\> 00000                |
    * | "ffffff"             | The millionths of a second in a date and time value.                                      | 2009-06-15T13:45:30.6175420 -\> 617542<br>2009-06-15T13:45:30.0000004 -\> 000000              |
    * | "F"                  | If non-zero, the tenths of a second in a date and time value.                             | 2009-06-15T13:45:30.6170000 -\> 6<br>2009-06-15T13:45:30.0400000 -\> 0                        |
    * | "FF"                 | If non-zero, the hundredths of a second in a date and time value.                         | 2009-06-15T13:45:30.6170000 -\> 62<br>2009-06-15T13:45:30.0040000 -\> 0                       |
    * | "FFF"                | If non-zero, the milliseconds in a date and time value.                                   | 2009-06-15T13:45:30.6170000 -\> 617<br>2009-06-15T13:45:30.0004000 -\> 0                      |
    * | "FFFF"               | If non-zero, the ten thousandths of a second in a date and time value.                    | 2009-06-15T13:45:30.6175000 -\> 6175<br>2009-06-15T13:45:30.0000400 -\> 0                     |
    * | "FFFFF"              | If non-zero, the hundred thousandths of a second in a date and time value.                | 2009-06-15T13:45:30.6175400 -\> 61754<br>2009-06-15T13:45:30.0000040 -\> 0                    |
    * | "FFFFFF"             | If non-zero, the millionths of a second in a date and time value.                         | 2009-06-15T13:45:30.6175420 -\> 617542<br>2009-06-15T13:45:30.0000004 -\> 0                   |
    * | "g"                  | The period or era. Always shown.                                                          | 2009-06-15T13:45:30.6170000 -\> A.D.<br>2009-06-15T13:45:30.6170000 B.C. -\> B.C.             |
    * | "G"                  | The period or era. Only B.C. is shown.                                                    | 2009-06-15T13:45:30.6170000 -\> (nothing)<br>2009-06-15T13:45:30.6170000 B.C. -\> B.C.        |
    * | "h"                  | The hour, using a 12-hour clock from 1 to 12.                                             | 2009-06-15T01:45:30 -\> 1<br>2009-06-15T13:45:30 -\> 1                                        |
    * | "hh"                 | The hour, using a 12-hour clock from 01 to 12.                                            | 2009-06-15T01:45:30 -\> 01<br>2009-06-15T13:45:30 -\> 01                                      |
    * | "H"                  | The hour, using a 24-hour clock from 0 to 23.                                             | 2009-06-15T01:45:30 -\> 1<br>2009-06-15T13:45:30 -\> 13                                       |
    * | "HH"                 | The hour, using a 24-hour clock from 00 to 23.                                            | 2009-06-15T01:45:30 -\> 01<br>2009-06-15T13:45:30 -\> 13                                      |
    * | "i"                  | The milliseconds, from 0 through 999.                                                     | 2009-06-15T13:45:30.0175420 -\> 17<br>2009-06-15T13:45:30.0005420 -\> 0                       |
    * | "iii"                | The milliseconds, from 000 through 999.                                                   | 2009-06-15T13:45:30.0175420 -\> 017<br>2009-06-15T13:45:30.0005420 -\> 000                    |
    * | "l"                  | DST offset in hours, with no leading zeros.                                               | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +1                                                |
    * | "ll"                 | DST offset in hours, with a leading zero for a single-digit value.                        | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +01                                               |
    * | "lll"                | DST offset in hours and minutes with ':'.                                                 | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +01:00                                            |
    * | "llll"               | DST offset in hours and minutes.                                                          | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +0100                                             |
    * | "n"                  | The period or era represented by sign. Both + and - are shown.                            | 2009-06-15T13:45:30.6170000 -\> +<br>2009-06-15T13:45:30.6170000 B.C. -\> -                   |
    * | "N"                  | The period or era represented by sign. Only minus sign is shown.                          | 2009-06-15T13:45:30.6170000 -\> (nothing)<br>2009-06-15T13:45:30.6170000 B.C. -\> -           |
    * | "m"                  | The minute, from 0 through 59.                                                            | 2009-06-15T01:09:30 -\> 9<br>2009-06-15T13:29:30 -\> 29                                       |
    * | "mm"                 | The minute, from 00 through 59.                                                           | 2009-06-15T01:09:30 -\> 09<br>2009-06-15T13:29:30 -\> 29                                      |
    * | "M"                  | The month, from 1 through 12.                                                             | 2009-06-15T13:45:30 -\> 6                                                                     |
    * | "MM"                 | The month, from 01 through 12.                                                            | 2009-06-15T13:45:30 -\> 06                                                                    |
    * | "MMM"                | The abbreviated name of the month.                                                        | 2009-06-15T13:45:30 -\> Jun                                                                   |
    * | "MMMM"               | The full name of the month.                                                               | 2009-06-15T13:45:30 -\> June                                                                  |
    * | "s"                  | The second, from 0 through 59.                                                            | 2009-06-15T13:45:09 -\> 9                                                                     |
    * | "ss"                 | The second, from 00 through 59.                                                           | 2009-06-15T13:45:09 -\> 09                                                                    |
    * | "t"                  | The first character of the AM/PM designator.                                              | 2009-06-15T13:45:30 -\> P                                                                     |
    * | "tt"                 | The AM/PM designator.                                                                     | 2009-06-15T13:45:30 -\> PM                                                                    |
    * | "u"                  | The microseconds, from 0 through 999.                                                     | 2009-06-15T13:45:30.0170420 -\> 42<br>2009-06-15T13:45:30.6780004 -\> 0                       |
    * | "uuu"                | The microseconds, from 000 through 999.                                                   | 2009-06-15T13:45:30.0170420 -\> 042<br>2009-06-15T13:45:30.6780004 -\> 000                    |
    * | "y"                  | The year, from 0 to 99.                                                                   | 0001-01-01T00:00:00 -\> 1<br>0900-01-01T00:00:00 -\> 0<br>2009-06-15T13:45:30 -\> 9           |
    * | "yy"                 | The year, from 00 to 99.                                                                  | 0001-01-01T00:00:00 -\> 01<br>0900-01-01T00:00:00 -\> 00<br>2009-06-15T13:45:30 -\> 09        |
    * | "yyy"                | The year as a three-digit number.                                                         | 0001-01-01T00:00:00 -\> 001<br>0900-01-01T00:00:00 -\> 900<br>2009-06-15T13:45:30 -\> 009     |
    * | "yyyy"               | The year as a four-digit number.                                                          | 0001-01-01T00:00:00 -\> 0001<br>0900-01-01T00:00:00 -\> 0900<br>2009-06-15T13:45:30 -\> 2009  |
    * | "yyyyy"              | The year as a five-digit number.                                                          | 0001-01-01T00:00:00 -\> 00001<br>2009-06-15T13:45:30 -\> 02009                                |
    * | "z"                  | Time zone offset in hours from UTC, with no leading zeros.                                | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> -7                                                |
    * | "zz"                 | Time zone offset in hours from UTC, with a leading zero for a single-digit value.         | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -07                                                |
    * | "zzz"                | Time zone offset in hours and minutes with ':' from UTC.                                  | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -07:00                                             |
    * | "zzzz"               | Time zone offset in hours and minutes from UTC.                                           | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -0700                                              |
    * | "Z"                  | Time zone and DST offset in hours from UTC, with no leading zeros.                        | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -6                                                 |
    * | "ZZ"                 | Time zone and DST offset in hours from UTC, with a leading zero for a single-digit value. | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -06                                                |
    * | "ZZZ"                | Time zone and DST offset in hours and minutes with ':' from UTC                           | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -06:00                                             |
    * | "ZZZZ"               | Time zone and DST offset in hours and minutes from UTC                                    | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -0600                                              |
    * | "string" or 'string' | Literal string delimiter.                                                                 | 2009-06-15T13:45:30 ("arr:" h:m t) -\> arr: 1:45 P                                            |
    * | "\"                  | The escape character.                                                                     | 2009-06-15T13:45:30 (h \h) -\> 1 h                                                            |
    * | Any other character  | The character is copied to the result string unchanged.                                   | 2009-06-15T01:45:30 (arr hh:mm t) -\> arr 01:45 A                                             |
    * Format specifiers "f", "F", "y" has not limited maximum count, but other specifiers specified in table above has limited maximum counts.
    *
    * @param value Raw value of DateTime in microseconds from start of epoch.
    * @param ds Date structure with calculated fields.
    * @param isDSResolved True, if 'date' parameter has to be used or false when 'value' parameter with raw DateTime value has to be used.
    * @param ts Time structure with calculated fields.
    * @param isTSResolved True, if 'time' parameter has to be used or false when 'value' parameter with raw DateTime value has to be used.
    * @param timeZoneOffset Time zone offset in minutes.
    * @param DSTOffset DST offset in minutes.
    * @param monthNames Array with custom month names. This array has to contain exactly 12 strings. If set to NULL, English names are used.
    * @param weekDayNames Array with custom days of week. This array has to contain exactly 7 strings and first name has to be Sunday, then Monday and so on. If set to NULL, English names are used.
    * @return Returns pointer to buffer, where null terminator was inserted.
    */
    char* dateTimeToArray(char* buffer, size_t bufferSize, const char* format, int64_t value, date_s ds, bool isDSResolved, time_s ts, bool isTSResolved, int16_t timeZoneOffset = 0, int16_t DSTOffset = 0, const char* const* monthNames = NULL, const char* const* weekDayNames = NULL);

    /**
    * @brief Converts DateTime represented by raw value to string.
    * @param buffer Buffer, where string will be written.
    * @param bufferSize Size of buffer including null terminator.
    * @param format Custom date and time format.
    * ## Format specifiers
    * The following table describes the custom date and time format specifiers and
    * displays a result string produced by each format specifier.
    *
    * | Format specifier     | Description                                                                               | Example                                                                                       |
    * |----------------------|-------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------|
    * | "d"                  | The day of the month, from 1 through 31.                                                  | 2009-06-01T13:45:30 -\> 1<br>2009-06-15T13:45:30 -\> 15                                       |
    * | "dd"                 | The day of the month, from 01 through 31.                                                 | 2009-06-01T13:45:30 -\> 01<br>2009-06-15T13:45:30 -\> 15                                      |
    * | "ddd"                | The abbreviated name of the day of the week.                                              | 2009-06-15T13:45:30 -\> Mon                                                                   |
    * | "dddd"               | The full name of the day of the week.                                                     | 2009-06-15T13:45:30 -\> Monday                                                                |
    * | "f"                  | The tenths of a second in a date and time value.                                          | 2009-06-15T13:45:30.6170000 -\> 6<br>2009-06-15T13:45:30.04 -\> 0                             |
    * | "ff"                 | The hundredths of a second in a date and time value.                                      | 2009-06-15T13:45:30.6170000 -\> 62<br>2009-06-15T13:45:30.0040000 -\> 00                      |
    * | "fff"                | The milliseconds in a date and time value.                                                | 2009-06-15T13:45:30.6170000 -\> 617<br>2009-06-15T13:45:30.0004000 -\> 000                    |
    * | "ffff"               | The ten thousandths of a second in a date and time value.                                 | 2009-06-15T13:45:30.6175000 -\> 6175<br>2009-06-15T13:45:30.0000400 -\> 0000                  |
    * | "fffff"              | The hundred thousandths of a second in a date and time value.                             | 2009-06-15T13:45:30.6175400 -\> 61754<br>2009-06-15T13:45:30.0000040 -\> 00000                |
    * | "ffffff"             | The millionths of a second in a date and time value.                                      | 2009-06-15T13:45:30.6175420 -\> 617542<br>2009-06-15T13:45:30.0000004 -\> 000000              |
    * | "F"                  | If non-zero, the tenths of a second in a date and time value.                             | 2009-06-15T13:45:30.6170000 -\> 6<br>2009-06-15T13:45:30.0400000 -\> 0                        |
    * | "FF"                 | If non-zero, the hundredths of a second in a date and time value.                         | 2009-06-15T13:45:30.6170000 -\> 62<br>2009-06-15T13:45:30.0040000 -\> 0                       |
    * | "FFF"                | If non-zero, the milliseconds in a date and time value.                                   | 2009-06-15T13:45:30.6170000 -\> 617<br>2009-06-15T13:45:30.0004000 -\> 0                      |
    * | "FFFF"               | If non-zero, the ten thousandths of a second in a date and time value.                    | 2009-06-15T13:45:30.6175000 -\> 6175<br>2009-06-15T13:45:30.0000400 -\> 0                     |
    * | "FFFFF"              | If non-zero, the hundred thousandths of a second in a date and time value.                | 2009-06-15T13:45:30.6175400 -\> 61754<br>2009-06-15T13:45:30.0000040 -\> 0                    |
    * | "FFFFFF"             | If non-zero, the millionths of a second in a date and time value.                         | 2009-06-15T13:45:30.6175420 -\> 617542<br>2009-06-15T13:45:30.0000004 -\> 0                   |
    * | "g"                  | The period or era. Always shown.                                                          | 2009-06-15T13:45:30.6170000 -\> A.D.<br>2009-06-15T13:45:30.6170000 B.C. -\> B.C.             |
    * | "G"                  | The period or era. Only B.C. is shown.                                                    | 2009-06-15T13:45:30.6170000 -\> (nothing)<br>2009-06-15T13:45:30.6170000 B.C. -\> B.C.        |
    * | "h"                  | The hour, using a 12-hour clock from 1 to 12.                                             | 2009-06-15T01:45:30 -\> 1<br>2009-06-15T13:45:30 -\> 1                                        |
    * | "hh"                 | The hour, using a 12-hour clock from 01 to 12.                                            | 2009-06-15T01:45:30 -\> 01<br>2009-06-15T13:45:30 -\> 01                                      |
    * | "H"                  | The hour, using a 24-hour clock from 0 to 23.                                             | 2009-06-15T01:45:30 -\> 1<br>2009-06-15T13:45:30 -\> 13                                       |
    * | "HH"                 | The hour, using a 24-hour clock from 00 to 23.                                            | 2009-06-15T01:45:30 -\> 01<br>2009-06-15T13:45:30 -\> 13                                      |
    * | "i"                  | The milliseconds, from 0 through 999.                                                     | 2009-06-15T13:45:30.0175420 -\> 17<br>2009-06-15T13:45:30.0005420 -\> 0                       |
    * | "iii"                | The milliseconds, from 000 through 999.                                                   | 2009-06-15T13:45:30.0175420 -\> 017<br>2009-06-15T13:45:30.0005420 -\> 000                    |
    * | "l"                  | DST offset in hours, with no leading zeros.                                               | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +1                                                |
    * | "ll"                 | DST offset in hours, with a leading zero for a single-digit value.                        | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +01                                               |
    * | "lll"                | DST offset in hours and minutes with ':'.                                                 | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +01:00                                            |
    * | "llll"               | DST offset in hours and minutes.                                                          | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +0100                                             |
    * | "n"                  | The period or era represented by sign. Both + and - are shown.                            | 2009-06-15T13:45:30.6170000 -\> +<br>2009-06-15T13:45:30.6170000 B.C. -\> -                   |
    * | "N"                  | The period or era represented by sign. Only minus sign is shown.                          | 2009-06-15T13:45:30.6170000 -\> (nothing)<br>2009-06-15T13:45:30.6170000 B.C. -\> -           |
    * | "m"                  | The minute, from 0 through 59.                                                            | 2009-06-15T01:09:30 -\> 9<br>2009-06-15T13:29:30 -\> 29                                       |
    * | "mm"                 | The minute, from 00 through 59.                                                           | 2009-06-15T01:09:30 -\> 09<br>2009-06-15T13:29:30 -\> 29                                      |
    * | "M"                  | The month, from 1 through 12.                                                             | 2009-06-15T13:45:30 -\> 6                                                                     |
    * | "MM"                 | The month, from 01 through 12.                                                            | 2009-06-15T13:45:30 -\> 06                                                                    |
    * | "MMM"                | The abbreviated name of the month.                                                        | 2009-06-15T13:45:30 -\> Jun                                                                   |
    * | "MMMM"               | The full name of the month.                                                               | 2009-06-15T13:45:30 -\> June                                                                  |
    * | "s"                  | The second, from 0 through 59.                                                            | 2009-06-15T13:45:09 -\> 9                                                                     |
    * | "ss"                 | The second, from 00 through 59.                                                           | 2009-06-15T13:45:09 -\> 09                                                                    |
    * | "t"                  | The first character of the AM/PM designator.                                              | 2009-06-15T13:45:30 -\> P                                                                     |
    * | "tt"                 | The AM/PM designator.                                                                     | 2009-06-15T13:45:30 -\> PM                                                                    |
    * | "u"                  | The microseconds, from 0 through 999.                                                     | 2009-06-15T13:45:30.0170420 -\> 42<br>2009-06-15T13:45:30.6780004 -\> 0                       |
    * | "uuu"                | The microseconds, from 000 through 999.                                                   | 2009-06-15T13:45:30.0170420 -\> 042<br>2009-06-15T13:45:30.6780004 -\> 000                    |
    * | "y"                  | The year, from 0 to 99.                                                                   | 0001-01-01T00:00:00 -\> 1<br>0900-01-01T00:00:00 -\> 0<br>2009-06-15T13:45:30 -\> 9           |
    * | "yy"                 | The year, from 00 to 99.                                                                  | 0001-01-01T00:00:00 -\> 01<br>0900-01-01T00:00:00 -\> 00<br>2009-06-15T13:45:30 -\> 09        |
    * | "yyy"                | The year as a three-digit number.                                                         | 0001-01-01T00:00:00 -\> 001<br>0900-01-01T00:00:00 -\> 900<br>2009-06-15T13:45:30 -\> 009     |
    * | "yyyy"               | The year as a four-digit number.                                                          | 0001-01-01T00:00:00 -\> 0001<br>0900-01-01T00:00:00 -\> 0900<br>2009-06-15T13:45:30 -\> 2009  |
    * | "yyyyy"              | The year as a five-digit number.                                                          | 0001-01-01T00:00:00 -\> 00001<br>2009-06-15T13:45:30 -\> 02009                                |
    * | "z"                  | Time zone offset in hours from UTC, with no leading zeros.                                | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> -7                                                |
    * | "zz"                 | Time zone offset in hours from UTC, with a leading zero for a single-digit value.         | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -07                                                |
    * | "zzz"                | Time zone offset in hours and minutes with ':' from UTC.                                  | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -07:00                                             |
    * | "zzzz"               | Time zone offset in hours and minutes from UTC.                                           | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -0700                                              |
    * | "Z"                  | Time zone and DST offset in hours from UTC, with no leading zeros.                        | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -6                                                 |
    * | "ZZ"                 | Time zone and DST offset in hours from UTC, with a leading zero for a single-digit value. | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -06                                                |
    * | "ZZZ"                | Time zone and DST offset in hours and minutes with ':' from UTC                           | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -06:00                                             |
    * | "ZZZZ"               | Time zone and DST offset in hours and minutes from UTC                                    | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -0600                                              |
    * | "string" or 'string' | Literal string delimiter.                                                                 | 2009-06-15T13:45:30 ("arr:" h:m t) -\> arr: 1:45 P                                            |
    * | "\"                  | The escape character.                                                                     | 2009-06-15T13:45:30 (h \h) -\> 1 h                                                            |
    * | Any other character  | The character is copied to the result string unchanged.                                   | 2009-06-15T01:45:30 (arr hh:mm t) -\> arr 01:45 A                                             |
    * Format specifiers "f", "F", "y" has not limited maximum count, but other specifiers specified in table above has limited maximum counts.
    *
    * @param value Raw value of DateTime in microseconds from start of epoch.
    * @param timeZoneOffset Time zone offset in minutes.
    * @param DSTOffset DST offset in minutes.
    * @param monthNames Array with custom month names. This array has to contain exactly 12 strings. If set to NULL, English names are used.
    * @param weekDayNames Array with custom days of week. This array has to contain exactly 7 strings and first name has to be Sunday, then Monday and so on. If set to NULL, English names are used.
    * @return Returns pointer to buffer, where null terminator was inserted.
    */
    char* dateTimeToArray(char* buffer, size_t bufferSize, const char* format, int64_t value, int16_t timeZoneOffset = 0, int16_t DSTOffset = 0, const char* const* monthNames = NULL, const char* const* weekDayNames = NULL);

    /**
    * @brief Converts DateTime represented by date_s and time_s to string.
    * @param buffer Buffer, where string will be written.
    * @param bufferSize Size of buffer including null terminator.
    * @param format Custom date and time format.
    * ## Format specifiers
    * The following table describes the custom date and time format specifiers and
    * displays a result string produced by each format specifier.
    *
    * | Format specifier     | Description                                                                               | Example                                                                                       |
    * |----------------------|-------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------|
    * | "d"                  | The day of the month, from 1 through 31.                                                  | 2009-06-01T13:45:30 -\> 1<br>2009-06-15T13:45:30 -\> 15                                       |
    * | "dd"                 | The day of the month, from 01 through 31.                                                 | 2009-06-01T13:45:30 -\> 01<br>2009-06-15T13:45:30 -\> 15                                      |
    * | "ddd"                | The abbreviated name of the day of the week.                                              | 2009-06-15T13:45:30 -\> Mon                                                                   |
    * | "dddd"               | The full name of the day of the week.                                                     | 2009-06-15T13:45:30 -\> Monday                                                                |
    * | "f"                  | The tenths of a second in a date and time value.                                          | 2009-06-15T13:45:30.6170000 -\> 6<br>2009-06-15T13:45:30.04 -\> 0                             |
    * | "ff"                 | The hundredths of a second in a date and time value.                                      | 2009-06-15T13:45:30.6170000 -\> 62<br>2009-06-15T13:45:30.0040000 -\> 00                      |
    * | "fff"                | The milliseconds in a date and time value.                                                | 2009-06-15T13:45:30.6170000 -\> 617<br>2009-06-15T13:45:30.0004000 -\> 000                    |
    * | "ffff"               | The ten thousandths of a second in a date and time value.                                 | 2009-06-15T13:45:30.6175000 -\> 6175<br>2009-06-15T13:45:30.0000400 -\> 0000                  |
    * | "fffff"              | The hundred thousandths of a second in a date and time value.                             | 2009-06-15T13:45:30.6175400 -\> 61754<br>2009-06-15T13:45:30.0000040 -\> 00000                |
    * | "ffffff"             | The millionths of a second in a date and time value.                                      | 2009-06-15T13:45:30.6175420 -\> 617542<br>2009-06-15T13:45:30.0000004 -\> 000000              |
    * | "F"                  | If non-zero, the tenths of a second in a date and time value.                             | 2009-06-15T13:45:30.6170000 -\> 6<br>2009-06-15T13:45:30.0400000 -\> 0                        |
    * | "FF"                 | If non-zero, the hundredths of a second in a date and time value.                         | 2009-06-15T13:45:30.6170000 -\> 62<br>2009-06-15T13:45:30.0040000 -\> 0                       |
    * | "FFF"                | If non-zero, the milliseconds in a date and time value.                                   | 2009-06-15T13:45:30.6170000 -\> 617<br>2009-06-15T13:45:30.0004000 -\> 0                      |
    * | "FFFF"               | If non-zero, the ten thousandths of a second in a date and time value.                    | 2009-06-15T13:45:30.6175000 -\> 6175<br>2009-06-15T13:45:30.0000400 -\> 0                     |
    * | "FFFFF"              | If non-zero, the hundred thousandths of a second in a date and time value.                | 2009-06-15T13:45:30.6175400 -\> 61754<br>2009-06-15T13:45:30.0000040 -\> 0                    |
    * | "FFFFFF"             | If non-zero, the millionths of a second in a date and time value.                         | 2009-06-15T13:45:30.6175420 -\> 617542<br>2009-06-15T13:45:30.0000004 -\> 0                   |
    * | "g"                  | The period or era. Always shown.                                                          | 2009-06-15T13:45:30.6170000 -\> A.D.<br>2009-06-15T13:45:30.6170000 B.C. -\> B.C.             |
    * | "G"                  | The period or era. Only B.C. is shown.                                                    | 2009-06-15T13:45:30.6170000 -\> (nothing)<br>2009-06-15T13:45:30.6170000 B.C. -\> B.C.        |
    * | "h"                  | The hour, using a 12-hour clock from 1 to 12.                                             | 2009-06-15T01:45:30 -\> 1<br>2009-06-15T13:45:30 -\> 1                                        |
    * | "hh"                 | The hour, using a 12-hour clock from 01 to 12.                                            | 2009-06-15T01:45:30 -\> 01<br>2009-06-15T13:45:30 -\> 01                                      |
    * | "H"                  | The hour, using a 24-hour clock from 0 to 23.                                             | 2009-06-15T01:45:30 -\> 1<br>2009-06-15T13:45:30 -\> 13                                       |
    * | "HH"                 | The hour, using a 24-hour clock from 00 to 23.                                            | 2009-06-15T01:45:30 -\> 01<br>2009-06-15T13:45:30 -\> 13                                      |
    * | "i"                  | The milliseconds, from 0 through 999.                                                     | 2009-06-15T13:45:30.0175420 -\> 17<br>2009-06-15T13:45:30.0005420 -\> 0                       |
    * | "iii"                | The milliseconds, from 000 through 999.                                                   | 2009-06-15T13:45:30.0175420 -\> 017<br>2009-06-15T13:45:30.0005420 -\> 000                    |
    * | "l"                  | DST offset in hours, with no leading zeros.                                               | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +1                                                |
    * | "ll"                 | DST offset in hours, with a leading zero for a single-digit value.                        | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +01                                               |
    * | "lll"                | DST offset in hours and minutes with ':'.                                                 | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +01:00                                            |
    * | "llll"               | DST offset in hours and minutes.                                                          | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +0100                                             |
    * | "n"                  | The period or era represented by sign. Both + and - are shown.                            | 2009-06-15T13:45:30.6170000 -\> +<br>2009-06-15T13:45:30.6170000 B.C. -\> -                   |
    * | "N"                  | The period or era represented by sign. Only minus sign is shown.                          | 2009-06-15T13:45:30.6170000 -\> (nothing)<br>2009-06-15T13:45:30.6170000 B.C. -\> -           |
    * | "m"                  | The minute, from 0 through 59.                                                            | 2009-06-15T01:09:30 -\> 9<br>2009-06-15T13:29:30 -\> 29                                       |
    * | "mm"                 | The minute, from 00 through 59.                                                           | 2009-06-15T01:09:30 -\> 09<br>2009-06-15T13:29:30 -\> 29                                      |
    * | "M"                  | The month, from 1 through 12.                                                             | 2009-06-15T13:45:30 -\> 6                                                                     |
    * | "MM"                 | The month, from 01 through 12.                                                            | 2009-06-15T13:45:30 -\> 06                                                                    |
    * | "MMM"                | The abbreviated name of the month.                                                        | 2009-06-15T13:45:30 -\> Jun                                                                   |
    * | "MMMM"               | The full name of the month.                                                               | 2009-06-15T13:45:30 -\> June                                                                  |
    * | "s"                  | The second, from 0 through 59.                                                            | 2009-06-15T13:45:09 -\> 9                                                                     |
    * | "ss"                 | The second, from 00 through 59.                                                           | 2009-06-15T13:45:09 -\> 09                                                                    |
    * | "t"                  | The first character of the AM/PM designator.                                              | 2009-06-15T13:45:30 -\> P                                                                     |
    * | "tt"                 | The AM/PM designator.                                                                     | 2009-06-15T13:45:30 -\> PM                                                                    |
    * | "u"                  | The microseconds, from 0 through 999.                                                     | 2009-06-15T13:45:30.0170420 -\> 42<br>2009-06-15T13:45:30.6780004 -\> 0                       |
    * | "uuu"                | The microseconds, from 000 through 999.                                                   | 2009-06-15T13:45:30.0170420 -\> 042<br>2009-06-15T13:45:30.6780004 -\> 000                    |
    * | "y"                  | The year, from 0 to 99.                                                                   | 0001-01-01T00:00:00 -\> 1<br>0900-01-01T00:00:00 -\> 0<br>2009-06-15T13:45:30 -\> 9           |
    * | "yy"                 | The year, from 00 to 99.                                                                  | 0001-01-01T00:00:00 -\> 01<br>0900-01-01T00:00:00 -\> 00<br>2009-06-15T13:45:30 -\> 09        |
    * | "yyy"                | The year as a three-digit number.                                                         | 0001-01-01T00:00:00 -\> 001<br>0900-01-01T00:00:00 -\> 900<br>2009-06-15T13:45:30 -\> 009     |
    * | "yyyy"               | The year as a four-digit number.                                                          | 0001-01-01T00:00:00 -\> 0001<br>0900-01-01T00:00:00 -\> 0900<br>2009-06-15T13:45:30 -\> 2009  |
    * | "yyyyy"              | The year as a five-digit number.                                                          | 0001-01-01T00:00:00 -\> 00001<br>2009-06-15T13:45:30 -\> 02009                                |
    * | "z"                  | Time zone offset in hours from UTC, with no leading zeros.                                | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> -7                                                |
    * | "zz"                 | Time zone offset in hours from UTC, with a leading zero for a single-digit value.         | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -07                                                |
    * | "zzz"                | Time zone offset in hours and minutes with ':' from UTC.                                  | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -07:00                                             |
    * | "zzzz"               | Time zone offset in hours and minutes from UTC.                                           | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -0700                                              |
    * | "Z"                  | Time zone and DST offset in hours from UTC, with no leading zeros.                        | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -6                                                 |
    * | "ZZ"                 | Time zone and DST offset in hours from UTC, with a leading zero for a single-digit value. | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -06                                                |
    * | "ZZZ"                | Time zone and DST offset in hours and minutes with ':' from UTC                           | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -06:00                                             |
    * | "ZZZZ"                | Time zone and DST offset in hours and minutes from UTC                                    | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -0600                                              |
    * | "string" or 'string' | Literal string delimiter.                                                                 | 2009-06-15T13:45:30 ("arr:" h:m t) -\> arr: 1:45 P                                            |
    * | "\"                  | The escape character.                                                                     | 2009-06-15T13:45:30 (h \h) -\> 1 h                                                            |
    * | Any other character  | The character is copied to the result string unchanged.                                   | 2009-06-15T01:45:30 (arr hh:mm t) -\> arr 01:45 A                                             |
    * Format specifiers "f", "F", "y" has not limited maximum count, but other specifiers specified in table above has limited maximum counts.
    *
    * @param date Date structure with calculated fields.
    * @param time Time structure with calculated fields.
    * @param timeZoneOffset Time zone offset in minutes.
    * @param DSTOffset DST offset in minutes.
    * @param monthNames Array with custom month names. This array has to contain exactly 12 strings. If set to NULL, English names are used.
    * @param weekDayNames Array with custom days of week. This array has to contain exactly 7 strings and first name has to be Sunday, then Monday and so on. If set to NULL, English names are used.
    * @return Returns pointer to buffer, where null terminator was inserted.
    */
    char* dateTimeToArray(char* buffer, size_t bufferSize, const char* format, date_s date, time_s time, int16_t timeZoneOffset = 0, int16_t DSTOffset = 0, const char* const* monthNames = NULL, const char* const* weekDayNames = NULL);


    /**
    * @brief Converts DateTime to string.
    * @param format Custom date and time format.
    * ## Format specifiers
    * The following table describes the custom date and time format specifiers and
    * displays a result string produced by each format specifier.
    *
    * | Format specifier     | Description                                                                               | Example                                                                                       |
    * |----------------------|-------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------|
    * | "d"                  | The day of the month, from 1 through 31.                                                  | 2009-06-01T13:45:30 -\> 1<br>2009-06-15T13:45:30 -\> 15                                       |
    * | "dd"                 | The day of the month, from 01 through 31.                                                 | 2009-06-01T13:45:30 -\> 01<br>2009-06-15T13:45:30 -\> 15                                      |
    * | "ddd"                | The abbreviated name of the day of the week.                                              | 2009-06-15T13:45:30 -\> Mon                                                                   |
    * | "dddd"               | The full name of the day of the week.                                                     | 2009-06-15T13:45:30 -\> Monday                                                                |
    * | "f"                  | The tenths of a second in a date and time value.                                          | 2009-06-15T13:45:30.6170000 -\> 6<br>2009-06-15T13:45:30.04 -\> 0                             |
    * | "ff"                 | The hundredths of a second in a date and time value.                                      | 2009-06-15T13:45:30.6170000 -\> 62<br>2009-06-15T13:45:30.0040000 -\> 00                      |
    * | "fff"                | The milliseconds in a date and time value.                                                | 2009-06-15T13:45:30.6170000 -\> 617<br>2009-06-15T13:45:30.0004000 -\> 000                    |
    * | "ffff"               | The ten thousandths of a second in a date and time value.                                 | 2009-06-15T13:45:30.6175000 -\> 6175<br>2009-06-15T13:45:30.0000400 -\> 0000                  |
    * | "fffff"              | The hundred thousandths of a second in a date and time value.                             | 2009-06-15T13:45:30.6175400 -\> 61754<br>2009-06-15T13:45:30.0000040 -\> 00000                |
    * | "ffffff"             | The millionths of a second in a date and time value.                                      | 2009-06-15T13:45:30.6175420 -\> 617542<br>2009-06-15T13:45:30.0000004 -\> 000000              |
    * | "F"                  | If non-zero, the tenths of a second in a date and time value.                             | 2009-06-15T13:45:30.6170000 -\> 6<br>2009-06-15T13:45:30.0400000 -\> 0                        |
    * | "FF"                 | If non-zero, the hundredths of a second in a date and time value.                         | 2009-06-15T13:45:30.6170000 -\> 62<br>2009-06-15T13:45:30.0040000 -\> 0                       |
    * | "FFF"                | If non-zero, the milliseconds in a date and time value.                                   | 2009-06-15T13:45:30.6170000 -\> 617<br>2009-06-15T13:45:30.0004000 -\> 0                      |
    * | "FFFF"               | If non-zero, the ten thousandths of a second in a date and time value.                    | 2009-06-15T13:45:30.6175000 -\> 6175<br>2009-06-15T13:45:30.0000400 -\> 0                     |
    * | "FFFFF"              | If non-zero, the hundred thousandths of a second in a date and time value.                | 2009-06-15T13:45:30.6175400 -\> 61754<br>2009-06-15T13:45:30.0000040 -\> 0                    |
    * | "FFFFFF"             | If non-zero, the millionths of a second in a date and time value.                         | 2009-06-15T13:45:30.6175420 -\> 617542<br>2009-06-15T13:45:30.0000004 -\> 0                   |
    * | "g"                  | The period or era. Always shown.                                                          | 2009-06-15T13:45:30.6170000 -\> A.D.<br>2009-06-15T13:45:30.6170000 B.C. -\> B.C.             |
    * | "G"                  | The period or era. Only B.C. is shown.                                                    | 2009-06-15T13:45:30.6170000 -\> (nothing)<br>2009-06-15T13:45:30.6170000 B.C. -\> B.C.        |
    * | "h"                  | The hour, using a 12-hour clock from 1 to 12.                                             | 2009-06-15T01:45:30 -\> 1<br>2009-06-15T13:45:30 -\> 1                                        |
    * | "hh"                 | The hour, using a 12-hour clock from 01 to 12.                                            | 2009-06-15T01:45:30 -\> 01<br>2009-06-15T13:45:30 -\> 01                                      |
    * | "H"                  | The hour, using a 24-hour clock from 0 to 23.                                             | 2009-06-15T01:45:30 -\> 1<br>2009-06-15T13:45:30 -\> 13                                       |
    * | "HH"                 | The hour, using a 24-hour clock from 00 to 23.                                            | 2009-06-15T01:45:30 -\> 01<br>2009-06-15T13:45:30 -\> 13                                      |
    * | "i"                  | The milliseconds, from 0 through 999.                                                     | 2009-06-15T13:45:30.0175420 -\> 17<br>2009-06-15T13:45:30.0005420 -\> 0                       |
    * | "iii"                | The milliseconds, from 000 through 999.                                                   | 2009-06-15T13:45:30.0175420 -\> 017<br>2009-06-15T13:45:30.0005420 -\> 000                    |
    * | "l"                  | DST offset in hours, with no leading zeros.                                               | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +1                                                |
    * | "ll"                 | DST offset in hours, with a leading zero for a single-digit value.                        | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +01                                               |
    * | "lll"                | DST offset in hours and minutes with ':'.                                                 | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +01:00                                            |
    * | "llll"               | DST offset in hours and minutes.                                                          | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +0100                                             |
    * | "n"                  | The period or era represented by sign. Both + and - are shown.                            | 2009-06-15T13:45:30.6170000 -\> +<br>2009-06-15T13:45:30.6170000 B.C. -\> -                   |
    * | "N"                  | The period or era represented by sign. Only minus sign is shown.                          | 2009-06-15T13:45:30.6170000 -\> (nothing)<br>2009-06-15T13:45:30.6170000 B.C. -\> -           |
    * | "m"                  | The minute, from 0 through 59.                                                            | 2009-06-15T01:09:30 -\> 9<br>2009-06-15T13:29:30 -\> 29                                       |
    * | "mm"                 | The minute, from 00 through 59.                                                           | 2009-06-15T01:09:30 -\> 09<br>2009-06-15T13:29:30 -\> 29                                      |
    * | "M"                  | The month, from 1 through 12.                                                             | 2009-06-15T13:45:30 -\> 6                                                                     |
    * | "MM"                 | The month, from 01 through 12.                                                            | 2009-06-15T13:45:30 -\> 06                                                                    |
    * | "MMM"                | The abbreviated name of the month.                                                        | 2009-06-15T13:45:30 -\> Jun                                                                   |
    * | "MMMM"               | The full name of the month.                                                               | 2009-06-15T13:45:30 -\> June                                                                  |
    * | "s"                  | The second, from 0 through 59.                                                            | 2009-06-15T13:45:09 -\> 9                                                                     |
    * | "ss"                 | The second, from 00 through 59.                                                           | 2009-06-15T13:45:09 -\> 09                                                                    |
    * | "t"                  | The first character of the AM/PM designator.                                              | 2009-06-15T13:45:30 -\> P                                                                     |
    * | "tt"                 | The AM/PM designator.                                                                     | 2009-06-15T13:45:30 -\> PM                                                                    |
    * | "u"                  | The microseconds, from 0 through 999.                                                     | 2009-06-15T13:45:30.0170420 -\> 42<br>2009-06-15T13:45:30.6780004 -\> 0                       |
    * | "uuu"                | The microseconds, from 000 through 999.                                                   | 2009-06-15T13:45:30.0170420 -\> 042<br>2009-06-15T13:45:30.6780004 -\> 000                    |
    * | "y"                  | The year, from 0 to 99.                                                                   | 0001-01-01T00:00:00 -\> 1<br>0900-01-01T00:00:00 -\> 0<br>2009-06-15T13:45:30 -\> 9           |
    * | "yy"                 | The year, from 00 to 99.                                                                  | 0001-01-01T00:00:00 -\> 01<br>0900-01-01T00:00:00 -\> 00<br>2009-06-15T13:45:30 -\> 09        |
    * | "yyy"                | The year as a three-digit number.                                                         | 0001-01-01T00:00:00 -\> 001<br>0900-01-01T00:00:00 -\> 900<br>2009-06-15T13:45:30 -\> 009     |
    * | "yyyy"               | The year as a four-digit number.                                                          | 0001-01-01T00:00:00 -\> 0001<br>0900-01-01T00:00:00 -\> 0900<br>2009-06-15T13:45:30 -\> 2009  |
    * | "yyyyy"              | The year as a five-digit number.                                                          | 0001-01-01T00:00:00 -\> 00001<br>2009-06-15T13:45:30 -\> 02009                                |
    * | "z"                  | Time zone offset in hours from UTC, with no leading zeros.                                | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> -7                                                |
    * | "zz"                 | Time zone offset in hours from UTC, with a leading zero for a single-digit value.         | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -07                                                |
    * | "zzz"                | Time zone offset in hours and minutes with ':' from UTC.                                  | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -07:00                                             |
    * | "zzzz"               | Time zone offset in hours and minutes from UTC.                                           | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -0700                                              |
    * | "Z"                  | Time zone and DST offset in hours from UTC, with no leading zeros.                        | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -6                                                 |
    * | "ZZ"                 | Time zone and DST offset in hours from UTC, with a leading zero for a single-digit value. | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -06                                                |
    * | "ZZZ"                | Time zone and DST offset in hours and minutes with ':' from UTC                           | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -06:00                                             |
    * | "ZZZZ"                | Time zone and DST offset in hours and minutes from UTC                                    | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -0600                                              |
    * | "string" or 'string' | Literal string delimiter.                                                                 | 2009-06-15T13:45:30 ("arr:" h:m t) -\> arr: 1:45 P                                            |
    * | "\"                  | The escape character.                                                                     | 2009-06-15T13:45:30 (h \h) -\> 1 h                                                            |
    * | Any other character  | The character is copied to the result string unchanged.                                   | 2009-06-15T01:45:30 (arr hh:mm t) -\> arr 01:45 A                                             |
    * Format specifiers "f", "F", "y" has not limited maximum count, but other specifiers specified in table above has limited maximum counts.
    *
    * @param value Raw value of DateTime in microseconds from start of epoch.
    * @param ds Date structure with calculated fields.
    * @param isDSResolved True, if 'date' parameter has to be used or false when 'value' parameter with raw DateTime value has to be used.
    * @param ts Time structure with calculated fields.
    * @param isTSResolved True, if 'time' parameter has to be used or false when 'value' parameter with raw DateTime value has to be used.
    * @param timeZoneOffset Time zone offset in minutes.
    * @param DSTOffset DST offset in minutes.
    * @param monthNames Array with custom month names. This array has to contain exactly 12 strings. If set to NULL, English names are used.
    * @param weekDayNames Array with custom days of week. This array has to contain exactly 7 strings and first name has to be Sunday, then Monday and so on. If set to NULL, English names are used.
    * @return Returns converted string.
    */
    #ifdef ARDUINO
    String dateTimeToString(const char* format, int64_t value, date_s ds, bool isDSResolved, time_s ts, bool isTSResolved, int16_t timeZoneOffset = 0, int16_t DSTOffset = 0, const char* const* monthNames = NULL, const char* const* weekDayNames = NULL);
    #else
    std::string dateTimeToString(const char* format, int64_t value, date_s ds, bool isDSResolved, time_s ts, bool isTSResolved, int16_t timeZoneOffset = 0, int16_t DSTOffset = 0, const char* const* monthNames = NULL, const char* const* weekDayNames = NULL);
    #endif // ARDUINO


    /**
    * @brief Converts DateTime represented by raw value to string.
    * @param format Custom date and time format.
    * ## Format specifiers
    * The following table describes the custom date and time format specifiers and
    * displays a result string produced by each format specifier.
    *
    * | Format specifier     | Description                                                                               | Example                                                                                       |
    * |----------------------|-------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------|
    * | "d"                  | The day of the month, from 1 through 31.                                                  | 2009-06-01T13:45:30 -\> 1<br>2009-06-15T13:45:30 -\> 15                                       |
    * | "dd"                 | The day of the month, from 01 through 31.                                                 | 2009-06-01T13:45:30 -\> 01<br>2009-06-15T13:45:30 -\> 15                                      |
    * | "ddd"                | The abbreviated name of the day of the week.                                              | 2009-06-15T13:45:30 -\> Mon                                                                   |
    * | "dddd"               | The full name of the day of the week.                                                     | 2009-06-15T13:45:30 -\> Monday                                                                |
    * | "f"                  | The tenths of a second in a date and time value.                                          | 2009-06-15T13:45:30.6170000 -\> 6<br>2009-06-15T13:45:30.04 -\> 0                             |
    * | "ff"                 | The hundredths of a second in a date and time value.                                      | 2009-06-15T13:45:30.6170000 -\> 62<br>2009-06-15T13:45:30.0040000 -\> 00                      |
    * | "fff"                | The milliseconds in a date and time value.                                                | 2009-06-15T13:45:30.6170000 -\> 617<br>2009-06-15T13:45:30.0004000 -\> 000                    |
    * | "ffff"               | The ten thousandths of a second in a date and time value.                                 | 2009-06-15T13:45:30.6175000 -\> 6175<br>2009-06-15T13:45:30.0000400 -\> 0000                  |
    * | "fffff"              | The hundred thousandths of a second in a date and time value.                             | 2009-06-15T13:45:30.6175400 -\> 61754<br>2009-06-15T13:45:30.0000040 -\> 00000                |
    * | "ffffff"             | The millionths of a second in a date and time value.                                      | 2009-06-15T13:45:30.6175420 -\> 617542<br>2009-06-15T13:45:30.0000004 -\> 000000              |
    * | "F"                  | If non-zero, the tenths of a second in a date and time value.                             | 2009-06-15T13:45:30.6170000 -\> 6<br>2009-06-15T13:45:30.0400000 -\> 0                        |
    * | "FF"                 | If non-zero, the hundredths of a second in a date and time value.                         | 2009-06-15T13:45:30.6170000 -\> 62<br>2009-06-15T13:45:30.0040000 -\> 0                       |
    * | "FFF"                | If non-zero, the milliseconds in a date and time value.                                   | 2009-06-15T13:45:30.6170000 -\> 617<br>2009-06-15T13:45:30.0004000 -\> 0                      |
    * | "FFFF"               | If non-zero, the ten thousandths of a second in a date and time value.                    | 2009-06-15T13:45:30.6175000 -\> 6175<br>2009-06-15T13:45:30.0000400 -\> 0                     |
    * | "FFFFF"              | If non-zero, the hundred thousandths of a second in a date and time value.                | 2009-06-15T13:45:30.6175400 -\> 61754<br>2009-06-15T13:45:30.0000040 -\> 0                    |
    * | "FFFFFF"             | If non-zero, the millionths of a second in a date and time value.                         | 2009-06-15T13:45:30.6175420 -\> 617542<br>2009-06-15T13:45:30.0000004 -\> 0                   |
    * | "g"                  | The period or era. Always shown.                                                          | 2009-06-15T13:45:30.6170000 -\> A.D.<br>2009-06-15T13:45:30.6170000 B.C. -\> B.C.             |
    * | "G"                  | The period or era. Only B.C. is shown.                                                    | 2009-06-15T13:45:30.6170000 -\> (nothing)<br>2009-06-15T13:45:30.6170000 B.C. -\> B.C.        |
    * | "h"                  | The hour, using a 12-hour clock from 1 to 12.                                             | 2009-06-15T01:45:30 -\> 1<br>2009-06-15T13:45:30 -\> 1                                        |
    * | "hh"                 | The hour, using a 12-hour clock from 01 to 12.                                            | 2009-06-15T01:45:30 -\> 01<br>2009-06-15T13:45:30 -\> 01                                      |
    * | "H"                  | The hour, using a 24-hour clock from 0 to 23.                                             | 2009-06-15T01:45:30 -\> 1<br>2009-06-15T13:45:30 -\> 13                                       |
    * | "HH"                 | The hour, using a 24-hour clock from 00 to 23.                                            | 2009-06-15T01:45:30 -\> 01<br>2009-06-15T13:45:30 -\> 13                                      |
    * | "i"                  | The milliseconds, from 0 through 999.                                                     | 2009-06-15T13:45:30.0175420 -\> 17<br>2009-06-15T13:45:30.0005420 -\> 0                       |
    * | "iii"                | The milliseconds, from 000 through 999.                                                   | 2009-06-15T13:45:30.0175420 -\> 017<br>2009-06-15T13:45:30.0005420 -\> 000                    |
    * | "l"                  | DST offset in hours, with no leading zeros.                                               | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +1                                                |
    * | "ll"                 | DST offset in hours, with a leading zero for a single-digit value.                        | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +01                                               |
    * | "lll"                | DST offset in hours and minutes with ':'.                                                 | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +01:00                                            |
    * | "llll"               | DST offset in hours and minutes.                                                          | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +0100                                             |
    * | "n"                  | The period or era represented by sign. Both + and - are shown.                            | 2009-06-15T13:45:30.6170000 -\> +<br>2009-06-15T13:45:30.6170000 B.C. -\> -                   |
    * | "N"                  | The period or era represented by sign. Only minus sign is shown.                          | 2009-06-15T13:45:30.6170000 -\> (nothing)<br>2009-06-15T13:45:30.6170000 B.C. -\> -           |
    * | "m"                  | The minute, from 0 through 59.                                                            | 2009-06-15T01:09:30 -\> 9<br>2009-06-15T13:29:30 -\> 29                                       |
    * | "mm"                 | The minute, from 00 through 59.                                                           | 2009-06-15T01:09:30 -\> 09<br>2009-06-15T13:29:30 -\> 29                                      |
    * | "M"                  | The month, from 1 through 12.                                                             | 2009-06-15T13:45:30 -\> 6                                                                     |
    * | "MM"                 | The month, from 01 through 12.                                                            | 2009-06-15T13:45:30 -\> 06                                                                    |
    * | "MMM"                | The abbreviated name of the month.                                                        | 2009-06-15T13:45:30 -\> Jun                                                                   |
    * | "MMMM"               | The full name of the month.                                                               | 2009-06-15T13:45:30 -\> June                                                                  |
    * | "s"                  | The second, from 0 through 59.                                                            | 2009-06-15T13:45:09 -\> 9                                                                     |
    * | "ss"                 | The second, from 00 through 59.                                                           | 2009-06-15T13:45:09 -\> 09                                                                    |
    * | "t"                  | The first character of the AM/PM designator.                                              | 2009-06-15T13:45:30 -\> P                                                                     |
    * | "tt"                 | The AM/PM designator.                                                                     | 2009-06-15T13:45:30 -\> PM                                                                    |
    * | "u"                  | The microseconds, from 0 through 999.                                                     | 2009-06-15T13:45:30.0170420 -\> 42<br>2009-06-15T13:45:30.6780004 -\> 0                       |
    * | "uuu"                | The microseconds, from 000 through 999.                                                   | 2009-06-15T13:45:30.0170420 -\> 042<br>2009-06-15T13:45:30.6780004 -\> 000                    |
    * | "y"                  | The year, from 0 to 99.                                                                   | 0001-01-01T00:00:00 -\> 1<br>0900-01-01T00:00:00 -\> 0<br>2009-06-15T13:45:30 -\> 9           |
    * | "yy"                 | The year, from 00 to 99.                                                                  | 0001-01-01T00:00:00 -\> 01<br>0900-01-01T00:00:00 -\> 00<br>2009-06-15T13:45:30 -\> 09        |
    * | "yyy"                | The year as a three-digit number.                                                         | 0001-01-01T00:00:00 -\> 001<br>0900-01-01T00:00:00 -\> 900<br>2009-06-15T13:45:30 -\> 009     |
    * | "yyyy"               | The year as a four-digit number.                                                          | 0001-01-01T00:00:00 -\> 0001<br>0900-01-01T00:00:00 -\> 0900<br>2009-06-15T13:45:30 -\> 2009  |
    * | "yyyyy"              | The year as a five-digit number.                                                          | 0001-01-01T00:00:00 -\> 00001<br>2009-06-15T13:45:30 -\> 02009                                |
    * | "z"                  | Time zone offset in hours from UTC, with no leading zeros.                                | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> -7                                                |
    * | "zz"                 | Time zone offset in hours from UTC, with a leading zero for a single-digit value.         | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -07                                                |
    * | "zzz"                | Time zone offset in hours and minutes with ':' from UTC.                                  | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -07:00                                             |
    * | "zzzz"               | Time zone offset in hours and minutes from UTC.                                           | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -0700                                              |
    * | "Z"                  | Time zone and DST offset in hours from UTC, with no leading zeros.                        | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -6                                                 |
    * | "ZZ"                 | Time zone and DST offset in hours from UTC, with a leading zero for a single-digit value. | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -06                                                |
    * | "ZZZ"                | Time zone and DST offset in hours and minutes with ':' from UTC                           | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -06:00                                             |
    * | "ZZZZ"                | Time zone and DST offset in hours and minutes from UTC                                    | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -0600                                              |
    * | "string" or 'string' | Literal string delimiter.                                                                 | 2009-06-15T13:45:30 ("arr:" h:m t) -\> arr: 1:45 P                                            |
    * | "\"                  | The escape character.                                                                     | 2009-06-15T13:45:30 (h \h) -\> 1 h                                                            |
    * | Any other character  | The character is copied to the result string unchanged.                                   | 2009-06-15T01:45:30 (arr hh:mm t) -\> arr 01:45 A                                             |
    * Format specifiers "f", "F", "y" has not limited maximum count, but other specifiers specified in table above has limited maximum counts.
    *
    * @param value Raw value of DateTime in microseconds from start of epoch.
    * @param timeZoneOffset Time zone offset in minutes.
    * @param DSTOffset DST offset in minutes.
    * @param monthNames Array with custom month names. This array has to contain exactly 12 strings. If set to NULL, English names are used.
    * @param weekDayNames Array with custom days of week. This array has to contain exactly 7 strings and first name has to be Sunday, then Monday and so on. If set to NULL, English names are used.
    * @return Returns converted string.
    */
    #ifdef ARDUINO
    String dateTimeToString(const char* format, int64_t value, int16_t timeZoneOffset = 0, int16_t DSTOffset = 0, const char* const* monthNames = NULL, const char* const* weekDayNames = NULL);
    #else
    std::string dateTimeToString(const char* format, int64_t value, int16_t timeZoneOffset = 0, int16_t DSTOffset = 0, const char* const* monthNames = NULL, const char* const* weekDayNames = NULL);
    #endif // ARDUINO

    /**
    * @brief Converts DateTime represented by date_s and time_s to string.
    * @param format Custom date and time format.
    * ## Format specifiers
    * The following table describes the custom date and time format specifiers and
    * displays a result string produced by each format specifier.
    *
    * | Format specifier     | Description                                                                               | Example                                                                                       |
    * |----------------------|-------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------|
    * | "d"                  | The day of the month, from 1 through 31.                                                  | 2009-06-01T13:45:30 -\> 1<br>2009-06-15T13:45:30 -\> 15                                       |
    * | "dd"                 | The day of the month, from 01 through 31.                                                 | 2009-06-01T13:45:30 -\> 01<br>2009-06-15T13:45:30 -\> 15                                      |
    * | "ddd"                | The abbreviated name of the day of the week.                                              | 2009-06-15T13:45:30 -\> Mon                                                                   |
    * | "dddd"               | The full name of the day of the week.                                                     | 2009-06-15T13:45:30 -\> Monday                                                                |
    * | "f"                  | The tenths of a second in a date and time value.                                          | 2009-06-15T13:45:30.6170000 -\> 6<br>2009-06-15T13:45:30.04 -\> 0                             |
    * | "ff"                 | The hundredths of a second in a date and time value.                                      | 2009-06-15T13:45:30.6170000 -\> 62<br>2009-06-15T13:45:30.0040000 -\> 00                      |
    * | "fff"                | The milliseconds in a date and time value.                                                | 2009-06-15T13:45:30.6170000 -\> 617<br>2009-06-15T13:45:30.0004000 -\> 000                    |
    * | "ffff"               | The ten thousandths of a second in a date and time value.                                 | 2009-06-15T13:45:30.6175000 -\> 6175<br>2009-06-15T13:45:30.0000400 -\> 0000                  |
    * | "fffff"              | The hundred thousandths of a second in a date and time value.                             | 2009-06-15T13:45:30.6175400 -\> 61754<br>2009-06-15T13:45:30.0000040 -\> 00000                |
    * | "ffffff"             | The millionths of a second in a date and time value.                                      | 2009-06-15T13:45:30.6175420 -\> 617542<br>2009-06-15T13:45:30.0000004 -\> 000000              |
    * | "F"                  | If non-zero, the tenths of a second in a date and time value.                             | 2009-06-15T13:45:30.6170000 -\> 6<br>2009-06-15T13:45:30.0400000 -\> 0                        |
    * | "FF"                 | If non-zero, the hundredths of a second in a date and time value.                         | 2009-06-15T13:45:30.6170000 -\> 62<br>2009-06-15T13:45:30.0040000 -\> 0                       |
    * | "FFF"                | If non-zero, the milliseconds in a date and time value.                                   | 2009-06-15T13:45:30.6170000 -\> 617<br>2009-06-15T13:45:30.0004000 -\> 0                      |
    * | "FFFF"               | If non-zero, the ten thousandths of a second in a date and time value.                    | 2009-06-15T13:45:30.6175000 -\> 6175<br>2009-06-15T13:45:30.0000400 -\> 0                     |
    * | "FFFFF"              | If non-zero, the hundred thousandths of a second in a date and time value.                | 2009-06-15T13:45:30.6175400 -\> 61754<br>2009-06-15T13:45:30.0000040 -\> 0                    |
    * | "FFFFFF"             | If non-zero, the millionths of a second in a date and time value.                         | 2009-06-15T13:45:30.6175420 -\> 617542<br>2009-06-15T13:45:30.0000004 -\> 0                   |
    * | "g"                  | The period or era. Always shown.                                                          | 2009-06-15T13:45:30.6170000 -\> A.D.<br>2009-06-15T13:45:30.6170000 B.C. -\> B.C.             |
    * | "G"                  | The period or era. Only B.C. is shown.                                                    | 2009-06-15T13:45:30.6170000 -\> (nothing)<br>2009-06-15T13:45:30.6170000 B.C. -\> B.C.        |
    * | "h"                  | The hour, using a 12-hour clock from 1 to 12.                                             | 2009-06-15T01:45:30 -\> 1<br>2009-06-15T13:45:30 -\> 1                                        |
    * | "hh"                 | The hour, using a 12-hour clock from 01 to 12.                                            | 2009-06-15T01:45:30 -\> 01<br>2009-06-15T13:45:30 -\> 01                                      |
    * | "H"                  | The hour, using a 24-hour clock from 0 to 23.                                             | 2009-06-15T01:45:30 -\> 1<br>2009-06-15T13:45:30 -\> 13                                       |
    * | "HH"                 | The hour, using a 24-hour clock from 00 to 23.                                            | 2009-06-15T01:45:30 -\> 01<br>2009-06-15T13:45:30 -\> 13                                      |
    * | "i"                  | The milliseconds, from 0 through 999.                                                     | 2009-06-15T13:45:30.0175420 -\> 17<br>2009-06-15T13:45:30.0005420 -\> 0                       |
    * | "iii"                | The milliseconds, from 000 through 999.                                                   | 2009-06-15T13:45:30.0175420 -\> 017<br>2009-06-15T13:45:30.0005420 -\> 000                    |
    * | "l"                  | DST offset in hours, with no leading zeros.                                               | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +1                                                |
    * | "ll"                 | DST offset in hours, with a leading zero for a single-digit value.                        | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +01                                               |
    * | "lll"                | DST offset in hours and minutes with ':'.                                                 | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +01:00                                            |
    * | "llll"               | DST offset in hours and minutes.                                                          | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +0100                                             |
    * | "n"                  | The period or era represented by sign. Both + and - are shown.                            | 2009-06-15T13:45:30.6170000 -\> +<br>2009-06-15T13:45:30.6170000 B.C. -\> -                   |
    * | "N"                  | The period or era represented by sign. Only minus sign is shown.                          | 2009-06-15T13:45:30.6170000 -\> (nothing)<br>2009-06-15T13:45:30.6170000 B.C. -\> -           |
    * | "m"                  | The minute, from 0 through 59.                                                            | 2009-06-15T01:09:30 -\> 9<br>2009-06-15T13:29:30 -\> 29                                       |
    * | "mm"                 | The minute, from 00 through 59.                                                           | 2009-06-15T01:09:30 -\> 09<br>2009-06-15T13:29:30 -\> 29                                      |
    * | "M"                  | The month, from 1 through 12.                                                             | 2009-06-15T13:45:30 -\> 6                                                                     |
    * | "MM"                 | The month, from 01 through 12.                                                            | 2009-06-15T13:45:30 -\> 06                                                                    |
    * | "MMM"                | The abbreviated name of the month.                                                        | 2009-06-15T13:45:30 -\> Jun                                                                   |
    * | "MMMM"               | The full name of the month.                                                               | 2009-06-15T13:45:30 -\> June                                                                  |
    * | "s"                  | The second, from 0 through 59.                                                            | 2009-06-15T13:45:09 -\> 9                                                                     |
    * | "ss"                 | The second, from 00 through 59.                                                           | 2009-06-15T13:45:09 -\> 09                                                                    |
    * | "t"                  | The first character of the AM/PM designator.                                              | 2009-06-15T13:45:30 -\> P                                                                     |
    * | "tt"                 | The AM/PM designator.                                                                     | 2009-06-15T13:45:30 -\> PM                                                                    |
    * | "u"                  | The microseconds, from 0 through 999.                                                     | 2009-06-15T13:45:30.0170420 -\> 42<br>2009-06-15T13:45:30.6780004 -\> 0                       |
    * | "uuu"                | The microseconds, from 000 through 999.                                                   | 2009-06-15T13:45:30.0170420 -\> 042<br>2009-06-15T13:45:30.6780004 -\> 000                    |
    * | "y"                  | The year, from 0 to 99.                                                                   | 0001-01-01T00:00:00 -\> 1<br>0900-01-01T00:00:00 -\> 0<br>2009-06-15T13:45:30 -\> 9           |
    * | "yy"                 | The year, from 00 to 99.                                                                  | 0001-01-01T00:00:00 -\> 01<br>0900-01-01T00:00:00 -\> 00<br>2009-06-15T13:45:30 -\> 09        |
    * | "yyy"                | The year as a three-digit number.                                                         | 0001-01-01T00:00:00 -\> 001<br>0900-01-01T00:00:00 -\> 900<br>2009-06-15T13:45:30 -\> 009     |
    * | "yyyy"               | The year as a four-digit number.                                                          | 0001-01-01T00:00:00 -\> 0001<br>0900-01-01T00:00:00 -\> 0900<br>2009-06-15T13:45:30 -\> 2009  |
    * | "yyyyy"              | The year as a five-digit number.                                                          | 0001-01-01T00:00:00 -\> 00001<br>2009-06-15T13:45:30 -\> 02009                                |
    * | "z"                  | Time zone offset in hours from UTC, with no leading zeros.                                | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> -7                                                |
    * | "zz"                 | Time zone offset in hours from UTC, with a leading zero for a single-digit value.         | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -07                                                |
    * | "zzz"                | Time zone offset in hours and minutes with ':' from UTC.                                  | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -07:00                                             |
    * | "zzzz"               | Time zone offset in hours and minutes from UTC.                                           | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -0700                                              |
    * | "Z"                  | Time zone and DST offset in hours from UTC, with no leading zeros.                        | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -6                                                 |
    * | "ZZ"                 | Time zone and DST offset in hours from UTC, with a leading zero for a single-digit value. | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -06                                                |
    * | "ZZZ"                | Time zone and DST offset in hours and minutes with ':' from UTC                           | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -06:00                                             |
    * | "ZZZZ"                | Time zone and DST offset in hours and minutes from UTC                                    | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -0600                                              |
    * | "string" or 'string' | Literal string delimiter.                                                                 | 2009-06-15T13:45:30 ("arr:" h:m t) -\> arr: 1:45 P                                            |
    * | "\"                  | The escape character.                                                                     | 2009-06-15T13:45:30 (h \h) -\> 1 h                                                            |
    * | Any other character  | The character is copied to the result string unchanged.                                   | 2009-06-15T01:45:30 (arr hh:mm t) -\> arr 01:45 A                                             |
    * Format specifiers "f", "F", "y" has not limited maximum count, but other specifiers specified in table above has limited maximum counts.
    *
    * @param date Date structure with calculated fields.
    * @param time Time structure with calculated fields.
    * @param timeZoneOffset Time zone offset in minutes.
    * @param DSTOffset DST offset in minutes.
    * @param monthNames Array with custom month names. This array has to contain exactly 12 strings. If set to NULL, English names are used.
    * @param weekDayNames Array with custom days of week. This array has to contain exactly 7 strings and first name has to be Sunday, then Monday and so on. If set to NULL, English names are used.
    * @return Returns converted string.
    */
    #ifdef ARDUINO
    String dateTimeToString(const char* format, date_s date, time_s time, int16_t timeZoneOffset = 0, int16_t DSTOffset = 0, const char* const* monthNames = NULL, const char* const* weekDayNames = NULL);
    #else
    std::string dateTimeToString(const char* format, date_s date, time_s time, int16_t timeZoneOffset = 0, int16_t DSTOffset = 0, const char* const* monthNames = NULL, const char* const* weekDayNames = NULL);
    #endif // ARDUINO

    /**
    * @brief Parses date and time from string.
    * @param buffer Buffer, where DateTime is written in text form.
    * @param bufferSize Size of buffer including null terminator.
    * @param format Custom date and time format of DateTime in text form.
    * ## Format specifiers
    * The following table describes the custom date and time format specifiers and
    * displays a parsed value produced by each format specifier.
    *
    * | Format specifier     | Description                                                                                                                                                    | Example                                                                                                     |
    * |----------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------|
    * | "d"                  | The day of the month, from 1 through 31.                                                                                                                       | 2 -\> 0001-01-02<br>05 -\> 0001-01-05<br>12 -\> 0001-01-12                                                  |
    * | "dd"                 | The day of the month, from 01 through 31. Expected exact two digits.                                                                                           | 2 -\> fail<br>05 -\> 0001-01-05<br>12 -\> 0001-01-12                                                        |
    * | "f"                  | The tenths of a second in a date and time value. Expected exact one digit.                                                                                     | (nothing) -\> fail<br>5 -\> T00:00:00.500000<br>12 -\> fail                                                 |
    * | "ff"                 | The hundredths of a second in a date and time value. Expected exact two digits.                                                                                | (nothing) -\> fail<br>5 -\> fail<br>15 -\> T00:00:00.150000<br>158 -\> fail                                 |
    * | "fff"                | The milliseconds in a date and time value. Expected exact three digits.                                                                                        | (nothing) -\> fail<br>5 -\> fail<br>158 -\> T00:00:00.158000<br>1582 -\> fail                               |
    * | "ffff"               | The ten thousandths of a second in a date and time value. Expected exact four digits.                                                                          | (nothing) -\> fail<br>5 -\> fail<br>1582 -\> T00:00:00.158200<br>15823 -\> fail                             |
    * | "fffff"              | The hundred thousandths of a second in a date and time value. Expected exact five digits.                                                                      | (nothing) -\> fail<br>5 -\> fail<br>15823 -\> T00:00:00.158230<br>158236 -\> fail                           |
    * | "ffffff"             | The millionths of a second in a date and time value. Expected exact six digits.                                                                                | (nothing) -\> fail<br>5 -\> fail<br>158236 -\> T00:00:00.158236<br>1582368 -\> fail                         |
    * | "F"                  | The tenths of a second in a date and time value. Parses exactly one digit, digits count can be greather than zero. Parsed value is not rounded.                | (nothing) -\> fail<br>5 -\> T00:00:00.500000<br>12 -\> T00:00:00.100000                                     |
    * | "FF"                 | The hundredths of a second in a date and time value. Parses exactly two digits, digits count can be greather than zero. Parsed value is not rounded.           | (nothing) -\> fail<br>5 -\> T00:00:00.500000<br>15 -\> T00:00:00.150000<br>158 -\> T00:00:00.150000         |
    * | "FFF"                | The milliseconds in a date and time value. Parses exactly three digits, digits count can be greather than zero. Parsed value is not rounded.                   | (nothing) -\> fail<br>5 -\> T00:00:00.500000<br>158 -\> T00:00:00.158000<br>1582 -\> T00:00:00.158000       |
    * | "FFFF"               | The ten thousandths of a second in a date and time value. Parses exactly four digits, digits count can be greather than zero. Parsed value is not rounded.     | (nothing) -\> fail<br>5 -\> T00:00:00.500000<br>1582 -\> T00:00:00.158200<br>15823 -\> T00:00:00.158200     |
    * | "FFFFF"              | The hundred thousandths of a second in a date and time value. Parses exactly five digits, digits count can be greather than zero. Parsed value is not rounded. | (nothing) -\> fail<br>5 -\> T00:00:00.500000<br>15823 -\> T00:00:00.158230<br>158236 -\> T00:00:00.158230   |
    * | "FFFFFF"             | The millionths of a second in a date and time value. Parses exactly six digits, digits count can be greather than zero. Parsed value is not rounded.           | (nothing) -\> fail<br>5 -\> T00:00:00.500000<br>158236 -\> T00:00:00.158236<br>1582368 -\> T00:00:00.158236 |
    * | "g"                  | The period or era. Accepted values: "B.C.", "b.c.", "A.D.", "a.d.". Era has to be present, else parsing will fail.                                             | B.C. or b.c. -\> B.C.<br>A.D. or a.d. -\> A.D.<br>(nothing or any character except: BbAa) -\> fail          |
    * | "G"                  | The period or era. Accepted values: "B.C.", "b.c.", "A.D.", "a.d.". If era is not presen, A.D. era is assumed.                                                 | B.C. or b.c. -\> B.C.<br>A.D. or a.d. -\> A.D.<br>(nothing or any character except: BbAa) -\> A.D.          |
    * | "h"                  | The hour, using a 12-hour clock from 1 to 12.                                                                                                                  | 2 -\> T02:00:00<br>05 -\> T05:00:00<br>12 -\> T12:00:00                                                     |
    * | "hh"                 | The hour, using a 12-hour clock from 01 to 12. Expected exact two digits.                                                                                      | 2 -\> fail<br>05 -\> T05:00:00<br>12 -\> T12:00:00                                                          |
    * | "H"                  | The hour, using a 24-hour clock from 0 to 23.                                                                                                                  | 2 -\> T02:00:00<br>05 -\> T05:00:00<br>12 -\> T12:00:00                                                     |
    * | "HH"                 | The hour, using a 24-hour clock from 00 to 23. Expected exact two digits.                                                                                      | 2 -\> fail<br>05 -\> T05:00:00<br>12 -\> T12:00:00                                                          |
    * | "i"                  | The milliseconds, from 0 through 999.                                                                                                                          | 2 -\> T00:00:00.002<br>05 -\> T00:00:00.050<br>012 -\> T00:00:00.012                                        |
    * | "iii"                | The milliseconds, from 000 through 999. Expected exact three digits.                                                                                           | 2 -\> fail<br>05 -\> fail<br>012 -\> T00:00:00.012                                                          |
    * | "l"                  | DST offset in hours, with no leading zeros. If 'Z' or 'z' is found, no DST offset is set.                                                                      | 6 -\> +06:00<br>07 -\> +07:00<br>007 -\> fail<br>+6 -\> +06:00<br>-07 -\> -07:00<br>Z or z -\> +00:00       |
    * | "ll"                 | DST offset in hours, with a leading zero for a single-digit value. If 'Z' or 'z' is found, no DST offset is set.                                               | 6 -\> fail<br>07 -\> +07:00<br>007 -\> fail<br>+6 -\> fail<br>-07 -\> -07:00<br>Z or z -\> +00:00           |
    * | "lll"                | DST offset in hours and minutes with ':'. If 'Z' or 'z' is found, no DST offset is set.                                                                        | 0600 -\> fail<br>07:30 -\> +07:30<br>+06:30 -\> +06:30<br>-06:30 -\> -06:30<br>Z or z -\> +00:00            |
    * | "llll"               | DST offset in hours and minutes. If 'Z' or 'z' is found, no DST offset is set.                                                                                 | 06:00 -\> fail<br>0730 -\> +07:30<br>+0630 -\> +06:30<br>-0630 -\> -06:30<br>Z or z -\> +00:00              |
    * | "n"                  | The period or era represented by sign. Accepted values: '+' or '-'. Sign has to be present, else parsing will fail.                                            | - -\> B.C.<br>+ -\> A.D.<br>(nothing or any character except: +-) -\> fail                                  |
    * | "N"                  | The period or era represented by sign. Accepted values: '+' or '-'. If sign is not presen, A.D. era is assumed.                                                | - -\> B.C.<br>+ -\> A.D.<br>(nothing or any character except: +-) -\> A.D.                                  |
    * | "m"                  | The minute, from 0 through 59.                                                                                                                                 | 2 -\> T00:02:00<br>05 -\> T00:05:00<br>12 -\> T00:12:00                                                     |
    * | "mm"                 | The minute, from 00 through 59. Expected exact two digits.                                                                                                     | 2 -\> fail<br>05 -\> T00:05:00<br>12 -\> T00:12:00                                                          |
    * | "M"                  | The month, from 1 through 12.                                                                                                                                  | 2 -\> 0001-02-01<br>05 -\> 0001-05-01<br>12 -\> 0001-12-01                                                  |
    * | "MM"                 | The month, from 01 through 12. Expected exact two digits.                                                                                                      | 2 -\> fail<br>05 -\> 0001-05-01<br>12 -\> 0001-12-01                                                        |
    * | "MMM"                | The abbreviated name of the month. Case insensitive matching of first 3 letters of month.                                                                      | Jun -\> 0001-06-01<br>June -\> fail<br>jun -\> 0001-06-01<br>                                               |
    * | "MMMM"               | The full name of the month. Case insensitive matching of month name.                                                                                           | Jun -\> fail<br>June -\> 0001-06-01<br>june -\> 0001-06-01<br>                                              |
    * | "s"                  | The second, from 0 through 59.                                                                                                                                 | 2 -\> T00:00:02<br>05 -\> T00:00:05<br>12 -\> T00:00:12                                                     |
    * | "ss"                 | The second, from 00 through 59.                                                                                                                                | 2 -\> fail<br>05 -\> T00:00:05<br>12 -\> T00:00:12                                                          |
    * | "t"                  | The first character of the AM/PM designator. Accepted values: 'A', 'a', 'P', 'p'.                                                                              | A or a -\> am<br> P or p -\> pm<br>AM or am -\> fail<br> PM or pm -\> fail<br>                              |
    * | "tt"                 | The AM/PM designator. Accepted values: "AM, "am", "PM, "pm".                                                                                                   | AM or am -\> am<br> PM or pm -\> pm<br>A or a -\> fail<br> P or p -\> fail<br>                              |
    * | "u"                  | The microseconds, from 0 through 999.                                                                                                                          | 2 -\> T00:00:00.000002<br>05 -\> T00:00:00.000050<br>012 -\> T00:00:00.000012                               |
    * | "uuu"                | The microseconds, from 000 through 999. Expected exact three digits.                                                                                           | 2 -\> fail<br>05 -\> fail<br>012 -\> T00:00:00.000012                                                       |
    * | "y"                  | The year, from 1 to 292277. Parses 1 to 6 year digits.                                                                                                         | 12 -\> 0012<br>0012 -\> 0012<br>2022 -\> 2022<br>15646 -\> 15646                                            |
    * | "yy"                 | The year, from 00 to 99. Year 00 is 2000 and year 99 is 2099.                                                                                                  | 12 -\> 2012<br>0012 -\> fail<br>5 -\> fail<br>05 -\> 2005                                                   |
    * | "yyy"                | The year as a three-digit number. Year 000 is 2000 and year 999 is 2999.                                                                                       | 12 -\> fail<br>0012 -\> fail<br>012 -\> 2012                                                                |
    * | "yyyy"               | The year as a four-digit number. Expected exact four digits.                                                                                                   | 12 -\> fail<br>0012 -\> 0012<br>15456 -\> fail<br>2012 -\> 2012                                             |
    * | "yyyyy"              | The year as a five-digit number. Expected exact five digits.                                                                                                   | 12 -\> fail<br>00012 -\> 00012<br>15456 -\> 15456<br>2012 -\> fail                                          |
    * | "yyyyyy"             | The year as a six-digit number. Expected exact six digits.                                                                                                     | 12 -\> fail<br>000012 -\> 000012<br>154568 -\> 154568<br>2012 -\> fail                                      |
    * | "z"                  | Time zone offset in hours from UTC, with no leading zeros. If 'Z' or 'z' is found, UTC time zone is set.                                                       | 6 -\> +06:00<br>07 -\> +07:00<br>007 -\> fail<br>+6 -\> +06:00<br>-07 -\> -07:00<br>Z or z -\> +00:00       |
    * | "zz"                 | Time zone offset in hours from UTC, with a leading zero for a single-digit value. If 'Z' or 'z' is found, UTC time zone is set.                                | 6 -\> fail<br>07 -\> +07:00<br>007 -\> fail<br>+6 -\> fail<br>-07 -\> -07:00<br>Z or z -\> +00:00           |
    * | "zzz"                | Time zone offset in hours and minutes with ':' from UTC. If 'Z' or 'z' is found, UTC time zone is set.                                                         | 0600 -\> fail<br>07:30 -\> +07:30<br>+06:30 -\> +06:30<br>-06:30 -\> -06:30<br>Z or z -\> +00:00            |
    * | "zzzz"               | Time zone offset in hours and minutes from UTC. If 'Z' or 'z' is found, UTC time zone is set.                                                                  | 06:00 -\> fail<br>0730 -\> +07:30<br>+0630 -\> +06:30<br>-0630 -\> -06:30<br>Z or z -\> +00:00              |
    * | "Z"                  | Time zone and DST offset in hours from UTC, with no leading zeros. If 'Z' or 'z' is found, UTC time zone is set.                                               | 6 -\> +06:00<br>07 -\> +07:00<br>007 -\> fail<br>+6 -\> +06:00<br>-07 -\> -07:00<br>Z or z -\> +00:00       |
    * | "ZZ"                 | Time zone and DST offset in hours from UTC, with a leading zero for a single-digit value. If 'Z' or 'z' is found, UTC time zone is set.                        | 6 -\> fail<br>07 -\> +07:00<br>007 -\> fail<br>+6 -\> fail<br>-07 -\> -07:00<br>Z or z -\> +00:00           |
    * | "ZZZ"                | Time zone and DST offset in hours and minutes with ':' from UTC. If 'Z' or 'z' is found, UTC time zone is set.                                                 | 0600 -\> fail<br>07:30 -\> +07:30<br>+06:30 -\> +06:30<br>-06:30 -\> -06:30<br>Z or z -\> +00:00            |
    * | "ZZZZ"                | Time zone and DST offset in hours and minutes from UTC. If 'Z' or 'z' is found, UTC time zone is set.                                                          | 06:00 -\> fail<br>0730 -\> +07:30<br>+0630 -\> +06:30<br>-0630 -\> -06:30<br>Z or z -\> +00:00              |
    * | "string" or 'string' | Literal string delimiter. Text is not parsed, but can be matched, see matchText parameter.                                                                     | "Day 28" : "Day "d -\> pass<br>if matchText == false: "abc 28" : "Day "d -\> pass<br>if matchText == false: "a 28" : "Day "d -\> fail<br>if matchText == true: "abc 28" : "Day "d -\> fail |
    * | "\"                  | The escape character.                                                                                                                                          |                                                                                                             |
    * | Any other character  | Text character is not parsed, but can be matched, see matchText parameter.                                                                                     | "12:30" : HH:mm -\> pass<br>if matchText == false: "12.30" : HH:mm -\> pass<br>if matchText == false: "12::30" : HH:mm -\> fail<br>if matchText == true: "12.30" : HH:mm -\> fail |
    * Format specifiers "f", "F" has not limited maximum count, but other specifiers specified in table above has limited maximum counts.
    *
    * @param parsedValue Parsed fields of DateTime.
    * @param timeZoneOffset Time zone offset in minutes.
    * @param DSTOffset DST offset in minutes.
    * @param TZandDSToffset Time zone and DST offset (sometimes called as UTC offset) in minutes.
    * @param matchText True to exactly match text characters. If set to false, just count and position of text characters is checked.
    * @param monthNames Array with custom month names. This array has to contain exactly 12 strings. If set to NULL, English names are used.
    * @return Returns value greather than 0, when parsing was successful. This value is count of parsed characters from buffer.
    *         Returns negative or 0, when parsing failed. This value is zero based position of character from buffer, where parsing failed.
    */
    int parseDateTime(const char* buffer, int bufferSize, const char* format, date_time_s& parsedValue, int16_t& timeZoneOffset, int16_t& DSTOffset, int16_t& TZandDSToffset, bool matchText = false, const char* const* monthNames = NULL);



#ifdef ARDUINO

    template<class T>
    class DTStaticPrintable : public Printable, public T {
        inline size_t printTo(Print& p) const override {
            return T::printTo(p);
        }
    };

#endif // ARDUINO


} //dtlib namespace




METHOD_CHECKER(has_getRaw, getRaw, int64_t, ());
METHOD_CHECKER(has_getRawNoSync, getRawNoSync, int64_t, ());
METHOD_CHECKER(has_getDateTimeEnh, getDateTimeEnh, DateTime_DST_tuple, ());

METHOD_CHECKER(has_getRawValueDer, getRawValueDer, int64_t, ());
METHOD_CHECKER(has_setRawValueDer, setRawValueDer, void, (1LL));
METHOD_CHECKER(has_addRawValueDer, addRawValueDer, void, (1LL));

METHOD_CHECKER(has_getRawTimeSync, getRawTimeSync, int64_t, ());
METHOD_CHECKER(has_preSetSync, preSetSync, void, ());
METHOD_CHECKER(has_getAndPreSetSync, getAndPreSetSync, int64_t, ());

METHOD_CHECKER(has_setRawTimeTD, setRawTimeTD, void, (1LL));
METHOD_CHECKER(has_addRawTimeTD, addRawTimeTD, void, (1LL));

struct DSTAdjustment;
struct TimeZone;

METHOD_CHECKER(has_getTimeZoneOffsetMinutes, getTimeZoneOffsetMinutes, int16_t, ());
METHOD_CHECKER(has_getCurrentDSTOffsetMinutes, getCurrentDSTOffsetMinutes, int16_t, ());
METHOD_CHECKER(has_getDSTOffsetMinutes, getCurrentDSTOffsetMinutes, int16_t, ());
METHOD_CHECKER(has_getTimeZone, getTimeZone, TimeZone, ());
METHOD_CHECKER(has_getDST, getDST, DSTAdjustment, ());
MTYPE_CHECKER_ANY(has_setTimeZone, setTimeZone);
METHOD_CHECKER(has_isDST, isDST, bool, ());

METHOD_CHECKER(has_getRawSyncTime, getRawSyncTime, DT_SYNC_TYPE, ());

METHOD_CHECKER(has_unappliedOffset, unappliedOffset, int64_t, (1));

#endif // !_DATE_TIME_HELPERS_H
