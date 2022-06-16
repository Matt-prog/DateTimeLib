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
The following classes are defined in this library:
+ `DateTime` - can store date and time from year 292277 B.C. to 292277 A.D. with a resolution in microseconds.
+ `DateTimeSysSync` - acts as clock, which are synchronized with system clock. It's value is constantly updating.
+ `DateTimeTZ` - same as `DateTime`, but stores time zone and DST(daylight saving time) offsets. It also stores DST adjustments
rules. Every time, when DateTime value is set or (de)incremented, DST adjustment rules are checked and time is adjusted if needed.
+ `DateTimeTZSysSync` - same as `DateTimeTZ` and `DateTimeSysSync`, but DST adjustment is also checked, when any date or time field
is read. For better performance, every time DST is adjusted, next adjustment day is calculated. After that, it only checks, if
the day, when adjustment need to be done, is over or not. Because functions for reading date or time fields are marked with `const` keyword,
next adjustment day cannot be calculated inside getters, but only checked. That's why `recalcDST()` function should be called
at least once per day (or for better performance more often). `recalcDST()` function checks DST adjustment only if next adjustment
day is over. When next adjustment day is over, `recalcDST()` was not called yet and some getter is called, offset of next DST period
is added to the result. Thanks this, correct value will be returned even, if `recalcDST()` was not called, but this will be true only until
next adjustment.
```
period         |   DST         |                   NO DST                   |                 DST
               |---------------|--------------------------------------------|------------------------->
no recalcDST() | +1   +1   +1  | +0   +0   +0   +0   +0   +0   +0   +0   +0 | +0   +0   +0   +0   +0  
recalcDST()    | +1   +1   +1  | +0   +0   +0   +0   +0   +0   +0   +0   +0 | +1   +1   +1   +1   +1  
```

## TODO
- [ ] TimeSpan
- [ ] TimeZone and DSTAdjustment
- [ ] TimeZoneInfo and POSIX time zone
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
