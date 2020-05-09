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
	void Set_Active_State(CActive_Icon::ACTIVE_STATE eState, _uint iIndex);

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);

private:
	void	SetUp_Default();

	void	Set_SkillIcon(_uint iIndex, Skill_Index eSkill_Index); // 스킬 아이콘 세팅 함수
	void	Set_SkillGaugeCur(_ulong iNum);
	void	Set_SkillGaugeMax(_ulong iNum);

public:
	_ulong Get_SkillGaugeCur();
	_ulong Get_SkillGaugeMax();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	CActive_Icon*			m_pActive[8];
	CIconBase*				m_pIconBase[8];
	CPlayerFontUI*			m_pSkillGaugeCur = nullptr;
	CPlayerFontUI*			m_pSkillGaugeMax = nullptr;

public:
	static CSkillUI*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
