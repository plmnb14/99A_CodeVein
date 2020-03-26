#pragma once

#include "Management.h"
#include "Client_Defines.h"

#include "Item.h"
#include "ItemSlot.h"

BEGIN(Client)

class CItem_Manager : public CBase
{
	DECLARE_SINGLETON(CItem_Manager)

private:
	explicit CItem_Manager();
	virtual ~CItem_Manager();

public:
	void Add_Item(CItem::ITEM_TYPE eType, _uint iItemNum);
	vector<CItemSlot*>* Get_Slot(CItem::ITEM_TYPE eType) { return &m_SlotVec[eType]; }

private:
	vector<CItem*> m_ItemVec;
	vector<CItemSlot*> m_SlotVec[CItem::ITEM_END];

public:
	virtual void Free();
};

END