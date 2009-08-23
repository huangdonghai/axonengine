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
half4 Expand(half4 a) {
	return a * 2 - 1;
}

half3 Expand(half3 a) {
	return a * 2 - 1;
}

half Expand(half a) {
	return a * 2 - 1;
}

half2 Expand(half2 a) {
	return a * 2 - 1;
}

// map from -1..1 to 0..1
half4 Unexpand(half4 a) {
	return a * 0.5 + 0.5;
}

half3 Unexpand(half3 a) {
	return a * 0.5 + 0.5;
}

half Unexpand(half a) {
	return a * 0.5 + 0.5;
}

half2 Unexpand(half2 a) {
	return a * 0.5 + 0.5;
}

half4 GetNormal(sampler2D samp, float2 uv) {
	half4 result;

	result = tex2D(samp, uv);
	result.rgb = result.rgb * 2 - 1;

	return result;
}

half4 GetNormalNormalized(sampler2D samp, float2 uv) {
	half4 result;

	result = tex2D(samp, uv);
	result.rgb = result.rgb * 2 - 1;
	result.rgb = normalize(result.rgb);

	return result;
}


//  Constructs a Z-axis rotation matrix
float3x3 RotationMatrix_zAxis(float fAngle) {
	// compute sin/cos of fAngle
	float2 vSinCos;
	sincos(fAngle, vSinCos.x, vSinCos.y);

	return float3x3(
		vSinCos.y, -vSinCos.x,	0.0f, 
		vSinCos.x,	vSinCos.y,	0.0f, 
		0.0f,		0.0f,		1.0f);
}

//  Constructs a Y-axis rotation matrix
float3x3 RotationMatrix_yAxis(float fAngle) {
	// compute sin/cos of fAngle
	float2 vSinCos;
	sincos(fAngle, vSinCos.x, vSinCos.y);

	return float3x3(
		vSinCos.y,	0.0f, vSinCos.x,
		0.0f,		1.0f, 0.0f,
		-vSinCos.x, 0.0f, vSinCos.y);
}

//  Constructs a X-axis rotation matrix
float3x3 RotationMatrix_xAxis(float fAngle) {
	// compute sin/cos of fAngle
	float2 vSinCos;
	sincos(fAngle, vSinCos.x, vSinCos.y);

	return float3x3(
		1.0f, 0.0f,		0.0f,
		0.0f, vSinCos.y, -vSinCos.x,
		0.0f, vSinCos.x, vSinCos.y);
}

float3x3 RotationMatrix_angles(float pitch, float yaw) {
	float	sp, sy, cp, cy;

	sincos(yaw, sy, cy);
	sincos(pitch, sp, cp);

	return float3x3(
		cp * cy,	-sy,	sp * cy,
		cp * sy,	cy,		sp * sy,
		-sp,		0,		cp
		);
}

