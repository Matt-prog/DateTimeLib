# DateTimeLib
Cross-platform library for processing date and time with time zone and daylight saving time.
The main advantage is that it works with a resolution in microseconds with wide range (years from 292277 B.C. to 292277 A.D.).
During the design of this library, we tried to optimize it for speed and low RAM consumption, that's why it can be also used
with Arduino. Only Gregorian calendar is supported. This library is an enhanced version of the [DateTime](https://github.com/Matt-prog/DateTime) which is deprecated.

## Supported OS/MCUs:
- **Windows**
- **Linux**
- **Mac OS**
- **Arduino**
Minumum supported C++ version is C++11.

## Usage
### Date and time
The following date and time classes are defined in this library:
+ `DateTime` - can store date and time from year 292277 B.C. (macro `MIN_YEAR`) to 292277 A.D. (macro `MAX_YEAR`) with a resolution in microseconds. Date and time
is stored as count of microseconds from start of the epoch. This value is stored as 64 bit signed number.
+ `DateTimeSysSync` - acts as clock, which are synchronized with system clock. It's value is constantly updating.
+ `DateTimeTZ` - same as `DateTime`, but stores time zone and DST(daylight saving time) offsets. It also stores DST adjustments
rules. Every time, when DateTime value is set or (de)incremented, DST adjustment rules are checked and time is adjusted if needed.
+ `DateTimeTZSysSync` - same as `DateTimeTZ` and `DateTimeSysSync`, but DST adjustment is also checked, when any date or time field
is read. For better performance, every time DST is adjusted, next adjustment day is calculated. After that, it only checks, if
the day, when DST adjustment need to be done, is over or not. Because functions for reading date or time fields are marked with `const` keyword,
next adjustment day cannot be calculated inside getters, but only checked. That's why `recalcDST()` function should be called
at least once per period or for better performance more often. `recalcDST()` function checks DST adjustment only if next adjustment
day is over, that's why it can be called in main loop. When next adjustment day is over, `recalcDST()` was not called yet and some
getter is called, offset of next DST period is added to the result. Thanks this, correct value will be returned even, if `recalcDST()`
was not called, but this will be true only until next adjustment. In table below, you can see the difference between two cases, when
`recalcDST()` is never called and `recalcDST()` is called at least one time in period. At first DST adjustment, in both cases time is adjusted,
but at second DST adjustment, only case, when `recalcDST()` is called is DST adjusted. That's because next DST adjustment was not established
in `recalcDST()` function.
```
period         |   Summer      |                    Winter                  |               Summer
               |---------------|--------------------------------------------|------------------------->
no recalcDST() | +1   +1   +1  | +0   +0   +0   +0   +0   +0   +0   +0   +0 | +0   +0   +0   +0   +0  
recalcDST()    | +1   +1   +1  | +0   +0   +0   +0   +0   +0   +0   +0   +0 | +1   +1   +1   +1   +1  
```
All `DateTime` instances does not supports leap seconds.

#### Converting DateTime to string or char array
Any `DateTime` instance can be converted to `string` (on Arduino to `String`) using member function called `toString()` or to char array using `toArray()`.
Both funcions has parameter `format`, which specifies format of converted date and time. We based our design of format specifiers on [C# DateTime custom format specifiers](https://docs.microsoft.com/en-us/dotnet/standard/base-types/custom-date-and-time-format-strings). Table below shows all available specifiers to be used with any `DateTime` instance.
| Format specifier     | Description                                                                               | Example                                                                                       |
|----------------------|-------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------|
| "d"                  | The day of the month, from 1 through 31.                                                  | 2009-06-01T13:45:30 -\> 1<br>2009-06-15T13:45:30 -\> 15                                       |
| "dd"                 | The day of the month, from 01 through 31.                                                 | 2009-06-01T13:45:30 -\> 01<br>2009-06-15T13:45:30 -\> 15                                      |
| "ddd"                | The abbreviated name of the day of the week.                                              | 2009-06-15T13:45:30 -\> Mon                                                                   |
| "dddd"               | The full name of the day of the week.                                                     | 2009-06-15T13:45:30 -\> Monday                                                                |
| "f"                  | The tenths of a second in a date and time value.                                          | 2009-06-15T13:45:30.6170000 -\> 6<br>2009-06-15T13:45:30.04 -\> 0                             |
| "ff"                 | The hundredths of a second in a date and time value.                                      | 2009-06-15T13:45:30.6170000 -\> 62<br>2009-06-15T13:45:30.0040000 -\> 00                      |
| "fff"                | The milliseconds in a date and time value.                                                | 2009-06-15T13:45:30.6170000 -\> 617<br>2009-06-15T13:45:30.0004000 -\> 000                    |
| "ffff"               | The ten thousandths of a second in a date and time value.                                 | 2009-06-15T13:45:30.6175000 -\> 6175<br>2009-06-15T13:45:30.0000400 -\> 0000                  |
| "fffff"              | The hundred thousandths of a second in a date and time value.                             | 2009-06-15T13:45:30.6175400 -\> 61754<br>2009-06-15T13:45:30.0000040 -\> 00000                |
| "ffffff"             | The millionths of a second in a date and time value.                                      | 2009-06-15T13:45:30.6175420 -\> 617542<br>2009-06-15T13:45:30.0000004 -\> 000000              |
| "F"                  | If non-zero, the tenths of a second in a date and time value.                             | 2009-06-15T13:45:30.6170000 -\> 6<br>2009-06-15T13:45:30.0400000 -\> 0                        |
| "FF"                 | If non-zero, the hundredths of a second in a date and time value.                         | 2009-06-15T13:45:30.6170000 -\> 62<br>2009-06-15T13:45:30.0040000 -\> 0                       |
| "FFF"                | If non-zero, the milliseconds in a date and time value.                                   | 2009-06-15T13:45:30.6170000 -\> 617<br>2009-06-15T13:45:30.0004000 -\> 0                      |
| "FFFF"               | If non-zero, the ten thousandths of a second in a date and time value.                    | 2009-06-15T13:45:30.6175000 -\> 6175<br>2009-06-15T13:45:30.0000400 -\> 0                     |
| "FFFFF"              | If non-zero, the hundred thousandths of a second in a date and time value.                | 2009-06-15T13:45:30.6175400 -\> 61754<br>2009-06-15T13:45:30.0000040 -\> 0                    |
| "FFFFFF"             | If non-zero, the millionths of a second in a date and time value.                         | 2009-06-15T13:45:30.6175420 -\> 617542<br>2009-06-15T13:45:30.0000004 -\> 0                   |
| "g"                  | The period or era. Always shown.                                                          | 2009-06-15T13:45:30.6170000 -\> A.D.<br>2009-06-15T13:45:30.6170000 B.C. -\> B.C.             |
| "G"                  | The period or era. Only B.C. is shown.                                                    | 2009-06-15T13:45:30.6170000 -\> (nothing)<br>2009-06-15T13:45:30.6170000 B.C. -\> B.C.        |
| "h"                  | The hour, using a 12-hour clock from 1 to 12.                                             | 2009-06-15T01:45:30 -\> 1<br>2009-06-15T13:45:30 -\> 1                                        |
| "hh"                 | The hour, using a 12-hour clock from 01 to 12.                                            | 2009-06-15T01:45:30 -\> 01<br>2009-06-15T13:45:30 -\> 01                                      |
| "H"                  | The hour, using a 24-hour clock from 0 to 23.                                             | 2009-06-15T01:45:30 -\> 1<br>2009-06-15T13:45:30 -\> 13                                       |
| "HH"                 | The hour, using a 24-hour clock from 00 to 23.                                            | 2009-06-15T01:45:30 -\> 01<br>2009-06-15T13:45:30 -\> 13                                      |
| "i"                  | The milliseconds, from 0 through 999.                                                     | 2009-06-15T13:45:30.0175420 -\> 17<br>2009-06-15T13:45:30.0005420 -\> 0                       |
| "iii"                | The milliseconds, from 000 through 999.                                                   | 2009-06-15T13:45:30.0175420 -\> 017<br>2009-06-15T13:45:30.0005420 -\> 000                    |
| "l"                  | DST offset in hours, with no leading zeros.                                               | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +1                                                |
| "ll"                 | DST offset in hours, with a leading zero for a single-digit value.                        | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +01                                               |
| "lll"                | DST offset in hours and minutes with ':'.                                                 | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +01:00                                            |
| "llll"               | DST offset in hours and minutes.                                                          | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> +0100                                             |
| "n"                  | The period or era represented by sign. Both + and - are shown.                            | 2009-06-15T13:45:30.6170000 -\> +<br>2009-06-15T13:45:30.6170000 B.C. -\> -                   |
| "N"                  | The period or era represented by sign. Only minus sign is shown.                          | 2009-06-15T13:45:30.6170000 -\> (nothing)<br>2009-06-15T13:45:30.6170000 B.C. -\> -           |
| "m"                  | The minute, from 0 through 59.                                                            | 2009-06-15T01:09:30 -\> 9<br>2009-06-15T13:29:30 -\> 29                                       |
| "mm"                 | The minute, from 00 through 59.                                                           | 2009-06-15T01:09:30 -\> 09<br>2009-06-15T13:29:30 -\> 29                                      |
| "M"                  | The month, from 1 through 12.                                                             | 2009-06-15T13:45:30 -\> 6                                                                     |
| "MM"                 | The month, from 01 through 12.                                                            | 2009-06-15T13:45:30 -\> 06                                                                    |
| "MMM"                | The abbreviated name of the month.                                                        | 2009-06-15T13:45:30 -\> Jun                                                                   |
| "MMMM"               | The full name of the month.                                                               | 2009-06-15T13:45:30 -\> June                                                                  |
| "s"                  | The second, from 0 through 59.                                                            | 2009-06-15T13:45:09 -\> 9                                                                     |
| "ss"                 | The second, from 00 through 59.                                                           | 2009-06-15T13:45:09 -\> 09                                                                    |
| "t"                  | The first character of the AM/PM designator.                                              | 2009-06-15T13:45:30 -\> P                                                                     |
| "tt"                 | The AM/PM designator.                                                                     | 2009-06-15T13:45:30 -\> PM                                                                    |
| "u"                  | The microseconds, from 0 through 999.                                                     | 2009-06-15T13:45:30.0170420 -\> 42<br>2009-06-15T13:45:30.6780004 -\> 0                       |
| "uuu"                | The microseconds, from 000 through 999.                                                   | 2009-06-15T13:45:30.0170420 -\> 042<br>2009-06-15T13:45:30.6780004 -\> 000                    |
| "y"                  | The year, from 0 to 99.                                                                   | 0001-01-01T00:00:00 -\> 1<br>0900-01-01T00:00:00 -\> 0<br>2009-06-15T13:45:30 -\> 9           |
| "yy"                 | The year, from 00 to 99.                                                                  | 0001-01-01T00:00:00 -\> 01<br>0900-01-01T00:00:00 -\> 00<br>2009-06-15T13:45:30 -\> 09        |
| "yyy"                | The year as a three-digit number.                                                         | 0001-01-01T00:00:00 -\> 001<br>0900-01-01T00:00:00 -\> 900<br>2009-06-15T13:45:30 -\> 009     |
| "yyyy"               | The year as a four-digit number.                                                          | 0001-01-01T00:00:00 -\> 0001<br>0900-01-01T00:00:00 -\> 0900<br>2009-06-15T13:45:30 -\> 2009  |
| "yyyyy"              | The year as a five-digit number.                                                          | 0001-01-01T00:00:00 -\> 00001<br>2009-06-15T13:45:30 -\> 02009                                |
| "z"                  | Time zone offset in hours from UTC, with no leading zeros.                                | 2009-06-15T13:45:30 GMT-07:00 DST+01:00 -\> -7                                                |
| "zz"                 | Time zone offset in hours from UTC, with a leading zero for a single-digit value.         | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -07                                                |
| "zzz"                | Time zone offset in hours and minutes with ':' from UTC.                                  | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -07:00                                             |
| "zzzz"               | Time zone offset in hours and minutes from UTC.                                           | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -0700                                              |
| "Z"                  | Time zone and DST offset in hours from UTC, with no leading zeros.                        | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -6                                                 |
| "ZZ"                 | Time zone and DST offset in hours from UTC, with a leading zero for a single-digit value. | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -06                                                |
| "ZZZ"                | Time zone and DST offset in hours and minutes with ':' from UTC                           | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -06:00                                             |
| "ZZZ"                | Time zone and DST offset in hours and minutes from UTC                                    | 2009-06-15T13:45:30 GMT-07:00 DST+01:00-\> -0600                                              |
| "string" or 'string' | Literal string delimiter.                                                                 | 2009-06-15T13:45:30 ("arr:" h:m t) -\> arr: 1:45 P                                            |
| "\\"                  | The escape character.                                                                     | 2009-06-15T13:45:30 (h \h) -\> 1 h                                                            |
| *Any other character*  | The character is copied to the result string unchanged.                                   | 2009-06-15T01:45:30 (arr hh:mm t) -\> arr 01:45 A                                             |

Format specifiers "f", "F", "y" has not limited maximum count, but other specifiers specified in table above has limited maximum counts. Those are the most used formats:
```
yyyy-MM-ddTHH:mm:ssZZZ          -> ISO8601
yyyy-MM-ddTHH:mm:ss.fffZZZ      -> ISO8601 with milliseconds
yyyy-MM-ddTHH:mm:ss.ffffffZZZ   -> ISO8601 with microseconds
yyyy-MM-ddTHH:mm:ss             -> ISO8601 without time zone offset
yyyy-MM-ddTHH:mm:ss.fff         -> ISO8601 with milliseconds, without time zone offset
yyyy-MM-ddTHH:mm:ss.ffffff      -> ISO8601 with microseconds, without time zone offset
d/M/yyyy                        -> Short date
dd/MM/yyyy                      -> Short date with leading zeros
dddd, MMMM d, yyyy              -> Long date
dddd, MMMM dd, yyyy             -> Long date with leading zeros
```

#### Parsing DateTime
Any `DateTime` instance can be parsed using member function `parse()`. It has `format` parameter too, which has exactly same format specifiers, see table above. There is also option `matchText`, which can be set to *true* to exactly match text, which is defined in `format`. If set to *false*, only count of text characters is matched. When parsing was successful function returns count of parsed characters. If parsing failed, it will return negative or zero value, which is position of character, which caused parsing error.

### Time span (duration)
A `TimeSpan` class represents duration or offset. It works with same principle as `DateTime`, so duration is represented as count of microseconds.
This value is stored in 64 bit signed integer. It's range is from -106,751,983 to 106,751,981 days. It is also defined in macros `MIN_DAYS` and `MAX_DAYS`.
A `TimeSpan` is returned as result of some `DateTime` operators:
+ `DateTime - DateTime = TimeSpan`
+ `DateTime + TimeSpan = DateTime`
+ `DateTime - TimeSpan = DateTime`

#### Converting TimeSpan to string or char array
`TimeSpan` can be also converted to string or char array same as `DateTime`, but it has slightly different format specifiers:
| Format specifier     | Description                                                                                            | Example                                                                                    |
|----------------------|--------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------|
| "d"                  | The absolute number of whole days in the time interval.                                                | 1012d 12:30:21 -> 1012<br>-10d 12:30:21 -> 10                                              |
| "dd" - "dddddddd"    | The absolute number of whole days in the time interval, padded with leading zeros as needed.           | for ddddd: -1012d 12:30:21 -> 01012<br>0d 12:30:21 -> 0                                    |
| "f"                  | The tenths of a second in a TimeSpan value. This value is always positive.                             | 1012d 12:30:21.6170000 -> 6<br>1012d 12:30:21.04 -> 0                                      |
| "ff"                 | The hundredths of a second in a TimeSpan value. This value is always positive.                         | 1012d 12:30:21.6170000 -> 62<br>1012d 12:30:21.0040000 -> 00                               |
| "fff"                | The milliseconds in a TimeSpan value. This value is always positive.                                   | 1012d 12:30:21.6170000 -> 617<br>1012d 12:30:21.0004000 -> 000                             |
| "ffff"               | The ten thousandths of a second in a TimeSpan value. This value is always positive.                    | 1012d 12:30:21.6175000 -> 6175<br>1012d 12:30:21.0000400 -> 0000                           |
| "fffff"              | The hundred thousandths of a second in a TimeSpan value. This value is always positive.                | 1012d 12:30:21.6175400 -> 61754<br>1012d 12:30:21.0000040 -> 00000                         |
| "ffffff"             | The millionths of a second in a TimeSpan value. This value is always positive.                         | 1012d 12:30:21.6175420 -> 617542<br>1012d 12:30:21.0000004 -> 000000                       |
| "F"                  | If non-zero, the tenths of a second in a TimeSpan value. This value is always positive.                | 1012d 12:30:21.6170000 -> 6<br>1012d 12:30:21.0400000 -> 0                                 |
| "FF"                 | If non-zero, the hundredths of a second in a TimeSpan value. This value is always positive.            | 1012d 12:30:21.6170000 -> 62<br>1012d 12:30:21.0040000 -> 0                                |
| "FFF"                | If non-zero, the milliseconds in a TimeSpan value. This value is always positive.                      | 1012d 12:30:21.6170000 -> 617<br>1012d 12:30:21.0004000 -> 0                               |
| "FFFF"               | If non-zero, the ten thousandths of a second in a TimeSpan value. This value is always positive.       | 1012d 12:30:21.6175000 -> 6175<br>1012d 12:30:21.0000400 -> 0                              |
| "FFFFF"              | If non-zero, the hundred thousandths of a second in a TimeSpan value. This value is always positive.   | 1012d 12:30:21.6175400 -> 61754<br>1012d 12:30:21.0000040 -> 0                             |
| "FFFFFF"             | If non-zero, the millionths of a second in a TimeSpan value. This value is always positive.            | 1012d 12:30:21.6175420 -> 617542<br>1012d 12:30:21.0000004 -> 0                            |
| "h"                  | The hour, using a 24-hour clock from 0 to 23. This value is always positive.                           | 1012d 13:30:21 -> 13<br>1012d 2:30:21 -> 2                                                 |
| "hh"                 | The hour, using a 24-hour clock from 00 to 23. This value is always positive.                          | 1012d 13:30:21 -> 13<br>1012d 2:30:21 -> 02                                                |
| "i"                  | The milliseconds, from 0 through 999. This value is always positive.                                   | 1012d 12:30:21.0175420 -> 18<br>2009-06-15T13:45:30.0002420 -> 0                           |
| "iii"                | The milliseconds, from 000 through 999. This value is always positive.                                 | 1012d 12:30:21.6175420 -> 618<br>2009-06-15T13:45:30.0002420 -> 000                        |
| "n"                  | Sign character. Both + and - are shown.                                                                | 1012d 12:30:21 -> +<br>-1012d 12:30:21 -> -                                                |
| "N"                  | Sign character. Only minus sign is shown.                                                              | 1012d 12:30:21 -> (nothing)<br>-1012d 12:30:21 -> -                                        |
| "m"                  | The minute, from 0 through 59. This value is always positive.                                          | 1012d 1:05:01.0170420 -> 5<br>1012d 1:29:01 -> 29                                          |
| "mm"                 | The minute, from 00 through 59. This value is always positive.                                         | 1012d 1:05:01.0170420 -> 05<br>1012d 1:29:01 -> 29                                         |
| "s"                  | The second, from 0 through 59. This value is always positive.                                          | 1012d 1:05:01.0170420 -> 1                                                                 |
| "ss"                 | The second, from 00 through 59. This value is always positive.                                         | 1012d 1:05:01.0170420 -> 01                                                                |
| "u"                  | The microseconds, from 0 through 999. This value is always positive.                                   | 1012d 1:45:21.0170420 -> 42<br>1012d 1:45:21.6780004 -> 0                                  |
| "uuu"                | The microseconds, from 000 through 999. This value is always positive.                                 | 1012d 1:45:21.0170420 -> 042<br>1012d 1:45:21.6780004 -> 000                               |
| "string" or 'string' | Literal string delimiter.                                                                              | 1012d 1:45:21 ("arr:" h:m t) -> arr: 1:45                                                  |
| "\"                  | The escape character.                                                                                  | 1012d 12:30:21 (h \h) -> 12 h                                                              |
| Any other character  | The character is copied to the result string unchanged.                                                | 1012d 12:30:21 (arr hh:mm) -> arr 12:30                                                    |

Format specifiers "f", "F", "y" has not limited maximum count, but other specifiers specified in table above has limited maximum counts.

Parsing is not supported for `TimeSpan` yet.

### DateTime and TimeSpan operators
#### Conversion operators
All `DateTime` instances can be converted to each other, but each conversion behaves differently:
```
DateTime -> DateTime                   - [implicit] date and time fields are copied
DateTime -> DateTimeSysSync            - [implicit] date and time fields are copied and time will be constantly updating
DateTime -> DateTimeTZ                 - [implicit] date and time fields are copied and UTC time zone is set
DateTime -> DateTimeTZSysSync          - [implicit] date and time fields are copied, UTC time zone is set and time will be constantly updating
DateTimeSysSync -> DateTime            - [explicit] time is captured and copied to DateTime
DateTimeSysSync -> DateTimeSysSync     - [implicit] copies date and time fields and synchronizes it, so it will have exactly same value
DateTimeSysSync -> DateTimeTZ          - [explicit] same as conversion to DateTime, but time zone is set to UTC
DateTimeSysSync -> DateTimeTZSysSync   - [implicit] copies date and time fields and synchronizes it, so it will have exactly same value. Time zone is set to UTC
DateTimeTZ -> DateTime                 - [explicit] local date and time fields are copied
DateTimeTZ -> DateTimeSysSync          - [explicit] local date and time fields are copied and time will be constantly updating
DateTimeTZ -> DateTimeTZ               - [implicit] date and time fields with time zone are copied
DateTimeTZ -> DateTimeTZSysSync        - [implicit] date and time fields with time zone are copied and time will be constantly updating
DateTimeTZSysSync -> DateTime          - [explicit] local time is captured and copied to DateTime
DateTimeTZSysSync -> DateTimeSysSync   - [explicit] copies local date and time fields and synchronizes it, so it will have exactly same value
DateTimeTZSysSync -> DateTimeTZ        - [explicit] same as conversion to DateTime, but also time zone is copied
DateTimeTZSysSync -> DateTimeTZSysSync - [implicit] copies date and time fields with time zone and synchronizes it, so it will have exactly same value
```
Conversion from `DateTimeSysSync` (or `DateTimeTZSysSync`) to `DateTime` (or `DateTimeTZ`) can be used to capture current time. It can be used for example for measuring duration of some event:
```
DateTimeSysSync clock = DateTimeSysSync::now(); //Synchronizes time with OS, cannot be used with Arduino
DateTime start = (DateTime)clock;

//Some event, which duration will be measured

DateTime end = (DateTime)clock;
TimeSpan duration = end - start; //Duration of event
```

#### Arithmetic operators
All `DateTime` instances (`DateTimeSysSync`, `DateTimeTZ`, `DateTimeTZSysSync`) supports those arithmetic operators:
```
DateTime - DateTime = TimeSpan
DateTime + TimeSpan = DateTime //+= can be used too
DateTime + int64_t  = DateTime //+= can be used too
DateTime - TimeSpan = DateTime //-= can be used too
DateTime - int64_t  = DateTime //-= can be used too
DateTime++
DateTime--

DateTimeSysSync - DateTimeSysSync = TimeSpan
DateTimeSysSync + TimeSpan = DateTimeSysSync //+= can be used too
...
```
Oprators can also combine different types of `DateTime` instances, for example:
```
DateTimeSysSync - DateTime = TimeSpan
DateTimeTZ - DateTime = TimeSpan
...
```
Result of arithmetic or comparison operators when combining different types of `DateTime` instances can be different when:
+ **both** subtracted types contains time zone (`DateTimeTZ` and `DateTimeTZSysSync`) time is converted to UTC first and after that subtraction is done,
+ **only one** subtracted type contains time zone (`DateTimeTZ` and `DateTimeTZSysSync`) local time is used during subtraction.

#### Comparison operators
All `DateTime` instances (`DateTimeSysSync`, `DateTimeTZ`, `DateTimeTZSysSync`) supports those comparison operators:
```
DateTime == DateTime
DateTime != DateTime
DateTime > DateTime
DateTime < DateTime
DateTime >= DateTime
DateTime <= DateTime

DateTimeSysSync == DateTimeSysSync
DateTimeSysSync != DateTimeSysSync
...
```
Oprators can also combine different types of `DateTime` instances, for example:
```
DateTimeSysSync == DateTime
DateTimeTZ != DateTime
...
```
Result of arithmetic or comparison operators when combining different types of `DateTime` instances can be different when:
+ **both** compared types contains time zone (`DateTimeTZ` and `DateTimeTZSysSync`) time is converted to UTC first and after that comparison is done,
+ **only one** compared type contains time zone (`DateTimeTZ` and `DateTimeTZSysSync`) local time is used during comparison.

### Time zone and DST adjustment
The following related classes are defined in this library:
+ `TimeZone` - represents time zone offset from UTC (negative to west, positive to east). Offset is represented with resolution of 15 minutes.
+ `DSTTransitionRule` - represents one transition rule. There are 3 types of transition rule representation:
  + **Floating** - commonly used type. Rule contains fields: *transition hour*, *transition month*, *week of month*, *day of week* and optionally *days offset*.
  Example: *"Last sunday in March at 1:00"*.
  + **Date** - exact date, when transition happend is specified. Rule contains fields: *transition hour*, *transition month*, *day of month* and optionally *days offset*. Example: *"The twentieth of March at 1:00"*. This type handles leap days.
  + **Fixes** - zero based day number, when transition happens. Value can be from range 0 to 364. Rule contains fields: *transition hour*, *transition day of year*. Example: *"The first hundred day of year"*. This type does not handles leap days.
  
  All fields in transition rule has to be in local time, so they must have applied time zone offset and DST adjustment end rules must have applied DST offset. 
+ `DSTAdjustment` - contains start and end transition rule and DST offset.
+ `TimeZoneInfo` - contains all informations about current time zone. Fields:
  + `timeZone` - time zone offset.
  + `DST` - DST transition rules and offset.
  + `standardName` - Name of time zone without DST. On Windows it can be translated to system language.
  + `daylightName` - Name of time zone with DST. On Windows it can be translated to system language.
  + `keyName` - Key name of time zone. On Linux and Mac Olson name (e.g. "Europe/Berlin") on Windows, Windows time zone name (e.g. "Standard European Time").
  + `standardABR` - Abbreviation time zone name, for example: "CET".
  + `daylightABR` - Abbreviation time zone name with DST, for example: "CEST".
  
  `TimeZoneInfo` can be converted to or parsed from POSIX time zone format. `standardABR` and `daylightABR` must not be empty before conversion. Parsing
  won't update `keyName`, `standardName` and `daylightName` fields, because those are not specified in POSIX time zone format.

## Class diagram of DateTime
This library uses static polymorphism (no virtual methods, just templates) on DateTime classes. This system was chosen to achieve the best performance and extensibility. There are 3 main base classes, which was not mentioned yet:
+ `DateTimeBase` - class, that specifies basic interface for all DateTime's. It does not specifies how date and time fields are stored, only specifies functions, that can convert date and time fields to raw 64-bit signed integer and basic arithmetic and comparison operators. This interface is prepared for RTC extension of current library.
+ `DateTimeRawBase` - class, that is derived from `DateTimeBase`, which can also store raw date and time value. It also defines all methods for `DateTime` class.
+ `DateTimeTZBase` - class, that is derived from `DateTimeRawBase`, which can also store time zone and DST adjustment and defines all methods for `DateTimeTZ` class.
DateTime class diagram is located [here](docs/DateTime_class_diagram.png).

### Extensions
As mentioned above, current DateTime library can be extended. For example if you want to create `DateTimeRTC` class, which will be wrapper for real time clock on MCU, you have to do this:
1. `DateTimeRTC` **must** contain functions:
  + `int64_t getRawValueDer() const` - which reads all date and time fields from RTC and converts this time to raw 64-bit signed integer value. For conversion from date and time fields you can use `dtlib::dateTimeToRaw(date_time_s)` helper function.
  + `void setRawValueDer(int64_t)` - which sets all date and time fields in RTC from raw 64-bit signed integer value. For conversion raw value to date and time fields you can use `dtlib::rawToDateTime(int64_t)` helper function.
  + `void addRawValueDer(int64_t)` - which adds specified amount of microseconds to date and time. Both `dtlib::dateTimeToRaw(date_time_s)` and `dtlib::rawToDateTime(int64_t)` helper functions can be used.
2. `DateTimeRTC` **may** contain functions to support time zone and DST:
  + `void setRawTimeTD(int64_t)` - which sets raw value and checks DST adjustment.
  + `void addRawTimeTD(int64_t)` - which adds value to raw value and checks DST adjustment.
  + `int16_t getTimeZoneOffsetMinutes() const` - which gets time zone offset in minutes.
  + `int16_t getCurrentDSTOffsetMinutes() const` - which gets current DST offset in minutes.
3. You **must** specify all methods with same name and same paramters as `DateTimeRawBase`. To specify those methods, it is recommended to use helper functions from `dtlib` namespace from file `DateTimeHelpers.h`.
4. If `DateTimeRTC` would also store time zone, `set()` member function has to be specified also for classes, whic has `int16_t getTimeZoneOffsetMinutes() const` and `int16_t getCurrentDSTOffsetMinutes() const` functions. For inspiration, you can see, how is it implemented in `DateTimeTZ.h` file.
5. You **must** specify all constructors.

## Future plans
- [ ] Add `isDST` flags to `date_s`, `time_s`, `date_time_s` and make `DateTimeTZ` and `DateTimeTZSysSync` to accept it.
- [ ] Make possible to get system time zone info on Linux and Mac OS.
- [ ] Make possible to get at least time zone abbreviation names on Windows with `getCurrentSystemTZInfo().
- [ ] Make `setSystemTime()` and `setSystemTimeUTC()` for ESP32 and ESP8266.
- [ ] Make possible conversions to/from chrono library classes.
- [ ] Add conversions from/to Julian date.
- [ ] Maybe add software alarm from old [DateTime](https://github.com/Matt-prog/DateTime) library.
- [ ] Add more examples.

## Projects, which uses this library
+ [WorldTimeAPI client](https://github.com/Matt-prog/WorldTimeAPI) for Windows, Linux, Mac OS, ESP32 and ESP8266 implemented in C++ language.

## Contribution
Any contribution is appreciated.

## Author
👤 [Matej Fitoš](https://github.com/Matt-prog)

## License
Copyright © 2022 [Matej Fitoš](https://github.com/Matt-prog).

This project is [MIT](https://github.com/Matt-prog/WorldTimeAPI/blob/main/LICENSE) licensed.
