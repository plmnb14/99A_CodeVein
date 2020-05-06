#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CSkillIcon final : public CUI
{
private:
	explicit CSkillIcon(_Device pDevice);
	explicit CSkillIcon(const CSkillIcon& rhs);
	virtual ~CSkillIcon() = default;

public:
	Skill_ID Get_Skill_ID() { return m_eSkillID; }

public:
	void Set_Skill_ID(Skill_ID eSkillID) { m_eSkillID = eSkillID; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

public:
	_bool Pt_InRect();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	Skill_ID				m_eSkillID = SkillID_End;

public:
	static CSkillIcon*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END