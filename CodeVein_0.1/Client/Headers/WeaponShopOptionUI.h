#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CWeaponShopOptionUI final : public CUI
{
public:
	enum WEAPONSHOP_MENU
	{
		MENU_NONE,
		MENU_UPGRADE,
		MENU_BUY,
		MENU_SELL,
		MENU_END
	};

	enum WEAPONSHOP_OPTION
	{
		OPTION_UPGRADE,
		OPTION_BUY,
		OPTION_SELL,
		OPTION_BYEBYE,
		OPTION_END
	};
	enum WEAPONSHOP_OPTION_BUY
	{
		OPTION_BUY_WEAPON,
		OPTION_BUY_DRAINWEAPON,
		OPTION_BUY_END
	};
private:
	explicit CWeaponShopOptionUI(_Device pDevice);
	explicit CWeaponShopOptionUI(const CWeaponShopOptionUI& rhs);
	virtual ~CWeaponShopOptionUI() = default;

public:
	WEAPONSHOP_OPTION Get_Option() { return m_eOption; }

public:
	void Set_Select(_bool bIsSelect) { m_bIsSelect = bIsSelect; }
	void Set_Option(WEAPONSHOP_OPTION eOption) { m_eOption = eOption; }
	void Set_BuyOption(WEAPONSHOP_OPTION_BUY eOption) { m_eBuyOption = eOption; }
	void Set_Menu(WEAPONSHOP_MENU eMenu) { m_eMenu = eMenu; }
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iIndex);

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CCollider*				m_pCollider = nullptr;

private:
	_float					m_fAlpha = 0.f;
	_bool					m_bIsSelect = false;

	WEAPONSHOP_OPTION		m_eOption = OPTION_END;
	WEAPONSHOP_OPTION_BUY	m_eBuyOption = OPTION_BUY_END;
	WEAPONSHOP_MENU			m_eMenu = MENU_END;

public:
	static CWeaponShopOptionUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
