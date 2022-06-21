/**
 * @file DateTimeTZ.h
 * @brief This file contains base class for DateTime with time zone and DST adjustment.
 *
 * @see DateTimeTZBase
 * @see DateTimeTZ
 *
 * # Credits
 * @author Matej Fitoš
 * @date Jun 4, 2022
 */

#ifndef DATE_TIME_TZ_H
#define DATE_TIME_TZ_H

#include "DateTimeBase.h"

METHOD_CHECKER(has_isSyncDST, isSyncDST, bool, ());
METHOD_CHECKER(has_calcNextTransOnSet, calcNextTransOnSet, void, (1LL, true));
METHOD_CHECKER(has_recalcDST, recalcDST, int64_t, ());

/**
* @class DateTimeTZBase
* @brief Base for all DateTime classes with time zone and DST adjustment,
* which uses raw value (microseconds elapsed from start of the epoch).
* @note This class work only with the Gregorian calendar.
*
* ## How to derive from this class
* Your class can optionally contains those member functions:
* + <b>For time zone and DST adjustment:</b>
*   - <i>bool isSyncDST() const</i> - which checks, if DST adjustment is applyied on synced time,
*   - <i>void calcNextTransOnSet(int64_t newVal, DST_app)</i> - which calculates next DST transition when any time and date field is set,
* Derived class declaration has to be like this:
* @code{.cpp}
* class YourClassName : public DateTimeTZBase<YourClassName>{
*   //Your code and functions goes here
* };
* @endcode
* Or like this, when you want to derive derived class:
* @code{.cpp}
* template<class derivedClass>
* class YourClassName : public DateTimeTZBase<derivedClass>{
*   //Your code and functions goes here
* };
* @endcode
* 
* ## Time zone and operators
* Operators have different behavior, when:
* + DateTimeTZ or DateTimeTZSysSync and DateTimeTZ or DateTimeTZSysSync is compared or subtracted
*    - UTC time is used
* + DateTime without time zone or DST adjustment is compared or subtracted
*    - Local time is used
* 
* @todo Make set member function to accept same parameters as constructor. For example set(date_time_s, TimeZone) is missing.
*
* @see DateTimeRawBase
* @see DateTimeBase
* @see DateTime
* @see DateTimeSysSync
* @see DateTimeTZ
* @see DateTimeTZSysSync
* @see TimeSpan
* @see TimeZone
* @see DSTAdjustment
*/
template <class derivedClass>
class DateTimeTZBase : public DateTimeRawBase<derivedClass>
{
public:

	using DateTimeRawBase<derivedClass>::DateTimeRawBase; //Using base class constructors

	/**
	* @brief Default constructor, which set DateTime to the 0001/01/01 00:00:00.000000 with default time zone and DST value.
	*/
	DateTimeTZBase() : DateTimeRawBase<derivedClass>()
	{ }

#if 1 == 0

	/**
	* @brief Constructor, which sets DateTime from raw value with default time zone and DST value.
	* @param raw Raw value in microseconds from the begin of epoch (0001/1/1).
	* @note Date and time has to be without applyied DST.
	*/
	DateTimeTZBase(int64_t raw) : DateTimeRawBase<derivedClass>(raw)
	{ }
#endif

	/**
	* @brief Constructor, which sets DateTime from raw value with time zone and DST value.
	* @note Raw value is in specific time zone with specific DST.
	* @param raw Raw value in microseconds from the begin of epoch (0001/1/1).
	* @note Date and time has to be without applyied DST.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZBase(int64_t raw, TimeZone tz) :
		tzInfo(tz)
	{
		DateTimeRawBase<derivedClass>::setRawTime(raw);
	}

	/**
	* @brief Constructor, which sets DateTime from raw value with time zone and DST value.
	* @note Raw value is in specific time zone with specific DST.
	* @param raw Raw value in microseconds from the begin of epoch (0001/1/1).
	* @note Date and time has to be without applyied DST.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZBase(int64_t raw, TimeZones::TimeZoneName tz) :
		tzInfo(tz)
	{
		DateTimeRawBase<derivedClass>::setRawTime(raw);
	}

	/**
	* @brief Constructor, which sets DateTime from raw value with time zone and DST value.
	* @note Raw value is in specific time zone with specific DST.
	* @param raw Raw value in microseconds from the begin of epoch (0001/1/1).
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZBase(int64_t raw, TimeZone tz, DSTAdjustment dst, bool DST_app = false) :
		tzInfo(tz),
		adj(dst, DST_app)
	{
		DateTimeRawBase<derivedClass>::setRawTime(raw);
	}

	/**
	* @brief Constructor, which sets DateTime from raw value with time zone and DST value.
	* @note Raw value is in specific time zone with specific DST.
	* @param raw Raw value in microseconds from the begin of epoch (0001/1/1).
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZBase(int64_t raw, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false) :
		tzInfo(tz),
		adj(dst, DST_app)
	{
		DateTimeRawBase<derivedClass>::setRawTime(raw);
	}


#if 1 == 0
	/**
	* @brief Constructor, which sets DateTime from date fields with default time zone and DST value.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @note Date and time has to be without applyied DST.
	*/
	DateTimeTZBase(int32_t year, uint8_t month, uint8_t day) : DateTimeRawBase<derivedClass>(year, month, day)
	{ }
#endif

	/**
	* @brief Constructor, which sets DateTime from date fields with time zone and DST value.
	* @note Date is in specific time zone with specific DST.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @note Date and time has to be without applyied DST.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZBase(int32_t year, uint8_t month, uint8_t day, TimeZone tz) :
		tzInfo(tz)
	{
		DateTimeRawBase<derivedClass>::set(year, month, day);
	}

	/**
	* @brief Constructor, which sets DateTime from date fields with time zone and DST value.
	* @note Date is in specific time zone with specific DST.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @note Date and time has to be without applyied DST.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZBase(int32_t year, uint8_t month, uint8_t day, TimeZones::TimeZoneName tz) :
		tzInfo(tz)
	{
		DateTimeRawBase<derivedClass>::set(year, month, day);
	}

	/**
	* @brief Constructor, which sets DateTime from date fields with time zone and DST value.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZBase(int32_t year, uint8_t month, uint8_t day, TimeZone tz, DSTAdjustment dst, bool DST_app = false) :
		tzInfo(tz),
		adj(dst, DST_app)
	{
		DateTimeRawBase<derivedClass>::set(year, month, day);
	}

	/**
	* @brief Constructor, which sets DateTime from date fields with time zone and DST value.
	* @note Date is in specific time zone with specific DST.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZBase(int32_t year, uint8_t month, uint8_t day, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false) :
		tzInfo(tz),
		adj(dst, DST_app)
	{
		DateTimeRawBase<derivedClass>::set(year, month, day);
	}

#if 1 == 0
	/**
	* @brief Constructor, which sets DateTime from date and time fields with default time zone and DST value.
	* DateTime will be synchronized from this time.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @param hours Hours in 12-hour clock format or 24-hour clock format.
	* @param minutes Minutes from 0 to 59.
	* @param seconds Seconds from 0 to 59.
	* @param milliseconds Milliseconds from 0 to 999.
	* @param microseconds Microseconds from 0 to 999.
	* @note Date and time has to be without applyied DST.
	*/
	DateTimeTZBase(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes = 0, uint8_t seconds = 0, uint16_t milliseconds = 0, uint16_t microseconds = 0) :
		DateTimeRawBase<derivedClass>(year, month, day, hours, minutes, seconds, milliseconds, microseconds)
	{ }
#endif

