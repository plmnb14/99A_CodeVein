#include "stdafx.h"
#include "Scene_Stage_Training.h"

#include "CameraMgr.h"
#include "Effect.h"
#include "UI.h"
#include "UI_Manager.h"
#include "ParticleMgr.h"

#include "MassageUI.h"
#include "MonsterUI.h"


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

	// 트레이닝 맵은 그냥 로드 가능해욤
	g_pManagement->LoadCreateObject_FromPath(m_pGraphic_Device, L"Stage_Training.dat");

	m_pNavMesh = static_cast<Engine::CNavMesh*>(g_pManagement->Clone_Component(SCENE_STATIC, L"NavMesh"));
	m_pNavMesh->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");

	CNavMesh* pNavMesh = static_cast<Engine::CNavMesh*>(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE)->Get_Component(L"NavMesh"));
	pNavMesh->Reset_NaviMesh();
	pNavMesh->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");



	return S_OK;
}

_int CScene_Stage_Training::Update_Scene(_double TimeDelta)
{
	CUI_Manager::Get_Instance()->Update_UI();

	return _int();
}

HRESULT CScene_Stage_Training::Render_Scene()
{
	IF_NOT_NULL(m_pNavMesh)
		m_pNavMesh->Render_NaviMesh();

	return S_OK;
}

HRESULT CScene_Stage_Training::Ready_Layer_Player(const _tchar * pLayerTag)
{
	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_Player", SCENE_STAGE, pLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CScene_Stage_Training::Ready_Layer_Dummy(const _tchar * pLayerTag)
{
	// 디버깅용 더미 타겟
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_Dummy", SCENE_STAGE, pLayerTag)))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////


	return S_OK;
}

HRESULT CScene_Stage_Training::Ready_Layer_Enemies()
{
	// 몬스터 레이어만 미리 추가
	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Monster")))
		return E_FAIL;

	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_Dummy", SCENE_STAGE, L"Layer_Monster")))
	//	return E_FAIL;

	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"Monster_PoisonButterfly", SCENE_STAGE, L"Layer_Monster")))
		return E_FAIL;
	
	// 검은 성게
	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"Monster_BlackUrchin", SCENE_STAGE, L"Layer_Monster")))
	//	return E_FAIL;
	// 검은 늑대
	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"Monster_BlackWolf", SCENE_STAGE, L"Layer_Monster")))
	//	return E_FAIL;

	//// 검겐지
	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"Monster_SwordGenji", SCENE_STAGE, L"Layer_Monster", &CSwordGenji::INFO(CSwordGenji::White))))
	//	return E_FAIL;
	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"Monster_SwordGenji", SCENE_STAGE, L"Layer_Monster", &CSwordGenji::INFO(CSwordGenji::Normal))))
	//	return E_FAIL;
	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"Monster_SwordGenji", SCENE_STAGE, L"Layer_Monster", &CSwordGenji::INFO(CSwordGenji::Jungle))))
	//	return E_FAIL;

	//// 총겐지
	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"Monster_GunGenji", SCENE_STAGE, L"Layer_Monster", &CGunGenji::INFO(CGunGenji::White))))
	//	return E_FAIL;
	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"Monster_GunGenji", SCENE_STAGE, L"Layer_Monster", &CGunGenji::INFO(CGunGenji::Normal))))
	//	return E_FAIL;
	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"Monster_GunGenji", SCENE_STAGE, L"Layer_Monster", &CGunGenji::INFO(CGunGenji::Jungle))))
	//	return E_FAIL;


	//// 검방패겐지
	////if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"Monster_SwordShieldGenji", SCENE_STAGE, L"Layer_Monster")))
	////	return E_FAIL;
	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"Monster_SwordShieldGenji", SCENE_STAGE, L"Layer_Monster", &CSwordShieldGenji::INFO(CSwordShieldGenji::White))))
	//	return E_FAIL;
	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"Monster_SwordShieldGenji", SCENE_STAGE, L"Layer_Monster", &CSwordShieldGenji::INFO(CSwordShieldGenji::Normal))))
	//	return E_FAIL;
	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"Monster_SwordShieldGenji", SCENE_STAGE, L"Layer_Monster", &CSwordShieldGenji::INFO(CSwordShieldGenji::Jungle))))
	//	return E_FAIL;
	
	
	// 야차맨
	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"Monster_YachaMan", SCENE_STAGE, L"Layer_Monster")))
	//	return E_FAIL;


	CGameObject* pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_Dummy", nullptr);

	TARGET_TO_TRANS(pInstance)->Set_Pos(_v3{ 0.f, 0.f, 4.f });

	if (FAILED(g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr)))
		return E_FAIL;

	// 보스 레이어만 미리 추가
	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Boss")))
		return E_FAIL;

	// 독나방
	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"Monster_PoisonButterfly", SCENE_STAGE, L"Layer_Monster")))
	//	return E_FAIL;

	// 투사체 레이어만 미리 추가
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

