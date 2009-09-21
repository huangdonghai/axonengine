/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

namespace Axon {
	inline const char* xGetTypeName(int t) {
		switch (t) {
		case Asset::kModelFile:
			return "Model File";
		case Asset::kAnimFile:
			return "Animation File";
		case Asset::kFont:
			return "Font";
		case Asset::kTexture:
			return "Texture";
		case Asset::kMaterialFile:
			return "Material File";
		case Asset::kMaterial:
			return "Material";
		}

		return "Unknown";
	}
}

namespace Axon {

	/* class Asset
	   
	   
	   
	   Asset base  */

	Asset::Asset()
		: m_isDefaulted(false)
		, m_isInDeleteList(false)
	{

	}

	void Asset::deleteThis()
	{
		g_assetManager->removeAsset(const_cast<Asset*>(this));
	}

	//--------------------------------------------------------------------------
	// class AssetManager
	//--------------------------------------------------------------------------

	typedef List<Asset*>	AssetList;

	AX_BEGIN_COMMAND_MAP(AssetManager)
		AX_COMMAND_ENTRY("assetlist", list_f)
		AX_COMMAND_ENTRY("texlist",	texlist_f)
		AX_COMMAND_ENTRY("matlist",	matlist_f)
	AX_END_COMMAND_MAP()

	class AssetManager::Data {
	public:
		Data(int poolsize)
			: factory(nullptr)
			, defaulted(nullptr)
			, assetDict()
			, poolSize(poolsize)
		{}

		AssetFactory* factory;
		Asset* defaulted;
		AssetDict assetDict;
		AssetList assetNotRefed;
		AssetFactory::PoolHint poolHint;
		int poolSize;
	};

	AssetManager::AssetManager() {
		memset(m_datas, 0, sizeof(m_datas));
		m_frameId = 0;
	}

	AssetManager::~AssetManager() {
	}


	void AssetManager::initialize() {
		g_cmdSystem->registerHandler(this);
	}

	void AssetManager::finalize() {
		g_cmdSystem->registerHandler(this);
	}


	// add factory
	void AssetManager::registerType(int type, AssetFactory* factory) {
		SCOPE_LOCK;

		if (type < 0 || type >= Asset::MAX_TYPES)
			Errorf("out of bound");

		if (m_datas[type] != nullptr) {
			Errorf("already registered");
		}

		int poolsize = factory->getPoolSize();
		m_datas[type] = TypeNew<Data>(poolsize);
		m_datas[type]->factory = factory;
		m_datas[type]->poolHint = factory->getPoolHint();

		Asset* defaulted = factory->getDefaulted();

		if (defaulted == nullptr) {
			defaulted = factory->create();
			if (!defaulted->doInit("default", 0)) {
				factory->destroy(defaulted);
				defaulted = nullptr;
			}
		}
		m_datas[type]->defaulted = defaulted;
		if (defaulted != nullptr) {
			defaulted->m_isDefaulted = true;
		} else {
			// TODO
			Debugf("%s: can't found defaulted asset for type\n", __func__);
		}
	}

	void AssetManager::removeType(int type) {
		SCOPE_LOCK;

		checkType(__func__, type);
		//factory is freed by caller

		TypeDelete(m_datas[type]);
	}

	void AssetManager::runFrame() {
		SCOPE_LOCK;

		AX_ASSERT(m_frameId < std::numeric_limits<int>::max());
		m_frameId++;

		// TODO: free old asset
		for (int i = 0; i < Asset::MAX_TYPES; i++) {
			Data* d = m_datas[i];
			if (!d) {
				continue;
			}

			if (d->poolHint == AssetFactory::Immortal) {
				continue;
			}

#if 0
			if (d->poolHint == AssetFactory::Pooled) {

			}
#endif
			AssetList::iterator it = d->assetNotRefed.begin();
			while (it != d->assetNotRefed.end()) {
				Asset* asset = *it;
				if (asset->getRefCount()) {
					it = d->assetNotRefed.erase(it);
					asset->m_isInDeleteList = false;
					continue;
				}

				if (asset->m_frameId < m_frameId - 2) {
					d->assetDict.erase(asset->m_key);
					it = d->assetNotRefed.erase(it);
					d->factory->destroy(asset);
				} else {
					++it;
				}
			}
		}
	}


	// create an empty asset
	Asset* AssetManager::createEmptyAsset(int type) {
		SCOPE_LOCK;

		checkType(__func__, type);

		return m_datas[type]->factory->create();
	}

