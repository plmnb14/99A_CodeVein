#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CPlayerFontUI;
class CShopItemIcon final : public CUI
{
private:
	explicit CShopItemIcon(_Device pDevice);
	explicit CShopItemIcon(const CShopItemIcon& rhs);
	virtual ~CShopItemIcon() = default;

public:
	void	Set_Active(_bool bIsActive);
	void	Set_Alpha(_float fAlpha);
	void	Set_WeaponDescType(WEAPON_ALL_DATA eType);
	void	Set_ArmorDescType(ARMOR_All_DATA eType);
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

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CBuffer_RcTex*		m_pBufferCom = nullptr;

private:
	WEAPON_ALL_DATA		m_eWeaponDesc = WEAPON_ALL_DATA::WpnAll_END;
	ARMOR_All_DATA		m_eArmorDesc = ARMOR_All_DATA::ArmorAll_END;

	_bool				m_bLateInit = false;
	_int				m_iTexIdx = 0;
	_float				m_fAlpha = 0.f;

public:
	static CShopItemIcon*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
