/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


// render module cvar definitions
// AX_CVARDECL(name, defaultstring, flags)
AX_CVARDECL(r_vsync, "0", Cvar::Archive);
AX_CVARDECL(r_hdr, "0", Cvar::Archive);
AX_CVARDECL(r_bloom, "0", Cvar::Archive);
AX_CVARDECL(r_fog, "1", Cvar::Archive);
AX_CVARDECL(r_motionBlur, "0", Cvar::Archive);
AX_CVARDECL(r_exposure, "0", Cvar::Cheat);
AX_CVARDECL(r_wireframe, "0", Cvar::Cheat);
AX_CVARDECL(r_shaderDebug, "0", Cvar::Cheat);
AX_CVARDECL(r_debugNoShader, "0", Cvar::Cheat);
AX_CVARDECL(r_showTangents, "0", Cvar::Cheat);
AX_CVARDECL(r_showNormal, "0", Cvar::Cheat);
AX_CVARDECL(r_showStat, "0", Cvar::Cheat);
AX_CVARDECL(r_multiThread, "1", Cvar::Archive | Cvar::Latch);
AX_CVARDECL(r_bumpmap, "1", Cvar::Archive);
AX_CVARDECL(r_specular, "1", Cvar::Archive);
AX_CVARDECL(r_detail, "1", Cvar::Archive);
AX_CVARDECL(r_lightmap, "1", Cvar::Archive);
AX_CVARDECL(r_lockLOD, "0", Cvar::Cheat);
AX_CVARDECL(r_viewDistCull, "24", Cvar::Archive);
AX_CVARDECL(r_hardwareQuery, "2", Cvar::Archive);
AX_CVARDECL(r_cullActor, "1", Cvar::Cheat);
AX_CVARDECL(image_mip, "0", Cvar::Archive);

AX_CVARDECL(r_terrain, "1", Cvar::Cheat);
AX_CVARDECL(r_terrainLOD, "10", Cvar::Archive);
AX_CVARDECL(r_terrainDetailDist, "256", Cvar::Archive);
AX_CVARDECL(r_terrainShadow, "0", Cvar::Archive);
AX_CVARDECL(r_terrainUseZonePrim, "1", Cvar::Cheat);
AX_CVARDECL(r_terrainLayerCombine, "1", Cvar::Cheat);

AX_CVARDECL(r_showMemoryInfo, "0", Cvar::Cheat);
AX_CVARDECL(r_frustumCull, "1", Cvar::Cheat);
AX_CVARDECL(r_ignorMesh, "0", Cvar::Cheat);
AX_CVARDECL(r_useUi, "1", Cvar::Cheat);
AX_CVARDECL(r_shaderQuality, "1", Cvar::Archive);

// shadow
AX_CVARDECL(r_csmClipCamera, "1", Cvar::Cheat);
AX_CVARDECL(r_csmSplits, "4", Cvar::Archive);
AX_CVARDECL(r_csmLambda, "0.9", Cvar::Archive);
AX_CVARDECL(r_csmRange, "256", Cvar::Archive);
AX_CVARDECL(r_csmAtlas, "1", Cvar::Cheat | Cvar::Archive);
AX_CVARDECL(r_csmCull, "1", Cvar::Cheat);
AX_CVARDECL(r_shadow, "1", Cvar::Archive);
AX_CVARDECL(r_shadowMapSize, "1024", Cvar::Archive);
AX_CVARDECL(r_shadowFormat, "1", Cvar::Archive);
AX_CVARDECL(r_shadowGen, "1", Cvar::Cheat);
AX_CVARDECL(r_shadowMask, "1", Cvar::Cheat);
AX_CVARDECL(gl_shadowOffsetFactor, "1", Cvar::Archive);
AX_CVARDECL(gl_shadowOffsetUnits, "1", Cvar::Archive);
AX_CVARDECL(r_vsm, "1", Cvar::Archive);
AX_CVARDECL(r_shadowBlur, "0", Cvar::Archive);
AX_CVARDECL(r_shadowBlurSize, "7", Cvar::Archive);
AX_CVARDECL(r_showShadowMask, "0", Cvar::Cheat);
AX_CVARDECL(r_showShadowMap, "0", Cvar::Cheat);
AX_CVARDECL(r_showLightBuf, "0", Cvar::Cheat);
AX_CVARDECL(r_lightBuf, "1", Cvar::Cheat);
AX_CVARDECL(r_helper, "1", Cvar::Cheat|Cvar::Archive);
AX_CVARDECL(r_showPhysics, "0", Cvar::Cheat|Cvar::Archive);
AX_CVARDECL(r_showNode, "0", Cvar::Cheat|Cvar::Archive);
AX_CVARDECL(r_shadowPoolSize, "32", Cvar::Archive);

AX_CVARDECL(r_useDepthBoundsExt, "1", Cvar::Cheat);
AX_CVARDECL(r_useMaskVolumeFront, "1", Cvar::Cheat);
AX_CVARDECL(r_znearMin, "0.125", Cvar::Archive);
AX_CVARDECL(r_geoInstancing, "2", Cvar::Archive);
AX_CVARDECL(r_reflection, "0", Cvar::Archive);
AX_CVARDECL(r_refraction, "1", Cvar::Archive);
AX_CVARDECL(r_sky, "1", Cvar::Cheat);
AX_CVARDECL(r_water, "1", Cvar::Cheat);
AX_CVARDECL(r_entity, "1", Cvar::Cheat);
AX_CVARDECL(r_speedtree, "1", Cvar::Cheat);
AX_CVARDECL(r_font, "1", Cvar::Cheat);
AX_CVARDECL(r_nearLod, "50", Cvar::Archive);
AX_CVARDECL(r_farLod, "1000", Cvar::Archive);
AX_CVARDECL(r_forceLod, "-1", Cvar::Cheat);
AX_CVARDECL(r_forceUpdateMaterialMacro, "0", Cvar::Cheat);
AX_CVARDECL(r_framebuffer, "1", Cvar::Cheat);
AX_CVARDECL(r_nulldraw, "0", Cvar::Cheat);

AX_CVARDECL(r_grass, "1", Cvar::Cheat);
AX_CVARDECL(r_grassDistance, "35", Cvar::Cheat);
//AX_CVARDECL(r_grassAdjust, "10", Cvar::Cheat);

AX_CVARDECL(r_driver, "d3d9", Cvar::Latch | Cvar::Archive);

AX_CVARDECL(r_msaa, "0", Cvar::Latch | Cvar::Archive);
