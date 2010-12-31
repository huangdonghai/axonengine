/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

#ifdef AX_CONFIG_OPTION_USE_SPEEDTREE_40

const int NUM_WIND_MATRIX = 3;
const int NUM_LEAF_ANGLES = 8;

AX_BEGIN_NAMESPACE

TreeAsset::TreeAsset(TreeManager *forest)
{
	m_manager = forest;
	m_treeRt = new CSpeedTreeRT();

	m_branchMat = nullptr;
	m_frondMat = nullptr;
	m_leafCardMat = nullptr;
	m_leafMeshMat = nullptr;

	m_numBranchLods = 0;
	m_numFrondLods = 0;
	m_numLeafLods = 0;

	TypeZeroArray(m_branchPrims);
	TypeZeroArray(m_frondPrims);
	TypeZeroArray(m_leafPrims);
}

TreeAsset::~TreeAsset()
{
	for (int i = 0; i < MAX_LODS; i++) {
		SafeDelete(m_branchPrims[i]);
		SafeDelete(m_frondPrims[i]);
		SafeDelete(m_leafPrims[i]);
	}

	delete m_treeRt;
	m_manager->removeAsset(this);
}

bool TreeAsset::load(const std::string &filename, int seed)
{
	m_treeRt->SetNumWindMatrices(NUM_WIND_MATRIX);

	unsigned char *fbuf;
	size_t fsize;
	fsize = g_fileSystem->readFile(filename, (void**)&fbuf);

	if (!fsize || !fbuf) {
		return false;
	}

	bool v = m_treeRt->LoadTree(fbuf, (uint_t)fsize);

	if (!v) {
		return false;
	}

	// scale to meter
	float treesize, treesizevar;
	m_treeRt->GetTreeSize(treesize, treesizevar);
	treesize *= 0.3f;
	treesizevar *= 0.3f;
	m_treeRt->SetTreeSize(treesize, treesizevar);

	// make sure SpeedTreeRT generates normals
	m_treeRt->SetBranchLightingMethod(CSpeedTreeRT::LIGHT_DYNAMIC);
	m_treeRt->SetLeafLightingMethod(CSpeedTreeRT::LIGHT_DYNAMIC);
	m_treeRt->SetFrondLightingMethod(CSpeedTreeRT::LIGHT_DYNAMIC);

	// set the wind method to use the GPU
	m_treeRt->SetBranchWindMethod(CSpeedTreeRT::WIND_GPU);
	m_treeRt->SetLeafWindMethod(CSpeedTreeRT::WIND_GPU);
	m_treeRt->SetFrondWindMethod(CSpeedTreeRT::WIND_GPU);

	v = m_treeRt->Compute(nullptr, seed);

	if (!v) {
		return false;
	}

	// make the leaves rock in the wind
	m_treeRt->SetLeafRockingState(true);

	// billboard setup
	m_treeRt->SetDropToBillboard(true);

	m_filename = filename;
	m_filepath = PathUtil::getDir(m_filename);

	loadMaterials();

	buildPrimitives();

	m_key = genKey(m_filename, seed);

	return true;
}

std::string TreeAsset::getKey() const
{
	return m_key;
}

BoundingBox TreeAsset::getBoundingBox() const
{
	BoundingBox bbox;
	m_treeRt->GetBoundingBox((float*)&bbox);
	return bbox;
}

Primitives TreeAsset::getAllPrimitives(float lod) const
{
	Primitives result;

	CSpeedTreeRT::SLodValues lv;

	m_treeRt->GetLodValues(lv, lod);

	if (lv.m_nBranchActiveLod >= 0 && m_branchPrims[lv.m_nBranchActiveLod]) {
		result.push_back(m_branchPrims[lv.m_nBranchActiveLod]);
	}

	if (lv.m_nFrondActiveLod >= 0 && m_frondPrims[lv.m_nFrondActiveLod]) {
		result.push_back(m_frondPrims[lv.m_nFrondActiveLod]);
	}

	if (lv.m_anLeafActiveLods[0] >= 0 && m_leafPrims[lv.m_anLeafActiveLods[0]]) {
		result.push_back(m_leafPrims[lv.m_anLeafActiveLods[0]]);
	}

	return result;
}