	Asset* AssetManager::findAsset(int type, const String& name, intptr_t arg) {
		SCOPE_LOCK;

		checkType(__func__, type);

		Data* d = m_datas[type];

		AssetDict::iterator it =d->assetDict.find(d->factory->generateKey(name, arg));
		if ( it != d->assetDict.end()) {
			it->second->addref();
			return it->second;
		}

		Asset* a = d->factory->create();
		if (a->init(name, arg)) {
			a->m_key = a->getKey();
			AX_ASSERT(!a->m_key.empty());
			d->assetDict[a->m_key] = a;

			return a;
		}

		d->factory->destroy(a);
		SafeAddRef(d->defaulted);
		return d->defaulted;
	}

	Asset* AssetManager::uniqueAsset(int type, const String& name, intptr_t arg /*= 0 */) {
		SCOPE_LOCK;

		checkType(__func__, type);

		Data* d = m_datas[type];

		Asset* a = d->factory->create();
		if (a->init(name, arg)) {
			a->m_key = a->getKey() + "$" + Uuid::generateUuid();
			d->assetDict[a->m_key] = a;

			return a;
		}

		d->factory->destroy(a);
		return d->defaulted;
	}

	void AssetManager::addAsset(int type, const String& key, Asset* asset) {
		SCOPE_LOCK;

		checkType(__func__, type);

		asset->m_key = key;

		Data* d = m_datas[type];
		AssetDict::iterator it =d->assetDict.find(asset->m_key);
		if ( it != d->assetDict.end()) {
			Errorf("%s: duplicated asset name", __func__);
		}

		d->assetDict[asset->m_key] = asset;
		asset->m_frameId = m_frameId;
	}

#if 0
	void AssetManager::freeAsset(Asset* asset) {
		SCOPE_LOCK;

		if (!asset)
			return;

		if (asset->isDefaulted())
			return;
#if 1
		asset->release();
#endif
	}
#endif

	void AssetManager::removeAsset(Asset* asset) {
		SCOPE_LOCK;

		AX_ASSERT(asset->getRefCount() == 0);
		AX_ASSERT(asset);

		if (asset->isDefaulted())
			return;

		int type = asset->getType();
		checkType(__func__, type);

		Data* d = m_datas[type];
		AssetDict::iterator it =d->assetDict.find(asset->m_key);
		if (it == d->assetDict.end()) {
			d->factory->destroy(asset);
			Debugf("%s: not found asset key", __func__);
			return;
		}

		// if is immortal asset, return
		if (d->poolHint == AssetFactory::Immortal) {
			return;
		}

		Asset* a = it->second;
		//AX_ASSERT(a == asset);

		// or, add to not referenced list
		asset->m_frameId = m_frameId;
		if (!asset->m_isInDeleteList) {
			d->assetNotRefed.push_back(asset);
			asset->m_isInDeleteList = true;
		}
		return;
	}

	void AssetManager::checkType(const char* func, int type) {
		if (type < 0 || type >= Asset::MAX_TYPES)
			Errorf("%s: out of bound", func);

		if (m_datas[type] == nullptr) {
			Errorf("%s: not yet registered", func);
		}
	}

	void AssetManager::listType(int type, const char* filter) {
		SCOPE_LOCK;

		if (type < 0 || type >= Asset::MAX_TYPES)
			return;

		const Data* d = m_datas[type];

		if (d == nullptr)
			return;

		Printf("List %s asset:\n", xGetTypeName(type));

		int count = 0;
		AssetDict::const_iterator it = d->assetDict.begin();
		for (; it != d->assetDict.end(); ++it) {
			Printf("%d %s\n", it->second->getRefCount(), it->second->m_key.c_str());
			count++;
		}

		Printf("total %d %s(s)\n", count, xGetTypeName(type));
	}


	void AssetManager::list_f(const CmdArgs& param) {
		SCOPE_LOCK;

		int type = 0;
		if (param.tokened.size() > 1)
			type = atoi(param.tokened[1].c_str());

		listType(type, nullptr);
	}

	void AssetManager::texlist_f(const CmdArgs& param) {
#if 0
		SCOPE_LOCK;

		const Data* d = m_datas[Asset::kTexture];

		if (d == nullptr)
			return;

		Printf("List %s texture:\n", xGetTypeName(Asset::kTexture));

		int count = 0;
		AssetDict::const_iterator it = d->assetDict.begin();
		for (; it != d->assetDict.end(); ++it) {
			Texture* tex = dynamic_cast<Texture*>(it->second);
			if (!tex) {
				continue;
			}

			int width, height, depth;
			tex->getSize(width, height, depth);
			Printf("%4d %4d %4d %8s %s\n",tex->getRefCount(), width, height, tex->getFormat().getStringName(), tex->m_key.c_str());
			count++;
		}

		Printf("total %d %s(s)\n", count, xGetTypeName(Asset::kTexture));
#endif
	}

	void AssetManager::matlist_f(const CmdArgs& param) {
		listType(Asset::kMaterial, nullptr);
	}

} // namespace Axon

