/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_RENDER_ALL_H
#define AX_RENDER_ALL_H

AX_BEGIN_NAMESPACE
AX_END_NAMESPACE

template <typename T> static inline T *GetPtrHelper(T *ptr) { return ptr; }
template <typename Wrapper> static inline typename Wrapper::const_pointer GetPtrHelper(const Wrapper &p) { return p.get(); }
template <typename Wrapper> static inline typename Wrapper::pointer GetPtrHelper(Wrapper &p) { return p.get(); }

#define AX_DECLARE_DATA(Class) \
	typedef Class DataClass; \
	Class *d_func() { return reinterpret_cast<Class *>( GetPtrHelper(m_data)); } \
	const Class *d_func() const { return reinterpret_cast<const Class *>( GetPtrHelper(m_data)); }

#define AX_DECLARE_BACKEND(Class) \
	typedef Class BackendClass; \
	Class *b_func() { return reinterpret_cast<Class *>( GetPtrHelper(m_backend)); } \
	const Class *b_func() const { return reinterpret_cast<const Class *>( GetPtrHelper(m_backend)); }

#define AX_DATA \
	DataClass *d = d_func();

#define AX_CONSTDATA \
	const DataClass *d = d_func();

#define AX_BACKEND \
	BackendClass *b = b_func();

#define AX_CONST_BACKEND \
	const BackendClass *b = b_func();


#include "query.h"
#include "texture.h"
#include "sampler.h"
#include "textureatlas.h"
#include "materialdecl.h"
#include "shader.h"
#include "material.h"
#include "camera.h"
#include "target.h"
#include "interaction.h"
#include "primitive.h"
#include "render_entity.h"
#include "light.h"
#include "fog.h"
#include "terrain.h"
#include "driver.h"
#include "font.h"
#include "wind.h"
#include "timeofday.h"
#include "outdoorenv.h"
#include "render_world.h"
#include "queue.h"
#include "render_system.h"

#endif // end guardian

