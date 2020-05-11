#pragma once

#include "Management.h"
#include "Client_Defines.h"

#include "Expendables.h"
#include "Expendables_Slot.h"

#include "Material.h"
#include "Material_Slot.h"

#include "Weapon.h"
#include "Weapon_Slot.h"

BEGIN(Client)

class CItem_Manager : public CBase
{
	DECLARE_SINGLETON(CItem_Manager)

private:
	explicit CItem_Manager();
	virtual ~CItem_Manager();

public:
	/*소모품*/
	vector<CExpendables*>*	Get_Expendables() { return &m_vecExpendables; } // 가지고 있는 소모품 정보들
	vector<CExpendables_Slot*>*	Get_QuickSlot() { return &m_vecQuickSlot; } // 현재 등록된 퀵슬롯 정보들
	_uint Get_QuickSlotSize() { return _uint(m_vecQuickSlot.size()); } // 현재 등록된 퀵슬롯 사이즈 반환
																	   /*재료(혈영)*/
	vector<CMaterial*>*		Get_Material() { return &m_vecMaterial; }
	/*무기 데이터*/
	
	_uint Get_WeaponDataSize() { return _uint(m_vecWeaponData.size()); }
	
	
	

public:
	HRESULT Add_Item_Prototype(_Device pDevice);
	

private:
	vector<CExpendables*>	m_vecExpendables;
	vector<CExpendables_Slot*>	m_vecQuickSlot;
	vector<CMaterial*>		m_vecMaterial;
	vector<WEAPON_DATA>	m_vecWeaponData;


public:
	virtual void Free();
};

END