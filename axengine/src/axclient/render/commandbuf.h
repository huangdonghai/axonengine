#ifndef AX_COMMANDBUF_H
#define AX_COMMANDBUF_H

AX_BEGIN_NAMESPACE

class RenderInterface
{
public:
	handle_t (*createTexture2D)()
};

class RenderBackendResource
{
public:
	RenderBackendResource();
	virtual ~RenderBackendResource();

	virtual void deleteThis()
	{
	}

private:
	AtomicInt m_ref;
};

class RenderFrontendResource
{
public:
	RenderFrontendResource();
	virtual ~RenderFrontendResource();

	virtual void deleteThis() {}
	
private:
	AtomicInt m_ref;
};


class MaterialBr : public RenderBackendResource
{};

class MaterialFr : public RenderFrontendResource
{
public:
private:
	MaterialBr *m_br;
};


class ElementBr : public RenderBackendResource
{};

class ElementFr : public RenderFrontendResource
{
public:
private:
	ElementBr *m_br;

	bool m_isDirty;		// dirty
	bool m_isVertexBufferDirty;
	bool m_isIndexBufferDirty;
	bool m_isWorldSpace; // primitive already in world space, so don't need model transform

	int m_cachedId;		// used by render driver
	int m_cachedFrame;

	Element::ElementType m_type;
	NewMaterial m_material;
	NewTexture m_lightMap;

	bool m_isMatrixSet;
	Matrix4 m_matrix;

	int m_activedIndexes;

	int m_chainId;
	Interaction *m_headInteraction;
};


class RenderCommand
{
public:
	virtual ~RenderCommand() {}
	virtual void exec() = 0;
};

struct CommandBuf
{
	enum {
		MaxBufSize = 64 * 1024
	};

	int bufSize;
	byte_t buf[MaxBufSize];
};

#define AX_QUEUE_RENDER_COMMAND_0(tag, code)
#define AX_QUEUE_RENDER_COMMAND_1(tag, t1, a1, v1, code)
#define AX_QUEUE_RENDER_COMMAND_2(tag, t1, a1, v1, t2, a2, v2, code)
#define AX_QUEUE_RENDER_COMMAND_3(tag, t1, a1, v1, t2, a2, v2, t3, a3, v3, code)


AX_END_NAMESPACE

#endif // AX_COMMANDBUF_H
