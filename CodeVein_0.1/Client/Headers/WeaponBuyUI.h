#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CPlayerFontUI;
class CWeapon_Inven_InShop;
class CWeaponShopUI;
class CStatusUI;
class CShopItemIcon;
class CWeaponBuyUI final : public CUI
{
public:
	enum MOVE_TYPE 
	{
		MOVE_STEP,
		MOVE_ROLLING,
		MOVE_HEAVYROLLING,
		MOVE_END
	};

	enum SHOP_OPTION
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
	explicit CWeaponBuyUI(_Device pDevice);
	explicit CWeaponBuyUI(const CWeaponBuyUI& rhs);
	virtual ~CWeaponBuyUI() = default;

public:
	void	Set_Active(_bool bIsActive);
	void	Set_Parent(CWeaponShopUI* pParent) { m_pParent = pParent; }
	void	Set_Inven(CWeapon_Inven_InShop* pInven) { m_pInven = pInven; }
	void	Set_WeaponMoveType(MOVE_TYPE eType);
	void	Set_WeaponDescType(WEAPON_ALL_DATA eType);
	void	Set_ArmorDescType(ARMOR_All_DATA eType);
	void	Set_ShopType(SHOP_OPTION eType);

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
	void	Check_ItemOption_Weapon();
	void	Check_ItemOption_Armor();
	void	Set_NoneSelect();
	void	Check_LateInit();
	void	Check_MoveType();
	void	Check_Desc();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	CStatusUI*				m_pStatusUI = nullptr;
	CWeapon_Inven_InShop*	m_pInven = nullptr;
	CWeaponShopUI*			m_pParent = nullptr;
	CShopItemIcon*			m_pShopItemIcon = nullptr;

	CPlayerFontUI*			m_pFontDamage = nullptr;
	CPlayerFontUI*			m_pMyHazeCnt = nullptr;
	CPlayerFontUI*			m_pPriceHazeCnt = nullptr;
	CPlayerFontUI*			m_pFontReinforce = nullptr;
	CPlayerFontUI*			m_pFontPlusOption_0 = nullptr;
	CPlayerFontUI*			m_pFontPlusOption_1 = nullptr;
	CPlayerFontUI*			m_pFontHP = nullptr;

	CWeaponBuyUI*			m_pWeaponMoveTypeUI = nullptr;
	CWeaponBuyUI*			m_pWeaponDescUI = nullptr;
	MOVE_TYPE				m_eMoveType = MOVE_TYPE::MOVE_END;
	_int					m_iTexIndex = 0;

	WEAPON_ALL_DATA			m_eWeaponDesc = WEAPON_ALL_DATA::WpnAll_END;
	ARMOR_All_DATA			m_eArmorDesc = ARMOR_All_DATA::ArmorAll_END;
	SHOP_OPTION				m_eType = SHOP_END;

	_bool					m_bLateInit = false;

public:
	static CWeaponBuyUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