static inline void setMaterialColor(Material *mat, const float *f, float scaler, float ambientscaler)
{
	Vector3 diffuse(f[0], f[1], f[2]);
	Vector3 ambient(f[3], f[4], f[5]);
	Vector3 specular(f[6], f[7], f[8]);
	Vector3 emissive(f[9], f[10], f[11]);
	float shiness = f[12];

	mat->setDiffuse(diffuse * scaler);
	mat->setSpecular(specular * scaler);
	mat->setShiness(shiness);
}

void TreeAsset::loadMaterials()
{
	CSpeedTreeRT::SMapBank sMapBank;
	CSpeedTreeRT::SLightShaderParams sShaderParams;

	m_treeRt->GetMapBank(sMapBank);
	m_treeRt->GetLightShaderParams(sShaderParams);

	std::string texname = m_filepath + PathUtil::getName(sMapBank.m_pBranchMaps[CSpeedTreeRT::TL_DIFFUSE]);
	Texture *tex = new Texture(texname);

	m_branchMat = new Material("_branch");
	m_branchMat->setTexture(MaterialTextureId::Diffuse, tex);

	setMaterialColor(m_branchMat, m_treeRt->GetBranchMaterial(), sShaderParams.m_fGlobalLightScalar * sShaderParams.m_fBranchLightScalar, sShaderParams.m_fAmbientScalar);

	texname = m_filepath + PathUtil::getName(sMapBank.m_pCompositeMaps[CSpeedTreeRT::TL_DIFFUSE]);
	tex = new Texture(texname);
	m_frondMat = new Material("_frond");
	m_frondMat->setTexture(MaterialTextureId::Diffuse, tex);

	setMaterialColor(m_frondMat, m_treeRt->GetFrondMaterial(), sShaderParams.m_fGlobalLightScalar * sShaderParams.m_fFrondLightScalar, sShaderParams.m_fAmbientScalar);

	m_leafCardMat = new Material("_leafcard");
	m_leafCardMat->setTexture(MaterialTextureId::Diffuse, tex);
	setMaterialColor(m_leafCardMat, m_treeRt->GetLeafMaterial(), sShaderParams.m_fGlobalLightScalar * sShaderParams.m_fLeafLightScalar, sShaderParams.m_fAmbientScalar);

	m_leafMeshMat = new Material("_leafmesh");
	m_leafMeshMat->setTexture(MaterialTextureId::Diffuse, tex);
	setMaterialColor(m_leafMeshMat, m_treeRt->GetLeafMaterial(), sShaderParams.m_fGlobalLightScalar * sShaderParams.m_fLeafLightScalar, sShaderParams.m_fAmbientScalar);
}

void TreeAsset::buildPrimitives()
{
	buildBranch();
	buildFrond();
	buildLeafCard();
	buildLeafMesh();
}

