/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "shadow.fxh"

struct LToutput {
	half3 final;
	half3 ambient;
	half3 diffuse;
	half3 specular;
};

struct LightParams {
	float3		worldpos;
	float4		screenTc;
	half3		normal;

	// K
	half4		Ca;		// color of ambient, most of time same with diffuse map
	half3		Cd;		// color of diffuse map
	half3		Cs;		// color of specular map

	// specular parameter
	bool		calcSpecular;
	half3		viewDir;
	half		shiness;

	// fog from vertex shader
	half		fog;

	// result
	LToutput	result;
};

#if 0

int LT_getType(int id) {
	if (id == 0) {
		return G_LT_TYPE0;
	} else if (id == 1) {
		return G_LT_TYPE1;
	} else if (id == 2) {
		return G_LT_TYPE2;
	} else {
		return G_LT_TYPE3;
	}
}

float3 LT_getPos(int id) {
	if (id == 0) {
		return g_lightPos0.xyz;
	} else if (id == 1) {
		return g_lightPos1.xyz;
	} else if (id == 2) {
		return g_lightPos2.xyz;
	} else {
		return g_lightPos3.xyz;
	}
}

half3 LT_getDiffuse(int id) {
	if (id == 0) {
		return g_lightColor0.xyz;
	} else if (id == 1) {
		return g_lightColor1.xyz;
	} else if (id == 2) {
		return g_lightColor2.xyz;
	} else {
		return g_lightColor3.xyz;
	}
}

half3 LT_getSpecular(int id) {
	if (id == 0) {
		return g_lightColor0.xyz;
	} else if (id == 1) {
		return g_lightColor1.xyz;
	} else if (id == 2) {
		return g_lightColor2.xyz;
	} else {
		return g_lightColor3.xyz;
	}
}

half4 LT_getShadowChannel(int id) {
	if (id == 0) {
		return g_shadowChannel0;
	} else if (id == 1) {
		return g_shadowChannel1;
	} else if (id == 2) {
		return g_shadowChannel2;
	} else {
		return g_shadowChannel3;
	}
}
#endif

void LT_calcLight(inout LightParams self, int i) {
//	int lighttype = LT_getType(i);

//	if (lighttype == LightType_global) {
		half3 lightdir = g_globalLightPos.xyz;
		self.result.ambient = lerp(g_skyColor.xyz * self.Ca.w, g_skyColor.xyz, Unexpand(self.normal.z));
		half NdotL = dot(self.normal, lightdir);
		half selfshadow = saturate(NdotL*4.0);
		self.result.diffuse = saturate(NdotL) * selfshadow * g_globalLightColor.xyz;

		if (self.calcSpecular) {
#if 0
			half3 H = normalize(self.viewDir + lightdir);
			half NdotH = saturate(dot(self.normal, H));
			self.result.specular = pow(NdotH, g_matShiness) * g_globalLightColor.w * self.result.diffuse;
#else
			half3 R = 2 * NdotL * self.normal - lightdir;
			half RdotV = saturate(dot(self.viewDir, R));
			self.result.specular = pow(RdotV, g_matShiness) * g_globalLightColor.w * self.result.diffuse;
#endif
		}

// 	} else if (lighttype == LightType_point) {
// 
// 	} else if (lighttype == LightType_spot) {
// 
// 	}
}

#if 0
half4 LT_getShadowMask(float2 tc) {
	if (!G_LT_HAVESHADOW) {
		return 1;
	}

	return tex2D(g_shadowMask, tc);
}

half4 LT_getShadowMask(float4 tc) {
	if (!G_LT_HAVESHADOW) {
		return half4(1,1,1,1);
	}

	return tex2Dproj(g_shadowMask, tc.xyzw).xyzw;
}
#endif

half4 LT_getLightbufer(float4 screenTc) {
	if (G_REFLECTION) {
		return half4(1,1,1,1);
	}

	return half4(tex2Dproj(g_lightBuffer, screenTc));
}

#if 0
half LT_getShadow(inout LightParams self, int i) {
	if (!G_LT_HAVESHADOW) {
		return 1;
	}

	return LT_getShadowMask(self.screenTc);
}
#endif

half3 LT_calcAllLights(inout LightParams self) {
	LToutput result = (LToutput)0;

#if G_DEBUG == DEBUG_DIFFUSEMAP
	return self.Cd;
#endif

#if G_DEBUG == DEBUG_NORMALMAP
	return self.normal;
#endif

#if G_DEBUG == DEBUG_SPECULARMAP
	return self.Cs;
#endif


#if G_REFLECTION
	LT_calcLight(self, 0);
	result.diffuse += self.result.diffuse;
	if (self.calcSpecular) {
		result.specular += self.result.specular;
	}
#else

	half4 lightbuf = LT_getLightbufer(self.screenTc) * 4;
	result.diffuse += lightbuf.xyz;
	result.specular += lightbuf.w;
#endif
	result.final = (result.diffuse) * g_matDiffuse.xyz * self.Cd;

#if G_DEBUG == DEBUG_AMBIENT
//	return result.ambient* g_matAmbient.xyz;
#endif

#if G_DEBUG == DEBUG_DIFFUSE
	return result.diffuse* g_matDiffuse.xyz;
#endif

#if G_DEBUG == DEBUG_SPECULAR
	return result.specular* g_matSpecular.xyz;
#endif

	if (self.calcSpecular) {
		result.final += (result.specular) * self.Cs * g_matSpecular.xyz;
	}
//	return lightbuf.w;

	self.result = result;

#if G_FOG
	result.final = lerp(g_fogParams.xyz, result.final, self.fog);
#if G_DEBUG == DEBUG_FOG
	return self.fog;
#endif
#endif

	return FinalOutput(result.final);
}
