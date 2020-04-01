#pragma once
#include "Client_Defines.h"

#include "UI.h"
#include "Weapon.h"

BEGIN(Client)
class CWeapon;
class CSelect_UI;
class CWeapon_Slot final : public CUI
{
private:
	explicit CWeapon_Slot(_Device pDevice);
	explicit CWeapon_Slot(const CWeapon_Slot& rhs);
	virtual ~CWeapon_Slot() = default;

public:
	bool Get_Select() { return m_bIsSelect; }
	void Set_Select(_bool bIsSelect) { m_bIsSelect = bIsSelect; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	_bool Pt_InRect();
	CWeapon::WEAPON_DATA Get_Type();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();


private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;


	CSelect_UI*				m_pSelectUI = nullptr;
	_bool					m_bIsSelect = false;

public:
	static CWeapon_Slot*		Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};

END

