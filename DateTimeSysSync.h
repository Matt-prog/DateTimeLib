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

#if defined(DT_GET_CURRENT_FUNC)
	/**
	* @brief Gets local time.
	*/
	inline static DateTimeSysSync now() {
		bool isDST;
		return DT_GET_CURRENT_FUNC(isDST);
	}
#endif // !DT_GET_CURRENT_FUNC

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
		if constexpr (has_unappliedOffset<T>::value) {
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

	template<class T> friend class ::DateTimeBase;
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
		if constexpr (has_unappliedOffset<T>::value) {
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

	friend class DateTimeTZSysSync;
	template<typename T> friend class DateTimeTZBase;

	DT_SYNC_TYPE syncTime;

#if DT_UNDER_OS > 0
	static DateTimeSysSync getCurrentTime(bool& isDST);
#endif
};

/*template<bool val> class tester;
tester<has_preSetSync<DateTimeSysSync, void(DateTimeSysSync::*)()>::value> t;*/

#endif // !DATE_TIME_SYS_SYNC_H