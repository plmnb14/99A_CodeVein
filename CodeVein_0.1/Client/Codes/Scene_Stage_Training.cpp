#include "stdafx.h"
#include "Scene_Stage_Training.h"

#include "CameraMgr.h"
#include "Effect.h"
#include "UI.h"
#include "UI_Manager.h"
#include "BossHP.h"
#include "ParticleMgr.h"

#include "MassageUI.h"
#include "MonsterUI.h"

#include "MonsterHeaders.h"

#include "Player_Colleague.h"

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
	g_pManagement->LoadCreateObject_FromPath(m_pGraphic_Device, L"Object_Stage_Training.dat");

	return S_OK;
}

_int CScene_Stage_Training::Update_Scene(_double TimeDelta)
{
	CUI_Manager::Get_Instance()->Update_UI();
	
	//Create_Fog(TimeDelta);

	return _int();
}

HRESULT CScene_Stage_Training::Render_Scene()
{
	return S_OK;
}

HRESULT CScene_Stage_Training::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameObject* pInstance = g_pManagement->Get_GameObjectBack(pLayerTag, SCENE_MORTAL);

	pInstance->Set_Enable(true);

	TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	TARGET_TO_TRANS(pInstance)->Set_Pos(V3_NULL);
	TARGET_TO_TRANS(pInstance)->Set_Angle(V3_NULL);

	return S_OK;
}

HRESULT CScene_Stage_Training::Ready_Layer_Enemies()
{
	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Monster")))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Boss")))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_MonsterProjectile")))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_BossUI")))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Mistletoe")))
		return E_FAIL;

	CGameObject* pInstance = nullptr;

	// ³×¸ð³×¸ð µ¿·á
	//pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_Colleague", nullptr);
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(5.f, 0.f, 5.f));
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Colleague", nullptr);

	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_BossMassageUI", SCENE_STAGE, L"Layer_BossMassageUI")))
	//	return E_FAIL;

	/*if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_Get_Item", SCENE_STAGE, L"Layer_Get_ItemUI")))
		return E_FAIL;*/

	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_Pickup_Item", SCENE_STAGE, L"Layer_PickUPUI")))
	//	return E_FAIL;

	// °ËÀº ´Á´ë
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_BlackWolf", nullptr);
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(4.f, 0.f, 4.f));
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	// ¾ßÂ÷¸Ç
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_YachaMan", nullptr);
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(4.f, 0.f, 4.f));
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	// ±èÇåÅÍ
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter", &CHunter::tagHunterInfo(25.f, 99999.f, 15.f, 5.f, 10.f, 10, CHunter::WEAPON_ANITYPE::GUN));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(4.f, 0.f, 4.f));
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	// ¸ùÅ°.D.·çÇÇ
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Monkey", &CMonkey::tagMonkeyInfo(25.f,120.f,15.f,5.f,10.f,10));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(4.f, 0.f, 4.f));
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	// ¿¹Æ¼
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Yeti", nullptr);
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(4.f, 0.f, 4.f));
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//====================================================================================================================================================
	// °Ë°ÕÁö
	//====================================================================================================================================================
	
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::White, CSwordGenji::Sit1, 10.f, 5.f, 2.f));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(-8.f, 0.f, -8.f));
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
	
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji", &CGunGenji::INFO(CGunGenji::White, CGunGenji::CheckGun, 10.f, 3.f, 2.f));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(8.f, 0.f, -8.f));
	////TARGET_TO_TRANS(pInstance)->Set_Pos(V3_NULL);
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji", &CGunGenji::INFO(CGunGenji::Jungle));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(V3_NULL);
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	//
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji", &CGunGenji::INFO(CGunGenji::Normal));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(V3_NULL);
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	
	//====================================================================================================================================================


	//====================================================================================================================================================
	// Ã¢ ¹æÆÐ °ÕÁö
	//====================================================================================================================================================
	
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordShieldGenji", &CSwordShieldGenji::INFO(CSwordShieldGenji::White, CSwordShieldGenji::LookAround1, 10.f, 5.f, 2.f));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(-8.f, 0.f, 8.f));
	////TARGET_TO_TRANS(pInstance)->Set_Pos(V3_NULL);
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

	//CGameObject* pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_Dummy", nullptr);
	//
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3{ 0.f, 0.f, 4.f });
	//
	//if (FAILED(g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr)))
	//	return E_FAIL;


	// µ¶³ª¹æ
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_PoisonButterfly", &CPoisonButterfly::INFO(10.f, 5.f, 2.f));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(8.f, 0.f, 8.f));
	////TARGET_TO_TRANS(pInstance)->Set_Pos(V3_NULL);
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Boss", nullptr);

	// ¿©¿ÕÀÇ ±â»ç
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_QueensKnight", &CQueensKnight::INFO(10.f, 5.f, 2.f));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(3.f, 0.f, 3.f));
	////TARGET_TO_TRANS(pInstance)->Set_Pos(V3_NULL);
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	// ¾óÀ½¿©ÀÚ
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_IceGirl", &CIceGirl::INFO(10.f, 5.f, 2.f));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(3.f, 0.f, 3.f));
	////TARGET_TO_TRANS(pInstance)->Set_Pos(V3_NULL);
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Boss", nullptr);

	// ºÒ³²ÀÚ
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_FireBoy", &CIceGirl::INFO(10.f, 5.f, 2.f));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(3.f, 0.f, 3.f));
	////TARGET_TO_TRANS(pInstance)->Set_Pos(V3_NULL);
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Boss", nullptr);

	// »ç½¿¿Õ
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_DeerKing", &CDeerKing::INFO(10.f, 5.f, 2.f));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(3.f, 0.f, 3.f));
	////TARGET_TO_TRANS(pInstance)->Set_Pos(V3_NULL);
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Boss", nullptr);

	// ³×¸ð³×¸ð µ¿·á
	//pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_Colleague", nullptr);
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(5.f, 0.f, 5.f));
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Boss", nullptr);

	return S_OK;
}

