/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "../private.h"

#define BYTE_OFFSET(p, off) ((byte_t*)p + off)

namespace { namespace Internal {

	using namespace Axon;

	// return -1 if not found
	int getLodFromName(const char* name) {
		if (!name) {
			return -1;
		}

		size_t len = strlen(name);
		if (!len) {
			return -1;
		}

		int s = len - 1;
		int c = 0;
		for (; s >= 0; s--) {
			if (!isdigit(name[s])) {
				break;
			}
			c++;
		}

		if (!c) {
			return -1;
		}

		s++;	// point to first digit

		if (s < 4) {
			return -1;
		}

		const char* _lod = name + s - 4;
		if (_strnicmp(_lod, "_lod", 4) != 0) {
			return -1;
		}

		return atoi(name + s);
	}

	class VertexBufferReader {
	public:
		VertexBufferReader(hkxVertexBuffer* real) {
			m_real = real;
			m_stride = m_real->getVertexDesc()->m_stride;
			m_posDecl = m_real->getVertexDesc()->getElementDecl(hkxVertexDescription::HKX_DU_POSITION, 0);
			m_tcDecl = m_real->getVertexDesc()->getElementDecl(hkxVertexDescription::HKX_DU_TEXCOORD, 0);
			m_lightmapTcDecl = m_real->getVertexDesc()->getElementDecl(hkxVertexDescription::HKX_DU_TEXCOORD, 1);
			m_weightDecl = m_real->getVertexDesc()->getElementDecl(hkxVertexDescription::HKX_DU_BLENDWEIGHTS, 0);
			m_blendIdxDecl = real->getVertexDesc()->getElementDecl(hkxVertexDescription::HKX_DU_BLENDINDICES, 0);

			m_normalDecl = real->getVertexDesc()->getElementDecl(hkxVertexDescription::HKX_DU_NORMAL, 0);
			m_tangentDecl = real->getVertexDesc()->getElementDecl(hkxVertexDescription::HKX_DU_TANGENT, 0);
			m_binormalDecl = real->getVertexDesc()->getElementDecl(hkxVertexDescription::HKX_DU_BINORMAL, 0);

			m_vertexColorDecl = real->getVertexDesc()->getElementDecl(hkxVertexDescription::HKX_DU_COLOR, 0);
		}

		bool haveNormal() {
			return m_normalDecl != 0;
		}

		bool haveTangents() {
			return m_normalDecl && m_tangentDecl && m_binormalDecl;
		}

		int getCount() const {
			return m_real->getNumVertices();
		}

		const Vector3& getPostion(int index) {
			AX_ASSERT(index < getCount());
			AX_ASSERT(m_posDecl);

			void* pdata = (byte_t*)m_real->getVertexData() + index * m_stride;
			pdata = (byte_t*)pdata + m_posDecl->m_byteOffset;
			return *(Vector3*)pdata;
		}

		const Vector3& getNormal(int index) {
			AX_ASSERT(index < getCount());
			AX_ASSERT(m_normalDecl);

			void* pdata = (byte_t*)m_real->getVertexData() + index * m_stride;
			pdata = (byte_t*)pdata + m_normalDecl->m_byteOffset;
			return *(Vector3*)pdata;
		}

		const Vector3& getTangent(int index) {
			AX_ASSERT(index < getCount());
			AX_ASSERT(m_tangentDecl );

			void* pdata = (byte_t*)m_real->getVertexData() + index * m_stride;
			pdata = (byte_t*)pdata + m_tangentDecl->m_byteOffset;
			return *(Vector3*)pdata;
		}

		const Vector3& getBinormal(int index) {
			AX_ASSERT(index < getCount());
			AX_ASSERT(m_binormalDecl );

			void* pdata = (byte_t*)m_real->getVertexData() + index * m_stride;
			pdata = (byte_t*)pdata + m_binormalDecl->m_byteOffset;
			return *(Vector3*)pdata;
		}

		Vector2 getTexcoord(int index) {
			AX_ASSERT(index < getCount());

			Vector2 result;
			void* pdata = (byte_t*)m_real->getVertexData() + index * m_stride;
			if (m_tcDecl->m_type == hkxVertexDescription::HKX_DT_FLOAT2) {
				float* pf = (float*)((byte_t*)pdata + m_tcDecl->m_byteOffset);
				result.x = pf[0];
				result.y = 1.0f - pf[1];
				return result;

			} else if (m_tcDecl->m_type == hkxVertexDescription::HKX_DT_INT16) {
				hkInt16* pi = (hkInt16*)((byte_t*)pdata + m_tcDecl->m_byteOffset);
				result.x = pi[0] / 3276.8f;
				result.y = 1.0f - pi[1] / 3276.8f;
				return result;
			} else {
//				AX_ASSERT(0);
				return Vector2(0, 0);		// make compiler happy
			}
		}

		Vector2 getLightmapTexcoord(int index) {
			AX_ASSERT(index < getCount());

			Vector2 result(0,0);

			if (!m_lightmapTcDecl) {
				return result;
			}

			void* pdata = (byte_t*)m_real->getVertexData() + index * m_stride;
			if (m_lightmapTcDecl->m_type == hkxVertexDescription::HKX_DT_FLOAT2) {
				float* pf = (float*)((byte_t*)pdata + m_lightmapTcDecl->m_byteOffset);
				result.x = pf[0];
				result.y = 1.0f - pf[1];
				return result;

			} else if (m_lightmapTcDecl->m_type == hkxVertexDescription::HKX_DT_INT16) {
				hkInt16* pi = (hkInt16*)((byte_t*)pdata + m_lightmapTcDecl->m_byteOffset);
				result.x = pi[0] / 3276.8f;
				result.y = 1.0f - pi[1] / 3276.8f;
				return result;
			} else {
				//				AX_ASSERT(0);
				return result;		// make compiler happy
			}
		}

