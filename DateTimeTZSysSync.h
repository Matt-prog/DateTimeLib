/**
 * @file DateTimeTZSysSync.h
 * @brief This file contains class DateTimeTZSysSync. It is DateTimeTZ class,
 * which is synchronized by software.
 *
 * @see DateTimeTZSysSync
 *
 * # Credits
 * @author Matej Fitoš
 * @date Jun 4, 2022
 */

#ifndef DATE_TIME_TZ_SYS_SYNC_H
#define DATE_TIME_TZ_SYS_SYNC_H

#include "DateTimeTZ.h"
#include "DateTimeSysSync.h"


/**
* @class DateTimeTZSysSync
* @brief DateTime class, which is synchronized with system clock. It means, that any time
* you read some field from this class, it will be always up to date. This class has to be
* initialized with valid date and time to return valid value. Synchronization principle
* is that when reading a time and date value, the time that has elapsed since the last
* setting of any time or date field, is added to it. On computer with any operational system
* is used chrono library for synchronization. On Arduino or STM32 is used micros64() or
* millis() function. This value may be inaccurate when using micros64() or millis() function,
* because they don't have to always return accurate value. When using millis() for synchronization,
* microseconds field stays unchanged.<br>
* Difference between DateTimeSysSync and DateTimeTZSysSync is, that DateTimeTZSysSync has extra
* fields for time zone and DST adjustment.
* 
* ## Extra member functions
* + **On Windows OS:**
*   - now() returns current system time.
*   - systemTZ - field with system time zone offset captured at start of program.
*   - getSystemTZ() - gets system time zone offset.
*   - systemDSTAdjustment - field with system DST adjustment captured at start of program.
*   - getSystemDSTAdjustment() - gets system DST adjustment.
* + **On Linux or Mac OS:**
*   - now() returns current system time.
*   - systemTZ - field with system time zone and DST offset captured at start of program.
*   - getSystemTZ() - gets system time zone offset.
*   @note systemDSTAdjustment cannot be retrieved right now on Linux or Mac OS.
* 
* ## DST region check
* DateTimeTZSysSync value is constantly incrementing, because it's value is synchronized with system time.
* Every time you get any time or date field, it is recalculated. During this recalculation DST region would
* be recalculated every time. DST region check performance is not best, that's why it was optimized like this:
* 1. Every time you set any DateTime fields, next DST transition date is calculated.
* 2. Every time you get any DateTime field, it is just checked, if sync time is before or after next DST transition date.
*    If before, just synchronized DateTime value is returned. If after, synchronized value after DST adjustment is returned.
* This solution has one disadvantage. All member functions, which retrieves any date and time field is marked as const,
* so it cannot set new next DST transition date if needed. That's why we have created recalcDST() function. It checks,
* if sync time is after next DST transition date. If yes, new next DST transition date is calculated and DST adjustment
* is applyied to sync time. recalcDST() should be called at least one time per day or more often to achieve better berformance
* at retrieving time or date fields.
* 
* ## Time zone and operators
* Operators have different behavior, when:
* + DateTimeTZ or DateTimeTZSysSync and DateTimeTZ or DateTimeTZSysSync is compared or subtracted
*    - UTC time is used
* + DateTime without time zone or DST adjustment is compared or subtracted
*    - Local time is used
*
* @see DateTimeTZ
* @see DateTimeTZBase
* @see DateTimeSysSync
* @see TimeZone
* @see DSTAdjustment
*/
class DateTimeTZSysSync : public DateTimeTZBase<DateTimeTZSysSync>
{
public:
	using DateTimeTZBase<DateTimeTZSysSync>::DateTimeTZBase; //Using inherited constructors

	/**
	* @brief Default constructor, which set DateTime to the 0001/01/01 00:00:00.000000 with default time zone and DST value.
	*/
	DateTimeTZSysSync() : DateTimeTZBase<DateTimeTZSysSync>()
	{ }

	/**
	* @brief Constructor, which sets DateTime from raw value with default time zone and DST value.
	* @param raw Raw value in microseconds from the begin of epoch (0001/1/1).
	* @note Date and time has to be without applyied DST.
	*/
	DateTimeTZSysSync(int64_t raw)
	{
		DateTimeRawBase<DateTimeTZSysSync>::setRawTime(raw);
	}

