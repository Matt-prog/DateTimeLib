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
- [ ] Formatting and parsing
- [ ] Leap seconds are unsupported
- [ ] Prepared for extensions (RTC, GPS time, ...)
- [ ] Static polymorphism and UML class diagram
- [ ] Reference to WorldTimeAPI and NTPClient class.
- [ ] Future plans (Retrieving system time zone and DST adjustment for Linux and Mac OS)
- [ ] Difference between old and new DateTime
- [ ] Make old DateTime deprecated.
