#include "TimeSpan.h"

TimeSpan::TimeSpan(int32_t days, int32_t hours, int32_t minutes, int32_t seconds, int32_t milliseconds, int32_t microseconds) {
	set(days, hours, minutes, seconds, milliseconds, microseconds);
}

//Days can be negative, other fields has to be always positive
void TimeSpan::set(int32_t days, int32_t hours, int32_t minutes, int32_t seconds, int32_t milliseconds, int32_t microseconds) {
	raw_time = (int64_t)days * DAY;
	raw_time += (int64_t)hours * HOUR;
	raw_time += (int64_t)minutes * MINUTE;
	raw_time += (int64_t)seconds * SECOND;
	raw_time += (int64_t)milliseconds * MILLISECOND;
	raw_time += (int64_t)microseconds * MICROSECOND;
}

void TimeSpan::set(time_span_s ts) {
	raw_time = (int64_t)ts.days * DAY;
	raw_time += (int64_t)ts.hours * HOUR;
	raw_time += (int64_t)ts.minutes * MINUTE;
	raw_time += (int64_t)ts.seconds * SECOND;
	raw_time += (int64_t)ts.milliseconds * MILLISECOND;
	raw_time += (int64_t)ts.microseconds * MICROSECOND;
}

time_span_s TimeSpan::getTimeSpanStruct() const {
	time_span_s ret;
	int64_t val = raw_time;
	ret.days = (int32_t)(val / DAY);
	val = val % DAY;
	ret.hours = (int8_t)(val / HOUR);
	val = val % HOUR;
	ret.minutes = (int8_t)(val / MINUTE);
	val = val % MINUTE;
	ret.seconds = (int8_t)(val / SECOND);
	val = val % SECOND;
	ret.milliseconds = (int16_t)(val / MILLISECOND);
	val = val % MILLISECOND;
	ret.microseconds = (int16_t)val;
	return ret;
}

void TimeSpan::setDays(int32_t days) {
	raw_time = raw_time % DAY + ((int64_t)days) * DAY;
	
	/*bool oldPositive = raw_time >= 0;
	bool currentPositive = days >= 0;

	if (oldPositive != currentPositive) {
		//Negation of other fields needed
		raw_time = -(raw_time % DAY) + ((int64_t)days) * DAY;
	}
	else {
		//Just basic calculation
		raw_time = raw_time % DAY + ((int64_t)days) * DAY;
	}*/
}

int32_t TimeSpan::getDays() const {
	return (int32_t)(raw_time / DAY);
}

void TimeSpan::setHours(int8_t hours) {
	int64_t micr = raw_time % DAY;
	int8_t old_hours = (int8_t)(micr / HOUR);
	raw_time -= ((int64_t)(hours - (int8_t)old_hours)) * HOUR;
}

int8_t TimeSpan::getHours() const {
	return (int8_t)((raw_time % DAY) / HOUR);
}

void TimeSpan::setMinutes(int8_t minutes) {
	int64_t micr = raw_time % HOUR;
	int8_t old_minutes = (int8_t)(micr / MINUTE);
	raw_time -= ((int64_t)(minutes - (int8_t)old_minutes)) * MINUTE;
}

int8_t TimeSpan::getMinutes() const {
	return (int8_t)((raw_time % HOUR) / MINUTE);
}

void TimeSpan::setSeconds(int8_t seconds) {
	int64_t micr = raw_time % MINUTE;
	int8_t old_seconds = (int8_t)(micr / SECOND);
	raw_time -= ((int64_t)(seconds - (int8_t)old_seconds)) * SECOND;
}

int8_t TimeSpan::getSeconds() const {
	return (int8_t)((raw_time % MINUTE) / SECOND);
}

void TimeSpan::setMilliseconds(int16_t milliseconds) {
	int64_t micr = raw_time % SECOND;
	int16_t old_milliseconds = (int8_t)(micr / MILLISECOND);
	raw_time -= ((int64_t)(milliseconds - (int8_t)old_milliseconds)) * MILLISECOND;
}

int16_t TimeSpan::getMilliseconds() const {
	return (int16_t)((raw_time % SECOND) / MILLISECOND);
}

void TimeSpan::setMicroseconds(int16_t microseconds) {
	int64_t micr = raw_time % MILLISECOND;
	int16_t old_microseconds = (int8_t)(micr / MILLISECOND);
	raw_time -= ((int64_t)(microseconds - (int8_t)old_microseconds)) * MILLISECOND;
}

int16_t TimeSpan::getMicroseconds() const {
	return (int16_t)((raw_time % MILLISECOND) / MICROSECOND);
}

TimeSpan TimeSpan::getDuration() const {
	//Absolute value
	if (raw_time >= 0) return *this;
	else return TimeSpan(-raw_time);
}

const TimeSpan TimeSpan::MaxValue = TimeSpan(INT64_MAX);
const TimeSpan TimeSpan::MinValue = TimeSpan(INT64_MIN);
const TimeSpan TimeSpan::Zero = TimeSpan(0LL);