	/**
	* @brief Constructor, which sets DateTime from date and time fields with time zone and DST value.
	* @note Date and time is in specific time zone with specific DST.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @param hours Hours in 12-hour clock format or 24-hour clock format.
	* @note Date and time has to be without applyied DST.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZBase(int32_t year, uint8_t month, uint8_t day, hour_t hours, TimeZone tz) :
		tzInfo(tz)
	{
		DateTimeRawBase<derivedClass>::set(year, month, day, hours);
	}

	/**
	* @brief Constructor, which sets DateTime from date and time fields with time zone and DST value.
	* @note Date and time is in specific time zone with specific DST.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @param hours Hours in 12-hour clock format or 24-hour clock format.
	* @note Date and time has to be without applyied DST.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZBase(int32_t year, uint8_t month, uint8_t day, hour_t hours, TimeZones::TimeZoneName tz) :
		tzInfo(tz)
	{
		DateTimeRawBase<derivedClass>::set(year, month, day, hours);
	}

	/**
	* @brief Constructor, which sets DateTime from date and time fields with time zone and DST value.
	* @note Date and time is in specific time zone with specific DST.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @param hours Hours in 12-hour clock format or 24-hour clock format.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZBase(int32_t year, uint8_t month, uint8_t day, hour_t hours, TimeZone tz, DSTAdjustment dst, bool DST_app = false) :
		tzInfo(tz),
		adj(dst, DST_app)
	{
		DateTimeRawBase<derivedClass>::set(year, month, day, hours);
	}

	/**
	* @brief Constructor, which sets DateTime from date and time fields with time zone and DST value.
	* @note Date and time is in specific time zone with specific DST.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @param hours Hours in 12-hour clock format or 24-hour clock format.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZBase(int32_t year, uint8_t month, uint8_t day, hour_t hours, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false) :
		tzInfo(tz),
		adj(dst, DST_app)
	{
		DateTimeRawBase<derivedClass>::set(year, month, day, hours);
	}

	/**
	* @brief Constructor, which sets DateTime from date and time fields with time zone and DST value.
	* @note Date and time is in specific time zone with specific DST.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @param hours Hours in 12-hour clock format or 24-hour clock format.
	* @param minutes Minutes from 0 to 59.
	* @note Date and time has to be without applyied DST.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZBase(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, TimeZone tz) :
		tzInfo(tz)
	{
		DateTimeRawBase<derivedClass>::set(year, month, day, hours, minutes);
	}

	/**
	* @brief Constructor, which sets DateTime from date and time fields with time zone and DST value.
	* @note Date and time is in specific time zone with specific DST.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @param hours Hours in 12-hour clock format or 24-hour clock format.
	* @param minutes Minutes from 0 to 59.
	* @note Date and time has to be without applyied DST.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZBase(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, TimeZones::TimeZoneName tz) :
		tzInfo(tz)
	{
		DateTimeRawBase<derivedClass>::set(year, month, day, hours, minutes);
	}

	/**
	* @brief Constructor, which sets DateTime from date and time fields with time zone and DST value.
	* @note Date and time is in specific time zone with specific DST.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @param hours Hours in 12-hour clock format or 24-hour clock format.
	* @param minutes Minutes from 0 to 59.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZBase(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, TimeZone tz, DSTAdjustment dst, bool DST_app = false) :
		tzInfo(tz),
		adj(dst, DST_app)
	{
		DateTimeRawBase<derivedClass>::set(year, month, day, hours, minutes);
	}

	/**
	* @brief Constructor, which sets DateTime from date and time fields with time zone and DST value.
	* @note Date and time is in specific time zone with specific DST.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @param hours Hours in 12-hour clock format or 24-hour clock format.
	* @param minutes Minutes from 0 to 59.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZBase(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false) :
		tzInfo(tz),
		adj(dst, DST_app)
	{
		DateTimeRawBase<derivedClass>::set(year, month, day, hours, minutes);
	}

	/**
	* @brief Constructor, which sets DateTime from date and time fields with time zone and DST value.
	* @note Date and time is in specific time zone with specific DST.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @param hours Hours in 12-hour clock format or 24-hour clock format.
	* @param minutes Minutes from 0 to 59.
	* @param seconds Seconds from 0 to 59.
	* @note Date and time has to be without applyied DST.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZBase(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, uint8_t seconds, TimeZone tz) :
		tzInfo(tz)
	{
		DateTimeRawBase<derivedClass>::set(year, month, day, hours, minutes, seconds);
	}

	/**
	* @brief Constructor, which sets DateTime from date and time fields with time zone and DST value.
	* @note Date and time is in specific time zone with specific DST.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @param hours Hours in 12-hour clock format or 24-hour clock format.
	* @param minutes Minutes from 0 to 59.
	* @param seconds Seconds from 0 to 59.
	* @note Date and time has to be without applyied DST.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZBase(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, uint8_t seconds, TimeZones::TimeZoneName tz) :
		tzInfo(tz)
	{
		DateTimeRawBase<derivedClass>::set(year, month, day, hours, minutes, seconds);
	}

	/**
	* @brief Constructor, which sets DateTime from date and time fields with time zone and DST value.
	* @note Date and time is in specific time zone with specific DST.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @param hours Hours in 12-hour clock format or 24-hour clock format.
	* @param minutes Minutes from 0 to 59.
	* @param seconds Seconds from 0 to 59.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZBase(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, uint8_t seconds, TimeZone tz, DSTAdjustment dst, bool DST_app = false) :
		tzInfo(tz),
		adj(dst, DST_app)
	{
		DateTimeRawBase<derivedClass>::set(year, month, day, hours, minutes, seconds);
	}

	/**
	* @brief Constructor, which sets DateTime from date and time fields with time zone and DST value.
	* @note Date and time is in specific time zone with specific DST.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @param hours Hours in 12-hour clock format or 24-hour clock format.
	* @param minutes Minutes from 0 to 59.
	* @param seconds Seconds from 0 to 59.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZBase(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, uint8_t seconds, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false) :
		tzInfo(tz),
		adj(dst, DST_app)
	{
		DateTimeRawBase<derivedClass>::set(year, month, day, hours, minutes, seconds);
	}

	/**
	* @brief Constructor, which sets DateTime from date and time fields with time zone and DST value.
	* @note Date and time is in specific time zone with specific DST.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @param hours Hours in 12-hour clock format or 24-hour clock format.
	* @param minutes Minutes from 0 to 59.
	* @param seconds Seconds from 0 to 59.
	* @param milliseconds Milliseconds from 0 to 999.
	* @note Date and time has to be without applyied DST.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZBase(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds, TimeZone tz) :
		tzInfo(tz)
	{
		DateTimeRawBase<derivedClass>::set(year, month, day, hours, minutes, seconds, milliseconds);
	}

	/**
	* @brief Constructor, which sets DateTime from date and time fields with time zone and DST value.
	* @note Date and time is in specific time zone with specific DST.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @param hours Hours in 12-hour clock format or 24-hour clock format.
	* @param minutes Minutes from 0 to 59.
	* @param seconds Seconds from 0 to 59.
	* @param milliseconds Milliseconds from 0 to 999.
	* @note Date and time has to be without applyied DST.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZBase(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds, TimeZones::TimeZoneName tz) :
		tzInfo(tz)
	{
		DateTimeRawBase<derivedClass>::set(year, month, day, hours, minutes, seconds, milliseconds);
	}

	/**
	* @brief Constructor, which sets DateTime from date and time fields with time zone and DST value.
	* @note Date and time is in specific time zone with specific DST.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @param hours Hours in 12-hour clock format or 24-hour clock format.
	* @param minutes Minutes from 0 to 59.
	* @param seconds Seconds from 0 to 59.
	* @param milliseconds Milliseconds from 0 to 999.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZBase(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds, TimeZone tz, DSTAdjustment dst, bool DST_app = false) :
		tzInfo(tz),
		adj(dst, DST_app)
	{
		DateTimeRawBase<derivedClass>::set(year, month, day, hours, minutes, seconds, milliseconds);
	}

	/**
	* @brief Constructor, which sets DateTime from date and time fields with time zone and DST value.
	* @note Date and time is in specific time zone with specific DST.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @param hours Hours in 12-hour clock format or 24-hour clock format.
	* @param minutes Minutes from 0 to 59.
	* @param seconds Seconds from 0 to 59.
	* @param milliseconds Milliseconds from 0 to 999.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZBase(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false) :
		tzInfo(tz),
		adj(dst, DST_app)
	{
		DateTimeRawBase<derivedClass>::set(year, month, day, hours, minutes, seconds, milliseconds);
	}

	/**
	* @brief Constructor, which sets DateTime from date and time fields with time zone and DST value.
	* @note Date and time is in specific time zone with specific DST.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @param hours Hours in 12-hour clock format or 24-hour clock format.
	* @param minutes Minutes from 0 to 59.
	* @param seconds Seconds from 0 to 59.
	* @param milliseconds Milliseconds from 0 to 999.
	* @param microseconds Microseconds from 0 to 999.
	* @note Date and time has to be without applyied DST.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZBase(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds, uint16_t microseconds, TimeZone tz) :
		tzInfo(tz)
	{
		DateTimeRawBase<derivedClass>::set(year, month, day, hours, minutes, seconds, milliseconds, microseconds);
	}

	/**
	* @brief Constructor, which sets DateTime from date and time fields with time zone and DST value.
	* @note Date and time is in specific time zone with specific DST.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @param hours Hours in 12-hour clock format or 24-hour clock format.
	* @param minutes Minutes from 0 to 59.
	* @param seconds Seconds from 0 to 59.
	* @param milliseconds Milliseconds from 0 to 999.
	* @param microseconds Microseconds from 0 to 999.
	* @note Date and time has to be without applyied DST.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZBase(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds, uint16_t microseconds, TimeZones::TimeZoneName tz) :
		tzInfo(tz)
	{
		DateTimeRawBase<derivedClass>::set(year, month, day, hours, minutes, seconds, milliseconds, microseconds);
	}

	/**
	* @brief Constructor, which sets DateTime from date and time fields with time zone and DST value.
	* @note Date and time is in specific time zone with specific DST.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @param hours Hours in 12-hour clock format or 24-hour clock format.
	* @param minutes Minutes from 0 to 59.
	* @param seconds Seconds from 0 to 59.
	* @param milliseconds Milliseconds from 0 to 999.
	* @param microseconds Microseconds from 0 to 999.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZBase(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds, uint16_t microseconds, TimeZone tz, DSTAdjustment dst, bool DST_app = false) :
		tzInfo(tz),
		adj(dst, DST_app)
	{
		DateTimeRawBase<derivedClass>::set(year, month, day, hours, minutes, seconds, milliseconds, microseconds);
	}

	/**
	* @brief Constructor, which sets DateTime from date and time fields with time zone and DST value.
	* @note Date and time is in specific time zone with specific DST.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @param hours Hours in 12-hour clock format or 24-hour clock format.
	* @param minutes Minutes from 0 to 59.
	* @param seconds Seconds from 0 to 59.
	* @param milliseconds Milliseconds from 0 to 999.
	* @param microseconds Microseconds from 0 to 999.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZBase(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds, uint16_t microseconds, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false) :
		tzInfo(tz),
		adj(dst, DST_app)
	{
		DateTimeRawBase<derivedClass>::set(year, month, day, hours, minutes, seconds, milliseconds, microseconds);
	}

	/**
	* @brief Constructor, which creates DateTime from tm.
	* @note Date and time is in specific time zone with specific DST.
    * @param dt_s Date and time specified by tm structure.
	*/
	DateTimeTZBase(tm dt_s) : DateTimeRawBase<derivedClass>(dt_s)
	{}

