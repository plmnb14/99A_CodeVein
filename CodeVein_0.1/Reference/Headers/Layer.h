#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	explicit CLayer();
	virtual ~CLayer() = default;
public:
	CGameObject* Get_GameObjectBack();
	list<CGameObject*> Get_GameObjectList();

public: 
	HRESULT Ready_Layer();
	HRESULT Add_GameObject(CGameObject* pGameObject);
	_int Update_Layer(_double TimeDelta);
	_int Late_Update_Layer(_double TimeDelta);
private:
	list<CGameObject*>			m_ObjectList;
	typedef list<CGameObject*>	OBJECTLIST;
public:
	static CLayer* Create();
	virtual void Free();
};

END