void TreeAsset::buildBranch()
{
	// query vertex attribute data
	CSpeedTreeRT::SGeometry geometry;
	m_treeRt->GetGeometry(geometry, SpeedTree_BranchGeometry);
	const CSpeedTreeRT::SGeometry::SIndexed &b = geometry.m_sBranches;

	int numVerts = b.m_nNumVertices;
	int numLods = m_treeRt->GetNumBranchLodLevels();
	m_numBranchLods = numLods;

	if (numLods <= 0) {
		return;
	}

	if (b.m_pNumStrips[0] != 1) {
		return;
	}

	int numIndexes = b.m_pStripLengths[0][0] + 1;	// +1 for swap face

	if (numVerts <= 0 || numIndexes <= 0) {
		return;
	}

	MeshPrim *lod0 = new MeshPrim(MeshPrim::HintStatic);

	lod0->init(numVerts, numIndexes);

	// we use triangle strip for branches
	lod0->setStriped(true);
	lod0->setMaterial(m_branchMat);

	MeshVertex *verts = lod0->lockVertexes();

	for (int i = 0; i < numVerts; i++) {
		const float *diffuseTc = b.m_pTexCoords[CSpeedTreeRT::TL_DIFFUSE] + i * 2;

		float fWindMatrixIndex1 = float(int(b.m_pWindMatrixIndices[0][i] * 10.0f / RenderWind::NUM_WIND_MATRIXES));
		float fWindMatrixWeight1 = b.m_pWindWeights[0][i];
		float fWindMatrixIndex2 = float(int(b.m_pWindMatrixIndices[1][i] * 10.0f / RenderWind::NUM_WIND_MATRIXES));
		float fWindMatrixWeight2 = b.m_pWindWeights[1][i];

		verts[i].position.set(b.m_pCoords[i*3+0], b.m_pCoords[i*3+1], b.m_pCoords[i*3+2]);
		verts[i].streamTc.set(diffuseTc[0], diffuseTc[1], fWindMatrixIndex1 + fWindMatrixWeight1, fWindMatrixIndex2 + fWindMatrixWeight2);
		verts[i].color = Rgba::White;
		verts[i].normal.set( b.m_pNormals[i * 3 + 0], b.m_pNormals[i * 3 + 1], b.m_pNormals[i * 3 + 2], 1);
		verts[i].tangent.set(b.m_pTangents[i*3+0], b.m_pTangents[i*3+1], b.m_pTangents[i*3+2], 0);
		//verts[i].binormal =(verts[i].normal ^ verts[i].tangent).getNormalized();
	}

	lod0->unlockVertexes();

	ushort_t *idxes = lod0->lockIndexes();
	idxes[0] = b.m_pStrips[0][0][0];
	for (int i = 0; i < numIndexes-1; i++) {
		idxes[i+1] = b.m_pStrips[0][0][i];
	}

	lod0->unlockIndexes();

	m_branchPrims[0] = lod0;

	// load lower lod
	for (int i = 1; i < numLods; i++) {
		if (b.m_pNumStrips[i] != 1) {
			continue;
		}

		numIndexes = b.m_pStripLengths[i][0] + 1;	// +1 for swap face
		if (numIndexes < 3) {
			continue;
		}

		RefPrim *ref = new RefPrim(Primitive::HintStatic);
		ref->init(lod0, numIndexes);

		idxes = ref->lockIndexes();
		idxes[0] = b.m_pStrips[i][0][0];
		for (int j = 0; j < numIndexes-1; j++) {
			idxes[j+1] = b.m_pStrips[i][0][j];
		}
		ref->unlockIndexes();

		m_branchPrims[i] = ref;
	}
}

void TreeAsset::buildFrond()
{
	CSpeedTreeRT::SGeometry sGeometry;
	m_treeRt->GetGeometry(sGeometry, SpeedTree_FrondGeometry);
	const CSpeedTreeRT::SGeometry::SIndexed &b = sGeometry.m_sFronds;

	int numVerts = b.m_nNumVertices;
	int numLods = m_treeRt->GetNumFrondLodLevels();
	m_numFrondLods = numLods;

	if (!numLods) {
		return;
	}

	int numIndexes = b.m_pStripLengths[0][0] + 1; // +1 for swap faces

	if (!numVerts || !numIndexes) {
		return;
	}

	MeshPrim *lod0 = new MeshPrim(MeshPrim::HintStatic);

	lod0->init(numVerts, numIndexes);

	// we use triangle strip for branches
	lod0->setStriped(true);
	lod0->setMaterial(m_frondMat);

	// fill vertexes
	MeshVertex *verts = lod0->lockVertexes();
	for (int i = 0; i < numVerts; i++) {
		const float *diffuseTc = b.m_pTexCoords[CSpeedTreeRT::TL_DIFFUSE] + i * 2;

		float fWindMatrixIndex1 = float(int(b.m_pWindMatrixIndices[0][i] * 10.0f / RenderWind::NUM_WIND_MATRIXES));
		float fWindMatrixWeight1 = b.m_pWindWeights[0][i];
		float fWindMatrixIndex2 = float(int(b.m_pWindMatrixIndices[1][i] * 10.0f / RenderWind::NUM_WIND_MATRIXES));
		float fWindMatrixWeight2 = b.m_pWindWeights[1][i];

		verts[i].position.set(b.m_pCoords[i*3+0], b.m_pCoords[i*3+1], b.m_pCoords[i*3+2]);
		verts[i].streamTc.set(diffuseTc[0], 1.0 - diffuseTc[1], fWindMatrixIndex1 + fWindMatrixWeight1, fWindMatrixIndex2 + fWindMatrixWeight2);
		verts[i].color = Rgba::White;
		verts[i].normal.set( b.m_pNormals[i * 3 + 0], b.m_pNormals[i * 3 + 1], b.m_pNormals[i * 3 + 2], 1.0f);
		verts[i].tangent.set(b.m_pTangents[i*3+0], b.m_pTangents[i*3+1], b.m_pTangents[i*3+2], 0);
		//verts[i].binormal =(verts[i].normal ^ verts[i].tangent).getNormalized();
	}
	lod0->unlockVertexes();

	// fill indexes
	ushort_t *idxes = lod0->lockIndexes();
	idxes[0] = b.m_pStrips[0][0][0];
	for (int i = 0; i < numIndexes-1; i++) {
		idxes[i+1] = b.m_pStrips[0][0][i];
	}

	// we need swap face for speedtree
	for (int i = 0; i <(numIndexes) / 2; i++) {
		std::swap(idxes[i*2], idxes[i*2+1]);
	}

	lod0->unlockIndexes();

	m_frondPrims[0] = lod0;

	// load lower lod
	for (int i = 1; i < numLods; i++) {
		if (b.m_pNumStrips[i] != 1) {
			continue;
		}

		numIndexes = b.m_pStripLengths[i][0] + 1;	// +1 for swap face
		if (numIndexes < 3) {
			continue;
		}

		RefPrim *ref = new RefPrim(Primitive::HintStatic);
		ref->init(lod0, numIndexes);

		idxes = ref->lockIndexes();
		idxes[0] = b.m_pStrips[i][0][0];
		for (int j = 0; j < numIndexes-1; j++) {
			idxes[j+1] = b.m_pStrips[i][0][j];
		}
		ref->unlockIndexes();

		m_frondPrims[i] = ref;
	}
}

