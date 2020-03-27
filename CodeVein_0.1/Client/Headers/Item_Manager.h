#pragma once

#include "Management.h"
#include "Client_Defines.h"

#include "Item.h"

BEGIN(Client)

class CItem_Manager : public CBase
{
	DECLARE_SINGLETON(CItem_Manager)

private:
	explicit CItem_Manager();
	virtual ~CItem_Manager();

public:
	_uint		Get_ItemCnt(CItem::ITEM_TYPE eType, _uint iItemNumber);

public:
	HRESULT		Ready_Item_Prototype(_Device pDevice);
	void		Add_ConsumeItem(_uint iItemNum);
	void		Add_MaterialItem(_uint iItemNum);
	
private:
	vector<CItem*>	m_ConsumeVec[16];
	vector<CItem*>	m_MaterialVec[16];
	vector<CItem*>	m_ArmorVec[16];
	vector<CItem*>	m_WeaponVec[16];

public:
	virtual void Free();
};

END