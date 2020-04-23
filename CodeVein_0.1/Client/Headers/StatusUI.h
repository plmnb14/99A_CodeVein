#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CConditionUI;
class CExpUI;
class CButton_UI;
class CStatusUI final : public CUI
{
private:
	explicit CStatusUI(_Device pDevice);
	explicit CStatusUI(const CStatusUI& rhs);
	virtual ~CStatusUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	void	SetUp_Default();
	void	Exit_This_UI();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	vector<CConditionUI*>	m_vecConditionUI;
	CExpUI*					m_pExpUI = nullptr;
	CButton_UI*				m_pExitUI = nullptr;

public:
	static CStatusUI*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