	/**
	* @brief Constructor, which creates DateTime from tm.
	* @note Date and time is in specific time zone with specific DST.
    * @param dt_s Date and time specified by tm structure.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZBase(tm dt_s, TimeZones::TimeZoneName tz) :
		DateTimeRawBase<derivedClass>(dt_s),
		tzInfo(tz)
	{}

	/**
	* @brief Constructor, which creates DateTime from tm.
	* @note Date and time is in specific time zone with specific DST.
    * @param dt_s Date and time specified by tm structure.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZBase(tm dt_s, TimeZone tz) :
		DateTimeRawBase<derivedClass>(dt_s),
		tzInfo(tz)
	{}

	/**
	* @brief Constructor, which creates DateTime from tm.
	* @note Date and time is in specific time zone with specific DST.
    * @param dt_s Date and time specified by tm structure.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	*/
	DateTimeTZBase(tm dt_s, TimeZones::TimeZoneName tz, DSTAdjustment dst) :
		DateTimeRawBase<derivedClass>(dt_s),
		tzInfo(tz),
		adj(dst, dt_s.tm_isdst)
	{}

	/**
	* @brief Constructor, which creates DateTime from tm.
	* @note Date and time is in specific time zone with specific DST.
    * @param dt_s Date and time specified by tm structure.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	*/
	DateTimeTZBase(tm dt_s, TimeZone tz, DSTAdjustment dst) :
		DateTimeRawBase<derivedClass>(dt_s),
		tzInfo(tz),
		adj(dst, dt_s.tm_isdst)
	{}



