#include "stdafx.h"
#include "Scene_Stage_02.h"

#include "CameraMgr.h"
#include "Effect.h"
#include "UI.h"
#include "UI_Manager.h"
#include "ParticleMgr.h"
#include "ScriptManager.h"

CScene_Stage_02::CScene_Stage_02(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CScene(pGraphic_Device)
{

}

HRESULT CScene_Stage_02::Ready_Scene()
{
	if (FAILED(Ready_LightDesc()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(L"Layer_Player")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Environment(L"Layer_Environment")))
		return E_FAIL;

	//if (m_bLoadStaticMesh)
	g_pManagement->LoadCreateObject_FromPath(m_pGraphic_Device, L"Object_Stage_02.dat");

	CScriptManager::Get_Instance()->Set_StageIdx(1);
	CScriptManager::Get_Instance()->Ready_Script_DynamicObject(1);

	return S_OK;
}

_int CScene_Stage_02::Update_Scene(_double TimeDelta)
{
	CUI_Manager::Get_Instance()->Update_UI();

	return _int();
}

HRESULT CScene_Stage_02::Render_Scene()
{

	return S_OK;
}

HRESULT CScene_Stage_02::Ready_Layer_Player(const _tchar * pLayerTag)
{
	// 몬스터 레이어만 미리 추가
	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Monster")))
		return E_FAIL;

	// 보스 레이어만 미리 추가
	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Boss")))
		return E_FAIL;

	// 투사체 레이어 추가
	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_MonsterProjectile")))
		return E_FAIL;

	CGameObject* pInstance = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);

	pInstance->Set_Enable(true);

	TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Stage_01.dat");
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(0);
	TARGET_TO_NAV(pInstance)->Set_Index(5);
	TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(150.484f, -18.08f, 70.417f));
	TARGET_TO_TRANS(pInstance)->Set_Angle(V3_NULL);

	pInstance = nullptr;;

	return S_OK;


	//if(FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_PlayerHP", SCENE_STAGE, L"Layer_PlayerHP")))
	//	return E_FAIL;
	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_PlayerST", SCENE_STAGE, L"Layer_PlayerST")))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CScene_Stage_02::Ready_Layer_Environment(const _tchar* pLayerTag)
{
	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_Sky", SCENE_STAGE, pLayerTag)))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_BossMassageUI", SCENE_STAGE, L"Layer_BossMassageUI")))
		return E_FAIL;

	return S_OK;
}


HRESULT CScene_Stage_02::Ready_LightDesc()
{
	D3DLIGHT9		LightDesc;
	ZeroMemory(&LightDesc, sizeof(D3DLIGHT9));

	LightDesc.Type = D3DLIGHT_DIRECTIONAL;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 0.882f, 0.801f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;
	// In.WorldSpace
	_v3 vLightDir = _v3(0.2f, 1.f, 0.9f);

	V3_NORMAL_SELF(&vLightDir);

	LightDesc.Direction = vLightDir;
	//LightDesc.Direction = _v3(0.0f, 0.f, 1.f);

	if (FAILED(g_pManagement->Add_Light(m_pGraphic_Device, LightDesc)))
		return E_FAIL;

	return NOERROR;
}

CScene_Stage_02 * CScene_Stage_02::Create(LPDIRECT3DDEVICE9 pGraphic_Device, _bool _bLoadStatic)
{
	CScene_Stage_02*	pInstance = new CScene_Stage_02(pGraphic_Device);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MSG_BOX("Failed To Creating CScene_Stage_02");
		Safe_Release(pInstance);
	}

	pInstance->m_bLoadStaticMesh = _bLoadStatic;

	return pInstance;
}

void CScene_Stage_02::Free()
{
	//Safe_Release(m_pNavMesh);

	CScene::Free();
}

