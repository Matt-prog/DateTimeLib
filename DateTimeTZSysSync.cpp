#include "DateTimeTZSysSync.h"


DateTimeSysSync DateTimeTZSysSync::getSysSyncUTC() const {
	DateTimeSysSync ret(*this);
	int64_t tzOffset = (int64_t)getTimeZoneOffset();
	int64_t dstOffset = (int64_t)getCurrentDSTOffset();
	ret -= (tzOffset + dstOffset);
	return ret;
}

int64_t DateTimeTZSysSync::recalcDST() {
	DT_SYNC_TYPE currentTime = DT_SYNC_FUNC();
	int64_t rawTime = DateTimeBase::getRawTimeNoSync() + (int64_t)((currentTime - syncTime)) * DT_SYNC_RESOLUTION;
	if (DateTimeTZBase<DateTimeTZSysSync>::adj.noDST()) return rawTime;

	bool overTransition = (nextDSTTransitionDT <= rawTime); //Checking if synced time is over next DST transition time
	if (overTransition) {
		//We are now over next DST transition, so we need to calculate new next DST transition
		int64_t rawTimeNoDST = rawTime;
		bool oldDST = DateTimeTZBase<DateTimeTZSysSync>::adj.isDST();
		if (oldDST) {
			rawTimeNoDST -= (int64_t)DateTimeTZBase<DateTimeTZSysSync>::getDSTOffset(); //Getting time without DST
		}
		bool newIsDST;
		nextDSTTransitionDT = DateTimeTZBase<DateTimeTZSysSync>::adj.getNextTransitionDate(rawTimeNoDST, newIsDST);
		newIsDST = !newIsDST;
		if (oldDST != newIsDST) {
			int64_t offset;
			if (newIsDST) {
				//Applying DST offset
				offset = (int64_t)getDSTOffset();
			}
			else {
				//Removing DST offset
				offset = -(int64_t)getDSTOffset();
			}
			DateTimeTZBase<DateTimeTZSysSync>::addRawTimeNoTD(offset);
			adj.setDST(newIsDST);
			return rawTime + offset;
		}
	}
	return rawTime;
}

DateTime_DST_tuple DateTimeTZSysSync::getDateTimeEnh() const {
	DT_SYNC_TYPE currentTime = DT_SYNC_FUNC();
	int64_t raw = DateTimeBase::getRawTimeNoSync() + (int64_t)((currentTime - syncTime)) * DT_SYNC_RESOLUTION;
	bool isDST_ = DateTimeTZBase<DateTimeTZSysSync>::adj.isDST();
	bool overTransition = (nextDSTTransitionDT <= raw); //Checking if synced time is over next DST transition time
	if (overTransition) {
		if (isDST_) {
			//Was DST, but now is not
			raw -= (int64_t)getDSTOffset();
		}
		else {
			//Was not DST, but now is
			raw += (int64_t)getDSTOffset();
		}
		isDST_ = !isDST_;
	}
	return DateTime_DST_tuple(raw, isDST_);
}

int64_t DateTimeTZSysSync::getRawTimeSync() const {
	DT_SYNC_TYPE currentTime = DT_SYNC_FUNC();
	int64_t raw = DateTimeBase::getRawTimeNoSync() + (int64_t)((currentTime - syncTime)) * DT_SYNC_RESOLUTION;
	bool overTransition = (nextDSTTransitionDT <= raw); //Checking if synced time is over next DST transition time
	if (overTransition) {
		if (DateTimeTZBase<DateTimeTZSysSync>::adj.isDST()) {
			//Was DST, but now is not
			return raw - (int64_t)getDSTOffset();
		}
		else {
			//Was not DST, but now is
			return raw + (int64_t)getDSTOffset();
		}
	}
	else {
		//No change
		return raw;
	}
}

void DateTimeTZSysSync::preSetSync() {
	syncTime = DT_SYNC_FUNC();
}

int64_t DateTimeTZSysSync::getAndPreSetSync() {
	DT_SYNC_TYPE currentTime = DT_SYNC_FUNC();
	int64_t raw = DateTimeBase::getRawTimeNoSync() + (int64_t)((currentTime - syncTime)) * DT_SYNC_RESOLUTION;
	bool overTransition = (nextDSTTransitionDT <= raw); //Checking if synced time is over next DST transition time
	if (overTransition) {
		if (DateTimeTZBase<DateTimeTZSysSync>::adj.isDST()) {
			//Was DST, but now is not
			raw -= (int64_t)getDSTOffset();
		}
		else {
			//Was not DST, but now is
			raw += (int64_t)getDSTOffset();
		}
	}
	syncTime = currentTime;
	return raw;
}