float3x3 RotationMatrix_angles(float2 angles) {
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

float3x3 RotationMatrix_angles(float pitch, float yaw, float roll) {
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
float4 PackFloat(const float value) {
	const float4 bitSh = float4(256.0*256.0*256.0, 256.0*256.0, 256.0, 1.0);
	const float4 bitMsk = float4(0.0, 1.0/256.0, 1.0/256.0, 1.0/256.0);
	float4 res = frac(value * bitSh);
	res -= res.xxyz * bitMsk;
	return res;
}

// Unpacking a [0-1] float value from a 4D vector where each component was a 8-bits integer:
float UnpackFloat(const float4 value) {
	const float4 bitSh = float4(1.0/(256.0*256.0*256.0), 1.0/(256.0*256.0), 1.0/256.0, 1.0);
	return(dot(value, bitSh));
}

float2 PackFloat16(float depth) {
	depth /= 4;

	float Integer = floor(depth);
	float fraction = frac(depth);

	return float2(Integer/256, fraction);
}
float UnpackFloat16(float2 depth) {
	const float2 unpack = float2(1024.0f, 4.0f);

	return dot(unpack, depth);
}

float HardStep(float min, float max, float v) {
	return saturate((v - min) / (max - min));
}

float IsInRange(float min, float max, float v) {
	return step(v - min, 0) - step(v-max, 0);
}

float4 Clip2Screen(float4 p) {
#if G_OPENGL
	return float4((p.xyz + p.w) * 0.5, p.w);
#else
	return float4((float2(p.x + p.w, p.w - p.y) + g_sceneSize.zw * p.w) * 0.5, p.zw);
#endif
}

// wave function
float Phase_sin(float t) {
	return sin(t * 3.14 * 2);
}

float Phase_squal(float t) {
	return step(0.5, t);
}

float Phase_sawTooth(float t) {
	return t * 2 - 1;
}

float Phase_invSawTooth(float t) {
	return 1.0 - Phase_sawTooth(t);
}

half Rgb2Lum(half3 color) {
//	return dot(color, half3(0.3, 0.59,0.11));
	return dot(color, half3(0.2125, 0.7154,0.0721));
}

float4 getQuaternionFromAxis(float3 axis, float angle)
{
    float4 q;
    float r = radians(angle * 0.5f);
    float s,c;
    
    sincos(r, s, c);
    q.xyz = axis * s;
    q.w = c;
	
	return q;
}

float3 transformPointFromQuaternion(float4 q,float3 a)
{
    float4 rQ = q;
    float3 UV;
    float3 UUV;
	float3 QVec;
	QVec.xyz = rQ.xyz;
	UV = cross(QVec ,a);
	UUV = cross(QVec,UV);
	UV *= (2.0f * rQ.w);
	UUV *= 2.0f;
	return a + UV + UUV;
}

float4x4 Matrix4Multiplication(float4x4 m,float4x4 im) 
{
	float4x4 om;
	om[0][0] = m[0][0]*im[0][0] + m[1][0]*im[0][1] + m[2][0]*im[0][2] + m[3][0]*im[0][3];
	om[0][1] = m[0][1]*im[0][0] + m[1][1]*im[0][1] + m[2][1]*im[0][2] + m[3][1]*im[0][3];
	om[0][2] = m[0][2]*im[0][0] + m[1][2]*im[0][1] + m[2][2]*im[0][2] + m[3][2]*im[0][3];
	om[0][3] = m[0][3]*im[0][0] + m[1][3]*im[0][1] + m[2][3]*im[0][2] + m[3][3]*im[0][3];

	om[1][0] = m[0][0]*im[1][0] + m[1][0]*im[1][1] + m[2][0]*im[1][2] + m[3][0]*im[1][3];
	om[1][1] = m[0][1]*im[1][0] + m[1][1]*im[1][1] + m[2][1]*im[1][2] + m[3][1]*im[1][3];
	om[1][2] = m[0][2]*im[1][0] + m[1][2]*im[1][1] + m[2][2]*im[1][2] + m[3][2]*im[1][3];
	om[1][3] = m[0][3]*im[1][0] + m[1][3]*im[1][1] + m[2][3]*im[1][2] + m[3][3]*im[1][3];

	om[2][0] = m[0][0]*im[2][0] + m[1][0]*im[2][1] + m[2][0]*im[2][2] + m[3][0]*im[2][3];
	om[2][1] = m[0][1]*im[2][0] + m[1][1]*im[2][1] + m[2][1]*im[2][2] + m[3][1]*im[2][3];
	om[2][2] = m[0][2]*im[2][0] + m[1][2]*im[2][1] + m[2][2]*im[2][2] + m[3][2]*im[2][3];
	om[2][3] = m[0][3]*im[2][0] + m[1][3]*im[2][1] + m[2][3]*im[2][2] + m[3][3]*im[2][3];

	om[3][0] = m[0][0]*im[3][0] + m[1][0]*im[3][1] + m[2][0]*im[3][2] + m[3][0]*im[3][3];
	om[3][1] = m[0][1]*im[3][0] + m[1][1]*im[3][1] + m[2][1]*im[3][2] + m[3][1]*im[3][3];
	om[3][2] = m[0][2]*im[3][0] + m[1][2]*im[3][1] + m[2][2]*im[3][2] + m[3][2]*im[3][3];
	om[3][3] = m[0][3]*im[3][0] + m[1][3]*im[3][1] + m[2][3]*im[3][2] + m[3][3]*im[3][3];
	return om;
}

float4x4 getTranslateMatrix4(float x, float y, float z)
{
    float4x4 m;
	m[0][0] = 1; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
	m[1][0] = 0; m[1][1] = 1; m[1][2] = 0; m[1][3] = 0;
	m[2][0] = 0; m[2][1] = 0; m[2][2] = 1; m[2][3] = 0;
	m[3][0] = x; m[3][1] = y; m[3][2] = z; m[3][3] = 1;

	return m;
}

float4x4 getMatrix4FromQuaternion(float4 q)
{
    float4x4 m;
    float x,y,z,w;
	x = q.x;
	y = q.y;
	z = q.z;
	w = q.w;
	m[0][0] = 1 - (2*y*y + 2*z*z);
	m[0][1] = 2*x*y + 2*z*w;
	m[0][2] = 2*x*z - 2*y*w;
	m[0][3] = 0;
	m[1][0] = 2*x*y - 2*z*w;
	m[1][1] = 1 - (2*x*x + 2*z*z);
	m[1][2] = 2*y*z + 2*x*w;
	m[1][3] = 0;
	m[2][0] = 2*x*z + 2*y*w;
	m[2][1] = 2*y*z - 2*x*w;
	m[2][2] = 1 - (2*x*x + 2*y*y);
	m[2][3] = 0;
	m[3][0] = 0;
	m[3][1] = 0;
	m[3][2] = 0;
	m[3][3] = 1;
	
	return m;
}

float4x4 fromRotate(float3 pos,float3 axis,float angles,float radius)
{
    float4 q;
    float4x4 rotateMatrix;

	//q = getQuaternionFromAxis(axis,180.0f);
	
	//float3 inversePos = transformPointFromQuaternion(q,pos);

	float3 dir = float3(0.0f,1.0f,0.0f);// = pos - inversePos;
	
	float3 transPos = dir * radius;

	float4x4 transMatrix1,transMatrix2;

	float3 distance = transPos - pos;

    transMatrix1 = getTranslateMatrix4(-distance.x,-distance.y,-distance.z);
    transMatrix2 = getTranslateMatrix4(distance.x,distance.y,distance.z);

    q = getQuaternionFromAxis(axis,angles);
	
	rotateMatrix = getMatrix4FromQuaternion(q);
	
	float4x4 resultMat;
	float4x4 realRotateMat;
		
	realRotateMat = Matrix4Multiplication(rotateMatrix,transMatrix2);
	resultMat     = Matrix4Multiplication(transMatrix1,realRotateMat);
         
	return resultMat;
}

float4 transformPoint4FromMatrix4(float4x4 m,float4 p)
{
	float4 op;
	op.x = p.x * m[0][0] + p.y * m[1][0] + p.z * m[2][0] + p.w * m[3][0];
	op.y = p.x * m[0][1] + p.y * m[1][1] + p.z * m[2][1] + p.w * m[3][1];
	op.z = p.x * m[0][2] + p.y * m[1][2] + p.z * m[2][2] + p.w * m[3][2];
	op.w = p.x * m[0][3] + p.y * m[1][3] + p.z * m[2][3] + p.w * m[3][3];

	return op;
}

float3 transformPoint3FromMatrix4(float4x4 m,float3 p)
{
	float4 temp = transformPoint4FromMatrix4(m,float4(p.x,p.y,p.z,1.0f));
	temp /= temp.w;
	return temp.xyz;
}


float4 normalizeQuaternion(float4 q)
{
    float4 rQ;
    
    float length = sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    float scale  = 1.0f / length;
    
    rQ = q * scale;
    
    return rQ;
}

float4 fromVectorToVector(float3 from,float3 dest,float3 fallbackAxis)
{
    float4 q;
	// Copy, since cannot modify local
	float3 v0 = from;
	float3 v1 = dest;
	v0 = normalize(v0);
	v1 = normalize(v1);

	float d = dot(v0 , v1);
	// If dot == 1, vectors are the same
	if (d >= 1.0f)
	{
		q = float4(1.0f,0.0f,0.0f,0.0f);
	}
	if (d < (1e-6f - 1.0f))
	{
		if (fallbackAxis.x != 0.0f)
		{
		    if (fallbackAxis.y != 0.0f)
		    {
		        if (fallbackAxis.z != 0.0f)
		        {
		            //rotate 180 Quaternions about the fallback axis
		            q = getQuaternionFromAxis(fallbackAxis,180.0f);
		        }
		    }
		}
		else
		{
			// Generate an axis
			float3 axis = cross(float3(1.0f,0.0f,0.0f),v0);
			if (length(axis) == 0.0f) // pick another if colinear
				axis = cross(float3(0.0f,1.0f,0.0f),v0);
			axis = normalize(axis);
            q = getQuaternionFromAxis(axis,180.0f);
		}
	}
	else
	{
		float s = sqrt((1+d)*2);
		float invs = 1 / s;

		float3 c = cross(v0 , v1);
                
		q.x = c.x * invs;
		q.y = c.y * invs;
		q.z = c.z * invs;
		q.w = s * 0.5;
        
		normalize(q);
	}
	
	return q;
}		

half Dif2Spec(half3 color) {
#if 0
	half result = Rgb2Lum(color);
	return saturate(result * 2 - 0.5);
#else
	return Rgb2Lum(color);
#endif
}