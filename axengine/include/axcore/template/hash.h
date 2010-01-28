/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_CORE_HASH_H
#define AX_CORE_HASH_H

/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

#if 0
AX_BEGIN_NAMESPACE

/*------------------------------------------------------------------------------
	hash -- hash template class, like stdext::hashxxx
------------------------------------------------------------------------------*/

// default hash bucket size for hash initialization
enum {
	DEFAULT_HASH_BUCKET_SIZE = 128
};

// TODO: type traits for object move and copy, for POD data type
template <class T1, class T2>
inline void _construct(T1 *p, const T2 &value) {
	new(p) T1(value);	// placement new
}

template <class T>
inline void _destroy(T *pointer) {
	pointer->~T();
}

template <class Key>
struct __hash {
};

template <class T>
struct __hash<T*> {
	size_t operator()(T *p) const { return size_t((void*)p); }
};

template <class T>
struct __hash<const T*> {
	size_t operator()(T *p) const { return size_t((const void*)p); }
};

template <class T>
struct identity : public std::unary_function<T, T> {
	const T &operator()(const T &x) const { return x; }
};

template <class Pair>
struct select1st : public std::unary_function<Pair, typename Pair::first_type> {
	const typename Pair::first_type &operator()(const Pair &x) const {
		return x.first;
	}
};

template <class Pair>
struct select2nd : public std::unary_function<Pair, typename Pair::second_type> {
	const typename Pair::second_type &operator()(const Pair &x) const {
		return x.second;
	}
};

