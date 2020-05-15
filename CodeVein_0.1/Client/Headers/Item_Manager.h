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
	
	_uint Get_WeaponDataSize() { return _uint(m_vecWeaponData.size()); }
	
	
public:
	_uint	Get_PickUp_Number() { return m_iPickUp_ItemNumber; }
	_bool	Get_PickUI() { return m_bCheck_PickUI; }

	void	Set_PickUp_Number(_uint _iNumber) { m_iPickUp_ItemNumber = _iNumber; }
	void	Set_PickUI(_bool _Check) { m_bCheck_PickUI = _Check; }

public:
	HRESULT Add_Item_Prototype(_Device pDevice);
	

private:
	vector<CExpendables*>		m_vecExpendables;
	vector<CExpendables_Slot*>	m_vecQuickSlot;
	vector<CMaterial*>			m_vecMaterial;
	vector<WEAPON_DATA>			m_vecWeaponData;

	_uint		m_iPickUp_ItemNumber = 0;

	_bool		m_bCheck_PickUI = false;


public:
	virtual void Free();
};

END