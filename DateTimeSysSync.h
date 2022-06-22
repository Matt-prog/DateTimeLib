/**
 * @file DateTimeSysSync.h
 * @brief This file contains class DateTimeSysSync. It is DateTime class,
 * which is synchronized by software.
 *
 * @see DateTimeSysSync
 *
 * # Credits
 * @author Matej Fitoš
 * @date Jun 4, 2022
 */

#ifndef DATE_TIME_SYS_SYNC_H
#define DATE_TIME_SYS_SYNC_H


#include "DateTimeBase.h"

#if DT_UNDER_OS > 0
#include <chrono>

#else


#endif

/**
* @class DateTimeSysSync
* @brief DateTime class, which is synchronized with system clock. It means, that any time
* you read some field from this class, it will be always up to date. This class has to be
* initialized with valid date and time to return valid value. Synchronization works on the
* principle that when reading a time and date value, the time that has elapsed since the last
* setting of any time or date field is added to it. On computer with any operational system
* is used chrono library for synchronization. On Arduino or STM32 is used micros64() or
* millis() function. This value may be inaccurate when using micros64() or millis() function,
* because they don't have to always return accurate value. When using millis() for synchronization,
* microseconds field stays unchanged.
* 
* ## Extra member functions
* + **On Windows OS, Linux or Mac OS:**
*   - now() returns current system time.
*   @note systemDSTAdjustment cannot be retrieved right now on Linux or Mac OS.
* 
* @see DateTime
* @see DateTimeBase
*/
class DateTimeSysSync : public DateTimeRawBase<DateTimeSysSync>
{
public:

	using DateTimeRawBase<DateTimeSysSync>::DateTimeRawBase; //Using base constructors

	/**
	* @brief Default constructor, which set DateTime to the 0001/01/01 00:00:00.000000 with no time zone and DST adjustment.
	*/
	DateTimeSysSync() : DateTimeRawBase<DateTimeSysSync>()
	{ }

	/**
	* @brief Construct DateTimeSysSync from DateTime. This constructor is also used as an implicit conversion.
	* @param dt DateTime
	*/
	DateTimeSysSync(const DateTime& dt) {
		DateTimeBase<DateTimeSysSync>::setRawTime(dt.getRaw());
	}

	/**
	* @brief Construct DateTimeSysSync from any other DateTimeBase or class, which has int64_t getRaw() member function and is not synchronized.
	* This constructor is also used as an explicit conversion.
	* @param dt Any other DateTimeBase, which have not synchronized time.
	*/
	template<class DTclass, typename dtlib::enable_if<has_getRaw<DTclass>::value && !(has_getRawNoSync<DTclass>::value && has_getRawSyncTime<DTclass>::value), int>::type = 0>
	explicit DateTimeSysSync(const DTclass& dt) {
		DateTimeBase<DateTimeSysSync>::setRawTime(dt.getRaw());
	}

	/**
	* @brief Construct DateTimeSysSync from any other DateTimeBase or class, which has int64_t getRawNoSync() member function and 
	* DT_SYNC_TYPE getRawSyncTime() member function.
	* This constructor is also used as an explicit conversion.
	* @param dt Any other DateTimeBase, which have synchronized time.
	*/
	template<class DTclass, typename dtlib::enable_if<has_getRawNoSync<DTclass>::value && has_getRawSyncTime<DTclass>::value, int>::type = 1>
	explicit DateTimeSysSync(const DTclass& dt) {
		DateTimeBase::setRawTime(dt.getRawNoSync());
		syncTime = static_cast<const DTclass*>(&dt)->getRawSyncTime();
	}

	/**
	* @brief Sets current any DateTime instance from any other DateTime instance, which is synced by software.
	* @param dt DateTime to set.
	*/
	template<class T, typename dtlib::enable_if<has_getRawNoSync<T>::value && has_getRawSyncTime<T>::value, int>::type = 1>
	void set(const DateTimeBase<T>& dt) {
		DateTimeBase::setRawTime(dt.getRawNoSync());
		syncTime = static_cast<const T*>(&dt)->getRawSyncTime();
	}

