#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Material.h"

BEGIN(Client)
class CPlayerFontUI;
class CWeaponUpgradingPopup final : public CUI
{
public:
	enum POPUP_TYPE
	{
		POPUP_SUCCESS,
		POPUP_FAILED,
		POPUP_END
	};

private:
	explicit CWeaponUpgradingPopup(_Device pDevice);
	explicit CWeaponUpgradingPopup(const CWeaponUpgradingPopup& rhs);
	virtual ~CWeaponUpgradingPopup() = default;

public:
	void	Set_Active(_bool bIsActive);
	void	Set_PopupType(POPUP_TYPE eType);

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	void	Change_Texture(const _tchar* _Name);
	void	SetUp_Default();

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CBuffer_RcTex*		m_pBufferCom = nullptr;

private:
	POPUP_TYPE			m_ePopupType = POPUP_END;
	_bool				m_bLateInit = false;
	_int				m_iTexIdx = 0;
	_float				m_fAlpha = 0.f;
	_float				m_fFadeDelay = 0.f;
	_bool				m_bFadeInStart = false;
	_bool				m_bFadeOutStart = true;

public:
	static CWeaponUpgradingPopup*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