	/**
	* @brief Constructor, which creates DateTime from date_time_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param dt_s date_time_s structure with date and time fields.
	*/
	DateTimeTZBase(date_time_s dt_s) : DateTimeRawBase<derivedClass>(dt_s)
	{}

	/**
	* @brief Constructor, which creates DateTime from date_time_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param dt_s date_time_s structure with date and time fields.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZBase(date_time_s dt_s, TimeZones::TimeZoneName tz) :
		DateTimeRawBase<derivedClass>(dt_s),
		tzInfo(tz)
	{}

	/**
	* @brief Constructor, which creates DateTime from date_time_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param dt_s date_time_s structure with date and time fields.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZBase(date_time_s dt_s, TimeZone tz) :
		DateTimeRawBase<derivedClass>(dt_s),
		tzInfo(tz)
	{}

	/**
	* @brief Constructor, which creates DateTime from date_time_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param dt_s date_time_s structure with date and time fields.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZBase(date_time_s dt_s, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false) :
		DateTimeRawBase<derivedClass>(dt_s),
		tzInfo(tz),
		adj(dst, DST_app)
	{}

	/**
	* @brief Constructor, which creates DateTime from date_time_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param dt_s date_time_s structure with date and time fields.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZBase(date_time_s dt_s, TimeZone tz, DSTAdjustment dst, bool DST_app = false) :
		DateTimeRawBase<derivedClass>(dt_s),
		tzInfo(tz),
		adj(dst, DST_app)
	{}

	/**
	* @brief Constructor, which creates DateTime from time_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param t_s time_s structure with time fields.
	*/
	DateTimeTZBase(time_s t_s) : DateTimeRawBase<derivedClass>(t_s)
	{}

	/**
	* @brief Constructor, which creates DateTime from time_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param t_s time_s structure with time fields.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZBase(time_s t_s, TimeZones::TimeZoneName tz) :
		DateTimeRawBase<derivedClass>(t_s),
		tzInfo(tz)
	{}

	/**
	* @brief Constructor, which creates DateTime from time_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param t_s time_s structure with time fields.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZBase(time_s t_s, TimeZone tz) :
		DateTimeRawBase<derivedClass>(t_s),
		tzInfo(tz)
	{}

	/**
	* @brief Constructor, which creates DateTime from time_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param t_s time_s structure with time fields.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZBase(time_s t_s, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false) :
		DateTimeRawBase<derivedClass>(t_s),
		tzInfo(tz),
		adj(dst, DST_app)
	{}

	/**
	* @brief Constructor, which creates DateTime from time_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param t_s time_s structure with time fields.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZBase(time_s t_s, TimeZone tz, DSTAdjustment dst, bool DST_app = false) :
		DateTimeRawBase<derivedClass>(t_s),
		tzInfo(tz),
		adj(dst, DST_app)
	{}

	/**
	* @brief Constructor, which creates DateTime from date_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param d_s date_s structure with date fields.
	*/
	DateTimeTZBase(date_s d_s) : DateTimeRawBase<derivedClass>(d_s)
	{}

	/**
	* @brief Constructor, which creates DateTime from date_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param d_s date_s structure with date fields.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZBase(date_s d_s, TimeZones::TimeZoneName tz) :
		DateTimeRawBase<derivedClass>(d_s),
		tzInfo(tz)
	{}

	/**
	* @brief Constructor, which creates DateTime from date_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param d_s date_s structure with date fields.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZBase(date_s d_s, TimeZone tz) :
		DateTimeRawBase<derivedClass>(d_s),
		tzInfo(tz)
	{}

	/**
	* @brief Constructor, which creates DateTime from date_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param d_s date_s structure with date fields.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZBase(date_s d_s, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false) :
		DateTimeRawBase<derivedClass>(d_s),
		tzInfo(tz),
		adj(dst, DST_app)
	{}

	/**
	* @brief Constructor, which creates DateTime from date_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param d_s date_s structure with date fields.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZBase(date_s d_s, TimeZone tz, DSTAdjustment dst, bool DST_app = false) :
		DateTimeRawBase<derivedClass>(d_s),
		tzInfo(tz),
		adj(dst, DST_app)
	{}


	/**
	* @brief Construct DateTimeTZBase from any other DateTimeBase or class which has int64_t getRaw() member function.
	* This class can has those optional member functions:
	* - TimeZone getTimeZone() - gets time zone of dt parameter
	* - DSTAdjustment getDST() - gets DST adjustment of dt parameter
	* This constructor is also used as an explicit conversion.
	* @param dt Any other DateTimeBase.
	*/
	template<class DTclass>
	explicit DateTimeTZBase(const DTclass& dt) {
		set(dt);
	}

