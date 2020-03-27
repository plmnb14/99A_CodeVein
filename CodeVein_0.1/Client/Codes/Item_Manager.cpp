#include "stdafx.h"
#include "..\Headers\Item_Manager.h"

#include "Consume_Item.h"

IMPLEMENT_SINGLETON(CItem_Manager)

CItem_Manager::CItem_Manager()
{
}


CItem_Manager::~CItem_Manager()
{
}

_uint CItem_Manager::Get_ItemCnt(CItem::ITEM_TYPE eType, _uint iItemNumber)
{
	if (0 == iItemNumber)
		return 0;

	_uint iItemCnt = 0;

	switch (eType)
	{
	case CItem::CONSUME:
		iItemCnt = _uint(m_ConsumeVec[iItemNumber].size());
		break;
	case CItem::MATERIAL:
		iItemCnt = _uint(m_MaterialVec[iItemNumber].size());
		break;
	case CItem::ARMOR:
		iItemCnt = _uint(m_ArmorVec[iItemNumber].size());
		break;
	case CItem::WEAPON:
		iItemCnt = _uint(m_WeaponVec[iItemNumber].size());
		break;
	}

	return _uint(iItemCnt);
}


HRESULT CItem_Manager::Ready_Item_Prototype(_Device pDevice)
{
	g_pManagement->Add_Prototype(L"GameObject_Consume_Item", CConsume_Item::Create(pDevice));

	return NOERROR;
}

void CItem_Manager::Add_ConsumeItem(_uint iItemNum)
{
	if (0 == iItemNum)
		return;

	CItem* pConsume_Item = static_cast<CItem*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Consume_Item", nullptr));
	pConsume_Item->Set_ItemNumber(iItemNum);
	m_ConsumeVec[iItemNum].push_back(pConsume_Item);
}

void CItem_Manager::Add_MaterialItem(_uint iItemNum)
{
}





void CItem_Manager::Free()
{
	LOOP(16)
	{
		for_each(m_ConsumeVec[i].begin(), m_ConsumeVec[i].end(), Safe_Release<CItem*>);
	}
	LOOP(16)
	{
		for_each(m_MaterialVec[i].begin(), m_MaterialVec[i].end(), Safe_Release<CItem*>);
	}
	LOOP(16)
	{
		for_each(m_ArmorVec[i].begin(), m_ArmorVec[i].end(), Safe_Release<CItem*>);
	}
	LOOP(16)
	{
		for_each(m_WeaponVec[i].begin(), m_WeaponVec[i].end(), Safe_Release<CItem*>);
	}
}
