/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_CALENDAR_H
#define AX_CALENDAR_H

namespace Axon {

	enum { START_YEAR = 1901 };

	class AX_API DateTime {
	public:
		struct Data {
			ushort_t year;				// from 1901 to 2050
			ushort_t month;				// [ 1..12 ]
			ushort_t day;				// [ 1..31 ]
			ushort_t hour;				// [ 0..23 ]
			ushort_t minute;				// [ 0..59 ]
			ushort_t second;				// [ 0..59 ]
			ushort_t milliseconds;		// [ 0..999 ]

			// below is calculated out
			ushort_t dayOfWeek;			// [ 0..6 ], 0 is sunday

			// for Lunar Calendar
			ushort_t tiangan;			// [ 0..9 ]
			ushort_t dizhi;				// [ 0..11 ]
			ushort_t lunarYear;			// [ 1900 .. 2050 ]
			ushort_t lunarMonth;			// [ 1..12 ]
			bool leapMonth;			// true or false
			ushort_t lunarDay;			// [ 1..30 ]
			ushort_t solarTerm;			// 0 isn't a solar day, or is [ 1..24]

			// for sun and moon
			float sunAngle;			// in degree, 0~360
			float moonAngle;			// in degree, 0~360

			// total time in milliseconds from 1901
			ulonglong_t totaltime;
		};

		DateTime();
		~DateTime();

		void initSystemTime();
		void initSystemTime(uint_t start_time);

		void init(ushort_t year, ushort_t month, ushort_t day, ushort_t hour, ushort_t minute, ushort_t second, uint_t start_time);
		void init(ushort_t year, ushort_t month, ushort_t day, ushort_t hour, ushort_t minute, ushort_t second);
		void init(ushort_t hour, ushort_t minute, ushort_t second, uint_t startTime);
		void init(ushort_t hour, ushort_t minute, ushort_t second);

		Data getData();
#if 0
		Data getData(uint_t now_time);
#endif
		void update(int curtime);

	public:
		// 判断year是不是闰年
		static bool isLeapYear(ushort_t year);

		// 计算year, month, day对应是星期几 1年1月1日 --- 65535年12月31日
		static ushort_t weekDay(ushort_t year, ushort_t month, ushort_t day);

		// 返回year年month月的天数 1年1月 --- 65535年12月
		static ushort_t monthDays(ushort_t year, ushort_t month);

		// 返回阴历lunarYear年阴历lunarMonth月的天数，如果lunarMonth为闰月
		// 高字为第二个lunarMonth月的天数，否则高字为0
		//  1901年1月---2050年12月
		static int lunarMonthDays(ushort_t lunarYear, ushort_t lunarMonth);

		// 返回阴历lunarYear年的总天数
		//  1901年1月---2050年12月
		static ushort_t lunarYearDays(ushort_t lunarYear);

		// return lunar year's tiangan and dizhi
		static void lunarYearToTianganDizhi(ushort_t lunarYear, ushort_t& tiangan, ushort_t& dizhi);

		// 返回阴历lunarYear年的闰月月份，如没有返回0
		//  1901年1月---2050年12月
		static ushort_t getLeapMonth(ushort_t lunarYear);

		// 把iYear年格式化成天干记年法表示的字符串
		static String formatLunarYear(ushort_t  year);

		// 把iMonth格式化成中文字符串
		static String formatMonth(ushort_t month, bool lunar = true);

		// 把iDay格式化成中文字符串
		static String formatLunarDay(ushort_t  day);

		// 计算公历两个日期间相差的天数  1年1月1日 --- 65535年12月31日
		static int calcDateDiff(ushort_t endYear, ushort_t endMonth, ushort_t endDay, ushort_t startYear = START_YEAR, ushort_t startMonth =1, ushort_t startDay =1);

		// 计算公历iYear年iMonth月iDay日对应的阴历日期,返回对应的阴历节气 0-24
		// 1901年1月1日---2050年12月31日
		static ushort_t getLunarDate(ushort_t year, ushort_t month, ushort_t day, ushort_t &lunarYear, ushort_t &lunarMonth, ushort_t &lunarDay, bool& leapMonth);

	protected:
		// 计算从1901年1月1日过span_days天后的阴历日期
		static void l_calcLunarDate(ushort_t &year, ushort_t &month ,ushort_t &day, bool& leapMonth, int span_days);

		// 计算从1901年1月1日过span_days天后的阳历日期
		static void l_calcDate(ushort_t &year, ushort_t &month ,ushort_t &day, int span_days);

		// 计算公历iYear年iMonth月iDay日对应的节气 0-24，0表不是节气
		static ushort_t l_getLunarHolDay(ushort_t year, ushort_t month, ushort_t day);

	private:
		Data m_startData;
		uint_t m_startMilliseconds;
		Data m_curData;
		uint_t m_curMilliseconds;
	};

} // namespace Axon

#endif // AX_CALENDAR_H
