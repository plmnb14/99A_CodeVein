#pragma once

#include "Base.h"
#include "..\Headers\ObjectPool.h"

BEGIN(Engine)

class ENGINE_DLL CObjectPool_Manager final : public CBase
{
	DECLARE_SINGLETON(CObjectPool_Manager)

private:
	explicit CObjectPool_Manager();
	virtual ~CObjectPool_Manager() = default;

public:
	_int	Update_ObjectPool(_double dTimeDelta);
	_int	LateUpdate_ObjectPool(_double dTimeDelta);

public:
	HRESULT	Create_Object(const _tchar* pContainerTag, void* pArg);

public:
	HRESULT	Create_ObjectPool(const _tchar* pContainerTag, const _tchar* pPrototypeTag, _uint iPoolSize);

private:
	map<const _tchar*, CObjectPool*>			m_Container;
	typedef map<const _tchar*, CObjectPool*>	CONTAINER;
public:
	virtual void Free();

};

END