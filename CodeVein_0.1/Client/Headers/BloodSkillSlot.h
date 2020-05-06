#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CBloodSkillCursor;
class CBloodSkillSlot final : public CUI
{
private:
	explicit CBloodSkillSlot(_Device pDevice);
	explicit CBloodSkillSlot(const CBloodSkillSlot& rhs);
	virtual ~CBloodSkillSlot() = default;

public:
	Skill_ID Get_Skill_ID() { return m_eSkillID; }
	_bool Get_Release() { return m_bIsRelease; }

public:
	void Set_SkillID(Skill_ID eSkillID) { m_eSkillID = eSkillID; }
	void Set_Select(_bool bIsSelect) { m_bIsSelect = bIsSelect; }
	void Set_Release(_bool bIsRelease) { m_bIsRelease = bIsRelease; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iDiffuse);
	void	SetUp_Default();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CCollider*				m_pCollider = nullptr;

private:
	Skill_ID				m_eSkillID = SkillID_End;
	CBloodSkillCursor*		m_pCursor = nullptr;
	_bool					m_bIsSelect = false;
	_float					m_fSpeed = 0.f;
	_bool					m_bIsRelease = false;

public:
	static CBloodSkillSlot*		Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
