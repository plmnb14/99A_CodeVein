#include "stdafx.h"
#include "..\Headers\Item_Manager.h"



IMPLEMENT_SINGLETON(CItem_Manager)

CItem_Manager::CItem_Manager()
{
}


CItem_Manager::~CItem_Manager()
{
	Free();
}

CItem * CItem_Manager::Get_Item(CItem::ITEM_ID eID, _uint iIndex)
{
	return m_vecItem[eID][iIndex];
}

_uint CItem_Manager::Get_ItemCount(CItem::ITEM_ID eID)
{
	return _uint(m_vecItem[eID].size());
}

CItem * CItem_Manager::Get_ItemBack(CItem::ITEM_ID eID)
{
	if (m_vecItem[eID].size() <= 0)
		return nullptr;

	return m_vecItem[eID].back();
}

HRESULT CItem_Manager::Ready_Item_Prototype(_Device pDevice)
{
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Item", CItem::Create(pDevice))))
		return E_FAIL;

	return NOERROR;
}

void CItem_Manager::Add_Item(CItem::ITEM_TYPE eType, CItem::ITEM_ID eID)
{
	CItem::ITEM_INFO* pItemInfo = nullptr;
	pItemInfo = new CItem::ITEM_INFO;

	pItemInfo->eType = eType;
	pItemInfo->eID = eID;

	g_pManagement->Add_GameObject_ToLayer(L"GameObject_Item", SCENE_STAGE, L"Layer_Item", pItemInfo);

	CItem* pItem = static_cast<CItem*>(g_pManagement->Get_GameObjectBack(L"Layer_Item", SCENE_STAGE));

	m_vecItem[eID].push_back(pItem);

	m_bUpdateData = true;
}

void CItem_Manager::Delete_Item(CItem::ITEM_ID eID)
{
	if (0 >= m_vecItem[eID].size() || eID == CItem::ID_NONE)
		return;

	m_vecItem[eID].pop_back();
}

void CItem_Manager::Free()
{
	LOOP(CItem::ID_END)
		for_each(m_vecItem[i].begin(), m_vecItem[i].end(), Safe_Release<CItem*>);
}

