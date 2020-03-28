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
	CItem*	Get_Item(CItem::ITEM_ID eID, _uint iIndex);
	_uint	Get_ItemCount(CItem::ITEM_ID eID);
	_bool	Get_IsUpdateItem() { return m_bUpdateData; }
	CItem*	Get_ItemBack(CItem::ITEM_ID eID);

public:
	void	Set_IsUpdateItem() { m_bUpdateData = false; }

public:
	HRESULT Ready_Item_Prototype(_Device pDevice);
	void Add_Item(CItem::ITEM_TYPE eType, CItem::ITEM_ID eID);
	void Delete_Item(CItem::ITEM_ID eID);
	void Save_Item();
	void Load_Item();

private:
	vector<CItem*>	m_vecItem[CItem::ID_END];
	_bool			m_bUpdateData = false;
	void Add_Item(CItem::ITEM_TYPE eType, _uint iItemNum);
	vector<CItemSlot*>* Get_Slot(CItem::ITEM_TYPE eType) { return &m_SlotVec[eType]; }

private:
	vector<CItem*> m_ItemVec;
	

public:
	virtual void Free();
};

END