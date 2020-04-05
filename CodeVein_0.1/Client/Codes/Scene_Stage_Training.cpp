#include "stdafx.h"
#include "Scene_Stage_Training.h"

#include "CameraMgr.h"
#include "Effect.h"
#include "UI.h"
#include "UI_Manager.h"
#include "ParticleMgr.h"

#include "SwordGenji.h"
#include "GunGenji.h"
#include "SwordShieldGenji.h"


CScene_Stage_Training::CScene_Stage_Training(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CScene(pGraphic_Device)
{

}

HRESULT CScene_Stage_Training::Ready_Scene()
{
	if (FAILED(Ready_LightDesc()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(L"Layer_Player")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Enemies()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Environment()))
		return E_FAIL;

	// Æ®·¹ÀÌ´× ¸ÊÀº ±×³É ·Îµå °¡´ÉÇØ¿è
	g_pManagement->LoadCreateObject_FromPath(m_pGraphic_Device, L"Stage_Training.dat");

	return S_OK;
}

_int CScene_Stage_Training::Update_Scene(_double TimeDelta)
{
	CUI_Manager::Get_Instance()->Update_UI();

	return _int();
}

HRESULT CScene_Stage_Training::Render_Scene()
{
	return S_OK;
}

HRESULT CScene_Stage_Training::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameObject* pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_Player" , nullptr);
	
	TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	TARGET_TO_TRANS(pInstance)->Set_Pos(V3_NULL);
	TARGET_TO_TRANS(pInstance)->Set_Angle(V3_NULL);

	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Player", nullptr);

	return S_OK;
}

HRESULT CScene_Stage_Training::Ready_Layer_Dummy(const _tchar * pLayerTag)
{
	// µð¹ö±ë¿ë ´õ¹Ì Å¸°Ù
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_Dummy", SCENE_STAGE, pLayerTag)))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////


	return S_OK;
}

HRESULT CScene_Stage_Training::Ready_Layer_Enemies()
{
	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Monster")))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Boss")))
		return E_FAIL;

	CGameObject* pInstance = nullptr;

	
	// °ËÀº ¼º°Ô
	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"Monster_BlackUrchin", SCENE_STAGE, L"Layer_Monster")))
	//	return E_FAIL;
	// °ËÀº ´Á´ë
	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"Monster_BlackWolf", SCENE_STAGE, L"Layer_Monster")))
	//	return E_FAIL;

	//====================================================================================================================================================
	// ÃÑ°ÕÁö
	//====================================================================================================================================================
	
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji" , &CSwordGenji::INFO(CSwordGenji::White));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(V3_NULL);
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::Jungle));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(V3_NULL);
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::Normal));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(V3_NULL);
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	
	//====================================================================================================================================================


	//====================================================================================================================================================
	// ÃÑ°ÕÁö
	//====================================================================================================================================================
	
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji", &CGunGenji::INFO(CGunGenji::White));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(V3_NULL);
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji", &CGunGenji::INFO(CGunGenji::Jungle));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(V3_NULL);
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji", &CGunGenji::INFO(CGunGenji::Normal));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(V3_NULL);
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	
	//====================================================================================================================================================


	//====================================================================================================================================================
	// Ã¢ ¹æÆÐ °ÕÁö
	//====================================================================================================================================================
	
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordShieldGenji" , &CSwordShieldGenji::INFO(CSwordShieldGenji::White));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(V3_NULL);
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordShieldGenji", &CSwordShieldGenji::INFO(CSwordShieldGenji::Jungle));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(V3_NULL);
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordShieldGenji", &CSwordShieldGenji::INFO(CSwordShieldGenji::Normal));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(V3_NULL);
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	
	//====================================================================================================================================================

	// ¾ßÂ÷¸Ç
	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"Monster_YachaMan", SCENE_STAGE, L"Layer_Monster")))
	//	return E_FAIL;

	//CGameObject* pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_Dummy", nullptr);
	//
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3{ 0.f, 0.f, 4.f });
	//
	//if (FAILED(g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr)))
	//	return E_FAIL;



	// µ¶³ª¹æ
	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"Monster_PoisonButterfly", SCENE_STAGE, L"Layer_Monster")))
	//	return E_FAIL;

	// Åõ»çÃ¼ ·¹ÀÌ¾î¸¸ ¹Ì¸® Ãß°¡
	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_MonsterProjectile")))
		return E_FAIL;

	return S_OK;
}

HRESULT CScene_Stage_Training::Ready_Layer_Environment()
{

	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_Sky", SCENE_STAGE, L"Layer_Sky")))
		return E_FAIL;

	return S_OK;
}


HRESULT CScene_Stage_Training::Ready_LightDesc()
{
	D3DLIGHT9		LightDesc;
	ZeroMemory(&LightDesc, sizeof(D3DLIGHT9));

	LightDesc.Type = D3DLIGHT_DIRECTIONAL;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;
	// In.WorldSpace
	LightDesc.Direction = _v3(1.f, 1.f, -1.f);
	
	if (FAILED(g_pManagement->Add_Light(m_pGraphic_Device, LightDesc)))
		return E_FAIL;

	//LightDesc.Type = D3DLIGHT_POINT;
	//LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//LightDesc.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
	//LightDesc.Specular = LightDesc.Diffuse;
	//// In.WorldSpace
	////LightDesc.Direction = _v3(1.f, 1.f, -1.f);
	//LightDesc.Position = _v3(45.f, 45.f, 40.f);
	//LightDesc.Range = 3000.f;
	//
	//if (FAILED(g_pManagement->Add_Light(m_pGraphic_Device, LightDesc)))
	//	return E_FAIL;

	return NOERROR;
}

CScene_Stage_Training * CScene_Stage_Training::Create(LPDIRECT3DDEVICE9 pGraphic_Device, _bool _bLoadStatic)
{
	CScene_Stage_Training*	pInstance = new CScene_Stage_Training(pGraphic_Device);

	pInstance->m_bLoadStaticMesh = _bLoadStatic;

	if (FAILED(pInstance->Ready_Scene()))
	{
		MSG_BOX("Failed To Creating CScene_Stage_Training");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScene_Stage_Training::Free()
{
	Safe_Release(m_pNavMesh);

	CScene::Free();
}