bool DateTimeTZSysSync::isSyncDST() const {
	bool negNeeded = isOverNextTrans(); //Checking if synced time is over next DST transition time
	return negNeeded != DateTimeTZBase<DateTimeTZSysSync>::adj.isDST();
}

void DateTimeTZSysSync::calcNextTransOnSet(int64_t newRaw, bool is_DST) {
	if (!DateTimeTZBase<DateTimeTZSysSync>::adj.noDST()) {
		//We are now over next DST transition, so we need to calculate new next DST transition
		if (is_DST) {
			newRaw -= (int64_t)DateTimeTZBase<DateTimeTZSysSync>::getDSTOffset(); //Getting time without DST
		}
		bool newIsDST;
		nextDSTTransitionDT = DateTimeTZBase<DateTimeTZSysSync>::adj.getNextTransitionDate(newRaw, newIsDST);
	}
	else {
		nextDSTTransitionDT = DateTime::MaxValue;
	}
}

/*void DateTimeTZSysSync::ctorBeforeCtor() {
	nextDSTTransitionDT = DateTime::MaxValue;
}*/

bool DateTimeTZSysSync::isOverNextTrans() const {
	DT_SYNC_TYPE currentTime = DT_SYNC_FUNC();
	int64_t raw = DateTimeBase::getRawTimeNoSync() + (int64_t)((currentTime - syncTime)) * DT_SYNC_RESOLUTION;
	return nextDSTTransitionDT <= raw;
}

int64_t DateTimeTZSysSync::unappliedOffset(DT_SYNC_TYPE currentTime) const {
	//DT_SYNC_TYPE currentTime = DT_SYNC_FUNC();
	int64_t raw = DateTimeBase::getRawTimeNoSync() + (int64_t)((currentTime - syncTime)) * DT_SYNC_RESOLUTION;
	bool isDST_ = DateTimeTZBase<DateTimeTZSysSync>::adj.isDST();
	bool overTransition = (nextDSTTransitionDT <= raw); //Checking if synced time is over next DST transition time
	if (overTransition) {
		if (isDST_) {
			//Was DST, but now is not
			return -(int64_t)getDSTOffset();
		}
		else {
			//Was not DST, but now is
			return (int64_t)getDSTOffset();
		}
	}
	return 0LL;
}

void DateTimeTZSysSync::setSyncedTime(int64_t baseRawValue, DT_SYNC_TYPE syncTime_new, bool DST_app) {
	syncTime = syncTime_new; //Copying sync time

	if (DateTimeTZBase<DateTimeTZSysSync>::adj.noDST()) {
		DateTimeTZBase<DateTimeTZSysSync>::setRawTimeNoTD(baseRawValue);
		adj.setDST(false);
		nextDSTTransitionDT = DateTime::MaxValue;
		return;
	}

	if (DST_app) {
		baseRawValue -= (int64_t)DateTimeTZBase<DateTimeTZSysSync>::getDSTOffset(); //Getting time without DST
	}
	DT_SYNC_TYPE currentTime = DT_SYNC_FUNC();
	int64_t currRawTime = baseRawValue + (int64_t)((currentTime - syncTime)) * DT_SYNC_RESOLUTION;

	bool newIsDST;
	nextDSTTransitionDT = DateTimeTZBase<DateTimeTZSysSync>::adj.getNextTransitionDate(currRawTime, newIsDST);
	newIsDST = !newIsDST; //getting DST of region before next transition

	if (newIsDST) {
		baseRawValue += (int64_t)DateTimeTZBase<DateTimeTZSysSync>::getDSTOffset(); //Applying DST offset
	}

	DateTimeTZBase<DateTimeTZSysSync>::setRawTimeNoTD(baseRawValue);
	adj.setDST(newIsDST);
}



#if DT_UNDER_OS > 0
DateTimeTZSysSync DateTimeTZSysSync::getSysTime() {
	DateTimeSysSync ndt = DateTimeSysSync::getSysTimeUTC();

	DSTAdjustment adj = getSystemDST();
	TimeZone tz = getSystemTZ();
	ndt += tz.getTimeZoneOffset();

	return DateTimeTZSysSync(ndt, tz, adj, false);
}

#elif defined(ESP32) || defined(ESP8266)

DateTimeTZSysSync DateTimeTZSysSync::getSysTime() {
	DateTimeSysSync ret = DateTimeSysSync::getSysTimeUTC();
	TimeZoneInfo tzinfo = TimeZoneInfo::getSystemTZInfo();
	ret += tzinfo.timeZone.getTimeZoneOffset();
	return DateTimeTZSysSync(ret, tzinfo.timeZone, tzinfo.DST, false);
}

#endif // !DT_UNDER_OS > 0