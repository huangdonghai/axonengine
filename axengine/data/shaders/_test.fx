//-----------------------------------------------------------------------------
// Path:  SDK\MEDIA\programs\PracticalCubeShadowMaps
// File:  PracticalCubeShadowMaps.fx
// 
// Copyright NVIDIA Corporation 2004
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
//-----------------------------------------------------------------------------

float4x4 cWorld;                        // object->world space matrix
float4x4 cWorldIT;                      // inverse transpose of object->world matrix
float4x4 cScreenViewProjection;         // world->clip space matrix

sampler  ColorSampler : register(s1);

static const float4 cAmbient = float4(0.3, 0.3, 0.3, 0);

//-----------------------------------------------------------------------------

struct VS_INPUT {
	float4 Position : POSITION;
	float3 Normal   : NORMAL;
	float2 Texture  : TEXCOORD0;
	float3x3 TangentSpace  : TEXCOORD1;
};

struct QUAD_INPUT {
	float4 pos : POSITION;
};

struct VS_OUTPUT {
	float4 Position  : POSITION;
	float2 Texture   : TEXCOORD0;
};

//----------------------------------------------------------------------------
VS_OUTPUT Transform_VS(VS_INPUT IN)
{
	VS_OUTPUT OUT = (VS_OUTPUT)0;
	float4 positionWorld = mul(IN.Position, cWorld);
	OUT.Position = mul(positionWorld, cScreenViewProjection);
	OUT.Texture  = IN.Texture;
	return OUT;
}


float4 Ambient_PS( VS_OUTPUT IN ) : COLOR
{
	return tex2D( ColorSampler, IN.Texture ) * cAmbient;
}

VS_OUTPUT GreenQuad_VS( QUAD_INPUT IN )
{
	VS_OUTPUT OUT = (VS_OUTPUT)0;
	OUT.Position = IN.pos;
	return OUT;
}

float4 GreenQuad_PS( VS_OUTPUT IN ) : COLOR
{
	return float4(0,1,0,1);
}

//-----------------------------------------------------------------------------
//  Techniques
//

technique TransformAmbient
{
	pass P0
	{
		AlphaBlendEnable = false;
		AlphaTestEnable = false;
		CullMode = None;
		ZEnable = true;
		ZWriteEnable = true;
		ZFunc = less;
		Lighting = false;
		ColorWriteEnable = RED|GREEN|BLUE|ALPHA;        

		VertexShader = compile vs_2_0 Transform_VS();
		PixelShader = compile ps_2_0 Ambient_PS();
	}
}

technique GreenQuad
{
	pass P0
	{
		AlphaBlendEnable = true;
		SrcBlend = DestColor;
		DestBlend = Zero;
		AlphaTestEnable = false;
		CullMode = None;
		ZEnable = false;
		ZWriteEnable = false;
		Lighting = false;
		ColorWriteEnable = RED|GREEN|BLUE|ALPHA;        

		VertexShader = compile vs_1_1 GreenQuad_VS();
		PixelShader = compile ps_1_1  GreenQuad_PS();        
	}
}

//-----------------------------------------------------------------------------

