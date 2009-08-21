/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_PHYSICS_SYSTEM_H
#define AX_PHYSICS_SYSTEM_H

namespace Axon { namespace Physics {

	class System {
	public:
		System();
		~System();

		void initialize();
		void finalize();

	private:
	};
}} // namespace Axon::Physics

#endif // end guardian

