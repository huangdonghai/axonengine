#include "../private.h"

AX_BEGIN_NAMESPACE

//--------------------------------------------------------------------------
// class TextureWrap
//--------------------------------------------------------------------------

TextureResource::ResourceDict TextureResource::ms_resources;
std::list<TextureResource*> TextureResource::ms_asioList;


TextureResource::TextureResource(const FixedString &key, InitFlags flags)
{
	m_isFileTexture = true;
	m_fileLoaded = false;
	m_handle = 0;
	m_initFlags = flags;
	m_size.set(0, 0);

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
{
	m_isFileTexture = false;
	m_fileLoaded = false;
	m_initFlags = flags|Texture::Dynamic;
	m_size = size;

	g_apiWrap->createTexture2D(m_handle, format, size.width, size.height, flags);

	setKey(key);
	ms_resources[key] = this;
}

TextureResource::~TextureResource()
{
	g_apiWrap->deleteTexture2D(m_handle);
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
	ResourceDict::iterator it = ms_resources.find(name);

	if (it != ms_resources.end())
		return it->second;

	TextureResource *resource = new TextureResource(name, flags);

	return TextureResourcePtr(resource);
}

TextureResourcePtr TextureResource::createResource(const FixedString &debugname, TexFormat format, const Size &size, int flags)
{
	return new TextureResource(debugname, format, size, flags);
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
	return m_size;
}

AX_END_NAMESPACE
