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
		CONSUME,
		MATERIAL,
		QUEST,
		ITEM_END
	};

protected:
	explicit CItem(_Device pGraphic_Device);
	explicit CItem(const CItem& rhs);
	virtual ~CItem() = default;

public:
	ITEM_TYPE Get_Type() { return m_eType; }
	_uint		Get_ItemNumber() { return m_iItemNumber; }

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

protected:
	ITEM_TYPE				m_eType;
	_uint					m_iItemNumber = 0; // 아이템 식별번호

public:
	virtual CGameObject*	Clone_GameObject(void* pArg) = 0;
	virtual void			Free();
};

END