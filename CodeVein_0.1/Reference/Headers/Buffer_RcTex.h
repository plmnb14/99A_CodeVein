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

public:
	static CBuffer_RcTex* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone_Component(void* pArg = nullptr);

protected:
	virtual void Free();


};

END