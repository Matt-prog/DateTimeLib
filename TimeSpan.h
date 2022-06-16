/**
 * @file TimeSpan.h
 * @brief This file contains class TimeSpan, which represents elapsed time or time difference.
 *
 * @see TimeSpan
 *
 * # Credits
 * @author Matej Fitoš
 * @date Jun 4, 2022
 */

#ifndef _TIME_SPAN_H
#define _TIME_SPAN_H

#include "DateTimeHelpers.h"

struct time_span_s {
    time_span_s() {}

    time_span_s(int32_t days_, int8_t hours_, int8_t minutes_, int8_t seconds_, int16_t milliseconds_, int16_t microseconds_) :
        days(days_),
        hours(hours_),
        minutes(minutes_),
        seconds(seconds_),
        milliseconds(milliseconds_),
        microseconds(microseconds_)
    {}

    int32_t days = 0;
    int8_t hours = 0;
    int8_t minutes = 0;
    int8_t seconds = 0;
    int16_t milliseconds = 0;
    int16_t microseconds = 0;
};

/**
* @class TimeSpan
* @brief A TimeSpan object represents a time interval (duration of time or elapsed time)
* that is measured as a positive or negative number of days, hours, minutes, seconds, milliseconds and microseconds.
* TimeSpan represents time interval, but iy you want to represent date and time, use DateTime class.
* TimeSpan is internally represented as int64_t, so it's size is always 8 bytes.
* 
* ## Negative TimeSpan
* TimeSpan works slightly different with negative numbers than DateTime. In DateTime, only year field can be negative,
* but other fields are still positive. For example:
* @verbatim
* 12/31/0001 BC 12:31:05 => year = -1, but month and day are still positive
* (d = 31, m = 12, y = -1)
* + 1 day
* -------------
* 01/01/0001    12:31:05 => time fields are unchanged
* (d = 1, m = 1, y = 1)
* @endverbatim
* In TimeSpan, value is mirrored about TimeSpan equal to zero like this:
* @verbatim
* -1 day 12:31:05
* (d = -1, h = -12, m = -31, s = -5)
* + 1 day
* -------------
* -0 day 12:31:05 => still negative, same time fields
* (d = 0, h = -12, m = -31, s = -5)
* + 1 day
* -------------
* 0 day 11:28:55  => value is positive and time fields were changed, because value
*                    is now mirrored around TimeSpan equals to 0.
* (d = 0, h = 11, m = 28, s = 55)
* @endverbatim
* To understand it better, we can plot it like this:
* @verbatim
* DateTime:
* year    |  <-----------|----------->  - can be negative
* month   |  ----------->|----------->  - still positive
* day     |  ----------->|----------->  - still positive
* hours   |  ----------->|----------->  - still positive
* minutes |  ----------->|----------->  - still positive
* ...        -           0           +
* 
* TimeSpan:
* days    |  <-----------|----------->  - can be negative
* hours   |  <-----------|----------->  - can be negative
* minutes |  <-----------|----------->  - can be negative
* ...        -           0           +
* @endverbatim
*/
class TimeSpan
{
public:
    //constructors:
    TimeSpan() : raw_time(0){ }

    /**
    * @brief Constructor, which constructs instance from raw value.
    * @param raw Raw value in microseconds.
    */
    TimeSpan(int64_t raw) : raw_time(raw){ }

    /**
    * @brief Constructor, that sets all fields of the current TimeSpan. This is most efficient method how to set all fields.
    * Fields can have positive or negative values. If you want to create negative TimeSpan, negative or 0 values has to be set.
    * Values has no limit, so for example if you set 61 seconds, it will be written as 1 minute and 2 seconds.
    * @param days Count of elapsed days.
    * @param hours Count of elapsed hours.
    * @param minutes Count of elapsed minutes.
    * @param seconds Count of elapsed seconds.
    * @param milliseconds Count of elapsed milliseconds.
    * @param microseconds Count of elapsed microseconds.
    */
    TimeSpan(int32_t days, int32_t hours = 0, int32_t minutes = 0, int32_t seconds = 0, int32_t milliseconds = 0, int32_t microseconds = 0);

    /**
    * @brief Sets all fields of the current TimeSpan. This is most efficient method how to set all fields.
    * Fields can have positive or negative values. If you want to create negative TimeSpan, negative or 0 values has to be set.
    * Values has no limit, so for example if you set 61 seconds, it will be written as 1 minute and 2 seconds.
    * @param days Count of elapsed days.
    * @param hours Count of elapsed hours.
    * @param minutes Count of elapsed minutes.
    * @param seconds Count of elapsed seconds.
    * @param milliseconds Count of elapsed milliseconds.
    * @param microseconds Count of elapsed microseconds.
    */
    void set(int32_t days, int32_t hours = 0, int32_t minutes = 0, int32_t seconds = 0, int32_t milliseconds = 0, int32_t microseconds = 0);

