/**
 * @file DateTimeBase.h
 * @brief This file contains base class of DateTime.
 *
 * @see DateTimeBase
 * @see DateTimeRawBase
 * @see DateTime
 *
 * # Credits
 * @author Matej Fitoš
 * @date Jun 4, 2022
 */


#ifndef _DATE_TIME_BASE_H
#define _DATE_TIME_BASE_H

#include "TimeZone.h"


/**
* @class DateTimeBase
* @brief Base class of all DateTime classes. It does not contains any field, but only member functions. Thanks this, it acts as
* interface (abstract) class. This class won't works as it is, but has to be derived. Static polymorphism is used to override
* member functions. That's why there are no "virtual" keywords. DateTimeBase uses count of microseconds from start of the epoch
* as raw value. This value is represented by int64_t.
* 
* ## How to derive from this class
* At first, derived class has to contains those member functions:
* - <i>int64_t getRawValueDer() const</i> - which gets raw value (microseconds from start of the epoch),
* - <i>void setRawValueDer(int64_t)</i> - which sets value to raw value (microseconds from start of the epoch),
* - <i>void addRawValueDer(int64_t)</i> - which adds value to raw value (microseconds from start of the epoch).
* For example see DateTimeRawBase class. Your class can optionally contains those member functions:
* + <b>For system synchronization:</b>
*   - <i>int64_t getRawTimeSync() const</i> - which gets raw value after synchronization with the system clock (microseconds from start of the epoch).
*   - <i>void preSetSync()</i> - captures system clock value before setting new value or any date and time field (microseconds from start of the epoch).
*   - <i>int64_t getAndPreSetSync()</i> - combines getRawTimeSync() and getAndPreSetSync() to one function for better performance (microseconds from start of the epoch).
* + <b>For time zone and DST adjustment:</b>
*   - <i>void setRawTimeTD(int64_t)</i> - which sets raw value and checks DST adjustment (microseconds from start of the epoch),
*   - <i>void addRawTimeTD(int64_t)</i> - which adds value to raw value and checks DST adjustment (microseconds from start of the epoch).
* Derived class declaration has to be like this:
* @code{.cpp}
* class YourClassName : public DateTimeBase<YourClassName>{
*   //Your code and functions goes here
* };
* @endcode
* Or like this, when you want to derive derived class:
* @code{.cpp}
* template<class derivedClass>
* class YourClassName : public DateTimeBase<derivedClass>{
*   //Your code and functions goes here
* };
* @endcode
* 
* @see DateTimeRawBase
* @see DateTime
* @see DateTimeSysSync
* @see DateTimeTZBase
* @see DateTimeTZ
* @see DateTimeTZSysSync
* @see TimeSpan
*/
template<class derivedSyncClass>
class DateTimeBase {
public:
    static_assert(!has_getRawValueDer<derivedSyncClass>::value, "Inherited class from DateTimeBase must have 'int64_t getRawValueDer() const' member function.");
    static_assert(!has_setRawValueDer<derivedSyncClass>::value, "Inherited class from DateTimeBase must have 'void setRawValueDer(int64_t raw)' member function.");
    static_assert(!has_addRawValueDer<derivedSyncClass>::value, "Inherited class from DateTimeBase must have 'void addRawValueDer(int64_t raw)' member function.");

    /**
    * @brief Default constructor, which set DateTime to the 0001/01/01 00:00:00.000000 with no time zone and DST adjustment.
    */
    DateTimeBase()
    {
        syncBeforeSet();
    }

    /**
    * @brief Constructor, which sets DateTime from raw value.
    * @param raw Raw value in microseconds from the begin of epoch (0001/1/1).
    */
    explicit DateTimeBase(int64_t raw)
    {
        syncBeforeSet();
        setRawTime(raw);
    }

    /**
    * @brief Sets DateTime from raw value.
    * @param raw Raw value in microseconds from the begin of epoch (0001/1/1).
    */
    inline void setRaw(int64_t raw) {
        syncBeforeSet();
        setRawTime(raw);
    }

    /**
    * @brief Gets raw date time value.
    * @return Returns raw value in microseconds from the begin of epoch (0001/1/1).
    */
    inline int64_t getRaw() const {
        return getRawTime();
    }

    /**
    * @brief Gets raw date time value, which is not synchronized.
    * @return Returns raw value in microseconds from the begin of epoch (0001/1/1).
    */
    inline int64_t getRawNoSync() const {
        return getRawTimeNoSync();
    }

    /**
    * @brief Sets current any DateTime instance from any other DateTime instance.
    * @note This member function copyies only raw value.
    */
    template<class T>
    void set(const DateTimeBase<T>& dt) {
        syncBeforeSet();
        setRawTime(dt.getRawTime());
    }

    /**
    * @brief Explicit conversion operator to int64_t, which returns synchronized raw value, same getRaw() function. 
    */
    explicit operator int64_t() const{
        return getRaw();
    }

    //Arithmetic operators

    derivedSyncClass operator+(const TimeSpan& ts2) const {
        derivedSyncClass ret = *static_cast<const derivedSyncClass*>(this);
        ret.addRawTime(ts2.getRaw());
        return ret;
    }

    derivedSyncClass operator+(const int64_t raw) const {
        derivedSyncClass ret = *static_cast<const derivedSyncClass*>(this);
        ret.addRawTime(raw);
        return ret;
    }

    derivedSyncClass operator-(const TimeSpan& ts2) const {
        derivedSyncClass ret = *static_cast<const derivedSyncClass*>(this);
        ret.addRawTime(-ts2.getRaw());
        return ret;
    }

    derivedSyncClass operator-(const int64_t raw) const {
        derivedSyncClass ret = *static_cast<const derivedSyncClass*>(this);
        ret.addRawTime(-raw);
        return ret;
    }

    template<typename T>
    TimeSpan operator-(const DateTimeBase<T>& raw) const {
        //Just sync time here
        return TimeSpan(getRawTime() - raw.getRawTime());
    }


    derivedSyncClass operator+=(const TimeSpan& ts2) {
        addRawTime(ts2.getRaw());
        return *static_cast<derivedSyncClass*>(this);
    }

    derivedSyncClass operator+=(const int64_t _raw) {
        addRawTime(_raw);
        return *static_cast<derivedSyncClass*>(this);
    }

    derivedSyncClass& operator++() {
        addRawTime(1);
        return *static_cast<derivedSyncClass*>(this);
    }

    derivedSyncClass operator++(int) {
        derivedSyncClass ret = *static_cast<derivedSyncClass*>(this);
        addRawTime(1);
        return ret;
    }

    derivedSyncClass operator-=(const TimeSpan& ts2) {
        addRawTime(-ts2.getRaw());
        return *static_cast<derivedSyncClass*>(this);
    }

    derivedSyncClass operator-=(const int64_t _raw) {
        addRawTime(-_raw);
        return *static_cast<derivedSyncClass*>(this);
    }

    derivedSyncClass& operator--() {
        addRawTime(-1);
        return *static_cast<derivedSyncClass*>(this);
    }

    derivedSyncClass operator--(int) {
        derivedSyncClass ret = *static_cast<derivedSyncClass*>(this);
        addRawTime(-1);
        return ret;
    }

    //Comparison operators

    template<class T>
    bool operator==(DateTimeBase<T>& dt2) const{
        return getRawTime() == dt2.getRawTime();
    }

    bool operator==(int64_t mil) const {
        return getRawTime() == mil;
    }

    template<class T>
    bool operator!=(DateTimeBase<T>& dt2) const {
        return getRawTime() != dt2.getRawTime();
    }

    bool operator!=(int64_t mil) const {
        return getRawTime() != mil;
    }

    template<class T>
    bool operator>=(DateTimeBase<T>& dt2) const {
        return getRawTime() >= dt2.getRawTime();
    }

    bool operator>=(int64_t mil) const {
        return getRawTime() >= mil;
    }

    template<class T>
    bool operator<=(DateTimeBase<T>& dt2) const {
        return getRawTime() <= dt2.getRawTime();
    }

    bool operator<=(int64_t mil) const {
        return getRawTime() <= mil;
    }

