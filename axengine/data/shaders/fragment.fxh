/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



static const float MAX_EXPOSURE = 4.0;

float4 FP_texgen(int texgen, VertexOut vertOut) {
	float3 viewDir = g_cameraPos.xyz - vertOut.worldPos;

	if (texgen == TexGen_default) {
		return float4(vertOut.streamTc.xy, 0, 1);
	} else if (texgen == TexGen_baseTc) {
		return float4(vertOut.streamTc.xy, 0, 1);
	} else if (texgen == TexGen_lightmapTc) {
		return float4(vertOut.streamTc.zw, 0, 1);
	} else if (texgen == TexGen_vertex) {
		return float4(vertOut.worldPos, 1);
	} else if (texgen == TexGen_normal) {
		return float4(vertOut.normal, 1);
	} else if (texgen == TexGen_reflect) {
		return float4(reflect(viewDir, vertOut.normal), 1);
	} else if (texgen == TexGen_refract) {
		return float4(refract(viewDir, vertOut.normal, 1.3), 1);
	} else if (texgen == TexGen_sphereMap) {
		return vertOut.streamTc;		// todo
	}

}

float4 FP_texgen(int texgen, float4x4 mtx, VertexOut vertOut) {
	float4 result = FP_texgen(texgen, vertOut);

	if (texgen != TexGen_default) {
		result = mul(mtx, result);
	}

	return result;
}

// fragment shader library
struct FragParams {
	float4	diffuseTc;
	float4	specularTc;
	float4	normalTc;
	float4	detailTc;
	float4	emissionTc;
};

half4 FP_GetDiffuse(VertexOut vo) {
#if !G_HAVE_DIFFUSE
	return 1;
#else
	return tex2D(g_diffuseMap, vo.streamTc.xy);
#endif
}

half3 FP_GetNormal(float3 n, float3 t, float3 b, float2 tc) {
#if NO_NORMALMAPPING
	return normalize(n);
#else
	half3x3 axis = half3x3(t, b, n);
#if G_HAVE_NORMAL
	half3 N = GetNormal(g_normalMap, tc).rgb;
#else
	half3 N = half3(0,0,1);
#endif
#if G_HAVE_DETAIL_NORMAL
	N.rgb += GetNormal(g_detailNormalMap, tc * g_layerScale).rgb;
#endif
	N.rgb = normalize(mul(N.rgb, axis));
	return N;
#endif
}

half3 FinalOutput(half3 input) {
#if 0 && !G_HDR
	return input;
#endif

	return input;
//	return input * 0.25;
//	half3 c = input * g_exposure.y;
//	return c / (1 + c);
//	return 1 - exp2(-input * g_exposure.y * 3);
}

half3 HdrDecode(half3 input) {
	return -log2(1- input);
}

float GetDepth(half4 gbuffer) {
	return gbuffer.w;
}