		Rgba getVertexColor(int index) {
			AX_ASSERT(index < getCount());

			Rgba result = Rgba::White;
			if (!m_vertexColorDecl) {
				return result;
			}

			void* pdata = (byte_t*)m_real->getVertexData() + index * m_stride;
			if (m_vertexColorDecl->m_type == hkxVertexDescription::HKX_DT_UINT32) {
				result = *(Rgba*)((byte_t*)pdata + m_vertexColorDecl->m_byteOffset);
				return result;
			}

			return result;
		}

		const float* getPositionPointer() {
			return (const float*)(m_real->getVertexData() + m_posDecl->m_byteOffset);
		}

		const hkUint8* getBlendWeightPointer() {
			if (!m_weightDecl) {
				return 0;
			}

			return (const hkUint8*)(m_real->getVertexData() + m_weightDecl->m_byteOffset);
		}

		const hkUint8* getBlendIndicesPointer() {
			if (!m_blendIdxDecl) {
				return 0;
			}

			return (const hkUint8*)(m_real->getVertexData() + m_blendIdxDecl->m_byteOffset);
		}

		hkUint8 getBonesPerVertex() {
			return 4;
		}

		hkxVertexBuffer* m_real;
		int m_stride;
		const hkxVertexDescription::ElementDecl* m_posDecl;
		const hkxVertexDescription::ElementDecl* m_tcDecl;
		const hkxVertexDescription::ElementDecl* m_lightmapTcDecl;
		const hkxVertexDescription::ElementDecl* m_weightDecl;
		const hkxVertexDescription::ElementDecl* m_blendIdxDecl;
		const hkxVertexDescription::ElementDecl* m_normalDecl;
		const hkxVertexDescription::ElementDecl* m_tangentDecl;
		const hkxVertexDescription::ElementDecl* m_binormalDecl;
		const hkxVertexDescription::ElementDecl* m_vertexColorDecl;
	};

	class MaterialReader {
	public:
		MaterialReader(const hkxMaterial* hkmat) : m_hkmat(hkmat) {}

		Texture* getLightmap() {
			if (!m_hkmat) {
				return nullptr;
			}

			// find lightmap
			for (int i = 0; i < m_hkmat->m_numStages; i++) {
				hkxMaterial::TextureStage* stage = &m_hkmat->m_stages[i];
				if (stage->m_tcoordChannel != 1) {
					continue;
				}

				if (stage->m_usageHint != hkxMaterial::TEX_UNKNOWN) {
					continue;
				}

				if (stage->m_texture.m_class != &hkxTextureFileClass) {
					continue;
				}

				Texture* tex = convert(stage);

				if (!tex) {
					continue;
				}

				return tex;
			}

			return nullptr;
		}

		void parseMaterialName(const char* hkname, String& axname, StringPairSeq& keyvalues ) {
			StringSeq t1 = StringUtil::tokenizeSeq(hkname, '?');

			if (t1.size()) {
				StringSeq t2 = StringUtil::tokenizeSeq(t1[0].c_str(), '#');
				if (t2.size()) {
					axname = t2[0];
				}

				if (t2.size()>1) {
//					number = atoi(t2[1].c_str());
				}
			}

			if (t1.size() < 2)
				return;

			StringSeq t2 = StringUtil::tokenizeSeq(t1[1].c_str(), '&');
			for (size_t i = 0; i < t2.size(); i++) {
				StringSeq t3 = StringUtil::tokenizeSeq(t2[i].c_str(), '=');

				if (t3.size() < 2) {
					continue;
				}
				keyvalues.push_back(std::make_pair(t3[0], t3[1]));
			}
		}

		SamplerType guessTypeFromName(const String& texname) {
			SamplerType samplertype = SamplerType::NUMBER_ALL;

			String filename = PathUtil::getName(texname);
			if (filename.size() < 7)
				return samplertype;

			if (StringUtil::strnicmp(filename.c_str(), "m_detail", 7) != 0)
				return samplertype;

			size_t size = filename.size();
			if (filename[size-2] == '_' && filename[size-1] == 'n') {
				samplertype = SamplerType::DetailNormal;
			} else {
				samplertype = SamplerType::Detail;
			}

			return samplertype;
		}

