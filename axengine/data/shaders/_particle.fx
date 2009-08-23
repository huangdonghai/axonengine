/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "affectorinc.fxh"

float Script : STANDARDSGLOBAL <
	// sort hint
	int		SortHint = SortHint_aboveWater;

	// technique
	string TechniqueZpass = "";
	string TechniqueShadowGen = "";
	string TechniqueMain = "main";
	string TechniqueGlow = "";
	string TechniqueLayer = "";
> = 0.8;



float  value[35];
	
/*********** Generic Vertex Shader ******/

VertexOut VP_main(VertexIn IN) {
    VertexOut OUT = (VertexOut)0;
    
    float2 bbSize = float2(value[25],value[26]);
    float3 vertical = float3(value[27],value[28],value[29]);
    float3 landScapediff = float3(value[30],value[31],value[32]);
    float  elpsedTime = value[33];
    float3 rVertical = vertical;
    float3 rLandScapediff = landScapediff;

    float2 rScale = IN.normal.xy;
    
    VertexIn saveIN = IN;
    float2   rSaveScale = rScale;
    float    changePos = 0.0f;
    float2   startUV = float2(0.0f,0.0f);
    float    uvOffset = 1.0f;
    for(int i = 0; i < S_NUM_AFFECTORS; i++) 
    {
        int macro = getMacro(i); 
		if (macro == FlickerAffector) 
		{
		    float durationTime = value[0];
            float transitionTime = value[1];
		    float cycleTime = 2.0f * durationTime + 2.0f * transitionTime;
            float timePasted = IN.normal.z / cycleTime;
            float cutTime = (timePasted - frac(timePasted)) * cycleTime;
            timePasted = IN.normal.z - cutTime + elpsedTime;
    
            if (timePasted > durationTime)
            {
                if (transitionTime > 0)
                {
                    float startAlpha = IN.color.w;
                    float endAlpha   = 0.0f;
                    float scale;
        
                    scale = (timePasted - durationTime) / transitionTime;
        
                    if (timePasted > (durationTime + transitionTime))
                    {
                        if (timePasted < (2.0f * durationTime + transitionTime))
                        {
                            IN.color.w = 1.0f;
                        }
                        else
                        {
                            startAlpha = 0.0f;
                            endAlpha   = IN.color.w;
                            scale = (timePasted - (2.0f * durationTime + transitionTime)) / (transitionTime);
                        }
                    }
  
                    IN.color.w = startAlpha + scale * (endAlpha - startAlpha);
                }
            } 
            else 
            {
                 IN.color.w = 0.0f;
            }
        
        } 
		else if (macro == ScaleAffector) 
		{
		    float3 scaleSpeed = float3(value[3],value[4],value[5]);
		    rScale = rScale + scaleSpeed.xy * (IN.normal.z + elpsedTime);
		}
		else if (macro == RotationAffector)
		{  
		    float radius = IN.st.y; 
            if (radius < 0.01f)
            {
                float4 q = getQuaternionFromAxis(float3(1.0f,0.0f,0.0f),IN.st2.x);
                rVertical = transformPointFromQuaternion(q,rVertical);
                rLandScapediff = transformPointFromQuaternion(q,rLandScapediff);
                rVertical = normalize(rVertical);
                rLandScapediff = normalize(rLandScapediff);
            }
            else
            {
                float4x4 rotate = fromRotate(IN.xyz,float3(1.0f,0.0f,0.0f),IN.st2.x,IN.st2.y);
                IN.xyz = transformPoint3FromMatrix4(rotate,IN.xyz);
            }
            changePos = 1.0f;
		}
		else if (macro == LinearForceAffector)
		{
		    float3 force = float3(value[6],value[7],value[8]);
		    float3 deltaVal = 0.5f * force * (IN.normal.z + elpsedTime) * (IN.normal.z + elpsedTime);
            IN.binormal += force * (IN.normal.z + elpsedTime); 
            IN.xyz += deltaVal;
            changePos = 1.0f;
		}
		else if (macro == PlaneCollisionAffector)
		{
		    float3 planePoint = float3(value[9],value[10],value[11]);
            float3 planeNormal = float3(value[12],value[13],value[14]);
            float  bounce = value[15];
		    float planeDistance = - dot(planeNormal , planePoint) * length(planeNormal);
            float3 direction = IN.binormal * elpsedTime;
    
            float jdgVal = dot(planeNormal,(IN.xyz + direction)) + planeDistance;
            if (jdgVal <= 0.0)
	        {
		        float a = dot(planeNormal ,IN.xyz) + planeDistance;
		        // for intersection point
	        	float3 directionPart = direction * (- a / dot(direction , planeNormal));
		        // set new position
		        IN.xyz = (IN.xyz + (directionPart)) + (((directionPart) - direction) * bounce);
		        // reflect direction vector
		        IN.binormal = (IN.binormal - (planeNormal * (2.0 * dot(IN.binormal , planeNormal)))) * bounce;
		    }
		    
		    changePos = 1.0f;
        }
        else if (macro == FaceToAxis)
        {
            float4 fQ = fromVectorToVector(vertical,IN.binormal,float3(0.0f,0.0f,0.0f));
            rVertical = transformPointFromQuaternion(fQ,rVertical);
    
            rLandScapediff = transformPointFromQuaternion(fQ,rLandScapediff);
    
            if (IN.binormal.x > 0.0f)
            {
                rLandScapediff = -rLandScapediff;
                rLandScapediff.z = 0.0f;
            }
            rVertical = normalize(rVertical);
            rLandScapediff = normalize(rLandScapediff);
        }
        else if (macro == AlphaFaderAffector)
        {
            float fadeInTime = value[16];
            float fadeOutTime = value[17];
            float curTime =  (IN.normal.z + elpsedTime);
            float fadeOutStartTime = IN.tangent.z - fadeOutTime;
            float scale;
            float alpha = IN.color.w;
            if (curTime < fadeInTime)
            {
                if (fadeInTime > 0.0f)
                {
                    scale = curTime / fadeInTime;
                
                    IN.color.w = scale;
                }
               
            }
            else
            {
                IN.color.w = 1.0f;
            }
           
            if (curTime > fadeOutStartTime)
            {
                if (fadeOutTime > 0.0f)
                {
                    float remainTime = (IN.tangent.z - curTime);
                   
                    scale = (IN.tangent.z - curTime) / fadeOutTime;
            
                    IN.color.w = scale;                   
                }
                else
                {
                    IN.color.w = 0.0f;
                }
            }
            
            IN.color.w = clamp(IN.color.w,0.0f,1.0f);
        }
        else if (macro == ColorFaderAffector)
        {
            float4 colorAdjust = float4(value[18],value[19],value[20],value[21]);
            float4 colorFade = colorAdjust * IN.normal.z;
          
            IN.color += colorFade;
            
            IN.color.x = clamp(IN.color.x,0.0f,1.0f);
            IN.color.y = clamp(IN.color.y,0.0f,1.0f);
            IN.color.z = clamp(IN.color.z,0.0f,1.0f);
            IN.color.w = clamp(IN.color.w,0.0f,1.0f);
        }
        else
        {
            float  animationSetPictureNum = value[22];
            float  aniPictureNumPerLine = value[23]; 
            float  startPictureIndex = value[24];
            float  isLoop = value[34];
            if (IN.normal.z > IN.binormal.y)
			{
                float pastedTime = IN.normal.z - IN.binormal.y;
				float oneFrameTime = 1.0f / IN.binormal.z;
				float oneAnimTime = (oneFrameTime * animationSetPictureNum);
				float playNum = IN.normal.z / oneAnimTime;
				playNum = playNum - frac(playNum);

				if (playNum > 1 && isLoop == 0.0f)
			    {
					pastedTime = playNum * oneAnimTime;
				}
				else
				{
                    pastedTime = (pastedTime - playNum * oneAnimTime);
				}

                float setNum = (pastedTime / oneFrameTime);
                setNum = setNum - frac(setNum);
				float curAnimSetPictureIndex = setNum + startPictureIndex;
				float startIndex = (IN.binormal.x - 1) * animationSetPictureNum + curAnimSetPictureIndex; 
                 
                float uIndex = startIndex / aniPictureNumPerLine;
                uIndex = frac(uIndex);
                uIndex = uIndex * aniPictureNumPerLine;
                uIndex = uIndex - frac(uIndex);
                float vIndex = startIndex / aniPictureNumPerLine;
                vIndex = vIndex - frac(vIndex);
                uvOffset = 1.0f / (aniPictureNumPerLine);
				startUV.y = uIndex * uvOffset;
				startUV.x = vIndex * uvOffset;
				
				IN.st = float2(startUV.x + (-0.5f - IN.tangent.y) * uvOffset,startUV.y + (-0.5f + IN.tangent.x) * uvOffset);
			}
        }
    }

    if (IN.tangent.z == -1.0f)
    {         
        IN = saveIN;
        rScale = rSaveScale;
    }
    
    IN.xyz = IN.xyz + IN.tangent.x * rScale.x * bbSize[0] * rLandScapediff + IN.tangent.y * rScale.y * bbSize[1] * rVertical;
    
    //if (IN.tangent.z == -2.0f)
    //    IN.xyz = float3(1e08,1e08,1e08);
        
	OUT.color = IN.color;

	OUT.worldPos = VP_modelToWorld(IN, IN.xyz);
	OUT.hpos = VP_worldToClip(OUT.worldPos);

	VP_final(IN, OUT);

	return OUT;
}

/********* pixel shaders ********/
half4 FP_main(VertexOut IN) : COLOR {
	half4 c = IN.color;

#if G_HAVE_DIFFUSE
	c *= tex2D(g_diffuseMap, IN.streamTc.xy);
#endif

	return c;
}

//////////////////////////////////////////////////////////////////////
// TECHNIQUES ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


technique main {
    pass p0 {
        VERTEXPROGRAM = compile VP_3_0 VP_main();
		FRAGMENTPROGRAM = compile FP_3_0 FP_main();

	    DEPTHTEST = true;
		DEPTHMASK = false;
		CULL_ENABLED;
		BLEND_ADD;
    }
}

/***************************** eof ***/