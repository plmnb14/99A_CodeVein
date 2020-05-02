#include "..\Headers\ObjectPool.h"
#include "..\Headers\Management.h"

CObjectPool::CObjectPool()
{
}

HRESULT CObjectPool::Ready_ObjectPool(const _tchar* pPrototypeTag, _uint iPoolSize)
{
	for (_uint i = 0; i < iPoolSize; ++i)
	{
		CGameObject* pInstance = CManagement::Get_Instance()->Clone_GameObject_Return(pPrototypeTag, nullptr);

		if (nullptr == pInstance)
			return E_FAIL;

		m_Container.push(pInstance);
	}

	return S_OK;
}

_int CObjectPool::Update_ObjectPool(_double dTimeDelta)
{
	_int iProgress = 0;

	for (auto& iter_begin = m_listRunningObject.begin(); iter_begin != m_listRunningObject.end(); )
	{
		iProgress = (*iter_begin)->Update_GameObject(dTimeDelta);

		if (DEAD_OBJ == iProgress)
		{
			m_Container.push((*iter_begin));

			iter_begin = m_listRunningObject.erase(iter_begin);
		}
		else
			iter_begin++;
	}

	return NO_EVENT;
}

_int CObjectPool::LateUpdate_ObjectPool(_double dTimeDelta)
{
	for (auto& Object : m_listRunningObject)
		Object->Late_Update_GameObject(dTimeDelta);

	return NO_EVENT;
}

void CObjectPool::Create_Object(void * pArg)
{
	if (0 < m_Container.size())
	{
		CGameObject* pInstance = m_Container.front();

		pInstance->Ready_GameObject(pArg);

		m_listRunningObject.push_back(pInstance);
		m_Container.pop();
	}

}

CObjectPool * CObjectPool::Create_ObjectPool(const _tchar* pPrototypeTag, _uint iPoolSize)
{
	CObjectPool* pInstance = new CObjectPool;

	if (FAILED(pInstance->Ready_ObjectPool(pPrototypeTag, iPoolSize)))
	{
		MSG_BOX("Failed To Create CObjectPool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CObjectPool::Free()
{
	size_t iQueueSize = m_Container.size();
	for (size_t i = 0; i < iQueueSize; ++i)
	{
		Safe_Release(m_Container.front());
		m_Container.pop();
	}

	for (auto& i : m_listRunningObject)
		Safe_Release(i);

	m_listRunningObject.clear();

}
