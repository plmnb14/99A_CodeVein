#pragma once

#include "Client_Defines.h"
#include "Client_Item_Enum.h"
#include "UI.h"

#include "Pet_Slot.h"
#include "Inventory_Icon.h"

BEGIN(Client)

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

private:
	void					Click_Inven();
	void					Reset_SlotSelect();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_Default();
	HRESULT					SetUp_ConstantTable();

public:
	void Add_Pet(CPet::PET_TYPE ePetType);

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

	CInventory_Icon*		m_pExitIcon = nullptr;

	vector<CPet_Slot*>		m_vecPetSlot;
};

END