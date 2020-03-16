#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CBuffer_CubeTex final : public CVIBuffer
{
private:
	explicit CBuffer_CubeTex(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBuffer_CubeTex(const CBuffer_CubeTex& rhs);
	virtual ~CBuffer_CubeTex() = default;
public:
	virtual HRESULT Ready_Component_Prototype();
	virtual HRESULT Ready_Component(void* pArg);
	void Render_VIBuffer();
public:
	static CBuffer_CubeTex* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone_Component(void* pArg);
protected:
	virtual void Free();
};

END