void TreeAsset::buildLeafCard()
{
	const int numLods = m_treeRt->GetNumLeafLodLevels();
	m_numLeafLods = numLods;

	CSpeedTreeRT::SGeometry sGeometry;
	m_treeRt->GetGeometry(sGeometry, SpeedTree_LeafGeometry);

	for (int i = 0; i < numLods; i++) {
		const CSpeedTreeRT::SGeometry::SLeaf &sLeaves = sGeometry.m_pLeaves[i];

		MeshPrim *prim = new MeshPrim(MeshPrim::HintStatic);
		prim->setMaterial(m_leafCardMat);
		prim->init(sLeaves.m_nNumLeaves * 4, sLeaves.m_nNumLeaves * 6);

		MeshVertex *verts = prim->lockVertexes();
		ushort_t *indexes = prim->lockIndexes();

		int numLeafCards = 0;

		for (int j = 0; j < sLeaves.m_nNumLeaves; j++) {
			const CSpeedTreeRT::SGeometry::SLeaf::SCard *pCard = sLeaves.m_pCards + sLeaves.m_pLeafCardIndices[j];

			if (pCard->m_pMesh) {
				continue;
			}

			Vector3 offsets[4];
			offsets[0].set(-1, -1, 0);
			offsets[1].set(-1, 1, 0);
			offsets[2].set(1, 1, 0);
			offsets[3].set(1, -1, 0);

			for (int k = 0; k < 4; ++k) {
				MeshVertex *vert = verts + j * 4 + k;
				// xy = diffuse texcoords
				// zw = compressed wind parameters
				float fWindMatrixIndex1 = float(int(sLeaves.m_pWindMatrixIndices[0][j] * 10.0f / NUM_WIND_MATRIX));
				float fWindMatrixWeight1 = sLeaves.m_pWindWeights[0][j];
				float fWindMatrixIndex2 = float(int(sLeaves.m_pWindMatrixIndices[1][j] * 10.0f / NUM_WIND_MATRIX));
				float fWindMatrixWeight2 = sLeaves.m_pWindWeights[1][j];
//				cBuffer.TexCoord4(0, pCard->m_pTexCoords[k * 2], pCard->m_pTexCoords[k * 2 + 1], fWindMatrixIndex1 + fWindMatrixWeight1, fWindMatrixIndex2 + fWindMatrixWeight2);
				vert->streamTc.set(pCard->m_pTexCoords[k * 2], 1.0f - pCard->m_pTexCoords[k * 2 + 1], fWindMatrixIndex1 + fWindMatrixWeight1, fWindMatrixIndex2 + fWindMatrixWeight2);

// tex layer 1: .x = width, .y = height, .z = pivot x, .w = pivot.y
//						cBuffer.TexCoord4(1, pCard->m_fWidth, pCard->m_fHeight, pCard->m_afPivotPoint[0] - 0.5f, pCard->m_afPivotPoint[1] - 0.5f);
				vert->tangent.set(pCard->m_fWidth, pCard->m_fHeight, pCard->m_afPivotPoint[0] - 0.5f, pCard->m_afPivotPoint[1] - 0.5f);
				//vert->binormal.set();

				vert->tangent.set(sLeaves.m_pCenterCoords[j * 3 + 0], sLeaves.m_pCenterCoords[j * 3 + 1], sLeaves.m_pCenterCoords[j * 3 + 2]/*, float(k) */, 0);
// tex layer 2: .x = angle.x, .y = angle.y, .z = leaf angle index [0,c_nNumSpeedWindAngles-1], .w = dimming
//						cBuffer.TexCoord4(2, Math::d2r(pCard->m_afAngleOffsets[0]), Math::d2r(pCard->m_afAngleOffsets[1]),  float(j % NUM_LEAF_ANGLES), sLeaves.m_pDimming[j]);
				vert->color.set(pCard->m_afAngleOffsets[0] + 127, pCard->m_afAngleOffsets[1] + 127, j % NUM_LEAF_ANGLES, sLeaves.m_pDimming[j] * 255);

				// normal
				vert->normal.set(sLeaves.m_pNormals + j * 12 + k * 3);

				// coordinate
				vert->position.set(sLeaves.m_pCenterCoords[j * 3 + 0], sLeaves.m_pCenterCoords[j * 3 + 1], sLeaves.m_pCenterCoords[j * 3 + 2]/*, float(k) */);
				vert->position.y += pCard->m_fWidth * offsets[k].x * 0.5f;
				vert->position.z += pCard->m_fHeight * offsets[k].y * 0.5f;
			}

			ushort_t *idx = indexes + j * 6;
			idx[0] = j * 4;
			idx[1] = j * 4 + 1;
			idx[2] = j * 4 + 2;
			idx[3] = j * 4;
			idx[4] = j * 4 + 2;
			idx[5] = j * 4 + 3;

			numLeafCards++;
		}

		prim->unlockVertexes();
		prim->unlockIndexes();

		if (!numLeafCards) {
			delete prim;
		} else {
			prim->setActivedIndexes(numLeafCards * 6);
			m_leafPrims[i] = prim;
		}
	}
}