    /**
    * @brief Sets all time span fields from time_span_s structure.
    * @param ts TimeSpan structure.
    */
    void set(time_span_s ts);

    time_span_s getTimeSpanStruct() const;

    void setDays(int32_t days);

    inline void addDays(int32_t days) {
        raw_time += (int64_t)days * DAY;
    }

    int32_t getDays() const;


    void setHours(int8_t hours);

    inline void addHours(int32_t hours) {
        raw_time += (int64_t)hours * HOUR;
    }

    inline void addHours(int64_t hours) {
        raw_time += (int64_t)hours * HOUR;
    }

    int8_t getHours() const;

    void setMinutes(int8_t minutes);

    inline void addMinutes(int32_t minutes) {
        raw_time += (int64_t)minutes * MINUTE;
    }

    inline void addMinutes(int64_t minutes) {
        raw_time += (int64_t)minutes * MINUTE;
    }

    int8_t getMinutes() const;


    void setSeconds(int8_t seconds);

    inline void addSeconds(int32_t seconds) {
        raw_time += (int64_t)seconds * SECOND;
    }

    inline void addSeconds(int64_t seconds) {
        raw_time += (int64_t)seconds * SECOND;
    }

    int8_t getSeconds() const;


    void setMilliseconds(int16_t milliseconds);

    inline void addMilliseconds(int32_t milliseconds) {
        raw_time += (int64_t)milliseconds * MILLISECOND;
    }

    inline void addMilliseconds(int64_t milliseconds) {
        raw_time += (int64_t)milliseconds * MILLISECOND;
    }

    int16_t getMilliseconds() const;


    void setMicroseconds(int16_t microseconds);

    inline void addMicroseconds(int32_t microseconds) {
        raw_time += microseconds;
    }

    inline void addMicroseconds(int64_t microseconds) {
        raw_time += microseconds;
    }

    int16_t getMicroseconds() const;


    TimeSpan getDuration() const;

    inline bool isNegative() const {
        return raw_time < 0;
    }

    inline void setRaw(int64_t raw) {
        raw_time = raw;
    }

