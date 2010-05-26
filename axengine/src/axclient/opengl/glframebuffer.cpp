/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "private.h"

AX_BEGIN_NAMESPACE

//--------------------------------------------------------------------------
// class GLtarget
//--------------------------------------------------------------------------

GLtarget::GLtarget(GLframebuffer *fb, int width, int height, TexFormat format, bool asrenderbuffer) {
	m_isRenderBuffer = false;
	m_framebuffer = fb;
	m_boundIndex = -1;
	m_depthTarget = nullptr;
	TypeZeroArray(colorAttached);

	m_storeHint = RenderTarget::Free;

	m_texture = nullptr;
	m_lastFrameUsed = 0;
	m_renderBuffer = 0;

	m_width = width;
	m_height = height;

	// create asset
	if (!m_isRenderBuffer) {
		String texname;
		StringUtil::sprintf(texname, "_RenderTarget_%d_%d_%d", m_width, m_height, g_system->generateId());
#if 0
		m_texture = dynamic_cast<GLtexture*>(g_assetManager->createEmptyAsset(Texture::AssetType));
		m_texture->initialize(format, width, height);


		g_assetManager->addAsset(Asset::kTexture, texname, m_texture);
#else
		m_texture = AX_REFPTR_CAST(GLtexture, new Texture(texname, format, m_width, m_height));
#endif
	} else {

	}
}

GLtarget::~GLtarget() {
	SafeDecRef(m_texture);
}

Rect GLtarget::getRect() {
	Rect rect;
	int depth;
	rect.x = rect.y = 0;
	m_texture->getSize(rect.width, rect.height, depth);

	return rect;
}

RenderTarget::Type GLtarget::getType() {
	return kTexture;
}

void GLtarget::bind() {
	m_framebuffer->bind(this);
}

void GLtarget::unbind() {
	m_framebuffer->unbind();
}

void GLtarget::attachDepth(RenderTarget *depth) {
	AX_ASSERT(depth->isTexture());
	AX_ASSERT(depth->isDepthFormat());
	GLtarget *gldepth = dynamic_cast<GLtarget*>(depth);
	attachDepth(gldepth);
}

void GLtarget::attachDepth(GLtarget *depth) {
	if (!isColorFormat()) {
		Errorf("GLtarget::attachDepth: attach depth to not a color target");
		return;
	}

	if (depth == m_depthTarget) {
		return;
	}

	m_depthTarget = depth;
}

void GLtarget::attachColor(int index, RenderTarget *c) {
	AX_ASSERT(index < MAX_COLOR_ATTACHMENT);
	colorAttached[index] = (GLtarget*)c;
}

void GLtarget::detachColor(int index) {
	AX_ASSERT(index < MAX_COLOR_ATTACHMENT);
	colorAttached[index] = nullptr;
}

void GLtarget::detachAllColor() {
	TypeZeroArray(colorAttached);
}

RenderTarget *GLtarget::getColorAttached(int index) const {
	AX_ASSERT(index < MAX_COLOR_ATTACHMENT);
	return colorAttached[index];
}

void GLtarget::setHint(AllocHint hint, int frame) {
	m_storeHint = hint;

	if (hint != Free) {
		m_lastFrameUsed = frame;
	}
}

GLtarget *GLtarget::getColorAttachedGL(int index) const {
	AX_ASSERT(index < MAX_COLOR_ATTACHMENT);
	return colorAttached[index];
}

//--------------------------------------------------------------------------
// class GLrenderbuffer
//--------------------------------------------------------------------------

#if 0
// GeForce 6x only support GL_DEPTH_COMPONENT24 depth render buffer
GLrenderbuffer::GLrenderbuffer(GLenum internelformat, GLsizei width, GLsizei height)
	: m_object(0)
	, m_internalFormat(internelformat)
	, m_width(width)
	, m_height(height)
{
	glGenRenderbuffersEXT(1, &m_object);

	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_object);

	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, m_internalFormat, m_width, m_height);

	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

	GLrender::checkErrors();
}

