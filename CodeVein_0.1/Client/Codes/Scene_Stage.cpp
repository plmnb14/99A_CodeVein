#include "stdafx.h"
#include "..\Headers\Scene_Stage.h"
#include "CameraMgr.h"
#include "Effect.h"
#include "UI.h"
#include "UI_Manager.h"

CScene_Stage::CScene_Stage(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CScene(pGraphic_Device)
{

}

HRESULT CScene_Stage::Ready_Scene()
{
	if (FAILED(Ready_LightDesc()))
		return E_FAIL;

	
	if (FAILED(Ready_Layer_Camera(L"Layer_Camera")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(L"Layer_Player")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(L"Layer_Monster")))
		return E_FAIL;

	g_pManagement->LoadCreateObject_FromPath(m_pGraphic_Device, L"Stage_Test.dat");

	m_pNavMesh = static_cast<Engine::CNavMesh*>(g_pManagement->Clone_Component(SCENE_STATIC, L"NavMesh"));
	m_pNavMesh->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Test.dat");


	if(FAILED(CUI_Manager::Get_Instance()->SetUp_UILayer()))
		return E_FAIL;

	return S_OK;
}

_int CScene_Stage::Update_Scene(_double TimeDelta)
{
	

	// -------------- UI Manager ----------------------

	CUI_Manager::Get_Instance()->Update_UI();

	return _int();
}

HRESULT CScene_Stage::Render_Scene()
{
	m_pNavMesh->Render_NaviMesh();

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Player(const _tchar * pLayerTag)
{
	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_Player", SCENE_STAGE, pLayerTag)))
		return E_FAIL;

	return S_OK;
}
HRESULT CScene_Stage::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	// 여기서 카메라 준비

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"Monster_TestMonster", SCENE_STAGE, pLayerTag)))
		return E_FAIL;

	// 디버깅용 더미 타겟
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_Dummy", SCENE_STAGE, L"Layer_Dummy")))
		return E_FAIL;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_Sky", SCENE_STAGE, pLayerTag)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	return S_OK;
}


HRESULT CScene_Stage::Ready_LightDesc()
{
	D3DLIGHT9		LightDesc;
	ZeroMemory(&LightDesc, sizeof(D3DLIGHT9));

	LightDesc.Type = D3DLIGHT_DIRECTIONAL;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;
	// In.WorldSpace
	LightDesc.Direction = _v3(1.f, 1.f, -1.f);

	if (FAILED(g_pManagement->Add_Light(m_pGraphic_Device, LightDesc)))
		return E_FAIL;

	return NOERROR;
}

CScene_Stage * CScene_Stage::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CScene_Stage*	pInstance = new CScene_Stage(pGraphic_Device);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MSG_BOX("Failed To Creating CScene_Stage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScene_Stage::Free()
{
	Safe_Release(m_pNavMesh);

	CScene::Free();
}

