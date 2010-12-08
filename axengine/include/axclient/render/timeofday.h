/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_RENDER_TIMEOFDAY_H
#define AX_RENDER_TIMEOFDAY_H

AX_BEGIN_NAMESPACE

class TimeOfDay {
public:
	enum Category {
		CatSun, CatDaySky, CatNightSky, CatMoon, CatDayLight, CatNightLight
	};

	enum ItemKind {
		kSunColor,

		NUM_ITEM_KINDS
	};

	enum ItemDataType {
		dFloat, dColor
	};

	union ItemValue {
		float fValue;
		int colorValue;
	};

	struct ItemData {
		float time;		// time in [0~1]
		ItemValue value;
	};

	typedef std::vector<ItemData> ItemDataSeq;

	class Item {
	public:
		std::string getName() const;
		void addControlPoint(int minutes, float value);
		void addControlPoint(int minutes, Rgb color);
		float getFloatValue(int minutes) const;
		Rgb getColorValue(int minutes) const;

	private:
		std::string m_name;
		Category m_category;
		ItemDataType m_dataType;
		ItemDataSeq m_datas;
	};

	TimeOfDay();
	~TimeOfDay();

	void addValue(ItemKind k, int minutes, float value);
	void addValue(ItemKind k, int minutes, Rgb color);

private:
	Item m_items[NUM_ITEM_KINDS];
};

AX_END_NAMESPACE

#endif // AX_RENDER_TIMEOFDAY_H