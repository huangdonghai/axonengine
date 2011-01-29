#include "../private.h"

AX_BEGIN_NAMESPACE

//--------------------------------------------------------------------------
// class TextureWrap
//--------------------------------------------------------------------------

TextureResource::ResourceDict TextureResource::ms_resources;
TextureResourcePtr TextureResource::ms_default;

TextureResource::TextureResource(const FixedString &key, InitFlags flags)
{
	m_isFileTexture = true;
	m_fileLoaded = false;
	m_handle = 0;
	m_initFlags = flags;
	m_texType = TexType::Unknown;
	m_width = m_height = m_depth = 0;

	std::string filename = key.toString() + ".dds";

	m_ioRequest = new IoRequest(this, filename);
	if (1 || m_initFlags.isSet(Texture::Dynamic)) {
		// dynamic texture need sync upload
		g_fileSystem->syncRead(m_ioRequest);
		g_apiWrap->createTextureFromFileInMemory(m_handle, m_ioRequest);
		m_ioRequest = 0;
		m_fileLoaded = true;
	} else {
		// static texture can async upload
		g_fileSystem->queAsioRead(m_ioRequest);
	}
	// add to dictionary
	setKey(key);
	ms_resources[key] = this;
}

TextureResource::TextureResource(const FixedString &key, TexFormat format, const Size &size, InitFlags flags)
{ _init(key, TexType::_2D, format, size.width, size.height, 1, flags); }

TextureResource::TextureResource(const FixedString &key, TexType textype, TexFormat format, int width, int height, float depth, InitFlags flags)
{ _init(key, textype, format, width, height, depth, flags); }


TextureResource::~TextureResource()
{
	g_apiWrap->deleteTexture(m_handle);
}

void TextureResource::uploadSubTexture(const Rect &rect, const void *pixels, TexFormat format)
{
	AX_ASSURE(m_initFlags.isSet(Texture::Dynamic));
	g_apiWrap->uploadSubTexture(m_handle, rect, pixels, format);
}

void TextureResource::generateMipmap()
{
	g_apiWrap->generateMipmap(m_handle);
}

TextureResourcePtr TextureResource::findResource(const FixedString &name, int flags)
{
	if (!ms_default) {
		FixedString defaultName = "textures/default";
		ms_default = new TextureResource(defaultName, 0);
	}

	ResourceDict::iterator it = ms_resources.find(name);

	if (it != ms_resources.end())
		return it->second;

	TextureResource *resource = new TextureResource(name, flags);

	return TextureResourcePtr(resource);
}

bool TextureResource::event(Event *e)
{
	Event::Type eventType = e->type();

	if (eventType == Event::AsioCompleted) {
		AX_ASSERT(m_isFileTexture);
		AsioCompletedEvent *ace = static_cast<AsioCompletedEvent *>(e);
		IoRequest *request = ace->asioRequest();
		AX_ASSERT(request->eventHandler() == this);
		AX_ASSERT(m_ioRequest == request);
		g_apiWrap->createTextureFromFileInMemory(m_handle, request);
		m_ioRequest = 0;
		m_fileLoaded = true;
		return true;
	} else {
		return IEventHandler::event(e);
	}
}


void TextureResource::onDestroy()
{
	ms_resources.erase(getKey());
}

bool TextureResource::canBeDeletedNow()
{
	if (m_isFileTexture && !m_fileLoaded)
		return false;
	else
		return true;
}

Size TextureResource::size() const
{
	AX_ASSURE(!m_isFileTexture);
	return Size(m_width, m_height);
}

void TextureResource::_init(const FixedString &key, TexType textype, TexFormat format, int width, int height, float depth, InitFlags flags)
{
	m_isFileTexture = false;
	m_fileLoaded = false;
	m_initFlags = flags|Texture::Dynamic;
	m_texType = textype;
	m_width = width;
	m_height = height;
	m_depth = depth;

	g_apiWrap->createTexture(m_handle, textype, format, width, height, depth, flags);

	setKey(key);
	ms_resources[key] = this;
}


AX_END_NAMESPACE