		void fillStatges(TexturePtr samplers[SamplerType::NUMBER_ALL], TexturePtr& lightmap)
		{
			for (int i = 0; i < m_hkmat->m_numStages; i++) {
				hkxMaterial::TextureStage* stage = &m_hkmat->m_stages[i];
				String filename = getTextureFilename(stage);

				if (filename.empty())
					continue;

				TexturePtr tex = Texture::load(filename);

				if (!tex) {
					continue;
				}

				SamplerType samplertype = SamplerType::NUMBER_ALL;
				if (stage->m_tcoordChannel == 0) {
					switch (stage->m_usageHint) {
					case hkxMaterial::TEX_UNKNOWN:
					case hkxMaterial::TEX_SPECULARANDGLOSS:
					case hkxMaterial::TEX_OPACITY:
					case hkxMaterial::TEX_GLOSS:
					case hkxMaterial::TEX_REFRACTION:
					case hkxMaterial::TEX_NOTEXPORTED:
					default:
						samplertype = guessTypeFromName(filename);
						break;
					case hkxMaterial::TEX_DIFFUSE:
						samplertype = SamplerType::Diffuse;
						break;
					case hkxMaterial::TEX_REFLECTION:
						samplertype = SamplerType::Envmap;
						break;
					case hkxMaterial::TEX_BUMP:
					case hkxMaterial::TEX_NORMAL:
						samplertype = SamplerType::Normal;
						break;
					case hkxMaterial::TEX_DISPLACEMENT:
						samplertype = SamplerType::Displacement;
						break;
					case hkxMaterial::TEX_SPECULAR:
						samplertype = SamplerType::Specular;
						break;
					case hkxMaterial::TEX_EMISSIVE:
						samplertype = SamplerType::Emission;
						break;
					}

				} else if (stage->m_tcoordChannel == 1) {
					lightmap = tex;
				}

				if (samplertype == SamplerType::NUMBER_ALL)
					continue;

				samplers[samplertype] = tex;

				// set texture set
				if (samplertype == SamplerType::Diffuse) {
#if 0
					if (!samplers[SamplerType::Normal]) {
						tex = Texture::load(filename + "_n");
						if (tex->isDefaulted()) {
							tex->release();
						} else {
							samplers[SamplerType::Normal] = tex;
						}
					}
					if (!samplers[SamplerType::Specular]) {
						tex = Texture::load(filename + "_s");
						if (tex->isDefaulted()) {
							tex->release();
						} else {
							samplers[SamplerType::Specular] = tex;
						}
					}
					if (!samplers[SamplerType::Emission]) {
						tex = Texture::load(filename + "_g");
						if (tex->isDefaulted()) {
							tex->release();
						} else {
							samplers[SamplerType::Emission] = tex;
						}
					}
#endif
				}

				if (samplertype == SamplerType::Detail && !samplers[SamplerType::DetailNormal]) {
					tex = Texture::load(filename + "_n");
					samplers[SamplerType::DetailNormal] = tex;
				}
			}
		}

		MaterialPtr getMaterial() {
			if (!m_hkmat) {
				return Material::load("default");
			}

			String fn = findStageFilename(hkxMaterial::TEX_DIFFUSE, 0);

			String axname;
			StringPairSeq keyvalues;

			if (m_hkmat->m_name && m_hkmat->m_name[0] == '_') {
#if 0
				// remove #
				String name = m_hkmat->m_name;
				size_t pos = name.rfind('#');
				if (pos != String::npos) {
					name.resize(pos);
				}
				
				if (fn.empty())
					return Material::load(name);

				Render::Material* uniquemat = Material::loadUnique(name);
				uniquemat->setTextureSet(fn);
				return uniquemat;
#else
				parseMaterialName(m_hkmat->m_name, axname, keyvalues);
#endif
			} else {
				axname = fn;
			}

			TexturePtr samplers[SamplerType::NUMBER_ALL];
			TexturePtr lightmap = 0;
			TypeZeroArray(samplers);

			fillStatges(samplers, lightmap);

			MaterialPtr mat = Material::loadUnique(axname);

			// set samplers to material
			for (int i = 0; i < SamplerType::NUMBER_ALL; i++) {
				if (samplers[i]) {
					mat->setTexture(i, samplers[i].get());
				}
			}

			// set params
			for (size_t i = 0; i < keyvalues.size(); i++) {
				StringPair& pair = keyvalues[i];
				if (pair.first == "ds" || pair.first == "detailscale") {
					float dt = atof(pair.second.c_str());

					mat->setDetailScale(dt);
				}
			}

			return mat;

#if 0
			if (fn.empty()) {
				return Material::load("default");
			} else {
				return Material::load(fn);
			}
#endif
		}

	private:
		String findStageFilename(hkxMaterial::TextureType textype, int channel) {
			String result;

			hkxMaterial::TextureStage* stage = findStage(textype, channel);

			if (!stage) {
				return result;
			}

			return getTextureFilename(stage);
		}

		hkxMaterial::TextureStage* findStage(hkxMaterial::TextureType textype, int channel) {
			for (int i = 0; i < m_hkmat->m_numStages; i++) {
				hkxMaterial::TextureStage* stage = &m_hkmat->m_stages[i];
				if (stage->m_tcoordChannel != channel) {
					continue;
				}

				if (stage->m_usageHint != textype) {
					continue;
				}

				return stage;
			}

			return nullptr;
		}

		String getTextureFilename(hkxMaterial::TextureStage* stage) {
			String result;

			if (stage->m_texture.m_class != &hkxTextureFileClass) {
				return result;
			}

			hkxTextureFile* texfile = (hkxTextureFile*)stage->m_texture.m_object;
			if (!texfile) {
				return result;
			}

			result = PathUtil::getRelativePath(l2u(texfile->m_filename));
			result = PathUtil::removeExt(result);

			return result;
		}

		Texture* convert(hkxMaterial::TextureStage* stage) {
			String fn = getTextureFilename(stage);

			Texture* tex = Texture::load(fn);
			return tex;
		}

	private:
		const hkxMaterial* m_hkmat;
	};

	class IStream {
	public:
		IStream(const String& filename) {
			m_hkstream = 0;

			m_size = g_fileSystem->readFile(filename, &m_buf);

			if (!m_size) {
				return;
			}

			m_hkstream = new hkIstream(m_buf, m_size);
		}

		~IStream() {
			if (m_buf) {
				g_fileSystem->freeFile(m_buf);
			}
		}

		hkIstream* m_hkstream;
		void* m_buf;
		size_t m_size;
	};

}} // namespace anynomus::Internal

