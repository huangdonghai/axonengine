#ifndef AX_EDITOR_GFXTOOLS_H
#define AX_EDITOR_GFXTOOLS_H

AX_BEGIN_NAMESPACE

	//--------------------------------------------------------------------------
	class GfxTool : public Tool
	{
	public:
		enum GfxToolType {
			// object creation
			CreateParticle, CreatRibbon
		};

		GfxTool(GfxContext *ctx);
		virtual ~GfxTool();

	protected:
		GfxContext *m_gfxContext;
	};

	//--------------------------------------------------------------------------
	class CreateParticleEmitterTool : public GfxTool
	{
	public:
		CreateParticleEmitterTool(GfxContext* ctx);
		virtual ~CreateParticleEmitterTool();

		// implement Tool
		virtual void doBindView(View* view) {}
		virtual void doPress(int x, int y, int flags, float pressure);
		virtual void doDrag(int x, int y, int flags, float pressure);
		virtual void doMove(int x, int y) {}
		virtual void doRelease(int x, int y);
		virtual void doRender(const RenderCamera& camera) {}

	private:
	};

AX_END_NAMESPACE

#endif // AX_EDITOR_GFXTOOLS_H