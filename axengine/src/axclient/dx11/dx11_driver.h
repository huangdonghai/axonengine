#ifndef AX_DX11_DRIVER_H
#define AX_DX11_DRIVER_H

AX_BEGIN_NAMESPACE

class DX11_Driver : public IRenderDriver, public ICmdHandler
{
	AX_DECLARE_FACTORY();
	AX_DECLARE_COMMAND_HANDLER(DX11_Driver);

public:
	DX11_Driver();
	virtual ~DX11_Driver();

	// implement IRenderDriver
	virtual void initialize(SyncEvent &syncEvent);
	virtual void finalize();

	virtual const ShaderInfo *findShaderInfo(const FixedString &key);

protected:
	void createInputLayouts();

private:
};

AX_END_NAMESPACE

#endif // AX_DX11_DRIVER_H