template<typename T>
void hash_combine(size_t & seed, T const & v) {
	seed ^= __hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

/// 
inline size_t __hash_string(const char *s) {
	unsigned long h = 0;
	for (; *s; ++s)
		h = 5*h + *s;

	return size_t(h);
}

inline size_t __hash_string(const wchar_t *s) {
	unsigned long h = 0;
	for (; *s; ++s)
		h = 5*h + *s;

	return size_t(h);
}

template<>
struct __hash<char*> {
	size_t operator()(const char *s) const { return __hash_string(s); }
};

template<>
struct __hash<const char*> {
	size_t operator()(const char *s) const { return __hash_string(s); }
};

template<>
struct __hash<String> {
	size_t operator()(const String &s) const { return __hash_string(s.c_str()); }
};

template<>
struct __hash<const String> {
	size_t operator()(const String &s) const { return __hash_string(s.c_str()); }
};

template<>
struct __hash<char> {
	size_t operator()(char x) const { return x; }
};

template<>
struct __hash<unsigned char> {
	size_t operator()(unsigned char x) const { return x; }
};
template<>
struct __hash<signed char> {
	size_t operator()(unsigned char x) const { return x; }
};
template<>
struct __hash<short> {
	size_t operator()(short x) const { return x; }
};
template<>
struct __hash<unsigned short> {
	size_t operator()(unsigned short x) const { return x; }
};
template<>
struct __hash<int> {
	size_t operator()(int x) const { return x; }
};
template<>
struct __hash<unsigned int> {
	size_t operator()(unsigned int x) const { return x; }
};
template<>
struct __hash<long> {
	size_t operator()(long x) const { return x; }
};
template<>
struct __hash<unsigned long> {
	size_t operator()(unsigned long x) const { return x; }
};
template<>
struct __hash<ulonglong_t> {
	size_t operator()(ulonglong_t x) const { return x; }
};
template<>
struct __hash<void*> {
	size_t operator()(void *x) const { return (size_t)x; }
};

#if 0
template<>
struct __hash<size_t> {
	size_t operator()(size_t x) const { return x; }
};
template<>
struct __hash<intptr_t> {
	size_t operator()(intptr_t x) const { return x; }
};
template<>
struct __hash<uintptr_t> {
	size_t operator()(uintptr_t x) const { return x; }
};
#endif

// pre declare
template <class Key, class Value, class HashFn, class ExtractKey, class EqualKey>
struct __hash_const_iterator;

template <class Key, class Value, class HashFn, class ExtractKey, class EqualKey>
class __hash_table;


template <class Value>
struct __hash_node {
	__hash_node *mNext;
	Value mValue;		/// in map, contain key&data std::pair
};

template <class Key, class Value, class HashFn, class ExtractKey, class EqualKey>
struct __hash_iterator {
	typedef __hash_table<Key, Value, HashFn, ExtractKey, EqualKey>			HashTable;
	typedef __hash_iterator<Key, Value, HashFn, ExtractKey, EqualKey>		iterator;
	typedef __hash_const_iterator<Key, Value, HashFn, ExtractKey, EqualKey>	const_iterator;
	typedef __hash_node<Value>		Node;
	typedef Value value_type;
	typedef Value &reference;
	typedef Value *const_pointer;

	__hash_iterator(Node *n, HashTable *tab) : mCurrent(n), mHashTable(tab) {}
	__hash_iterator() {}

	reference operator*() const { return mCurrent->mValue; }
	const_pointer operator->() const { return &(operator*()); }
	bool operator==(const iterator &it) const { return mCurrent == it.mCurrent; }
	bool operator!=(const iterator &it) const { return mCurrent != it.mCurrent; }
	iterator operator++(int) { iterator tmp = *this; ++*this; return tmp; }
	iterator &operator++() {
		const Node *old = mCurrent;
		mCurrent = mCurrent->mNext;
		if (!mCurrent) {
			size_t bucket = mHashTable->BucketNum(old->mValue);
			while (!mCurrent && ++bucket < mHashTable->mBuckets.size()) {
				mCurrent = mHashTable->mBuckets[bucket];
			}
		}
		return *this;
	}

public:
	Node *mCurrent;
	HashTable *mHashTable;
};

template <class Key, class Value, class HashFn, class ExtractKey, class EqualKey>
struct __hash_const_iterator {
	typedef __hash_table<Key, Value, HashFn, ExtractKey, EqualKey>			HashTable;
	typedef __hash_node<Value>												Node;

	typedef __hash_iterator<Key, Value, HashFn, ExtractKey, EqualKey>		iterator;
	typedef __hash_const_iterator<Key, Value, HashFn, ExtractKey, EqualKey>	const_iterator;
	typedef Value value_type;
	typedef const Value &reference;
	typedef const Value *const_pointer;

	__hash_const_iterator(const Node *n, const HashTable *tab) : mCurrent(n), mHashTable(tab) {}
	/// cast from non const iterator
	__hash_const_iterator(const iterator &it) : mCurrent(it.mCurrent), mHashTable(it.mHashTable) {}
	__hash_const_iterator() {}

	reference operator*() const { return mCurrent->mValue; }
	const_pointer operator->() const { return &(operator*()); }
	bool operator==(const const_iterator &it) const { return mCurrent == it.mCurrent; }
	bool operator!=(const const_iterator &it) const { return mCurrent != it.mCurrent; }
	const_iterator operator++(int) { const_iterator tmp = *this; ++*this; return tmp; }
	const_iterator &operator++() {
		const Node *old = mCurrent;
		mCurrent = mCurrent->mNext;
		if (!mCurrent) {
			size_t bucket = mHashTable->BucketNum(old->mValue);
			while (!mCurrent && ++bucket < mHashTable->mBuckets.size()) {
				mCurrent = mHashTable->mBuckets[bucket];
			}
		}
		return *this;
	}

public:
	const Node *mCurrent;
	const HashTable *mHashTable;
};

template <class Key, class Value, class HashFn, class ExtractKey, class EqualKey>
class __hash_table {
public:
	typedef HashFn Hasher;
	typedef EqualKey KeyEqual;
	typedef Key key_type;
	typedef Value value_type;
	typedef value_type *pointer;
	typedef const value_type *const_pointer;
	typedef value_type &reference;
	typedef const value_type &const_reference;

	typedef __hash_iterator<Key, Value, HashFn, ExtractKey, EqualKey>		iterator;
	typedef __hash_const_iterator<Key, Value, HashFn, ExtractKey, EqualKey>	const_iterator;
	friend struct __hash_iterator<Key, Value, HashFn, ExtractKey, EqualKey>;
	friend struct __hash_const_iterator<Key, Value, HashFn, ExtractKey, EqualKey>;

public:
	__hash_table(size_t n, const HashFn &hf, const EqualKey &eql)
		: hash(hf) , equals(eql)
	{
		InitializeBuckets(n);
	}
	explicit __hash_table(size_t n) {
		InitializeBuckets(n);
	}
	__hash_table() {
		InitializeBuckets(64);
	}
	__hash_table(const __hash_table &ht)
		: hash(ht.hash), equals(ht.equals), get_key(ht.get_key), mNumElements(0)
	{
		CopyFrom(ht);
	}
	~__hash_table() { clear(); }
	__hash_table &operator=(const __hash_table &ht) {
		if (&ht != this) {
			clear();
			hash = ht.hash;
			equals = ht.equals;
			get_key = ht.get_key;
			CopyFrom(ht);
		}
		return *this;
	}

	size_t size() const { return mNumElements; }
	size_t max_size() const { return size_t(-1); }
	bool empty() const { return size() == 0; }

	iterator begin() {
		for (size_t n = 0; n < mBuckets.size(); ++n)
			if (mBuckets[n])
				return iterator(mBuckets[n], this);
		return end();
	}

	iterator end() { return iterator(0, this); }

	const_iterator begin() const {
		for (size_t n = 0; n < mBuckets.size(); ++n)
			if (mBuckets[n])
				return const_iterator(mBuckets[n], this);
		return end();
	}

	const_iterator end() const { return const_iterator(0, this); }

public:
	size_t bucket_count() const { return mBuckets.size(); }
	size_t max_bucket_count() const { return size_t(-1); }

	size_t elems_in_bucket(size_t bucket) const {
		size_t result = 0;
		for (Node *cur = mBuckets[bucket]; cur; cur = cur->mNext)
			result += 1;
		return result;
	}

	std::pair<iterator,bool> InsertUnique(const value_type &obj) {
		const size_t n = BucketNum(obj);
		Node *first = mBuckets[n];

		for (Node *cur = first; cur; cur = cur->mNext)
			if (equals(get_key(obj), get_key(cur->mValue)))
				return std::pair<iterator,bool>(iterator(cur, this), false);

		Node *tmp = NewNode(obj);
		tmp->mNext = first;
		mBuckets[n] = tmp;
		++mNumElements;
		return std::pair<iterator,bool>(iterator(tmp, this), true);
	}

	iterator insert_equal(const value_type &obj) {
		const size_t n = BucketNum(obj);
		Node *first = mBuckets[n];

		for (Node *cur = first; cur; cur = cur->mNext) {
			if (equals(get_key(cur->mValue), get_key(obj))) {
				Node *tmp = NewNode(obj);
				tmp->mNext = cur->mNext;
				cur->mNext = tmp;
				++mNumElements;
				return iterator(tmp, this);
			}
		}

		Node *tmp = NewNode(obj);
		tmp->mNext = first;
		mBuckets[n] = tmp;
		++mNumElements;
		return iterator(tmp, this);
	}

	iterator find(const key_type &key) {
		size_t n = BucketNumForKey(key);
		Node *first;
		for (first = mBuckets[n];
			first && !equals(get_key(first->mValue), key);
			first = first->mNext)
		{}
		return iterator(first, this);
	}

	const_iterator find(const key_type &key) const {
		size_t n = BucketNumForKey(key);
		const Node *first;
		for (first = mBuckets[n]
		; first && !equals(get_key(first->mValue), key)
			; first = first->mNext)
		{}
		return const_iterator(first, this);
	}

	bool IsExist(const key_type &key) const {
		size_t n = BucketNumForKey(key);
		const Node *first;
		for (first = mBuckets[n];
			first && !equals(get_key(first->mValue), key);
			first = first->mNext)
		{}
		return const_iterator(first, this) != end();
	}

	reference FindOrInsert(const value_type &obj) {
		size_t n = BucketNum(obj);
		Node *first = mBuckets[n];

		for (Node *cur = first; cur; cur = cur->mNext)
			if (equals(get_key(cur->mValue), get_key(obj)))
				return cur->mValue;

		Node *tmp = NewNode(obj);
		tmp->mNext = first;
		mBuckets[n] = tmp;
		++mNumElements;
		return tmp->mValue;
	}
	size_t count(const key_type &key) const {
		const size_t n = BucketNumForKey(key);
		size_t result = 0;

		for (const Node *cur = mBuckets[n]; cur; cur = cur->mNext)
			if (equals(get_key(cur->mValue), key))
				++result;
		return result;
	}

	std::pair<iterator, iterator> EqualRange(const key_type &key);
	std::pair<const_iterator, const_iterator> EqualRange(const key_type &key) const;

	size_t erase(const key_type &key) {
		const size_t n = BucketNumForKey(key);
		Node *first = mBuckets[n];
		size_t erased = 0;

		if (first) {
			Node *cur = first;
			Node *next = cur->mNext;
			while (next) {
				if (equals(get_key(next->mValue), key)) {
					cur->mNext = next->mNext;
					DeleteNode(next);
					next = cur->mNext;
					++erased;
					--mNumElements;
				} else {
					cur = next;
					next = cur->mNext;
				}
			}
			if (equals(get_key(first->mValue), key)) {
				mBuckets[n] = first->mNext;
				DeleteNode(first);
				++erased;
				--mNumElements;
			}
		}
		return erased;
	}
	void erase(const iterator &it) {
		if (Node *const p = it.mCurrent) {
			const size_t n = BucketNumForKey(get_key(p->mValue));
			Node *cur = mBuckets[n];

			if (cur == p) {
				mBuckets[n] = cur->mNext;
				DeleteNode(cur);
				--mNumElements;
			} else {
				Node *next = cur->mNext;
				while (next) {
					if (next == p) {
						cur->mNext = next->mNext;
						DeleteNode(next);
						--mNumElements;
						break;
					} else {
						cur = next;
						next = cur->mNext;
					}
				}
			}
		}
	}

	void erase(const const_iterator &it) {
		erase(iterator(const_cast<Node*>(it.mCurrent), const_cast<__hash_table*>(it.mHashTable)));
	}

	void clear() {
		for (size_t i = 0; i < mBuckets.size(); ++i) {
			Node *cur = mBuckets[i];
			while (cur != 0) {
				Node *next = cur->mNext;
				DeleteNode(cur);
				cur = next;
			}
			mBuckets[i] = 0;
		}
		mNumElements = 0;
	}

private:
	typedef __hash_node<Value>		Node;
	typedef Allocator<Node>	NodeAllocator;
	NodeAllocator _allocator;

	static size_t _Quantize(size_t size) {
		size_t n;
		for (n=1; n<size; n=n+n)
			;
		return n;
	}

	size_t BucketNumForKey(const key_type &key) const {
		return hash(key) & (mBuckets.size()-1);
	}

	size_t BucketNum(const value_type &obj) const {
		return BucketNumForKey(get_key(obj));
	}

	void InitializeBuckets(size_t n) {
		const size_t n_buckets = _Quantize(n);
		mBuckets.reserve(n_buckets);
		mBuckets.insert(mBuckets.end(), n_buckets, (Node*) 0);
		mNumElements = 0;
	}

	Node *NewNode(const value_type &obj) {
		Node *n = _allocator.allocate(1);
		n->mNext = 0;
		try {
			_construct(&n->mValue, obj);
			return n;
		} catch(...) {
			_allocator.deallocate(n, 1);
			throw;
		}
	}

	void DeleteNode(Node *n) {
		_destroy(&n->mValue);
		_allocator.deallocate(n, 1);
	}

	void EraseBucket(const size_t n, Node *first, Node *last);
	void EraseBucket(const size_t n, Node *last);

	void CopyFrom(const __hash_table &ht) {
		mBuckets.clear();
		mBuckets.reserve(ht.mBuckets.size());
		mBuckets.insert(mBuckets.end(), ht.mBuckets.size(), (Node*) 0);
		try {
			for (size_t i = 0; i < ht.mBuckets.size(); ++i) {
				if (const Node *cur = ht.mBuckets[i]) {
					Node *copy = NewNode(cur->mValue);
					mBuckets[i] = copy;

					for (Node *next = cur->mNext; next; cur = next, next = cur->mNext) {
						copy->mNext = NewNode(next->mValue);
						copy = copy->mNext;
					}
				}
			}
			mNumElements = ht.mNumElements;
		} catch(...) {
			clear(); throw;
		}
	}
private:
	Hasher hash;
	KeyEqual equals;
	ExtractKey get_key;
	std::vector<Node*>	mBuckets;
	size_t mNumElements;
};

/*-----------------------------------------------------------------------------
hash_set -- like std::hash_set
-----------------------------------------------------------------------------*/

template < class Value, class HashFn=__hash<Value>, class EqualKey=std::equal_to<Value> >
class hash_set {
private:
	typedef __hash_table<Value, Value, HashFn, identity<Value>, EqualKey >	ht;
	ht rep;

public:
	typedef typename ht::key_type key_type;
	typedef typename ht::value_type value_type;
	typedef typename ht::Hasher Hasher;
	typedef typename ht::KeyEqual KeyEqual;

	typedef typename ht::const_pointer const_pointer;
	typedef typename ht::const_reference reference;
	typedef typename ht::const_reference const_reference;

	typedef typename ht::const_iterator iterator;
	typedef typename ht::const_iterator const_iterator;

public:
	hash_set() : rep(DEFAULT_HASH_BUCKET_SIZE, Hasher(), KeyEqual()) {}
	explicit hash_set(size_t n) : rep(n, Hasher(), KeyEqual()) {}
	hash_set(size_t n, const Hasher &hf) : rep(n, hf, KeyEqual()) {}
	hash_set(size_t n, const Hasher &hf, const KeyEqual &eql) : rep(n, hf, eql) {}


public:
	size_t size() const { return rep.size(); }
	size_t max_size() const { return rep.max_size(); }
	bool empty() const { return rep.empty(); }

	iterator begin() const { return rep.begin(); }
	iterator end() const { return rep.end(); }

public:
	std::pair<iterator, bool> insert(const value_type &obj) {
		return rep.InsertUnique(obj);
	}

	iterator find(const key_type &key) const { return rep.find(key); }
	size_t count(const key_type &key) const { return rep.count(key); }

	size_t erase(const key_type &key) {return rep.erase(key); }
	void erase(iterator it) { rep.erase(it); }
	void clear() { rep.clear(); }

public:
	size_t bucket_count() const { return rep.bucket_count(); }
	size_t max_bucket_count() const { return rep.max_bucket_count(); }
	size_t elems_in_bucket(size_t n) const { return rep.elems_in_bucket(n); }
};

/*-----------------------------------------------------------------------------
hash_multiset -- like std::hash_multiset
-----------------------------------------------------------------------------*/

template < class Value, class HashFn=__hash<Value>, class EqualKey=std::equal_to<Value> >
class hash_multiset {
private:
	typedef __hash_table<Value, Value, HashFn, identity<Value>, EqualKey >	ht;
	ht rep;

public:
	typedef typename ht::Hasher Hasher;
	typedef typename ht::KeyEqual KeyEqual;

	typedef typename ht::key_type key_type;
	typedef typename ht::value_type value_type;

	typedef typename ht::pointer pointer;
	typedef typename ht::const_pointer const_pointer;
	typedef typename ht::const_reference reference;
	typedef typename ht::const_reference const_reference;

	typedef typename ht::iterator iterator;
	typedef typename ht::const_iterator const_iterator;

public:
	hash_multiset() : rep(DEFAULT_HASH_BUCKET_SIZE, Hasher(), KeyEqual()) {}
	explicit hash_multiset(size_t n) : rep(n, Hasher(), KeyEqual()) {}
	hash_multiset(size_t n, const Hasher &hf) : rep(n, hf, KeyEqual()) {}
	hash_multiset(size_t n, const Hasher &hf, const KeyEqual &eql) : rep(n, hf, eql) {}


public:
	size_t size() const { return rep.size(); }
	size_t max_size() const { return rep.max_size(); }
	bool empty() const { return rep.empty(); }

	const_iterator begin() const { return rep.begin(); }
	const_iterator end() const { return rep.end(); }

public:
	iterator insert(const value_type &obj) {
		return rep.insert_equal(obj);
	}

	const_iterator find(const key_type &key) const { return rep.find(key); }
	size_t count(const key_type &key) const { return rep.count(key); }

	size_t erase(const key_type &key) {return rep.erase(key); }
	void erase(iterator it) { rep.erase(it); }
	void clear() { rep.clear(); }

public:
	size_t bucket_count() const { return rep.bucket_count(); }
	size_t max_bucket_count() const { return rep.max_bucket_count(); }
	size_t elems_in_bucket(size_t n) const { return rep.elems_in_bucket(n); }
};

/*-----------------------------------------------------------------------------
hash_map -- like std::hash_map
-----------------------------------------------------------------------------*/
template <class Key
	,class T
	,class HashFcn = __hash<Key>
	,class EqualKey = std::equal_to<Key> >
class hash_map {
private:
	typedef __hash_table< Key
		, std::pair<const Key, T>
		, HashFcn
		, select1st<std::pair<const Key, T> >
		, EqualKey> ht;
	ht rep;

public:
	typedef typename ht::key_type key_type;
	typedef T data_type;
	typedef T mapped_type;
	typedef typename ht::value_type value_type;
	typedef typename ht::Hasher Hasher;
	typedef typename ht::KeyEqual KeyEqual;

	typedef typename ht::pointer pointer;
	typedef typename ht::const_pointer const_pointer;
	typedef typename ht::reference reference;
	typedef typename ht::const_reference const_reference;

	typedef typename ht::iterator iterator;
	typedef typename ht::const_iterator const_iterator;

public:
	hash_map() : rep(DEFAULT_HASH_BUCKET_SIZE, Hasher(), KeyEqual()) {}
	explicit hash_map(size_t n) : rep(n, Hasher(), KeyEqual()) {}
	hash_map(size_t n, const Hasher &hf) : rep(n, hf, KeyEqual()) {}
	hash_map(size_t n, const Hasher &hf, const KeyEqual &eql) : rep(n, hf, eql) {}


public:
	size_t size() const { return rep.size(); }
	size_t max_size() const { return rep.max_size(); }
	bool empty() const { return rep.empty(); }

	iterator begin() { return rep.begin(); }
	iterator end() { return rep.end(); }
	const_iterator begin() const { return rep.begin(); }
	const_iterator end() const { return rep.end(); }

public:
	std::pair<iterator, bool>	insert(const value_type &obj) { return rep.InsertUnique(obj); }
	std::pair<iterator, bool>	insert(const key_type &key, const data_type &data)
	{ return rep.InsertUnique(std::make_pair(key, data)); }
	iterator find(const key_type &key) { return rep.find(key); }
	const_iterator find(const key_type &key) const { return rep.find(key); }
	bool exist(const key_type &key) const { return rep.IsExist(key); }
	size_t count(const key_type &key) const { return rep.count(key); }
	size_t erase(const key_type &key) {return rep.erase(key); }
	void erase(iterator it) { rep.erase(it); }
	void clear() { rep.clear(); }

	T &operator[](const key_type &key) {
		return rep.FindOrInsert(value_type(key, T())).second;
	}

public:
	size_t bucket_count() const { return rep.bucket_count(); }
	size_t max_bucket_count() const { return rep.max_bucket_count(); }
	size_t elems_in_bucket(size_t n) const { return rep.elems_in_bucket(n); }
};

/*-----------------------------------------------------------------------------
hash_multimap -- like std::hash_multimap
-----------------------------------------------------------------------------*/
template <class Key
	,class T
	,class HashFcn = __hash<Key>
	,class EqualKey = std::equal_to<Key> >
class hash_multimap {
private:
	typedef __hash_table< Key
		, std::pair<const Key, T>
		, HashFcn
		, select1st<std::pair<const Key, T> >
		, EqualKey> ht;
	ht rep;

public:
	typedef typename ht::key_type key_type;
	typedef T data_type;
	typedef T mapped_type;
	typedef typename ht::value_type value_type;
	typedef typename ht::Hasher Hasher;
	typedef typename ht::KeyEqual KeyEqual;

	typedef typename ht::const_pointer const_pointer;
	typedef typename ht::reference reference;
	typedef typename ht::const_reference const_reference;

	typedef typename ht::iterator iterator;
	typedef typename ht::const_iterator const_iterator;

public:
	hash_multimap() : rep(DEFAULT_HASH_BUCKET_SIZE, Hasher(), KeyEqual()) {}
	explicit hash_multimap(size_t n) : rep(n, Hasher(), KeyEqual()) {}
	hash_multimap(size_t n, const Hasher &hf) : rep(n, hf, KeyEqual()) {}
	hash_multimap(size_t n, const Hasher &hf, const KeyEqual &eql) : rep(n, hf, eql) {}


public:
	size_t size() const { return rep.size(); }
	size_t max_size() const { return rep.max_size(); }
	bool empty() const { return rep.empty(); }

	iterator begin() { return rep.begin(); }
	iterator end() { return rep.end(); }
	const_iterator begin() const { return rep.begin(); }
	const_iterator end() const { return rep.end(); }

public:
	iterator insert(const value_type &obj) { return rep.insert_equal(obj); }
	iterator insert(const key_type &key, const data_type &data)
	{ return rep.insert_equal(std::make_pair(key, data)); }
	iterator find(const key_type &key) { return rep.find(key); }
	const_iterator find(const key_type &key) const { return rep.find(key); }
	size_t count(const key_type &key) const { return rep.count(key); }
	size_t erase(const key_type &key) {return rep.erase(key); }
	void erase(iterator it) { rep.erase(it); }
	void clear() { rep.clear(); }

public:
	size_t bucket_count() const { return rep.bucket_count(); }
	size_t max_bucket_count() const { return rep.max_bucket_count(); }
	size_t elems_in_bucket(size_t n) const { return rep.elems_in_bucket(n); }
};


/// 
inline size_t hash_istring(const char *s) {
	if (!s)
		return 0;

	uint_t h = 0;
	for (; *s; ++s)
		h = 5*h + tolower(*s);

	return size_t(h);
}

/// 
inline size_t hash_filename(const char *s) {
	if (!s)
		return 0;

	uint_t h = 0;
	for (; *s; ++s) {
		char letter = *s;
		if (letter=='\\') {
			letter='/';
		}
		h = 5*h + tolower(letter);
	}

	return size_t(h);
}

/// 
inline size_t hash_istring(const wchar_t *s) {
	if (!s)
		return 0;

	uint_t h = 0;
	for (; *s; ++s)
		h = 5*h + tolower(*s);

	return size_t(h);
}

/// 
inline size_t hash_filename(const wchar_t *s) {
	if (!s)
		return 0;

	uint_t h = 0;
	for (; *s; ++s) {
		char letter = *s;
		if (letter=='\\') {
			letter='/';
		}
		h = 5 * h + tolower(letter);
	}

	return size_t(h);
}

struct hash_str {
	inline size_t operator()(const String &s) const { return __hash<const char*>()(s.c_str()); }
	inline size_t operator()(const char *s) const { return __hash<const char*>()(s); }
};

struct hash_istr {
	inline size_t operator()(const String &s) const { return hash_istring(s.c_str()); }
	inline size_t operator()(const char *s) const { return hash_istring(s); }
};

struct hash_pathname {
	inline size_t operator()(const String &s) const { return hash_filename(s.c_str()); }
	inline size_t operator()(const char *s) const { return hash_filename(s); }
};

inline char __tofilenamechar(char c) {
	c = tolower(c);
	if (c == '\\') c = '/';
	return c;
}

struct equal_pathname {
	inline bool operator()(const String &s1, const String &s2) const {
		return operator()(s1.c_str(), s2.c_str());
	}
	inline bool operator()(const char *s1, const char *s2) const {
		for (uint_t i = 0; *s1 && *s2; i++, s1++, s2++) {
			if (__tofilenamechar(*s1) != __tofilenamechar(*s2))
				return false;
		}
		if (*s1 || *s2)	// if one isn't end
			return false;

		return true;
	}
};

struct equal_istr {
	inline bool operator()(const String &s1, const String &s2) const {
		return StringUtil::stricmp(s1.c_str(), s2.c_str()) == 0;
	}
	inline bool operator()(const char *s1, const char *s2) const {
		return StringUtil::stricmp(s1, s2) == 0;
	}
};

struct equal_str {
	inline bool operator()(const String &s1, const String &s2) const {
		return strcmp(s1.c_str(), s2.c_str()) == 0;
	}
	inline bool operator()(const char *s1, const char *s2) const {
		return strcmp(s1, s2) == 0;
	}
};

template <class Key
	,class T
	,class HashFcn = hash_pathname
	,class EqualKey = equal_pathname >
class PathMap : public hash_map< Key,T,HashFcn,EqualKey >
{};

}