	/**
	* @brief Construct DateTimeTZBase from any other DateTimeBase or class which has int64_t getRaw() member function.
	* This class can has those optional member functions:
	* - DSTAdjustment getDST() - gets DST adjustment of dt parameter
	* This constructor is also used as an explicit conversion.
	* @param dt Any other DateTimeBase.
	* @note Date and time is in specific time zone.
	* @param tz Time zone informations, that will be overwritten.
	*/
	template<class DTclass>
	explicit DateTimeTZBase(const DTclass& dt, TimeZone tz) {
		set(dt, tz);
	}

	/**
	* @brief Construct DateTimeTZBase from any other DateTimeBase or class which has int64_t getRaw() member function.
	* This class can has those optional member functions:
	* - DSTAdjustment getDST() - gets DST adjustment of dt parameter
	* This constructor is also used as an explicit conversion.
	* @param dt Any other DateTimeBase.
	* @note Date and time is in specific time zone.
	* @param tz Time zone informations, that will be overwritten.
	*/
	template<class DTclass>
	explicit DateTimeTZBase(const DTclass& dt, TimeZones::TimeZoneName tz) {
		set(dt, tz);
	}

	/**
	* @brief Construct DateTimeTZBase from any other DateTimeBase or class which has int64_t getRaw() member function.
	* This constructor is also used as an explicit conversion.
	* @param dt Any other DateTimeBase.
	* @note Date and time is in specific time zone with specific DST.
	* @param tz Time zone informations, that will be overwritten.
	* @param dst DST adjustment rules and offset, that will be overwritten.
	* @param DST_app True if DST is applyied to date and time.
	*/
	template<class DTclass>
	explicit DateTimeTZBase(const DTclass& dt, TimeZone tz, DSTAdjustment dst, bool DST_app = false) {
		set(dt, tz, dst, DST_app);
	}

	/**
	* @brief Construct DateTimeTZBase from any other DateTimeBase or class which has int64_t getRaw() member function.
	* This constructor is also used as an explicit conversion.
	* @param dt Any other DateTimeBase.
	* @note Date and time is in specific time zone with specific DST.
	* @param tz Time zone informations, that will be overwritten.
	* @param dst DST adjustment rules and offset, that will be overwritten.
	* @param DST_app True if DST is applyied to date and time.
	*/
	template<class DTclass>
	explicit DateTimeTZBase(const DTclass& dt, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false) {
		set(dt, tz, dst, DST_app);
	}



	/**
	* @brief Construct DateTimeTZBase from any other DateTime. This constructor is also used as implicit conversion.
	* @param dt Any other DateTime.
	*/
	DateTimeTZBase(const DateTime& dt) {
		DateTimeBase<derivedClass>::setRawTime(dt.getRaw());
	}

	/**
	* @brief Construct DateTimeTZBase from any other DateTime. This constructor is also used as implicit conversion.
	* @param dt Any other DateTime.
	* @note Date and time is in specific time zone.
	* @param tz Time zone informations, that will be overwritten.
	*/
	DateTimeTZBase(const DateTime& dt, TimeZone tz) :
		tzInfo(tz)
	{
		DateTimeBase<derivedClass>::setRawTime(dt.getRaw());
	}

	/**
	* @brief Construct DateTimeTZBase from any other DateTime. This constructor is also used as implicit conversion.
	* @param dt Any other DateTime.
	* @note Date and time is in specific time zone.
	* @param tz Time zone informations, that will be overwritten.
	*/
	DateTimeTZBase(const DateTime& dt, TimeZones::TimeZoneName tz) :
		tzInfo(tz)
	{
		DateTimeBase<derivedClass>::setRawTime(dt.getRaw());
	}

	/**
	* @brief Construct DateTimeTZBase from any other DateTime. This constructor is also used as implicit conversion.
	* @param dt Any other DateTime.
	* @note Date and time is in specific time zone with specific DST.
	* @param tz Time zone informations, that will be overwritten.
	* @param dst DST adjustment rules and offset, that will be overwritten.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZBase(const DateTime& dt, TimeZone tz, DSTAdjustment dst, bool DST_app = false) :
		tzInfo(tz),
		adj(dst, DST_app)
	{
		DateTimeBase<derivedClass>::setRawTime(dt.getRaw());
	}

	/**
	* @brief Construct DateTimeTZBase from any other DateTime. This constructor is also used as implicit conversion.
	* @param dt Any other DateTime.
	* @note Date and time is in specific time zone with specific DST.
	* @param tz Time zone informations, that will be overwritten.
	* @param dst DST adjustment rules and offset, that will be overwritten.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZBase(const DateTime& dt, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false) :
		tzInfo(tz),
		adj(dst, DST_app)
	{
		DateTimeBase<derivedClass>::setRawTime(dt.getRaw());
	}


	/**
	* @brief Sets current any DateTime instance from any other DateTime instance.
	* @param dt DateTime to set.
	*/
	template<class T, typename dtlib::enable_if<has_getRaw<T>::value && has_getDST<T>::value, int>::type = 0>
	void set(const DateTimeBase<T>& dt) {
		CONSTEXPR_IF (has_getTimeZone<T>::value) {
			tzInfo = static_cast<const T*>(&dt)->getTimeZone();
		}
		adj = static_cast<const T*>(&dt)->getDST();

		CONSTEXPR_IF (has_getDateTimeEnh<T>::value) {
			DateTime_DST_tuple tup = static_cast<const T*>(&dt)->getDateTimeEnh(); //getting DateTime with isDST flag at same time
			adj.setDST(tup.isDST);
			DateTimeBase<derivedClass>::setRawTime(tup.value);
		}
		else {
			DateTimeBase<derivedClass>::setRawTime(dt.getRaw());
		}
	}

	/**
	* @brief Sets current any DateTime instance from any other DateTime instance.
	* @param dt DateTime to set.
	* @param DST_app True if DST is applyied to date and time.
	*/
	template<class T, typename dtlib::enable_if<has_getRaw<T>::value && !has_getDST<T>::value, int>::type = 0>
	void set(const DateTimeBase<T>& dt, bool DST_app = false) {
		CONSTEXPR_IF (has_getTimeZone<T>::value) {
			tzInfo = static_cast<const T*>(&dt)->getTimeZone();
		}
		adj.setDST(DST_app);

		CONSTEXPR_IF (has_getDateTimeEnh<T>::value) {
			DateTime_DST_tuple tup = static_cast<const T*>(&dt)->getDateTimeEnh(); //getting DateTime with isDST flag at same time
			DateTimeBase<derivedClass>::setRawTime(tup.value);
		}
		else {
			DateTimeBase<derivedClass>::setRawTime(dt.getRaw());
		}
	}

