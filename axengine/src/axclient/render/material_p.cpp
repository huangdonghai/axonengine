#include "../private.h"

AX_BEGIN_NAMESPACE

namespace
{
	int getMapType(const char *name) {
		if (Strequ(name, "diffuse")) {
			return MaterialTextureId::Diffuse;
		} else if (Strequ(name, "specular")) {
			return MaterialTextureId::Specular;
		} else if (Strequ(name, "normal")) {
			return MaterialTextureId::Normal;
		}

		return -1;
	}
}

// static member
MaterialDecl *MaterialDecl::ms_defaulted;
MaterialDecl::MaterialDeclDict MaterialDecl::ms_declDict;
MaterialDecl::FailureSet MaterialDecl::ms_failureSet;
// end static member


MaterialDecl::MaterialDecl()
{
	m_shaderGenMask = 0;
	m_surfaceType = SurfaceType::Dust;
	const char *s = m_surfaceType.toString();
	SurfaceType st2 = SurfaceType::fromString("Water");

	memset(m_textures, 0, sizeof(TextureDef*) * MaterialTextureId::MaxType);
	m_diffuse.set(1,1,1);
	m_specular.set(0,0,0);
	m_emission.set(0,0,0);
	m_shiness = 10.f;
	m_opacity = 1.0f;
	m_detailScale.set(20,20,20,20);

	m_flags = 0;

	m_depthWrite = true;
	m_depthTest = true;
	m_cullMode = RasterizerDesc::CullMode_Back;
	m_wireframed = false;
	m_blendMode = Material::BlendMode_Disabled;
	m_sortHint = Material::SortHint_Opacit;

	TypeZeroArray(m_features);
}

MaterialDecl::~MaterialDecl()
{
	// free texture
	// free params
}

bool MaterialDecl::tryLoad(const std::string &name)
{
	m_key = normalizeKey(name);

	std::string filename = m_key.toString() + ".mtr";

	TiXmlDocument doc;

	doc.LoadAxonFile(filename);

	if (doc.Error()) {
		Printf("%s: error parse %s in line %d - %s"
			, __func__
			, filename.c_str()
			, doc.ErrorRow()
			, doc.ErrorDesc());
		doc.Clear();
		return false;
	}

	const TiXmlElement *root = doc.FirstChildElement("material");
	const TiXmlAttribute *attr = NULL;

	// no root
	if (!root)
		goto error_exit;

	// parse attribute
	for (attr = root->FirstAttribute(); attr; attr = attr->Next()) {
		const std::string &attrname = attr->NameTStr();
		if (attrname == "flags") {

		} else if (attrname == "shader") {
			m_shaderName = attr->Value();
		} else if (attrname == "twosided") {
			if (attr->IntValue()) m_cullMode = RasterizerDesc::CullMode_None;
		} else if (attrname == "depthTest") {
			m_depthTest = attr->IntValue();
		} else if (attrname == "depthWrite") {
			m_depthWrite = attr->IntValue();
		} else if (attrname == "blendMode") {
			if (attr->ValueStr() == "add") {
				m_blendMode = Material::BlendMode_Add;
			} else if (attr->ValueStr() == "blend") {
				m_blendMode = Material::BlendMode_Blend;
			} else if (attr->ValueStr() == "modulate") {
				m_blendMode = Material::BlendMode_Modulate;
			} else {
				m_blendMode = Material::BlendMode_Disabled;
			}
		} else if (attrname == "wireframed") {
			if (attr->IntValue()) m_wireframed = true;
		} else if (attrname == "physicsHelper") {
			m_flags.set(Material::Flag_PhysicsHelper, attr->IntValue() ? true : false);
		} else if (attrname == "diffuse") {
			m_diffuse.fromString(attr->Value());
		} else if (attrname == "features") {
			int value = attr->IntValue();
			for (int i = 0; i < Material::MAX_FEATURES; i++) {
				if (value & (1<<i))
					m_features[i] = true;
				else
					m_features[i] = false;
			}
		}
	}

	// parse texture
	const TiXmlElement *elem = NULL;
	for (elem = root->FirstChildElement(); elem; elem = elem->NextSiblingElement()) {
		if (Strequ(elem->Value(), "texture")) {
			int maptype = -1;
			for (attr = elem->FirstAttribute(); attr; attr = attr->Next()) {
				if (Strequ(attr->Name(), "map")) {
					maptype = getMapType(attr->Value());
					if (maptype < 0)
						continue;
					AX_ASSERT(maptype < MaterialTextureId::MaxType);
					if (m_textures[maptype] != nullptr)
						continue;

					m_textures[maptype] = new TextureDef;

				} else if (Strequ(attr->Name(), "file")) {
					if (maptype < 0)
						Errorf("%s: map not set", __func__);
					m_textures[maptype]->file = attr->Value();
				} else if (attr->NameTStr() == "clamp") {
					m_textures[maptype]->desc.clampMode = SamplerDesc::ClampMode_Border;
				} else if (attr->NameTStr() == "clampToEdge") {
					m_textures[maptype]->desc.clampMode = SamplerDesc::ClampMode_Clamp;
				}
			}

		} else if (Strequ(elem->Value(), "shaderParam")) {

		}
	}

#if 0
	if (!(arg & JustParse)) {
		// set texture
		for (int i = 0; i < MapNumber; i++) {
			if (m_textures[i] == nullptr)
				continue;

			m_textures[i]->texture = gAssetManager->findAsset<Texture>(m_textures[i]->file);
		}
	}
#endif
	// free XML doc
	doc.Clear();
	return true;

error_exit:
	doc.Clear();
	return false;
}

FixedString MaterialDecl::normalizeKey(const std::string &name)
{
	FixedString key;

	if (!PathUtil::haveDir(name))
		key = "materials/" + name;
	else
		key = name;

	key = PathUtil::removeExt(key.toString());

	return key;
}

MaterialDecl *MaterialDecl::load(const std::string &name)
{
	// normalize key first
	FixedString key = normalizeKey(name);

	{ // check if already loaded
		MaterialDeclDict::const_iterator it = ms_declDict.find(key);
		if (it != ms_declDict.end())
			return it->second;
	}

	{
		// check if already checked
		FailureSet::const_iterator it = ms_failureSet.find(key);
		if (it != ms_failureSet.end()) {
			return ms_defaulted;
		}
	}

	// try load
	MaterialDecl *result = new MaterialDecl();
	bool success = result->tryLoad(key.toString());
	if (success) {
		ms_declDict[key] = result;
	} else {
		delete result;
		result = ms_defaulted;
		ms_failureSet.insert(key);
	}

	return result;
}

void MaterialDecl::initManager()
{
	ms_defaulted = load("default");
}

void MaterialDecl::finalizeManager()
{
	// do nothing
}

AX_END_NAMESPACE
