#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CPlayer;
class CWeapon;
class CWeapon_Slot;
class CExplainWeaponUI;
class CWeapon_Inven final : public CUI
{
private:
	explicit CWeapon_Inven(_Device pDevice);
	explicit CWeapon_Inven(const CWeapon_Inven& rhs);
	virtual ~CWeapon_Inven() = default;

public:
	WPN_PARAM Get_UseWeaponParam(_uint iIndex) { return m_UseWeaponParam[iIndex]; }
	vector<CWeapon_Slot*>* Get_VecWeaponSlot() { return &m_vecWeaponSlot; }

public:
	HRESULT Set_WeaponData_FromWeapon();

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable();
	void					Click_Inven();
	void					Regist_Weapon(CWeapon_Slot* pWeaponSlot);
	void					UnRegist_Weapon(CWeapon_Slot* pWeaponSlot);
	HRESULT					SetUp_WeaponData();
	void					SetUp_Default();
	void					SetUp_SlotPos();
	void					SetUp_SubUI_Active(_bool bIsActive);
	void					Late_Init();

public:
	void Add_Weapon(WPN_PARAM tAddWpnParam);
	void Sell_Weapon();

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	vector<CWeapon_Slot*>	m_vecWeaponSlot;
	WPN_PARAM				m_UseWeaponParam[2];
	WPN_PARAM				m_tWeaponParam[WPN_DATA_End + 1];
	CExplainWeaponUI*		m_pExplainUI = nullptr;
	_bool					m_bIsSubActive = false;

	CPlayer*				m_pPlayer = nullptr;
	CWeapon*				m_pTempWeapon = nullptr;

	_bool					m_bLateInit = false;
public:
	static CWeapon_Inven*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END

