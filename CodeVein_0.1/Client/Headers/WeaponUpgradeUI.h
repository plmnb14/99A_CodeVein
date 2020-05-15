#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Material.h"

BEGIN(Client)
class CWeapon_Inven_InShop;
class CPlayerFontUI;
class CWeaponUpgradeOptionUI;
class CWeaponUpgradeSuccessPopupUI;
class CShopActionFailedPopup;
class CShopItemIcon;
class CWeaponUpgradeUI final : public CUI
{
public:
	enum MOVE_TYPE
	{
		MOVE_STEP,
		MOVE_ROLLING,
		MOVE_HEAVYROLLING,
		MOVE_END
	};
	enum UPGRADE_TYPE
	{
		UPGRADE_WEAPON,
		UPGRADE_ARMOR,
		UPGRADE_END
	};
private:
	explicit CWeaponUpgradeUI(_Device pDevice);
	explicit CWeaponUpgradeUI(const CWeaponUpgradeUI& rhs);
	virtual ~CWeaponUpgradeUI() = default;

public:
	void	Set_Active(_bool bIsActive);
	void	Set_Inven(CWeapon_Inven_InShop* pParent) { m_pInven = pParent; }
	void	Set_WeaponDescType(WEAPON_ALL_DATA eType);
	void	Set_ArmorDescType(ARMOR_All_DATA eType);
	void	Set_WeaponMoveType(MOVE_TYPE eType);
	void	Set_UpgradeType(UPGRADE_TYPE eType) { m_eUpgradeType = eType; }
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

	void	Check_LateInit();
	void	Check_ItemOption_Weapon();
	void	Check_ItemOption_Armor();
	void	Check_WeaponName();
	void	Check_MoveType();
	void	Check_Option();

	void	Upgrade_Weapon();
	void	Upgrade_Armor();

	_int	Get_MyMaterial(CMaterial::MATERIAL_TYPE eType);
	_int	Get_RequireMaterial(CMaterial::MATERIAL_TYPE eType, _int iReinforce);
	_float	Get_PlusDamage(_float fDamage, _int iReinforce);
	_bool	Get_UpgradeSuccess(_int iReinforce);
	_int	Get_UpgradeSuccessPercentage(_int iReinforce);

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	CWeapon_Inven_InShop*				m_pInven = nullptr;
	vector<CWeaponUpgradeOptionUI*>		m_vecOption;

	CShopActionFailedPopup*				m_pShopActionFailedPopup = nullptr;
	CWeaponUpgradeSuccessPopupUI*		m_pUpgradeResultPopup = false;
	CShopItemIcon*						m_pShopItemIcon = nullptr;

	CWeaponUpgradeUI*					m_pWeaponNameUI = nullptr;
	CWeaponUpgradeUI*					m_pWeaponMoveTypeUI = nullptr;

	UPGRADE_TYPE						m_eUpgradeType = UPGRADE_END;
	WEAPON_ALL_DATA						m_eWeaponDesc = WEAPON_ALL_DATA::WpnAll_END;
	ARMOR_All_DATA						m_eArmorDesc = ARMOR_All_DATA::ArmorAll_END;
	MOVE_TYPE							m_eMoveType = MOVE_TYPE::MOVE_END;

	CPlayerFontUI*						m_pFontReinforceHeader = nullptr;
	CPlayerFontUI*						m_pFontReinforceDesc = nullptr;
	CPlayerFontUI*						m_pFontReinforceAfterDesc = nullptr;
	CPlayerFontUI*						m_pFontPlusDamageDesc = nullptr;
	CPlayerFontUI*						m_pFontPlusDamageAfterDesc = nullptr;
	CPlayerFontUI*						m_pFontPlusHPDesc = nullptr;
	CPlayerFontUI*						m_pFontPlusHPAfterDesc = nullptr;
	CPlayerFontUI*						m_pFontTotalHPDesc = nullptr;
	CPlayerFontUI*						m_pFontTotalHPAfterDesc = nullptr;
	CPlayerFontUI*						m_pFontDamageDesc = nullptr;
	CPlayerFontUI*						m_pFontDamageAfterDesc = nullptr;

	CPlayerFontUI*						m_pFontRequireHaze = nullptr;
	CPlayerFontUI*						m_pFontMyHaze = nullptr;

	CPlayerFontUI*						m_pFontRequireMatCnt_0 = nullptr;
	CPlayerFontUI*						m_pFontRequireMatCnt_1 = nullptr;
	CPlayerFontUI*						m_pFontRequireMatCnt_2 = nullptr;
	CPlayerFontUI*						m_pFontMyMatCnt_0 = nullptr;
	CPlayerFontUI*						m_pFontMyMatCnt_1 = nullptr;
	CPlayerFontUI*						m_pFontMyMatCnt_2 = nullptr;
	CPlayerFontUI*						m_pFontSlash_0 = nullptr;
	CPlayerFontUI*						m_pFontSlash_1 = nullptr;
	CPlayerFontUI*						m_pFontSlash_2 = nullptr;

	CPlayerFontUI*						m_pFontUpgradePercentage = nullptr;

	_bool								m_bLateInit = false;
	_int								m_iTexIdx = 0;
public:
	static CWeaponUpgradeUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