#endif // end guardian
#endif

AX_BEGIN_NAMESPACE

/// 
inline size_t hash_string(const char *s) {
	unsigned long h = 0;
	for (; *s; ++s)
		h = 5*h + *s;

	return size_t(h);
}

inline size_t hash_string(const wchar_t *s) {
	unsigned long h = 0;
	for (; *s; ++s)
		h = 5*h + *s;

	return size_t(h);
}

/// 
inline size_t hash_istring(const char *s) {
	if (!s)
		return 0;

	uint_t h = 0;
	for (; *s; ++s)
		h = 5*h + tolower(*s);

	return size_t(h);
}

/// 
inline size_t hash_filename(const char *s) {
	if (!s)
		return 0;

	uint_t h = 0;
	for (; *s; ++s) {
		char letter = *s;
		if (letter=='\\') {
			letter='/';
		}
		h = 5*h + tolower(letter);
	}

	return size_t(h);
}

/// 
inline size_t hash_istring(const wchar_t *s) {
	if (!s)
		return 0;

	uint_t h = 0;
	for (; *s; ++s)
		h = 5*h + tolower(*s);

	return size_t(h);
}

/// 
inline size_t hash_filename(const wchar_t *s) {
	if (!s)
		return 0;

	uint_t h = 0;
	for (; *s; ++s) {
		char letter = *s;
		if (letter=='\\') {
			letter='/';
		}
		h = 5 * h + tolower(letter);
	}

	return size_t(h);
}