    template<class T>
    bool operator<(DateTimeBase<T>& dt2) const {
        return getRawTime() < dt2.getRawTime();
    }

    bool operator<(int64_t mil) const {
        return getRawTime() < mil;
    }

    template<class T>
    bool operator>(DateTimeBase<T>& dt2) const {
        return getRawTime() > dt2.getRawTime();
    }

    bool operator>(int64_t mil) const {
        return getRawTime() > mil;
    }

    /**
    * @brief Gets Unix time, which is count of seconds from 1970/01/01.
    */
    inline int64_t getUnix() const {
        return (getRawTime() - 62135596800000000LL) / SECOND;
    }

    /**
    * @brief Factory method, which creates DateTime instance from Unix time.
    * @note Microseconds and milliseconds fields will be set to 0.
    * @param unixTime Unix time, which is count of seconds from 1970/01/01.
    */
    static derivedSyncClass fromUnix(int64_t unixTime) {
        unixTime = (unixTime * SECOND) + 62135596800000000LL;
        return derivedSyncClass(unixTime);
    }

    /**
    * @brief Gets Date and Time represented as OLE Automation date.
    */
    inline double getOADate() const {
        return ((double)(getRawTime() - 59926435200000000LL)) / DAY;
        return 0.00;
    }

    /**
    * @brief Factory method, which creates DateTime instance from OADate.
    * @param OADate Date and time represented as an OLE Automation date.
    */
    static derivedSyncClass fromOADate(double OADate) {
        int64_t rawVal = (OADate * DAY);
        rawVal += 59926435200000000LL;
        return derivedSyncClass(rawVal);
    }

    /**
    * @brief Gets time_t time, which is usually (depends on std library) count of seconds from 1970/01/01.
    */
    inline time_t get_time_t() const {
        return (getRawTime() - 62135596800000000LL) / (SECOND / CLOCKS_PER_SEC);
    }

    /**
    * @brief Factory method, which creates DateTime instance from time_t.
    * @note Microseconds and milliseconds fields will be set to 0.
    * @param time Time, which is usually (depends on std library) count of seconds from 1970/01/01.
    */
    static derivedSyncClass from_time_t(time_t time) {
        time = (time * (SECOND / CLOCKS_PER_SEC)) + 62135596800000000LL;
        return derivedSyncClass(time);
    }

    /**
    * @brief Gets tm time structure.
    */
    tm get_tm() const {
        DateTime_DST_tuple tup;
        if constexpr (has_getDateTimeEnh<derivedSyncClass>::value) {
            tup = static_cast<const derivedSyncClass*>(this)->getDateTimeEnh(); //getting DateTime with isDST flag at same time
        }
        else {
            if constexpr (has_isDST<derivedSyncClass>::value) {
                tup.isDST = static_cast<const derivedSyncClass*>(this)->isDST();
            }
            else {
                tup.isDST = false;
            }
            tup.value = getRawTime();
        }
        date_time_s dt_s = dtlib::rawToDateTime(tup.value);
        tm t;
        t.tm_isdst = tup.isDST;
        t.tm_hour = dt_s.hours;
        t.tm_min = dt_s.minutes;
        t.tm_sec = dt_s.seconds;
        t.tm_year = 1900 - dt_s.year; //TODO maybe this is not true on ESP32/ESP8266 - test
        t.tm_mon = (int8_t)dt_s.month - 1;
        t.tm_mday = dt_s.day;
        t.tm_wday = (int8_t)dt_s.dayOfWeek - 1;
        bool isLeap = dtlib::isLeapYear(dt_s.year);
        t.tm_yday = dtlib::getDayOfYearFromMonth(dt_s.month, isLeap) + dt_s.day - 1;
        return t;
    }

    /**
    * @brief Factory method, which creates DateTime instance from tm structure.
    * @note Microseconds and milliseconds fields will be set to 0.
    * @warning tm_isdst flag is ignored.
    * @param time Time, which is usually (depends on std library) count of seconds from 1970/01/01.
    */
    static derivedSyncClass from_tm(tm time) {
        date_time_s dt_s;
        dt_s.hours = time.tm_hour;
        dt_s.minutes = time.tm_min;
        dt_s.seconds = time.tm_sec;

        dt_s.year = time.tm_year + 1900; //TODO maybe this is not true on ESP32/ESP8266 - test
        dt_s.month = time.tm_mon + 1;
        dt_s.day = time.tm_mday;

        return derivedSyncClass(time);
    }

protected:
    /**
    * @brief Gets raw time value after synchronization.
    * If synchronization is not supported, getRawTimeNoSync() is called.
    * @note It is recommended to call this member function instead of getRawTimeNoSync().
    */
    inline int64_t getRawTime() const {
        if constexpr (has_getRawTimeSync<derivedSyncClass>::value) {
            return static_cast<const derivedSyncClass*>(this)->getRawTimeSync();
        }
        else {
            return getRawTimeNoSync();
        }
    }

    /**
    * @brief Gets raw time value without synchronization.
    * @note It is recommended to call getRawTime() instead of getRawTimeNoSync().
    */
    inline int64_t getRawTimeNoSync() const {
        return static_cast<const derivedSyncClass*>(this)->getRawValueDer();
    }

    /**
    * @brief Sets raw time value. DST offset may be calculated here.
    * If DST offset is not supported, setRawTimeNoTD() is called.
    * @note It is recommended to call this member function instead of setRawTimeNoTD().
    * @note Do not forget to call syncBeforeSet() at the begin of member function, when setRawTime() is called. But
    * do not call it when adding offset to raw time.
    * @param value Raw value to set.
    */
    inline void setRawTime(int64_t value) {
        if constexpr (has_setRawTimeTD<derivedSyncClass>::value) {
            static_cast<derivedSyncClass*>(this)->setRawTimeTD(value);
        }
        else {
            setRawTimeNoTD(value);
        }
    }

    /**
    * @brief Sets raw time without DST offset calculation
    * @note Do not forget to call syncBeforeSet() at the begin of member function, when setRawTime() is called. But
    * do not call it when adding offset to raw time.
    * @note It is recommended to call setRawTime() instead of setRawTimeNoTD().
    * @param value Raw value to set.
    */
    inline void setRawTimeNoTD(int64_t value) {
        static_cast<derivedSyncClass*>(this)->setRawValueDer(value);
    }

    /**
    * @brief Adds value to raw value. DST offset may be calculated here.
    * If DST offset is not supported, addRawTimeNoTD() is called.
    * @param value Amount microseconds to add to value.
    * @note Do not need to call syncBeforeSet() before this member function.
    * @note It is recommended to call this member function instead of addRawTimeNoTD().
    */
    inline void addRawTime(int64_t value) {
        if constexpr (has_addRawTimeTD<derivedSyncClass>::value) {
            static_cast<derivedSyncClass*>(this)->addRawTimeTD(value);
        }
        else {
            addRawTimeNoTD(value);
        }
    }

    /**
    * @brief Adds value to raw value.
    * @param value Amount microseconds to add to value.
    * @note Do not need to call syncBeforeSet() before this member function.
    * @note It is recommended to call addRawTime() instead of addRawTimeNoTD().
    */
    inline void addRawTimeNoTD(int64_t value) {
        static_cast<derivedSyncClass*>(this)->addRawValueDer(value);
    }

    /**
    * @brief This function has to be called before setting some time or date field value, but should not be
    * called, when offset is added to DateTime (for example when using addHours() or + operator). Synchronization is done here.
    * @note It is recommended to call it before raw value calculation or at the begin of member function.
    */
    inline void syncBeforeSet() {
        if constexpr (has_preSetSync<derivedSyncClass>::value) {
            static_cast<derivedSyncClass*>(this)->preSetSync();
        }
    }
    /**
    * @brief This function has to be called before setting and calculating raw value. Synchronization is done here.
    * @return Returns raw value after synchronization.
    */
    inline int64_t getAndSyncBeforeSet() {
        if constexpr (has_getAndPreSetSync<derivedSyncClass>::value) {
            return static_cast<derivedSyncClass*>(this)->getAndPreSetSync();
        }
        else {
            return getRawTime();
        }
    }

};

