#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CLoadingBar;
class CLoadingScreen final : public CUI
{
private:
	explicit CLoadingScreen(_Device pDevice);
	explicit CLoadingScreen(const CLoadingScreen& rhs);
	virtual ~CLoadingScreen() = default;

public:
	_float Get_Alpha() { return m_fAlpha; }

public:
	void Set_Alpha(_float fAlpha) { m_fAlpha = fAlpha; };
	void Set_FadeSpeed(_float fFadeSpeed) { m_fFadeSpeed = fFadeSpeed; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	CLoadingBar*			m_pLoadingBar = nullptr;
	_float					m_fAlpha = 0.f;
	_float					m_fFadeSpeed = 0.f;

public:
	static CLoadingScreen* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};
END
