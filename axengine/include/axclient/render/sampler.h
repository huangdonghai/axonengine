#ifndef AX_RENDER_SAMPLER_H
#define AX_RENDER_SAMPLER_H

AX_BEGIN_NAMESPACE

AX_DECLARE_REFPTR(SamplerState);
AX_DECLARE_REFPTR(BlendState);
AX_DECLARE_REFPTR(DepthStencilState);
AX_DECLARE_REFPTR(RasterizerState);

class SamplerState : public RefObject
{
public:
	SamplerState();
	virtual ~SamplerState();

private:
	Handle m_h;
};

class BlendState : public RefObject
{
public:
	BlendState();
	virtual ~BlendState();

private:
	Handle m_h;
};

class DepthStencilState : public RefObject
{
public:
	DepthStencilState();
	virtual ~DepthStencilState();

private:
	Handle m_h;
};

class RasterizerState : public RefObject
{
public:
	RasterizerState();
	virtual ~RasterizerState();

private:
	Handle m_h;
};

AX_END_NAMESPACE

#endif // AX_RENDER_SAMPLER_H
