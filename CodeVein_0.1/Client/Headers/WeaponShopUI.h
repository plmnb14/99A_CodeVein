#pragma once

#include "Client_Defines.h"
#include "UI.h"

#include "WeaponShopOptionUI.h"

BEGIN(Client)
class CWeaponBuyUI;
class CWeaponShopOptionUI;
class CWeapon_Inven_InShop;
class CWeaponShopUI final : public CUI
{
private:
	explicit CWeaponShopUI(_Device pDevice);
	explicit CWeaponShopUI(const CWeaponShopUI& rhs);
	virtual ~CWeaponShopUI() = default;

public:
	CWeapon_Inven_InShop*	Get_InvenBuyWeapon() { return m_pWeaponBuyInven; }
	CWeapon_Inven_InShop*	Get_InvenBuyArmor() { return m_pArmorBuyInven; }
	CWeapon_Inven_InShop*	Get_InvenSellWeapon() { return m_pWeaponSellInven; }
	CWeapon_Inven_InShop*	Get_InvenSellArmor() { return m_pArmorSellInven; }
	CWeapon_Inven_InShop*	Get_InvenUpgradeWeapon() { return m_pWeaponUpgradeInven; }
	CWeapon_Inven_InShop*	Get_InvenUpgradeArmor() { return m_pArmorUpgradeInven; }
	_bool					Get_OtherPopupOn();

public:
	void Set_PopupOn(_bool bPopup) { m_bPopupOn = bPopup; }
	void Setup_AfterClone();
	void Set_Active(_bool bActive);

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(CShader* pShader);
	void Click_Option();
	void Reset_Option();
	void Check_Key();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	_float					m_fAlpha = 0.f;
	_uint					m_iSelectIndex = 0;
	_float					m_fCross = 100.f;
	_bool					m_bFirestMenu = false;
	_bool					m_bPopupOn = false;

	CWeaponShopOptionUI::WEAPONSHOP_OPTION	m_eState = CWeaponShopOptionUI::WEAPONSHOP_OPTION::OPTION_END;

	vector<CWeaponShopOptionUI*>	m_vecOption;
	CWeaponBuyUI*					m_pBuyUI;
	CWeapon_Inven_InShop*			m_pWeaponBuyInven;
	CWeapon_Inven_InShop*			m_pArmorBuyInven;
	CWeapon_Inven_InShop*			m_pWeaponSellInven;
	CWeapon_Inven_InShop*			m_pArmorSellInven;
	CWeapon_Inven_InShop*			m_pWeaponUpgradeInven;
	CWeapon_Inven_InShop*			m_pArmorUpgradeInven;

	_bool					m_bIsSubActive = false;

public:
	static CWeaponShopUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
