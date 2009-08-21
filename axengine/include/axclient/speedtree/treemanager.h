/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_CLIENT_SPEEDTREE_MANAGER_H
#define AX_CLIENT_SPEEDTREE_MANAGER_H

namespace Axon { namespace Render {

	class AX_API TreeManager : public Render::IEntityManager {
	public:
		TreeManager();
		~TreeManager();

		TreeAsset* findAsset(const String& name, int seed);

		void addAsset(TreeAsset* wrapper);
		void removeAsset(TreeAsset* wrapper);
		void addTree(TreeActor* tree);
		void removeTree(TreeActor* tree);

		// implement IEntityManager
		virtual bool isSupportExt(const String& ext) const;
		virtual Entity* create(const String& name, intptr_t arg = 0);
		virtual void updateForFrame(QueuedScene* qscene );
		virtual void issueToQueue(QueuedScene* qscene);

	private:
		typedef Dict<String, TreeAsset*>	TreeAssetDict;
		TreeAssetDict m_treeAssetDict;

		typedef DictSet<TreeActor*>		TreeDict;
		TreeDict m_treeDict;

		TreeAsset* m_defaulted;
	};

}} // namespace Axon::Render

#endif // end guardian

