#pragma once

#include "Base.h"
#include <queue>

BEGIN(Engine)

class CGameObject;

class CObjectPool final : public CBase
{
private:
	explicit CObjectPool();
	virtual ~CObjectPool() = default;

public:
	_int			Update_ObjectPool(_double dTimeDelta);
	_int			LateUpdate_ObjectPool(_double dTimeDelta);

public:
	void			Create_Object(void* pArg);

private:
	HRESULT			Ready_ObjectPool(const _tchar* pPrototypeTag, _uint iPoolSize);

private:
	queue<CGameObject*>			m_Container;

	list<CGameObject*>			m_listRunningObject;

public:
	static CObjectPool* Create_ObjectPool(const _tchar* pPrototypeTag, _uint iPoolSize);
	virtual void Free();

};

END