/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_LINK_H
#define AX_LINK_H

AX_BEGIN_NAMESPACE

#if 0
template< class type >
class Link {
public:
	Link();
	~Link();

	bool isEmptyList( void ) const;
	bool isInList( void ) const;
	int getListSize( void ) const;
	void clearList( void );

	void insertBefore( Link &node );
	void insertAfter( Link &node );
	void addToEnd( Link &node );
	void addToFront( Link &node );

	void removeFromList( void );

	type *getNext( void ) const;
	type *getPrev( void ) const;

	type *getOwner( void ) const;
	void setOwner( type *object );

	Link *getHead( void ) const;
	Link *getNextNode( void ) const;
	Link *getPrevNode( void ) const;

private:
	Link *m_head;
	Link *m_next;
	Link *m_prev;
	type *m_owner;
};

template< class type >
Link<type>::Link() {
	m_owner = 0;
	m_head = this;	
	m_next = this;
	m_prev = this;
}

template< class type >
Link<type>::~Link() {
	clearList();
}

template< class type >
bool Link<type>::isEmptyList( void ) const {
	return m_head->m_next == m_head;
}

template< class type >
bool Link<type>::isInList( void ) const {
	return m_head != this;
}

template< class type >
int Link<type>::getListSize( void ) const {
	Link<type> *node;
	int num;

	num = 0;
	for( node = m_head->m_next; node != m_head; node = node->m_next ) {
		num++;
	}

	return num;
}

template< class type >
void Link<type>::clearList( void ) {
	if ( m_head == this ) {
		while( m_next != this ) {
			m_next->removeFromList();
		}
	} else {
		removeFromList();
	}
}

template< class type >
void Link<type>::removeFromList( void ) {
	m_prev->m_next = m_next;
	m_next->m_prev = m_prev;

	m_next = this;
	m_prev = this;
	m_head = this;
}

template< class type >
void Link<type>::insertBefore( Link &node ) {
	removeFromList();

	m_next = &node;
	m_prev = node.m_prev;
	node.m_prev = this;
	m_prev->m_next = this;
	m_head = node.m_head;
}

template< class type >
void Link<type>::insertAfter( Link &node ) {
	removeFromList();

	m_prev = &node;
	m_next = node.m_next;
	node.m_next = this;
	m_next->m_prev = this;
	m_head = node.m_head;
}

template< class type >
void Link<type>::addToEnd( Link &node ) {
	insertBefore( *node.m_head );
}

template< class type >
void Link<type>::addToFront( Link &node ) {
	insertAfter( *node.m_head );
}

template< class type >
Link<type> *Link<type>::getHead( void ) const {
	return m_head;
}

template< class type >
type *Link<type>::getNext( void ) const {
	if ( !m_next || ( m_next == m_head ) ) {
		return NULL;
	}
	return m_next->m_owner;
}

template< class type >
type *Link<type>::getPrev( void ) const {
	if ( !m_prev || ( m_prev == m_head ) ) {
		return NULL;
	}
	return m_prev->m_owner;
}

template< class type >
Link<type> *Link<type>::getNextNode( void ) const {
	if ( m_next == m_head ) {
		return NULL;
	}
	return m_next;
}

template< class type >
Link<type> *Link<type>::getPrevNode( void ) const {
	if ( m_prev == m_head ) {
		return NULL;
	}
	return m_prev;
}

template< class type >
type *Link<type>::getOwner( void ) const {
	return m_owner;
}

template< class type >
void Link<type>::setOwner( type *object ) {
	m_owner = object;
}

// Copyright 2009 Netease Inc. All Rights Reserved.
// Author: tangxiliu@corp.netease.com (Tang Xi Liu)