	/**
	* @brief Sets current any DateTime instance from any other DateTime instance.
	* @param dt DateTime to set.
	* @param tz Time zone informations, that will be overwritten.
	*/
	template<class T, typename dtlib::enable_if<has_getRaw<T>::value&& has_getDST<T>::value, int>::type = 0>
	void set(const DateTimeBase<T>& dt, TimeZone tz) {
		tzInfo = tz;
		adj = static_cast<const T*>(&dt)->getDST();

		CONSTEXPR_IF (has_getDateTimeEnh<T>::value) {
			DateTime_DST_tuple tup = static_cast<const T*>(&dt)->getDateTimeEnh(); //getting DateTime with isDST flag at same time
			adj.setDST(tup.isDST);
			DateTimeBase<derivedClass>::setRawTime(tup.value);
		}
		else {
			DateTimeBase<derivedClass>::setRawTime(dt.getRaw());
		}
	}

	/**
	* @brief Sets current any DateTime instance from any other DateTime instance.
	* @param dt DateTime to set.
	* @param tz Time zone informations, that will be overwritten.
	* @param DST_app True if DST is applyied to date and time.
	*/
	template<class T, typename dtlib::enable_if<has_getRaw<T>::value && !has_getDST<T>::value, int>::type = 0>
	void set(const DateTimeBase<T>& dt, TimeZone tz, bool DST_app = false) {
		tzInfo = tz;
		adj.setDST(DST_app);

		CONSTEXPR_IF (has_getDateTimeEnh<T>::value) {
			DateTime_DST_tuple tup = static_cast<const T*>(&dt)->getDateTimeEnh(); //getting DateTime with isDST flag at same time
			DateTimeBase<derivedClass>::setRawTime(tup.value);
		}
		else {
			DateTimeBase<derivedClass>::setRawTime(dt.getRaw());
		}
	}

	/**
	* @brief Sets current any DateTime instance from any other DateTime instance.
	* @param dt DateTime to set.
	* @param tz Time zone informations, that will be overwritten.
	*/
	template<class T, typename dtlib::enable_if<has_getRaw<T>::value && has_getDST<T>::value, int>::type = 0>
	void set(const DateTimeBase<T>& dt, TimeZones::TimeZoneName tz) {
		set(dt, tz);
	}

	/**
	* @brief Sets current any DateTime instance from any other DateTime instance.
	* @param dt DateTime to set.
	* @param tz Time zone informations, that will be overwritten.
	* @param DST_app True if DST is applyied to date and time.
	*/
	template<class T, typename dtlib::enable_if<has_getRaw<T>::value && !has_getDST<T>::value, int>::type = 0>
	void set(const DateTimeBase<T>& dt, TimeZones::TimeZoneName tz, bool DST_app = false) {
		set(dt, tz, DST_app);
	}


	/**
	* @brief Sets current any DateTime instance from any other DateTime instance.
	* @param dt DateTime to set.
	* @param tz Time zone informations, that will be overwritten.
	* @param dst DST adjustment rules and offset, that will be overwritten.
	*/
	template<class T, typename dtlib::enable_if<has_getRaw<T>::value && has_getDST<T>::value, int>::type = 0>
	void set(const DateTimeBase<T>& dt, TimeZone tz, DSTAdjustment dst) {
		tzInfo = tz;
		adj = dst;

		CONSTEXPR_IF (has_getDateTimeEnh<T>::value) {
			DateTime_DST_tuple tup = static_cast<const T*>(&dt)->getDateTimeEnh(); //getting DateTime with isDST flag at same time
			adj.setDST(tup.isDST);
			DateTimeBase<derivedClass>::setRawTime(tup.value);
		}
		else {
			DateTimeBase<derivedClass>::setRawTime(dt.getRaw());
		}
	}

	/**
	* @brief Sets current any DateTime instance from any other DateTime instance.
	* @param dt DateTime to set.
	* @param tz Time zone informations, that will be overwritten.
	* @param dst DST adjustment rules and offset, that will be overwritten.
	* @param DST_app True if DST is applyied to date and time.
	*/
	template<class T, typename dtlib::enable_if<has_getRaw<T>::value && !has_getDST<T>::value, int>::type = 0>
	void set(const DateTimeBase<T>& dt, TimeZone tz, DSTAdjustment dst, bool DST_app = false) {
		tzInfo = tz;
		adj = dst;
		adj.setDST(DST_app);

		CONSTEXPR_IF (has_getDateTimeEnh<T>::value) {
			DateTime_DST_tuple tup = static_cast<const T*>(&dt)->getDateTimeEnh(); //getting DateTime with isDST flag at same time
			DateTimeBase<derivedClass>::setRawTime(tup.value);
		}
		else {
			DateTimeBase<derivedClass>::setRawTime(dt.getRaw());
		}
	}

	/**
	* @brief Sets current any DateTime instance from any other DateTime instance.
	* @param dt DateTime to set.
	* @param tz Time zone informations, that will be overwritten.
	* @param dst DST adjustment rules and offset, that will be overwritten.
	*/
	template<class T, typename dtlib::enable_if<has_getRaw<T>::value&& has_getDST<T>::value, int>::type = 0>
	void set(const DateTimeBase<T>& dt, TimeZones::TimeZoneName tz, DSTAdjustment dst) {
		set(dt, tz, dst);
	}

	/**
	* @brief Sets current any DateTime instance from any other DateTime instance.
	* @param dt DateTime to set.
	* @param tz Time zone informations, that will be overwritten.
	* @param dst DST adjustment rules and offset, that will be overwritten.
	* @param DST_app True if DST is applyied to date and time.
	*/
	template<class T, typename dtlib::enable_if<has_getRaw<T>::value && !has_getDST<T>::value, int>::type = 0>
	void set(const DateTimeBase<T>& dt, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false) {
		set(dt, tz, dst, DST_app);
	}


	/**
	* @brief Gets time zone info.
	*/
	inline TimeZone getTimeZone() const {
		return tzInfo;
	}

	/**
	* @brief Gets time zone offset represented by TimeSpan.
	*/
	inline TimeSpan getTimeZoneOffset() const {
		return tzInfo.getTimeZoneOffset();
	}

	/**
	* @brief Gets time zone offset in minutes.
	*/
	inline int16_t getTimeZoneOffsetMinutes() const {
		return tzInfo.getTimeZoneOffsetTotalMinutes();
	}

