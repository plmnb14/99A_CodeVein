#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class COKMessageUI;
class CSkillAcquisitionUI final : public CUI
{
private:
	explicit CSkillAcquisitionUI(_Device pDevice);
	explicit CSkillAcquisitionUI(const CSkillAcquisitionUI& rhs);
	virtual ~CSkillAcquisitionUI() = default;

public:
	void Set_Skill(Skill_ID eSkillID) { m_eSkillType = eSkillID; }

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
	void	Click_OKMsgUI();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	Skill_ID m_eSkillType = SkillID_End;
	COKMessageUI*			m_pOKMsg = nullptr;
	_float					m_fCross = 100.f;
	_float					m_fAlpha = 0.f;
	
public:
	static CSkillAcquisitionUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
