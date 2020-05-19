#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CPlayerFontUI;
class CExplainWeaponUI final : public CUI
{
private:
	explicit CExplainWeaponUI(_Device pDevice);
	explicit CExplainWeaponUI(const CExplainWeaponUI& rhs);
	virtual ~CExplainWeaponUI() = default;


public:
	void Set_WeaponParam(WPN_PARAM tWpnParam) { m_tWeaponParam = tWpnParam; }

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable(_uint iIndex);

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

private:
	WPN_PARAM				m_tWeaponParam;
	_uint					m_iInfoIdx = 0;
	_uint					m_iWeaponIconIdx = 0;
	_uint					m_iWeaponMoveIdx = 0;
	CPlayerFontUI*			m_pAttackFont = nullptr;
	CPlayerFontUI*			m_pAddDamageFont = nullptr;
	CPlayerFontUI*			m_pReinForceFont = nullptr;

public:
	static CExplainWeaponUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