char* TimeSpan::toArray(char* buffer, size_t bufferSize, const char* format) const {
	if (bufferSize == 0) return buffer;
	bufferSize--;

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
				case 'd': {	//days
					int32_t days = getDays();
					if (days < 0) days = -days; //Absolute value
					if (sameCnt == 1) sameCnt = 0;
					char* newBuffer = dtlib::intToStr(buffer, bufferSize, days, sameCnt);
					bufferSize -= newBuffer - buffer;
					buffer = newBuffer;
					break;
				}
				case 'f': //Fraction of seconds
				case 'F': {
					if (sameCnt == 1) sameCnt = 0;
					char* newBuffer = dtlib::ms_usFractToStr(buffer, bufferSize, (raw_time % SECOND), sameCnt, lastChar == 'f');
					bufferSize -= newBuffer - buffer;
					buffer = newBuffer;
					break;
				}
				case 'h': {	//hours
					int8_t hours = getHours();
					if (hours < 0) hours = -hours; //Absolute value
					char* newBuffer = dtlib::intToStr2(buffer, bufferSize, hours, sameCnt > 1);
					bufferSize -= newBuffer - buffer;
					buffer = newBuffer;
					break;
				}
				case 'm': {	//minutes
					int8_t minutes = getMinutes();
					if (minutes < 0) minutes = -minutes; //Absolute value
					char* newBuffer = dtlib::intToStr2(buffer, bufferSize, minutes, sameCnt > 1);
					bufferSize -= newBuffer - buffer;
					buffer = newBuffer;
					break;
				}
				case 's': {	//seconds
					int16_t seconds = getSeconds();
					if (seconds < 0) seconds = -seconds; //Absolute value
					char* newBuffer = dtlib::intToStr2(buffer, bufferSize, seconds, sameCnt > 1);
					bufferSize -= newBuffer - buffer;
					buffer = newBuffer;
					break;
				}
				case 'i': {	//milliseconds
					int16_t milliseconds = getMilliseconds();
					if (milliseconds < 0) milliseconds = -milliseconds; //Absolute value
					if (sameCnt == 1) sameCnt = 0;
					char* newBuffer = dtlib::intToStr(buffer, bufferSize, milliseconds, sameCnt);
					bufferSize -= newBuffer - buffer;
					buffer = newBuffer;
					break;
				}
				case 'u': {	//microseconds
					int16_t microseconds = getMicroseconds();
					if (microseconds < 0) microseconds = -microseconds; //Absolute value
					if (sameCnt == 1) sameCnt = 0;
					char* newBuffer = dtlib::intToStr(buffer, bufferSize, microseconds, sameCnt);
					bufferSize -= newBuffer - buffer;
					buffer = newBuffer;
					break;
				}
				case 'N':
				case 'n': // B.C. or A.C. represented by sign
				if (lastChar == 'n' || isNegative()) {
					if (bufferSize < 1) {
						bufferSize = 0;
					}
					else {
						buffer[0] = (isNegative()) ? '-' : '+';
						bufferSize--;
						buffer++;
					}
				}
				break;

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


#ifdef ARDUINO
String TimeSpan::toString(const char* format) const {
#else
std::string TimeSpan::toString(const char* format) const {
#endif // ARDUINO
	char lastChar = 0;
	int8_t sameCnt = 1;

#ifdef ARDUINO
	String ret;
#else
	std::string ret;
#endif // ARDUINO
	ret.reserve(strlen(format) + 15); //Try to predict result size

	char buffer[10];

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
				case 'd': {	//days
					int32_t days = getDays();
					if (days < 0) days = -days; //Absolute value
					if (sameCnt == 1) sameCnt = 0;
					char* newBuffer = dtlib::intToStr(buffer, sizeof(buffer) - 1, days, sameCnt);
					newBuffer[0] = 0; //Null terminator
					ret += buffer;
					break;
				}
				case 'f': //Fraction of seconds
				case 'F': {
					if (sameCnt == 1) sameCnt = 0;
					else if (sameCnt > sizeof(buffer) - 1) sameCnt = sizeof(buffer) - 1;
					char* newBuffer = dtlib::ms_usFractToStr(buffer, sizeof(buffer) - 1, (raw_time % SECOND), sameCnt, lastChar == 'f');
					newBuffer[0] = 0; //Null terminator
					ret += buffer;
					break;
				}
				case 'h': {	//hours
					int8_t hours = getHours();
					if (hours < 0) hours = -hours; //Absolute value
					char* newBuffer = dtlib::intToStr2(buffer, sizeof(buffer) - 1, hours, sameCnt > 1);
					newBuffer[0] = 0; //Null terminator
					ret += buffer;
					break;
				}
				case 'm': {	//minutes
					int8_t minutes = getMinutes();
					if (minutes < 0) minutes = -minutes; //Absolute value
					char* newBuffer = dtlib::intToStr2(buffer, sizeof(buffer) - 1, minutes, sameCnt > 1);
					newBuffer[0] = 0; //Null terminator
					ret += buffer;
					break;
				}
				case 's': {	//seconds
					int16_t seconds = getSeconds();
					if (seconds < 0) seconds = -seconds; //Absolute value
					char* newBuffer = dtlib::intToStr2(buffer, sizeof(buffer) - 1, seconds, sameCnt > 1);
					newBuffer[0] = 0; //Null terminator
					ret += buffer;
					break;
				}
				case 'i': {	//milliseconds
					int16_t milliseconds = getMilliseconds();
					if (milliseconds < 0) milliseconds = -milliseconds; //Absolute value
					if (sameCnt == 1) sameCnt = 0;
					char* newBuffer = dtlib::intToStr(buffer, sizeof(buffer) - 1, milliseconds, sameCnt);
					newBuffer[0] = 0; //Null terminator
					ret += buffer;
					break;
				}
				case 'u': {	//microseconds
					int16_t microseconds = getMicroseconds();
					if (microseconds < 0) microseconds = -microseconds; //Absolute value
					if (sameCnt == 1) sameCnt = 0;
					char* newBuffer = dtlib::intToStr(buffer, sizeof(buffer) - 1, microseconds, sameCnt);
					newBuffer[0] = 0; //Null terminator
					ret += buffer;
					break;
				}
				case 'N':
				case 'n': // B.C. or A.C. represented by sign
					if (lastChar == 'n' || isNegative()) {
						ret += (isNegative()) ? '-' : '+';
					}
					break;

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