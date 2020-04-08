#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CQuickSlot;
class CExpendables_Inven;
class CMaterial_Inven;
class CWeapon_Inven;
class CArmor_Inven;
class CInventory_Icon;
class CTotal_Inven;
class CInventory final : public CUI
{
private:
	explicit CInventory(_Device pDevice);
	explicit CInventory(const CInventory& rhs);
	virtual ~CInventory() = default;

public:
	_bool Get_Detail() { return m_bIsDetail; }

public:
	void Set_Detail(_bool bIsDetail) { m_bIsDetail = bIsDetail; }

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable();
	void					SetUp_Default();
	void					Click_Icon();

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

private:
	CQuickSlot*				m_pQuickSlot = nullptr;
	CExpendables_Inven*		m_pExpInven = nullptr;
	CMaterial_Inven*		m_pMtrInven = nullptr;
	CWeapon_Inven*			m_pWeaponInven = nullptr;
	CArmor_Inven*			m_pArmorInven = nullptr;
	CTotal_Inven*			m_pTotalInven = nullptr;
	vector<CInventory_Icon*> m_vecIcon;
	CInventory_Icon*		m_pClickIcon = nullptr;
	_bool					m_bIsDetail = false;
	

public:
	static CInventory*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END