	/**
	* @brief Constructor, which sets DateTime from raw value with time zone and DST value.
	* @note Raw value is in specific time zone with specific DST.
	* @param raw Raw value in microseconds from the begin of epoch (0001/1/1).
	* @note Date and time has to be without applyied DST.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZSysSync(int64_t raw, TimeZone tz)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeRawBase<DateTimeTZSysSync>::setRawTime(raw);
	}

	/**
	* @brief Constructor, which sets DateTime from raw value with time zone and DST value.
	* @note Raw value is in specific time zone with specific DST.
	* @param raw Raw value in microseconds from the begin of epoch (0001/1/1).
	* @note Date and time has to be without applyied DST.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZSysSync(int64_t raw, TimeZones::TimeZoneName tz)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeRawBase<DateTimeTZSysSync>::setRawTime(raw);
	}

	/**
	* @brief Constructor, which sets DateTime from raw value with time zone and DST value.
	* @note Raw value is in specific time zone with specific DST.
	* @param raw Raw value in microseconds from the begin of epoch (0001/1/1).
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZSysSync(int64_t raw, TimeZone tz, DSTAdjustment dst, bool DST_app = false)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeTZBase<DateTimeTZSysSync>::adj = DSTAdjustment(dst, DST_app);
		DateTimeRawBase<DateTimeTZSysSync>::setRawTime(raw);
	}

	/**
	* @brief Constructor, which sets DateTime from raw value with time zone and DST value.
	* @note Raw value is in specific time zone with specific DST.
	* @param raw Raw value in microseconds from the begin of epoch (0001/1/1).
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZSysSync(int64_t raw, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeTZBase<DateTimeTZSysSync>::adj = DSTAdjustment(dst, DST_app);
		DateTimeRawBase<DateTimeTZSysSync>::setRawTime(raw);
	}


	/**
	* @brief Constructor, which sets DateTime from date fields with default time zone and DST value.
	* @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including.
	* @param month Month from 1 to 12.
	* @param day Day of month from 1 to 28/29/30/31.
	* @note Date and time has to be without applyied DST.
	*/
	DateTimeTZSysSync(int32_t year, uint8_t month, uint8_t day)
	{
		DateTimeRawBase<DateTimeTZSysSync>::set(year, month, day);
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
	DateTimeTZSysSync(int32_t year, uint8_t month, uint8_t day, TimeZone tz)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeRawBase<DateTimeTZSysSync>::set(year, month, day);
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
	DateTimeTZSysSync(int32_t year, uint8_t month, uint8_t day, TimeZones::TimeZoneName tz)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeRawBase<DateTimeTZSysSync>::set(year, month, day);
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
	DateTimeTZSysSync(int32_t year, uint8_t month, uint8_t day, TimeZone tz, DSTAdjustment dst, bool DST_app = false)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeTZBase<DateTimeTZSysSync>::adj = DSTAdjustment(dst, DST_app);
		DateTimeRawBase<DateTimeTZSysSync>::set(year, month, day);
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
	DateTimeTZSysSync(int32_t year, uint8_t month, uint8_t day, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeTZBase<DateTimeTZSysSync>::adj = DSTAdjustment(dst, DST_app);
		DateTimeRawBase<DateTimeTZSysSync>::set(year, month, day);
	}

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
	DateTimeTZSysSync(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes = 0, uint8_t seconds = 0, uint16_t milliseconds = 0, uint16_t microseconds = 0)
	{
		DateTimeRawBase<DateTimeTZSysSync>::set(year, month, day, hours, minutes, seconds, milliseconds, microseconds);
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
	DateTimeTZSysSync(int32_t year, uint8_t month, uint8_t day, hour_t hours, TimeZone tz)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeRawBase<DateTimeTZSysSync>::set(year, month, day, hours);
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
	DateTimeTZSysSync(int32_t year, uint8_t month, uint8_t day, hour_t hours, TimeZones::TimeZoneName tz)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeRawBase<DateTimeTZSysSync>::set(year, month, day, hours);
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
	DateTimeTZSysSync(int32_t year, uint8_t month, uint8_t day, hour_t hours, TimeZone tz, DSTAdjustment dst, bool DST_app = false)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeTZBase<DateTimeTZSysSync>::adj = DSTAdjustment(dst, DST_app);
		DateTimeRawBase<DateTimeTZSysSync>::set(year, month, day, hours);
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
	DateTimeTZSysSync(int32_t year, uint8_t month, uint8_t day, hour_t hours, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeTZBase<DateTimeTZSysSync>::adj = DSTAdjustment(dst, DST_app);
		DateTimeRawBase<DateTimeTZSysSync>::set(year, month, day, hours);
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
	DateTimeTZSysSync(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, TimeZone tz)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeRawBase<DateTimeTZSysSync>::set(year, month, day, hours, minutes);
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
	DateTimeTZSysSync(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, TimeZones::TimeZoneName tz)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeRawBase<DateTimeTZSysSync>::set(year, month, day, hours, minutes);
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
	DateTimeTZSysSync(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, TimeZone tz, DSTAdjustment dst, bool DST_app = false)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeTZBase<DateTimeTZSysSync>::adj = DSTAdjustment(dst, DST_app);
		DateTimeRawBase<DateTimeTZSysSync>::set(year, month, day, hours, minutes);
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
	DateTimeTZSysSync(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeTZBase<DateTimeTZSysSync>::adj = DSTAdjustment(dst, DST_app);
		DateTimeRawBase<DateTimeTZSysSync>::set(year, month, day, hours, minutes);
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
	DateTimeTZSysSync(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, uint8_t seconds, TimeZone tz)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeRawBase<DateTimeTZSysSync>::set(year, month, day, hours, minutes, seconds);
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
	DateTimeTZSysSync(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, uint8_t seconds, TimeZones::TimeZoneName tz)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeRawBase<DateTimeTZSysSync>::set(year, month, day, hours, minutes, seconds);
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
	DateTimeTZSysSync(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, uint8_t seconds, TimeZone tz, DSTAdjustment dst, bool DST_app = false)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeTZBase<DateTimeTZSysSync>::adj = DSTAdjustment(dst, DST_app);
		DateTimeRawBase<DateTimeTZSysSync>::set(year, month, day, hours, minutes, seconds);
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
	DateTimeTZSysSync(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, uint8_t seconds, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeTZBase<DateTimeTZSysSync>::adj = DSTAdjustment(dst, DST_app);
		DateTimeRawBase<DateTimeTZSysSync>::set(year, month, day, hours, minutes, seconds);
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
	DateTimeTZSysSync(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds, TimeZone tz)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeRawBase<DateTimeTZSysSync>::set(year, month, day, hours, minutes, seconds, milliseconds);
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
	DateTimeTZSysSync(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds, TimeZones::TimeZoneName tz)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeRawBase<DateTimeTZSysSync>::set(year, month, day, hours, minutes, seconds, milliseconds);
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
	DateTimeTZSysSync(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds, TimeZone tz, DSTAdjustment dst, bool DST_app = false)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeTZBase<DateTimeTZSysSync>::adj = DSTAdjustment(dst, DST_app);
		DateTimeRawBase<DateTimeTZSysSync>::set(year, month, day, hours, minutes, seconds, milliseconds);
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
	DateTimeTZSysSync(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeTZBase<DateTimeTZSysSync>::adj = DSTAdjustment(dst, DST_app);
		DateTimeRawBase<DateTimeTZSysSync>::set(year, month, day, hours, minutes, seconds, milliseconds);
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
	DateTimeTZSysSync(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds, uint16_t microseconds, TimeZone tz)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeRawBase<DateTimeTZSysSync>::set(year, month, day, hours, minutes, seconds, milliseconds, microseconds);
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
	DateTimeTZSysSync(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds, uint16_t microseconds, TimeZones::TimeZoneName tz)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeRawBase<DateTimeTZSysSync>::set(year, month, day, hours, minutes, seconds, milliseconds, microseconds);
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
	DateTimeTZSysSync(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds, uint16_t microseconds, TimeZone tz, DSTAdjustment dst, bool DST_app = false)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeTZBase<DateTimeTZSysSync>::adj = DSTAdjustment(dst, DST_app);
		DateTimeRawBase<DateTimeTZSysSync>::set(year, month, day, hours, minutes, seconds, milliseconds, microseconds);
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
	DateTimeTZSysSync(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds, uint16_t microseconds, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeTZBase<DateTimeTZSysSync>::adj = DSTAdjustment(dst, DST_app);
		DateTimeRawBase<DateTimeTZSysSync>::set(year, month, day, hours, minutes, seconds, milliseconds, microseconds);
	}

	/**
	* @brief Constructor, which creates DateTime from tm.
	* @note Date and time is in specific time zone with specific DST.
	* @param dt_s Date and time specified by tm structure.
	*/
	DateTimeTZSysSync(tm dt_s)
	{
		DateTimeRawBase<DateTimeTZSysSync>::set(dt_s);
	}

	/**
	* @brief Constructor, which creates DateTime from tm.
	* @note Date and time is in specific time zone with specific DST.
	* @param dt_s Date and time specified by tm structure.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZSysSync(tm dt_s, TimeZones::TimeZoneName tz)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeRawBase<DateTimeTZSysSync>::set(dt_s);
	}

	/**
	* @brief Constructor, which creates DateTime from tm.
	* @note Date and time is in specific time zone with specific DST.
	* @param dt_s Date and time specified by tm structure.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZSysSync(tm dt_s, TimeZone tz)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeRawBase<DateTimeTZSysSync>::set(dt_s);
	}

	/**
	* @brief Constructor, which creates DateTime from tm.
	* @note Date and time is in specific time zone with specific DST.
	* @param dt_s Date and time specified by tm structure.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	*/
	DateTimeTZSysSync(tm dt_s, TimeZone tz, DSTAdjustment dst)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeTZBase<DateTimeTZSysSync>::adj = DSTAdjustment(dst, dt_s.tm_isdst);
		DateTimeRawBase<DateTimeTZSysSync>::set(dt_s);
	}

	/**
	* @brief Constructor, which creates DateTime from tm.
	* @note Date and time is in specific time zone with specific DST.
	* @param dt_s Date and time specified by tm structure.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	*/
	DateTimeTZSysSync(tm dt_s, TimeZones::TimeZoneName tz, DSTAdjustment dst)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeTZBase<DateTimeTZSysSync>::adj = DSTAdjustment(dst, dt_s.tm_isdst);
		DateTimeRawBase<DateTimeTZSysSync>::set(dt_s);
	}

	/**
	* @brief Constructor, which creates DateTime from date_time_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param dt_s date_time_s structure with date and time fields.
	*/
	DateTimeTZSysSync(date_time_s dt_s)
	{
		DateTimeRawBase<DateTimeTZSysSync>::set(dt_s);
	}

	/**
	* @brief Constructor, which creates DateTime from date_time_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param dt_s date_time_s structure with date and time fields.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZSysSync(date_time_s dt_s, TimeZones::TimeZoneName tz)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeRawBase<DateTimeTZSysSync>::set(dt_s);
	}

	/**
	* @brief Constructor, which creates DateTime from date_time_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param dt_s date_time_s structure with date and time fields.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZSysSync(date_time_s dt_s, TimeZone tz)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeRawBase<DateTimeTZSysSync>::set(dt_s);
	}

	/**
	* @brief Constructor, which creates DateTime from date_time_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param dt_s date_time_s structure with date and time fields.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZSysSync(date_time_s dt_s, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeTZBase<DateTimeTZSysSync>::adj = DSTAdjustment(dst, DST_app);
		DateTimeRawBase<DateTimeTZSysSync>::set(dt_s);
	}

	/**
	* @brief Constructor, which creates DateTime from date_time_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param dt_s date_time_s structure with date and time fields.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZSysSync(date_time_s dt_s, TimeZone tz, DSTAdjustment dst, bool DST_app = false)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeTZBase<DateTimeTZSysSync>::adj = DSTAdjustment(dst, DST_app);
		DateTimeRawBase<DateTimeTZSysSync>::set(dt_s);
	}

	/**
	* @brief Constructor, which creates DateTime from time_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param t_s time_s structure with time fields.
	*/
	DateTimeTZSysSync(time_s t_s)
	{
		DateTimeRawBase<DateTimeTZSysSync>::set(t_s);
	}

	/**
	* @brief Constructor, which creates DateTime from time_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param t_s time_s structure with time fields.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZSysSync(time_s t_s, TimeZones::TimeZoneName tz)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeRawBase<DateTimeTZSysSync>::set(t_s);
	}

	/**
	* @brief Constructor, which creates DateTime from time_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param t_s time_s structure with time fields.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZSysSync(time_s t_s, TimeZone tz)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeRawBase<DateTimeTZSysSync>::set(t_s);
	}

	/**
	* @brief Constructor, which creates DateTime from time_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param t_s time_s structure with time fields.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZSysSync(time_s t_s, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeTZBase<DateTimeTZSysSync>::adj = DSTAdjustment(dst, DST_app);
		DateTimeRawBase<DateTimeTZSysSync>::set(t_s);
	}

	/**
	* @brief Constructor, which creates DateTime from time_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param t_s time_s structure with time fields.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZSysSync(time_s t_s, TimeZone tz, DSTAdjustment dst, bool DST_app = false)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeTZBase<DateTimeTZSysSync>::adj = DSTAdjustment(dst, DST_app);
		DateTimeRawBase<DateTimeTZSysSync>::set(t_s);
	}

	/**
	* @brief Constructor, which creates DateTime from date_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param d_s date_s structure with date fields.
	*/
	DateTimeTZSysSync(date_s d_s)
	{
		DateTimeRawBase<DateTimeTZSysSync>::set(d_s);
	}

	/**
	* @brief Constructor, which creates DateTime from date_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param d_s date_s structure with date fields.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZSysSync(date_s d_s, TimeZones::TimeZoneName tz)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeRawBase<DateTimeTZSysSync>::set(d_s);
	}

	/**
	* @brief Constructor, which creates DateTime from date_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param d_s date_s structure with date fields.
	* @param tz Time zone and DST info.
	*/
	DateTimeTZSysSync(date_s d_s, TimeZone tz)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeRawBase<DateTimeTZSysSync>::set(d_s);
	}

	/**
	* @brief Constructor, which creates DateTime from date_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param d_s date_s structure with date fields.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZSysSync(date_s d_s, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeTZBase<DateTimeTZSysSync>::adj = DSTAdjustment(dst, DST_app);
		DateTimeRawBase<DateTimeTZSysSync>::set(d_s);
	}

	/**
	* @brief Constructor, which creates DateTime from date_s.
	* @note Date and time is in specific time zone with specific DST.
	* @param d_s date_s structure with date fields.
	* @param tz Time zone and DST info.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZSysSync(date_s d_s, TimeZone tz, DSTAdjustment dst, bool DST_app = false)
	{
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = TimeZone(tz);
		DateTimeTZBase<DateTimeTZSysSync>::adj = DSTAdjustment(dst, DST_app);
		DateTimeRawBase<DateTimeTZSysSync>::set(d_s);
	}

	/**
	* @brief Construct DateTimeTZSysSync from DateTimeSysSync with no time zone and no DST adjustment.
	* @param dt DateTimeSysSync.
	*/
	DateTimeTZSysSync(const DateTimeSysSync& dt)
	{
		set(dt);
	}

	/**
	* @brief Construct DateTimeTZSysSync from DateTimeSysSync with specified time zone.
	* @param dt DateTimeSysSync.
	* @note Date and time is in specified time zone.
	* @param tz Time zone informations, that will be overwritten.
	*/
	DateTimeTZSysSync(const DateTimeSysSync& dt, TimeZone tz)
	{
		set(dt, tz);
	}

	/**
	* @brief Construct DateTimeTZSysSync from DateTimeSysSync with specified time zone.
	* @param dt DateTimeSysSync.
	* @note Date and time is in specified time zone.
	* @param tz Time zone informations, that will be overwritten.
	*/
	DateTimeTZSysSync(const DateTimeSysSync& dt, TimeZones::TimeZoneName tz)
	{
		set(dt, tz);
	}

	/**
	* @brief Construct DateTimeTZSysSync from DateTimeSysSync with specified time zone.
	* @param dt DateTimeSysSync.
	* @note Date and time is in specified time zone.
	* @param tz Time zone information.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZSysSync(const DateTimeSysSync& dt, TimeZone tz, DSTAdjustment dst, bool DST_app = false)
	{
		set(dt, tz, dst, DST_app);
	}

	/**
	* @brief Construct DateTimeTZSysSync from DateTimeSysSync with specified time zone.
	* @param dt DateTimeSysSync.
	* @note Date and time is in specified time zone.
	* @param tz Time zone information.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZSysSync(const DateTimeSysSync& dt, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false)
	{
		set(dt, tz, dst, DST_app);
	}

	/**
	* @brief Construct DateTimeTZSysSync from DateTime with no time zone and no DST adjustment.
	* @param dt DateTime.
	*/
	DateTimeTZSysSync(const DateTime& dt)
	{
		set(dt);
	}

	/**
	* @brief Construct DateTimeTZSysSync from DateTime with specified time zone.
	* @param dt DateTime.
	* @note Date and time is in specified time zone.
	* @param tz Time zone informations, that will be overwritten.
	*/
	DateTimeTZSysSync(const DateTime& dt, TimeZone tz)
	{
		set(dt, tz);
	}

	/**
	* @brief Construct DateTimeTZSysSync from DateTime with specified time zone.
	* @param dt DateTime.
	* @note Date and time is in specified time zone.
	* @param tz Time zone informations, that will be overwritten.
	*/
	DateTimeTZSysSync(const DateTime& dt, TimeZones::TimeZoneName tz)
	{
		set(dt, tz);
	}

	/**
	* @brief Construct DateTimeTZSysSync from DateTime with specified time zone.
	* @param dt DateTime.
	* @note Date and time is in specified time zone.
	* @param tz Time zone information.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZSysSync(const DateTime& dt, TimeZone tz, DSTAdjustment dst, bool DST_app = false)
	{
		set(dt, tz, dst, DST_app);
	}

	/**
	* @brief Construct DateTimeTZSysSync from DateTime with specified time zone.
	* @param dt DateTime.
	* @note Date and time is in specified time zone.
	* @param tz Time zone information.
	* @param dst DST adjustment rules and offset.
	* @param DST_app True if DST is applyied to date and time.
	*/
	DateTimeTZSysSync(const DateTime& dt, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false)
	{
		set(dt, tz, dst, DST_app);
	}

	/**
	* @brief Construct DateTimeTZSysSync from any other DateTimeBase or class which has int64_t getRaw() member function.
	* This class can has those optional member functions:
	* - TimeZone getTimeZone() - gets time zone of dt parameter
	* - DSTAdjustment getDST() - gets DST adjustment of dt parameter
	* This constructor is also used as an explicit conversion.
	* @param dt Any other DateTimeBase.
	*/
	template<class DTclass>
	explicit DateTimeTZSysSync(const DTclass& dt) {
		set(dt);
	}

	/**
	* @brief Construct DateTimeTZSysSync from any other DateTimeBase or class which has int64_t getRaw() member function.
	* This class can has those optional member functions:
	* - DSTAdjustment getDST() - gets DST adjustment of dt parameter
	* This constructor is also used as an explicit conversion.
	* @param dt Any other DateTimeBase.
	* @note Date and time is in specific time zone.
	* @param tz Time zone informations, that will be overwritten.
	*/
	template<class DTclass>
	explicit DateTimeTZSysSync(const DTclass& dt, TimeZone tz) {
		set(dt, tz);
	}

	/**
	* @brief Construct DateTimeTZSysSync from any other DateTimeBase or class which has int64_t getRaw() member function.
	* This class can has those optional member functions:
	* - DSTAdjustment getDST() - gets DST adjustment of dt parameter
	* This constructor is also used as an explicit conversion.
	* @param dt Any other DateTimeBase.
	* @note Date and time is in specific time zone.
	* @param tz Time zone informations, that will be overwritten.
	*/
	template<class DTclass>
	explicit DateTimeTZSysSync(const DTclass& dt, TimeZones::TimeZoneName tz) {
		set(dt, tz);
	}

	/**
	* @brief Construct DateTimeTZSysSync from any other DateTimeBase or class which has int64_t getRaw() member function.
	* This constructor is also used as an explicit conversion.
	* @param dt Any other DateTimeBase.
	* @note Date and time is in specific time zone with specific DST.
	* @param tz Time zone informations, that will be overwritten.
	* @param dst DST adjustment rules and offset, that will be overwritten.
	* @param DST_app True if DST is applyied to date and time.
	*/
	template<class DTclass>
	explicit DateTimeTZSysSync(const DTclass& dt, TimeZone tz, DSTAdjustment dst, bool DST_app = false) {
		set(dt, tz, dst, DST_app);
	}

	/**
	* @brief Construct DateTimeTZSysSync from any other DateTimeBase or class which has int64_t getRaw() member function.
	* This constructor is also used as an explicit conversion.
	* @param dt Any other DateTimeBase.
	* @note Date and time is in specific time zone with specific DST.
	* @param tz Time zone informations, that will be overwritten.
	* @param dst DST adjustment rules and offset, that will be overwritten.
	* @param DST_app True if DST is applyied to date and time.
	*/
	template<class DTclass>
	explicit DateTimeTZSysSync(const DTclass& dt, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false) {
		set(dt, tz, dst, DST_app);
	}


	/**
	* @brief Sets current any DateTimeTZSysSync instance from any other DateTime instance. DST adjustment and
	* time zone changes only if dt parameter is DateTimeTZ or DateTimeTZSysSync.
	* @param dt DateTime to set.
	*/
	template<class T, typename dtlib::enable_if<
		((has_getRaw<T>::value && !(has_getRawSyncTime<T>::value)) //Basic
		|| (has_getRawNoSync<T>::value && (has_getRawSyncTime<T>::value))) //Sync
		&& has_getDST<T>::value,
		int>::type = 0>
	void set(const DateTimeBase<T>& dt) {
		if constexpr (has_getTimeZone<T>::value) {
			DateTimeTZBase<DateTimeTZSysSync>::tzInfo = static_cast<const T*>(&dt)->getTimeZone();
		}
		DateTimeTZBase<DateTimeTZSysSync>::adj = static_cast<const T*>(&dt)->getDST();

		if constexpr (has_getRawSyncTime<T>::value) {
			//System synchronized DateTime
			setSyncedTime(dt.getRawNoSync(), static_cast<const T*>(&dt)->getRawSyncTime(), DateTimeTZBase<DateTimeTZSysSync>::adj.isDST());
		}
		else {
			if constexpr (has_getDateTimeEnh<T>::value) {
				DateTime_DST_tuple tup = static_cast<const T*>(&dt)->getDateTimeEnh(); //getting DateTime with isDST flag at same time
				adj.setDST(tup.isDST);
				DateTimeBase<DateTimeTZSysSync>::setRawTime(tup.value);
			}
			else {
				DateTimeBase<DateTimeTZSysSync>::setRawTime(dt.getRaw());
			}
		}
	}

	/**
	* @brief Sets current any DateTimeTZSysSync instance from any other DateTime instance. DST adjustment and
	* time zone changes only if dt parameter is DateTimeTZ or DateTimeTZSysSync.
	* @param dt DateTime to set.
	* @param DST_app True if DST is applyied to date and time.
	*/
	template<class T, typename dtlib::enable_if<
		((has_getRaw<T>::value && !(has_getRawSyncTime<T>::value)) //Basic
		|| (has_getRawNoSync<T>::value && (has_getRawSyncTime<T>::value))) //Sync
		&& !has_getDST<T>::value,
		int>::type = 1>
	void set(const DateTimeBase<T>& dt, bool DST_app = false) {
		if constexpr (has_getTimeZone<T>::value) {
			DateTimeTZBase<DateTimeTZSysSync>::tzInfo = static_cast<const T*>(&dt)->getTimeZone();
		}
		adj.setDST(DST_app);

		if constexpr (has_getRawSyncTime<T>::value) {
			//System synchronized DateTime
			setSyncedTime(dt.getRawNoSync(), static_cast<const T*>(&dt)->getRawSyncTime(), DST_app);
		}
		else {
			DateTimeBase<DateTimeTZSysSync>::setRawTime(dt.getRaw());
		}
	}

	/**
	* @brief Sets current any DateTimeTZSysSync instance from any other DateTime instance. DST adjustment and
	* time zone changes only if dt parameter is DateTimeTZ or DateTimeTZSysSync.
	* @param dt DateTime to set.
	* @param tz Time zone informations, that will be overwritten.
	*/
	template<class T, typename dtlib::enable_if<
		((has_getRaw<T>::value && !(has_getRawSyncTime<T>::value)) //Basic
		|| (has_getRawNoSync<T>::value && (has_getRawSyncTime<T>::value))) //Sync
		&& has_getDST<T>::value,
		int>::type = 0>
	void set(const DateTimeBase<T>& dt, TimeZone tz) {
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = tz;
		DateTimeTZBase<DateTimeTZSysSync>::adj = static_cast<const T*>(&dt)->getDST();

		if constexpr (has_getRawSyncTime<T>::value) {
			//System synchronized DateTime
			setSyncedTime(dt.getRawNoSync(), static_cast<const T*>(&dt)->getRawSyncTime(), DateTimeTZBase<DateTimeTZSysSync>::adj.isDST());
		}
		else {
			if constexpr (has_getDateTimeEnh<T>::value) {
				DateTime_DST_tuple tup = static_cast<const T*>(&dt)->getDateTimeEnh(); //getting DateTime with isDST flag at same time
				adj.setDST(tup.isDST);
				DateTimeBase<DateTimeTZSysSync>::setRawTime(tup.value);
			}
			else {
				DateTimeBase<DateTimeTZSysSync>::setRawTime(dt.getRaw());
			}
		}
	}

	/**
	* @brief Sets current any DateTimeTZSysSync instance from any other DateTime instance. DST adjustment and
	* time zone changes only if dt parameter is DateTimeTZ or DateTimeTZSysSync.
	* @param dt DateTime to set.
	* @param tz Time zone informations, that will be overwritten.
	* @param DST_app True if DST is applyied to date and time.
	*/
	template<class T, typename dtlib::enable_if<
		((has_getRaw<T>::value && !(has_getRawSyncTime<T>::value)) //Basic
		|| (has_getRawNoSync<T>::value && (has_getRawSyncTime<T>::value))) //Sync
		&& !has_getDST<T>::value,
		int>::type = 1>
	void set(const DateTimeBase<T>& dt, TimeZone tz, bool DST_app = false) {
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = tz;
		adj.setDST(DST_app);

		if constexpr (has_getRawSyncTime<T>::value) {
			//System synchronized DateTime
			setSyncedTime(dt.getRawNoSync(), static_cast<const T*>(&dt)->getRawSyncTime(), DST_app);
		}
		else {
			DateTimeBase<DateTimeTZSysSync>::setRawTime(dt.getRaw());
		}
	}

	/**
	* @brief Sets current any DateTimeTZSysSync instance from any other DateTime instance. DST adjustment and
	* time zone changes only if dt parameter is DateTimeTZ or DateTimeTZSysSync.
	* @param dt DateTime to set.
	* @param tz Time zone informations, that will be overwritten.
	*/
	template<class T, typename dtlib::enable_if<
		((has_getRaw<T>::value && !(has_getRawSyncTime<T>::value)) //Basic
		|| (has_getRawNoSync<T>::value && (has_getRawSyncTime<T>::value))) //Sync
		&& has_getDST<T>::value,
		int>::type = 0>
	void set(const DateTimeBase<T>& dt, TimeZones::TimeZoneName tz) {
		set(dt, TimeZone(tz));
	}

	/**
	* @brief Sets current any DateTimeTZSysSync instance from any other DateTime instance. DST adjustment and
	* time zone changes only if dt parameter is DateTimeTZ or DateTimeTZSysSync.
	* @param dt DateTime to set.
	* @param tz Time zone informations, that will be overwritten.
	* @param DST_app True if DST is applyied to date and time.
	*/
	template<class T, typename dtlib::enable_if<
		((has_getRaw<T>::value && !(has_getRawSyncTime<T>::value)) //Basic
		|| (has_getRawNoSync<T>::value && (has_getRawSyncTime<T>::value))) //Sync
		&& !has_getDST<T>::value,
		int>::type = 1>
	void set(const DateTimeBase<T>& dt, TimeZones::TimeZoneName tz, bool DST_app = false) {
		set(dt, TimeZone(tz), DST_app);
	}

	/**
	* @brief Sets current any DateTimeTZSysSync instance from any other DateTime instance. DST adjustment and
	* time zone changes only if dt parameter is DateTimeTZ or DateTimeTZSysSync.
	* @param dt DateTime to set.
	* @param tz Time zone informations, that will be overwritten.
	* @param dst DST adjustment rules and offset, that will be overwritten.
	* @param DST_app True if DST is applyied to date and time.
	*/
	template<class T, typename dtlib::enable_if<
		((has_getRaw<T>::value && !(has_getRawSyncTime<T>::value)) //Basic
		|| (has_getRawNoSync<T>::value && (has_getRawSyncTime<T>::value))), //Sync
		int>::type = 1>
	void set(const DateTimeBase<T>& dt, TimeZone tz, DSTAdjustment dst, bool DST_app = false) {
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = tz;
		DateTimeTZBase<DateTimeTZSysSync>::adj = dst;

		if constexpr (has_getRawSyncTime<T>::value) {
			//System synchronized DateTime
			setSyncedTime(dt.getRawNoSync(), static_cast<const T*>(&dt)->getRawSyncTime(), DST_app);
		}
		else {
			if constexpr (has_getDateTimeEnh<T>::value) {
				DateTime_DST_tuple tup = static_cast<const T*>(&dt)->getDateTimeEnh(); //getting DateTime with isDST flag at same time
				adj.setDST(DST_app);
				DateTimeBase<DateTimeTZSysSync>::setRawTime(tup.value);
			}
			else {
				adj.setDST(DST_app);
				DateTimeBase<DateTimeTZSysSync>::setRawTime(dt.getRaw());
			}
		}
	}

	/**
	* @brief Sets current any DateTimeTZSysSync instance from any other DateTime instance. DST adjustment and
	* time zone changes only if dt parameter is DateTimeTZ or DateTimeTZSysSync.
	* @param dt DateTime to set.
	* @param tz Time zone informations, that will be overwritten.
	* @param dst DST adjustment rules and offset, that will be overwritten.
	* @param DST_app True if DST is applyied to date and time.
	*/
	template<class T, typename dtlib::enable_if<
		((has_getRaw<T>::value && !(has_getRawSyncTime<T>::value)) //Basic
		|| (has_getRawNoSync<T>::value && (has_getRawSyncTime<T>::value))), //Sync
		int>::type = 1>
	void set(const DateTimeBase<T>& dt, TimeZones::TimeZoneName tz, DSTAdjustment dst, bool DST_app = false) {
		set(dt, TimeZone(tz), dst, DST_app);
	}

	/**
	* @brief Sets current any DateTimeTZSysSync instance from any other DateTime instance. DST adjustment and
	* time zone changes only if dt parameter is DateTimeTZ or DateTimeTZSysSync.
	* @param dt DateTime to set.
	* @param tz Time zone informations, that will be overwritten.
	* @param dst DST adjustment rules and offset, that will be overwritten.
	*/
	template<class T, typename dtlib::enable_if<
		((has_getRaw<T>::value && !(has_getRawSyncTime<T>::value)) //Basic
			|| (has_getRawNoSync<T>::value && (has_getRawSyncTime<T>::value))), //Sync
		int>::type = 1>
	void set(const DateTimeBase<T>& dt, TimeZone tz, DSTAdjustment dst) {
		DateTimeTZBase<DateTimeTZSysSync>::tzInfo = tz;
		DateTimeTZBase<DateTimeTZSysSync>::adj = dst;

		if constexpr (has_getRawSyncTime<T>::value) {
			//System synchronized DateTime
			setSyncedTime(dt.getRawNoSync(), static_cast<const T*>(&dt)->getRawSyncTime(), DateTimeTZBase<DateTimeTZSysSync>::adj.isDST());
		}
		else {
			if constexpr (has_getDateTimeEnh<T>::value) {
				DateTime_DST_tuple tup = static_cast<const T*>(&dt)->getDateTimeEnh(); //getting DateTime with isDST flag at same time
				adj.setDST(tup.isDST);
				DateTimeBase<DateTimeTZSysSync>::setRawTime(tup.value);
			}
			else {
				adj.setDST(DateTimeTZBase<DateTimeTZSysSync>::adj.isDST());
				DateTimeBase<DateTimeTZSysSync>::setRawTime(dt.getRaw());
			}
		}
	}

	/**
	* @brief Sets current any DateTimeTZSysSync instance from any other DateTime instance. DST adjustment and
	* time zone changes only if dt parameter is DateTimeTZ or DateTimeTZSysSync.
	* @param dt DateTime to set.
	* @param tz Time zone informations, that will be overwritten.
	* @param dst DST adjustment rules and offset, that will be overwritten.
	*/
	template<class T, typename dtlib::enable_if<
		((has_getRaw<T>::value && !(has_getRawSyncTime<T>::value)) //Basic
			|| (has_getRawNoSync<T>::value && (has_getRawSyncTime<T>::value))), //Sync
		int>::type = 1>
	void set(const DateTimeBase<T>& dt, TimeZones::TimeZoneName tz, DSTAdjustment dst) {
		set(dt, TimeZone(tz), dst);
	}

	/**
	* @brief Gets UTC DateTimeSysSync.
	*/
	DateTimeSysSync getSysSyncUTC() const;

	/**
	* @brief Sets UTC DateTime.
	* @param dt UTC DateTime. Time zone or DST is ignored here, only value as it is is used.
	*/
	template<class T>
	void setUTC(const DateTimeBase<T>& dt) {
		
		if constexpr (has_getRawSyncTime<T>::value) {
			//System synchronized DateTime
			int64_t dateTime = dt.getRawNoSync();
			int64_t tzOffset = (int64_t)getTimeZoneOffset();
			dateTime += tzOffset; //Adding time zone offset

			setSyncedTime(dateTime, static_cast<const T*>(&dt)->getRawSyncTime(), false);
		}
		else {
			DateTimeTZBase<DateTimeTZSysSync>::setUTC(dt);
		}
	}

	/**
	* @brief Calculates next DST transition date and time. Thanks to this function, DST region does not need to be
	* calculated and check every time, when reading some date and time field.
	* @note Is is recommended to call this function at least once per day. It can be also called often, it does not
	* decrease performance so much, because next DST transition time is calculated only if needed.
	* @return Returns raw value at time, when calculation was done.
	*/
	int64_t recalcDST();

	/**
	* @brief Gets DateTime and DST flag tuple. This member function gets isDST flag at time, when DateTime was get from
	* synchronized time.
	*/
	DateTime_DST_tuple getDateTimeEnh() const;

	/**
	* @brief Gets time, that elapsed from last sync. Time is synchronized when
	* any kind of set method is called (set(), setHours(), ...).
	* @return Returns time, that elapsed from last sync as TimeSpan.
	*/
	inline TimeSpan getTimeFromLastSync() const {
		DT_SYNC_TYPE currentTime = DT_SYNC_FUNC();
		return TimeSpan((int64_t)((currentTime - syncTime) * DT_SYNC_RESOLUTION));
	}

	/**
	* @brief Gets system DST adjustment rules and offset.
	* @return Returns system DST adjustment rules and offset.
	*/
	static inline DSTAdjustment getSystemDST() {
		return DSTAdjustment::getSystemDST();
	}

	/**
	* @brief Gets system time zone.
	* @return Returns system time zone.
	*/
	static inline TimeZone getSystemTZ() {
		return TimeZone::getSystemTZ();
	}

#if (DT_SUPPORTS_NOW != 0)
	/**
	* @brief Gets local system time. For accurate time measuring it is recommended to get this
	* value before measuring, see example:
	* @code{.cpp}
	* //WRONG:
	* DateTime start = (DateTime)DateTimeSysSync::now();
	* //Code to be measured is here
	* DateTime end = (DateTime)DateTimeSysSync::now();
	* TimeSpan duration = end - start; //Duration value will be greather than real duration, because now() is slow
	*
	* //OK:
	* DateTimeSysSync clock = DateTimeSysSync::now();
	* DateTime start = (DateTime)clock;
	* //Code to be measured is here
	* DateTime end = (DateTime)clock;
	* TimeSpan duration = end - start; //Duration will be almost same as real duration
	* @endcode
	*/
	inline static DateTimeTZSysSync now() {
		return getSysTime();
	}

	/**
	* @brief Gets local system time. For accurate time measuring it is recommended to get this
	* value before measuring, see example:
	* @code{.cpp}
	* //WRONG:
	* DateTime start = (DateTime)DateTimeSysSync::nowUTC();
	* //Code to be measured is here
	* DateTime end = (DateTime)DateTimeSysSync::nowUTC();
	* TimeSpan duration = end - start; //Duration value will be greather than real duration, because nowUTC() is slow
	*
	* //OK:
	* DateTimeSysSync clock = DateTimeSysSync::nowUTC();
	* DateTime start = (DateTime)clock;
	* //Code to be measured is here
	* DateTime end = (DateTime)clock;
	* TimeSpan duration = end - start; //Duration will be almost same as real duration
	* @endcode
	*/
	inline static DateTimeTZSysSync nowUTC() {
		return getSysTimeUTC();
	}
#endif // (DT_SUPPORTS_NOW != 0)

	/**
	* @brief Gets raw sync time.
	*/
	inline DT_SYNC_TYPE getRawSyncTime() const {
		return syncTime;
	}

	/**
	* @brief Returns true if current sync time is in DST region or not.
	* @note isDST() can be called instead.
	*/
	bool isSyncDST() const;

	/**
	* @brief Returns true if current time is over next DST transition time and recalcDST should be called.
	* @note This function is called in recalcDST() and DST offset is automatically recalculated only if this function returns true.
	*/
	bool isOverNextTrans() const;

	/**
	* @brief Gets DST or time zone offset, that was not applied to rawValue (which can be obtained calling getRawTimeNoSync()) yet.
	* @param currentTime Current sync time.
	* @warning This method should not be used, use unappliedOffset() with no parameter instead.
	* @return Returns DST or time zone offset in microseconds, that was not applied to raw time yet. Zero is returned, if isOverNextTrans() is false.
	*/
	int64_t unappliedOffset(DT_SYNC_TYPE currentTime) const;

	/**
	* @brief Gets DST or time zone offset, that was not applied to rawValue (which can be obtained calling getRawTimeNoSync()) yet.
	* @return Returns DST or time zone offset in microseconds, that was not applied to raw time yet. Zero is returned, if isOverNextTrans() is false.
	*/
	inline int64_t unappliedOffset() const {
		return unappliedOffset(DT_SYNC_FUNC());
	}


	template<typename T, typename dtlib::enable_if<has_getRawSyncTime<T>::value && !(has_getCurrentDSTOffsetMinutes<T>::value && has_getTimeZoneOffsetMinutes<T>::value), int>::type = 1>
	TimeSpan operator-(const DateTimeBase<T>& raw) const {
		
		int64_t syncOffset = (getRawSyncTime() - static_cast<const T*>(&raw)->getRawSyncTime()) * DT_SYNC_RESOLUTION;

		//Appling unapplied offset to raw values.
		DT_SYNC_TYPE currSyncTime = DT_SYNC_FUNC();
		int64_t thisVal = DateTimeBase<DateTimeTZSysSync>::getRawTimeNoSync() + unappliedOffset(currSyncTime);
		int64_t othVal;
		if constexpr (has_unappliedOffset<T>::value) othVal = raw.getRawNoSync() + raw.unappliedOffset(currSyncTime);
		else othVal = raw.getRawNoSync();

		return TimeSpan(thisVal - othVal - syncOffset);
	}

	template<typename T, typename dtlib::enable_if<has_getRawSyncTime<T>::value && (has_getDSTOffsetMinutes<T>::value && has_getDST<T>::value && has_getTimeZoneOffsetMinutes<T>::value), int>::type = 2>
	TimeSpan operator-(const DateTimeBase<T>& raw) const {
		int16_t ofs1 = getTimeZoneOffsetMinutes() + ((DateTimeTZBase<DateTimeTZSysSync>::adj.isDST()) ? DateTimeTZBase<DateTimeTZSysSync>::getDSTOffsetMinutes() : 0);
		int16_t ofs2 = static_cast<const T*>(&raw)->getTimeZoneOffsetMinutes() + ((static_cast<const T*>(&raw)->getDST().isDST()) ? static_cast<const T*>(&raw)->getDSTOffsetMinutes() : 0);
		int64_t syncOffset = (getRawSyncTime() - static_cast<const T*>(&raw)->getRawSyncTime()) * DT_SYNC_RESOLUTION;
		int64_t offset = ((int64_t)(ofs1 - ofs2)) * MINUTE + syncOffset;

		//Appling unapplied offset to raw values.
		DT_SYNC_TYPE currSyncTime = DT_SYNC_FUNC();
		int64_t thisVal = DateTimeBase<DateTimeTZSysSync>::getRawTimeNoSync() + unappliedOffset(currSyncTime);
		int64_t othVal;
		if constexpr (has_unappliedOffset<T>::value) othVal = raw.getRawNoSync() + static_cast<const T*>(&raw)->unappliedOffset(currSyncTime);
		else othVal = raw.getRawNoSync();

		return TimeSpan(thisVal - othVal - offset);
	}

	//Comparison operators

	template<class T>
	bool operator==(DateTimeBase<T>& dt2) const {
		return compare(dt2) == 0;
	}

	template<class T>
	bool operator!=(DateTimeBase<T>& dt2) const {
		return compare(dt2) != 0;
	}

	template<class T>
	bool operator>=(DateTimeBase<T>& dt2) const {
		return compare(dt2) >= 0;
	}

	template<class T>
	bool operator<=(DateTimeBase<T>& dt2) const {
		return compare(dt2) <= 0;
	}

	template<class T>
	bool operator>(DateTimeBase<T>& dt2) const {
		return compare(dt2) > 0;
	}

	template<class T>
	bool operator<(DateTimeBase<T>& dt2) const {
		return compare(dt2) < 0;
	}

protected:

	template<class T> friend class ::DateTimeBase;
	template<class T> friend class ::DateTimeTZBase;
	friend class DateTimeSysSync;
	template<class T, typename Y> friend struct has_getRawTimeSync;
	template<class T, typename Y> friend struct has_preSetSync;
	template<class T, typename Y> friend struct has_getAndPreSetSync;
	template<class T, typename Y> friend struct has_isSyncDST;
	template<class T, typename Y> friend struct has_calcNextTransOnSet;
	//template<class T, typename Y> friend struct has_ctorBeforeCtor;
	template<class T, typename Y> friend struct has_recalcDST;

	/**
	* @brief Updates raw value and returns it.
	* @return Returns synchronized raw value.
	*/
	int64_t getRawTimeSync() const;

	/**
	* @brief Synchronizes value with clock before setting new raw value.
	* @note This has to be called before setting new raw value, but should not be called
	* when some offset is applyied to raw value.
	*/
	void preSetSync();

	/**
	* @brief preSetSync() and getRawTimeSync() combined in one method.
	* @return Returns synchronized raw value.
	*/
	int64_t getAndPreSetSync();

	DT_SYNC_TYPE syncTime;
	
	/**
	* @brief Next DST transition date and time.
	*/
	DateTime nextDSTTransitionDT = DateTime::MaxValue;

	/**
	* @brief Calculates next transition time on set.
	* @param newRaw Raw value (can has applyied DST offset).
	* @param is_DST True if newRaw has applyied DST offset.
	*/
	void calcNextTransOnSet(int64_t newRaw, bool is_DST);

	/**
	* @brief This member function is called before class constructor in DateTimeBase.
	*/
	//void ctorBeforeCtor();

	/**
	* @brief Sets synchronized DateTime value from another synchronized DateTime.
	* @param baseRawValue Non-synchronized raw value of synchronized DateTime captured at syncTime_new time.
	* @param syncTime_new SyncTime variable, which was captured at same time, baseRawValue was set.
	* @param DST_app True if DST is applyied to baseRawValue.
	*/
	void setSyncedTime(int64_t baseRawValue, DT_SYNC_TYPE syncTime_new, bool DST_app);

	/**
	* @brief Compares this DateTime with another date time, which is synced by system time.
	* @param dt2 DateTime synced with system time, e.g. DateTimeSysSync.
	* @return Returns -1 if current DateTime value is lower, 0 if both are equal, 1 if current value is greather.
	*/
	template<class T, typename dtlib::enable_if<has_getRawSyncTime<T>::value && !(has_getDSTOffsetMinutes<T>::value && has_getDST<T>::value && has_getTimeZoneOffsetMinutes<T>::value), int>::type = 1>
	int8_t compare(DateTimeBase<T>& dt2) const{
		int64_t syncOffset = (getRawSyncTime() - static_cast<const T*>(&dt2)->getRawSyncTime()) * DT_SYNC_RESOLUTION;

		//Appling unapplied offset to raw values.
		DT_SYNC_TYPE currSyncTime = DT_SYNC_FUNC();
		int64_t thisVal = DateTimeBase<DateTimeTZSysSync>::getRawTimeNoSync() + unappliedOffset(currSyncTime);
		int64_t othVal;
		if constexpr (has_unappliedOffset<T>::value) othVal = dt2.getRawNoSync() + static_cast<const T*>(&dt2)->unappliedOffset(currSyncTime);
		else othVal = dt2.getRawNoSync();

		thisVal -= syncOffset;

		if (thisVal < othVal) {
			return -1;
		}
		else {
			return thisVal > othVal;
		}
	}

	/**
	* @brief Compares this DateTime with another date time, which is synced by system time and has TZ and DST.
	* @param dt2 DateTime synced with system time and has TZ and DST, e.g. DateTimeTZSysSync.
	* @return Returns -1 if current DateTime value is lower, 0 if both are equal, 1 if current value is greather.
	*/
	template<class T, typename dtlib::enable_if<has_getRawSyncTime<T>::value && (has_getDSTOffsetMinutes<T>::value && has_getDST<T>::value && has_getTimeZoneOffsetMinutes<T>::value), int>::type = 2>
	int8_t compare(DateTimeBase<T>& dt2) const {
		int16_t ofs1 = getTimeZoneOffsetMinutes() + ((DateTimeTZBase<DateTimeTZSysSync>::adj.isDST()) ? DateTimeTZBase<DateTimeTZSysSync>::getDSTOffsetMinutes() : 0);
		int16_t ofs2 = static_cast<const T*>(&dt2)->getTimeZoneOffsetMinutes() + ((static_cast<const T*>(&dt2)->getDST().isDST()) ? static_cast<const T*>(&dt2)->getDSTOffsetMinutes() : 0);
		int64_t syncOffset = (getRawSyncTime() - static_cast<const T*>(&dt2)->getRawSyncTime()) * DT_SYNC_RESOLUTION;
		int64_t offset = ((int64_t)(ofs1 - ofs2)) * MINUTE + syncOffset;

		//Appling unapplied offset to raw values.
		DT_SYNC_TYPE currSyncTime = DT_SYNC_FUNC();
		int64_t thisVal = DateTimeBase<DateTimeTZSysSync>::getRawTimeNoSync() + unappliedOffset(currSyncTime);
		int64_t othVal;
		if constexpr (has_unappliedOffset<T>::value) othVal = dt2.getRawNoSync() + static_cast<const T*>(&dt2)->unappliedOffset(currSyncTime);
		else othVal = dt2.getRawNoSync();

		thisVal -= offset;

		if (thisVal < othVal) {
			return -1;
		}
		else {
			return thisVal > othVal;
		}
	}


#if (DT_SUPPORTS_NOW != 0)
	/**
	* @brief Gets system time in UTC.
	* @note This function works only for: Windows, Linux, Mac OS, ESP8266 and ESP32
	* @return Returns system time in UTC.
	*/
	inline static DateTimeTZSysSync getSysTimeUTC() {
		return DateTimeTZSysSync(DateTimeSysSync::getSysTimeUTC());
	}

	/**
	* @brief Gets local system time.
	* @note This function works only for: Windows, Linux, Mac OS, ESP8266 and ESP32
	* @return Returns local system time.
	*/
	static DateTimeTZSysSync getSysTime();
#endif // (DT_SUPPORTS_NOW != 0)
};

#endif // !DATE_TIME_TZ_SYS_SYNC_H