	/**
	* @brief Sets time zone info.
	* @param tz Time zone info to set.
	* @param recalculateTime If true, current time will be converted to UTC and then converted to new time zone.
	* Else time stays unchanged.
	*/
	void setTimeZone(TimeZone tz, bool recalculateTime = true) {
		if (recalculateTime) {
			//No sync here, because only time zone has changed.
			int64_t diff = (int32_t)tz.getTimeZone() - (int32_t)tzInfo.getTimeZone();
			diff = diff * 15 * MINUTE;
			DateTimeBase<derivedClass>::addRawTime(diff); //This calls addRawTimeTD(), but we want it
		}
		tzInfo = tz;
	}

	/**
	* @brief Sets time zone info.
	* @param tz Time zone info to set.
	* @param recalculateTime If true, current time will be converted to UTC and then converted to new time zone.
	* Else time stays unchanged.
	*/
	inline void setTimeZone(TimeZones tz, bool recalculateTime = true) {
		setTimeZone(TimeZone(tz), recalculateTime);
	}


	/**
	* @brief Gets DST adjustment.
	*/
	inline DSTAdjustment getDST() const {
		return adj;
	}

	/**
	* @brief Gets current DST offset represented by TimeSpan. If DST is not applyied, 0 is returned.
	*/
	TimeSpan getCurrentDSTOffset() const {
		if (isDST() && !adj.noDST()) {
			return getDSTOffset();
		}
		else {
			return TimeSpan::Zero;
		}
	}

	/**
	* @brief Gets DST offset in minutes.
	*/
	int16_t getCurrentDSTOffsetMinutes() const {
		if (isDST() && !adj.noDST()) {
			return adj.getDSTOffsetTotalMinutes();
		}
		else {
			return 0;
		}
	}

	/**
	* @brief Gets DST offset represented by TimeSpan.
	*/
	inline TimeSpan getDSTOffset() const {
		return ((int64_t)adj.getDSTOffsetTotalMinutes()) * MINUTE;
	}

	/**
	* @brief Gets DST offset in minutes.
	*/
	inline int16_t getDSTOffsetMinutes() const {
		return adj.getDSTOffsetTotalMinutes();
	}

	/**
	* @brief Sets adjustment.
	* @param dst DSTAdjustment to set.
	* @param recalculateTime If true, current time will be converted to UTC and then converted to new DST.
	* Else time stays unchanged.
	*/
	void setDST(DSTAdjustment dst, bool recalculateTime = true) {
		if (recalculateTime) {
			//No sync here, because only time zone has changed.
			int16_t currentDST = 0;
			if (isDST()) {
				currentDST = adj.getDSTOffsetTotalMinutes();
			}

			int16_t newDST = 0;
			dst.setDST(dst.checkDSTRegion(DateTimeBase<derivedClass>::getRawTime()));
			if (dst.isDST()) {
				newDST = dst.getDSTOffsetTotalMinutes();
			}

			int64_t diff = currentDST - newDST;
			diff = diff * MINUTE;
			DateTimeBase<derivedClass>::addRawTimeNoTD(diff); //This does not call addRawTimeTD(), because it would cause endless recursion
		}
		adj = dst;
	}

	/**
	* @brief Checks if DST is applyied.
	*/
	inline bool isDST() const {
		CONSTEXPR_IF (has_isSyncDST<derivedClass>::value) {
			return static_cast<const derivedClass*>(this)->isSyncDST();
		}
		else {
			return adj.isDST();
		}
	}

	/**
	* @brief Gets UTC DateTime.
	*/
	DateTime getUTC() const {
		int64_t tzOffset = (int64_t)getTimeZoneOffset();
		int64_t dstOffset = (int64_t)getCurrentDSTOffset();
		int64_t dateTime = DateTimeBase<derivedClass>::getRawTime() - tzOffset - dstOffset;
		return dateTime;
	}

	/**
	* @brief Sets UTC DateTime.
	* @param dt UTC DateTime. Time zone or DST is ignored here, only value as it is is used.
	*/
	template<class T>
	void setUTC(const DateTimeBase<T>& dt) {
		DateTimeTZBase<derivedClass>::syncBeforeSet();
		int64_t dateTime = dt.getRawTime();

		int64_t tzOffset = (int64_t)getTimeZoneOffset();
		int64_t dstOffset = (int64_t)getDSTOffset();
		
		dateTime += tzOffset;

		bool isDST = adj.checkDSTRegion(dateTime);
		if (isDST) {
			dateTime += dstOffset;
		}

		DateTimeTZBase<derivedClass>::setRawTimeNoTD(dateTime); //This does not call addRawTimeTD(), because it would cause endless recursion
		adj.setDST(isDST);
	}

	/**
	* @brief Gets DateTime of next transition.
	* @param[out] nextTransIsDST True if next transition is to DST or false if not.
	* @return Returns DateTime of next transition. This value has DST applyied only if nextTransIsDST is false.
	*/
	inline DateTime getNextTransitionDate(bool& nextTransIsDST) const {
		return adj.getNextTransitionDate(*this, nextTransIsDST);
	}

	template<typename T, typename dtlib::enable_if<has_getCurrentDSTOffsetMinutes<T>::value && has_getTimeZoneOffsetMinutes<T>::value, int>::type = 0>
	TimeSpan operator-(const DateTimeBase<T>& raw) const {
		//Just sync time here
		int16_t ofs1 = getTimeZoneOffsetMinutes() + getCurrentDSTOffsetMinutes();
		int16_t ofs2 = static_cast<const T*>(&raw)->getTimeZoneOffsetMinutes() + static_cast<const T*>(&raw)->getCurrentDSTOffsetMinutes();
		int64_t offset = ((int64_t)(ofs1 - ofs2)) * MINUTE;
		return TimeSpan(DateTimeBase<derivedClass>::getRawTime() - raw.getRawTime() - offset);
	}

	//Comparison operators

	template<class T, typename dtlib::enable_if<has_getCurrentDSTOffsetMinutes<T>::value && has_getTimeZoneOffsetMinutes<T>::value, int>::type = 0>
	bool operator==(DateTimeBase<T>& dt2) const {
		int16_t ofs1 = getTimeZoneOffsetMinutes() + getCurrentDSTOffsetMinutes();
		int16_t ofs2 = static_cast<const T*>(&dt2)->getTimeZoneOffsetMinutes() + static_cast<const T*>(&dt2)->getCurrentDSTOffsetMinutes();
		int64_t offset = ((int64_t)(ofs1 - ofs2)) * MINUTE;
		return DateTimeBase<derivedClass>::getRawTime() - offset == dt2.getRawTime();
	}

