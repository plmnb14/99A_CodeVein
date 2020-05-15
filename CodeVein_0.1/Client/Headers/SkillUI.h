#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Active_Icon.h"
BEGIN(Client)
class CActive_Icon;
class CIconBase;
class CPlayerFontUI;
class CSkillUI final : public CUI
{
private:
	explicit CSkillUI(_Device pDevice);
	explicit CSkillUI(const CSkillUI& rhs);
	virtual ~CSkillUI() = default;

public:
	Skill_ID Get_Skill_ID(_uint iIndex);

public:
	void Set_Active_State(CActive_Icon::ACTIVE_STATE eState, _uint iIndex);

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);

private:
	void	SetUp_Default();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	CActive_Icon*			m_pActive[8];
	//CIconBase*				m_pIconBase[8];
	CPlayerFontUI*			m_pSkillGaugeCur = nullptr;
	CPlayerFontUI*			m_pSkillGaugeMax = nullptr;

public:
	static CSkillUI*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
