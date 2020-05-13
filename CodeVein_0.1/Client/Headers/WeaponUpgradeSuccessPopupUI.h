#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Material.h"

BEGIN(Client)
class CWeapon_Inven_InShop;
class CPlayerFontUI;
class CWeaponUpgradeOptionUI;
class CWeaponUpgradeSuccessPopupUI final : public CUI
{
public:

private:
	explicit CWeaponUpgradeSuccessPopupUI(_Device pDevice);
	explicit CWeaponUpgradeSuccessPopupUI(const CWeaponUpgradeSuccessPopupUI& rhs);
	virtual ~CWeaponUpgradeSuccessPopupUI() = default;

public:
	void	Set_Active(_bool bIsActive);

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
	void	Check_Option();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	CWeapon_Inven_InShop*				m_pInven = nullptr;
	vector<CWeaponUpgradeOptionUI*>		m_vecOption;

	_bool								m_bLateInit = false;
	_int								m_iTexIdx = 0;
public:
	static CWeaponUpgradeSuccessPopupUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
