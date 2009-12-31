#ifndef AX_EDITOR_GFXTOOLS_H
#define AX_EDITOR_GFXTOOLS_H

AX_BEGIN_NAMESPACE

//--------------------------------------------------------------------------
class GfxTool : public Tool
{
public:
	enum GfxToolType {
		// object creation
		CreateParticle = Tool::UserDefined, CreateRibbon
	};

	GfxTool(GfxContext *ctx);
	virtual ~GfxTool();

protected:
	GfxContext *m_gfxContext;
};

//--------------------------------------------------------------------------
class GfxObjectCreationTool : public GfxTool
{
public:
	GfxObjectCreationTool(GfxContext *ctx);
	virtual ~GfxObjectCreationTool();

	// implement Tool
	virtual void doBindView(View *view) {}
	virtual void doPress(int x, int y, int flags, float pressure);
	virtual void doDrag(int x, int y, int flags, float pressure);
	virtual void doMove(int x, int y);
	virtual void doRelease(int x, int y);
	virtual void doRender(const RenderCamera &camera);

	virtual GfxAgent *create() = 0;

private:
	GfxAgent *m_gfxAgent;
};

//--------------------------------------------------------------------------
class ParticleCreationTool : public GfxObjectCreationTool
{
public:
	ParticleCreationTool(GfxContext *ctx);
	virtual ~ParticleCreationTool();

	// implement GfxObjectCreationTool
	virtual GfxAgent *create();
};

class RibbonCreationTool : public GfxObjectCreationTool {
public:
	RibbonCreationTool(GfxContext *ctx);
	virtual ~RibbonCreationTool();

	// implement GfxObjectCreationTool
	virtual GfxAgent *create();
};

AX_END_NAMESPACE

#endif // AX_EDITOR_GFXTOOLS_H