/**
* @class DateTimeRawBase
* @brief Base for all DateTime classes, which uses raw value (microseconds elapsed from start of the epoch). This class
* contains private fields int64_t rawValue, which represents date and time. To access this raw value always use functions
* from DateTimeBase class. This class also contains calculations from raw value to date and time fields and opposite.
* @note This class work only with the Gregorian calendar.
* 
* ## How to derive from this class
* Your class can optionally contains those member functions:
* + <b>For time zone and DST adjustment:</b>
*   - <i>int16_t getTimeZoneOffsetMinutes() const</i> - which gets time zone offset in minutes,
*   - <i>int16_t getCurrentDSTOffsetMinutes() const</i> - which gets current DST offset in minutes,
*   - <i>void setTimeZone(TimeZone tz, bool recalculate)</i> - which sets time zone offset during parsing.
* Derived class declaration has to be like this:
* @code{.cpp}
* class YourClassName : public DateTimeRawBase<YourClassName>{
*   //Your code and functions goes here
* };
* @endcode
* Or like this, when you want to derive derived class:
* @code{.cpp}
* template<class derivedClass>
* class YourClassName : public DateTimeRawBase<derivedClass>{
*   //Your code and functions goes here
* };
* @endcode
* 
* @todo Add conversion to Julian day.
* 
* @see DateTimeBase
* @see DateTime
* @see DateTimeSysSync
* @see DateTimeTZBase
* @see DateTimeTZ
* @see DateTimeTZSysSync
* @see TimeSpan
*/
template<class derivedSyncClass>
class DateTimeRawBase : public DateTimeBase<derivedSyncClass>
{
public:

    /**
    * @brief Default constructor, which set DateTime to the 0001/01/01 00:00:00.000000 with no time zone and DST adjustment.
    */
    DateTimeRawBase():
        rawValue(0)
    {
        //DateTimeBase<derivedSyncClass>::syncBeforeSet();
        //DateTimeBase<derivedSyncClass>::setRawTime(0);
    }

    /**
    * @brief Constructor, which sets DateTime from raw value.
    * @param raw Raw value in microseconds from the begin of epoch (0001/1/1).
    */
    DateTimeRawBase(int64_t raw) : DateTimeBase<derivedSyncClass>(raw)
    {
        /*DateTimeBase<derivedSyncClass>::syncBeforeSet();
        DateTimeBase<derivedSyncClass>::setRawTime(raw);*/
    }

    /**
    * @brief Constructor, which sets DateTime from date fields.
    * @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including, excluding year 0.
    * @param month Month from 1 to 12.
    * @param day Day of month from 1 to 28/29/30/31.
    */
    DateTimeRawBase(int32_t year, uint8_t month, uint8_t day)
    {
        set(year, month, day);
    }

    /**
    * @brief Constructor, which sets DateTime from date and time fields.
    * @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including, excluding year 0.
    * @param month Month from 1 to 12.
    * @param day Day of month from 1 to 28/29/30/31.
    * @param hours Hours in 12-hour clock format or 24-hour clock format.
    * @param minutes Minutes from 0 to 59.
    * @param seconds Seconds from 0 to 59.
    * @param milliseconds Milliseconds from 0 to 999.
    * @param microseconds Microseconds from 0 to 999.
    */
    DateTimeRawBase(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes = 0, uint8_t seconds = 0, uint16_t milliseconds = 0, uint16_t microseconds = 0)
    {
        set(year, month, day, hours, minutes, seconds, milliseconds, microseconds);
    }

    /**
    * @brief Constructor, which creates DateTime from date_time_s.
    * @param dt_s date_time_s structure with date and time fields.
    */
    DateTimeRawBase(date_time_s dt_s)
    {
        set(dt_s);
    }

    /**
    * @brief Constructor, which creates DateTime from time_s.
    * @param t_s time_s structure with time fields.
    */
    DateTimeRawBase(time_s t_s)
    {
        set(t_s);
    }

    /**
    * @brief Constructor, which creates DateTime from date_s.
    * @param d_s date_s structure with date fields.
    */
    DateTimeRawBase(date_s d_s)
    {
        set(d_s);
    }

    /**
    * @brief Constructor, which creates DateTime from tm.
    * @param dt_s Date and time specified by tm structure.
    */
    DateTimeRawBase(tm dt_s)
    {
        set(dt_s);
    }

    /**
    * @brief Gets structure, which contains (almost) all calculated date and time fields. This is
    * the fastest method if you want to get all fields.
    * @return Calculates all date and time fields, which are returned as date_time_s structure.
    */
    date_time_s getDateTimeStruct() const {
        return dtlib::rawToDateTime(DateTimeBase<derivedSyncClass>::getRawTime());
    }

    /**
    * @brief Gets structure, which contains (almost) all calculated date fields. This is
    * the fastest method if you want to get all date fields.
    * @return Calculates all date fields, which are returned as date_s structure.
    */
    date_s getDateStruct() const {
        return dtlib::rawToDate(DateTimeBase<derivedSyncClass>::getRawTime());
    }

    /**
    * @brief Gets structure, which contains (almost) all calculated time fields. This is
    * the fastest method if you want to get all time fields.
    * @return Calculates all time fields, which are returned as time_s structure.
    */
    time_s getTimeStruct() const {
        return dtlib::rawToTime(DateTimeBase<derivedSyncClass>::getRawTime());
    }

    /**
    * @brief Gets the date component of this instance.
    */
    derivedSyncClass getDate() const {
        derivedSyncClass ret(*this);
        ret -= ret.getMicrosecondsOfDay();
        return ret;
    }

    /**
    * @brief Sets all date and time fields from date_time_s structure. This is one of the fastest
    * functions, if you want to set all fields.
    * @param dt_s date_time_s structure, which contains all date and time fields.
    */
    void set(date_time_s dt_s) {
        DateTimeBase<derivedSyncClass>::syncBeforeSet();
        DateTimeBase<derivedSyncClass>::setRawTime(dtlib::dateTimeToRaw(dt_s));
    }

    /**
    * @brief Sets DateTime from date and time fields.
    * This is one of the fastest functions, if you want to set all fields.
    * @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including, excluding year 0.
    * @param month Month from 1 to 12.
    * @param day Day of month from 1 to 28/29/30/31.
    * @param hours Hours in 12-hour clock format or 24-hour clock format.
    * @param minutes Minutes from 0 to 59.
    * @param seconds Seconds from 0 to 59.
    * @param milliseconds Milliseconds from 0 to 999.
    * @param microseconds Microseconds from 0 to 999.
    */
    void set(int32_t year, uint8_t month, uint8_t day, hour_t hours, uint8_t minutes = 0, uint8_t seconds = 0, uint16_t milliseconds = 0, uint16_t microseconds = 0) {
        DateTimeBase<derivedSyncClass>::syncBeforeSet();
        DateTimeBase<derivedSyncClass>::setRawTime(dtlib::dateTimeToRaw(date_time_s(year, month, day, hours, minutes, seconds, milliseconds, microseconds )));
    }

    /**
    * @brief Sets all date fields from date_s structure. This is one
    * of the fastest functions, if you want to set all date fields.
    * @note All time fields are set to zero.
    * @param d_s date_s structure, which contains all date fields.
    */
    void set(date_s d_s) {
        DateTimeBase<derivedSyncClass>::syncBeforeSet();
        DateTimeBase<derivedSyncClass>::setRawTime(dtlib::dateToRaw(d_s));
    }

    /**
    * @brief Constructor, which sets DateTime from date fields and all time fields are set to 0.
    * This is one of the fastest functions, if you want to set all fields.
    * @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including, excluding year 0.
    * @param month Month from 1 to 12.
    * @param day Day of month from 1 to 28/29/30/31.
    */
    void set(int32_t year, uint8_t month, uint8_t day) {
        DateTimeBase<derivedSyncClass>::syncBeforeSet();
        DateTimeBase<derivedSyncClass>::setRawTime(dtlib::dateToRaw(date_s(year, month, day)));
    }

