/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_CORE_NULLPTR_H
#define AX_CORE_NULLPTR_H

// simulates c++0x's nullptr, see http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2004/n1601.pdf

namespace Axon {

	const // this is a const object...
	class nullptr_t {
	public:
		template<class T>			// convertible to any type
		operator T*() const // of null non-member
		{ return 0; }				// pointer...
		template<class C, class T>	// or any type of null
		operator T C::*() const // member pointer...
		{ return 0; }
	private:
		void operator&() const;		// whose address can¡¯t be taken
	} nullptr = {};					// and whose name is nullptr

} // namespace Axon

#endif // end guardian