// An IntrusiveList<> is a doubly-linked list where the link pointers are
// embedded in the items stored in the list. Intrusive lists are circularly
// linked making inserting and removing an element constant time (and
// branch-free) operations if you already have the position you want to
// insert/remove at. Usage is similar to an STL list<>, with the addition of
// needing to embed an IntrusiveLink<> structure in the items you want to
// store in your IntrusiveList<>.
//
//   struct Foo {
//     IntrusiveLink<Foo> link;
//   };
//   typedef IntrusiveList<Foo, &Foo::link> FooList;
//
//   FooList l;
//   l.push_back(new Foo);
//   l.push_front(new Foo);
//   l.erase(&l.front());
//   l.erase(&l.back());
//
// Intrusive lists are primarily useful when you would have considered
// embedding link pointers in your class directly for space or performance
// reasons. An IntrusiveLink<> is the size of 2 pointers, usually 8
// bytes. Intrusive lists do not perform memory allocation (unlike the STL
// list<>) class and thus may use less memory than list<>. In particular, if
// the list elements are pointers to objects, using a list<> would perform an
// extra memory allocation for each list node structure, while an
// IntrusiveList<> would not.
//
// Because the list pointers are embedded in the objects stored in an
// IntrusiveList<>, erasing an item from a list is constant time. Consider the
// following:
//
//   map<string,Foo> foo_map;
//   list<Foo*> foo_list;
//
//   foo_list.push_back(&foo_map["bar"]);
//   foo_list.erase(&foo_map["bar"]); // Compile error!
//
// The problem here is that a Foo* doesn't know where on foo_list it resides,
// so removal requires iteration over the list. Various tricks can be performed
// to overcome this. For example, a foo_list::iterator can be stored inside of
// the Foo object. But at that point you'd be better of using an
// IntrusiveList<>:
//
//   map<string,Foo> foo_map;
//   IntrusiveList<Foo> foo_list;
//
//   foo_list.push_back(&foo_map["bar"]);
//   foo_list.erase(&foo_map["bar"]); // Yeah!
//
// Note that intrusive_lists come with a few limitations. The primary
// limitation is that the IntrusiveLink<> structure is not copyable or
// assignable. The result is that STL algorithms which mutate the order of
// iterators, such as reverse() and unique(), will not work by default with
// intrusive_lists. In order to allow these algorithms to work you'll need to
// define swap() and/or operator= for your class. And these routines must not
// swap or assign to the link pointers.
//
// Another limitation is that the IntrusiveList<> structure itself is not
// copyable or assignable since an item/link combination can only exist on one
// IntrusiveList<> at a time. This limitation is a result of the link pointers
// for an item being intrusive in the item itself. For example, the following
// will not compile:
//
//   FooList a;
//   FooList b(a); // no copy constructor
//   b = a;        // no assignment operator
//
// The similar STL code does work since the link pointers are external to the
// item:
//
//   list<int*> a;
//   a.push_back(new int);
//   list<int*> b(a);
//   CHECK(a.front() == b.front());

// The IntrusiveLink structure provides the next and previous pointers for an
// intrusive list item. The structure is intended to be intrusive in the class
// you want to store on an IntrusiveList<>. See the example above.
template <class T>
struct IntrusiveLink {
	IntrusiveLink() : next(NULL), prev(NULL) { }

	T *next;
	T *prev;

	// Returns true if the object is a member of some list.
	bool isLinked() const { return next != NULL; }

private:
	// Disabled copy constructor and assignment operator.
	IntrusiveLink(const IntrusiveLink<T> &link);
	IntrusiveLink<T>& operator=(const IntrusiveLink<T> &x);
};

template <class T, IntrusiveLink<T> (T::*link_ptr) = &T::m_link>
class IntrusiveList {
private:
	// The structure used for iterating over an IntrusiveList.
	template <class Ref, class Ptr>
	struct IntrusiveListIterator {
		typedef IntrusiveListIterator<T&, T*> iterator;
		typedef IntrusiveListIterator<const T&, const T*> const_iterator;
		typedef IntrusiveListIterator<Ref, Ptr> self;

		typedef std::bidirectional_iterator_tag iterator_category;
		typedef T value_type;
		typedef Ptr pointer;
		typedef const T *const_pointer;
		typedef Ref reference;
		typedef const T &const_reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;

		IntrusiveListIterator() : node(NULL) { }
		IntrusiveListIterator(pointer x) : node(x) { }
		IntrusiveListIterator(const iterator &x) : node(x.node) { }

		bool operator==(const const_iterator &x) const { return node == x.node; }
		bool operator!=(const const_iterator &x) const { return node != x.node; }

		reference operator*() const { return *node; }
		pointer operator->() const { return node; }

		self& operator++() { node = (node->*link_ptr).next; return *this; }
		self  operator++(int) { self tmp = *this; ++*this; return tmp; }
		self& operator--() { node = (node->*link_ptr).prev; return *this; }
		self  operator--(int) { self tmp = *this; --*this; return tmp; }