    /**
    * @brief Sets all time fields from time_s structure. This is one
    * of the fastest functions, if you want to set all time fields.
    * @note Date is set to 0001/01/01 A.D.
    * @param t_s time_s structure, which contains all time fields.
    */
    void set(time_s t_s) {
        DateTimeBase<derivedSyncClass>::syncBeforeSet();
        DateTimeBase<derivedSyncClass>::setRawTime(dtlib::timeToRaw(t_s));
    }

    /**
    * @brief Sets DateTime from tm structure.
    * @param dt_s Date and time specified by tm structure.
    */
    void set(tm time)
    {
        DateTimeBase<derivedSyncClass>::syncBeforeSet();
        date_time_s dt_s;
        dt_s.hours = time.tm_hour;
        dt_s.minutes = time.tm_min;
        dt_s.seconds = time.tm_sec;

        dt_s.year = time.tm_year + 1900;
        dt_s.month = time.tm_mon + 1;
        dt_s.day = time.tm_mday;
        DateTimeBase<derivedSyncClass>::setRawTime(dtlib::dateTimeToRaw(dt_s));
    }


    /**
    * @brief Sets all date fields from date_s structure, but time fields are kept.
    * @note To set time fields to zero, use set(date_s) member function.
    * @param d_s date_s structure, which contains all date fields.
    */
    void setDate(date_s d_s) {
        int64_t newRaw = DateTimeBase<derivedSyncClass>::getAndSyncBeforeSet();
        dtlib::setDateToRaw(newRaw, d_s);
        DateTimeBase<derivedSyncClass>::setRawTime(newRaw);
    }

    /**
    * @brief Sets all date fields, but time fields are kept.
    * @note To set time fields to zero, use set(int32_t year, uint8_t month, uint8_t day) member function.
    * @param year Year field, which can be in range from MAX_YEAR to MIN_YEAR including, excluding year 0.
    * @param month Month from 1 to 12.
    * @param day Day of month from 1 to 28/29/30/31.
    */
    void setDate(int32_t year, uint8_t month, uint8_t day) {
        int64_t newRaw = DateTimeBase<derivedSyncClass>::getAndSyncBeforeSet();
        dtlib::setDateToRaw(newRaw, date_s{ year, month, day,0 });
        DateTimeBase<derivedSyncClass>::setRawTime(newRaw);
    }

    /*
    * @brief Sets all date fields from another DateTime, but time fields are kept.
    * @note To set time fields to zero, use set(int32_t year, uint8_t month, uint8_t day) member function.
    * @param dt Another DateTime instance.
    */
    template<class T>
    void setDate(DateTimeBase<T> dt) {
        //TODO tests
        int64_t oldRaw = DateTimeBase<derivedSyncClass>::getAndSyncBeforeSet();
        int64_t newRaw = dt.getRawTime();

        oldRaw = dtlib::getMicrosOfDayFromRaw(oldRaw);
        newRaw = newRaw - dtlib::getMicrosOfDayFromRaw(newRaw);
        
        DateTimeBase<derivedSyncClass>::setRawTime(newRaw + oldRaw);
    }

    /**
    * @brief Sets all time fields from time_s structure, but date fields are kept.
    * @note To set date fields to 0001/01/01, use set(time_s) member function.
    * @param t_s time_s structure, which contains all time fields.
    */
    void setTime(time_s t_s) {
        int64_t newRaw = DateTimeBase<derivedSyncClass>::getAndSyncBeforeSet();
        dtlib::setTimeToRaw(newRaw, t_s);
        DateTimeBase<derivedSyncClass>::setRawTime(newRaw);
    }
    /**
    * @brief Sets all time fields, but date fields are kept.
    * @note To set date fields to 0001/01/01, use set(hour_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds, uint16_t microseconds) member function.
    * @param hours Hours in 12-hour clock format or 24-hour clock format.
    * @param minutes Minutes from 0 to 59.
    * @param seconds Seconds from 0 to 59.
    * @param milliseconds Milliseconds from 0 to 999.
    * @param microseconds Microseconds from 0 to 999.
    */
    void setTime(hour_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds = 0, uint16_t microseconds = 0) {
        int64_t newRaw = DateTimeBase<derivedSyncClass>::getAndSyncBeforeSet();
        dtlib::setTimeToRaw(newRaw, time_s( hours, minutes, seconds, milliseconds, microseconds ));
        DateTimeBase<derivedSyncClass>::setRawTime(newRaw);
    }

    /*
    * @brief Sets all date time from another DateTime, but date fields are kept.
    * @note To set date fields to 0001/01/01, use set(hour_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds, uint16_t microseconds) member function.
    * @param dt Another DateTime instance.
    */
    template<class T>
    void setTime(DateTimeBase<T> dt) {
        //TODO tests
        int64_t oldRaw = DateTimeBase<derivedSyncClass>::getAndSyncBeforeSet();
        int64_t newRaw = dt.getRawTime();

        oldRaw = oldRaw - dtlib::getMicrosOfDayFromRaw(oldRaw);
        newRaw = dtlib::getMicrosOfDayFromRaw(newRaw);

        DateTimeBase<derivedSyncClass>::setRawTime(newRaw + oldRaw);
    }

    /**
    * @brief Sets new value to hours field.
    * @note This function should be used only if hours need to be set. If you want to set all time fields, use setTime() method
    * and if you want to set all date time fields, use set().
    * @param hours New value to set. Hours can be specified in 12-hours format (in range from 1 to 12) or in 24-hours hormat
    * (in range from 0 to 24). For more info see hour_t.
    * @note If you pass number as hours parameter, setHours24() is called.
    */
    void setHours(hour_t hours) {
        int64_t newRaw = DateTimeBase<derivedSyncClass>::getAndSyncBeforeSet();
        dtlib::setHoursToRaw(newRaw, hours);
        DateTimeBase<derivedSyncClass>::setRawTime(newRaw);
    }

    /**
    * @brief Sets new value to hours field.
    * @note This function should be used only if hours need to be set. If you want to set all time fields, use setTime() method
    * and if you want to set all date time fields, use set().
    * @param hours New value to set. Hours in 12-hours format in range from 1 to 12.
    * @param isAM_ True if AM.
    */
    void setHours12(uint8_t hours, bool isAM_) {
        int64_t newRaw = DateTimeBase<derivedSyncClass>::getAndSyncBeforeSet();
        dtlib::setHoursToRaw(newRaw, hour_t(hours, false, isAM));
        DateTimeBase<derivedSyncClass>::setRawTime(newRaw);
    }

    /**
    * @brief Sets new value to hours field.
    * @note This function should be used only if hours need to be set. If you want to set all time fields, use setTime() method
    * and if you want to set all date time fields, use set().
    * @param hours New value to set. Hours in 24-hours format in range from 0 to 23.
    * @param isAM_ True if AM.
    */
    void setHours24(uint8_t hours) {
        int64_t newRaw = DateTimeBase<derivedSyncClass>::getAndSyncBeforeSet();
        dtlib::setHoursToRaw(newRaw, hour_t(hours, true));
        DateTimeBase<derivedSyncClass>::setRawTime(newRaw);
    }

    /**
    * @brief Adds hours to DateTime.
    * @param hours Hours to add.
    */
    void addHours(int32_t hours) {
        DateTimeBase<derivedSyncClass>::addRawTime(((int64_t)hours) * HOUR);
    }

    /**
    * @brief Adds hours to DateTime.
    * @param hours Hours to add.
    */
    void addHours(int64_t hours) {
        DateTimeBase<derivedSyncClass>::addRawTime(hours * HOUR);
    }

    /**
    * @brief Gets hours field.
    * @note This function should be used only if hours need to be known. If you want to get all time fields, use getTimeStruct()
    * and if you want to get all date time fields, use getDateTimeStruct().
    * @return Returns hours field value in range from 0 to 23.
    */
    hour_t getHours() const {
        return dtlib::getHoursFromRaw(DateTimeBase<derivedSyncClass>::getRawTime());
    }

