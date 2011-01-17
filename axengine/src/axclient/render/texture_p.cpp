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

	std::string filename = key.toString() + ".dds";

	m_asioRequest = new AsioRequest(this, filename);
	g_fileSystem->queAsioRead(m_asioRequest);

	// add to dictionary
	setKey(key);
	ms_resources[key] = this;
}

TextureResource::TextureResource(const FixedString &key, TexFormat format, const Size &size, InitFlags flags)
{
	m_isFileTexture = false;
	m_fileLoaded = false;

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
	g_apiWrap->uploadSubTexture(m_handle, rect, pixels, format, 0);
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
		AsioRequest *request = ace->asioRequest();
		AX_ASSERT(request->eventHandler() == this);
		AX_ASSERT(m_asioRequest == request);
		g_apiWrap->createTextureFromFileInMemory(m_handle, request);
		m_asioRequest = 0;
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

AX_END_NAMESPACE