void TreeAsset::buildLeafMesh()
{
	const int numLods = m_treeRt->GetNumLeafLodLevels();
	m_numLeafLods = numLods;

	CSpeedTreeRT::SGeometry sGeometry;
	m_treeRt->GetGeometry(sGeometry, SpeedTree_LeafGeometry);

	for (int i = 0; i < numLods; i++) {
		const CSpeedTreeRT::SGeometry::SLeaf &sLeaves = sGeometry.m_pLeaves[i];

		int numverts = 0;
		int numidxes = 0;
		for (int j = 0; j < sLeaves.m_nNumLeaves; j++) {
			const CSpeedTreeRT::SGeometry::SLeaf::SCard *pCard = sLeaves.m_pCards + sLeaves.m_pLeafCardIndices[j];

			if (!pCard->m_pMesh) {
				continue;
			}

			numverts += pCard->m_pMesh->m_nNumVertices;
			numidxes += pCard->m_pMesh->m_nNumIndices;
		}

		if (!numverts || !numidxes) {
			continue;
		}

		MeshPrim *prim = new MeshPrim(MeshPrim::HintStatic);
		prim->setMaterial(m_leafMeshMat);
		prim->init(numverts, numidxes);

		MeshVertex *verts = prim->lockVertexes();
		ushort_t *indexes = prim->lockIndexes();

		int curverts = 0;
		int curidxes = 0;

		for (int j = 0; j < sLeaves.m_nNumLeaves; j++) {
			const CSpeedTreeRT::SGeometry::SLeaf::SCard *pCard = sLeaves.m_pCards + sLeaves.m_pLeafCardIndices[j];

			if (!pCard->m_pMesh) {
				continue;
			}

			const CSpeedTreeRT::SGeometry::SLeaf::SMesh *pMesh = pCard->m_pMesh;

			Matrix matrix;
			Matrix3 &axis = matrix.axis;
			Vector3 &org = matrix.origin;

			for (int nVertex = 0; nVertex < pMesh->m_nNumVertices; ++nVertex) {
				float fWindMatrixIndex1 = float(int(sLeaves.m_pWindMatrixIndices[0][j] * 10.0f / NUM_WIND_MATRIX));
				float fWindMatrixWeight1 = sLeaves.m_pWindWeights[0][j];
				float fWindMatrixIndex2 = float(int(sLeaves.m_pWindMatrixIndices[1][j] * 10.0f / NUM_WIND_MATRIX));
				float fWindMatrixWeight2 = sLeaves.m_pWindWeights[1][j];

				MeshVertex *vert = verts + curverts + nVertex;

				// tex layer 0: .xy = diffuse texcoords, .z = leaf angle index [0,c_nNumSpeedWindAngles-1], .w = dimming
//					cBuffer.TexCoord4(0, pMesh->m_pTexCoords[nVertex * 2], pMesh->m_pTexCoords[nVertex * 2 + 1], float(j % NUM_LEAF_ANGLES), sLeaves.m_pDimming[j]);

				vert->streamTc.set(pMesh->m_pTexCoords[nVertex * 2], 1.0f - pMesh->m_pTexCoords[nVertex * 2 + 1], fWindMatrixIndex1 + fWindMatrixWeight1, fWindMatrixIndex2 + fWindMatrixWeight2);

				// orientation vectors
//					cBuffer.TexCoord3(1, sLeaves.m_pTangents[j * 12 + 0], sLeaves.m_pBinormals[j * 12 + 0], sLeaves.m_pNormals[j * 12 + 0]);
//					cBuffer.TexCoord3(2, sLeaves.m_pTangents[j * 12 + 2], sLeaves.m_pBinormals[j * 12 + 2], sLeaves.m_pNormals[j * 12 + 2]);

				axis[0].set(sLeaves.m_pTangents[j * 12 + 0], sLeaves.m_pTangents[j * 12 + 1], sLeaves.m_pTangents[j * 12 + 2]);
				axis[1].set(sLeaves.m_pBinormals[j * 12 + 0], sLeaves.m_pBinormals[j * 12 + 1], sLeaves.m_pBinormals[j * 12 + 2]);
				axis[2].set(sLeaves.m_pNormals[j * 12 + 0], sLeaves.m_pNormals[j * 12 + 1], sLeaves.m_pNormals[j * 12 + 2]);
				org.set(sLeaves.m_pCenterCoords[j * 3 + 0], sLeaves.m_pCenterCoords[j * 3 + 1], sLeaves.m_pCenterCoords[j * 3 + 2]);

				// offset (used to position the mesh on the tree)
				vert->tangent.set(sLeaves.m_pCenterCoords[j * 3 + 0], sLeaves.m_pCenterCoords[j * 3 + 1], sLeaves.m_pCenterCoords[j * 3 + 2], 0);

				// normal
				vert->normal.set(pMesh->m_pNormals + nVertex * 3);
				vert->normal.xyz() = axis * vert->normal.xyz();
				vert->normal.w = 1;

				// coordinate (3d coord of leaf mesh)
				vert->position.set(pMesh->m_pCoords[nVertex * 3 + 0], pMesh->m_pCoords[nVertex * 3 + 1], pMesh->m_pCoords[nVertex * 3 + 2]);
				vert->position = matrix * vert->position;

				vert->color.set(255, 255, j % NUM_LEAF_ANGLES, sLeaves.m_pDimming[j] * 255);
			}

			// setup the triangle indices
			for (int nIndex = 0; nIndex < pMesh->m_nNumIndices; ++nIndex)
				indexes[curidxes+nIndex] = curverts + pMesh->m_pIndices[nIndex];

			curverts += pMesh->m_nNumVertices;
			curidxes += pMesh->m_nNumIndices;
		}

		prim->unlockVertexes();
		prim->unlockIndexes();

		m_leafPrims[i] = prim;
	}
}