    /**
    * @brief Gets hours field in 12-hours format.
    * @note This function should be used only if hours need to be known. If you want to get all time fields, use getTimeStruct()
    * and if you want to get all date time fields, use getDateTimeStruct().
    * @return Returns hours field value in range from 1 to 12.
    */
    uint8_t getHours12() const {
        hour_t hours = dtlib::getHoursFromRaw(DateTimeBase<derivedSyncClass>::getRawTime());
        hours.convertTo12();
        return hours;
    }

    /**
    * @brief Gets hours field in 24-hours format.
    * @note This function should be used only if hours need to be known. If you want to get all time fields, use getTimeStruct()
    * and if you want to get all date time fields, use getDateTimeStruct().
    * @return Returns hours field value in range from 0 to 23.
    */
    uint8_t getHours24() const {
        return dtlib::getHoursFromRaw(DateTimeBase<derivedSyncClass>::getRawTime()); //No conversion needed
    }

    /**
    * @brief Returns true if current time period is AM.
    */
    bool isAM() const {
        return dtlib::getHoursFromRaw(DateTimeBase<derivedSyncClass>::getRawTime()).isAM();
    }


    /**
    * @brief Sets new value to minutes field.
    * @note This function should be used only if minutes need to be set. If you want to set all time fields, use setTime() method
    * and if you want to set all date time fields, use set().
    * @param minutes New value to set in range from 0 to 59.
    */
    void setMinutes(uint8_t minutes) {
        int64_t newRaw = DateTimeBase<derivedSyncClass>::getAndSyncBeforeSet();
        dtlib::setMinutesToRaw(newRaw, minutes);
        DateTimeBase<derivedSyncClass>::setRawTime(newRaw);
    }

    /**
    * @brief Adds minutes to DateTime.
    * @param minutes Minutes to add.
    */
    void addMinutes(int32_t minutes) {
        DateTimeBase<derivedSyncClass>::addRawTime(((int64_t)minutes) * MINUTE);
    }

    /**
    * @brief Adds minutes to DateTime.
    * @param minutes Minutes to add.
    */
    void addMinutes(int64_t minutes) {
        DateTimeBase<derivedSyncClass>::addRawTime(minutes * MINUTE);
    }

    /**
    * @brief Gets minutes field.
    * @note This function should be used only if minutes need to be known. If you want to get all time fields, use getTimeStruct()
    * and if you want to get all date time fields, use getDateTimeStruct().
    * @return Returns minutes field value in range from 0 to 59.
    */
    uint8_t getMinutes() const {
        return dtlib::getMinutesFromRaw(DateTimeBase<derivedSyncClass>::getRawTime());
    }

    /**
    * @brief Sets new value to seconds field.
    * @note This function should be used only if seconds need to be set. If you want to set all time fields, use setTime() method
    * and if you want to set all date time fields, use set().
    * @param seconds New value to set in range from 0 to 59.
    */
    void setSeconds(uint8_t seconds) {
        int64_t newRaw = DateTimeBase<derivedSyncClass>::getAndSyncBeforeSet();
        dtlib::setSecondsToRaw(newRaw, seconds);
        DateTimeBase<derivedSyncClass>::setRawTime(newRaw);
    }

    /**
    * @brief Adds seconds to DateTime.
    * @param seconds Seconds to add.
    */
    void addSeconds(int32_t seconds) {
        DateTimeBase<derivedSyncClass>::addRawTime(((int64_t)seconds) * SECOND);
    }

    /**
    * @brief Adds seconds to DateTime.
    * @param seconds Seconds to add.
    */
    void addSeconds(int64_t seconds) {
        DateTimeBase<derivedSyncClass>::addRawTime(seconds * SECOND);
    }

    /**
    * @brief Gets seconds field.
    * @note This function should be used only if seconds need to be known. If you want to get all time fields, use getTimeStruct()
    * and if you want to get all date time fields, use getDateTimeStruct().
    * @return Returns seconds field value in range from 0 to 59.
    */
    uint8_t getSeconds() const {
        return dtlib::getSecondsFromRaw(DateTimeBase<derivedSyncClass>::getRawTime());
    }

    /**
    * @brief Sets new value to milliseconds field.
    * @note This function should be used only if milliseconds need to be set. If you want to set all time fields, use setTime() method
    * and if you want to set all date time fields, use set().
    * @param milliseconds New value to set in range from 0 to 999.
    */
    void setMilliseconds(uint16_t milliseconds) {
        int64_t newRaw = DateTimeBase<derivedSyncClass>::getAndSyncBeforeSet();
        dtlib::setMillisToRaw(newRaw, milliseconds);
        DateTimeBase<derivedSyncClass>::setRawTime(newRaw);
    }

    /**
    * @brief Adds milliseconds to DateTime.
    * @param milliseconds Milliseconds to add.
    */
    void addMilliseconds(int32_t milliseconds) {
        DateTimeBase<derivedSyncClass>::addRawTime(((int64_t)milliseconds) * MILLISECOND);
    }

    /**
    * @brief Adds milliseconds to DateTime.
    * @param milliseconds Milliseconds to add.
    */
    void addMilliseconds(int64_t milliseconds) {
        DateTimeBase<derivedSyncClass>::addRawTime(milliseconds * MILLISECOND);
    }

    /**
    * @brief Gets milliseconds field.
    * @note This function should be used only if milliseconds need to be known. If you want to get all time fields, use getTimeStruct()
    * and if you want to get all date time fields, use getDateTimeStruct().
    * @return Returns milliseconds field value in range from 0 to 999.
    */
    uint16_t getMilliseconds() const {
        return dtlib::getMillisFromRaw(DateTimeBase<derivedSyncClass>::getRawTime());
    }

    /**
    * @brief Sets new value to microseconds field.
    * @note This function should be used only if microseconds need to be set. If you want to set all time fields, use setTime() method
    * and if you want to set all date time fields, use set().
    * @param microseconds New value to set in range from 0 to 999.
    */
    void setMicroseconds(uint16_t microseconds) {
        int64_t newRaw = DateTimeBase<derivedSyncClass>::getAndSyncBeforeSet();
        dtlib::setMicrosToRaw(newRaw, microseconds);
        DateTimeBase<derivedSyncClass>::setRawTime(newRaw);
    }

    /**
    * @brief Adds microseconds to DateTime.
    * @param microseconds Microseconds to add.
    */
    void addMicroseconds(int32_t microseconds) {
        DateTimeBase<derivedSyncClass>::addRawTime(((int64_t)microseconds) * MICROSECOND);
    }

    /**
    * @brief Adds microseconds to DateTime.
    * @param microseconds Microseconds to add.
    */
    void addMicroseconds(int64_t microseconds) {
        DateTimeBase<derivedSyncClass>::addRawTime(microseconds * MICROSECOND);
    }

    /**
    * @brief Gets microseconds field.
    * @note This function should be used only if microseconds need to be known. If you want to get all time fields, use getTimeStruct()
    * and if you want to get all date time fields, use getDateTimeStruct().
    * @return Returns microseconds field value in range from 0 to 999.
    */
    uint16_t getMicroseconds() const {
        return dtlib::getMicrosFromRaw(DateTimeBase<derivedSyncClass>::getRawTime());
    }

    /**
    * @brief Sets year.
    * @note This function should be used only if only this one parameter need to be set. If you want to set all date fields, use setDate()
    * which has better performance for setting all 3 date fields. If you want to set all date and time fields, use set(),
    * which has better performance for setting all date and time fields.
    * @param years New value of year to be set. Value can be in range from MIN_YEAR to MAX_YEAR except 0, because year 0 does not exists. Value
    * can be also negative, which means, that this is year B.C.
    */
    void setYear(int32_t year) {
        int64_t newRaw = DateTimeBase<derivedSyncClass>::getAndSyncBeforeSet();
        dtlib::setYearToRaw(newRaw, year);
        DateTimeBase<derivedSyncClass>::setRawTime(newRaw);
    }

    /**
    * @brief Adds amount of years to raw value. Only year field is affected, others stays unchanged.
    * @note This function has slightly worst performance than setYear().
    * @param years Value to add. This value can be also negative, which means, that it will be subtracted.
    */
    void addYears(int32_t years) {
        int64_t newRaw = DateTimeBase<derivedSyncClass>::getRawTime(); //No sync here
        dtlib::addYearsToRaw(newRaw, years);
        DateTimeBase<derivedSyncClass>::setRawTime(newRaw);
    }