struct hash_cstr {
	inline size_t operator()(const char *s) const { return hash_string(s); }
};

struct hash_istr {
	inline size_t operator()(const String &s) const { return hash_istring(s.c_str()); }
	inline size_t operator()(const char *s) const { return hash_istring(s); }
};

struct hash_pathname {
	inline size_t operator()(const String &s) const { return hash_filename(s.c_str()); }
	inline size_t operator()(const char *s) const { return hash_filename(s); }
};

inline char __tofilenamechar(char c) {
	c = tolower(c);
	if (c == '\\') c = '/';
	return c;
}

struct equal_pathname {
	inline bool operator()(const String &s1, const String &s2) const {
		return operator()(s1.c_str(), s2.c_str());
	}
	inline bool operator()(const char *s1, const char *s2) const {
		for (uint_t i = 0; *s1 && *s2; i++, s1++, s2++) {
			if (__tofilenamechar(*s1) != __tofilenamechar(*s2))
				return false;
		}
		if (*s1 || *s2)	// if one isn't end
			return false;

		return true;
	}
};

struct equal_istr {
	inline bool operator()(const String &s1, const String &s2) const {
		return StringUtil::stricmp(s1.c_str(), s2.c_str()) == 0;
	}
	inline bool operator()(const char *s1, const char *s2) const {
		return StringUtil::stricmp(s1, s2) == 0;
	}
};

struct equal_cstr {
	inline bool operator()(const char *s1, const char *s2) const {
		return strcmp(s1, s2) == 0;
	}
};

template<typename T>
void hash_combine(size_t & seed, T const & v) {
	seed ^= std::tr1::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

AX_END_NAMESPACE