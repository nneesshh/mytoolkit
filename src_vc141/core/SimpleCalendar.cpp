//------------------------------------------------------------------------------
//  SimpleCalendar.cpp
//  (C) 2016 n.lee
//------------------------------------------------------------------------------
#include "SimpleCalendar.h"

//------------------------------------------------------------------------------
/**

*/
CSimpleCalendar::CSimpleCalendar() {
	Init(time(nullptr));
}

//------------------------------------------------------------------------------
/**

*/
CSimpleCalendar::CSimpleCalendar(time_t tmNow) {
	Init(tmNow);
}

//------------------------------------------------------------------------------
/**

*/
CSimpleCalendar::~CSimpleCalendar() {

}

//------------------------------------------------------------------------------
/**

*/
void
CSimpleCalendar::Init(time_t tmNow) {
	int wday, mday;
	int year, mon;

	// today
	struct tm tm_ = FormatDate(tmNow, _rule._chToday, sizeof(_rule._chToday));

	// mday
	mday = tm_.tm_mday;

	// wday -- Sunday is the tm_wday beginning, but we use Monday as our week beginning
	wday = ((tm_.tm_wday - 1) + 7) % 7;

	// year
	year = tm_.tm_year;

	// mon
	mon = tm_.tm_mon;

	// day
	_rule._today = mktime(&tm_);
	_rule._today_mon_in_year = tm_.tm_mon + 1;
	_rule._today_day_in_month = tm_.tm_mday;

	_rule._yesterday = _rule._today - DAY_SECONDS;
	_rule._tomorrow = _rule._today + DAY_SECONDS;

	// weekday
	_rule._weekday = _rule._today - wday * DAY_SECONDS;
	_rule._weekdayPre = _rule._weekday - 7 * DAY_SECONDS;
	_rule._weekdayPost = _rule._weekday + 7 * DAY_SECONDS;

	// monthday
	_rule._monthday = _rule._today - (mday - 1) * DAY_SECONDS;

	tm_.tm_year = year;
	tm_.tm_mon = mon - 1;
	tm_.tm_mday = 1;
	_rule._monthdayPre = mktime(&tm_);

	tm_.tm_year = year;
	tm_.tm_mon = mon + 1;
	tm_.tm_mday = 1;
	_rule._monthdayPost = mktime(&tm_);

	// yesterday
	FormatDate(_rule._yesterday, _rule._chYesterday, sizeof(_rule._chYesterday));

	// tomorrow
	FormatDate(_rule._tomorrow, _rule._chTomorrow, sizeof(_rule._chTomorrow));

	// weekday
	FormatDate(_rule._weekday, _rule._chWeekday, sizeof(_rule._chWeekday));

	// weekday pre
	FormatDate(_rule._weekdayPre, _rule._chWeekdayPre, sizeof(_rule._chWeekdayPre));

	// weekday post
	FormatDate(_rule._weekdayPost, _rule._chWeekdayPost, sizeof(_rule._chWeekdayPost));

	// monthday
	FormatDate(_rule._monthday, _rule._chMonthday, sizeof(_rule._chMonthday));

	// monthday pre
	FormatDate(_rule._monthdayPre, _rule._chMonthdayPre, sizeof(_rule._chMonthdayPre));

	// monthday post
	FormatDate(_rule._monthdayPost, _rule._chMonthdayPost, sizeof(_rule._chMonthdayPost));
}

//------------------------------------------------------------------------------
/**

*/
void
CSimpleCalendar::Update(time_t tmNow) {
	if (tmNow - _rule._today >= DAY_SECONDS) {
		Init(_rule._today + DAY_SECONDS);
	}
}

//------------------------------------------------------------------------------
/**

*/
bool
CSimpleCalendar::IsYesterday(const char *sDay) {
	return IsYesterday(StringToDate(sDay));
}

//------------------------------------------------------------------------------
/**

*/
bool
CSimpleCalendar::IsToday(const char *sDay) {
	return IsToday(StringToDate(sDay));
}

//------------------------------------------------------------------------------
/**

*/
bool
CSimpleCalendar::IsTomorrow(const char *sDay) {
	return IsTomorrow(StringToDate(sDay));
}

/* -- EOF -- */