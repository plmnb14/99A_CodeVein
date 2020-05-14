#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Material.h"

BEGIN(Client)
class CWeapon_Inven_InShop;
class CPlayerFontUI;
class CWeaponUpgradeOptionUI;
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
private:
	explicit CWeaponUpgradeUI(_Device pDevice);
	explicit CWeaponUpgradeUI(const CWeaponUpgradeUI& rhs);
	virtual ~CWeaponUpgradeUI() = default;

public:
	void	Set_Active(_bool bIsActive);
	void	Set_Inven(CWeapon_Inven_InShop* pParent) { m_pInven = pParent; }
	void	Set_WeaponDescType(WEAPON_ALL_DATA eType);
	void	Set_WeaponMoveType(MOVE_TYPE eType);

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
	void	Check_ItemOption();
	void	Check_WeaponName();
	void	Check_MoveType();
	void	Check_Option();

	void	Upgrade_Weapon();

	_int	Get_MyMaterial(CMaterial::MATERIAL_TYPE eType);
	_int	Get_RequireMaterial(CMaterial::MATERIAL_TYPE eType, _int iReinforce);
	_float	Get_PlusDamage(_float fDamage, _int iReinforce);
	_float	Get_UpgradePrice(_int iReinforce);

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	CWeapon_Inven_InShop*				m_pInven = nullptr;
	vector<CWeaponUpgradeOptionUI*>		m_vecOption;

	CWeaponUpgradeUI*					m_pWeaponNameUI = nullptr;
	CWeaponUpgradeUI*					m_pWeaponMoveTypeUI = nullptr;

	WEAPON_ALL_DATA						m_eWeaponDesc = WEAPON_ALL_DATA::WpnAll_END;
	MOVE_TYPE							m_eMoveType = MOVE_TYPE::MOVE_END;

	CPlayerFontUI*						m_pFontReinforceHeader = nullptr;
	CPlayerFontUI*						m_pFontReinforceDesc = nullptr;
	CPlayerFontUI*						m_pFontReinforceAfterDesc = nullptr;
	CPlayerFontUI*						m_pFontPlusDamageDesc = nullptr;
	CPlayerFontUI*						m_pFontPlusDamageAfterDesc = nullptr;
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

	_bool								m_bLateInit = false;
	_int								m_iTexIdx = 0;
public:
	static CWeaponUpgradeUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
