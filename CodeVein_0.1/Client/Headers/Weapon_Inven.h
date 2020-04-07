#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Weapon_Slot.h"

BEGIN(Client)

class CWeapon;
class CWeapon_Slot;
class CWeapon_Inven final : public CUI
{
private:
	explicit CWeapon_Inven(_Device pDevice);
	explicit CWeapon_Inven(const CWeapon_Inven& rhs);
	virtual ~CWeapon_Inven() = default;

public:
	WEAPON_STATE Get_UseWeaponState(_uint iIndex) { return m_UseWeapon[iIndex]; }

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

public:
	void Add_Weapon(WEAPON_STATE eType);

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	vector<CWeapon_Slot*>	m_vecWeaponSlot;
	WEAPON_STATE			m_UseWeapon[2];

public:
	static CWeapon_Inven*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END

