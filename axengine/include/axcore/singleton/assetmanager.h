/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_ENGINE_ASSETMANAGER_H
#define AX_ENGINE_ASSETMANAGER_H

#if 0
AX_BEGIN_NAMESPACE


	//--------------------------------------------------------------------------
	// class Asset
	//--------------------------------------------------------------------------

	class AX_API Asset : public RefObject {
	public:
		friend class AssetManager;

		enum Type {
			kModelFile, kAnimFile, kFont, kTexture, kMaterialFile, kMaterial,
			kSkinModel,

			kUserDefined,	// user type add here

			MAX_TYPES = 16
		};

		virtual ~Asset() {}

		bool isDefaulted() { return m_isDefaulted; }
		FixedString getKey() const { return m_key; }
		void setKey(const FixedString& newkey) { m_key = newkey; }

		// implement RefObject
		virtual void deleteThis();

		virtual bool doInit(const String& name, intptr_t arg) = 0;
		virtual int getType() const = 0;

	protected:
		Asset();
		bool init(const String& key, intptr_t arg) { m_key = key; return doInit(key, arg); }

	private:
		FixedString m_key;
		int m_frameId;
		bool m_isDefaulted : 1;
		bool m_isInDeleteList : 1;
	};
	typedef Sequence<Asset*>	AssetSeq;

	//--------------------------------------------------------------------------
	// class AssetFactory
	//--------------------------------------------------------------------------

	class AssetFactory {
	public:
		enum PoolHint {
			Immortal,			// asset will never be freed
			Pooled,				// asset is pooled, will be freed pool is full and after one frame
			OneFrame // asset will be freed at least after one frame to ensure render backend don't need it again
		};
		/*
		   test
		*/
		virtual Asset* create() = 0;
		virtual void destroy(Asset* p) = 0;
		virtual PoolHint getPoolHint() = 0;
		virtual int getPoolSize() = 0;
		virtual String generateKey(const String& name, intptr_t arg) = 0;
		virtual Asset* getDefaulted() = 0;
	};

	template< class T >
	class AssetFactory_ : public AssetFactory {
	public:
		virtual Asset* create() {
			return new T();
		}
		virtual void destroy(Asset* p) {
			delete(p);
		}
		virtual PoolHint getPoolHint() {
			return OneFrame;
		}
		virtual int getPoolSize() {
			return 1024;
		}
		virtual String generateKey(const String& name, intptr_t arg) {
			return name;
		}
		virtual Asset* getDefaulted() {
			return nullptr;
		}
	};

	//--------------------------------------------------------------------------
	// class AssetManager
	//--------------------------------------------------------------------------

	class AX_API AssetManager : public ICmdHandler, public ThreadSafe {
		AX_DECLARE_COMMAND_HANDLER(AssetManager);
	public:
		friend class Asset;

		typedef Dict<String,Asset*>		AssetDict;
		typedef DictSet<Asset*>			AssetSet;
		typedef Sequence<Asset*>		AssetSeq;

		AssetManager();
		~AssetManager();

		void initialize();
		void finalize();

		// add factory
		void registerType(int type, AssetFactory* factory);
		void removeType(int type);
		void runFrame();

		// create an empty asset, not even add to manager for find
		Asset* createEmptyAsset(int type);

		// add an asset to manager for find
		void addAsset(int type, const String& key, Asset* asset);

		// find or create a new asset
		Asset* findAsset(int type, const String& name, intptr_t arg = 0);

		// create a unique asset, not shared with others. need use freeAsset to free it
		Asset* uniqueAsset(int type, const String& name, intptr_t arg = 0);

		template< class Q >
		Q* findAsset(const String& name, intptr_t arg = 0) {
			return dynamic_cast<Q*>(findAsset(Q::AssetType, name, arg));
		}

	protected:
		void removeAsset(Asset* asset);
		void checkType(const char* func, int type);
		void listType(int type, const char* filter);

		// console command
		void list_f(const CmdArgs& param);
		void texlist_f(const CmdArgs& param);
		void matlist_f(const CmdArgs& param);

	private:
		class Data;

		Data* m_datas[Asset::MAX_TYPES];
		int m_frameId;
	};

#if 0
	template< class T >
	T* FindAsset_(const String& name, intptr_t arg = 0) {
		return dynamic_cast<T*>(g_assetManager->findAsset(T::AssetType, name, arg));
	}

	template< class T >
	T* UniqueAsset_(const String& name, intptr_t arg = 0) {
		return dynamic_cast<T*>(g_assetManager->uniqueAsset(T::AssetType, name, arg));
	}
#endif

	template< class T >
	ResultPtr<T> FindAsset_(const String& name, intptr_t arg = 0) {
#if 0
		RefPtr<T> result;
		result << dynamic_cast<T*>(g_assetManager->findAsset(T::AssetType, name, arg));
		return result;
#else
		return ResultPtr<T>(dynamic_cast<T*>(g_assetManager->findAsset(T::AssetType, name, arg)));
#endif
	}

	template< class T >
	ResultPtr<T> UniqueAsset_(const String& name, intptr_t arg = 0) {
#if 0
		RefPtr<T> result;
		result << dynamic_cast<T*>(g_assetManager->uniqueAsset(T::AssetType, name, arg));
		return result;
#else
		return ResultPtr<T>(dynamic_cast<T*>(g_assetManager->uniqueAsset(T::AssetType, name, arg)));
#endif
	}


AX_END_NAMESPACE
#endif
#endif // end guardian