		// The node pointer needs to be public so that we can access it from
		// different IntrusiveListIterator<> specializations. (e.g. From the
		// copy-constructor from an iterator to a const_iterator).
		pointer node;
	};

	// Okay, here is the hokey and not quite safe part of the IntrusiveList<>
	// implementation. Basically, we want to construct a T* so that when we
	// dereference the pointer-to-data-member (i.e. link_ptr) we'll arrive back
	// at "this". Huh? We just want the following to work:
	//
	//   IntrusiveList<T,link_ptr> list;
	//   T *obj = list.end_node();
	//   assert(&(obj->*link_ptr) == &list);
	//
	// The reason for wanting this is that IntrusiveList<T> is a circularly
	// linked list with "this" acting as the sentinel node at the end of the
	// list. But "this" is of type IntrusiveList<T> and not T. Which is where
	// the hokey part comes in. The way pointers-to-data-members are implemented
	// for "complex" classes isn't necessarily a simple integer offset. Applying
	// the pointer-to-data-member (i.e. doing "obj->*link_ptr") for a virtual
	// base data member to a derived pointer will need to look inside "obj" to
	// find the offset of the virtual base. Consider the following:
	//
	// struct virtual_base {
	//   int x;
	// };
	//
	// struct derived : public virtual virtual_base {
	//   int offset_of(int virtual_base::*member_ptr) {
	//     return reinterpret_cast<char*>(&this->*member_ptr) -
	//         reinterpret_cast<char*>(this);
	//   }
	// };
	//
	// When "member_ptr" is applied to "this" in derived::offset_of(), the
	// compiler first casts "this" to a "virtual_base". Unlike for non-virtual
	// base classes, this involves looking inside of "this" to determine the
	// offset of the virtual base class from "this". (For non-virtual base
	// classes, the offset of a parent class can be determined at compile
	// time). In the above example, the cast from "this" to "virtual_base*" works
	// fine. The trickery below produces a fake T* which is not a pointer to a
	// valid T object. Looking inside this fake T* would be bad and could cause
	// crashes or other misbehavior. On the bright side, the trickery below
	// detects at runtime when application of the "link_ptr" needs to look inside
	// the object by applying "link_ptr" to "reinterpret_cast<T*>(0)". If
	// application of "link_ptr" needs to look inside the "T*", we'll get a nice
	// NULL pointer dereference. Even better, because "link_ptr" is a "T::*"
	// pointer-to-data-member, application of "link_ptr" to a "T*" does not need
	// to implicitly cast "T*" to a different type so the construction of
	// "end_node()" should always be safe.

	// Returns a pointer to the "end" node of the IntrusiveList. This is a
	// specially constructed T* which is really a pointer to "this". See the
	// discussion above.
	T* end_node() const {
		// This effectively calculates offsetof(T, link_ptr) by first creating a
		// fake T* from address 0 and applying link_ptr to find the address of the
		// link structure, and then casting the result to an integer type. Note
		// that a reasonable compiler (e.g. gcc-4) can compute this at compile time
		// and will end up performing only the link_offset subtraction in this
		// method at runtime.

		// Note that the following statement makes two assumptions that might be
		// problematic someday: a) assumes that a null pointer, cast to int, will
		// have the integer value 0 and b) using a pointer-to-data-member with a
		// null pointer constant for which a sufficiently smart compiler might warn
		// about.
		const ptrdiff_t link_offset =
			reinterpret_cast<ptrdiff_t>(
			&(reinterpret_cast<const T*>(0)->*link_ptr));

		// Now we construct our fake T* by subtracting the link offset from the
		// start address of "this".
		return reinterpret_cast<T*>(
			const_cast<char*>(reinterpret_cast<const char*>(this) - link_offset));
	}

public:
	typedef T value_type;
	typedef value_type *pointer;
	typedef const value_type *const_pointer;
	typedef value_type &reference;
	typedef const value_type &const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef typename IntrusiveListIterator<T&, T*> iterator;
	typedef IntrusiveListIterator<const T&, const T*> const_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;

public:
#if 0
	// Transfers the contents of (i, end) to dest.
	static void transfer(intrusive_list<T, link_ptr> *src,
		intrusive_list<T, link_ptr>::iterator i,
		intrusive_list<T, link_ptr> *dest) {
	}
#endif
	// Transfers the contents of one intrusive_list to another.
	static void transfer(IntrusiveList<T,link_ptr> *src,
		IntrusiveList<T,link_ptr> *dest) {
			if (!src->empty()) {
				dest->m_link.next = src->m_link.next;
				dest->m_link.prev = src->m_link.prev;
				(dest->m_link.next->*link_ptr).prev = dest->end_node();
				(dest->m_link.prev->*link_ptr).next = dest->end_node();
				src->clear();
			}
	}

