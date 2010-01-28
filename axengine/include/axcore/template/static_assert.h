/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

// like BOOST_STATIC_ASSERT
// when C++0x is out, we will use language's static_assert

#ifndef AX_STATICASSERT_H
#define AX_STATICASSERT_H

AX_BEGIN_NAMESPACE

// HP aCC cannot deal with missing names for template value parameters
template <bool x> struct STATIC_ASSERTION_FAILURE;

template <> struct STATIC_ASSERTION_FAILURE<true> { enum { value = 1 }; };

// HP aCC cannot deal with missing names for template value parameters
template<int x> struct static_assert_test{};

AX_END_NAMESPACE

#define AX_STATIC_ASSERT(B) \
	typedef AX_NAMESPACE::static_assert_test<\
	sizeof(AX_NAMESPACE::STATIC_ASSERTION_FAILURE< (bool)(B) >)>\
	AX_JOINT(ax_static_assert_typedef_, __LINE__)

#endif // JTL_STATICASSERT_HPP

