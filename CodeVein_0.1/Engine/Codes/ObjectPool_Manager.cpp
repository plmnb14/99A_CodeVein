#include "..\Headers\ObjectPool_Manager.h"

IMPLEMENT_SINGLETON(CObjectPool_Manager)

CObjectPool_Manager::CObjectPool_Manager()
{

}

_int CObjectPool_Manager::Update_ObjectPool(_double dTimeDelta)
{
	for (auto& i : m_Container)
	{
		i.second->Update_ObjectPool(dTimeDelta);
	}

	return _int();
}

_int CObjectPool_Manager::LateUpdate_ObjectPool(_double dTimeDelta)
{
	for (auto& i : m_Container)
	{
		i.second->LateUpdate_ObjectPool(dTimeDelta);
	}

	return _int();
}

HRESULT CObjectPool_Manager::Create_Object(const _tchar * pContainerTag, void* pArg)
{
	auto iter = find_if(m_Container.begin(), m_Container.end(), CTag_Finder(pContainerTag));

	if (iter == m_Container.end())
		return E_FAIL;

	iter->second->Create_Object(pArg);

	return S_OK;
}

HRESULT CObjectPool_Manager::Create_ObjectPool(const _tchar* pContainerTag, const _tchar * pPrototypeTag, _uint iPoolSize)
{
	CObjectPool* pObjectPool = CObjectPool::Create_ObjectPool(pPrototypeTag, iPoolSize);

	if (nullptr == pObjectPool)
		return E_FAIL;

	auto iter = find_if(m_Container.begin(), m_Container.end(), CTag_Finder(pContainerTag));

	if (iter == m_Container.end())
	{
		m_Container.insert(CONTAINER::value_type(pContainerTag, pObjectPool));
	}
	else
	{
		MSG_BOX("CObjectPool_Manager : Tag already exist");
		return E_FAIL;
	}

	return S_OK;
}

void CObjectPool_Manager::Free()
{
	for (auto& i : m_Container)
	{
		Safe_Release(i.second);
	}
	m_Container.clear();
}
