#ifndef AX_GL_DRIVER_H
#define AX_GL_DRIVER_H

AX_BEGIN_NAMESPACE

class GL_Driver : public IRenderDriver, public ICmdHandler
{
	AX_DECLARE_FACTORY();
	AX_DECLARE_COMMAND_HANDLER(GL_Driver);

public:
	GL_Driver();
	virtual ~GL_Driver();

	// implement IRenderDriver
	virtual bool initialize();
	virtual void finalize();

	virtual const ShaderInfo *findShaderInfo(const FixedString &key);

	static GLenum trTexFormat(TexFormat texFormat);
	static GLenum trShaderResourceViewFormat(TexFormat texFormat);
	static GLenum trRenderTargetFormat(TexFormat texFormat);

protected:
	void checkFormats();

private:
};

AX_END_NAMESPACE

#endif // AX_GL_DRIVER_H
