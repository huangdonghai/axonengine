#ifndef AX_RENDER_SAMPLER_H
#define AX_RENDER_SAMPLER_H

AX_BEGIN_NAMESPACE

AX_DECLARE_REFPTR(SamplerState);
AX_DECLARE_REFPTR(BlendState);
AX_DECLARE_REFPTR(DepthStencilState);
AX_DECLARE_REFPTR(RasterizerState);

class WrapObject : public RefObject
{

};

class SamplerState : public RefObject
{
public:
	SamplerState(const SamplerStateDesc &desc)
	{ g_apiWrap->createSamplerState(&m_h, desc); }

	virtual ~SamplerState()
	{ g_apiWrap->deleteSamplerState(&m_h); }

	virtual void deleteThis() {}

private:
	Handle m_h;
};

class BlendState : public RefObject
{
public:
	BlendState(const BlendStateDesc &desc);
	virtual ~BlendState();

private:
	Handle m_h;
};

class DepthStencilState : public RefObject
{
public:
	DepthStencilState(const DepthStencilStateDesc &desc);
	virtual ~DepthStencilState();

private:
	Handle m_h;
};

class RasterizerState : public RefObject
{
public:
	RasterizerState(const RasterizerStateDesc &desc);
	virtual ~RasterizerState();

private:
	Handle m_h;
};

AX_END_NAMESPACE

#endif // AX_RENDER_SAMPLER_H
