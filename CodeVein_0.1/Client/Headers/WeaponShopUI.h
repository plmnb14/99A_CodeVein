#pragma once

#include "Client_Defines.h"
#include "UI.h"

#include "WeaponShopOptionUI.h"
#include "WeaponBuyUI.h"
#include "Weapon_Inven_InShop.h"

BEGIN(Client)
class CWeaponShopUI final : public CUI
{
private:
	explicit CWeaponShopUI(_Device pDevice);
	explicit CWeaponShopUI(const CWeaponShopUI& rhs);
	virtual ~CWeaponShopUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	void Click_Option();
	void Reset_Option();
	void Check_Key();

public:
	void Active_SubUI(CWeaponShopOptionUI* pSelectOption);

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
	CWeapon_Inven_InShop*			m_pInven;
public:
	static CWeaponShopUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
