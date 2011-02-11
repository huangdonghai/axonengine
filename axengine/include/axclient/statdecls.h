/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


// render module statistic decls
// AX_STATDECL(name, desc, autoreset);
AX_STATDECL(stat_fps, "FPS", Stat::F_Int);
AX_STATDECL(stat_frameTime, "Frame Time", Stat::F_Int);
AX_STATDECL(stat_cacheTime, "Cache Time", Stat::F_Int);
AX_STATDECL(stat_frontendTime, "Frame Frontend Time", Stat::F_Int);
AX_STATDECL(stat_backendTime, "Frame Backend Time", Stat::F_Int);
AX_STATDECL(stat_staticsTime, "Statics Draw Time", Stat::F_Int);
AX_STATDECL(stat_frameRenderCommand, "Frame Render Command", Stat::F_Int);
AX_STATDECL(stat_frameRingBufferSize, "Frame Ring Buffer Size", Stat::F_Int);
AX_STATDECL(stat_terrainGenPrimsTime, "Terrain Generate Prims Time", Stat::F_Int | Stat::F_AutoReset);
AX_STATDECL(stat_findShaderTime, "Frame Find Shader Time", Stat::F_Int | Stat::F_AutoReset);
AX_STATDECL(stat_terrainVerts, "Terrain Vertexes", Stat::F_Int);
AX_STATDECL(stat_terrainTris, "Terrain Tris", Stat::F_Int);
AX_STATDECL(stat_chunkUpdated, "Terrain Chunks updated", Stat::F_Int);
AX_STATDECL(stat_numElements, "Elements", Stat::F_Int | Stat::F_AutoReset);
AX_STATDECL(stat_numDrawElements, "DrawElements", Stat::F_Int | Stat::F_AutoReset);
AX_STATDECL(stat_numTerrainElements, "Terrain Elements", Stat::F_Int | Stat::F_AutoReset);
AX_STATDECL(stat_numTerrainDrawElements, "Terrain DrawElements", Stat::F_Int | Stat::F_AutoReset);
AX_STATDECL(stat_numTerrainLayeredElements, "Terrain Layered Elements", Stat::F_Int | Stat::F_AutoReset);
AX_STATDECL(stat_numTerrainLayeredDrawElements, "Terrain Layered DrawElements", Stat::F_Int | Stat::F_AutoReset);
AX_STATDECL(stat_numTextElements, "Text Elements", Stat::F_Int | Stat::F_AutoReset);
AX_STATDECL(stat_numTextDrawElements, "Text DrawElements", Stat::F_Int | Stat::F_AutoReset);
AX_STATDECL(stat_numVertexBuffers, "VertexBuffers", Stat::F_Int);
AX_STATDECL(stat_numIndexBuffers, "IndexBuffers", Stat::F_Int);
AX_STATDECL(stat_vertexBufferMemory, "VertexBuffer Memory", Stat::F_Int);
AX_STATDECL(stat_indexBufferMemory, "IndexBuffer Memory", Stat::F_Int);
AX_STATDECL(stat_dynamicVBsize, "Dynamic VB size", Stat::F_Int);
AX_STATDECL(stat_usedVBsize, "Used Dynamic VB size", Stat::F_Int);
AX_STATDECL(stat_instanceBufSize, "Instance Buffer size", Stat::F_Int);
AX_STATDECL(stat_usedInstanceBufSize, "Used Instance Buffer size", Stat::F_Int);
AX_STATDECL(stat_dynamicIBsize, "Dynamic IB size", Stat::F_Int);
AX_STATDECL(stat_usedIBsize, "Used Dynamic IB size", Stat::F_Int);
AX_STATDECL(stat_numTextures, "Textures", Stat::F_Int);
AX_STATDECL(stat_textureMemory, "Texture Memory", Stat::F_Int);
AX_STATDECL(stat_numMaterials, "Materials", Stat::F_Int);
AX_STATDECL(stat_numShaders, "Shaders", Stat::F_Int);
AX_STATDECL(stat_staticPrims, "Static Prims", Stat::F_Int);
AX_STATDECL(stat_framePrims, "Frame Prims", Stat::F_Int);
AX_STATDECL(stat_changedPrims, "Changed Prims", Stat::F_Int | Stat::F_AutoReset);
AX_STATDECL(stat_frustumCullCall, "Frustum Cull Call", Stat::F_Int | Stat::F_AutoReset);
AX_STATDECL(stat_worldGlobalLights, "World Number Global Lights", Stat::F_Int);
AX_STATDECL(stat_worldLights, "World Number Lights", Stat::F_Int);
AX_STATDECL(stat_worldActors, "World Number Actors", Stat::F_Int);
AX_STATDECL(stat_worldInteractions, "World Number Interactions", Stat::F_Int);
AX_STATDECL(stat_csmSplit0Dist, "Cascade Shadowmap Dist 0", Stat::F_Int);
AX_STATDECL(stat_csmSplit1Dist, "Cascade Shadowmap Dist 1", Stat::F_Int);
AX_STATDECL(stat_csmSplit2Dist, "Cascade Shadowmap Dist 2", Stat::F_Int);
AX_STATDECL(stat_csmSplit3Dist, "Cascade Shadowmap Dist 3", Stat::F_Int);
AX_STATDECL(stat_csmPassed, "CSM Cull Passed", Stat::F_Int | Stat::F_AutoReset);
AX_STATDECL(stat_csmCulled, "CSM Culled", Stat::F_Int | Stat::F_AutoReset);
AX_STATDECL(stat_numGrasses, "Grasses Rendered", Stat::F_Int);
AX_STATDECL(stat_numVisQuery, "Visible Query", Stat::F_Int);
AX_STATDECL(stat_numCsmQuery, "CSM Query", Stat::F_Int);
AX_STATDECL(stat_numVisQueryCulled, "VisQuery Culled", Stat::F_Int | Stat::F_AutoReset);
AX_STATDECL(stat_instancedIA, "Instanced IA", Stat::F_Int | Stat::F_AutoReset);
AX_STATDECL(stat_shadowPoolSize, "Shadow Pool Size", Stat::F_Int);
AX_STATDECL(stat_shadowPoolUsed, "Shadow Pool Used", Stat::F_Int);
AX_STATDECL(stat_shadowPoolUpdate, "Shadow Pool Update", Stat::F_Int | Stat::F_AutoReset);
AX_STATDECL(stat_shadowPoolFreed, "Shadow Pool Freed", Stat::F_Int);
