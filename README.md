# DateTimeLib
Cross-platform library for processing date and time with time zone and daylight saving time. Only Gregorian calendar is supported.
The main advantage is that it works with a resolution in microseconds with wide range (years from 292277 B.C. to 292277 A.D.).
During the design of this library, we tried to optimize it for speed and low RAM consumption, that's why it can be also used
with Arduino. This library is an enhanced version of the [DateTime](https://github.com/Matt-prog/DateTime) which is deprecated.

## Supported OS/MCUs:
- **Windows**
- **Linux**
- **Mac OS**
- **Arduino**

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
  
## TODO
- [x] TimeSpan
- [x] TimeZone and DSTAdjustment
- [x] TimeZoneInfo and POSIX time zone
- [ ] Conversions and operators
- [ ] Examples (operators, capturing time from SysSync, ...)
- [x] Formatting and parsing
- [ ] Leap seconds are unsupported
- [ ] Prepared for extensions (RTC, GPS time, ...)
- [ ] Static polymorphism and UML class diagram
- [ ] Reference to WorldTimeAPI and NTPClient class.
- [ ] Future plans (Retrieving system time zone and DST adjustment for Linux and Mac OS)
- [ ] Difference between old and new DateTime
- [ ] Make old DateTime deprecated.