GLrenderbuffer::~GLrenderbuffer() {
	if (m_object)
		glDeleteRenderbuffersEXT(1, &m_object);
}

uint_t GLrenderbuffer::getObject() {
	return m_object;
}
#endif

//--------------------------------------------------------------------------
// class GLframebuffer
//--------------------------------------------------------------------------

GLframebuffer::GLframebuffer(int width, int height)
	: m_object(0)
	, m_object2(0)
	, m_width(width)
	, m_height(height)
//		, m_dirty(false)
{
//		m_boundColor = nullptr;
	TypeZeroArray(m_boundColor);
	m_boundDepth = nullptr;
	m_boundTarget = nullptr;

	glGenFramebuffersEXT(1, &m_object);
	glGenFramebuffersEXT(1, &m_object2);
}

GLframebuffer::~GLframebuffer() {
	if (m_object)
		glDeleteFramebuffersEXT(1, &m_object);

	if (m_object2) {
		glDeleteFramebuffersEXT(1, &m_object2);
	}
}


uint_t GLframebuffer::getObject() {
	return m_object;
}

void GLframebuffer::checkStatus(GLenum target) {
	GLenum status = glCheckFramebufferStatusEXT(target);
	char *error_string;

	if (status == GL_NO_ERROR || status == GL_FRAMEBUFFER_COMPLETE_EXT)
		return;

	switch (status) {
	case GL_FRAMEBUFFER_COMPLETE_EXT:
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
		error_string = "Framebuffer incomplete, incomplete attachment";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
		error_string = "Framebuffer incomplete, missing attachment";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
		error_string = "Framebuffer incomplete, attached images must have same dimensions";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
		error_string = "Framebuffer incomplete, attached images must have same format";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
		error_string = "Framebuffer incomplete, missing draw buffer";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
		error_string = "Framebuffer incomplete, missing read buffer";
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
		error_string = "Unsupported framebuffer format";
		break;
	default:
		error_string = "Framebuffer incomplete, unknow error";
		break;
	}

	Errorf("GLframebuffer::checkStatus: %s", error_string);
}

void GLframebuffer::bind(GLtarget *target) {
	m_boundTarget = target;

	if (!target) {
		unbind();
		return;
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_object);    
	if (m_boundTarget->isColorFormat()) {
		Sequence<GLenum> buffers;

		GLenum b = bindColor(m_boundTarget);
		buffers.push_back(b + GL_COLOR_ATTACHMENT0_EXT);

		for (int i = 0; i < RenderTarget::MAX_COLOR_ATTACHMENT; i++) {
			GLtarget *attached = m_boundTarget->getColorAttachedGL(i);
			if (!attached) {
				continue;
			}

			b = bindColor(attached);
			buffers.push_back(b + GL_COLOR_ATTACHMENT0_EXT);
		}
		if (buffers.size() == 1) {
			glDrawBuffer(buffers[0]);
		} else {
			glDrawBuffers(s2i(buffers.size()), &buffers[0]);
		}

		// attach depth and stencil
		GLtarget *depth = m_boundTarget->getDepthAttachedGL();
		if (!depth) {
			depth = allocTarget(RenderTarget::TemporalAlloc, TexFormat::D24);
		}

		TexFormat tf = depth->getTextureGL()->getFormat();

		bindDepth(depth);

		if (tf.isStencil()) {
			bindStencil(depth);
		} else {
			bindStencil(0);
		}
#if 0
		// attach color0
		GLtexture *tex = m_boundTarget->getTextureGL();
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, tex->getTarget(), tex->getObject(), 0);

		buffers.push_back(GL_COLOR_ATTACHMENT0_EXT);
		// attach color0~N
		for (int i = 0; i < Target::MAX_COLOR_ATTACHMENT; i++) {
			GLtarget *attached = m_boundTarget->getColorAttachedGL(i);
			if (!attached) {
				continue;
			}

			buffers.push_back(GL_COLOR_ATTACHMENT0_EXT + i + 1);
			GLtexture *tex = attached->getTextureGL();
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i + 1, tex->getTarget(), tex->getObject(), 0);
		}

		glDrawBuffers(s2i(buffers.size()), &buffers[0]);

		// attach depth and stencil
		m_boundDepth = m_boundTarget->getDepthAttachedGL();
		if (!m_boundDepth) {
			m_boundDepth = allocTarget(Target::TemporalAlloc, TexFormat::D24S8);
		}

		tex = m_boundDepth->getTextureGL();
		TexFormat tf = tex->getFormat();

		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, tex->getTarget(), tex->getObject(), 0);

		if (tf.isStencil()) {
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, tex->getTarget(), tex->getObject(), 0);
		} else {
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, tex->getTarget(), 0, 0);
		}
