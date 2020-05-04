#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CBloodSkillSlot;
class CReleaseQuestionUI;
class CSkillAcquisitionUI;
class CReleaseQuestionUI;
class CFighterReleaseUI final : public CUI
{
private:
	explicit CFighterReleaseUI(_Device pDevice);
	explicit CFighterReleaseUI(const CFighterReleaseUI& rhs);
	virtual ~CFighterReleaseUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iIndex);
	void	SetUp_Default();
	void	SetPos_SubUI();
	void	Click_SkillSlot();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	vector<CBloodSkillSlot*> m_vecSkillSlot;
	CReleaseQuestionUI*		m_pQuestionUI = nullptr;
	_float m_fCross = 100.f;
	CSkillAcquisitionUI*	m_pAcquisitionUI = nullptr;
	

public:
	static CFighterReleaseUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
