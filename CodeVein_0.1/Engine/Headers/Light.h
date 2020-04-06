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
	_mat	Get_LightViewProj() { return m_matView * m_matProj; }
public:
	void Set_Pos(_v3 vPos) { m_vPos = vPos; }
public:
	HRESULT Ready_Light(D3DLIGHT9 LightDesc);
	HRESULT Render_Light(CShader* pShader);
	void Update_Light();
private:
	LPDIRECT3DDEVICE9	m_pGraphic_Device = nullptr;
	D3DLIGHT9			m_LightDesc;
private:
	CBuffer_ViewPort*	m_pViewPortBuffer = nullptr;
private:
	_mat	m_matView = {};
	_mat	m_matProj = {};
	_v3		m_vPos = {0, 0, 0};
public:
	static CLight* Create(LPDIRECT3DDEVICE9 pGraphic_Device, D3DLIGHT9 LightDesc);
	virtual void Free();
};

END