void TreeAsset::issueToQueue(TreeEntity *actor, RenderScene *qscene)
{
#if 0
	if (r_geoInstancing->getInteger() != 0) {
		return;
	} else {
#endif
		float lod = actor->m_lod;
		CSpeedTreeRT::SLodValues lv;

		m_treeRt->GetLodValues(lv, lod);

		if ( 0 && qscene->sceneType != RenderScene::WorldMain) {
			lv.m_anLeafActiveLods[0] = m_numLeafLods - 1;
			lv.m_afLeafAlphaTestValues[0] = 127;
		}

		if (lv.m_nBranchActiveLod >= 0 && m_branchPrims[lv.m_nBranchActiveLod]) {
			qscene->addInteraction(actor, m_branchPrims[lv.m_nBranchActiveLod]);
		}

		if (lv.m_nFrondActiveLod >= 0 && m_frondPrims[lv.m_nFrondActiveLod]) {
			qscene->addInteraction(actor, m_frondPrims[lv.m_nFrondActiveLod]);
		}

		if (lv.m_anLeafActiveLods[0] >= 0 && m_leafPrims[lv.m_anLeafActiveLods[0]]) {
			qscene->addInteraction(actor, m_leafPrims[lv.m_anLeafActiveLods[0]]);
		}
#if 0
	}
#endif
}

