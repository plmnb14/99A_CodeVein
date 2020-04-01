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
	//vector<CWeapon::WEAPON_DATA>*	Get_WeaponData() { return &m_vecWeaponData; }
	_uint Get_WeaponDataSize() { return _uint(m_vecWeaponData.size()); }
	
	CWeapon::WEAPON_DATA Get_WeaponData(_uint iIndex);
	

public:
	HRESULT Add_Item_Prototype(_Device pDevice);
	

	void Add_Expendables(CExpendables::EXPEND_TYPE eType); // 해당 타입의 소모품 획득
	void Add_QuickSlot(CExpendables_Slot* pExpendSlot); //  소모품 인벤 슬롯 -> 퀵슬롯에 등록
	void Delete_QuickSlot(CExpendables_Slot* pExpendSlot); // 해당 슬롯에 들어있는 아이템 타입과 크기를 비교해서 같으면 퀵슬롯에서 삭제
	void Delete_Expendable(CExpendables::EXPEND_TYPE eType);
	void Delete_Expendables(CExpendables_Slot* pExpendSlot, _uint iDeletCnt); // 해당 슬롯에서 몇개를 팔것인지 설정

	void Add_Material(CMaterial::MATERIAL_TYPE eType);
	void Delete_Material(CMaterial::MATERIAL_TYPE eType);
	void Delete_Materials(CMaterial_Slot* pMaterialSlot, _uint iDeleteCnt);

	void Add_WeaponData(CWeapon::WEAPON_DATA WeaponData);
	void Delete_WeaponData(CWeapon::WEAPON_DATA WeaponData);

private:
	vector<CExpendables*>	m_vecExpendables;
	vector<CExpendables_Slot*>	m_vecQuickSlot;
	vector<CMaterial*>		m_vecMaterial;
	vector<CWeapon::WEAPON_DATA>	m_vecWeaponData;


public:
	virtual void Free();
};

END