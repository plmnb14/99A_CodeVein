#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Weapon_Slot.h"

BEGIN(Client)
class CWeaponUpgradeUI;
class CWeaponBuyPopupUI;
class CWeapon_Inven;
class CArmor_Inven;
class CWeapon;
class CWeapon_Slot;
class CArmor_Slot;
class CWeapon_Inven_InShop final : public CUI
{
public:
	enum INVEN_SHOP_OPTION
	{
		SHOP_WEAPON_BUY,
		SHOP_WEAPON_SELL,
		SHOP_ARMOR_BUY,
		SHOP_ARMOR_SELL,
		SHOP_ITEM_BUY,
		SHOP_ITEM_SELL,
		SHOP_WEAPON_UPGRADE,
		SHOP_ARMOR_UPGRADE,
		SHOP_END
	};
private:
	explicit CWeapon_Inven_InShop(_Device pDevice);
	explicit CWeapon_Inven_InShop(const CWeapon_Inven_InShop& rhs);
	virtual ~CWeapon_Inven_InShop() = default;

public:
	WPN_PARAM		Get_UseWeaponParam(_uint iIndex) { return m_UseWeaponParam[iIndex]; }
	CWeapon_Slot*	Get_HoverSlot_Weapon() { return m_pHoverSlot_Weapon; }
	CWeapon_Slot*	Get_SelectedSlot_Weapon() { return m_pSelectedSlot_Weapon; }
	CArmor_Slot*	Get_HoverSlot_Armor() { return m_pHoverSlot_Armor; }
	CArmor_Slot*	Get_SelectedSlot_Armor() { return m_pSelectedSlot_Armor; }
	_bool			Get_PopupOn();
	_bool			Get_CheckCloseUpgradePopup() { return m_bCloseUpgradePopup; }
	_float			Get_UpgradePrice(_int iReinforce);
	_float			Get_PlusDamage(_float fDamage, _int iReinforce);
	_float			Get_PlusDef(_float fDef, _int iReinforce);

public:
	void Set_CheckCloseUpgradePopup() { m_bCloseUpgradePopup = false; }

	void Setup_InvenType(INVEN_SHOP_OPTION eOption);
	void Refresh_Inven();
	void Buy_Weapon();
	void Buy_Armor();
	void Sell_Weapon();
	void Sell_Armor();
	void Upgrade_Weapon(WPN_PARAM tParam);
	void Upgrade_Armor(ARMOR_PARAM tParam);


public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable();
	void					Click_WeaponInven();
	void					Click_ArmorInven();

	HRESULT					SetUp_WeaponData(INVEN_SHOP_OPTION eShop);

public:
	void Add_Weapon(WPN_PARAM tAddWpnParam);
	void Add_Armor(ARMOR_PARAM tAddArmorParam);

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

	CWeaponBuyPopupUI*		m_pWeaponBuyPopup = nullptr;
	CWeaponBuyPopupUI*		m_pWeaponSellPopup = nullptr;
	CWeaponBuyPopupUI*		m_pArmorBuyPopup = nullptr;
	CWeaponBuyPopupUI*		m_pArmorSellPopup = nullptr;
	CWeaponUpgradeUI*		m_pWeaponUpgradePopup = nullptr;

	CWeapon_Inven*			m_pWeaponInventory = nullptr;
	CArmor_Inven*			m_pArmorInventory = nullptr;
	vector<CWeapon_Slot*>	m_vecWeaponSlot;
	vector<CArmor_Slot*>	m_vecArmorSlot;
	CWeapon_Slot*			m_pSelectedSlot_Weapon = nullptr;
	CWeapon_Slot*			m_pHoverSlot_Weapon = nullptr;
	CArmor_Slot*			m_pSelectedSlot_Armor = nullptr;
	CArmor_Slot*			m_pHoverSlot_Armor = nullptr;

	WPN_PARAM				m_UseWeaponParam[2];
	WPN_PARAM				m_tWeaponParam[WPN_DATA_End];
	ARMOR_PARAM				m_tArmorParam[ArmorAll_END];

	INVEN_SHOP_OPTION		m_eOption = SHOP_END;
	
	CWeapon*				m_pTempWeapon = nullptr;
	_bool					m_bCloseUpgradePopup = false;
	_bool					m_bIsSubActive = false;

public:
	static CWeapon_Inven_InShop*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END

