#include "..\Headers\Layer.h"

CLayer::CLayer()
{
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

	for (auto& pGameObject : m_ObjectList)
	{
		if (nullptr != pGameObject)
		{
			iProgress = pGameObject->Update_GameObject(TimeDelta);
			if (0 > iProgress)
				break;
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


