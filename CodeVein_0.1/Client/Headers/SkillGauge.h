#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CPlayerFontUI;
class CSkillGauge final : public CUI
{
private:
	explicit CSkillGauge(_Device pDevice);
	explicit CSkillGauge(const CSkillGauge& rhs);
	virtual ~CSkillGauge() = default;

public:
	void Set_CurSkillPoint(_uint iCurSkillPoint) { m_iCurSkillPoint = iCurSkillPoint; }
	void Set_MaxSkillPoint(_uint iMaxSkillPoint) { m_iMaxSkillPoint = iMaxSkillPoint; }

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
	_uint					m_iCurSkillPoint = 20;
	_uint					m_iMaxSkillPoint = 20;
	CPlayerFontUI*			m_pCurPointFont = nullptr;
	CPlayerFontUI*			m_pMaxPointFont = nullptr;

public:
	static CSkillGauge*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
