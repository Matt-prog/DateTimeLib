
#include "DateTimeHelpers.h"

#ifndef ARDUINO
#include <cstring>
#endif // !ARDUINO

#if DT_UNDER_OS > 0
int64_t getSysTicks() {
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now()).time_since_epoch()).count();
}
#endif // DT_UNDER_OS > 0

uint8_t Month::getMonthLength(Month month, bool leapYear) {
	return month.getMonthLength(leapYear);
}

uint8_t Month::getMonthLength(bool leapYear) const{
	uint8_t val = value - 1; //Move to 0 index
	if (val < 12) {
		static const uint8_t monthLengths[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
		if (leapYear && val == 1) {
			return 29;
		}
		else {
			return monthLengths[val];
		}
	}
	else {
		return 0;
	}
}

namespace dtlib {
	date_time_s rawToDateTime(int64_t raw) {
		date_time_s time;

		int32_t days;
		int64_t micr; //Remaining microseconds
		//calculate days from microseconds
		bool neg = raw < 0;
		if (neg) {
			raw++;
			days = (int32_t)((raw / DAY) - 1);
			micr = DAY + (raw % DAY) - 1;
		}
		else {
			days = (int32_t)(raw / DAY);
			micr = raw % DAY;
		}

		//Time calculation
		time.hours.setHours24((uint8_t)(micr / HOUR));
		micr = micr % HOUR;
		time.minutes = (uint8_t)(micr / MINUTE);
		int32_t micr32 = micr % MINUTE; //Using micr32 as int32_t to calculate using 32 bit arithmetic
		time.seconds = (uint8_t)(micr32 / SECOND);
		micr32 = micr32 % SECOND;
		time.milliseconds = (uint16_t)(micr32 / MILLISECOND);
		micr32 = micr32 % MILLISECOND;
		time.microseconds = micr32;

		//Date calculation
		year_day_tuple yd = getYearFromDays(days);
		month_day_tuple md = getMonthFromDayOfYear(yd.dayOfYear, isLeapYear(yd.year));
		time.year = yd.year;
		time.month = md.month;
		time.day = md.day;
		if (neg) {
			//BC
			time.dayOfWeek = (DayOfWeek)(7 + ((days - 5) % 7));
		}
		else {
			//AD
			time.dayOfWeek = (DayOfWeek)(((days + 1) % 7) + 1);
		}
		return time;
	}

	int64_t dateTimeToRaw(date_time_s time) {
		//Time calculation
		int32_t raw32 = time.microseconds * MICROSECOND;
		raw32 += time.milliseconds * MILLISECOND;
		raw32 += time.seconds * SECOND;
		int64_t raw = time.minutes * MINUTE;
		time.hours.convertTo24(); //Converting to 24 hour format.
		raw += time.hours * HOUR;
		raw += raw32;

		//Date calculation
		int32_t days = (time.day - 1);
		days += getDayOfYearFromMonth(time.month, isLeapYear(time.year));
		days += getDaysUntilYear(time.year);
		raw += days * DAY;

		return raw;
	}


	date_s rawToDate(int64_t raw) {
		date_s date;

		int32_t days;
		bool neg = raw < 0;
		if (neg) {
			raw++;
			days = (int32_t)((raw / DAY) - 1);
		}
		else {
			days = (int32_t)(raw / DAY);
		}

		//Date calculation
		year_day_tuple yd = getYearFromDays(days);
		month_day_tuple md = getMonthFromDayOfYear(yd.dayOfYear, isLeapYear(yd.year));
		date.year = yd.year;
		date.month = md.month;
		date.day = md.day;
		if (neg) {
			//BC
			date.dayOfWeek = (DayOfWeek)(7 + ((days - 5) % 7));
		}
		else {
			//AD
			date.dayOfWeek = (DayOfWeek)(((days + 1) % 7) + 1);
		}
		return date;
	}

	//TODO tests
	int64_t dateToRaw(date_s date) {
		//Date calculation
		int32_t days = (date.day - 1);
		days += getDayOfYearFromMonth(date.month, isLeapYear(date.year));
		days += getDaysUntilYear(date.year);
		return ((int64_t)days) * DAY;
	}

	//TODO tests
	void setDateToRaw(int64_t& raw, date_s date) {
		int64_t rem_micr = getMicrosOfDayFromRaw(raw);

		int32_t days = (date.day - 1);
		days += getDayOfYearFromMonth(date.month, isLeapYear(date.year));
		days += getDaysUntilYear(date.year);

		raw = rem_micr + ((int64_t)days) * DAY;
	}

	time_s rawToTime(int64_t raw) {
		time_s time;

		int64_t micr; //Remaining microseconds
		//calculate days from microseconds
		bool neg = raw < 0;
		if (neg) {
			raw++;
			micr = DAY + (raw % DAY) - 1;
		}
		else {
			micr = raw % DAY;
		}

		//Time calculation
		time.hours.setHours24((uint8_t)(micr / HOUR));
		micr = micr % HOUR;
		time.minutes = (uint8_t)(micr / MINUTE);
		int32_t micr32 = micr % MINUTE; //Using micr32 as int32_t to calculate using 32 bit arithmetic
		time.seconds = (uint8_t)(micr32 / SECOND);
		micr32 = micr32 % SECOND;
		time.milliseconds = (uint16_t)(micr32 / MILLISECOND);
		micr32 = micr32 % MILLISECOND;
		time.microseconds = micr32;

		return time;
	}

	//TODO tests
	int64_t timeToRaw(time_s time) {
		int32_t raw32 = time.microseconds * MICROSECOND;
		raw32 += time.milliseconds * MILLISECOND;
		raw32 += time.seconds * SECOND;
		int64_t raw = time.minutes * MINUTE;
		time.hours.convertTo24(); //Converting to 24 hour format
		raw += time.hours * HOUR;
		raw += raw32;
		return raw;
	}

	//TODO tests
	void setTimeToRaw(int64_t& raw, time_s time) {
		raw -= getMicrosOfDayFromRaw(raw); //Setting microseconds of year to 0

		//Calculating new microseconds of year
		int32_t raw32 = time.microseconds * MICROSECOND;
		raw32 += time.milliseconds * MILLISECOND;
		raw32 += time.seconds * SECOND;
		raw += time.minutes * MINUTE;
		time.hours.convertTo24(); //Converting to 24 hour format
		raw += time.hours * HOUR;
		raw += raw32;
	}



	hour_t getHoursFromRaw(int64_t raw) {
		int64_t micr; //Remaining microseconds
		bool neg = raw < 0;
		if (neg) {
			raw++;
			micr = DAY + (raw % DAY) - 1;
		}
		else {
			micr = raw % DAY;
		}

		return hour_t((uint8_t)(micr / HOUR));
	}

	void setHoursToRaw(int64_t& raw, hour_t val) {
		/*bool neg = raw < 0;
		if (neg) {
			int64_t rem1 = (raw + 1) % DAY - 1;
			int64_t rem2 = (raw + 1) % HOUR - 1;
			raw = raw - rem1 + rem2 - (DAY - (int64_t)val * HOUR - HOUR);
		}
		else {
			int64_t rem1 = raw % DAY;
			int64_t rem2 = raw % HOUR;
			raw = raw - rem1 + rem2 + ((int64_t)val * HOUR);
		}*/

		int64_t micr; //Remaining microseconds
		bool neg = raw < 0;
		if (neg) {
			micr = DAY + ((raw + 1) % DAY) - 1;
		}
		else {
			micr = raw % DAY;
		}
		int8_t hours = (int8_t)(micr / HOUR);
		val.convertTo24(); //Converting to 24 hour format
		raw -= ((int64_t)(hours - (int8_t)val)) * HOUR;
	}

	uint8_t getMinutesFromRaw(int64_t raw) {
		int64_t micr; //Remaining microseconds
		bool neg = raw < 0;
		if (neg) {
			raw++;
			micr = HOUR + (raw % HOUR) - 1;
		}
		else {
			micr = raw % HOUR;
		}

		return (uint8_t)(micr / MINUTE);
	}

	void setMinutesToRaw(int64_t& raw, uint8_t val) {
		/*/bool neg = raw < 0;
		if (neg) {
			int64_t rem1 = (raw + 1) % HOUR - 1;
			int64_t rem2 = (raw + 1) % MINUTE - 1;
			raw = raw - rem1 + rem2 - (HOUR - (int64_t)val * MINUTE - MINUTE);
		}
		else {
			int64_t rem1 = raw % HOUR;
			int64_t rem2 = raw % MINUTE;
			raw = raw - rem1 + rem2 + ((int64_t)val * MINUTE);
		}*/

		int64_t micr; //Remaining microseconds
		bool neg = raw < 0;
		if (neg) {
			micr = HOUR + ((raw + 1) % HOUR) - 1;
		}
		else {
			micr = raw % HOUR;
		}
		int8_t min = (int8_t)(micr / MINUTE);
		raw -= ((int64_t)(min - (int8_t)val)) * MINUTE;
	}

	uint8_t getSecondsFromRaw(int64_t raw) {
		int32_t micr; //Remaining microseconds
		bool neg = raw < 0;
		if (neg) {
			raw++;
			micr = MINUTE + (raw % MINUTE) - 1;
		}
		else {
			micr = raw % MINUTE;
		}

		return (uint8_t)(micr / SECOND);
	}

	void setSecondsToRaw(int64_t& raw, uint8_t val) {
		/*bool neg = raw < 0;
		if (neg) {
			int64_t rem1 = (raw + 1) % MINUTE - 1;
			int64_t rem2 = (raw + 1) % SECOND - 1;
			raw = raw - rem1 + rem2 - (MINUTE - (int32_t)val * SECOND - SECOND);
		}
		else {
			int64_t rem1 = raw % MINUTE;
			int64_t rem2 = raw % SECOND;
			raw = raw - rem1 + rem2 + ((int32_t)val * SECOND);
		}*/

		int64_t micr; //Remaining microseconds
		bool neg = raw < 0;
		if (neg) {
			micr = MINUTE + ((raw + 1) % MINUTE) - 1;
		}
		else {
			micr = raw % MINUTE;
		}
		int8_t sec = (int8_t)(micr / SECOND);
		raw -= ((int64_t)(sec - (int8_t)val)) * SECOND;
	}

	uint16_t getMillisFromRaw(int64_t raw) {
		int32_t micr; //Remaining microseconds
		bool neg = raw < 0;
		if (neg) {
			raw++;
			micr = SECOND + (raw % SECOND) - 1;
		}
		else {
			micr = raw % SECOND;
		}

		return (uint16_t)(micr / MILLISECOND);
	}

	void setMillisToRaw(int64_t& raw, uint16_t val) {
		/*bool neg = raw < 0;
		if (neg) {
			int64_t rem1 = (raw + 1) % SECOND - 1;
			int64_t rem2 = (raw + 1) % MILLISECOND - 1;
			raw = raw - rem1 + rem2 - (SECOND - (int32_t)val * MILLISECOND - MILLISECOND);
		}
		else {
			int64_t rem1 = raw % SECOND;
			int64_t rem2 = raw % MILLISECOND;
			raw = raw - rem1 + rem2 + ((int32_t)val * MILLISECOND);
		}*/

		int64_t micr; //Remaining microseconds
		bool neg = raw < 0;
		if (neg) {
			micr = SECOND + ((raw + 1) % SECOND) - 1;
		}
		else {
			micr = raw % SECOND;
		}
		int16_t mils = (int16_t)(micr / MILLISECOND);
		raw -= ((int64_t)(mils - (int16_t)val)) * MILLISECOND;
	}

	uint16_t getMicrosFromRaw(int64_t raw) {
		int32_t micr; //Remaining microseconds
		bool neg = raw < 0;
		if (neg) {
			raw++;
			micr = MILLISECOND + (raw % MILLISECOND) - 1;
		}
		else {
			micr = raw % MILLISECOND;
		}

		return micr / MICROSECOND;
	}

	void setMicrosToRaw(int64_t& raw, uint16_t val) {
		bool neg = raw < 0;
		if (neg) {
			int64_t rem1 = (raw + 1) % MILLISECOND - 1;
			raw = raw - rem1 - (MILLISECOND - val);
		}
		else {
			int64_t rem1 = raw % MILLISECOND;
			raw = raw - rem1 + val;
		}
	}


	int32_t getDaysFromRaw(int64_t raw) {
		int32_t days;
		bool neg = raw < 0;
		if (neg) {
			days = (int32_t)(((raw + 1) / DAY) - 1);
		}
		else {
			days = (int32_t)(raw / DAY);
		}
		return days;
	}

	void setDaysToRaw(int64_t& raw, int32_t val) {
		bool neg = raw < 0;
		int64_t rem1;
		if (neg) {
			rem1 = (raw + 1) % DAY - 1;
			raw = rem1 + ((int64_t)val) * DAY + DAY;
		}
		else {
			rem1 = raw % DAY;
			raw = rem1 + ((int64_t)val) * DAY;
		}
	}

	int64_t getMicrosOfDayFromRaw(int64_t raw) {
		int64_t micr;
		bool neg = raw < 0;
		if (neg) {
			micr = DAY + ((raw + 1) % DAY) - 1;
		}
		else {
			micr = raw % DAY;
		}
		return micr;
	}

	void setMicrosOfDayToRaw(int64_t& raw, int64_t val) {
		int64_t micr;
		bool neg = raw < 0;
		if (neg) {
			micr = DAY + ((raw + 1) % DAY) - 1;
		}
		else {
			micr = raw % DAY;
		}
		raw = raw - micr + val;
	}



	uint8_t getMonthDayFromRaw(int64_t raw) {
		int32_t days = getDaysFromRaw(raw);

		//Date calculation
		year_day_tuple yd = getYearFromDays(days);
		month_day_tuple md = getMonthFromDayOfYear(yd.dayOfYear, isLeapYear(yd.year));
		return md.day;
	}

	void setMonthDayToRaw(int64_t& raw, uint8_t val) {
		//Current values establishment
		int32_t days = getDaysFromRaw(raw);
		year_day_tuple yd = getYearFromDays(days);
		bool isLeap = isLeapYear(yd.year);
		month_day_tuple md = getMonthFromDayOfYear(yd.dayOfYear, isLeap);

		//Maximum day limit
		uint8_t maxDay = md.month.getMonthLength(isLeap);
		if (val > maxDay) val = maxDay;
		else if (val < 1) val = 1;

		//Days difference calculation
		int8_t val_dif = (int8_t)md.day - (int8_t)val;

		//Applying difference
		raw -= (int64_t)val_dif * DAY;
	}

	Month getMonthFromRaw(int64_t raw) {
		int32_t days = getDaysFromRaw(raw);
		year_day_tuple yd = getYearFromDays(days);
		month_day_tuple md = getMonthFromDayOfYear(yd.dayOfYear, isLeapYear(yd.year));
		return md.month;
	}

	void setMonthToRaw(int64_t& raw, Month val) {
		//Limits
		if (val < 1) val = 1;
		else if (val > 12) val = 12;

		//Current values establishment
		int32_t days = getDaysFromRaw(raw);
		year_day_tuple yd = getYearFromDays(days);
		bool isLeap = isLeapYear(yd.year);
		month_day_tuple md = getMonthFromDayOfYear(yd.dayOfYear, isLeap);

		//Maximum day limit
		uint8_t maxDay = val.getMonthLength(isLeap);
		if (md.day > maxDay) md.day = maxDay;

		//Days difference calculation
		int16_t newDayOfYear = getDayOfYearFromMonth(val, isLeap) + md.day - 1; //New day of year, which need to be set
		int16_t oldDayOfYear = yd.dayOfYear; //Old day of year
		int16_t dayDifference = oldDayOfYear - newDayOfYear;

		raw -= (int64_t)dayDifference * DAY;
	}

	//TODO tests
	void addMonthsToRaw(int64_t& raw, int32_t val) {
		int32_t days;
		int64_t micr; //Remaining microseconds
		//calculate days from microseconds
		bool neg = raw < 0;
		if (neg) {
			raw++;
			days = (int32_t)((raw / DAY) - 1);
			micr = DAY + (raw % DAY) - 1;
		}
		else {
			days = (int32_t)(raw / DAY);
			micr = raw % DAY;
		}

		//Date calculation
		year_day_tuple yd = getYearFromDays(days);
		month_day_tuple md = getMonthFromDayOfYear(yd.dayOfYear, isLeapYear(yd.year));
		int32_t newMonthDiff = md.month - 1 + val;
		int32_t yearDiff = newMonthDiff / 12;
		newMonthDiff %= 12;
		if (newMonthDiff < 0) {
			newMonthDiff = 12 + newMonthDiff; //Negative
		}
		else {
			newMonthDiff++;
		}

		int32_t newYearVal = yearDiff + yd.year; //Getting new year
		if (newYearVal <= 0 && yd.year > 0) newYearVal--; //Skip year 0
		else if (newYearVal >= 0 && yd.year < 0) newYearVal++; //Skip year 0

		bool newIsLeap = isLeapYear(newYearVal);
		uint8_t monthLength = Month::getMonthLength(newMonthDiff, newIsLeap);
		if (md.day > monthLength) {
			md.day = monthLength;
		}

		days = (md.day - 1);
		days += getDayOfYearFromMonth(newMonthDiff, newIsLeap);
		days += getDaysUntilYear(newYearVal);

		raw = micr + ((int64_t)days) * DAY;
	}

	void setYearToRaw(int64_t& raw, int32_t val) {
		//Current values establishment
		int32_t days = getDaysFromRaw(raw);
		year_day_tuple yd = getYearFromDays(days);
		bool newIsLeap = isLeapYear(val);
		bool oldIsLeap = isLeapYear(yd.year);

		//Leap day offset, to keep same month and day of month values
		if (newIsLeap != oldIsLeap) {
			if (newIsLeap && yd.dayOfYear >= 59) {
				yd.dayOfYear++;
			}
			else if (oldIsLeap && yd.dayOfYear >= 60) {
				yd.dayOfYear--;
			}
		}

		//Days difference calculation
		int32_t newDays = getDaysUntilYear(val) + yd.dayOfYear;
		int32_t oldDays = days;
		int32_t dayDifference = oldDays - newDays;

		//Applying difference
		raw -= (int64_t)dayDifference * DAY;
	}

	//TODO tests
	void addYearsToRaw(int64_t& raw, int32_t val) {
		//Current values establishment
		int32_t days = getDaysFromRaw(raw);
		year_day_tuple yd = getYearFromDays(days);

		//Calculation of new year value
		int32_t newYearVal = val + yd.year; //Getting new year
		if (newYearVal <= 0 && yd.year > 0) newYearVal--; //Skip year 0
		else if (newYearVal >= 0 && yd.year < 0) newYearVal++; //Skip year 0

		//Leap year check
		bool newIsLeap = isLeapYear(newYearVal);
		bool oldIsLeap = isLeapYear(yd.year);

		//Leap day offset, to keep same month and day of month values
		if (newIsLeap != oldIsLeap) {
			if (newIsLeap && yd.dayOfYear >= 59) {
				yd.dayOfYear++;
			}
			else if (oldIsLeap && yd.dayOfYear >= 60) {
				yd.dayOfYear--;
			}
		}

		//Days difference calculation
		int32_t newDays = getDaysUntilYear(newYearVal) + yd.dayOfYear;
		int32_t oldDays = days;
		int32_t dayDifference = oldDays - newDays;

		//Applying difference
		raw -= (int64_t)dayDifference * DAY;
	}

	uint8_t getWeekOfYearFromRaw(int64_t raw, DayOfWeek firstDayOfWeek) {
		//Current date info
		int32_t days = getDaysFromRaw(raw);
		year_day_tuple yd = getYearFromDays(days);
		bool isLeap = isLeapYear(yd.year);
		month_day_tuple md = getMonthFromDayOfYear(yd.dayOfYear, isLeap);

		//int32_t dayOfYearUntilMonth = getDayOfYearFromMonth(md.month, isLeap);
		int32_t daysUntilFirstDayOfYear = days - yd.dayOfYear;

		uint8_t dayOfWeekOffset = 0; //Day of week offset at first day of month
		if (days < 0) {
			//BC
			dayOfWeekOffset = (7 + ((daysUntilFirstDayOfYear - 4 + (7 - firstDayOfWeek)) % 7));
		}
		else {
			//AD
			dayOfWeekOffset = (((daysUntilFirstDayOfYear + 9 - firstDayOfWeek) % 7) + 1);
		}

		return ((yd.dayOfYear + dayOfWeekOffset - 1) / 7) + 1;
	}

	uint8_t getWeekOfMonthFromRaw(int64_t raw, DayOfWeek firstDayOfWeek) {
		//Current date info
		int32_t days = getDaysFromRaw(raw);
		year_day_tuple yd = getYearFromDays(days);
		bool isLeap = isLeapYear(yd.year);
		month_day_tuple md = getMonthFromDayOfYear(yd.dayOfYear, isLeap);

		int32_t dayOfYearUntilMonth = getDayOfYearFromMonth(md.month, isLeap);
		int32_t daysUntilFirstDayOfMonth = days - (yd.dayOfYear - dayOfYearUntilMonth);

		uint8_t dayOfWeekOffset = 0; //Day of week offset at first day of month
		if (daysUntilFirstDayOfMonth < 0) {
			//BC
			dayOfWeekOffset = (7 + ((daysUntilFirstDayOfMonth - 4 + (7 - firstDayOfWeek)) % 7));
		}
		else {
			//AD
			dayOfWeekOffset = (((daysUntilFirstDayOfMonth + 9 - firstDayOfWeek) % 7) + 1);
		}

		int8_t dayOfMonth = md.day + dayOfWeekOffset - 2; //zero based
		return (dayOfMonth / 7) + 1;
	}

	uint8_t getWeekDayOfMonthFromRaw(int64_t raw) {
		uint8_t md = getMonthDayFromRaw(raw);
		return ((md - 1) / 7) + 1;
	}


	void setDayOfYearToRaw(int64_t& raw, uint32_t val) {
		//Current values establishment
		int32_t days = getDaysFromRaw(raw);
		year_day_tuple yd = getYearFromDays(days);
		bool isLeap = isLeapYear(yd.year);

		//Limits check
		if (isLeap && val > 365) val = 365;
		else if (val > 364) val = 364;

		//Days difference calculation
		int32_t dayDifference = yd.dayOfYear - val;

		//Applying difference
		raw -= (int64_t)dayDifference * DAY;
	}



	int32_t getDaysUntilYear(int32_t year) {
		bool neg = (year < 0); //check if year is BC.
		int32_t year_tmp;
		int32_t days;
		if (neg) {
			//BC
			year_tmp = year + 1;
			days = 0 - 1; //this option is here because year 1 BC is also leap day, so 1 day has to be subtracted
		}
		else {
			//AD
			year--; //skip year 0 because it does not exist
			year_tmp = year;
			days = 0;
		}

		//Adding count of all days except leap days
		days += year * 365;

		//Adding count of leap days
		days += (year_tmp / 400) * 97 + ((year_tmp % 400) / 100) * 24 + (year_tmp % 100) / 4;

		return days;
	}

	year_day_tuple getYearFromDays(int32_t days) {
		bool neg = days < 0;
		if (neg) {
			days += 2; //Subtracting 2 day, because of lack 0 day
		}
		else {
			days += 365; //Adding 1 year, because of lack 0 day
		}

		int32_t years400 = days / DAYS_IN_400_YRS;
		days %= DAYS_IN_400_YRS;
		/*auto div_res = std::ldiv(days, DAYS_IN_400_YRS);
		int32_t years400 = div_res.quot;
		days = div_res.rem;*/

		int32_t years100 = days / DAYS_IN_100_YRS;
		days %= DAYS_IN_100_YRS;
		/*div_res = std::ldiv(days, DAYS_IN_100_YRS);
		int32_t years100 = div_res.quot;
		days = div_res.rem;*/

		int32_t years4 = days / DAYS_IN_4_YRS;
		days %= DAYS_IN_4_YRS;
		/*div_res = std::ldiv(days, DAYS_IN_4_YRS);
		int32_t years4 = div_res.quot;
		days = div_res.rem;*/

		int32_t years1 = days / DAYS_IN_YEAR;
		days %= DAYS_IN_YEAR;
		/*div_res = std::ldiv(days, DAYS_IN_YEAR);
		int32_t years1 = div_res.quot;
		days = div_res.rem;*/

		int32_t year = years400 * 400 + years100 * 100 + years4 * 4 + years1 - neg;
		bool isLeapYear = years1 == 0 && (years4 != 0 || years100 == 0); //True if year is leap year

		uint16_t daysOfYear;
		if (neg) {
			daysOfYear = 364 + days; //Simplified equation
		}
		else {
			daysOfYear = days + isLeapYear; //Adding leap day if current year is leap year
		}

		return year_day_tuple(year, daysOfYear);
	}

	/**
	* @brief Gets month and day of month from day of year. For example 43 => 13th of February.
	* @note This version uses binary search, which is much more faster than linear search using loop.
	* @param day Day of year from range 0-364 and 0-365 for leap year. The 1st of January is day 0.
	* @param leapYear True if year is leap year.
	* @return Returns structure, which contains month from rage 1-12 and day of month from range 1-31/30/28.
	*/
	month_day_tuple getMonthFromDayOfYear(uint16_t day, bool leapYear) {
		//Binary search using if (fastest way than using for or while)
		if (leapYear) {
			//Calculation for leap year
			if (day < 182) {
				//Jan, Feb, Mar, Apr, May, Jun
				if (day < 91) {
					//Jan, Feb, Mar
					if (day < 31) return month_day_tuple(1, day + 1); //January
					else if (day < 60) return month_day_tuple(2, day - 31 + 1); //February
					else return month_day_tuple(3, day - 60 + 1); //March
				}
				else {
					//Apr, May, Jun
					if (day < 121) return month_day_tuple(4, day - 91 + 1); //April
					else if (day < 152) return month_day_tuple(5, day - 121 + 1); //May
					else return month_day_tuple(6, day - 152 + 1); //June
				}
			}
			else {
				//Jul, Aug, Sep, Oct, Nov, Dec
				if (day < 274) {
					//Jul, Aug, Sep
					if (day < 213) return month_day_tuple(7, day - 182 + 1); //July
					else if (day < 244) return month_day_tuple(8, day - 213 + 1); //August
					else return month_day_tuple(9, day - 244 + 1); //September
				}
				else {
					//Oct, Nov, Dec
					if (day < 305) return month_day_tuple(10, day - 274 + 1); //October
					else if (day < 335) return month_day_tuple(11, day - 305 + 1); //November
					else return month_day_tuple(12, day - 335 + 1); //December
				}
			}
		}
		else {
			if (day < 181) {
				//Jan, Feb, Mar, Apr, May, Jun
				if (day < 90) {
					//Jan, Feb, Mar
					if (day < 31) return month_day_tuple(1, day + 1); //January
					else if (day < 59) return month_day_tuple(2, day - 31 + 1); //February
					else return month_day_tuple(3, day - 59 + 1); //March
				}
				else {
					//Apr, May, Jun
					if (day < 120) return month_day_tuple(4, day - 90 + 1); //April
					else if (day < 151) return month_day_tuple(5, day - 120 + 1); //May
					else return month_day_tuple(6, day - 151 + 1); //June
				}
			}
			else {
				//Jul, Aug, Sep, Oct, Nov, Dec
				if (day < 273) {
					//Jul, Aug, Sep
					if (day < 212) return month_day_tuple(7, day - 181 + 1); //July
					else if (day < 243) return month_day_tuple(8, day - 212 + 1); //August
					else return month_day_tuple(9, day - 243 + 1); //September
				}
				else {
					//Oct, Nov, Dec
					if (day < 304) return month_day_tuple(10, day - 273 + 1); //October
					else if (day < 334) return month_day_tuple(11, day - 304 + 1); //November
					else return month_day_tuple(12, day - 334 + 1); //December
				}
			}
		}
	}

	uint16_t getDayOfYearFromMonth(Month month, bool leapYear) {
		static const uint16_t monthDays[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

		month--; //Move to 0 index
		if (month > 11) month = 11; //Value limit - for sure
		if (leapYear && month >= 2) {
			return monthDays[month] + 1;
		}
		else {
			return monthDays[month];
		}
	}

	char* intToStr2(char* buffer, size_t bufferSize, uint32_t val, bool sameWidth) {
		if (bufferSize < 2) return buffer + bufferSize;
		uint8_t digit1 = val % 10;
		val /= 10;
		uint8_t digit2 = val % 10;

		if (digit2 > 0) {
			buffer[0] = digit2 + '0';
			buffer[1] = digit1 + '0';
			return buffer + 2;
		}
		else if (sameWidth) {
			buffer[0] = '0';
			buffer[1] = digit1 + '0';
			return buffer + 2;
		}
		else {
			buffer[0] = digit1 + '0';
			return buffer + 1;
		}
	}

	char* intToStr(char* buffer, size_t bufferSize, uint32_t val, int8_t width) {
		//Digits counting
		int8_t digits = 0;
		uint32_t testVal = val;
		while (testVal) {
			testVal /= 10;
			digits++;
		}
		if (digits == 0) digits = 1; //To show 0
		if (bufferSize < width) width = (int8_t)bufferSize;
		if (bufferSize < digits) digits = (int8_t)bufferSize;

		int8_t i = 0;
		//Fill space before first digit with zeros
		if (width > 0) {
			//Filling is allowed only if width is more than 0
			int8_t preFillCnt = width - digits;
			for (; i < preFillCnt; i++) {
				buffer[i] = '0';
			}
		}
		else {
			width = digits;
		}

		//Writing numbers
		int8_t j = width - 1;
		for (; j >= i; j--) {
			buffer[j] = (val % 10) + '0';
			val /= 10;
		}

		return buffer + width;
	}

	/**
	* @brief Converts time zone or DST to string.
	* @warning Null terminator is not inserted to the end of string.
	* @param buffer Buffer, where string will be written.
	* @param bufferSize Size of buffer without null terminator.
	* @param val Time zone or DST offset in minutes.
	* @param options There are 3 options:
	* + 1 - Prints offset hours without leading zeros. Example: +07:00 => +7
	* + 2 - Prints offset hours with leading zeros. Example: +07:00 => +07
	* + 3 - Prints offset hours and minutes with leading zeros and separator ':'. Example: +07:00 => +07:00
	* + 4 - Prints offset hours and minutes with leading zeros without separator. Example: +07:00 => +0700
	* @return Returns pointer in buffer to new position after last digit.
	*/
	static char* tzToStr(char* buffer, size_t bufferSize, int16_t val, uint8_t options) {
		if (bufferSize < 2) return buffer + bufferSize;

		int8_t ho = val / 60;
		int8_t mins = val % 60;
		if (val < 0) {
			buffer[0] = '-';
			ho = -ho; //Absolute value
			mins = -mins;
		}
		else {
			buffer[0] = '+';
		}
		bufferSize--;
		buffer++;

		char* newBuffer = intToStr2(buffer, bufferSize, ho, options > 1);
		bufferSize -= newBuffer - buffer;
		buffer = newBuffer;

		if (options >= 3) {
			if (options == 3) {
				if (bufferSize < 3) return buffer + bufferSize;
				buffer[0] = ':';
				bufferSize--;
				buffer++;
			}
			else {
				if (bufferSize < 2) return buffer + bufferSize;
			}

			char* newBuffer = intToStr2(buffer, bufferSize, mins, true);
			bufferSize -= newBuffer - buffer;
			buffer = newBuffer;
		}

		return buffer;
	}

	char* ms_usFractToStr(char* buffer, size_t bufferSize, int32_t microseconds, int8_t width, bool strictWidth) {
		if (microseconds < 0) {
			//Negative
			microseconds = -microseconds;
		}
		if (width > bufferSize) width = (int8_t)bufferSize; //Limit

		int8_t cnt = width;
		if (cnt > 6) cnt = 6;

		if (strictWidth) {
			//Filling with zeros
			for (uint8_t i = cnt; i < width; i++) {
				buffer[i] = '0';
			}
		}

		//Moving to right position
		int8_t i = cnt;
		for (; i < 5; i++) {
			microseconds /= 10;
		}
		if (i == 5) {
			//Rounding
			uint8_t lastDigit = microseconds % 10;
			microseconds /= 10;
			if (lastDigit >= 5) {
				microseconds++;
			}
		}

		//Resolving digits
		bool nonZeroFound = false;
		uint8_t digitsCnt = 0;
		cnt--;
		for (; cnt >= 0; cnt--) {
			char digit = (microseconds % 10) + '0';
			microseconds /= 10;

			if (!nonZeroFound) {
				nonZeroFound = digit != '0';
			}

			bool skipDigit = !strictWidth && !nonZeroFound;
			if (!skipDigit || cnt == 0) {
				buffer[cnt] = digit;
				digitsCnt++;
			}
		}

		if (strictWidth) {
			buffer += width;
		}
		else {
			buffer += digitsCnt;
		}
		return buffer;
	}

	char* dateTimeToArray(char* buffer, size_t bufferSize, const char* format, int64_t value, date_s ds, bool isDSResolved, time_s ts, bool isTSResolved, int16_t timeZoneOffset, int16_t DSTOffset, const char* const* monthNames, const char* const* weekDayNames) {
		if (bufferSize == 0) return buffer;
		bufferSize--;

		if (monthNames == NULL) {
			monthNames = dt_month_names;
		}

		if (weekDayNames == NULL) {
			weekDayNames = dt_day_names;
		}

		char lastChar = 0;
		int8_t sameCnt = 1;

		int i = 1;
		bool escape = false;
		bool isText = false;
		bool doubleQ = false;
		lastChar = format[0];
		do {
			char c = format[i];
			if (lastChar == c) {
				//Same character found
				sameCnt++;
			}
			else {
				//New character found
				bool handled;
				if (escape || isText) {
					handled = false;
					bool curentIsDQ = lastChar == '"';
					if (!escape && isText && (lastChar == '\'' || curentIsDQ)) {
						isText = !(curentIsDQ == doubleQ);
						handled = !isText;
					}
					escape = !escape && lastChar == '\\';
					if (escape) handled = escape;
				}
				else {
					handled = true;
					switch (lastChar) {
					case 'd': //day or day of week
						if (!isDSResolved) {
							isDSResolved = true;
							ds = rawToDate(value);
						}
						switch (sameCnt) {
						case 1: {
							char* newBuffer = intToStr2(buffer, bufferSize, ds.day, false);
							bufferSize -= newBuffer - buffer;
							buffer = newBuffer;
							break;
						}
						case 2: {
							char* newBuffer = intToStr2(buffer, bufferSize, ds.day, true);
							bufferSize -= newBuffer - buffer;
							buffer = newBuffer;
							break;
						}
						case 3:
							if (bufferSize >= 3) {
								uint8_t index = ds.dayOfWeek - 1;
								buffer[0] = weekDayNames[index][0];
								buffer[1] = weekDayNames[index][1];
								buffer[2] = weekDayNames[index][2];
								buffer += 3;
								bufferSize -= 3;
							}
							else {
								bufferSize = 0;
							}
							break;
						case 4: {
							size_t len = strlen(weekDayNames[ds.dayOfWeek - 1]);
							if (len > bufferSize) len = bufferSize; //Limit
#if _MSC_VER && !__INTEL_COMPILER
							//Specific code for MVSC
							memcpy_s(buffer, bufferSize, weekDayNames[ds.dayOfWeek - 1], len);
#else
							memcpy(buffer, dt_day_names[ds.dayOfWeek - 1], len);
#endif
							bufferSize -= len;
							buffer += len;
							break;
						}
						}
						break;

					case 'f': //Fraction
					case 'F': {
						if (sameCnt == 1) sameCnt = 0;
						if (!isTSResolved) {
							isTSResolved = true;
							ts = rawToTime(value);
						}
						uint32_t fraction = ((uint32_t)ts.milliseconds) * MILLISECOND + ts.microseconds;
						char* newBuffer = ms_usFractToStr(buffer, bufferSize, fraction, sameCnt, lastChar == 'f');
						bufferSize -= newBuffer - buffer;
						buffer = newBuffer;
						break;
					}

					case 'G':
					case 'g': // B.C. or A.C.
						if (lastChar == 'g' || value < 0) {
							if (bufferSize < 4) {
								bufferSize = 0;
							}
							else {
								if (value < 0) {
									buffer[0] = 'B';
									buffer[1] = '.';
									buffer[2] = 'C';
									buffer[3] = '.';
								}
								else {
									buffer[0] = 'A';
									buffer[1] = '.';
									buffer[2] = 'D';
									buffer[3] = '.';
								}
								bufferSize -= 4;
								buffer += 4;
							}
						}
						break;

					case 'N':
					case 'n': // B.C. or A.C. represented by sign
						if (lastChar == 'n' || value < 0) {
							if (bufferSize < 1) {
								bufferSize = 0;
							}
							else {
								buffer[0] = (value < 0) ? '-' : '+';
								bufferSize--;
								buffer++;
							}
						}
						break;
					case 'z': {//Time zone offset
						char* newBuffer = tzToStr(buffer, bufferSize, timeZoneOffset, sameCnt);
						bufferSize -= newBuffer - buffer;
						buffer = newBuffer;
						break;
					}
					case 'Z': {//Time zone and DST offset
						char* newBuffer = tzToStr(buffer, bufferSize, timeZoneOffset + DSTOffset, sameCnt);
						bufferSize -= newBuffer - buffer;
						buffer = newBuffer;
						break;
					}
					case 'l': {//DTS offset
						char* newBuffer = tzToStr(buffer, bufferSize, DSTOffset, sameCnt);
						bufferSize -= newBuffer - buffer;
						buffer = newBuffer;
						break;
					}
					case 'h': {	//12-hour format
						if (!isTSResolved) {
							isTSResolved = true;
							ts = rawToTime(value);
						}
						hour_t hours = ts.hours;
						hours.convertTo12();
						char* newBuffer = intToStr2(buffer, bufferSize, hours, sameCnt > 1);
						bufferSize -= newBuffer - buffer;
						buffer = newBuffer;
						break;
					}
					case 'H': {	//24-hour format
						if (!isTSResolved) {
							isTSResolved = true;
							ts = rawToTime(value);
						}
						hour_t hours = ts.hours;
						char* newBuffer = intToStr2(buffer, bufferSize, hours, sameCnt > 1);
						bufferSize -= newBuffer - buffer;
						buffer = newBuffer;
						break;
					}
					case 'm': {	//minutes
						if (!isTSResolved) {
							isTSResolved = true;
							ts = rawToTime(value);
						}
						char* newBuffer = intToStr2(buffer, bufferSize, ts.minutes, sameCnt > 1);
						bufferSize -= newBuffer - buffer;
						buffer = newBuffer;
						break;
					}
					case 'M':	//month
						if (!isDSResolved) {
							isDSResolved = true;
							ds = rawToDate(value);
						}
						switch (sameCnt) {
						case 1: {
							char* newBuffer = intToStr2(buffer, bufferSize, ds.month, false);
							bufferSize -= newBuffer - buffer;
							buffer = newBuffer;
							break;
						}
						case 2: {
							char* newBuffer = intToStr2(buffer, bufferSize, ds.month, true);
							bufferSize -= newBuffer - buffer;
							buffer = newBuffer;
							break;
						}
						case 3:
							if (bufferSize >= 3) {
								uint8_t index = ds.month - 1;
								buffer[0] = monthNames[index][0];
								buffer[1] = monthNames[index][1];
								buffer[2] = monthNames[index][2];
								buffer -= 3;
								bufferSize -= 3;
							}
							else {
								bufferSize = 0;
							}
							break;
						case 4: {
							size_t len = strlen(monthNames[ds.month - 1]);
							if (len > bufferSize) len = bufferSize; //Limit
#if _MSC_VER && !__INTEL_COMPILER
							//Specific code for MVSC
							memcpy_s(buffer, bufferSize, monthNames[ds.month - 1], len);
#else
							memcpy(buffer, dt_day_names[ds.dayOfWeek - 1], len);
#endif
							bufferSize -= len;
							buffer += len;
							break;
						}
						}
						break;

					case 's': {	//seconds
						if (!isTSResolved) {
							isTSResolved = true;
							ts = rawToTime(value);
						}
						char* newBuffer = intToStr2(buffer, bufferSize, ts.seconds, sameCnt > 1);
						bufferSize -= newBuffer - buffer;
						buffer = newBuffer;
						break;
					}
					case 'i': {	//milliseconds
						if (sameCnt == 1) sameCnt = 0;
						if (!isTSResolved) {
							isTSResolved = true;
							ts = rawToTime(value);
						}
						char* newBuffer = intToStr(buffer, bufferSize, ts.milliseconds, sameCnt);
						bufferSize -= newBuffer - buffer;
						buffer = newBuffer;
						break;
					}
					case 'u': {	//microseconds
						if (sameCnt == 1) sameCnt = 0;
						if (!isTSResolved) {
							isTSResolved = true;
							ts = rawToTime(value);
						}
						char* newBuffer = intToStr(buffer, bufferSize, ts.microseconds, sameCnt);
						bufferSize -= newBuffer - buffer;
						buffer = newBuffer;
						break;
					}
					case 't':	//AM/PM
						if (!isTSResolved) {
							isTSResolved = true;
							ts = rawToTime(value);
						}
						if (sameCnt == 1) {
							if (bufferSize < 1) {
								bufferSize = 0;
							}
							buffer[0] = (ts.hours.isAM()) ? 'A' : 'P';
							buffer++;
							bufferSize--;
						}
						else {
							if (bufferSize < 2) {
								bufferSize = 0;
							}
							buffer[0] = (ts.hours.isAM()) ? 'A' : 'P';
							buffer[1] = 'M';
							buffer += 2;
							bufferSize -= 2;
						}
						break;

					case 'y': {	//year
						if (!isDSResolved) {
							isDSResolved = true;
							ds = rawToDate(value);
						}
						uint32_t year;
						if (ds.year < 0) year = -ds.year; //Absolute value of year
						else year = ds.year;

						if (sameCnt <= 2) {
							char* newBuffer = intToStr2(buffer, bufferSize, year, sameCnt > 1);
							bufferSize -= newBuffer - buffer;
							buffer = newBuffer;
						}
						else {
							char* newBuffer = intToStr(buffer, bufferSize, year, sameCnt);
							bufferSize -= newBuffer - buffer;
							buffer = newBuffer;
						}
						break;
					}

					case '\\':
						escape = true;
						break;

					case '\'': //Text mark
						isText = !(isText && !doubleQ);
						doubleQ = false;
						break;

					case '"': //Text mark
						isText = !(isText && !doubleQ);
						doubleQ = true;
						break;

					default:
						handled = false;
						break;
					}
				}

				if (!handled) {
					//Text writing
					if (bufferSize < sameCnt) sameCnt = (int8_t)bufferSize;
					for (; sameCnt > 0; sameCnt--) {
						buffer[0] = lastChar;
						bufferSize--;
						buffer++;
					}
				}

				sameCnt = 1;
				lastChar = c;
			}
			i++;
		} while (lastChar != '\0' && bufferSize > 0);

		buffer[0] = '\0';

		return buffer; //Returns position of the null terminator
	}

	char* dateTimeToArray(char* buffer, size_t bufferSize, const char* format, int64_t value, int16_t timeZoneOffset, int16_t DSTOffset, const char* const* monthNames, const char* const* weekDayNames) {
		return dateTimeToArray(buffer, bufferSize, format, value, date_s(), false, time_s(), false, timeZoneOffset, DSTOffset, monthNames, weekDayNames);
	}

	char* dateTimeToArray(char* buffer, size_t bufferSize, const char* format, date_s date, time_s time, int16_t timeZoneOffset, int16_t DSTOffset, const char* const* monthNames, const char* const* weekDayNames) {
		return dateTimeToArray(buffer, bufferSize, format, 0, date, true, time, true, timeZoneOffset, DSTOffset, monthNames, weekDayNames);
	}


#ifdef ARDUINO
	String dateTimeToString(const char* format, int64_t value, date_s ds, bool isDSResolved, time_s ts, bool isTSResolved, int16_t timeZoneOffset, int16_t DSTOffset, const char* const* monthNames, const char* const* weekDayNames) {
#else
	std::string dateTimeToString(const char* format, int64_t value, date_s ds, bool isDSResolved, time_s ts, bool isTSResolved, int16_t timeZoneOffset, int16_t DSTOffset, const char* const* monthNames, const char* const* weekDayNames) {
#endif // ARDUINO
		if (monthNames == NULL) {
			monthNames = dt_month_names;
		}

		if (weekDayNames == NULL) {
			weekDayNames = dt_day_names;
		}

#ifdef ARDUINO
		String ret;
#else
		std::string ret;
#endif // ARDUINO
		ret.reserve(strlen(format) + 15); //Try to predict result size

		char buffer[10]; //Helper buffer

		char lastChar = 0;
		int8_t sameCnt = 1;

		int i = 1;
		bool escape = false;
		bool isText = false;
		bool doubleQ = false;
		lastChar = format[0];
		do {
			char c = format[i];
			if (lastChar == c) {
				//Same character found
				sameCnt++;
			}
			else {
				//New character found
				bool handled;
				if (escape || isText) {
					handled = false;
					bool curentIsDQ = lastChar == '"';
					if (!escape && isText && (lastChar == '\'' || curentIsDQ)) {
						isText = !(curentIsDQ == doubleQ);
						handled = !isText;
					}
					escape = !escape && lastChar == '\\';
					if (escape) handled = escape;
				}
				else {
					handled = true;
					switch (lastChar) {
					case 'd': //day or day of week
						if (!isDSResolved) {
							isDSResolved = true;
							ds = rawToDate(value);
						}
						switch (sameCnt) {
						case 1: {
							char* newBuffer = intToStr2(buffer, sizeof(buffer) - 1, ds.day, false);
							newBuffer[0] = 0; //Null terminator
							ret += buffer;
							break;
						}
						case 2: {
							char* newBuffer = intToStr2(buffer, sizeof(buffer) - 1, ds.day, true);
							newBuffer[0] = 0; //Null terminator
							ret += buffer;
							break;
						}
						case 3: {
							uint8_t index = ds.dayOfWeek - 1;
							buffer[0] = weekDayNames[index][0];
							buffer[1] = weekDayNames[index][1];
							buffer[2] = weekDayNames[index][2];
							buffer[3] = 0; //Null terminator
							ret += buffer;
							break;
						}
						case 4: {
							ret += weekDayNames[ds.dayOfWeek - 1];
							break;
						}
						}
						break;

					case 'f': //Fraction
					case 'F': {
						if (sameCnt == 1) sameCnt = 0;
						else if (sameCnt > sizeof(buffer) - 1) sameCnt = sizeof(buffer) - 1;

						if (!isTSResolved) {
							isTSResolved = true;
							ts = rawToTime(value);
						}

						uint32_t fraction = ((uint32_t)ts.milliseconds) * MILLISECOND + ts.microseconds;
						char* newBuffer = ms_usFractToStr(buffer, sizeof(buffer) - 1, fraction, sameCnt, lastChar == 'f');
						newBuffer[0] = 0; //Null terminator
						ret += buffer;
						break;
					}

					case 'G':
					case 'g': // B.C. or A.C.
						if (lastChar == 'g' || value < 0) {
							if (value < 0) {
								ret += "B.C.";
							}
							else {
								ret += "A.D.";
							}
						}
						break;

					case 'N':
					case 'n': // B.C. or A.C. represented by sign
						if (lastChar == 'n' || value < 0) {
							ret += (value < 0) ? '-' : '+';
						}
						break;
					case 'z': {//Time zone offset
						char* newBuffer = tzToStr(buffer, sizeof(buffer) - 1, timeZoneOffset, sameCnt);
						newBuffer[0] = 0; //Null terminator
						ret += buffer;
						break;
					}
					case 'Z': {//Time zone and DST offset
						char* newBuffer = tzToStr(buffer, sizeof(buffer) - 1, timeZoneOffset + DSTOffset, sameCnt);
						newBuffer[0] = 0; //Null terminator
						ret += buffer;
						break;
					}
					case 'l': {//DTS offset
						char* newBuffer = tzToStr(buffer, sizeof(buffer) - 1, DSTOffset, sameCnt);
						newBuffer[0] = 0; //Null terminator
						ret += buffer;
						break;
					}
					case 'h': {	//12-hour format
						if (!isTSResolved) {
							isTSResolved = true;
							ts = rawToTime(value);
						}
						hour_t hours = ts.hours;
						hours.convertTo12();
						char* newBuffer = intToStr2(buffer, sizeof(buffer) - 1, hours, sameCnt > 1);
						newBuffer[0] = 0; //Null terminator
						ret += buffer;
						break;
					}
					case 'H': {	//24-hour format
						if (!isTSResolved) {
							isTSResolved = true;
							ts = rawToTime(value);
						}
						hour_t hours = ts.hours;
						char* newBuffer = intToStr2(buffer, sizeof(buffer) - 1, hours, sameCnt > 1);
						newBuffer[0] = 0; //Null terminator
						ret += buffer;
						break;
					}
					case 'm': {	//minutes
						if (!isTSResolved) {
							isTSResolved = true;
							ts = rawToTime(value);
						}
						char* newBuffer = intToStr2(buffer, sizeof(buffer) - 1, ts.minutes, sameCnt > 1);
						newBuffer[0] = 0; //Null terminator
						ret += buffer;
						break;
					}
					case 'M':	//month
						if (!isDSResolved) {
							isDSResolved = true;
							ds = rawToDate(value);
						}
						switch (sameCnt) {
						case 1: {
							char* newBuffer = intToStr2(buffer, sizeof(buffer) - 1, ds.month, false);
							newBuffer[0] = 0; //Null terminator
							ret += buffer;
							break;
						}
						case 2: {
							char* newBuffer = intToStr2(buffer, sizeof(buffer) - 1, ds.month, true);
							newBuffer[0] = 0; //Null terminator
							ret += buffer;
							break;
						}
						case 3:
						{
							uint8_t index = ds.month - 1;
							buffer[0] = monthNames[index][0];
							buffer[1] = monthNames[index][1];
							buffer[2] = monthNames[index][2];
							buffer[3] = 0; //Null terminator
							ret += buffer;
						}
						break;
						case 4: {
							ret += monthNames[ds.month - 1];
							break;
						}
						}
						break;

					case 's': {	//seconds
						if (!isTSResolved) {
							isTSResolved = true;
							ts = rawToTime(value);
						}
						char* newBuffer = intToStr2(buffer, sizeof(buffer) - 1, ts.seconds, sameCnt > 1);
						newBuffer[0] = 0; //Null terminator
						ret += buffer;
						break;
					}
					case 'i': {	//milliseconds
						if (sameCnt == 1) sameCnt = 0;
						if (!isTSResolved) {
							isTSResolved = true;
							ts = rawToTime(value);
						}
						char* newBuffer = intToStr(buffer, sizeof(buffer) - 1, ts.milliseconds, sameCnt);
						newBuffer[0] = 0; //Null terminator
						ret += buffer;
						break;
					}
					case 'u': {	//microseconds
						if (sameCnt == 1) sameCnt = 0;
						if (!isTSResolved) {
							isTSResolved = true;
							ts = rawToTime(value);
						}
						char* newBuffer = intToStr(buffer, sizeof(buffer) - 1, ts.microseconds, sameCnt);
						newBuffer[0] = 0; //Null terminator
						ret += buffer;
						break;
					}
					case 't':	//AM/PM
						if (!isTSResolved) {
							isTSResolved = true;
							ts = rawToTime(value);
						}
						if (sameCnt == 1) {
							ret += (ts.hours.isAM()) ? 'A' : 'P';
						}
						else {
							if (ts.hours.isAM()) {
								ret += "AM";
							}
							else {
								ret += "PM";
							}
						}
						break;

					case 'y': {	//year
						if (!isDSResolved) {
							isDSResolved = true;
							ds = rawToDate(value);
						}
						uint32_t year;
						if (ds.year < 0) year = -ds.year; //Absolute value of year
						else year = ds.year;

						if (sameCnt > sizeof(buffer) - 1) sameCnt = sizeof(buffer) - 1;

						if (sameCnt <= 2) {
							char* newBuffer = intToStr2(buffer, sizeof(buffer) - 1, year, sameCnt > 1);
							newBuffer[0] = 0; //Null terminator
							ret += buffer;
						}
						else {
							char* newBuffer = intToStr(buffer, sizeof(buffer) - 1, year, sameCnt);
							newBuffer[0] = 0; //Null terminator
							ret += buffer;
						}
						break;
					}

					case '\\':
						escape = true;
						break;

					case '\'': //Text mark
						isText = !(isText && !doubleQ);
						doubleQ = false;
						break;

					case '"': //Text mark
						isText = !(isText && !doubleQ);
						doubleQ = true;
						break;

					default:
						handled = false;
						break;
					}
				}

				if (!handled) {
					//Text writing
					for (; sameCnt > 0; sameCnt--) {
						ret += lastChar;
					}
				}

				sameCnt = 1;
				lastChar = c;
			}
			i++;
		} while (lastChar != '\0');

#ifdef ARDUINO
		ret.reserve(ret.length() + 1); //Shring to fit by arduino
#else
		ret.shrink_to_fit();
#endif // ARDUINO

		return ret; //Returns position of the null terminator
	}

#ifdef ARDUINO
	String dateTimeToString(const char* format, int64_t value, int16_t timeZoneOffset, int16_t DSTOffset, const char* const* monthNames, const char* const* weekDayNames) {
#else
	std::string dateTimeToString(const char* format, int64_t value, int16_t timeZoneOffset, int16_t DSTOffset, const char* const* monthNames, const char* const* weekDayNames) {
#endif // ARDUINO
		return dateTimeToString(format, value, date_s(), false, time_s(), false, timeZoneOffset, DSTOffset, monthNames, weekDayNames);
	}

#ifdef ARDUINO
	String dateTimeToString(const char* format, date_s date, time_s time, int16_t timeZoneOffset, int16_t DSTOffset, const char* const* monthNames, const char* const* weekDayNames) {
#else
	std::string dateTimeToString(const char* format, date_s date, time_s time, int16_t timeZoneOffset, int16_t DSTOffset, const char* const* monthNames, const char* const* weekDayNames) {
#endif // ARDUINO
		return dateTimeToString(format, 0, date, true, time, true, timeZoneOffset, DSTOffset, monthNames, weekDayNames);
	}

	/**
	* @brief Parses unsigned number from string.
	* @warning Null terminator is not inserted to the end of string.
	* @param buffer Buffer, where string with number is located.
	* @param bufferSize Size of buffer without null terminator.
	* @param parsedValue Parsed unsigned integer value. If parsing failed, negative value is returned.
	* @param expectedDigits Expected count of digits. If parameter exactDigCnt is true, exact digits count has to be found
	* in string, else parsedValue will be false. If parameter exactDigCnt is false, parameter expectedDigits is maximum count of digits.
	* @param exactDigCnt True, to fill with zeros until width parameter.
	* @return Returns pointer in buffer to new position after last parsed digit.
	*/
	static const char* parseNumber(const char* buffer, size_t bufferSize, int& parsedValue, uint8_t expectedDigits, bool exactDigCnt) {
		if (bufferSize <= 0 || expectedDigits <= 0) {
			parsedValue = -1;
			return buffer;
		}

		parsedValue = 0;
		bool firstFound = false;
		while (expectedDigits > 0 && bufferSize > 0) {
			char c = *buffer;
			if (c >= '0' && c <= '9') { //Number found
				parsedValue *= 10;
				parsedValue += c - '0';
				firstFound = true;
			}
			else { //Non number character found
				if (exactDigCnt) {
					//expectedDigits is exact count of digits
					if (expectedDigits > 0) {
						parsedValue = -1; //Parsing error, unexpected character
						return buffer;
					}
				}
				else {
					//expectedDigits is maximum count of digits
					if (!firstFound) parsedValue = -1;
					return buffer;
				}
			}
			buffer++;
			bufferSize--;
			expectedDigits--;
		}
		if (!firstFound || (exactDigCnt && expectedDigits > 0)) {
			parsedValue = -1; //Parsing error, unexpected character
		}
		return buffer;
	}

	/**
	* @brief Converts char to lower variant
	*/
	static char toLowerChar(char c) {
		if (c >= 'A' && c <= 'Z') return c + 32;
		else return c;
	}

	int parseDateTime(const char* buffer, int bufferSize, const char* format, date_time_s & parsedValue, int16_t & timeZoneOffset, int16_t & DSTOffset, int16_t & TZandDSToffset, bool matchText, const char* const* monthNames) {
		if (bufferSize == 0) return false;
		//bufferSize--;

		if (monthNames == NULL) {
			monthNames = dt_month_names;
		}

		parsedValue = date_time_s();
		timeZoneOffset = 0;
		DSTOffset = 0;
		TZandDSToffset = 0;

		//char* buffer = buffer_;

		int oldBufferSize = bufferSize;

		bool negativeYear = false;
		bool isAM = false;

		char lastChar = 0;
		int8_t sameCnt = 1;

		int i = 1;
		bool escape = false;
		bool isText = false;
		bool doubleQ = false;
		lastChar = format[0];
		do {
			char c = format[i];
			if (lastChar == c) {
				//Same character found
				sameCnt++;
			}
			else {
				//New character found
				bool handled;
				if (escape || isText) {
					handled = false;
					bool curentIsDQ = lastChar == '"';
					if (!escape && isText && (lastChar == '\'' || curentIsDQ)) {
						isText = !(curentIsDQ == doubleQ);
						handled = !isText;
					}
					escape = !escape && lastChar == '\\';
					if (escape) handled = escape;
				}
				else {
					handled = true;
					switch (lastChar) {
					case 'f': //Fraction
					case 'F': {
						int parsedVal = 0;
						int digitsFound = 0;
						int maxDigits = (lastChar == 'f' && sameCnt < 6) ? sameCnt : 6;
						while (bufferSize > 0) {
							char c = *buffer;
							if (c >= '0' && c <= '9') { //Number found
								if (digitsFound < maxDigits) { //Captre only firts 6 digits
									parsedVal *= 10;
									parsedVal += c - '0';
								}
								digitsFound++;
							}
							else { //Non number character found
								if (lastChar == 'f' && digitsFound != sameCnt) {
									return bufferSize - oldBufferSize; //Parsing error
								}
								else {
									break;
								}
							}
							buffer++;
							bufferSize--;
						}

						for (uint8_t i = digitsFound; i < 6; i++) {
							parsedVal *= 10;
						}

						if (bufferSize <= 0 && lastChar == 'f' && digitsFound != sameCnt) {
							return bufferSize - oldBufferSize; //Parsing error
						}

						parsedValue.milliseconds = parsedVal / 1000;
						parsedValue.microseconds = parsedVal % 1000;

						break;
					}

					case 'G':
					case 'g': // B.C. or A.C.
						//B.C. or A.D. expected
						if (*buffer == 'A' || *buffer == 'a') {
							if (bufferSize >= 4 && buffer[1] == '.' && (buffer[2] == 'D' || buffer[2] == 'd') && buffer[3] == '.') {
								//A.D. or a.d. found
								negativeYear = false;
								bufferSize -= 4;
								buffer += 4;
							}
							else {
								return bufferSize - oldBufferSize; //Parsing error
							}
						}
						else if (*buffer == 'B' || *buffer == 'b') {
							if (bufferSize >= 4 && buffer[1] == '.' && (buffer[2] == 'C' || buffer[2] == 'c') && buffer[3] == '.') {
								//A.D. or a.d. found
								negativeYear = true;
								bufferSize -= 4;
								buffer += 4;
							}
							else {
								return bufferSize - oldBufferSize; //Parsing error
							}
						}
						else {
							if (lastChar == 'g') {
								return bufferSize - oldBufferSize; //Parsing error
							}
							else {
								negativeYear = false;
							}
						}
						break;

					case 'N':
					case 'n': // B.C. or A.C. represented by sign
						if (lastChar == 'n') {
							//Plus or minus char excpted
							if (*buffer == '+' || *buffer == '-') {
								negativeYear = *buffer == '-';
								bufferSize--;
								buffer++;
							}
							else {
								return bufferSize - oldBufferSize; //Parsing error
							}
						}
						else {
							//Minus or any other char excpted
							if (*buffer == '+' || *buffer == '-') {
								negativeYear = *buffer == '-';
								bufferSize--;
								buffer++;
							}
							else {
								negativeYear = false;
							}
						}
						break;
					case 'l':  //DTS offset
					case 'Z':  //Time zone and DST offset
					case 'z': {//Time zone offset
						int parsedVal1;
						int parsedVal2 = 0;
						const char* newBuffer;

						bool minusFound = buffer[0] == '-';
						if (minusFound || buffer[0] == '+') {
							bufferSize--;
							buffer++;
							if (bufferSize < 1) {
								return bufferSize - oldBufferSize; //Parsing error
							}
						}

						int offsVal;
						if (buffer[0] == 'z' || buffer[0] == 'Z') { //UTC time
							offsVal = 0;
							bufferSize--;
							buffer++;
						}
						else {
							if (sameCnt == 1 || sameCnt == 2) {
								newBuffer = parseNumber(buffer, bufferSize, parsedVal1, 2, sameCnt == 2);
								if (parsedVal1 < 0 || parsedVal1 > 14) {
									return bufferSize - oldBufferSize; //Parsing error
								}
							}
							else {
								newBuffer = parseNumber(buffer, bufferSize, parsedVal1, 2, false);
								if (parsedVal1 < 0 || parsedVal1 > 14) {
									return bufferSize - oldBufferSize; //Parsing error
								}
								bufferSize -= (int)(newBuffer - buffer);
								buffer = newBuffer;
								if (sameCnt == 3) {
									if (*buffer != ':') {
										return bufferSize - oldBufferSize; //Parsing error
									}
									bufferSize--;
									buffer++;
								}
								newBuffer = parseNumber(buffer, bufferSize, parsedVal2, 2, false);
								if (parsedVal2 < 0 || parsedVal1 > 59) {
									return bufferSize - oldBufferSize; //Parsing error
								}
							}
							offsVal = parsedVal1 * 60 + parsedVal2;
							if (minusFound) offsVal = -offsVal;

							bufferSize -= (int)(newBuffer - buffer);
							buffer = newBuffer;
						}

						switch (lastChar) {
						case 'z': timeZoneOffset = offsVal; break;//Time zone offset
						case 'Z': TZandDSToffset = offsVal; break;//Time zone and DST offset
						case 'l': DSTOffset = offsVal; break;//DTS offset
						}
						break;
					}
					case 'd':   //day of month
					case 'u':   //microseconds
					case 'i':   //milliseconds
					case 's':   //seconds
					case 'm':   //minutes
					case 'H':   //24-hour format
					case 'h': {	//12-hour format
						int parsedVal;
						const char* newBuffer;
						bool ms_or_us = lastChar == 'u' || lastChar == 'i';
						if (sameCnt == 1) {
							//Unknown count of digits
							newBuffer = parseNumber(buffer, bufferSize, parsedVal, ms_or_us ? 3 : 2, false);
						}
						else {
							//Exact excepted count of digits
							newBuffer = parseNumber(buffer, bufferSize, parsedVal, sameCnt, true);
						}
						bufferSize -= (int)(newBuffer - buffer);
						if (parsedVal < 0 || (ms_or_us && parsedVal > 999) || (lastChar == 'H' && parsedVal > 23) || (lastChar == 'h' && (parsedVal > 12 || parsedVal < 1)) || (lastChar == 'd' && (parsedVal > 31 || parsedVal < 1)) || parsedVal > 60) {
							return bufferSize - oldBufferSize; //Parsing error
						}
						buffer = newBuffer;

						switch (lastChar) {
						case 'd': parsedValue.day = parsedVal; break; //day of month
						case 'u': parsedValue.microseconds = parsedVal; break; //microseconds
						case 'i': parsedValue.milliseconds = parsedVal; break; //milliseconds
						case 's': parsedValue.seconds = parsedVal; break; //seconds
						case 'm': parsedValue.minutes = parsedVal; break; //minutes
						case 'H': parsedValue.hours.setHours24(parsedVal); break; //24-hour format
						case 'h': parsedValue.hours.setHours12(parsedVal, false); break; //12-hour format
						}
						break;
					}
					case 'M': //month
						if (sameCnt == 1 || sameCnt == 2) {
							int parsedVal;
							const char* newBuffer;
							if (sameCnt == 1) {
								//Unknown count of digits
								newBuffer = parseNumber(buffer, bufferSize, parsedVal, 2, false);
							}
							else {
								//Exact excepted count of digits
								newBuffer = parseNumber(buffer, bufferSize, parsedVal, sameCnt, true);
							}
							bufferSize -= (int)(newBuffer - buffer);
							if (parsedVal < 0) {
								return bufferSize - oldBufferSize - 1; //Parsing error
							}
							else if (parsedVal < 1 || parsedVal > 12) {
								return bufferSize - oldBufferSize; //Parsing error
							}
							buffer = newBuffer;
							parsedValue.month = parsedVal;
						}
						else if (sameCnt == 3) {
							if (bufferSize < 3) {
								return bufferSize - oldBufferSize; //Parsing error
							}
							bool found = false;
							for (uint8_t i = 0; i < 12; i++) {
								bool match = toLowerChar(monthNames[i][0]) == toLowerChar(buffer[0]) && toLowerChar(monthNames[i][1]) == toLowerChar(buffer[1]) && toLowerChar(monthNames[i][2]) == toLowerChar(buffer[2]);
								if (match) {
									found = true;
									parsedValue.month = i + 1;
									break;
								}
							}
							if (!found) {
								return bufferSize - oldBufferSize; //Parsing error
							}
							bufferSize -= 3;
							buffer += 3;
						}
						else if (sameCnt == 4) {
							bool found = false;
							for (uint8_t i = 0; i < 12; i++) {
								uint8_t j = 0;
								for (; monthNames[i][j] != 0 && bufferSize > j && toLowerChar(monthNames[i][j]) == toLowerChar(buffer[j]); j++);
								bool match = monthNames[i][j] == 0;
								if (match) {
									found = true;
									parsedValue.month = i + 1;
									bufferSize -= j;
									buffer += j;
									break;
								}
							}
							if (!found) {
								return bufferSize - oldBufferSize; //Parsing error
							}
						}
						break;
					case 't': {//AM/PM
						bool aFound = buffer[0] == 'A' || buffer[0] == 'a';
						bool apFound = aFound || buffer[0] == 'P' || buffer[0] == 'p';
						if (!apFound || bufferSize < 1) {
							return bufferSize - oldBufferSize; //Parsing error
						}
						if (sameCnt == 2) {
							if (bufferSize >= 2 && (buffer[1] == 'M' || buffer[1] == 'm')) {
								isAM = aFound;
							}
							else {
								return bufferSize - oldBufferSize; //Parsing error
							}
							buffer += 2;
							bufferSize -= 2;
						}
						else {
							isAM = aFound;
							buffer++;
							bufferSize--;
						}
					}
							break;

					case 'y': {	//year
						int parsedVal;
						const char* newBuffer;
						if (sameCnt == 1) {
							//Unknown count of digits
							newBuffer = parseNumber(buffer, bufferSize, parsedVal, 6, false);
						}
						else {
							//Exact excepted count of digits
							newBuffer = parseNumber(buffer, bufferSize, parsedVal, sameCnt, false);
						}
						bufferSize -= (int)(newBuffer - buffer);
						if (parsedVal < 0) {
							return bufferSize - oldBufferSize; //Parsing error
						}
						else if (sameCnt == 2 || sameCnt == 3) {
							//Two or three digit year notation
							parsedVal += 2000;
						}
						buffer = newBuffer;
						parsedValue.year = parsedVal;
						break;
					}

					case '\\':
						escape = true;
						break;

					case '\'': //Text mark
						isText = !(isText && !doubleQ);
						doubleQ = false;
						break;

					case '"': //Text mark
						isText = !(isText && !doubleQ);
						doubleQ = true;
						break;

					default:
						handled = false;
						break;
					}
				}

				if (!handled) {
					//Skipping text characters
					if (bufferSize < sameCnt) {
						return bufferSize - oldBufferSize; //Parsing failed
					}
					for (; sameCnt > 0; sameCnt--) {
						if (*buffer == '\0' || (matchText && *buffer != lastChar)) {
							return bufferSize - oldBufferSize; //Parsing failed
						}
						buffer++;
						bufferSize--;
					}
				}

				sameCnt = 1;
				lastChar = c;
				if (*buffer == '\0' && c != 'G' && c != 'N') {
					break; //End of buffer reached
				}
			}
			i++;
		} while (lastChar != '\0' && bufferSize > 0);

		if (negativeYear) parsedValue.year = -parsedValue.year;
		if (parsedValue.hours.is12()) {
			parsedValue.hours.setHours12(parsedValue.hours.getHours(), isAM);
		}

		if (lastChar == '\0') {
			return oldBufferSize - bufferSize; //Returns positive position if not failed
		}
		else {
			return bufferSize - oldBufferSize; //Returns negative position if failed
		}
	}
}