#endif
	} else {
		// no color attach
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		// attach depth and stencil
		GLtarget *depth = m_boundTarget;
		TexFormat tf = depth->getTextureGL()->getFormat();

		bindDepth(depth);

		if (tf.isStencil()) {
			bindStencil(depth);
		} else {
			bindStencil(0);
		}
	}

//		checkStatus(GL_FRAMEBUFFER_EXT);
}

void GLframebuffer::unbind() {
#if 0
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0);
	m_boundColor = 0;
	m_boundDepth = 0;
#endif

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
//		glReadBuffer(GL_BACK);
//		glDrawBuffer(GL_BACK);
}

void GLframebuffer::newFrame(int frame) {
	GLtargetseq::iterator it = m_targetpool.begin();
	while (it != m_targetpool.end()) {
		GLtarget *target = *it;

		if (target->getHint() == RenderTarget::PermanentAlloc) {
			++it;
			continue;
		}

		if (frame - target->m_lastFrameUsed > 50) {
			it = m_targetpool.erase(it);
			delete target;
			continue;
		}

		target->setHint(RenderTarget::Free, frame);
		++it;
	}
}

void GLframebuffer::endFrame() {
}

void GLframebuffer::blitColor(GLtexture *tex) {
	AX_ASSERT(m_boundColor && m_boundDepth);

	glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, m_object);
#if 0
	glFramebufferTexture2DEXT(GL_READ_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, m_colorTarget->getGLtexture()->getTarget(), m_colorTarget->getGLtexture()->getObject(), 0);
	glFramebufferTexture2DEXT(GL_READ_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, m_depthTarget->getGLtexture()->getTarget(), m_depthTarget->getGLtexture()->getObject(), 0);
#endif

	glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, m_object2);
	glFramebufferTexture2DEXT(GL_DRAW_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, tex->getTarget(), tex->getObject(), 0);
	glFramebufferTexture2DEXT(GL_DRAW_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, m_boundDepth->getTextureGL()->getTarget(), m_boundDepth->getTextureGL()->getObject(), 0);

	glBlitFramebufferEXT(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	checkStatus(GL_DRAW_FRAMEBUFFER_EXT);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_object);
	GLrender::checkErrors();
}

void GLframebuffer::blitDepth(GLtexture *tex) {
	AX_ASSERT(m_boundDepth);
	glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, m_object);

	glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, m_object2);
	glDrawBuffer(GL_NONE);
	glFramebufferTexture2DEXT(GL_DRAW_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, tex->getTarget(), tex->getObject(), 0);
	glFramebufferTexture2DEXT(GL_DRAW_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, tex->getTarget(), tex->getObject(), 0);
	checkStatus(GL_DRAW_FRAMEBUFFER_EXT);
	glBlitFramebufferEXT(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT, GL_NEAREST);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_object);
	GLrender::checkErrors();
}

bool GLframebuffer::checkColorFormat(TexFormat format) {
	return false;
}

GLtarget *GLframebuffer::allocTarget(RenderTarget::AllocHint hint, TexFormat texformat) {
	if (!glFramebufferManager->isFormatSupport(texformat)) {
		Errorf("GLframebuffer::allocTarget: can't support format %s", texformat.toString());
		return nullptr;
	}

	GLtarget *result = nullptr;
	AX_FOREACH(GLtarget *target, m_targetpool) {
		if (target->getHint() != RenderTarget::Free) {
			continue;
		}

		if (target->getTextureGL()->getFormat() != texformat) {
			continue;
		}

		result = target;
		break;
	}

	if (!result) {
		result = new GLtarget(this, m_width, m_height, texformat);
		m_targetpool.push_back(result);
	}

	result->setHint(hint, glFramebufferManager->getFrame());
	return result;
}

