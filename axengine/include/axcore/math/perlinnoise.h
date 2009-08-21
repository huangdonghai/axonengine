/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_ENGINE_PERLINNOISE_H
#define AX_ENGINE_PERLINNOISE_H


namespace Axon {

	class AX_API PerlinNoise {
	public:
		PerlinNoise(uint_t seed = 0);

		float noise(float x);
		float noise2(float x, float y);
		float noise3(float x, float y, float z);

		float noise2(const Vector2& v, int octaves, float persistence = 0.25f);

	private:
		int xrand();

	private:
		enum {
			B = 0x100, BM = 0xff, N = 0x1000, NP = 12,   /* 2^N */
			NM = 0xfff
		};

		uint_t m_holdrand;
		int p[B + B + 2];
		float g3[B + B + 2][3];
		float g2[B + B + 2][2];
		float 	g1[B + B + 2];
	};

} // namespace Axon


#endif // end guardian