#if 0
static inline void addInstance(QueuedScene *qscene, InstancePrim *gi)
{
	InstancePrim *cloned = new InstancePrim(Primitive::HintFrame);
	cloned->init(gi->getInstanced());
	cloned->setInstances(gi->getAllInstances());

	qscene->addInteraction(nullptr, cloned);
}

void TreeAsset::issueToQueueInstancing(QueuedScene *qscene) {
	if (!r_speedtree->getBool()) {
		return;
	}

	// clear old instancing param first
	for (int i = 0; i < MAX_LODS; i++) {
		if (m_branchInstances[i]) {
			m_branchInstances[i]->clearAllInstances();
		}
		if (m_frondInstances[i]) {
			m_frondInstances[i]->clearAllInstances();
		}
		if (m_leafInstances[i]) {
			m_leafInstances[i]->clearAllInstances();
		}
	}

	GeoInstance::Param param;

	AX_FOREACH(TreeActor *actor, m_treeActors) {
		if (actor->m_worldVisFrameId != qscene->worldFrameId) {
			// not visible
			continue;
		}

		param.worldMatrix = actor->getMatrix();
		param.userDefined = actor->getInstanceParam();

		float lod = actor->m_lod;
		CSpeedTreeRT::SLodValues lv;

		m_treeRt->GetLodValues(lv, lod);

		if (qscene->sceneType != QueuedScene::WorldMain) {
			lv.m_anLeafActiveLods[0] = m_numLeafLods - 1;
			lv.m_afLeafAlphaTestValues[0] = 127;
		}

		if (lv.m_nBranchActiveLod >= 0 && m_branchInstances[lv.m_nBranchActiveLod]) {
			m_branchInstances[lv.m_nBranchActiveLod]->addInstance(param);
		}

		if (lv.m_nFrondActiveLod >= 0 && m_frondInstances[lv.m_nFrondActiveLod]) {
			m_frondInstances[lv.m_nFrondActiveLod]->addInstance(param);
		}

		if (lv.m_anLeafActiveLods[0] >= 0 && m_leafInstances[lv.m_anLeafActiveLods[0]]) {
			m_leafInstances[lv.m_anLeafActiveLods[0]]->addInstance(param);
		}
	}

	// issue to queue
	for (int i = 0; i < MAX_LODS; i++) {
		if (m_branchInstances[i] && m_branchInstances[i]->getNumInstance()) {
			addInstance(qscene, m_branchInstances[i]);
		}
		if (m_frondInstances[i] && m_frondInstances[i]->getNumInstance()) {
			addInstance(qscene, m_frondInstances[i]);
		}
		if (m_leafInstances[i] && m_leafInstances[i]->getNumInstance()) {
			addInstance(qscene, m_leafInstances[i]);
		}
	}
}
#endif

AX_END_NAMESPACE

#endif // AX_CONFIG_OPTION_USE_SPEEDTREE_40