    /**
    * @brief Gets year.
    * @note This function should be used only if only this one parameter need to be get. If you want to get all date fields, use getDateStruct()
    * which has better performance for setting all 3 date fields. If you want to set all date and time fields, use getDateTimeStruct(),
    * which has better performance for setting all date and time fields.
    * @return Returns year from raw value. Value can be in range from MIN_YEAR to MAX_YEAR except 0, because year 0 does not exists. Value
    * can be also negative, which means, that this is year BC.
    */
    int32_t getYear() const {
        return dtlib::getYearFromRaw(DateTimeBase<derivedSyncClass>::getRawTime());
    }

    /**
    * @brief Checks if current year is leap year.
    */
    bool isCurrentYearLeap() const {
        return dtlib::isLeapYear(dtlib::getYearFromRaw(DateTimeBase<derivedSyncClass>::getRawTime()));
    }

    /**
    * @brief Sets month.
    * @note This function should be used only if only this one parameter need to be set. If you want to set all date fields, use setDate()
    * which has better performance for setting all 3 date fields. If you want to set all date and time fields, use set(),
    * which has better performance for setting all date and time fields.
    * @param val New value of month to be set. Value can be in range from 1 to 12.
    */
    void setMonth(Month month) {
        int64_t newRaw = DateTimeBase<derivedSyncClass>::getAndSyncBeforeSet();
        dtlib::setMonthToRaw(newRaw, month);
        DateTimeBase<derivedSyncClass>::setRawTime(newRaw);
    }

    /**
    * @brief Adds count of months to raw value. This function calculates the resulting month and year, taking into account leap years and
    * the number of days in a month, then adjusts the day part of the result.
    * @note This function has worse performance than setMonth(), try to avoid it.
    * @param months Value to add. This value can be also negative, which means, that it will be subtracted.
    */
    void addMonths(int32_t months) {
        int64_t newRaw = DateTimeBase<derivedSyncClass>::getRawTime(); //No sync here
        dtlib::addMonthsToRaw(newRaw, months);
        DateTimeBase<derivedSyncClass>::setRawTime(newRaw);
    }

    /**
    * @brief Gets month.
    * @note This function should be used only if only this one parameter need to be get. If you want to get all date fields, use getDateStruct()
    * which has better performance for setting all 3 date fields. If you want to set all date and time fields, use getDateTimeStruct(),
    * which has better performance for setting all date and time fields.
    * @return Returns month from raw value. Value can be in range from 1 to 12.
    */
    Month getMonth() const {
        return dtlib::getMonthFromRaw(DateTimeBase<derivedSyncClass>::getRawTime());
    }

    /**
    * @brief Sets day of month or basically day.
    * @note This function should be used only if only this one parameter need to be set. If you want to set all date fields, use setDate()
    * which has better performance for setting all 3 date fields. If you want to set all date and time fields, use set(),
    * which has better performance for setting all date and time fields.
    * @param val New value of day to be set. Value can be in range from 1 to 28/29/30/31. To get maximum value that can be set,
    * use getMonthLength() function.
    */
    void setDay(uint8_t day) {
        int64_t newRaw = DateTimeBase<derivedSyncClass>::getAndSyncBeforeSet();
        dtlib::setMonthDayToRaw(newRaw, day);
        DateTimeBase<derivedSyncClass>::setRawTime(newRaw);
    }

    /**
    * @brief Adds days to DateTime.
    * @param days Days to add.
    */
    void addDays(int32_t days) {
        int64_t newRaw = DateTimeBase<derivedSyncClass>::getRawTime(); //No sync here
        dtlib::addDaysToRaw(newRaw, days);
        DateTimeBase<derivedSyncClass>::setRawTime(newRaw);
    }

    /**
    * @brief Gets day of month or basically day.
    * @note This function should be used only if only this one parameter need to be get. If you want to get all date fields, use getDateStruct()
    * which has better performance for setting all 3 date fields. If you want to set all date and time fields, use getDateTimeStruct(),
    * which has better performance for setting all date and time fields.
    * @return Returns day in range from 1 to 28/29/30/31.
    */
    uint8_t getDay() const {
        return dtlib::getMonthDayFromRaw(DateTimeBase<derivedSyncClass>::getRawTime());
    }

    /**
    * @brief Gets current day of week.
    * @return Returns day of week, which is in range from 1(Sunday) to 7(Saturday). See enum DayOfWeek.
    */
    DayOfWeek getDayOfWeek() const {
        return dtlib::getDayOfWeekFromRaw(DateTimeBase<derivedSyncClass>::getRawTime());
    }

    /**
    * @brief Gets week of the year or simply week number.
    * @param firstDayOfWeek First day of week. In some countries week starts with Sunday, some with Monday.
    * @returns Week of the year from 1 to 52.
    */
    uint8_t getWeekOfYear(DayOfWeek firstDayOfWeek = DayOfWeek::Sunday) const {
        return dtlib::getWeekOfYearFromRaw(DateTimeBase<derivedSyncClass>::getRawTime(), firstDayOfWeek);
    }

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
    uint8_t getWeekOfMonth(DayOfWeek firstDayOfWeek = DayOfWeek::Sunday) const {
        return dtlib::getWeekOfMonthFromRaw(DateTimeBase<derivedSyncClass>::getRawTime(), firstDayOfWeek);
    }

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
    * @returns Week day of the month from 1(first) to 5(fifth).
    */
    inline WeekOfMonth getWeekDayOfMonth() const {
        return (WeekOfMonth)dtlib::getWeekDayOfMonthFromRaw(DateTimeBase<derivedSyncClass>::getRawTime());
    }

    /**
    * @brief Sets count of days, that elapsed from first day in current year.
    * @param dayOfYear Count of days, that elapsed from first day in current year. This value can be in range from 0 to 364 for basic years
    * and in range from 0 to 365 for leap years.
    */
    void setDayOfYear(uint16_t dayOfYear) {
        int64_t newRaw = DateTimeBase<derivedSyncClass>::getAndSyncBeforeSet();
        dtlib::setDayOfYearToRaw(newRaw, dayOfYear);
        DateTimeBase<derivedSyncClass>::setRawTime(newRaw);
    }

    /**
    * @brief Gets count of days, that elapsed from first day in current year.
    * @return Returns count of days, that elapsed from first day in current year. This value can be in range from 0 to 364 for basic years
    * and in range from 0 to 365 for leap years.
    */
    uint16_t getDayOfYear() const {
        return dtlib::getDayOfYearFromRaw(DateTimeBase<derivedSyncClass>::getRawTime());
    }

    /**
    * @brief Sets days, that elapsed from start of the epoch.
    * @param val Count of days, that elapsed from from year 1 (calendar start). This value can be in range from MIN_DAYS to MAX_DAYS. It
    * can be also negative, which means, that current date is B.C.
    */
    void setDaysFromEpoch(int32_t days) {
        int64_t newRaw = DateTimeBase<derivedSyncClass>::getAndSyncBeforeSet();
        dtlib::setDaysToRaw(newRaw, days);
        DateTimeBase<derivedSyncClass>::setRawTime(newRaw);
    }

    /**
    * @brief Gets days, that elapsed from start of the epoch.
    * @return Returns count of days that elapsed from from start of the epoch. Value can be in range from MIN_DAYS to MAX_DAYS. It
    * can be also negative, which means, that current date is B.C.
    */
    int32_t getDaysFromEpoch() const {
        return dtlib::getDaysFromRaw(DateTimeBase<derivedSyncClass>::getRawTime());
    }
    
    /**
    * @brief Sets count of microseconds, that elapsed since start of the current day.
    * @param val Count of microseconds, that elapsed since start of current year. This value can be in range from 0 to (DAY - 1).
    */
    void setMicrosecondsOfDay(int64_t microsOfDay) {
        int64_t newRaw = DateTimeBase<derivedSyncClass>::getAndSyncBeforeSet();
        dtlib::setMicrosOfDayToRaw(newRaw, microsOfDay);
        DateTimeBase<derivedSyncClass>::setRawTime(newRaw);
    }

