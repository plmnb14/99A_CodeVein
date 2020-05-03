#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CReleaseOption;
class CReleaseQuestionUI final : public CUI
{
private:
	explicit CReleaseQuestionUI(_Device pDevice);
	explicit CReleaseQuestionUI(const CReleaseQuestionUI& rhs);
	virtual ~CReleaseQuestionUI() = default;

public:
	_bool Get_Release() { return m_bIsRelease; }

public:
	void Set_Release(_bool bIsRelease) { m_bIsRelease = bIsRelease; }

public:
	void Set_Skill(Skill_ID eSkillID) { m_eReleaseSkill = eSkillID; }

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
	void	Click_Option();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	_float					m_fAlpha = 0.f;
	vector<CReleaseOption*>	m_vecOption;
	_float					m_fCross = 100.f;
	Skill_ID				m_eReleaseSkill = SkillID_End;
	_bool					m_bIsRelease = false;

public:
	static CReleaseQuestionUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
