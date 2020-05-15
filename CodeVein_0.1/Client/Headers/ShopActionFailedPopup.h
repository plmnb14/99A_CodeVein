#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Material.h"

BEGIN(Client)
class CPlayerFontUI;
class CShopActionFailedPopup final : public CUI
{
public:
	enum POPUP_TYPE
	{
		POPUP_BUY,
		POPUP_UPGRADE,
		POPUP_END
	};

private:
	explicit CShopActionFailedPopup(_Device pDevice);
	explicit CShopActionFailedPopup(const CShopActionFailedPopup& rhs);
	virtual ~CShopActionFailedPopup() = default;

public:
	void	Set_Active(_bool bIsActive);
	void	Set_Fade(_bool bIsActive);
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
	static CShopActionFailedPopup*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
