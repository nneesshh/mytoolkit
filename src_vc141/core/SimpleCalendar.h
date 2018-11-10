#pragma  once
//------------------------------------------------------------------------------
/**
	@class CSimpleCalendar

	(C) 2016 n.lee
*/
#include <time.h>
#include <stdio.h>
#include <string.h>

#include "../core/MyMacros.h"
#include "../core/snprintf/mysnprintf.h"

//
struct calendar_rule_t {
	char	_chYesterday[32];
	time_t	_yesterday;

	char	_chToday[32];
	time_t	_today;
	int		_today_mon_in_year;
	int		_today_day_in_month;

	char	_chTomorrow[32];
	time_t	_tomorrow;

	char	_chWeekday[32];
	time_t	_weekday;

	char	_chWeekdayPre[32];
	time_t	_weekdayPre;

	char	_chWeekdayPost[32];
	time_t	_weekdayPost;

	char	_chMonthday[32];
	time_t	_monthday;

	char	_chMonthdayPre[32];
	time_t	_monthdayPre;

	char	_chMonthdayPost[32];
	time_t	_monthdayPost;
};

//------------------------------------------------------------------------------
/**
@brief CSimpleCalendar
*/
class CSimpleCalendar {
public:
	CSimpleCalendar();
	CSimpleCalendar(time_t tmNow);
	~CSimpleCalendar();

	void						Init(time_t tmNow);
	void						Update(time_t tmNow);

	calendar_rule_t *			GetCalendarRule() {
		return &_rule;
	}

	bool						IsYesterday(const char *sDay);
	bool						IsYesterday(time_t day) {
		return (day == _rule._yesterday);
	}

	bool						IsToday(const char *sDay);
	bool						IsToday(time_t day) {
		return (day == _rule._today);
	}

	bool						IsTomorrow(const char *sDay);
	bool						IsTomorrow(time_t day) {
		return (day == _rule._tomorrow);
	}

public:
	static int					MonthFrom1970(time_t tmDaypoint) {
		struct tm tm_;
		localtime_s(&tm_, &tmDaypoint);
		return (tm_.tm_year - 70) * 12 + tm_.tm_mon;
	}

	static int					MonthInYear(time_t tmDaypoint) {
		struct tm tm_;
		localtime_s(&tm_, &tmDaypoint);
		return tm_.tm_mon + 1;
	}

	static int					DayInMonth(time_t tmDaypoint) {
		struct tm tm_;
		localtime_s(&tm_, &tmDaypoint);
		return tm_.tm_mday;
	}

	static time_t				WeekDay(time_t tmDaypoint) {
		struct tm tm_;
		localtime_s(&tm_, &tmDaypoint);
		tm_.tm_hour = 0;
		tm_.tm_min = 0;
		tm_.tm_sec = 0;
		tm_.tm_isdst = -1;
		time_t theday = mktime(&tm_);

		// wday -- Sunday is the tm_wday beginning, but we use Monday as our week beginning
		int wday = ((tm_.tm_wday - 1) + 7) % 7;
		return theday - wday * DAY_SECONDS;
	}

	static void					MonthDay(time_t tmDaypoint, time_t& outMonthDay, time_t& outMonthDayPre, time_t& outMonthDayPost) {
		struct tm tm_;
		localtime_s(&tm_, &tmDaypoint);

		int year = tm_.tm_year;
		int mon = tm_.tm_mon;
		int mday = 1;

		tm_.tm_year = year;
		tm_.tm_mon = mon;
		tm_.tm_mday = mday;
		tm_.tm_hour = 0;
		tm_.tm_min = 0;
		tm_.tm_sec = 0;
		tm_.tm_isdst = -1;
		outMonthDay = mktime(&tm_);

		tm_.tm_year = year;
		tm_.tm_mon = mon - 1;
		tm_.tm_mday = mday;
		tm_.tm_hour = 0;
		tm_.tm_min = 0;
		tm_.tm_sec = 0;
		tm_.tm_isdst = -1;
		outMonthDayPre = mktime(&tm_);

		tm_.tm_year = year;
		tm_.tm_mon = mon + 1;
		tm_.tm_mday = mday;
		tm_.tm_hour = 0;
		tm_.tm_min = 0;
		tm_.tm_sec = 0;
		tm_.tm_isdst = -1;
		outMonthDayPost = mktime(&tm_);
	}

	static time_t				DatetimeToDate(time_t tmDaypoint) {
		struct tm tm_;
		localtime_s(&tm_, &tmDaypoint);
		tm_.tm_hour = 0;
		tm_.tm_min = 0;
		tm_.tm_sec = 0;
		tm_.tm_isdst = -1;
		return mktime(&tm_);
	}

	static time_t				StringToDate(const char *str) {
		tm tm_;
		int year = 0, month = 1, day = 1;
		sscanf(str, "%d-%d-%d", &year, &month, &day);
		tm_.tm_year = year - 1900;
		tm_.tm_mon = month - 1;
		tm_.tm_mday = day;
		tm_.tm_hour = 0;
		tm_.tm_min = 0;
		tm_.tm_sec = 0;
		tm_.tm_isdst = -1;

		time_t t_ = mktime(&tm_);
		return t_;
	}

	static time_t				StringToDatetime(const char *str) {
		tm tm_;
		int year = 0, month = 1, day = 1, hour = 0, minute = 0, second = 0;
		sscanf(str, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
		tm_.tm_year = year - 1900;
		tm_.tm_mon = month - 1;
		tm_.tm_mday = day;
		tm_.tm_hour = hour;
		tm_.tm_min = minute;
		tm_.tm_sec = second;
		tm_.tm_isdst = -1;

		time_t t_ = mktime(&tm_);
		return t_;
	}

	static struct tm			FormatDate(time_t tmDay, char *chFormat, size_t szFormatLen) {
		struct tm tm_;
		localtime_s(&tm_, &tmDay);

		tm_.tm_hour = 0;
		tm_.tm_min = 0;
		tm_.tm_sec = 0;

		o_snprintf(
			chFormat
			, szFormatLen
			, "%04d-%02d-%02d"
			, tm_.tm_year + 1900
			, tm_.tm_mon + 1
			, tm_.tm_mday);
		return tm_;
	}

	static struct tm			FormatDatetime(time_t tmDaypoint, char *chFormat, size_t szFormatLen) {
		struct tm tm_;
		localtime_s(&tm_, &tmDaypoint);

		o_snprintf(
			chFormat
			, szFormatLen
			, "%04d-%02d-%02d %02d:%02d:%02d"
			, tm_.tm_year + 1900
			, tm_.tm_mon + 1
			, tm_.tm_mday
			, tm_.tm_hour
			, tm_.tm_min
			, tm_.tm_sec);
		return tm_;
	}

private:
	calendar_rule_t			_rule;

};

/*EOF*/