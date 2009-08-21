/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_RAY_H
#define AX_RAY_H

namespace Axon {

	struct AX_API Ray {
	public:
		Ray();
		Ray(const Vector3& start, const Vector3& dir);

	private:
		Vector3 m_start;
		Vector3 m_dir;
	};
} // namespace Axon

#endif // end guardian

