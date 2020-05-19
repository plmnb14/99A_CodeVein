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

	void					Add_Pet(CPet::PET_TYPE ePetType);

private:
	void					Click_Inven();
	void					Reset_SlotSelect();
	void					Check_Call_Pet(_bool _Check_Get_SlotSelect, _uint _Idx, CPet::PET_TYPE _eType);

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

	CInventory_Icon*		m_pExitIcon = nullptr;
	CInventory_Icon*		m_pSummonsBtn = nullptr;

	vector<CPet_Slot*>		m_vecPetSlot;
	//CPet::PET_TYPE			m_eNowType = CPet::PET_TYPE::PET_TYPE_END;
	//
	//_bool					m_bCanActivePet = true;
	//_uint					m_ivectorNum = 999999;

};

END