/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

#ifdef AX_CONFIG_OPTION_USE_SPEEDTREE_40

AX_BEGIN_NAMESPACE

TreeManager::TreeManager()
{
	m_defaulted = nullptr;
}

TreeManager::~TreeManager()
{
}

TreeAssetPtr TreeManager::findAsset(const String &name, int seed)
{
	String key = TreeAsset::genKey(name, seed);
	TreeAssetDict::iterator it = m_treeAssetDict.find(key);

	if (it != m_treeAssetDict.end()) {
		return it->second;
	}

	TreeAsset *result = new TreeAsset(this);
	bool v = result->load(name, seed);

	if (!v) {
		delete result;
		return m_defaulted;
	}

	m_treeAssetDict[key] = result;
	return result;
}

void TreeManager::addAsset(TreeAsset *wrapper)
{
	m_treeAssetDict[wrapper->getKey()] = wrapper;
}

void TreeManager::removeAsset(TreeAsset *wrapper)
{
	m_treeAssetDict.erase(wrapper->getKey());
}

void TreeManager::addTree(TreeEntity *tree)
{
	m_treeDict.insert(tree);
}

void TreeManager::removeTree(TreeEntity *tree)
{
	m_treeDict.erase(tree);
}

bool TreeManager::isSupportExt(const String &ext) const
{
	if (ext == "spt") {
		return true;
	}

	if (ext == "SPT") {
		return true;
	}

	return false;
}

RenderEntity *TreeManager::create(const String &name, intptr_t arg)
{
	return new TreeEntity(name, arg);
}

void TreeManager::updateForFrame(RenderScene *qscene)
{
	// do nothing
}

void TreeManager::issueToQueue(RenderScene *qscene)
{
#if 0
	if (!r_geoInstancing->getInteger()) {
		return;
	}
	// gen instance
	TreeAssetDict::iterator it = m_treeAssetDict.begin();
	for (; it != m_treeAssetDict.end(); ++it) {
		it->second->issueToQueueInstancing(qscene);
	}
#endif
}

AX_END_NAMESPACE

#endif // AX_CONFIG_OPTION_USE_SPEEDTREE_40