	// Constructor.
	IntrusiveList() {
		m_link.next = m_link.prev = end_node();
		m_size = 0;
	}

	// Iterator routines.
	iterator begin() { return iterator(m_link.next); }
	const_iterator begin() const { return const_iterator(m_link.next); }
	iterator end() { return iterator(end_node()); }
	const_iterator end() const { return const_iterator(end_node()); }
	reverse_iterator rbegin() { return reverse_iterator(end()); }
	const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
	reverse_iterator rend() { return reverse_iterator(begin()); }
	const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

	// Size routines.
	bool empty() const { return (m_link.next == end_node()); }
	size_type size() const { /*return distance(begin(), end());*/ return m_size; }
	size_type max_size() const { return size_type(-1); }

	// Front and back accessors.
	reference front() { return *m_link.next; }
	const_reference front() const { return *m_link.next; }
	reference back() { return *m_link.prev; }
	const_reference back() const { return *m_link.prev; }

	// Insertion routines.
	iterator insert(T *position, T *obj) {
		T *next = position;
		T *prev = (position->*link_ptr).prev;
		(next->*link_ptr).prev = obj;
		(obj->*link_ptr).next = next;
		(obj->*link_ptr).prev = prev;
		(prev->*link_ptr).next = obj;
		m_size++;
		return iterator(obj);
	}
	static iterator insert(iterator position, T *obj) {
		return insert(position.node, obj);
	}
	void push_front(T *obj) { insert(m_link.next, obj); }
	void push_back(T *obj) { insert(end_node(), obj); }

	// Removal routines.
	iterator erase(T *obj) {
		// Fix up the next and previous links for the previous and next objects.
		T *next = (obj->*link_ptr).next;
		T *prev = (obj->*link_ptr).prev;
		(next->*link_ptr).prev = prev;
		(prev->*link_ptr).next = next;
		// Zero out the next and previous links for the removed item. This will
		// cause any future attempt to remove the item from the list to cause a
		// crash instead of possibly corrupting the list structure.
		(obj->*link_ptr).next = NULL;
		(obj->*link_ptr).prev = NULL;
		m_size--;
		return iterator(next);
	}
	iterator erase(iterator position) { return erase(position.node); }
	void pop_front() { erase(m_link.next); }
	void pop_back() { erase(m_link.prev); }

	// Utility routines.
	void clear() {
		m_link.next = m_link.prev = end_node();
	}
	void swap(IntrusiveList<T,link_ptr> &x) {
		IntrusiveList<T,link_ptr> tmp;
		transfer(this, &tmp);
		transfer(&x, this);
		transfer(&tmp, &x);
	}

private:
	IntrusiveLink<T> m_link;
	size_type m_size;

private:
	// Disabled copy constructor and assignment operator.
	IntrusiveList(const IntrusiveList<T,link_ptr> &x);
	IntrusiveList<T,link_ptr>& operator=(const IntrusiveList<T,link_ptr> &x);
};
#endif

template <class type>
class IntrusiveLink {
public:
	IntrusiveLink() : next(0), prev(0) {}
	~IntrusiveLink() {}

	bool isLinked() const { return next != 0; }
	void unlink()
	{
		if (!isLinked()) return;

		prev->next = next;
		next->prev = prev;

		next = 0;
		prev = 0;
	}

	IntrusiveLink *next;
	IntrusiveLink *prev;
};

template <class T, IntrusiveLink<T> (T::*link_ptr) = &T::m_link>
class IntrusiveList {
private:
	// The structure used for iterating over an IntrusiveList.
	typedef IntrusiveList<T, link_ptr> ListType;

	template <class Ref, class Ptr>
	struct Iterator {
		typedef Iterator<T&, T*> iterator;
		typedef Iterator<const T&, const T*> const_iterator;
		typedef Iterator<Ref, Ptr> self;

		typedef std::bidirectional_iterator_tag iterator_category;
		typedef T value_type;
		typedef Ptr pointer;
		typedef const T *const_pointer;
		typedef Ref reference;
		typedef const T &const_reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;

