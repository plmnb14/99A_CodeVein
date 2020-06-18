#pragma once

#include "Pet_Slot.h"
#include "Inventory_Icon.h"

BEGIN(Client)

class CPet;

class CPet_Inven final : public CUI
{
private:
	explicit CPet_Inven(_Device pDevice);
	explicit CPet_Inven(const CPet_Inven& rhs);
	virtual ~CPet_Inven() = default;

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

	void					Add_Pet(ITEM_NAMETYPE ePetName);

private:
	void					Click_Inven();
	void					Reset_SlotSelect();
	void					Check_Call_Pet(_bool _Check_Get_SlotSelect, _uint _Idx, ITEM_NAMETYPE _eType);
	void					SetUp_SlotPos();
	void					SetUp_SubUI_Active(_bool bIsActive);
	HRESULT					Add_Component();
	HRESULT					SetUp_Default();
	HRESULT					SetUp_ConstantTable();

public:
	static CPet_Inven*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();

private:
	CBuffer_RcTex*			m_pBuffer = nullptr;
	CTransform*				m_pTransform = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CTexture*				m_pTexture = nullptr;
	CShader*				m_pShader = nullptr;

	_bool					m_bIsSubActive = false;
	CInventory_Icon*		m_pExitIcon = nullptr;

	vector<CPet_Slot*>		m_vecPetSlot;

};

END