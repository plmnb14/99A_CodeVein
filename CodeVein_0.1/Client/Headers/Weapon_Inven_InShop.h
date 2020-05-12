#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Weapon_Slot.h"

BEGIN(Client)
class CWeaponBuyPopupUI;
class CWeapon_Inven;
class CWeapon;
class CWeapon_Slot;
class CWeapon_Inven_InShop final : public CUI
{
public:
	enum INVEN_SHOP_OPTION
	{
		SHOP_BUY,
		SHOP_SELL,
		SHOP_END
	};
private:
	explicit CWeapon_Inven_InShop(_Device pDevice);
	explicit CWeapon_Inven_InShop(const CWeapon_Inven_InShop& rhs);
	virtual ~CWeapon_Inven_InShop() = default;

public:
	WPN_PARAM Get_UseWeaponParam(_uint iIndex) { return m_UseWeaponParam[iIndex]; }
	CWeapon_Slot* Get_HoverSlot() { return m_pHoverSlot; }
	CWeapon_Slot* Get_SelectedSlot() { return m_pSelectedSlot; }

public:
	void Setup_InvenType(INVEN_SHOP_OPTION eOption);

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable();
	void					Click_Inven();
	void					Buy_Weapon(CWeapon_Slot* pWeaponSlot);
	void					Sell_Weapon(CWeapon_Slot* pWeaponSlot);
	HRESULT					SetUp_WeaponData(INVEN_SHOP_OPTION eShop);

public:
	void Add_Weapon(WPN_PARAM tAddWpnParam);

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

	CWeaponBuyPopupUI*		m_pWeaponBuyPopup = nullptr;
	CWeapon_Inven*			m_pWeaponInventory = nullptr;
	vector<CWeapon_Slot*>	m_vecWeaponSlot;
	CWeapon_Slot*			m_pSelectedSlot = nullptr;
	CWeapon_Slot*			m_pHoverSlot = nullptr;
	WPN_PARAM				m_UseWeaponParam[2];
	WPN_PARAM				m_tWeaponParam[WPN_DATA_End];

	INVEN_SHOP_OPTION		m_eOption = SHOP_END;

public:
	static CWeapon_Inven_InShop*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END

