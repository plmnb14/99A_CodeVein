#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CMistletoeOptionUI;
class CStageSelectUI;
class CMistletoeUI final : public CUI
{
private:
	explicit CMistletoeUI(_Device pDevice);
	explicit CMistletoeUI(const CMistletoeUI& rhs);
	virtual ~CMistletoeUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
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
	_float					m_fAlpha = 0.f;
	vector<CMistletoeOptionUI*> m_vecOption;
	_uint					m_iSelectIndex = 0;
	CStageSelectUI*			m_pStageSelectUI = nullptr;

public:
	static CMistletoeUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
