#include "stdafx.h"
#include "..\Headers\Item_Manager.h"

IMPLEMENT_SINGLETON(CItem_Manager)

CItem_Manager::CItem_Manager()
{
}


CItem_Manager::~CItem_Manager()
{
}

void CItem_Manager::Add_Item(CItem::ITEM_TYPE eType, _uint iItemNum)
{
	CItem* pItem = nullptr;

	switch (eType)
	{
	case CItem::CONSUME:
		pItem = static_cast<CItem*>(g_pManagement->Get_GameObjectBack(L"Layer_ConsumeItem", SCENE_STAGE));
		pItem->Set_ItemNumber(iItemNum);
		break;
	default:
		return;
	}
	
	m_ItemVec.push_back(pItem);

	m_SlotVec[eType][iItemNum]->Add_Item(L"GameObject_ConsumeItem");
}

void CItem_Manager::Free()
{
	for_each(m_ItemVec.begin(), m_ItemVec.end(), Safe_Release<CItem*>);
	m_ItemVec.clear();
	m_ItemVec.shrink_to_fit();

	LOOP(CItem::ITEM_END)
	{
		for_each(m_SlotVec[i].begin(), m_SlotVec[i].end(), Safe_Release<CItemSlot*>);
		m_SlotVec[i].clear();
		m_SlotVec[i].shrink_to_fit();
	}
	
}
