#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CBloodSkillSlot;
class CBloodSkillCursor;
class CReleaseQuestionUI;
class CSkillReleaseUI final : public CUI
{
private:
	explicit CSkillReleaseUI(_Device pDevice);
	explicit CSkillReleaseUI(const CSkillReleaseUI& rhs);
	virtual ~CSkillReleaseUI() = default;

public:
	void Set_CodeID(BloodCode_ID eID) { m_eID = eID; }

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
	void	Click_SkillSlot();
	void	Reset_Select();

public:
	void Compare_Data();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	vector<CBloodSkillSlot*> m_vecSkillSlot;
	BloodCode_ID			m_eID;
	_v3						m_vSlotPosition;
	_float					m_fCross = 100.f;
	CReleaseQuestionUI*		m_pQuestionUI = nullptr;
	vector<Skill_ID>		m_vecData;

public:
	static CSkillReleaseUI*		Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
