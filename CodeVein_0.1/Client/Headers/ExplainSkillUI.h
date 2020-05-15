#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CExplainSkillUI final : public CUI
{
private:
	explicit CExplainSkillUI(_Device pDevice);
	explicit CExplainSkillUI(const CExplainSkillUI& rhs);
	virtual ~CExplainSkillUI() = default;

public:
	void Set_Type(Skill_ID eID) { m_eType = eID; }

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable();

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

private:
	Skill_ID				m_eType = SkillID_End;

public:
	static CExplainSkillUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END