    /**
    * @brief Gets count of microseconds, that elapsed since start of the current day.
    * @return Returns count of microseconds, that elapsed since start of the current year. This value can be in range from 0 to (DAY - 1).
    */
    int64_t getMicrosecondsOfDay() const {
        return dtlib::getMicrosOfDayFromRaw(DateTimeBase<derivedSyncClass>::getRawTime());
    }
    
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
    * @param monthNames Array with custom month names. This array has to contain exactly 12 strings. If set to NULL, English names are used.
    * @param weekDayNames Array with custom days of week. This array has to contain exactly 7 strings and first name has to be Sunday, then Monday and so on. If set to NULL, English names are used.
    * @return Returns pointer to buffer, where null terminator was inserted.
    */
    char* toArray(char* buffer, size_t bufferSize, const char* format = "yyyy-MM-ddTHH:mm:ss.ffffffZZZ", const char* const* monthNames = NULL, const char* const* weekDayNames = NULL) const {
        int16_t tz = 0;
        int16_t dst = 0;
        if constexpr (has_getTimeZoneOffsetMinutes<derivedSyncClass>::value) {
            tz = static_cast<const derivedSyncClass*>(this)->getTimeZoneOffsetMinutes();
        }
        if constexpr (has_getCurrentDSTOffsetMinutes<derivedSyncClass>::value) {
            dst = static_cast<const derivedSyncClass*>(this)->getCurrentDSTOffsetMinutes();
        }
        return dtlib::dateTimeToArray(buffer, bufferSize, format, DateTimeBase<derivedSyncClass>::getRawTime(), tz, dst, monthNames, weekDayNames);
    }

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
    * | "ZZZZ"               | Time zone and DST offset in hours and minutes from UTC                                    | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -0600                                              |
    * | "string" or 'string' | Literal string delimiter.                                                                 | 2009-06-15T13:45:30 ("arr:" h:m t) -\> arr: 1:45 P                                            |
    * | "\"                  | The escape character.                                                                     | 2009-06-15T13:45:30 (h \h) -\> 1 h                                                            |
    * | Any other character  | The character is copied to the result string unchanged.                                   | 2009-06-15T01:45:30 (arr hh:mm t) -\> arr 01:45 A                                             |
    * Format specifiers "f", "F", "y" has not limited maximum count, but other specifiers specified in table above has limited maximum counts.
    *
    * @param monthNames Array with custom month names. This array has to contain exactly 12 strings. If set to NULL, English names are used.
    * @param weekDayNames Array with custom days of week. This array has to contain exactly 7 strings and first name has to be Sunday, then Monday and so on. If set to NULL, English names are used.
    * @return Returns converted string.
    */
#ifdef ARDUINO
    String toString(const char* format = "yyyy-MM-ddTHH:mm:ss.ffffffZZZ", const char* const* monthNames = NULL, const char* const* weekDayNames = NULL) const {
#else
    std::string toString(const char* format = "yyyy-MM-ddTHH:mm:ss.ffffffZZZ", const char* const* monthNames = NULL, const char* const* weekDayNames = NULL) const {
#endif // ARDUINO
        int16_t tz = 0;
        int16_t dst = 0;
        if constexpr (has_getTimeZoneOffsetMinutes<derivedSyncClass>::value) {
            tz = static_cast<const derivedSyncClass*>(this)->getTimeZoneOffsetMinutes();
        }
        if constexpr (has_getCurrentDSTOffsetMinutes<derivedSyncClass>::value) {
            dst = static_cast<const derivedSyncClass*>(this)->getCurrentDSTOffsetMinutes();
        }
        return dtlib::dateTimeToString(format, DateTimeBase<derivedSyncClass>::getRawTime(), tz, dst, monthNames, weekDayNames);
    }

    /**
    * @brief Converts DateTime to ISO8601 string.
    * @param buffer Buffer, where string will be written.
    * @param bufferSize Size of buffer including null terminator.
    * @param includeTimeZone True to include time zone in string.
    * @param includeMillis True to include milliseconds in string.
    * @return Returns pointer to buffer, where null terminator was inserted.
    */
    char* toISO8601Array(char* buffer, size_t bufferSize, bool includeTimeZone = true, bool includeMillis = true) const {
        if (includeTimeZone) {
            if (includeMillis) {
                return toArray(buffer, bufferSize, "yyyy-MM-ddTHH:mm:ss.fffZZZ");
            }
            else {
                return toArray(buffer, bufferSize, "yyyy-MM-ddTHH:mm:ssZZZ");
            }
        }
        else{
            if (includeMillis) {
                return toArray(buffer, bufferSize, "yyyy-MM-ddTHH:mm:ss.fff");
            }
            else {
                return toArray(buffer, bufferSize, "yyyy-MM-ddTHH:mm:ss");
            }
        }
    }

    /**
    * @brief Converts DateTime to ISO8601 string.
    * @param buffer Buffer, where string will be written.
    * @param bufferSize Size of buffer including null terminator.
    * @param includeTimeZone True to include time zone in string.
    * @param includeMillis True to include milliseconds in string.
    * @return Returns pointer to buffer, where null terminator was inserted.
    */
#ifdef ARDUINO
    String toISO8601String(bool includeTimeZone = true, bool includeMillis = true) const {
#else
    std::string toISO8601String(bool includeTimeZone = true, bool includeMillis = true) const {
#endif // ARDUINO
        if (includeTimeZone) {
            if (includeMillis) {
                return toString("yyyy-MM-ddTHH:mm:ss.fffZZZ");
            }
            else {
                return toString("yyyy-MM-ddTHH:mm:ssZZZ");
            }
        }
        else {
            if (includeMillis) {
                return toString("yyyy-MM-ddTHH:mm:ss.fff");
            }
            else {
                return toString("yyyy-MM-ddTHH:mm:ss");
            }
        }
    }

#if DT_UNDER_OS > 0
    friend auto operator<<(std::ostream& os, derivedSyncClass const& m) -> std::ostream& {
        return os << m.toString();
    }
#endif //DT_UNDER_OS > 0

#ifdef ARDUINO

    /**
    * @brief Prints DateTime in ISO format to specified stream.
    * @param[in] p Stream, where to print.
    * @return Returns count of printed characters.
    */
    size_t printTo(Print& p) const {
        char buffer[35];
        char* newBuff = toArray(buffer, sizeof(buffer) / sizeof(char), "yyyy-MM-ddTHH:mm:ss.ffffffZZZ");
        return (size_t)(newBuff - buffer);
    }

    operator dtlib::DTStaticPrintable<derivedSyncClass>() const {
        return dtlib::DTStaticPrintable<derivedSyncClass>(*this);
    }

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
    * | "ZZZ"                | Time zone and DST offset in hours and minutes from UTC. If 'Z' or 'z' is found, UTC time zone is set.                                                          | 06:00 -\> fail<br>0730 -\> +07:30<br>+0630 -\> +06:30<br>-0630 -\> -06:30<br>Z or z -\> +00:00              |
    * | "string" or 'string' | Literal string delimiter. Text is not parsed, but can be matched, see matchText parameter.                                                                     | "Day 28" : "Day "d -\> pass<br>if matchText == false: "abc 28" : "Day "d -\> pass<br>if matchText == false: "a 28" : "Day "d -\> fail<br>if matchText == true: "abc 28" : "Day "d -\> fail |
    * | "\"                  | The escape character.                                                                                                                                          |                                                                                                             |
    * | Any other character  | Text character is not parsed, but can be matched, see matchText parameter.                                                                                     | "12:30" : HH:mm -\> pass<br>if matchText == false: "12.30" : HH:mm -\> pass<br>if matchText == false: "12::30" : HH:mm -\> fail<br>if matchText == true: "12.30" : HH:mm -\> fail |
    * Format specifiers "f", "F" has not limited maximum count, but other specifiers specified in table above has limited maximum counts.
    *
    * @param matchText True to exactly match text characters. If set to false, just count and position of text characters is checked.
    * @param monthNames Array with custom month names. This array has to contain exactly 12 strings. If set to NULL, English names are used.
    * @return Returns value greather than 0, when parsing was successful. This value is count of parsed characters from buffer.
    *         Returns negative or 0, when parsing failed. This value is zero based position of character from buffer, where parsing failed.
    */
    int parse(const char* buffer, int bufferSize, const char* format, bool matchText = false, const char* const* monthNames = NULL) {
        DateTimeBase<derivedSyncClass>::syncBeforeSet();
        
        date_time_s parsedVal;
        int16_t tzOffset;
        int16_t DSTOffset;
        int16_t tz_DST_Offset;
        int ret = dtlib::parseDateTime(buffer, bufferSize, format, parsedVal, tzOffset, DSTOffset, tz_DST_Offset, matchText, monthNames);

        DateTimeBase<derivedSyncClass>::setRawTime(dtlib::dateTimeToRaw(parsedVal));

        if constexpr (has_setTimeZone<derivedSyncClass>::value) {
            int16_t offset = tz_DST_Offset;
            if (offset == 0) {
                offset = tzOffset + DSTOffset;
            }
            static_cast<derivedSyncClass*>(this)->setTimeZone(TimeZone::fromTotalMinutesOffset(offset), false);
        }

        return ret;
    }

    /**
    * @brief Parses date and time from string.
    * @param str String, where DateTime is written in text form.
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
    * | "ZZZ"                | Time zone and DST offset in hours and minutes from UTC. If 'Z' or 'z' is found, UTC time zone is set.                                                          | 06:00 -\> fail<br>0730 -\> +07:30<br>+0630 -\> +06:30<br>-0630 -\> -06:30<br>Z or z -\> +00:00              |
    * | "string" or 'string' | Literal string delimiter. Text is not parsed, but can be matched, see matchText parameter.                                                                     | "Day 28" : "Day "d -\> pass<br>if matchText == false: "abc 28" : "Day "d -\> pass<br>if matchText == false: "a 28" : "Day "d -\> fail<br>if matchText == true: "abc 28" : "Day "d -\> fail |
    * | "\"                  | The escape character.                                                                                                                                          |                                                                                                             |
    * | Any other character  | Text character is not parsed, but can be matched, see matchText parameter.                                                                                     | "12:30" : HH:mm -\> pass<br>if matchText == false: "12.30" : HH:mm -\> pass<br>if matchText == false: "12::30" : HH:mm -\> fail<br>if matchText == true: "12.30" : HH:mm -\> fail |
    * Format specifiers "f", "F" has not limited maximum count, but other specifiers specified in table above has limited maximum counts.
    *
    * @param matchText True to exactly match text characters. If set to false, just count and position of text characters is checked.
    * @param monthNames Array with custom month names. This array has to contain exactly 12 strings. If set to NULL, English names are used.
    * @return Returns value greather than 0, when parsing was successful. This value is count of parsed characters from buffer.
    *         Returns negative or 0, when parsing failed. This value is zero based position of character from buffer, where parsing failed.
    */
#ifdef ARDUINO
    int parse(String str, const char* format, bool matchText = false, const char* const* monthNames = NULL) {
#else
    int parse(std::string str, const char* format, bool matchText = false, const char* const* monthNames = NULL) {
#endif // ARDUINO
        return parse(str.c_str(), str.length() + 1, format, matchText, monthNames);
    }

    /**
   * @brief Parses date and time from ISO8601 string.
   * @param buffer Buffer, where DateTime is written in text form.
   * @param bufferSize Size of buffer including null terminator.
   * @return Returns value greather than 0, when parsing was successful. This value is count of parsed characters from buffer.
   *         Returns negative or 0, when parsing failed. This value is zero based position of character from buffer, where parsing failed.
   */
    /*int parseISO8601(const char* buffer, size_t bufferSize) {
        TODO accept all ISO8601 formats
        DateTimeBase<derivedSyncClass>::syncBeforeSet();

        date_time_s parsedVal;
        int16_t tzOffset;
        int16_t DSTOffset;
        int16_t tz_DST_Offset;
        int ret = parseDateTime(buffer, bufferSize, "y-M-dTHH:mm:ss.FFFFFFzzzz", parsedVal, tzOffset, DSTOffset, tz_DST_Offset, true);

        DateTimeBase<derivedSyncClass>::setRawTime(dateTimeToRaw(parsedVal));

        if constexpr (has_setTimeZone<derivedSyncClass>::value) {
            static_cast<derivedSyncClass*>(this)->setTimeZone(TimeZone::fromTotalMinutesOffset(tzOffset), false);
        }

        return ret;
    }*/

    /**
    * @brief Maximum possible value, that can be used with DateTime.
    */
    const static derivedSyncClass MaxValue;

    /**
    * @brief Minimum possible value, that can be used with DateTime.
    */
    const static derivedSyncClass MinValue;

    /**
    * @brief DateTime value of first epoch day.
    */
    const static derivedSyncClass Zero;

    /**
    * @brief Unix base represented as DateTime.
    */
    const static derivedSyncClass UnixBase;

    /**
    * @brief OLE Automation date base represented as DateTime.
    */
    const static derivedSyncClass OABase;

protected:

    template <class T> friend class DateTimeBase;
    template <class T, class Y> friend struct has_getRawValueDer;
    template <class T, class Y> friend struct has_setRawValueDer;
    template <class T, class Y> friend struct has_addRawValueDer;

    /**
    * @brief Gets raw time value without synchronization.
    * @return Returns Raw time value in microseconds.
    */
    inline int64_t getRawValueDer() const {
        return rawValue;
    }

    /**
    * @brief Sets raw time without synchronization and without DST offset calculation
    * @param value Raw value to set in microseconds.
    */
    inline void setRawValueDer(int64_t value) {
        rawValue = value;
    }

    /**
    * @brief Adds value to raw time without synchronization and without DST offset calculation
    * @param value Raw value to set in microseconds.
    */
    inline void addRawValueDer(int64_t value) {
        rawValue += value;
    }

private:
    int64_t rawValue;
};

template <class T> const T DateTimeRawBase<T>::MaxValue(MAX_YEAR, 12, 31, 23, 59, 59, 999, 999);
template <class T> const T DateTimeRawBase<T>::Zero(0LL);
template <class T> const T DateTimeRawBase<T>::MinValue(MIN_YEAR, 0, 0, 0, 0, 0, 0, 0);
template <class T> const T DateTimeRawBase<T>::UnixBase(62135596800000000LL);
template <class T> const T DateTimeRawBase<T>::OABase(1899, 12, 30);



/**
* @class DateTime
* @brief The DateTime represents dates (year, month, day) and times (hours, minutes, seconds, milliseconds, microseconds) with values ranging
* from 00:00:00.000000 (midnight), January 1, 292277 Before Christ through 23:59:59.999999, December 31, 292277 A.D. (C.E.) in the
* Gregorian calendar. Time values are measured in microseconds. A particular date is the number of microseconds since 00:00:00.000000
* (midnight), January 1, 0001 A.D. (C.E.) in the Gregorian calendar. The number excludes ticks that would be added by leap seconds.
*/
class DateTime : public DateTimeRawBase<DateTime> {

public:
    using DateTimeRawBase<DateTime>::DateTimeRawBase; //Using constructors

    /**
    * @brief Default constructor, which set DateTime to the 0001/01/01 00:00:00.000000 with no time zone and DST adjustment.
    */
    DateTime() : DateTimeRawBase<DateTime>()
    { }

    /**
    * @brief Construct DateTime from any other DateTimeBase or any class, which has int64_t getRaw() member function.
    * This constructor is also used as explicit conversion.
    * @param dt Any other DateTimeBase (for example DateTimeTZ, DateTimeSysSync, ...)
    */
    template<class DTclass, typename = typename dtlib::enable_if<has_getRaw<DTclass>::value>::type>
    explicit DateTime(const DTclass& dt) {
        DateTimeBase<DateTime>::setRawTime(dt.getRaw());
    }
};

#endif // !_DATE_TIME_BASE_H