void GLframebuffer::freeTarget(RenderTarget *target) {
	if (!target->isTexture()) {
		Errorf("GLframebuffer::freeTarget: target isn't a texture");
		return;
	}

	GLtarget *gltarget = (GLtarget*)target;

#if 0
	if (gltarget->getHint() != Target::PermanentAlloc) {
		Errorf("GLframebuffer::freeTarget: only permanent target need to be free");
		return;
	}
#endif

	gltarget->setHint(RenderTarget::Free, glFramebufferManager->getFrame());
}

GLtexture *GLframebuffer::getBoundColor() const {
	return m_boundTarget->getTextureGL();

//		return (GLtexture*)m_boundColor->getTexture();
}

GLtexture *GLframebuffer::getBoundDepth() const {
	return m_boundDepth->getTextureGL();
}

GLenum GLframebuffer::bindColor(GLtarget *target) {
#if 1 // FOR TEST
	GLtexture *textest = m_boundTarget->getTextureGL();
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, textest->getTarget(), textest->getObject(), 0);
	return 0;
#endif
	if (target->m_boundIndex >= 0) {
		return target->m_boundIndex;
	}

	int found = -1;
	for (int i = 0; i < MAX_DRAW_BUFFERS; i++) {
		if (m_boundColor[i] == 0) {
			found = i;
			break;
		}
	}

	if (found == -1) {
		found = 0;
	}

	if (m_boundColor[found]) {
		m_boundColor[found]->m_boundIndex = -1;
	}
	m_boundColor[found] = target;
	target->m_boundIndex = found;

	GLtexture *tex = m_boundTarget->getTextureGL();
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT+found, tex->getTarget(), tex->getObject(), 0);
	return found;
}

void GLframebuffer::bindDepth(GLtarget *target) {
	{ // TEST
		m_boundDepth = target;

		GLtexture *tex = m_boundDepth->getTextureGL();
		TexFormat tf = tex->getFormat();

		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, tex->getTarget(), tex->getObject(), 0);
		return;
	}
	if (m_boundDepth == target) {
		return;
	}
	m_boundDepth = target;

	GLtexture *tex = m_boundDepth->getTextureGL();
	TexFormat tf = tex->getFormat();

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, tex->getTarget(), tex->getObject(), 0);
}

void GLframebuffer::bindStencil(GLtarget *target) {
	if (m_boundStencil == target) {
		return;
	}
	m_boundStencil = target;

	if (target) {
		GLtexture *tex = m_boundStencil->getTextureGL();
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, tex->getTarget(), tex->getObject(), 0);
	} else {
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0);
	}
}

//--------------------------------------------------------------------------
// class GLframebuffermanager
//--------------------------------------------------------------------------

GLframebuffermanager::GLframebuffermanager() {
	m_curFrame = 0;
}

GLframebuffermanager::~GLframebuffermanager() {
	GLframebufferDict::iterator it = m_framebuffers.begin();

	for (; it != m_framebuffers.end(); ++it) {
		Dict<int, GLframebuffer*>& col2 = it->second;
		Dict<int, GLframebuffer*>::iterator it2 = col2.begin();

		for (; it2 != col2.end(); ++it2) {
			SafeDelete(it2->second);
		}
	}
}

void GLframebuffermanager::initialize() {
	checkFormats();
}

void GLframebuffermanager::finalize() {
}

RenderTarget *GLframebuffermanager::allocTarget(RenderTarget::AllocHint hint, int width, int height, TexFormat texformat) {
	GLframebuffer *fb = getFramebuffer(width, height);
	return fb->allocTarget(hint, texformat);
}