HRESULT CScene_Stage_Training::Ready_Layer_Environment()
{
	CGameObject* pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_Sky", nullptr);

	TARGET_TO_TRANS(pInstance)->Set_Angle(AXIS_Y, D3DXToRadian(0.f));

	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Sky", nullptr);

	return S_OK;
}

void CScene_Stage_Training::Create_Fog(_double TimeDelta)
{
	const _float FOG_OFFSET = 10.f;

	m_fMapFogDelay += _float(TimeDelta);
	if (m_fMapFogDelay > FOG_OFFSET)
	{
		m_fMapFogDelay = 0.f;

		for (_int i = 0; i < 100; ++i)
		{
			_mat matRotY;
			_v3 vDir = _v3(1.f, 0.f, 1.f);
			D3DXMatrixIdentity(&matRotY);

			D3DXMatrixRotationY(&matRotY, D3DXToRadian(_float(CCalculater::Random_Num_Double(0, 360))));
			D3DXVec3TransformNormal(&vDir, &vDir, &matRotY);
			D3DXVec3Normalize(&vDir, &vDir);

			_float fMinRange = 40.f;
			_float fRandRange = _float(CCalculater::Random_Num_Double(0, 30));
			_v3 vRandPos = vDir * (fMinRange + fRandRange);

			g_pManagement->Create_Effect(L"MapMist", vRandPos + _v3(0.f, -0.5f, 0.f), nullptr);
		}
	}
}


HRESULT CScene_Stage_Training::Ready_LightDesc()
{
	D3DLIGHT9		LightDesc;
	ZeroMemory(&LightDesc, sizeof(D3DLIGHT9));

	_v3 vLightPos = _v3(5.f, -8.f, -5.f);
	V3_NORMAL_SELF(&vLightPos);

	LightDesc.Type = D3DLIGHT_DIRECTIONAL;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;
	// In.WorldSpace
	LightDesc.Direction = vLightPos;

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
	CScene::Free();
}

