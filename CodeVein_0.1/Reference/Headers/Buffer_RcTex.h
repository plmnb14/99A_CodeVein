#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CBuffer_RcTex final : public CVIBuffer
{
private:
	explicit CBuffer_RcTex(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBuffer_RcTex(const CBuffer_RcTex& rhs);
	virtual ~CBuffer_RcTex() = default;

public:
	virtual HRESULT Ready_Component_Prototype();
	virtual HRESULT Ready_Component(void* pArg);
	void Render_VIBuffer();
	void Render_Before_Instancing(_mat matInstance);
	void Render_DrawPrimitive_Instancing();
	void Render_After_Instancing();

public:
	static CBuffer_RcTex* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone_Component(void* pArg = nullptr);
	
private:
	IDirect3DVertexDeclaration9*    m_pVertexDeclHardware = NULL;
	IDirect3DVertexBuffer9*         m_pVBInstanceData = 0;
protected:
	virtual void Free();


};

END