    inline int64_t getRaw() const {
        return raw_time;
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
    * | Format specifier     | Description                                                                                            | Example                                                                                    |
    * |----------------------|--------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------|
    * | "d"                  | The absolute number of whole days in the time interval.                                                | 1012d 12:30:21 -> 1012<br>-10d 12:30:21 -> 10                                              |
    * | "dd" - "dddddddd"    | The absolute number of whole days in the time interval, padded with leading zeros as needed.           | for ddddd: -1012d 12:30:21 -> 01012<br>0d 12:30:21 -> 0                                    |
    * | "f"                  | The tenths of a second in a TimeSpan value. This value is always positive.                             | 1012d 12:30:21.6170000 -> 6<br>1012d 12:30:21.04 -> 0                                      |
    * | "ff"                 | The hundredths of a second in a TimeSpan value. This value is always positive.                         | 1012d 12:30:21.6170000 -> 62<br>1012d 12:30:21.0040000 -> 00                               |
    * | "fff"                | The milliseconds in a TimeSpan value. This value is always positive.                                   | 1012d 12:30:21.6170000 -> 617<br>1012d 12:30:21.0004000 -> 000                             |
    * | "ffff"               | The ten thousandths of a second in a TimeSpan value. This value is always positive.                    | 1012d 12:30:21.6175000 -> 6175<br>1012d 12:30:21.0000400 -> 0000                           |
    * | "fffff"              | The hundred thousandths of a second in a TimeSpan value. This value is always positive.                | 1012d 12:30:21.6175400 -> 61754<br>1012d 12:30:21.0000040 -> 00000                         |
    * | "ffffff"             | The millionths of a second in a TimeSpan value. This value is always positive.                         | 1012d 12:30:21.6175420 -> 617542<br>1012d 12:30:21.0000004 -> 000000                       |
    * | "F"                  | If non-zero, the tenths of a second in a TimeSpan value. This value is always positive.                | 1012d 12:30:21.6170000 -> 6<br>1012d 12:30:21.0400000 -> 0                                 |
    * | "FF"                 | If non-zero, the hundredths of a second in a TimeSpan value. This value is always positive.            | 1012d 12:30:21.6170000 -> 62<br>1012d 12:30:21.0040000 -> 0                                |
    * | "FFF"                | If non-zero, the milliseconds in a TimeSpan value. This value is always positive.                      | 1012d 12:30:21.6170000 -> 617<br>1012d 12:30:21.0004000 -> 0                               |
    * | "FFFF"               | If non-zero, the ten thousandths of a second in a TimeSpan value. This value is always positive.       | 1012d 12:30:21.6175000 -> 6175<br>1012d 12:30:21.0000400 -> 0                              |
    * | "FFFFF"              | If non-zero, the hundred thousandths of a second in a TimeSpan value. This value is always positive.   | 1012d 12:30:21.6175400 -> 61754<br>1012d 12:30:21.0000040 -> 0                             |
    * | "FFFFFF"             | If non-zero, the millionths of a second in a TimeSpan value. This value is always positive.            | 1012d 12:30:21.6175420 -> 617542<br>1012d 12:30:21.0000004 -> 0                            |
    * | "h"                  | The hour, using a 24-hour clock from 0 to 23. This value is always positive.                           | 1012d 13:30:21 -> 13<br>1012d 2:30:21 -> 2                                                 |
    * | "hh"                 | The hour, using a 24-hour clock from 00 to 23. This value is always positive.                          | 1012d 13:30:21 -> 13<br>1012d 2:30:21 -> 02                                                |
    * | "i"                  | The milliseconds, from 0 through 999. This value is always positive.                                   | 1012d 12:30:21.0175420 -> 18<br>2009-06-15T13:45:30.0002420 -> 0                           |
    * | "iii"                | The milliseconds, from 000 through 999. This value is always positive.                                 | 1012d 12:30:21.6175420 -> 618<br>2009-06-15T13:45:30.0002420 -> 000                        |
    * | "n"                  | Sign character. Both + and - are shown.                                                                | 1012d 12:30:21 -> +<br>-1012d 12:30:21 -> -                                                |
    * | "N"                  | Sign character. Only minus sign is shown.                                                              | 1012d 12:30:21 -> (nothing)<br>-1012d 12:30:21 -> -                                        |
    * | "m"                  | The minute, from 0 through 59. This value is always positive.                                          | 1012d 1:05:01.0170420 -> 5<br>1012d 1:29:01 -> 29                                          |
    * | "mm"                 | The minute, from 00 through 59. This value is always positive.                                         | 1012d 1:05:01.0170420 -> 05<br>1012d 1:29:01 -> 29                                         |
    * | "s"                  | The second, from 0 through 59. This value is always positive.                                          | 1012d 1:05:01.0170420 -> 1                                                                 |
    * | "ss"                 | The second, from 00 through 59. This value is always positive.                                         | 1012d 1:05:01.0170420 -> 01                                                                |
    * | "u"                  | The microseconds, from 0 through 999. This value is always positive.                                   | 1012d 1:45:21.0170420 -> 42<br>1012d 1:45:21.6780004 -> 0                                  |
    * | "uuu"                | The microseconds, from 000 through 999. This value is always positive.                                 | 1012d 1:45:21.0170420 -> 042<br>1012d 1:45:21.6780004 -> 000                               |
    * | "string" or 'string' | Literal string delimiter.                                                                              | 1012d 1:45:21 ("arr:" h:m t) -> arr: 1:45                                                  |
    * | "\"                  | The escape character.                                                                                  | 1012d 12:30:21 (h \h) -> 12 h                                                              |
    * | Any other character  | The character is copied to the result string unchanged.                                                | 1012d 12:30:21 (arr hh:mm) -> arr 12:30                                                    |
    * Format specifiers "f", "F", "y" has not limited maximum count, but other specifiers specified in table above has limited maximum counts.
    *
    * @param monthNames Array with custom month names. This array has to contain exactly 12 strings. If set to NULL, English names are used.
    * @param weekDayNames Array with custom days of week. This array has to contain exactly 7 strings and first name has to be Sunday, then Monday and so on. If set to NULL, English names are used.
    * @return Returns pointer to buffer, where null terminator was inserted.
    */
    char* toArray(char* buffer, size_t bufferSize, const char* format) const;

    /**
    * @brief Converts DateTime to string.
    * @param format Custom date and time format.
    * ## Format specifiers
    * The following table describes the custom date and time format specifiers and
    * displays a result string produced by each format specifier.
    *
    * | Format specifier     | Description                                                                                            | Example                                                                                    |
    * |----------------------|--------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------|
    * | "d"                  | The absolute number of whole days in the time interval.                                                | 1012d 12:30:21 -> 1012<br>-10d 12:30:21 -> 10                                              |
    * | "dd" - "dddddddd"    | The absolute number of whole days in the time interval, padded with leading zeros as needed.           | for ddddd: -1012d 12:30:21 -> 01012<br>0d 12:30:21 -> 0                                    |
    * | "f"                  | The tenths of a second in a TimeSpan value. This value is always positive.                             | 1012d 12:30:21.6170000 -> 6<br>1012d 12:30:21.04 -> 0                                      |
    * | "ff"                 | The hundredths of a second in a TimeSpan value. This value is always positive.                         | 1012d 12:30:21.6170000 -> 62<br>1012d 12:30:21.0040000 -> 00                               |
    * | "fff"                | The milliseconds in a TimeSpan value. This value is always positive.                                   | 1012d 12:30:21.6170000 -> 617<br>1012d 12:30:21.0004000 -> 000                             |
    * | "ffff"               | The ten thousandths of a second in a TimeSpan value. This value is always positive.                    | 1012d 12:30:21.6175000 -> 6175<br>1012d 12:30:21.0000400 -> 0000                           |
    * | "fffff"              | The hundred thousandths of a second in a TimeSpan value. This value is always positive.                | 1012d 12:30:21.6175400 -> 61754<br>1012d 12:30:21.0000040 -> 00000                         |
    * | "ffffff"             | The millionths of a second in a TimeSpan value. This value is always positive.                         | 1012d 12:30:21.6175420 -> 617542<br>1012d 12:30:21.0000004 -> 000000                       |
    * | "F"                  | If non-zero, the tenths of a second in a TimeSpan value. This value is always positive.                | 1012d 12:30:21.6170000 -> 6<br>1012d 12:30:21.0400000 -> 0                                 |
    * | "FF"                 | If non-zero, the hundredths of a second in a TimeSpan value. This value is always positive.            | 1012d 12:30:21.6170000 -> 62<br>1012d 12:30:21.0040000 -> 0                                |
    * | "FFF"                | If non-zero, the milliseconds in a TimeSpan value. This value is always positive.                      | 1012d 12:30:21.6170000 -> 617<br>1012d 12:30:21.0004000 -> 0                               |
    * | "FFFF"               | If non-zero, the ten thousandths of a second in a TimeSpan value. This value is always positive.       | 1012d 12:30:21.6175000 -> 6175<br>1012d 12:30:21.0000400 -> 0                              |
    * | "FFFFF"              | If non-zero, the hundred thousandths of a second in a TimeSpan value. This value is always positive.   | 1012d 12:30:21.6175400 -> 61754<br>1012d 12:30:21.0000040 -> 0                             |
    * | "FFFFFF"             | If non-zero, the millionths of a second in a TimeSpan value. This value is always positive.            | 1012d 12:30:21.6175420 -> 617542<br>1012d 12:30:21.0000004 -> 0                            |
    * | "h"                  | The hour, using a 24-hour clock from 0 to 23. This value is always positive.                           | 1012d 13:30:21 -> 13<br>1012d 2:30:21 -> 2                                                 |
    * | "hh"                 | The hour, using a 24-hour clock from 00 to 23. This value is always positive.                          | 1012d 13:30:21 -> 13<br>1012d 2:30:21 -> 02                                                |
    * | "i"                  | The milliseconds, from 0 through 999. This value is always positive.                                   | 1012d 12:30:21.0175420 -> 18<br>2009-06-15T13:45:30.0002420 -> 0                           |
    * | "iii"                | The milliseconds, from 000 through 999. This value is always positive.                                 | 1012d 12:30:21.6175420 -> 618<br>2009-06-15T13:45:30.0002420 -> 000                        |
    * | "n"                  | Sign character. Both + and - are shown.                                                                | 1012d 12:30:21 -> +<br>-1012d 12:30:21 -> -                                                |
    * | "N"                  | Sign character. Only minus sign is shown.                                                              | 1012d 12:30:21 -> (nothing)<br>-1012d 12:30:21 -> -                                        |
    * | "m"                  | The minute, from 0 through 59. This value is always positive.                                          | 1012d 1:05:01.0170420 -> 5<br>1012d 1:29:01 -> 29                                          |
    * | "mm"                 | The minute, from 00 through 59. This value is always positive.                                         | 1012d 1:05:01.0170420 -> 05<br>1012d 1:29:01 -> 29                                         |
    * | "s"                  | The second, from 0 through 59. This value is always positive.                                          | 1012d 1:05:01.0170420 -> 1                                                                 |
    * | "ss"                 | The second, from 00 through 59. This value is always positive.                                         | 1012d 1:05:01.0170420 -> 01                                                                |
    * | "u"                  | The microseconds, from 0 through 999. This value is always positive.                                   | 1012d 1:45:21.0170420 -> 42<br>1012d 1:45:21.6780004 -> 0                                  |
    * | "uuu"                | The microseconds, from 000 through 999. This value is always positive.                                 | 1012d 1:45:21.0170420 -> 042<br>1012d 1:45:21.6780004 -> 000                               |
    * | "string" or 'string' | Literal string delimiter.                                                                              | 1012d 1:45:21 ("arr:" h:m t) -> arr: 1:45                                                  |
    * | "\"                  | The escape character.                                                                                  | 1012d 12:30:21 (h \h) -> 12 h                                                              |
    * | Any other character  | The character is copied to the result string unchanged.                                                | 1012d 12:30:21 (arr hh:mm) -> arr 12:30                                                    |
    * Format specifiers "f", "F", "y" has not limited maximum count, but other specifiers specified in table above has limited maximum counts.
    *
    * @param monthNames Array with custom month names. This array has to contain exactly 12 strings. If set to NULL, English names are used.
    * @param weekDayNames Array with custom days of week. This array has to contain exactly 7 strings and first name has to be Sunday, then Monday and so on. If set to NULL, English names are used.
    * @return Returns pointer to buffer, where null terminator was inserted.
    */
#ifdef ARDUINO
    String toString(const char* format) const;
#else
    std::string toString(const char* format) const;
#endif // ARDUINO

    const static TimeSpan MaxValue;
    const static TimeSpan MinValue;
    const static TimeSpan Zero;

    static inline TimeSpan FromMicroseconds(int64_t micros) {
        return TimeSpan(micros * MICROSECOND);
    }

    static inline TimeSpan FromMilliseconds(int64_t milliseconds) {
        return TimeSpan(milliseconds * MILLISECOND);
    }

    static inline TimeSpan FromSeconds(int64_t seconds) {
        return TimeSpan(seconds * SECOND);
    }

    static inline TimeSpan FromMinutes(int64_t minutes) {
        return TimeSpan(minutes * MINUTE);
    }

    static inline TimeSpan FromHours(int64_t hours) {
        return TimeSpan(hours * HOUR);
    }

    static inline TimeSpan FromDays(int64_t days) {
        return TimeSpan(days * DAY);
    }

    //operators:
    explicit operator int64_t() const {
        return raw_time;
    }

    explicit operator int64_t() {
        return raw_time;
    }

    TimeSpan operator-() const{
        return TimeSpan(-raw_time);
    }

    TimeSpan operator+(const TimeSpan& ts2) const {
        return TimeSpan(raw_time + ts2.getRaw());
    }

    TimeSpan operator+(const int64_t raw) const {
        return TimeSpan(raw_time + raw);
    }

    TimeSpan operator-(const TimeSpan& ts2) const {
        return TimeSpan(raw_time - ts2.getRaw());
    }

    TimeSpan operator-(const int64_t raw) const {
        return TimeSpan(raw_time - raw);
    }

    TimeSpan& operator+=(TimeSpan& ts2) {
        raw_time += ts2.getRaw();
        return *this;
    }

    TimeSpan& operator+=(const int64_t _raw) {
        raw_time += _raw;
        return *this;
    }

    TimeSpan& operator-=(TimeSpan& ts2) {
        raw_time -= ts2.getRaw();
        return *this;
    }

    TimeSpan& operator-=(const int64_t _raw) {
        raw_time -= _raw;
        return *this;
    }

    bool operator==(const TimeSpan& ts2) const {
        return raw_time == ts2.getRaw();
    }

    bool operator==(const int64_t raw) const {
        return raw_time == raw;
    }

    bool operator!=(const TimeSpan& ts2) const {
        return raw_time != ts2.getRaw();
    }

    bool operator!=(const int64_t raw) const {
        return raw_time != raw;
    }

    bool operator>=(const TimeSpan& ts2) const {
        return raw_time >= ts2.getRaw();
    }

    bool operator>=(const int64_t raw) const {
        return raw_time >= raw;
    }

    bool operator<=(const TimeSpan& ts2) const {
        return raw_time <= ts2.getRaw();
    }

    bool operator<=(const int64_t raw) const {
        return raw_time <= raw;
    }

    bool operator<(const TimeSpan& ts2) const {
        return raw_time < ts2.getRaw();
    }

    bool operator<(const int64_t raw) const {
        return raw_time < raw;
    }

    bool operator>(const TimeSpan& ts2) const {
        return raw_time > ts2.getRaw();
    }

    bool operator>(const int64_t raw) const {
        return raw_time > raw;
    }

private:
    int64_t raw_time = 0;
};

#endif // !_TIME_SPAN_H