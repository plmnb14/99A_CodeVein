#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CSkillSlot final : public CUI
{
private:
	explicit CSkillSlot(_Device pDevice);
	explicit CSkillSlot(const CSkillSlot& rhs);
	virtual ~CSkillSlot() = default;

public:
	Skill_ID Get_SkillID() { return m_eSkillID; }
	_bool Get_Select() { return m_bIsSelect; }

public:
	void Set_SkillID(Skill_ID eID) { m_eSkillID = eID; }
	void Set_Select(_bool bIsSelect) { m_bIsSelect = bIsSelect; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iIndex);
	void SetUp_SkillIndex();

public:
	_bool Pt_InRect();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	_bool					m_bIsCollMouse = false;
	Skill_ID				m_eSkillID = SkillID_End;
	_bool					m_bIsSelect = false;

public:
	static CSkillSlot*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
