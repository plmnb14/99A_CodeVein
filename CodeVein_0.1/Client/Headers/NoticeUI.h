#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CNoticeUI final : public CUI
{
private:
	explicit CNoticeUI(_Device pDevice);
	explicit CNoticeUI(const CNoticeUI& rhs);
	virtual ~CNoticeUI() = default;

public:
	void Set_Skill_ID(Skill_ID eID) { m_eID = eID; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	Skill_ID				m_eID = SkillID_End;

public:
	static CNoticeUI*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
