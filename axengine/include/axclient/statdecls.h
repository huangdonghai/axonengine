/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


// render module statistic decls
// AX_STATDECL(name, desc, autoreset);
AX_STATDECL(stat_fps , "FPS"							, false);
AX_STATDECL(stat_frameTime , "Frame Time"					, false);
AX_STATDECL(stat_cacheTime , "Cache Time"					, false);
AX_STATDECL(stat_frontendTime , "Frame Frontend Time"			, false);
AX_STATDECL(stat_backendTime , "Frame Backend Time"			, false);
AX_STATDECL(stat_staticsTime , "Static Draw Time"			, false);
AX_STATDECL(stat_terrainGenPrimsTime , "Terrain Generate Prims Time"	, true);
AX_STATDECL(stat_findShaderTime , "Frame Find Shader Time"		, true);
AX_STATDECL(stat_shaderParamUpdated , "Shader Param Updated"		, true);
AX_STATDECL(stat_shaderParamCached , "Shader Param Cached"			, true);
AX_STATDECL(stat_shaderParamNotused , "Shader Param Notused"		, true);
AX_STATDECL(stat_terrainVerts , "Terrain Vertexes"			, false);
AX_STATDECL(stat_terrainTris , "Terrain Tris"				, false);
AX_STATDECL(stat_chunkUpdated , "Terrain Chunks updated"		, false);
AX_STATDECL(stat_numElements , "Elements"					, true);
AX_STATDECL(stat_numDrawElements , "DrawElements"				, true);
AX_STATDECL(stat_numTerrainElements , "Terrain Elements"			, true);
AX_STATDECL(stat_numTerrainDrawElements , "Terrain DrawElements"		, true);
AX_STATDECL(stat_numTerrainLayeredElements , "Terrain Layered Elements"	, true);
AX_STATDECL(stat_numTerrainLayeredDrawElements, "Terrain Layered DrawElements", true);
AX_STATDECL(stat_numTextElements , "Text Elements"				, true);
AX_STATDECL(stat_numTextDrawElements , "Text DrawElements"			, true);
AX_STATDECL(stat_numVertexBuffers , "VertexBuffers"				, false);
AX_STATDECL(stat_numIndexBuffers , "IndexBuffers"				, false);
AX_STATDECL(stat_vertexBufferMemory , "VertexBuffer Memory"			, false);
AX_STATDECL(stat_indexBufferMemory , "IndexBuffer Memory"			, false);
AX_STATDECL(stat_dynamicVBsize , "Dynamic VB size"				, false);
AX_STATDECL(stat_usedVBsize , "Used Dynamic VB size"		, false);
AX_STATDECL(stat_instanceBufSize , "Instance Buffer size"		, false);
AX_STATDECL(stat_usedInstanceBufSize , "Used Instance Buffer size"	, false);
AX_STATDECL(stat_dynamicIBsize , "Dynamic IB size"				, false);
AX_STATDECL(stat_usedIBsize , "Used Dynamic IB size"		, false);
AX_STATDECL(stat_numTextures , "Textures"					, false);
AX_STATDECL(stat_textureMemory , "Texture Memory"				, false);
AX_STATDECL(stat_numMaterials , "Materials"					, false);
AX_STATDECL(stat_numShaders , "Shaders"						, false);
AX_STATDECL(stat_staticPrims , "Static Prims"				, false);
AX_STATDECL(stat_framePrims , "Frame Prims"					, false);
AX_STATDECL(stat_changedPrims , "Changed Prims"				, true);
AX_STATDECL(stat_frustumCullCall , "Frustum Cull Call"			, true);
AX_STATDECL(stat_worldGlobalLights , "World Number Global Lights"	, false);
AX_STATDECL(stat_worldLights , "World Number Lights"			, false);
AX_STATDECL(stat_worldActors , "World Number Actors"			, false);
AX_STATDECL(stat_worldInteractions , "World Number Interactions"	, false);
AX_STATDECL(stat_exposure , "Exposure"					, false);
AX_STATDECL(stat_csmSplit0Dist , "Cascade Shadowmap Dist 0"	, false);
AX_STATDECL(stat_csmSplit1Dist , "Cascade Shadowmap Dist 1"	, false);
AX_STATDECL(stat_csmSplit2Dist , "Cascade Shadowmap Dist 2"	, false);
AX_STATDECL(stat_csmSplit3Dist , "Cascade Shadowmap Dist 3"	, false);
AX_STATDECL(stat_csmPassed , "CSM Cull Passed"				, true);
AX_STATDECL(stat_csmCulled , "CSM Culled"					, true);
AX_STATDECL(stat_numGrasses , "Grasses Rendered"			, false);
AX_STATDECL(stat_numVisQuery , "Visible Query"				, false);
AX_STATDECL(stat_numShadowQuery , "Shadow Query"				, false);
AX_STATDECL(stat_instancedIA , "Instanced IA"				, true);
AX_STATDECL(stat_shadowPoolSize , "Shadow Pool Size"			, false);
AX_STATDECL(stat_shadowPoolUsed , "Shadow Pool Used"			, false);
AX_STATDECL(stat_shadowPoolUpdate , "Shadow Pool Update"			, true);
AX_STATDECL(stat_shadowPoolFreed , "Shadow Pool Freed"			, false);
