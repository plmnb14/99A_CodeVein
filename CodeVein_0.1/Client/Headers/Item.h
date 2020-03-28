#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CItem : public CGameObject
{
public:
	enum ITEM_TYPE
	{
		CONSUME,	// 소비 아이템
		MATERIAL,	// 재료 아이템
		TYPE_END
	};
	enum ITEM_ID
	{
		ID_NONE,
		CONS_HP,
		CONS_SP,
		MAT_1,
		MAT_2,
		MAT_3,
		ID_END
	};
	typedef struct tagItemInfo
	{
		ITEM_TYPE	eType;
		ITEM_ID		eID;
	}ITEM_INFO;

protected:
	explicit CItem(_Device pGraphic_Device);
	explicit CItem(const CItem& rhs);
	virtual ~CItem() = default;

public:
	ITEM_TYPE	Get_ItemType() { return m_eType; }
	ITEM_ID		Get_ItemID() { return m_eID; }

public:
	ITEM_TYPE Get_Type() { return m_eType; }

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);

protected:
	ITEM_INFO*				m_pInfo = nullptr;
	ITEM_TYPE				m_eType;
	ITEM_ID					m_eID;

public:
	static CItem*			Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END