void GLframebuffermanager::freeTarget(RenderTarget *target) {
	if (!target->isTexture()) {
		return;
	}

	GLtarget *gltarget = (GLtarget*)target;
	gltarget->getFramebuffer()->freeTarget(gltarget);
}

bool GLframebuffermanager::isFormatSupport(TexFormat format) {
	return m_formatSupports[format];
}

void GLframebuffermanager::preFrame() {
	m_curFrame++;

	GLframebufferDict::iterator it = m_framebuffers.begin();

	for (; it != m_framebuffers.end(); ++it) {
		Dict<int, GLframebuffer*>& col2 = it->second;
		Dict<int, GLframebuffer*>::iterator it2 = col2.begin();

		for (; it2 != col2.end(); ++it2) {
			it2->second->newFrame(m_curFrame);
		}
	}
}

GLframebuffer *GLframebuffermanager::getFramebuffer(int width, int height) {
	GLframebuffer *fb = m_framebuffers[width][height];

	if (!fb) {
		fb = new GLframebuffer(width, height);
		m_framebuffers[width][height] = fb;
	}

	return fb;
}

void GLdriver::preFrame() {
	glThread->preFrame();
}

extern void trTexFormat(TexFormat tex_format, GLenum &dataformat, GLenum &datatype, GLenum &internal_format);

#define D(s) s, #s,

static struct {
	GLenum e;
	const char *s;
} formats[] = {
	D(GL_R3_G3_B2)
	D(GL_ALPHA4)
	D(GL_ALPHA8)             
	D(GL_ALPHA12)            
	D(GL_ALPHA16)            
	D(GL_LUMINANCE4)         
	D(GL_LUMINANCE8)         
	D(GL_LUMINANCE12)        
	D(GL_LUMINANCE16)        
	D(GL_LUMINANCE4_ALPHA4)  
	D(GL_LUMINANCE6_ALPHA2)  
	D(GL_LUMINANCE8_ALPHA8)  
	D(GL_LUMINANCE12_ALPHA4) 
	D(GL_LUMINANCE12_ALPHA12)
	D(GL_LUMINANCE16_ALPHA16)
	D(GL_INTENSITY)          
	D(GL_INTENSITY4)         
	D(GL_INTENSITY8)         
	D(GL_INTENSITY12)        
	D(GL_INTENSITY16)        
	D(GL_RGB4)               
	D(GL_RGB5)               
	D(GL_RGB8)               
	D(GL_RGB10)              
	D(GL_RGB12)              
	D(GL_RGB16)              
	D(GL_RGBA2)              
	D(GL_RGBA4)              
	D(GL_RGB5_A1)            
	D(GL_RGBA8)              
	D(GL_RGB10_A2)           
	D(GL_RGBA12)             
	D(GL_RGBA16)             

	D(GL_RGBA32F_ARB)
	D(GL_RGB32F_ARB)
	D(GL_ALPHA32F_ARB)
	D(GL_INTENSITY32F_ARB)
	D(GL_LUMINANCE32F_ARB)
	D(GL_LUMINANCE_ALPHA32F_ARB)
	D(GL_RGBA16F_ARB)
	D(GL_RGB16F_ARB)
	D(GL_ALPHA16F_ARB)
	D(GL_INTENSITY16F_ARB)
	D(GL_LUMINANCE16F_ARB)
	D(GL_LUMINANCE_ALPHA16F_ARB)
};

inline const char *getName(int e) {
	for (size_t i = 0; i < ArraySize(formats); i++) {
		if (formats[i].e == e) {
			return formats[i].s;
		}
	}

	return "UNKNOWN";
}

void GLframebuffermanager::checkFormats() {
	GLuint object;

	TypeZeroArray(m_formatSupports);

//		memset(m_formatSupports, 0xff, sizeof(m_formatSupports));

	glGenFramebuffersEXT(1, &object);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, object);