AX_BEGIN_NAMESPACE
	using namespace Internal;
	using Internal::IStream;

	//--------------------------------------------------------------------------
	// class HavokAnimator
	//--------------------------------------------------------------------------

	HavokAnimator::HavokAnimator(HavokRig* rig) {
		m_rig = rig;
		m_animator = new hkaAnimatedSkeleton(m_rig->m_havokSkeleton);
		m_animator->setReferencePoseWeightThreshold(0.001f);
	}

	HavokAnimator::~HavokAnimator() {
		SafeDelete(m_animator);
	}

	void HavokAnimator::addAnimation(HavokAnimation* anim) {
		if (!anim->isValid())
			return;

		m_animator->addAnimationControl(anim->m_controler);
		m_animations.push_back(anim);
	}

	void HavokAnimator::removeAnimation(HavokAnimation* anim) {
		if (!anim->isValid())
			return;

		m_animator->removeAnimationControl(anim->m_controler);
		m_animations.remove(anim);
	}

	void HavokAnimator::removeAllAnimation()
	{
	}

	void HavokAnimator::renderToPose(HavokPose* pose) {
		if (!pose->isValid())
			return;

		hkaPose* hkapose = pose->m_havokPose;

		m_animator->sampleAndCombineAnimations(hkapose->accessUnsyncedPoseLocalSpace().begin(), hkapose->getFloatSlotValues().begin() );
	}

	void HavokAnimator::step(int frametime) {
		m_animator->stepDeltaTime(frametime * 0.001f);
	}

	//--------------------------------------------------------------------------
	// class HavokAnimation
	//--------------------------------------------------------------------------

	HavokAnimation::HavokAnimation(const String& name)
	{
		m_package = 0;
		m_animBinding = 0;
		m_controler = 0;

		m_package = g_havokPackageManager->findPackage(name);
		if (!m_package) {
			return;
		}
		m_animBinding = m_package->getAnimation(0);

		if (!m_animBinding) {
			return;
		}

		m_controler = new hkaDefaultAnimationControl(m_animBinding);
		m_controler->setMasterWeight(1.0f);
		m_controler->setPlaybackSpeed(1.0f);
	}


	HavokAnimation::~HavokAnimation() {
		if (m_controler)
			m_controler->removeReference();
	}

	bool HavokAnimation::isValid() const
	{
		return m_controler != 0;
	}

	void HavokAnimation::setMasterWeight(float weight)
	{
		if (m_controler) m_controler->setMasterWeight(weight);
	}

	void HavokAnimation::easeIn(float duration)
	{
		if (m_controler) m_controler->easeIn(duration);
	}

	void HavokAnimation::easeOut(float duration)
	{
		if (m_controler) m_controler->easeOut(duration);
	}

	void HavokAnimation::setLocalTime(float localtime)
	{
		if (m_controler) m_controler->setLocalTime(localtime);
	}

	void HavokAnimation::setPlaybackSpeed(float speed)
	{
		if (m_controler) m_controler->setPlaybackSpeed(speed);
	}

	bool HavokAnimation::isAnimDone(float timeleft)
	{
		if (!m_controler) return true;
		return m_controler->getAnimationBinding()->m_animation->m_duration - m_controler->getLocalTime() <= timeleft;
	}

	//--------------------------------------------------------------------------
	// class HavokRig
	//--------------------------------------------------------------------------

	HavokPose* HavokRig::createPose() {
		return new HavokPose(this);
	}

	int HavokRig::findBoneIndexByName(const char* BoneName)
	{
		if (!m_havokSkeleton)
			return -1;

		if (!m_havokSkeleton->m_bones)
			return -1;

		for (int iS = 0; iS < m_havokSkeleton->m_numBones; ++iS)
		{
			if (!::strcmp(m_havokSkeleton->m_bones[iS]->m_name, BoneName))
				return iS;
		}

		return -1;
	}

	const char* HavokRig::findBoneNameByIndex(int BoneIndex)
	{
		if (!m_havokSkeleton)
			return 0;

		if (!m_havokSkeleton->m_bones)
			return 0;

		if (m_havokSkeleton->m_numBones <= BoneIndex)
			return 0;

		return m_havokSkeleton->m_bones[BoneIndex]->m_name;
	}

	int HavokRig::getBoneCount()
	{
		if (!m_havokSkeleton)
			return 0;

		if (!m_havokSkeleton->m_bones) // need?
			return 0;

		return m_havokSkeleton->m_numBones;
	}

	HavokRig::HavokRig(HavokPackage* package) : HavokPackable(package), m_havokSkeleton(0)
	{
		if (m_package) m_havokSkeleton = m_package->getSkeleton();
		SafeAddRef(m_package);
	}

	HavokRig::HavokRig(const String& name)
	{
		m_package = g_havokPackageManager->findPackage(name);
		m_havokSkeleton = 0;
		if (m_package) m_havokSkeleton = m_package->getSkeleton();
	}

	bool HavokRig::isValid() const
	{
		return m_package && m_havokSkeleton;
	}

	HavokPose::HavokPose(HavokRig* rig) {
		if (rig->m_havokSkeleton) {
			m_havokPose = new hkaPose(rig->m_havokSkeleton);
			m_havokPose->setToReferencePose();
		}
	}

	HavokPose::~HavokPose()
	{
		SafeDelete(m_havokPose);
	}

	bool HavokPose::isValid() const
	{
		return m_havokPose != nullptr;
	}

	//--------------------------------------------------------------------------
	// class HavokPackage::MeshData
	//--------------------------------------------------------------------------

	class HavokPackage::MeshData {
	public:
		HavokPackage* m_package;
		const char* m_name;
		hkxMesh* m_havokMesh;
		hkaMeshBinding* m_binding;
		hkaBoneAttachment* m_boneAttachment; // the animation is either a skin binding anim or attachment anim
		hkxMeshSection* m_section;
		RenderMesh* m_renderMesh;
		int m_lod;
		bool m_needFreeRenderMesh;
		bool m_haveLocalTransform;
		AffineMat m_localTransform;

		MeshData(HavokPackage* package) {
			m_package = package;
			m_name = nullptr;
			m_havokMesh = nullptr;
			m_binding = nullptr;
			m_boneAttachment = nullptr;
			m_section = nullptr;
			m_renderMesh = nullptr;
			m_lod = -1;
			m_needFreeRenderMesh = true;
			m_haveLocalTransform = false;
		}

		~MeshData() {
			if (m_needFreeRenderMesh) {
				SafeDelete(m_renderMesh);
			}
		}

		void generateStaticMesh(Primitive::Hint hint) {
			if (m_renderMesh) {
				return;
			}
			m_renderMesh = new RenderMesh(hint);

			int numverts = m_section->m_vertexBuffer->getNumVertices();
			int numidxes = m_section->getNumTriangles() * 3;
			m_renderMesh->initialize(numverts, numidxes);

			ushort_t* pidx = m_renderMesh->lockIndexes();
			for (hkUint32 k = 0; k < m_section->getNumTriangles(); k++) {
				hkUint32 a, b, c;
				m_section->getTriangleIndices(k, a, b, c);
				pidx[0] = a; pidx[1] = c; pidx[2] = b;
				pidx += 3;
			}
			m_renderMesh->unlockIndexes();

			VertexBufferReader helper(m_section->m_vertexBuffer);

			bool haveTangents = helper.haveTangents();

			Vertex* verts = m_renderMesh->lockVertexes();
			memset(verts, 0, sizeof(Vertex) * numverts);
			for (int i = 0; i < numverts; i++) {
				if (m_haveLocalTransform) {
					verts[i].xyz = m_localTransform * helper.getPostion(i);
				} else {
					verts[i].xyz = helper.getPostion(i);
				}
				verts[i].st = helper.getTexcoord(i);
				verts[i].st2 = helper.getLightmapTexcoord(i);
				verts[i].rgba = helper.getVertexColor(i);

				if (helper.haveNormal()) {
					if (m_haveLocalTransform) {
						verts[i].normal = m_localTransform.axis * helper.getNormal(i);
					} else {
						verts[i].normal = helper.getNormal(i);
					}
				} else {
					Errorf("mesh must have normal");
				}

				if (haveTangents) {
					if (m_haveLocalTransform) {
						verts[i].tangent = m_localTransform.axis * helper.getTangent(i);
						verts[i].binormal = m_localTransform.axis * -helper.getBinormal(i);
					} else {
						verts[i].tangent = helper.getTangent(i);
						verts[i].binormal = -helper.getBinormal(i);
					}
				}
			}
			m_renderMesh->unlockVertexes();

#if 0
			Render::Material* mat = Material::load(m_material);
#else
			const HavokPackage::MaterialMap* mm = m_package->findMaterialMap(m_section->m_material);
#endif
			m_renderMesh->setMaterial(mm->m_axMat.get());
			m_renderMesh->setLightMap(mm->m_lightMap);

			if (!haveTangents) {
				m_renderMesh->computeTangentSpaceSlow();
			}
		}
	};

	HavokPackage::HavokPackage(const String& filename) {
		m_loader = nullptr;
		m_root = nullptr;
		m_physicsData = nullptr;
		m_animationContainer = nullptr;
		m_sceneData = nullptr;
		m_ragdoll = nullptr;

		m_isMeshDataGenerated = false;
		m_isBboxGenerated = false;
		m_isStaticMeshGenerated = false;

		m_name = filename;
#if 0
		m_loader = new hkLoader();
		m_root = m_loader->load(filename.c_str());
#else
		IStream is(filename);

		if (!is.m_hkstream) {
			return;
		}

		hkBinaryTagfileReader reader;
		m_dataWorld = new hkDataWorldDict;

		// Load the tagfile and get access to the top level data object.
		hkDataObject root = reader.load(is.m_hkstream->getStreamReader(), *m_dataWorld);

		// Convert the top level object into the native C++ objects stored
		// in the hkResource.
		hkResource* loadedData = hkDataObjectUtil::toResource(root);

		if (!loadedData) {
			goto errexit;
		}

		// Get the SimpleObject as the top level object.
		m_root = loadedData->getContents<hkRootLevelContainer>();

errexit:
		if (!m_root) {
			Errorf("error file format, can't found root container. maybe you need re-export it");
			return;
		}

		// Later on, when simpleobject is not needed anymore we should
		// deallocate the memory containing the loaded data.
//		loadedData->removeReference();
#endif
		if (m_root) {
			m_physicsData = static_cast<hkpPhysicsData*>(m_root->findObjectByType(hkpPhysicsDataClass.getName()));
			m_animationContainer = reinterpret_cast<hkaAnimationContainer*>(m_root->findObjectByType(hkaAnimationContainerClass.getName()));
			m_sceneData = reinterpret_cast<hkxScene*>(m_root->findObjectByType(hkxSceneClass.getName()));
			m_ragdoll = reinterpret_cast<hkaRagdollInstance*>(m_root->findObjectByType(hkaRagdollInstanceClass.getName()));
		}
	}

	HavokPackage::~HavokPackage() {
		delete m_loader;
		delete m_dataWorld;

		g_havokPackageManager->removePackage(m_name);
		SafeClearContainer(m_meshDatas);
		SafeClearContainer(m_materialMaps);
	}

	PhysicsRigid* HavokPackage::getRigidBody() {
		if (!m_physicsData) {
			return nullptr;
		}

		const hkArray<hkpPhysicsSystem*>& systems = m_physicsData->getPhysicsSystems();
		if (systems.getSize() == 0) {
			return nullptr;
		}

		const hkArray<hkpRigidBody*>& rigids = systems[0]->getRigidBodies();
		if (rigids.getSize() == 0) {
			return nullptr;
		}

		hkpRigidBody* rigid = rigids[0]->clone();
		return new PhysicsRigid(this, rigid);
	}

	const BoundingBox& HavokPackage::getBoundingBox() {
		if (m_isBboxGenerated) {
			return m_staticBbox;
		}

		generateStaticMesh();

		// generate bbox
		m_staticBbox.clear();
		MeshDataList::const_iterator it = m_meshDatas.begin();
		for (; it != m_meshDatas.end(); ++it) {
#if 0
			hkxMeshSection* section = (*it)->m_section;

			VertexBufferReader helper(section->m_vertexBuffer);

			for (int j = 0; j < helper.getCount(); j++) {
				m_staticBbox.expandBy(helper.getPostion(j));
			}
#else
			RenderMesh* mesh = (*it)->m_renderMesh;

			if (!mesh) continue;

			const Vertex* verts = mesh->getVertexesPointer();
			for (int j = 0; j < mesh->getNumVertexes(); j++) {
				m_staticBbox.expandBy(verts[j].xyz);
			}
#endif
		}

		m_isBboxGenerated = true;

		return m_staticBbox;
	}

	void HavokPackage::generateMeshData() {
		if (m_isMeshDataGenerated) {
			return;
		}

		if (m_animationContainer) {
			// generate from skin binding
			for (int i = 0; i < m_animationContainer->m_numSkins; i++) {
				hkaMeshBinding* binding = m_animationContainer->m_skins[i];

				for (int j = 0; j < binding->m_mesh->m_numSections; j++) {
					hkxMeshSection* section = binding->m_mesh->m_sections[j];
					MeshData* data = new MeshData(this);
					data->m_name = getMeshName(binding->m_mesh);
					data->m_havokMesh = binding->m_mesh;
					data->m_section = section;
					data->m_binding = binding;
					data->m_lod = getLodFromName(data->m_name);

					m_meshDatas.push_back(data);
				}
			}

			// generate from attachments
			for (int i = 0; i < m_animationContainer->m_numAttachments; i++) {
				hkaBoneAttachment* BoneAttachment = m_animationContainer->m_attachments[i];

				//Check the attachment is a mesh
				if (hkString::strCmp(BoneAttachment->m_attachment.m_class->getName(), hkxMeshClass.getName()) == 0)
				{
					hkxMesh* HavokMesh = (hkxMesh*)BoneAttachment->m_attachment.m_object;

					for (int j = 0; j < HavokMesh->m_numSections; j++) {
						hkxMeshSection* section = HavokMesh->m_sections[j];
						MeshData* data = new MeshData(this);
						data->m_name = getMeshName(HavokMesh);
						data->m_havokMesh = HavokMesh;
						data->m_section = section;
						data->m_binding = 0;
						data->m_boneAttachment = BoneAttachment;
						data->m_lod = getLodFromName(data->m_name);

						m_meshDatas.push_back(data);
					}
				}
			}
		} else if (m_sceneData) {
			// generate from scene data
			for (int i = 0; i < m_sceneData->m_numMeshes; i++) {
				hkxMesh* mesh = m_sceneData->m_meshes[i];
				const char* name = getMeshName(mesh);
				for (int j = 0; j < mesh->m_numSections; j++) {
					hkxMeshSection* section = mesh->m_sections[j];
					MeshData* data = new MeshData(this);
					data->m_name = name;
					data->m_havokMesh = mesh;
					data->m_section = section;
					data->m_binding = findBinding(mesh);
					data->m_lod = getLodFromName(data->m_name);

					m_meshDatas.push_back(data);
				}
			}
		}

		findNodeTransform();

		m_isMeshDataGenerated = true;
	}

	void HavokPackage::initDynamicMeshes(MeshDataList& result) {
		generateMeshData();

		if (!result.empty()) {
			Errorf("%s: mesh list isn't empty");
			return;
		}

		AX_FOREACH(MeshData* data, m_meshDatas) {
			MeshData* newdata = new MeshData(this);
			*newdata = *data;

			if (newdata->m_binding || newdata->m_boneAttachment) {
				newdata->m_needFreeRenderMesh = true;
				newdata->m_renderMesh = nullptr;
				newdata->generateStaticMesh(Primitive::HintDynamic);
				result.push_back(newdata);
			} else {
				newdata->m_needFreeRenderMesh = false;
			}
		}
	}

	void HavokPackage::clearDynamicMeshes( MeshDataList& result )
	{
		SafeClearContainer(result);
	}

	void HavokPackage::generateStaticMesh() {
		if (m_isStaticMeshGenerated) {
			return;
		}

		generateMeshData();

		MeshDataList::const_iterator it = m_meshDatas.begin();
		for (; it != m_meshDatas.end(); ++it) {
			(*it)->generateStaticMesh(Primitive::HintStatic);
		}

		m_isStaticMeshGenerated = true;
	}

	inline const char* findMesh_r(hkxNode* node, hkxMesh* mesh) {
		if (!node) {
			return nullptr;
		}

		if (node->m_object.m_object == mesh) {
			return node->m_name;
		}

		for (int i = 0; i < node->m_numChildren; i++) {
			const char* result = findMesh_r(node->m_children[i], mesh);
			if (result) {
				return result;
			}
		}

		return nullptr;
	}

	const char* HavokPackage::getMeshName(hkxMesh* mesh) {
		if (!m_sceneData) {
			return nullptr;
		}

		return findMesh_r(m_sceneData->m_rootNode, mesh);
	}

	hkaMeshBinding* HavokPackage::findBinding(hkxMesh* mesh) {
		if (!m_animationContainer) {
			return nullptr;
		}

		for (int i = 0; i < m_animationContainer->m_numSkins; i++) {
			if (m_animationContainer->m_skins[i]->m_mesh == mesh) {
				return m_animationContainer->m_skins[i];
			}
		}

		return nullptr;
	}

	Primitives HavokPackage::getPrimitives() {
		generateStaticMesh();

		Primitives result;

		MeshDataList::const_iterator it = m_meshDatas.begin();
		for (; it != m_meshDatas.end(); ++it) {
			result.push_back((*it)->m_renderMesh);
		}

		return result;
	}


	void HavokPackage::issueToQueue(QueuedEntity* qactor, QueuedScene* qscene )
	{
		generateStaticMesh();

		MeshDataList::const_iterator it = m_meshDatas.begin();
		for (; it != m_meshDatas.end(); ++it) {
			qscene->addInteraction(qactor, (*it)->m_renderMesh);
		}
	}

	hkaSkeleton* HavokPackage::getSkeleton()
	{
		if (!m_animationContainer) {
			return 0;
		}

		if (!m_animationContainer->m_numSkeletons) {
			return 0;
		}

		return m_animationContainer->m_skeletons[0];
	}

	int HavokPackage::getAnimationCount()
	{
		if (!m_animationContainer) {
			return 0;
		}

		return m_animationContainer->m_numBindings;
	}

	hkaAnimationBinding* HavokPackage::getAnimation(int Index) {
		if (!m_animationContainer) {
			return nullptr;
		}

		if (!m_animationContainer->m_numBindings) {
			return nullptr;
		}

		return m_animationContainer->m_bindings[Index];
	}

	hkaRagdollInstance* HavokPackage::getRagdoll() const {
		return m_ragdoll;
	}

	hkaSkeletonMapper* HavokPackage::getMapper(hkaSkeletonMapper* current) const {
		if (!m_root) {
			return nullptr;
		}

		void *objectFound = m_root->findObjectByType(hkaSkeletonMapperClass.getName(), current);
		return reinterpret_cast<hkaSkeletonMapper*> (objectFound);
	}

	const HavokPackage::MaterialMap* HavokPackage::findMaterialMap(hkxMaterial* hkmat) {
		AX_FOREACH(const MaterialMap* mm, m_materialMaps) {
			if (mm->m_hkMat == hkmat) {
				return mm;
			}
		}

		MaterialMap* mm = new MaterialMap;
		mm->m_hkMat = hkmat;

		MaterialReader mr(hkmat);

		mm->m_axMat = mr.getMaterial();
		mm->m_lightMap = mr.getLightmap();

		m_materialMaps.push_back(mm);

		return mm;
	}

	hkpRigidBody* HavokPackage::getRigidBodyHk() const
	{
		if (!m_physicsData) {
			return nullptr;
		}

		const hkArray<hkpPhysicsSystem*>& systems = m_physicsData->getPhysicsSystems();
		if (systems.getSize() == 0) {
			return nullptr;
		}

		const hkArray<hkpRigidBody*>& rigids = systems[0]->getRigidBodies();
		if (rigids.getSize() == 0) {
			return nullptr;
		}

		return rigids[0]->clone();
	}

	void HavokPackage::findNodeTransform()
	{
//		return;

		if (!m_sceneData)
			return;

		hkxNode* root = m_sceneData->m_rootNode;
		hkMatrix4 identity = hkMatrix4::getIdentity();

		findNodeTransform_r(root, identity);
	}

	static bool isIdentity(const hkMatrix4& mtx)
	{
		return memcmp( &mtx, &hkMatrix4::getIdentity(), sizeof(hkMatrix4)) == 0;
	}

	void HavokPackage::findNodeTransform_r( hkxNode* node, const hkMatrix4& parentTransform )
	{
		if (!node)
			return;

		if (!node->m_numKeyFrames)
			return;

		hkMatrix4 localTransform;
		localTransform.setMul(parentTransform, node->m_keyFrames[0]);

		if (!isIdentity(localTransform)) {
			if (!node->m_object.m_class)
				goto final;

			bool isMesh = hkString::strCmp(node->m_object.m_class->getName(), hkxMeshClass.getName()) == 0;
			if (isMesh) {
				hkxMesh* mesh = (hkxMesh*)node->m_object.m_object;
				setMeshTransform(mesh, localTransform);
			}
		}

final:
		for (int i = 0; i < node->m_numChildren; i++) {
			findNodeTransform_r(node->m_children[i], localTransform);
		}
	}

	void HavokPackage::setMeshTransform( hkxMesh* mesh, const hkMatrix4& localTransform )
	{
		AX_FOREACH(MeshData* meshData, m_meshDatas) {
			if (meshData->m_havokMesh != mesh)
				continue;

			meshData->m_haveLocalTransform = true;
			meshData->m_localTransform = h2x(localTransform);
		}
	}

	//--------------------------------------------------------------------------
	// class HavokPackageManager
	//--------------------------------------------------------------------------

	HavokPackageManager::HavokPackageManager()
	{}

	HavokPackageManager::~HavokPackageManager() {
	}

	HavokPackage* HavokPackageManager::findPackage(const String& name) {
		PackageDict::iterator it = m_packageDict.find(name);
		if (it != m_packageDict.end()) {
			HavokPackage* package = it->second;
			package->addref();
			return package;
		}

		if (!g_fileSystem->isFileExist(name))
			return 0;

		HavokPackage* package = new HavokPackage(name);
		m_packageDict[name] = package;
		return package;
	}

	void HavokPackageManager::removePackage(const String& name) {
		PackageDict::iterator it = m_packageDict.find(name);

		if (it == m_packageDict.end()) {
			Errorf("HavokPackageManager::removePackage: package '%s' not found", name.c_str());
			return;
		}

		m_packageDict.erase(it);
	}

	//--------------------------------------------------------------------------
	// class HavokModel
	//--------------------------------------------------------------------------

	HavokModel::HavokModel(HavokPackage* package) : RenderEntity(kModel) {
		m_package = package;
		m_pose = nullptr;
		m_isMeshDataInited = false;
		SafeAddRef(m_package);
	}

	HavokModel::HavokModel(const String& name) : RenderEntity(kModel)
	{
		m_package = g_havokPackageManager->findPackage(name);
		m_pose = nullptr;
		m_isMeshDataInited = false;
	}

	HavokModel::~HavokModel() {
		if (m_package)
			m_package->clearDynamicMeshes(m_mestDataList);

		SafeRelease(m_package);
	}

	BoundingBox HavokModel::getLocalBoundingBox() {
		if (m_pose) {
			if (m_pose->m_havokPose) {
				hkAabb aabb;
				m_pose->m_havokPose->getModelSpaceAabb(aabb);
				h2x(aabb, m_poseBbox);
				return m_poseBbox;
			}
		}
		if (m_package)
			return m_package->getBoundingBox();

		return BoundingBox::UnitBox;
	}

	BoundingBox HavokModel::getBoundingBox()
	{
		return getLocalBoundingBox().getTransformed(m_affineMat);
	}

	Primitives HavokModel::getHitTestPrims() {
		if (!m_package)
			return Primitives();

		if (!m_pose) {
			return m_package->getPrimitives();
		}

		if (!m_isMeshDataInited) {
			m_package->initDynamicMeshes(m_mestDataList);
			m_isMeshDataInited = true;
		}

		if (m_poseDirty) {
			m_poseDirty = false;
			applyPose();
		}

		Primitives result;
		AX_FOREACH (HavokPackage::MeshData* data, m_mestDataList) {
			result.push_back(data->m_renderMesh);
		}
		return result;
	}

	void HavokModel::issueToQueue(QueuedScene* qscene)
	{
		if (!m_package)
			return;

		if (!m_pose) {
			m_package->issueToQueue(m_queued, qscene);
			return;
		}

		if (!m_isMeshDataInited) {
			m_package->initDynamicMeshes(m_mestDataList);
			m_isMeshDataInited = true;
		}

		if (m_poseDirty) {
			m_poseDirty = false;
			applyPose();
		}

		AX_FOREACH (HavokPackage::MeshData* data, m_mestDataList) {
			qscene->addInteraction(m_queued, data->m_renderMesh);
		}
	}

	HavokRig* HavokModel::findRig() const {
		return new HavokRig(m_package);
	}

	void HavokModel::setPose(const HavokPose* pose, int linkBoneIndex) {
		m_pose = (HavokPose*)pose;
		m_poseDirty = true;
	}

	void HavokModel::applyPose() {
		const int boneCount = m_pose->m_havokPose->getSkeleton()->m_numBones;

		// Construct the composite world transform
		hkLocalArray<hkTransform> compositeWorldInverse(boneCount);
		compositeWorldInverse.setSize(boneCount);

		const hkArray<hkQsTransform>& poseInWorld = m_pose->m_havokPose->getSyncedPoseModelSpace();

		hkaMeshBinding* meshbinding = nullptr;
		AX_FOREACH(HavokPackage::MeshData* data, m_mestDataList) {
			if (data->m_binding) {
				if (meshbinding != data->m_binding) {
					meshbinding = data->m_binding;

					hkInt16* usedBones = meshbinding->m_mappings ? meshbinding->m_mappings[0].m_mapping : HK_NULL;
					int numUsedBones = usedBones ? meshbinding->m_mappings[0].m_numMapping : boneCount;

					// Multiply through by the bind pose inverse world inverse matrices
					for (int p=0; p < numUsedBones; p++) {
						int boneIndex = usedBones? usedBones[p] : p;
						compositeWorldInverse[p].setMul(poseInWorld[boneIndex], meshbinding->m_boneFromSkinMeshTransforms[boneIndex]);
					}
				}

				hkaFPUSkinningDeformer skinner;
				// hkaVertexDeformerInput vdi;
				// vdi.m_deformPosition = true;
				// hkxVertexBuffer* vIn = data->m_section->m_vertexBuffer;

				hkaFPUSkinningDeformer::hkaFloatBinding binding;

				TypeZero(&binding);

				VertexBufferReader helper(data->m_section->m_vertexBuffer);

				binding.m_iPosBase = helper.getPositionPointer();
				binding.m_iPosStride = data->m_section->m_vertexBuffer->getVertexDesc()->m_stride / sizeof(float);

				binding.m_iWeightBase = helper.getBlendWeightPointer();
				binding.m_iWeightStride = binding.m_iPosStride;
				binding.m_iIndexBase = helper.getBlendIndicesPointer();
				binding.m_iIndexStride = binding.m_iPosStride;
				binding.m_bonesPerVertex = helper.getBonesPerVertex();

				binding.m_oPosBase = &data->m_renderMesh->lockVertexes()->xyz[0];
				binding.m_oPosStride = sizeof(RenderMesh::VertexType) / sizeof(float);

				binding.m_numVerts = data->m_renderMesh->getNumVertexes();

				hkaFPUSkinningDeformer::deform(compositeWorldInverse.begin(), binding);

				data->m_renderMesh->unlockVertexes();
			}
			else if (data->m_boneAttachment) {
				HK_ALIGN(float f[16], 16);

				hkaBoneAttachment* ba = data->m_boneAttachment;

				poseInWorld [ ba->m_boneIndex ].get4x4ColumnMajor(f);
				hkMatrix4 worldFromBone; worldFromBone.set4x4ColumnMajor(f);
				hkMatrix4 worldFromAttachment; worldFromAttachment.setMul(worldFromBone, ba->m_boneFromAttachment);
				worldFromAttachment.get4x4ColumnMajor(f);
				data->m_renderMesh->setMatrix(Matrix4(f));
			}

			 data->m_renderMesh->computeTangentSpace();
		} // AX_FOREACH(HavokPackage::MeshData* data, m_mestDataList)
	}

AX_END_NAMESPACE

