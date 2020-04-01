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
	/*�Ҹ�ǰ*/
	vector<CExpendables*>*	Get_Expendables() { return &m_vecExpendables; } // ������ �ִ� �Ҹ�ǰ ������
	vector<CExpendables_Slot*>*	Get_QuickSlot() { return &m_vecQuickSlot; } // ���� ��ϵ� ������ ������
	_uint Get_QuickSlotSize() { return _uint(m_vecQuickSlot.size()); } // ���� ��ϵ� ������ ������ ��ȯ
																	   /*���(����)*/
	vector<CMaterial*>*		Get_Material() { return &m_vecMaterial; }
	/*���� ������*/
	//vector<CWeapon::WEAPON_DATA>*	Get_WeaponData() { return &m_vecWeaponData; }
	_uint Get_WeaponDataSize() { return _uint(m_vecWeaponData.size()); }
	
	CWeapon::WEAPON_DATA Get_WeaponData(_uint iIndex);
	

public:
	HRESULT Add_Item_Prototype(_Device pDevice);
	

	void Add_Expendables(CExpendables::EXPEND_TYPE eType); // �ش� Ÿ���� �Ҹ�ǰ ȹ��
	void Add_QuickSlot(CExpendables_Slot* pExpendSlot); //  �Ҹ�ǰ �κ� ���� -> �����Կ� ���
	void Delete_QuickSlot(CExpendables_Slot* pExpendSlot); // �ش� ���Կ� ����ִ� ������ Ÿ�԰� ũ�⸦ ���ؼ� ������ �����Կ��� ����
	void Delete_Expendable(CExpendables::EXPEND_TYPE eType);
	void Delete_Expendables(CExpendables_Slot* pExpendSlot, _uint iDeletCnt); // �ش� ���Կ��� ��� �Ȱ����� ����

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