	template<class T, typename dtlib::enable_if<has_getCurrentDSTOffsetMinutes<T>::value&& has_getTimeZoneOffsetMinutes<T>::value, int>::type = 0>
	bool operator!=(DateTimeBase<T>& dt2) const {
		int16_t ofs1 = getTimeZoneOffsetMinutes() + getCurrentDSTOffsetMinutes();
		int16_t ofs2 = static_cast<const T*>(&dt2)->getTimeZoneOffsetMinutes() + static_cast<const T*>(&dt2)->getCurrentDSTOffsetMinutes();
		int64_t offset = ((int64_t)(ofs1 - ofs2)) * MINUTE;
		return DateTimeBase<derivedClass>::getRawTime() - offset != dt2.getRawTime();
	}

	template<class T, typename dtlib::enable_if<has_getCurrentDSTOffsetMinutes<T>::value&& has_getTimeZoneOffsetMinutes<T>::value, int>::type = 0>
	bool operator>=(DateTimeBase<T>& dt2) const {
		int16_t ofs1 = getTimeZoneOffsetMinutes() + getCurrentDSTOffsetMinutes();
		int16_t ofs2 = static_cast<const T*>(&dt2)->getTimeZoneOffsetMinutes() + static_cast<const T*>(&dt2)->getCurrentDSTOffsetMinutes();
		int64_t offset = ((int64_t)(ofs1 - ofs2)) * MINUTE;
		return DateTimeBase<derivedClass>::getRawTime() - offset >= dt2.getRawTime();
	}

	template<class T, typename dtlib::enable_if<has_getCurrentDSTOffsetMinutes<T>::value&& has_getTimeZoneOffsetMinutes<T>::value, int>::type = 0>
	bool operator<=(DateTimeBase<T>& dt2) const {
		int16_t ofs1 = getTimeZoneOffsetMinutes() + getCurrentDSTOffsetMinutes();
		int16_t ofs2 = static_cast<const T*>(&dt2)->getTimeZoneOffsetMinutes() + static_cast<const T*>(&dt2)->getCurrentDSTOffsetMinutes();
		int64_t offset = ((int64_t)(ofs1 - ofs2)) * MINUTE;
		return DateTimeBase<derivedClass>::getRawTime() - offset <= dt2.getRawTime();
	}

	template<class T, typename dtlib::enable_if<has_getCurrentDSTOffsetMinutes<T>::value&& has_getTimeZoneOffsetMinutes<T>::value, int>::type = 0>
	bool operator<(DateTimeBase<T>& dt2) const {
		int16_t ofs1 = getTimeZoneOffsetMinutes() + getCurrentDSTOffsetMinutes();
		int16_t ofs2 = static_cast<const T*>(&dt2)->getTimeZoneOffsetMinutes() + static_cast<const T*>(&dt2)->getCurrentDSTOffsetMinutes();
		int64_t offset = ((int64_t)(ofs1 - ofs2)) * MINUTE;
		return DateTimeBase<derivedClass>::getRawTime() - offset < dt2.getRawTime();
	}

	template<class T, typename dtlib::enable_if<has_getCurrentDSTOffsetMinutes<T>::value&& has_getTimeZoneOffsetMinutes<T>::value, int>::type = 0>
	bool operator>(DateTimeBase<T>& dt2) const {
		int16_t ofs1 = getTimeZoneOffsetMinutes() + getCurrentDSTOffsetMinutes();
		int16_t ofs2 = static_cast<const T*>(&dt2)->getTimeZoneOffsetMinutes() + static_cast<const T*>(&dt2)->getCurrentDSTOffsetMinutes();
		int64_t offset = ((int64_t)(ofs1 - ofs2)) * MINUTE;
		return DateTimeBase<derivedClass>::getRawTime() - offset > dt2.getRawTime();
	}

protected:

	template<class T> friend class DateTimeBase;
	template<class T, typename Y> friend struct has_getTimeZoneOffsetMinutes;
	template<class T, typename Y> friend struct has_getCurrentDSTOffsetMinutes;
	template<class T, typename Y> friend struct has_setRawTimeTD;
	template<class T, typename Y> friend struct has_addRawTimeTD;

	void setRawTimeTD(int64_t val) {
		//This function is called when some field is set. We will check here if DST is applyied.
		int64_t dtToCheck = val;
		bool oldDST = isDST();
		if (oldDST) {
			dtToCheck -= (int64_t)getDSTOffset(); //Getting time without DST
		}
		bool newDST = adj.checkDSTRegion(dtToCheck);
		if (oldDST != newDST) {
			if (newDST) {
				//Applying DST offset
				val += (int64_t)getDSTOffset();
			}
			else {
				//Removing DST offset
				val -= (int64_t)getDSTOffset();
			}
			adj.setDST(newDST);
		}
		DateTimeTZBase<derivedClass>::setRawTimeNoTD(val); //This does not call setRawTimeTD(), because it would cause endless recursion
		CONSTEXPR_IF (has_calcNextTransOnSet<derivedClass>::value) {
			static_cast<derivedClass*>(this)->calcNextTransOnSet(val, newDST);
		}
	}

	void addRawTimeTD(int64_t val) {
		//This function is called when some add function is called. We will check here if DST is applyied.
		int64_t rawVal = val + DateTimeTZBase<derivedClass>::getRawTime();
		int64_t dtToCheck = rawVal;
		bool oldDST = isDST();
		if (oldDST) {
			dtToCheck -= (int64_t)getDSTOffset(); //Getting time without DST
		}
		bool newDST = adj.checkDSTRegion(dtToCheck);
		if (oldDST != newDST) {
			if (newDST) {
				//Applying DST offset
				val += (int64_t)getDSTOffset();
			}
			else {
				//Removing DST offset
				val -= (int64_t)getDSTOffset();
			}
			adj.setDST(newDST);
		}
		DateTimeTZBase<derivedClass>::addRawTimeNoTD(val); //This does not call addRawTimeTD(), because it would cause endless recursion
		CONSTEXPR_IF (has_calcNextTransOnSet<derivedClass>::value) {
			static_cast<derivedClass*>(this)->calcNextTransOnSet(dtToCheck, false);
		}
	}

	TimeZone tzInfo;
	DSTAdjustment adj;
};

class DateTimeTZ : public DateTimeTZBase<DateTimeTZ> {
public:
	using DateTimeTZBase<DateTimeTZ>::DateTimeTZBase; //Using inherited constructors
};

#endif // !DATE_TIME_TZ_H
