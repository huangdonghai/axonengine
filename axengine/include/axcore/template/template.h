/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


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

#ifndef AX_TEMPLATE_H
#define AX_TEMPLATE_H

#include "link.h"
#include "enum.h"
#include "sharedptr.h"
#include "hash.h"
#include "nullptr.h"
#include "static_assert.h"
#include "foreach.h"

AX_BEGIN_NAMESPACE

	inline int s2i(size_t size) {
#ifdef _DEBUG
		if (size >= (size_t)std::numeric_limits<int>::max())
			Errorf("%s: overflowed", __func__);
#endif
		return (int)size;
	}

	template < class T >
	void DeletePointerArray(std::vector<T*>& array) {
		for (size_t i=0; i<array.size(); i++) {
			SafeDelete(array[i]);
		}
	}


	template <typename T>
	size_t add_unique(std::vector < T >& array, const T &add) {
		size_t i;
		for (i=0; i<array.size(); i++) {
			if (array[i] == add)
				return i;
		}
		array.push_back(add);
		return i;
	}

	// remove_const
	template< class T >
	struct remove_const {
		typedef T type;
	};

	template< class T >
	struct remove_const<const T> {
		typedef T type;
	};

	// remove_reference
	template< class T >
	struct remove_reference {
		typedef T type;
	};

	template< class T >
	struct remove_reference<T&> {
		typedef T type;
	};


	// remove_const_referance
	template< class T >
	struct remove_const_reference {
		typedef typename remove_const<typename remove_reference<T>::type>::type type;
	};


AX_END_NAMESPACE

#endif // AX_TEMPLATE_H
