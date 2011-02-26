/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "common.fxh"

VertexOut VP_main(MeshVertex IN)
{
    VertexOut OUT = (VertexOut)0;

#if !G_D3D
	OUT.color = IN.color.bgra;
#else
	OUT.color = IN.color;
#endif

	OUT.worldPos = VP_modelToWorld(IN, IN.position);
	OUT.hpos = VP_worldToClip(OUT.worldPos);

	VP_final(IN, OUT);

    return OUT;
}

half4 FP_main(VertexOut IN) : COLOR
{
	half4 c = IN.color;

	c *= FP_GetDiffuse(IN);

	return c;
}

technique Main {
    pass p0 {
        VertexShader = compile VS_3_0 VP_main();
		PixelShader = compile PS_3_0 FP_main();
    }
}

/***************************** eof ***/
