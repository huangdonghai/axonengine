#include "../private.h"

AX_BEGIN_NAMESPACE

//--------------------------------------------------------------------------
// class TextureWrap
//--------------------------------------------------------------------------

TextureResource::ResourceDict TextureResource::ms_resources;
std::list<TextureResource*> TextureResource::ms_asioList;


TextureResource::TextureResource(const FixedString &key, InitFlags flags)
{
	std::string filename = key.toString() + ".dds";

	m_asioRequest = new AsioRequest(this, filename);
	g_fileSystem->queAsioRead(m_asioRequest);

	// add to dictionary
	setKey(key);
	ms_resources[key] = this;
}

TextureResource::TextureResource(const FixedString &key, TexFormat format, int width, int height, InitFlags flags)
{
	g_apiWrap->createTexture2D(&m_handle, format, width, height, flags);

	setKey(key);
	ms_resources[key] = this;
}

TextureResource::~TextureResource()
{
	g_apiWrap->deleteTexture2D(&m_handle);
	ms_resources.erase(getKey());
}

void TextureResource::uploadSubTexture(const Rect &rect, const void *pixels, TexFormat format)
{
	g_apiWrap->uploadSubTexture(&m_handle, rect, pixels, format, 0);
}

void TextureResource::generateMipmap()
{
	g_apiWrap->generateMipmap(&m_handle);
}

TextureResourcePtr TextureResource::findResource(const FixedString &name, int flags)
{
	ResourceDict::iterator it = ms_resources.find(name);

	if (it != ms_resources.end())
		return it->second;

	TextureResource *resource = new TextureResource(name, flags);

	return TextureResourcePtr(resource);
}

TextureResourcePtr TextureResource::createResource(const FixedString &debugname, TexFormat format, int width, int height, int flags)
{
	return new TextureResource(debugname, format, width, height, flags);
}

void TextureResource::loadFileMemory( int size, void *data )
{

}

#if 0
void TextureResource::stepAsio()
{
	std::list<TextureResource*>::iterator it = ms_asioList.begin();

	while (it != ms_asioList.end()) {
		TextureResource *res = *it;
		if (res->m_asioRead.isDataReady()) {
			it = ms_asioList.erase(it);
			res->loadFileMemory(res->m_asioRead.getFileSize(), res->m_asioRead.getFileData());
			res->m_asioRead.freeData();
		} else {
			it++;
		}
	}
}
#endif

bool TextureResource::event(Event *e)
{
	Event::Type eventType = e->type();

	if (eventType == Event::AsioCompleted) {
		AsioCompletedEvent *ace = static_cast<AsioCompletedEvent *>(e);
		AsioRequest *request = ace->asioRequest();
		AX_ASSERT(request->eventHandler() == this);
		AX_ASSERT(m_asioRequest == request);
		loadFileMemory(request->fileSize(), request->fileData());
		return true;
	} else if(eventType == Event::ResourceUploaded) {
		// TODO
		return true;
	} else {
		return IEventHandler::event(e);
	}
}


void TextureResource::deleteThis()
{

}

AX_END_NAMESPACE
