/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


// for Euler angles
#define PITCH	0
#define YAW		1
#define ROLL	2
#define	SCALE	3

// map from [0..1] to [-1..1]
half4 Expand(half4 a)
{
	return a * 2 - 1;
}

half3 Expand(half3 a)
{
	return a * 2 - 1;
}

half Expand(half a)
{
	return a * 2 - 1;
}

half2 Expand(half2 a)
{
	return a * 2 - 1;
}

// map from -1..1 to 0..1
half4 Unexpand(half4 a)
{
	return a * 0.5 + 0.5;
}

half3 Unexpand(half3 a)
{
	return a * 0.5 + 0.5;
}

half Unexpand(half a)
{
	return a * 0.5 + 0.5;
}

half2 Unexpand(half2 a)
{
	return a * 0.5 + 0.5;
}

half4 GetNormal(sampler2D samp, float2 uv)
{
	half4 result;

	result = tex2D(samp, uv);
	result.rgb = result.rgb * 2 - 1;

	return result;
}

half4 GetNormalNormalized(sampler2D samp, float2 uv)
{
	half4 result;

	result = tex2D(samp, uv);
	result.rgb = result.rgb * 2 - 1;
	result.rgb = normalize(result.rgb);

	return result;
}


//  Constructs a Z-axis rotation matrix
float3x3 RotationMatrix_zAxis(float fAngle)
{
	// compute sin/cos of fAngle
	float2 vSinCos;
	sincos(fAngle, vSinCos.x, vSinCos.y);

	return float3x3(
		vSinCos.y, -vSinCos.x,	0.0f, 
		vSinCos.x,	vSinCos.y,	0.0f, 
		0.0f,		0.0f,		1.0f);
}

//  Constructs a Y-axis rotation matrix
float3x3 RotationMatrix_yAxis(float fAngle)
{
	// compute sin/cos of fAngle
	float2 vSinCos;
	sincos(fAngle, vSinCos.x, vSinCos.y);

	return float3x3(
		vSinCos.y,	0.0f, vSinCos.x,
		0.0f,		1.0f, 0.0f,
		-vSinCos.x, 0.0f, vSinCos.y);
}

//  Constructs a X-axis rotation matrix
float3x3 RotationMatrix_xAxis(float fAngle)
{
	// compute sin/cos of fAngle
	float2 vSinCos;
	sincos(fAngle, vSinCos.x, vSinCos.y);

	return float3x3(
		1.0f, 0.0f,		0.0f,
		0.0f, vSinCos.y, -vSinCos.x,
		0.0f, vSinCos.x, vSinCos.y);
}

float3x3 RotationMatrix_angles(float pitch, float yaw)
{
	float	sp, sy, cp, cy;

	sincos(yaw, sy, cy);
	sincos(pitch, sp, cp);

	return float3x3(
		cp * cy,	-sy,	sp * cy,
		cp * sy,	cy,		sp * sy,
		-sp,		0,		cp
		);
}

float3x3 RotationMatrix_angles(float2 angles)
{
	float2 s, c;
	sincos(angles, s, c);

	return float3x3(
		c.x * c.y,	-s.y,	s.x * c.y,
		c.x * s.y,	c.y,	s.x * s.y,
		-s.x,		0,		c.x
		);
#if 0
	float	sp, sy, cp, cy;

	sincos(yaw, sy, cy);
	sincos(pitch, sp, cp);

	return float3x3(
		cp * cy,	-sy,	sp * cy,
		cp * sy,	cy,		sp * sy,
		-sp,		0,		cp
		);
#endif
}

float3x3 RotationMatrix_angles(float pitch, float yaw, float roll)
{
	float	sr, sp, sy, cr, cp, cy;

	sincos(yaw, sy, cy);
	sincos(pitch, sp, cp);
	sincos(roll, sr, cr);

	return float3x3(
		cp * cy,	sr * sp * cy + cr * -sy,	cr * sp * cy + -sr * -sy,
		cp * sy,	sr * sp * sy + cr * cy,		cr * sp * sy + -sr * cy,
		-sp,		sr * cp,					cr * cp
	);
}

// Packing a [0-1] float value into a 4D vector where each component will be a 8-bits integer:
float4 PackFloat(const float value)
{
	const float4 bitSh = float4(256.0*256.0*256.0, 256.0*256.0, 256.0, 1.0);
	const float4 bitMsk = float4(0.0, 1.0/256.0, 1.0/256.0, 1.0/256.0);
	float4 res = frac(value * bitSh);
	res -= res.xxyz * bitMsk;
	return res;
}

// Unpacking a [0-1] float value from a 4D vector where each component was a 8-bits integer:
float UnpackFloat(const float4 value)
{
	const float4 bitSh = float4(1.0/(256.0*256.0*256.0), 1.0/(256.0*256.0), 1.0/256.0, 1.0);
	return(dot(value, bitSh));
}

float2 PackFloat16(float depth)
{
	depth /= 4;

	float Integer = floor(depth);
	float fraction = frac(depth);

	return float2(Integer/256, fraction);
}

float UnpackFloat16(float2 depth)
{
	const float2 unpack = float2(1024.0f, 4.0f);

	return dot(unpack, depth);
}

float HardStep(float min, float max, float v)
{
	return saturate((v - min) / (max - min));
}

float IsInRange(float min, float max, float v)
{
	return step(v - min, 0) - step(v-max, 0);
}

float4 Clip2Screen(float4 p)
{
#if G_OPENGL
	return float4((p.xyz + p.w) * 0.5, p.w);
#else
	return float4((float2(p.x + p.w, p.w - p.y) + g_sceneSize.zw * p.w) * 0.5, p.zw);
#endif
}

// wave function
float Phase_sin(float t)
{
	return sin(t * 3.14 * 2);
}

float Phase_squal(float t)
{
	return step(0.5, t);
}

float Phase_sawTooth(float t)
{
	return t * 2 - 1;
}

float Phase_invSawTooth(float t)
{
	return 1.0 - Phase_sawTooth(t);
}

half Rgb2Lum(half3 color)
{
//	return dot(color, half3(0.3, 0.59,0.11));
	return dot(color, half3(0.2125, 0.7154,0.0721));
}

half Dif2Spec(half3 color)
{
#if 0
	half result = Rgb2Lum(color);
	return saturate(result * 2 - 0.5);
#else
	return Rgb2Lum(color);
#endif
}