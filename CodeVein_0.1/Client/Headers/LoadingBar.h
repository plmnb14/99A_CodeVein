#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CLoadingBar final : public CUI
{
private:
	explicit CLoadingBar(_Device pDevice);
	explicit CLoadingBar(const CLoadingBar& rhs);
	virtual ~CLoadingBar() = default;

public:
	void Set_Finish() { m_bIsFinish = true; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	_double					m_dLoadingAni = 0.0;
	_bool					m_bIsFinish = false;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
public:
	static CLoadingBar* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};
END
