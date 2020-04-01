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
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable();

public:
	void Regist_Weapon(CWeapon::WEAPON_DATA eData);
	void Unregist_Weapon(CWeapon_Slot* pWeaponSlot);

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	_bool					m_bIsActive;
	vector<CWeapon_Slot*>	m_vecWeaponSlot;
	CWeapon_Slot*			m_pSelectSlot[2];

public:
	static CWeapon_Inven*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END