	/**
	* @brief Gets time, that elapsed from last sync. Time is synchronized when
	* any kind of set method is called (set(), setHours(), ...) except functions,
	* which adds some offset (addHours(), + operator, ...).
	* @return Returns time, that elapsed from last sync as TimeSpan.
	*/
	inline TimeSpan getTimeFromLastSync() const {
		DT_SYNC_TYPE currentTime = DT_SYNC_FUNC();
		return TimeSpan((int64_t)((currentTime - syncTime) * DT_SYNC_RESOLUTION));
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
	* @note It is not recommended to use this function,
	* because DST may not be adjusted when transition happends after reading this value.
	* Rather use DateTimeTZSysSync:now(), which contains also DST adjustment rules.
	*/
	inline static DateTimeSysSync now() {
		bool isDST;
		return getSysTime(isDST);
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
	inline static DateTimeSysSync nowUTC() {
		return getSysTimeUTC();
	}
#endif // (DT_SUPPORTS_NOW != 0)

#if DT_SUPPORTS_SET_SYS_TIME != 0
#if defined(ESP32) || defined(ESP8266)

	/**
	* @brief Sets system time as local time.
	* @param[in] time Local time to set.
	* @note If time is DateTimeTZ or DateTimeTZSysSync type, system time zone and DST adjustment will be set too.
	*/
	template<class T>
	static typename dtlib::enable_if<(has_getTimeZone<T>::value || has_getDST<T>::value), void>::type setSystemTime(const DateTimeBase<T>& time) {
		setSystemTimeUTC(static_cast<const T*>(&time)->getUTC()); //Set time as fast as possible
		
		//Then set time zone
		TimeZoneInfo tzinfo = TimeZoneInfo::getCurrentSystemTZInfo();
		tzinfo.timeZone = static_cast<const T*>(&time)->getTimeZone();
		tzinfo.DST = static_cast<const T*>(&time)->getDST();
		TimeZoneInfo::setSystemTZInfo(tzinfo);
	}

	/**
	* @brief Sets system time as local time.
	* @param[in] time Local time to set.
	* @param[in] isDST True if current time has DST applied.
	* @note Time zone and DST adjustment stays unchanged.
	*/
	template<class T>
	static typename dtlib::enable_if<!(has_getTimeZone<T>::value || has_getDST<T>::value), void>::type setSystemTime(const DateTimeBase<T>& time, bool isDST) {
		DateTimeSysSync utc = (DateTimeSysSync)time;

		TimeZoneInfo::loadSystemTZInfo();
		TimeZoneInfo& tzinfo = TimeZoneInfo::getSystemTZInfo();
		utc -= tzinfo.timeZone.getTimeZoneOffset();
		if (isDST) {
			utc -= tzinfo.DST.getDSTOffset();
		}
		setSystemTimeUTC((DateTime)utc);
	}

	/**
	* @brief Sets system time as UTC.
	* @param[in] time UTC time to set.
	*/
	inline static void setSystemTimeUTC(const DateTime& time) {
		struct timeval tv = time.get_timeval();
		settimeofday(&tv, NULL);
	}

#else
#error "Setting system time is not implemented!"
#endif // defined(ESP32) || defined(ESP8266)

#endif // DT_SUPPORTS_SET_SYS_TIME != 0





	/**
	* @brief Gets raw sync time.
	*/
	inline DT_SYNC_TYPE getRawSyncTime() const {
		return syncTime;
	}


	template<typename T, typename dtlib::enable_if<has_getRawSyncTime<T>::value, int>::type = 1>
	TimeSpan operator-(const DateTimeBase<T>& dt2) const {
		int64_t syncOffset = (getRawSyncTime() - dt2.getRawSyncTime()) * DT_SYNC_RESOLUTION;

		//Appling unapplied offset to raw values.
		int64_t thisVal = DateTimeBase<DateTimeSysSync>::getRawTimeNoSync();
		int64_t othVal;
		CONSTEXPR_IF (has_unappliedOffset<T>::value) {
			DT_SYNC_TYPE currSyncTime = DT_SYNC_FUNC();
			othVal = dt2.getRawNoSync() + static_cast<const T*>(&dt2)->unappliedOffset(currSyncTime);
		}
		else othVal = dt2.getRawNoSync();

		return TimeSpan(thisVal - othVal - syncOffset);
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
	bool operator<(DateTimeBase<T>& dt2) const {
		return compare(dt2) < 0;
	}

	template<class T>
	bool operator>(DateTimeBase<T>& dt2) const {
		return compare(dt2) > 0;
	}

protected:

	friend class DateTimeTZSysSync;
	template<class T> friend class ::DateTimeBase;
	template<typename T> friend class DateTimeTZBase;
	template<class T, typename Y> friend struct has_getRawTimeSync;
	template<class T, typename Y> friend struct has_preSetSync;
	template<class T, typename Y> friend struct has_getAndPreSetSync;

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

	/**
	* @brief Compares this DateTime with another date time, which is synced by system time.
	* @param dt2 DateTime synced with system time, e.g. DateTimeTZSysSync.
	* @return Returns -1 if current DateTime value is lower, 0 if both are equal, 1 if current value is greather.
	*/
	template<class T, typename dtlib::enable_if<has_getRawSyncTime<T>::value, int>::type = 0>
	int8_t compare(DateTimeBase<T>& dt2) const {
		int64_t syncOffset = (getRawSyncTime() - static_cast<const T*>(&dt2)->getRawSyncTime()) * DT_SYNC_RESOLUTION;

		//Appling unapplied offset to raw values.
		int64_t thisVal = DateTimeBase<DateTimeSysSync>::getRawTimeNoSync();
		int64_t othVal;
		CONSTEXPR_IF (has_unappliedOffset<T>::value) {
			DT_SYNC_TYPE currSyncTime = DT_SYNC_FUNC();
			othVal = dt2.getRawNoSync() + static_cast<const T*>(&dt2)->unappliedOffset(currSyncTime);
		}
		else othVal = dt2.getRawNoSync();

		thisVal -= syncOffset;

		if (thisVal < othVal) {
			return -1;
		}
		else {
			return thisVal > othVal;
		}
	}


	DT_SYNC_TYPE syncTime;

#if (DT_SUPPORTS_NOW != 0)
	/**
	* @brief Gets system time in UTC.
	* @note This function works only for: Windows, Linux, Mac OS, ESP8266 and ESP32
	* @return Returns system time in UTC.
	*/
	static DateTimeSysSync getSysTimeUTC();

	/**
	* @brief Gets local system time.
	* @note This function works only for: Windows, Linux, Mac OS, ESP8266 and ESP32
	* @param[out] isDST True if current date time had applied DST offset at capture time.
	* @return Returns local system time.
	*/
	static DateTimeSysSync getSysTime(bool& isDST);
#endif //(DT_SUPPORTS_NOW != 0)
};

/*template<bool val> class tester;
tester<has_preSetSync<DateTimeSysSync, void(DateTimeSysSync::*)()>::value> t;*/

#endif // !DATE_TIME_SYS_SYNC_H