#ifndef AX_SAMPLERDATA_H
#define AX_SAMPLERDATA_H

AX_BEGIN_NAMESPACE

extern ApiWrap *g_apiWrap;

class SamplerState : public RefObject
{
public:
	SamplerState(const SamplerStateDesc &desc)
	{ g_apiWrap->createSamplerState(&m_h, desc); }

	virtual ~SamplerState()
	{ g_apiWrap->deleteSamplerState(&m_h); }

	const SamplerStateDesc &getDesc() const
	{ return m_desc; }


private:
	SamplerStateDesc m_desc;
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

#endif // AX_SAMPLERDATA_H