		Iterator() : obj(0) { }
		Iterator(pointer x) : obj(x) { }
		Iterator(const iterator &x) : obj(x.obj) { }

		bool operator==(const const_iterator &x) const { return obj == x.obj; }
		bool operator!=(const const_iterator &x) const { return obj != x.obj; }

		reference operator*() const { return *obj; }
		pointer operator->() const { return obj; }

		self& operator++() { obj = ListType::node2obj((obj->*link_ptr).next); return *this; }
		self  operator++(int) { self tmp = *this; ++*this; return tmp; }
		self& operator--() { obj = ListType::node2obj((obj->*link_ptr).prev); return *this; }
		self  operator--(int) { self tmp = *this; --*this; return tmp; }

		pointer obj;
	};

	IntrusiveLink<T>* end_node() const {
		return &m_link;
	}

	T* end_obj() const {
		return node2obj(&m_link);
	}

	static T* node2obj(const IntrusiveLink<T> *node) {
		const ptrdiff_t link_offset = reinterpret_cast<ptrdiff_t>(&(reinterpret_cast<const T*>(0)->*link_ptr));

		return reinterpret_cast<T*>(const_cast<char*>(reinterpret_cast<const char*>(node) - link_offset));
	}

	static IntrusiveLink<T>* obj2node(T *obj) {
		return &(obj->*link_ptr);
	}

public:
	typedef T value_type;
	typedef value_type *pointer;
	typedef const value_type *const_pointer;
	typedef value_type &reference;
	typedef const value_type &const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef typename Iterator<T&, T*> iterator;
	typedef Iterator<const T&, const T*> const_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;

public:
	// Constructor.
	IntrusiveList()
	{
		m_link.next = m_link.prev = &m_link;
		m_maxUsed = 0;
	}

	// Iterator routines.
	iterator begin() { return iterator(node2obj(m_link.next)); }
	const_iterator begin() const { return const_iterator(node2obj(m_link.next)); }
	iterator end() { return iterator(end_obj()); }
	const_iterator end() const { return const_iterator(end_obj()); }
	reverse_iterator rbegin() { return reverse_iterator(end()); }
	const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
	reverse_iterator rend() { return reverse_iterator(begin()); }
	const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

	// Size routines.
	bool empty() const { return (m_link.next == end_node()); }
	size_type size() const { m_maxUsed = std::distance(begin(), end()); return m_maxUsed; }
	size_type max_size() const { return size_type(-1); }
	size_type max_used() const { return m_maxUsed; }

	// Front and back accessors.
	pointer front() { return node2obj(m_link.next); }
	const_pointer front() const { return node2obj(m_link.next); }
	pointer back() { return node2obj(m_link.prev); }
	const_pointer back() const { return node2obj(m_link.prev); }

	// Insertion routines.
	iterator insert(T *position, T *obj)
	{
		IntrusiveLink<T> *objLink = obj2node(obj);
		IntrusiveLink<T> *posLink = obj2node(position);
		objLink->unlink();
		objLink->next = posLink;
		objLink->prev = posLink->prev;
		posLink->prev = objLink;
		objLink->prev->next = objLink;
		m_maxUsed++;
		return iterator(obj);
	}

	iterator insert(iterator position, T *obj)
	{
		return insert(position.obj, obj);
	}

	void push_front(T *obj) { insert(front(), obj); }
	void push_back(T *obj) { insert(end_obj(), obj); }

	// Removal routines.
	iterator erase(T *obj) {
		IntrusiveLink<T> *link = &(obj->*link_ptr);
		IntrusiveLink<T> *next = link->next;
		link->prev->next = link->next;
		link->next->prev = link->prev;
		link->prev = link->next = 0;
		m_maxUsed--;
		return iterator(node2obj(next));
	}
	iterator erase(iterator position) { return erase(position.obj); }
	void pop_front() { erase(front()); }
	void pop_back() { erase(back()); }

	// Utility routines.
	void clear() {
		iterator it = begin();
		while (it != end())
			it = erase(it);
		m_maxUsed = 0;
	}

private:
	IntrusiveLink<T> m_link;
	mutable size_type m_maxUsed;

private:
	// Disabled copy constructor and assignment operator.
	IntrusiveList(const IntrusiveList<T,link_ptr> &x);
	IntrusiveList<T,link_ptr>& operator=(const IntrusiveList<T,link_ptr> &x);
};

AX_END_NAMESPACE

#endif
