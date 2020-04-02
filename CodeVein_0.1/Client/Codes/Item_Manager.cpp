#include "stdafx.h"
#include "..\Headers\Item_Manager.h"

#include "Armor.h"

IMPLEMENT_SINGLETON(CItem_Manager)

CItem_Manager::CItem_Manager()
{

}


CItem_Manager::~CItem_Manager()
{
	Free();
}


CWeapon::WEAPON_DATA CItem_Manager::Get_WeaponData(_uint iIndex)
{
	if (iIndex >= m_vecWeaponData.size())
		return CWeapon::WPN_DATA_End;

	return m_vecWeaponData[iIndex];
}

HRESULT CItem_Manager::Add_Item_Prototype(_Device pDevice)
{
	g_pManagement->Add_Prototype(L"GameObject_Expendables", CExpendables::Create(pDevice));
	g_pManagement->Add_Prototype(L"GameObject_Material", CMaterial::Create(pDevice));

	g_pManagement->Add_Prototype(L"GameObject_Armor", CArmor::Create(pDevice));

	return NOERROR;
}


void CItem_Manager::Add_Expendables(CExpendables::EXPEND_TYPE eType)
{
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_Expendables", SCENE_STAGE, L"Layer_Expendables");
	CExpendables* pExpendables = static_cast<CExpendables*>(g_pManagement->Get_GameObjectBack(L"Layer_Expendables", SCENE_STAGE));
	pExpendables->Set_Type(eType);
	m_vecExpendables.push_back(pExpendables);
}

void CItem_Manager::Add_QuickSlot(CExpendables_Slot* pExpendSlot)
{
	m_vecQuickSlot.push_back(pExpendSlot);
}

void CItem_Manager::Delete_QuickSlot(CExpendables_Slot* pExpendSlot)
{
	for (_uint i = 0; i < m_vecQuickSlot.size(); ++i)
	{
		if (m_vecQuickSlot[i]->Get_Type() == pExpendSlot->Get_Type() &&
			m_vecQuickSlot[i]->Get_Size() == pExpendSlot->Get_Size())
			m_vecQuickSlot.erase(m_vecQuickSlot.begin() + i);
	}

}

void CItem_Manager::Delete_Expendable(CExpendables::EXPEND_TYPE eType)
{
	for (_uint i = 0; i < m_vecExpendables.size(); ++i)
	{
		if (m_vecExpendables[i]->Get_Type() == eType)
		{
			m_vecExpendables.erase(m_vecExpendables.begin() + i);
			break;
		}
	}
}

void CItem_Manager::Delete_Expendables(CExpendables_Slot* pExpendSlot, _uint iDeletCnt)
{
	if (iDeletCnt > pExpendSlot->Get_Size())
		return;

	for (_uint i = 0; i < iDeletCnt; ++i)
	{
		Delete_Expendable(CExpendables::EXPEND_TYPE(pExpendSlot->Get_Type()));
		pExpendSlot->Delete_Item();
	}
}

void CItem_Manager::Add_Material(CMaterial::MATERIAL_TYPE eType)
{
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_Material", SCENE_STAGE, L"Layer_Material");
	CMaterial* pMaterial = static_cast<CMaterial*>(g_pManagement->Get_GameObjectBack(L"Layer_Material", SCENE_STAGE));
	pMaterial->Set_Type(eType);
	m_vecMaterial.push_back(pMaterial);
}

void CItem_Manager::Delete_Material(CMaterial::MATERIAL_TYPE eType)
{
	for (_uint i = 0; i < m_vecMaterial.size(); ++i)
	{
		if (m_vecMaterial[i]->Get_Type() == eType)
		{
			m_vecMaterial.erase(m_vecMaterial.begin() + i);
			break;
		}
	}
}

void CItem_Manager::Delete_Materials(CMaterial_Slot * pMaterialSlot, _uint iDeleteCnt)
{
	if (iDeleteCnt > pMaterialSlot->Get_Size())
		return;

	for (_uint i = 0; i < iDeleteCnt; ++i)
	{
		Delete_Material(CMaterial::MATERIAL_TYPE(pMaterialSlot->Get_Type()));
		pMaterialSlot->Delete_Item();
		
	}
}

void CItem_Manager::Add_WeaponData(CWeapon::WEAPON_DATA WeaponData)
{
	if (m_vecWeaponData.size() == 2)
		return;
	
	m_vecWeaponData.push_back(WeaponData);
}

void CItem_Manager::Delete_WeaponData(CWeapon::WEAPON_DATA WeaponData)
{
	for (_uint i = 0; i < m_vecWeaponData.size(); ++i)
	{
		if (m_vecWeaponData[i] == WeaponData)
		{
			m_vecWeaponData.erase(m_vecWeaponData.begin() + i);
			break;
		}
	}
}


void CItem_Manager::Free()
{
}