#if 0
	for (size_t i = 0; i < ArraySize(formats); i++) {
#if 0
		GLenum dataformat, datatype, iformat;

		TexFormat format = TexFormat::R32F;
		trTexFormat(format, dataformat, datatype, iformat);
#else
		GLenum iformat = formats[i].e;
#endif
		GLuint rbuf;
		glGenRenderbuffersEXT(1, &rbuf);

		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, rbuf);

		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, iformat, 32, 32);

		if (glGetError() != GL_NO_ERROR) {
			glDeleteRenderbuffersEXT(1, &rbuf);
			Printf("NOT SUPPORT %s\n", formats[i].s);
			continue;
		}

		int realf, r, g, b, a;
		glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_INTERNAL_FORMAT_EXT, &realf);
		glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_RED_SIZE_EXT, &r);
		glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_GREEN_SIZE_EXT, &g);
		glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_BLUE_SIZE_EXT, &b);
		glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_ALPHA_SIZE_EXT, &a);

		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, rbuf);

		GLenum status = glCheckFramebufferStatusEXT(GL_DRAW_FRAMEBUFFER_EXT);

		if (status == GL_NO_ERROR || status == GL_FRAMEBUFFER_COMPLETE_EXT) {
			Printf("SUPPORT %s, internal is %s, %d %d %d %d\n", formats[i].s, getName(realf), r, g, b, a);
		} else {
			Printf("NOT SUPPORT %s\n", formats[i].s);
		}

		glDeleteRenderbuffersEXT(1, &rbuf);
	}
#else
	for (int i = TexFormat::AUTO+1; i < TexFormat::MAX_NUMBER; i++) {
		m_formatSupports[i] = checkFormat(i);
	}
#endif

	glDrawBuffer(GL_BACK);
	glReadBuffer(GL_BACK);

	glDeleteFramebuffersEXT(1, &object);

	glGetError();
}

bool GLframebuffermanager::checkFormat(TexFormat format) {
	GLenum dataformat, datatype, iformat;
	trTexFormat(format, dataformat, datatype, iformat);

	if (!format) {
		return false;
	}

	GLuint rbuf;
	glGenRenderbuffersEXT(1, &rbuf);

	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, rbuf);

	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, iformat, 32, 32);

	if (glGetError() != GL_NO_ERROR) {
		glDeleteRenderbuffersEXT(1, &rbuf);
		Printf("FBO DON'T SUPPORT %s\n", format.toString());

		return false;
	}

	int realf, r, g, b, a, d, s;
	glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_INTERNAL_FORMAT_EXT, &realf);
	glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_RED_SIZE_EXT, &r);
	glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_GREEN_SIZE_EXT, &g);
	glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_BLUE_SIZE_EXT, &b);
	glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_ALPHA_SIZE_EXT, &a);
	glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_DEPTH_SIZE_EXT, &d);
	glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_STENCIL_SIZE_EXT, &s);

	if (format.isColor()) {
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, rbuf);
	} else if (format.isDepth()) {
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rbuf);

	} else if (format.isStencil()) {
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rbuf);
	}

	GLenum status = glCheckFramebufferStatusEXT(GL_DRAW_FRAMEBUFFER_EXT);

	if (status == GL_NO_ERROR || status == GL_FRAMEBUFFER_COMPLETE_EXT) {
		Printf("FBO SUPPORT %s, R%dG%dB%dA%dD%dS%d\n", format.toString(), r, g, b, a, d, s);
	} else {
		Printf("FBO DON'T SUPPORT %s\n", format.toString());
	}

	glDeleteRenderbuffersEXT(1, &rbuf);

	glGetError();

	return true;
}

TexFormat GLframebuffermanager::getSuggestFormat(RenderTarget::SuggestFormat sf)
{
	switch (sf) {
	case RenderTarget::LDR_COLOR:
		return TexFormat::BGRA8;
	case RenderTarget::MDR_COLOR:
		return TexFormat::RGBA16F;
	case RenderTarget::HDR_COLOR:
		return TexFormat::RGBA16F;
	case RenderTarget::SHADOW_MAP:
		if (r_shadowFormat.getInteger() == 0) {
			return TexFormat::D16;
		}
		return TexFormat::D24;
	}

	return 0;
}

AX_END_NAMESPACE

