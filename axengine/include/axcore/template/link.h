/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_LINK_H
#define AX_LINK_H

namespace Axon {

	template< class type >
	class Link {
	public:
		Link();
		~Link();

		bool isEmpty( void ) const;
		bool isInList( void ) const;
		int size( void ) const;
		void clear( void );

		void insertBefore( Link &node );
		void insertAfter( Link &node );
		void addToEnd( Link &node );
		void addToFront( Link &node );

		void remove( void );

		type* getNext( void ) const;
		type* getPrev( void ) const;

		type* getOwner( void ) const;
		void setOwner( type *object );

		Link* getHead( void ) const;
		Link* getNextNode( void ) const;
		Link* getPrevNode( void ) const;

	private:
		Link* m_head;
		Link* m_next;
		Link* m_prev;
		type* m_owner;
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
		clear();
	}

	template< class type >
	bool Link<type>::isEmpty( void ) const {
		return m_head->m_next == m_head;
	}

	template< class type >
	bool Link<type>::isInList( void ) const {
		return m_head != this;
	}

	template< class type >
	int Link<type>::size( void ) const {
		idLinkList<type>	*node;
		int num;

		num = 0;
		for( node = m_head->m_next; node != m_head; node = node->m_next ) {
			num++;
		}

		return num;
	}

	template< class type >
	void Link<type>::clear( void ) {
		if ( m_head == this ) {
			while( m_next != this ) {
				m_next->remove();
			}
		} else {
			remove();
		}
	}

	template< class type >
	void Link<type>::remove( void ) {
		m_prev->m_next = m_next;
		m_next->m_prev = m_prev;

		m_next = this;
		m_prev = this;
		m_head = this;
	}

	template< class type >
	void Link<type>::insertBefore( Link &node ) {
		remove();

		m_next = &node;
		m_prev = node.m_prev;
		node.m_prev = this;
		m_prev->m_next = this;
		m_head = node.m_head;
	}

	template< class type >
	void Link<type>::insertAfter( Link &node ) {
		remove();

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

} // namespace Axon

#endif
