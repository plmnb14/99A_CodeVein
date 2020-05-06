#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CBloodCodeSlot;
class CBloodCodeSelectUI;
class CSkillReleaseUI;
class CBloodCodeOwner;
class CBloodCodeMenuUI final : public CUI
{
private:
	explicit CBloodCodeMenuUI(_Device pDevice);
	explicit CBloodCodeMenuUI(const CBloodCodeMenuUI& rhs);
	virtual ~CBloodCodeMenuUI() = default;

public:
	void Set_IsChoise(_bool bIsChoise) { m_bIsChoiseBloodCode = bIsChoise; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iIndex);
	void SetUp_Default();
	

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	BloodCode_ID			m_eBloodCodeType = BloodCode_End;
	vector<CBloodCodeSlot*> m_vecBloodCodeSlot;
	_bool					m_bIsCodeSelect = false;
	_uint					m_iSelectIndex = 0;
	_float					m_fAlpha = 0.f;
	CBloodCodeSelectUI*		m_pBloodCodeSelectUI = nullptr;
	CSkillReleaseUI*		m_pSkillReleaseUI = nullptr;
	_bool					m_bIsChoiseBloodCode = false;
	CBloodCodeOwner*		m_pBloodCodeOwner = nullptr;

public:
	static CBloodCodeMenuUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
