#include "..\Headers\Layer.h"

CLayer::CLayer()
{
}

CGameObject* CLayer::Get_GameObjectBack()
{
	if (m_ObjectList.empty())
		return nullptr;

	return m_ObjectList.back();
}

list<CGameObject*>& CLayer::Get_GameObjectList()
{
	return m_ObjectList;
}

HRESULT CLayer::Ready_Layer()
{
	return NOERROR;
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_ObjectList.push_back(pGameObject);

	return NOERROR;
}

_int CLayer::Update_Layer(_double TimeDelta)
{
	_int	iProgress = 0;

	auto& iter_begin = m_ObjectList.begin();
	auto& iter_end = m_ObjectList.end();
	for (; iter_begin != iter_end; )
	{
		if (nullptr != *iter_begin)
		{
			//if (!(*iter_begin)->Get_Enable())
			//{
			//	iter_begin++;
			//	continue;
			//}

			iProgress = (*iter_begin)->Update_GameObject(TimeDelta);
			
			if (DEAD_OBJ == iProgress)
			{
				Safe_Release((*iter_begin));

				iter_begin = m_ObjectList.erase(iter_begin);
			}

			else
				iter_begin++;
		}
	}

	return _int(iProgress);
}

_int CLayer::Late_Update_Layer(_double TimeDelta)
{
	_int	iProgress = 0;

	for (auto& pGameObject : m_ObjectList)
	{
		if (nullptr != pGameObject)
		{
			//if (!pGameObject->Get_Enable())
			//{
			//	continue;
			//}

			iProgress = pGameObject->Late_Update_GameObject(TimeDelta);

			if (0 > iProgress)
				break;
		}
	}

	return _int(iProgress); 
}

CLayer * CLayer::Create()
{
	CLayer*	pInstance = new CLayer();

	if (FAILED(pInstance->Ready_Layer()))
	{
		MSG_BOX("Failed To Creating CLayer");
		Safe_Release(pInstance);
	}

	return pInstance;

}

void CLayer::Free()
{
	for (auto& pGameObject : m_ObjectList)
		Safe_Release(pGameObject);

	m_ObjectList.clear();
}


