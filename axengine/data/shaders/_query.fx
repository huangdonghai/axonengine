/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "common.fxh"


float4 VP_main(MeshVertex IN) : POSITION
{
	return VP_worldToClip(IN.position);
}

half4 FP_main(float4 IN : POSITION) : COLOR
{
	return half4(1,0,0,1);
}


technique Main {
    pass p0 {
        VertexShader = compile VS_3_0 VP_main();
		PixelShader = compile PS_3_0 FP_main();
    }
}

