#pragma once

#include "Base.h"
#include "Buffer_ViewPort.h"
#include "Shader.h"

BEGIN(Engine)

class CLight final : public CBase
{
private:
	explicit CLight(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLight() = default;
public:
	const D3DLIGHT9* Get_LightDesc() const {
		return &m_LightDesc; }
public:
	HRESULT Ready_Light(D3DLIGHT9 LightDesc);
	HRESULT Render_Light(CShader* pShader);
private:
	LPDIRECT3DDEVICE9	m_pGraphic_Device = nullptr;
	D3DLIGHT9			m_LightDesc;
private:
	CBuffer_ViewPort*	m_pViewPortBuffer = nullptr;
public:
	static CLight* Create(LPDIRECT3DDEVICE9 pGraphic_Device, D3DLIGHT9 LightDesc);
	virtual void Free();
};

END