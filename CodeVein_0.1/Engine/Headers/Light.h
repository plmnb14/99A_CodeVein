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
	const NEW_LIGHT* Get_LightDesc() const {
		return &m_LightDesc;
	}
	_mat	Get_LightViewProj() { return m_matView * m_matProj; }

public:
	_bool Get_Dead() { return m_bDead; }

public:
	void Set_Pos(_v3 vPos) { m_vPos = vPos; }

public:
	HRESULT Ready_Light(NEW_LIGHT LightDesc);
	HRESULT Render_Light(CShader* pShader);
	void Update_Light();

private:
	LPDIRECT3DDEVICE9	m_pGraphic_Device = nullptr;
	NEW_LIGHT			m_LightDesc;

private:
	_bool		m_bDecreAlpha = false;
	_bool		m_bLifeTime = false;

private:
	_float		m_fLifeTime_Cur = 0.f;
	_float		m_fLifeTime_Max = 0.f;
	_float		m_fAlpha = 1.f;

private:
	_bool		m_bDead = false;

private:
	CBuffer_ViewPort*	m_pViewPortBuffer = nullptr;

private:
	_mat	m_matView = {};
	_mat	m_matProj = {};
	_v3		m_vPos = {0, 0, 0};

public:
	static CLight* Create(LPDIRECT3DDEVICE9 pGraphic_Device, NEW_LIGHT LightDesc);
	virtual void Free();
};

END