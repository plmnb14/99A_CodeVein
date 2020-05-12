#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CWeapon_Inven_InShop;
class CWeapon_Inven_InShop;
class CPlayerFontUI;
class CWeaponBuyPopupOptionUI;
class CWeaponBuyPopupUI final : public CUI
{
public:
	enum POPUP_TYPE 
	{
		POPUP_WEAPON_BUY,
		POPUP_WEAPON_SELL,
		POPUP_ARMOR_BUY,
		POPUP_ARMOR_SELL,
		POPUP_ITEM_BUY,
		POPUP_ITEM_SELL,
		POPUP_END
	};
private:
	explicit CWeaponBuyPopupUI(_Device pDevice);
	explicit CWeaponBuyPopupUI(const CWeaponBuyPopupUI& rhs);
	virtual ~CWeaponBuyPopupUI() = default;

public:
	void	Set_Active(_bool bIsActive);
	void	Set_Inven(CWeapon_Inven_InShop* pParent) { m_pInven = pParent; }
	void	Set_Type(POPUP_TYPE eType) { m_eType = eType; }
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

	void Click_Option();
	void Reset_Option();
private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	void	Change_Texture(const _tchar* _Name);
	void	SetUp_Default();
	void	Check_ItemOption();

	void	Buy_Weapon();
	void	Sell_Weapon();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	CWeapon_Inven_InShop*				m_pInven = nullptr;
	vector<CWeaponBuyPopupOptionUI*>	m_vecOption;

	CPlayerFontUI*			m_pFontItemCount = nullptr;

	_float					m_fAlpha = 0.f;
	_uint					m_iSelectIndex = 0;
	_float					m_fCross = 100.f;

	_int					m_iTexIndex = 0;
	_int					m_iCount = 0;
	
	POPUP_TYPE				m_eType = POPUP_END;
public:
	static CWeaponBuyPopupUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
