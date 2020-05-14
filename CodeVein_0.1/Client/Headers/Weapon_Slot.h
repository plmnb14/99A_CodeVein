#pragma once
#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CWeapon;
class CWeapon_Slot final : public CUI
{
private:
	explicit CWeapon_Slot(_Device pDevice);
	explicit CWeapon_Slot(const CWeapon_Slot& rhs);
	virtual ~CWeapon_Slot() = default;

public:
	bool Get_Select() { return m_bIsSelect; }
	bool Get_SelectShop() { return m_bIsSelectShop; }
	WPN_PARAM Get_WeaponParam() { return m_tWeaponParam; }

public:
	void Set_Select(_bool bIsSelect) { m_bIsSelect = bIsSelect; }
	void Set_SelectShop(_bool bIsSelect) { m_bIsSelectShop = bIsSelect; }
	void Set_WeaponParam(WPN_PARAM tWeaponParam) { m_tWeaponParam = tWeaponParam; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	_bool Pt_InRect();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iIndex);

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	_bool					m_bIsSelect = false;
	_bool					m_bIsSelectShop = false;
	WPN_PARAM				m_tWeaponParam;
	_bool					m_bIsCollMouse = false;

public:
	static CWeapon_Slot*		Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};

END

