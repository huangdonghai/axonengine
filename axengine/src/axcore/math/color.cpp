/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/
#pragma hdrstop

#include "../private.h"

AX_BEGIN_NAMESPACE

	String Rgb::toString() const {
		String result;
		StringUtil::sprintf(result, "%d %d %d", r, g, b);
		return result;
	}

	bool Rgb::fromString(const char *str) {
		int _r, _g, _b;
		int v = sscanf(str, "%d %d %d", &_r, &_g, &_b);
		AX_ASSERT(v == 3);
		set(_r, _g, _b);

		return v == 3;
	}

	const Rgb Rgb::Black = Rgb(0, 0, 0);
	const Rgb Rgb::Red = Rgb(255, 0, 0);
	const Rgb Rgb::Green = Rgb(0, 255, 0);
	const Rgb Rgb::Blue = Rgb(0, 0, 255);
	const Rgb Rgb::Yellow = Rgb(255, 255, 0);
	const Rgb Rgb::Magenta = Rgb(255, 0, 255);
	const Rgb Rgb::Cyan = Rgb(0, 255, 255);
	const Rgb Rgb::White = Rgb(255, 255, 255);
	const Rgb Rgb::LtGrey = Rgb(191, 191, 191);
	const Rgb Rgb::MdGrey = Rgb(127, 127, 127);
	const Rgb Rgb::DkGrey = Rgb(63, 63, 63);

	const Rgb Rgb::ColorTable[]= {
		Black ,	
		Red ,	
		Green ,	
		Yellow ,	
		Blue ,	
		Cyan ,	
		Magenta ,	
		White ,	
	};


	const Rgba Rgba::Zero = Rgba(0, 0, 0, 0);
	const Rgba Rgba::Black = Rgba(0, 0, 0, 255);
	const Rgba Rgba::Red = Rgba(255, 0, 0, 255);
	const Rgba Rgba::Green = Rgba(0, 255, 0, 255);
	const Rgba Rgba::Blue = Rgba(0, 0, 255, 255);
	const Rgba Rgba::Yellow = Rgba(255, 255, 0, 255);
	const Rgba Rgba::Magenta = Rgba(255, 0, 255, 255);
	const Rgba Rgba::Cyan = Rgba(0, 255, 255, 255);
	const Rgba Rgba::White = Rgba(255, 255, 255, 255);
	const Rgba Rgba::LtGrey = Rgba(191, 191, 191, 255);
	const Rgba Rgba::MdGrey = Rgba(127, 127, 127, 255);
	const Rgba Rgba::DkGrey = Rgba(63, 63, 63, 255);

	const Rgba Rgba::ColorTable[]= {
		Black ,	
		Red ,	
		Green ,	
		Yellow ,	
		Blue ,	
		Cyan ,	
		Magenta ,	
		White ,	
	};

	String Rgba::toString() const {
		String result;

		StringUtil::sprintf(result, "%d %d %d %d", r,g,b,a);

		return result;
	}

	void Rgba::fromString(const char *str) {
		int _r, _g, _b, _a;
		int v = sscanf(str, "%d %d %d %d", &_r, &_g, &_b, &_a);
		AX_STRICT_ASSERT(v = 4);
		r = _r; g = _g; b = _b; a = _a;
	}


AX_END_NAMESPACE

