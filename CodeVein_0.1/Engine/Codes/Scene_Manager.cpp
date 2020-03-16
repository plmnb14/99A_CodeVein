#include "..\Headers\Scene_Manager.h"

IMPLEMENT_SINGLETON(CScene_Manager)

CScene_Manager::CScene_Manager()
{

}

HRESULT CScene_Manager::SetUp_CurrentScene(CScene * pScene)
{
	if (nullptr == pScene)
	{
		return E_FAIL;
	}

	if (0 != Safe_Release(m_pCurrentScene))
	{

	}

	m_pCurrentScene = pScene;

	return S_OK;
}

_int CScene_Manager::Update_Scene(_double TimeDelta)
{
	if (nullptr == m_pCurrentScene)
		return -1;

	return m_pCurrentScene->Update_Scene(TimeDelta);	
}

HRESULT CScene_Manager::Render_Scene()
{
	if (nullptr == m_pCurrentScene)
		return -1;

	return m_pCurrentScene->Render_Scene();
}

void CScene_Manager::Free()
{
	Safe_Release(m_pCurrentScene);
}
