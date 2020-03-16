#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CBuffer_ViewPort final : public CVIBuffer
{
private:
	explicit CBuffer_ViewPort(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBuffer_ViewPort(const CBuffer_ViewPort& rhs);
	virtual ~CBuffer_ViewPort() = default;
public: 
	virtual HRESULT Ready_Component_Prototype(_float fStartX, _float fStartY, _float fSizeX, _float fSizeY);
	virtual HRESULT Ready_Component(void* pArg);
	void Render_VIBuffer();
public:
	static CBuffer_ViewPort* Create(LPDIRECT3DDEVICE9 pGraphic_Device, _float fStartX, _float fStartY, _float fSizeX, _float fSizeY);
	virtual CComponent* Clone_Component(void* pArg = nullptr);
protected:
	virtual void Free();


};

END