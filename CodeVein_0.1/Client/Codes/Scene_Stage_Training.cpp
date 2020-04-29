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
#include "Flag.h"

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

	// Ʈ���̴� ���� �׳� �ε� �����ؿ�
	g_pManagement->LoadCreateObject_FromPath(m_pGraphic_Device, L"Object_Stage_Training.dat");

	return S_OK;
}

_int CScene_Stage_Training::Update_Scene(_double TimeDelta)
{
	CUI_Manager::Get_Instance()->Update_UI();
	
	//Create_Fog(TimeDelta);
	//Create_Snow(TimeDelta);

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

	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Pet")))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_MonsterProjectile")))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_BossUI")))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Mistletoe")))
		return E_FAIL;

	CGameObject* pInstance = nullptr;

	// �׸�׸� ����
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

	// ����
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Cocoon", &MONSTER_STATUS(MONSTER_COLORTYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None));
	TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(6.f, 0.f, 6.f));
	TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	// ����
	pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Cocoon", &MONSTER_STATUS(MONSTER_COLORTYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None));
	TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(4.f, 0.f, 4.f));
	TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//// ����
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter", &MONSTER_STATUS(MONSTER_COLORTYPE::COLOR_NONE, WEAPON_STATE::WEAPON_Gun));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(4.f, 0.f, 4.f));
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//// ��Ű
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Monkey", &MONSTER_STATUS(MONSTER_COLORTYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(4.f, 0.f, 4.f));
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//// Ÿ�̸�
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_ThaiMan", &MONSTER_STATUS(MONSTER_COLORTYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(4.f, 0.f, 4.f));
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//// ����
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Urchin", &MONSTER_STATUS(MONSTER_COLORTYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(4.f, 0.f, 4.f));
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//// ����
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Wolf", &MONSTER_STATUS(MONSTER_COLORTYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(4.f, 0.f, 4.f));
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//// ������
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_YachaMan", &MONSTER_STATUS(MONSTER_COLORTYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(4.f, 0.f, 4.f));
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//// ��Ƽ
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Yeti", &MONSTER_STATUS(MONSTER_COLORTYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(4.f, 0.f, 4.f));
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	// ��_������
	pInstance = g_pManagement->Clone_GameObject_Return(L"Pet_PoisonButterFly", nullptr);
	TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(4.f, 0.f, 4.f));
	TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Pet", nullptr);

	//====================================================================================================================================================
	// �˰���
	//====================================================================================================================================================

	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::Normal, CSwordGenji::Sit1, 70.f, 10.f, 1.f));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(-8.f, 0.f, -8.f));
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji", &CSwordGenji::INFO(CSwordGenji::Jungle, CSwordGenji::Sit2, 10.f, 5.f, 2.f));
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
	// �Ѱ���
	//====================================================================================================================================================
	
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji", &CGunGenji::INFO(CGunGenji::Normal, CGunGenji::CheckGun, 10.f, 3.f, 2.f));
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
	// â ���� ����
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


	// ������
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_PoisonButterfly", &CPoisonButterfly::INFO(10.f, 5.f, 2.f));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(8.f, 0.f, 8.f));
	////TARGET_TO_TRANS(pInstance)->Set_Pos(V3_NULL);
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Boss", nullptr);

	// ������ ���
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_QueensKnight", &CQueensKnight::INFO(10.f, 5.f, 2.f));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(3.f, 0.f, 3.f));
	////TARGET_TO_TRANS(pInstance)->Set_Pos(V3_NULL);
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	// ��������
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_IceGirl", &CIceGirl::INFO(10.f, 5.f, 2.f));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(3.f, 0.f, 3.f));
	////TARGET_TO_TRANS(pInstance)->Set_Pos(V3_NULL);
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Boss", nullptr);

	// �ҳ���
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_FireBoy", &CIceGirl::INFO(10.f, 5.f, 2.f));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(3.f, 0.f, 3.f));
	////TARGET_TO_TRANS(pInstance)->Set_Pos(V3_NULL);
	//TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	//TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Boss", nullptr);

	// �罿��
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_DeerKing", &CDeerKing::INFO(10.f, 5.f, 2.f));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(3.f, 0.f, 3.f));
	////TARGET_TO_TRANS(pInstance)->Set_Pos(V3_NULL);
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

	pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_Flag", &CFlag::INFO(5, 5, 2.f, 1.f));
	TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(0.f, 3.f, 3.f));
	//static_cast<CFlag*>(pInstance)->Set_Wind();	// �������ϸ� ����Ʈ ����
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Sky", nullptr);

	return S_OK;
}

void CScene_Stage_Training::Create_Fog(_double TimeDelta)
{
	CGameObject* pPlayer = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);
	if (!pPlayer)
		return;

	CTransform* pPlayerTrans = TARGET_TO_TRANS(pPlayer);
	_v3 vPlayerPos = pPlayerTrans->Get_Pos();

	const _float FOG_OFFSET = 10.f;

	//m_fMapFogDelay += _float(TimeDelta);
	//if (m_fMapFogDelay > FOG_OFFSET)
	//{
	//	m_fMapFogDelay = 0.f;
	//
	//	for (_int i = 0; i < 30; ++i)
	//	{
	//		_mat matRotY;
	//		_v3 vDir = _v3(1.f, 0.f, 1.f);
	//		D3DXMatrixIdentity(&matRotY);
	//
	//		D3DXMatrixRotationY(&matRotY, D3DXToRadian(_float(CCalculater::Random_Num_Double(0, 360))));
	//		D3DXVec3TransformNormal(&vDir, &vDir, &matRotY);
	//		D3DXVec3Normalize(&vDir, &vDir);
	//
	//		_float fMinRange = 5.f;
	//		_float fRandRange = _float(CCalculater::Random_Num_Double(0, 20));
	//		_v3 vRandPos = vDir * (fMinRange + fRandRange);
	//
	//		g_pManagement->Create_Effect(L"MapMist", vRandPos + _v3(0.f, -0.5f, 0.f), pPlayerTrans);
	//	}
	//}
	//
	const _float DUST_OFFSET = 0.3f;

	m_fMapWindDustDelay += _float(TimeDelta);
	if (m_fMapWindDustDelay > DUST_OFFSET)
	{
		m_fMapWindDustDelay = 0.f;

		for (_int i = 0; i < 10; ++i)
		{
			_mat matRotY;
			_v3 vDir = _v3(1.f, 0.f, 1.f);
			D3DXMatrixIdentity(&matRotY);

			D3DXMatrixRotationY(&matRotY, D3DXToRadian(_float(CCalculater::Random_Num_Double(0, 360))));
			D3DXVec3TransformNormal(&vDir, &vDir, &matRotY);
			D3DXVec3Normalize(&vDir, &vDir);

			_float fMinRange = 1.f;
			_float fRandRange = _float(CCalculater::Random_Num_Double(0, 30));
			_v3 vRandPos = vDir * (fMinRange + fRandRange) + _v3(0.f, 0.2f, 0.f);

			g_pManagement->Create_Effect(L"MapDust", vPlayerPos + vRandPos + _v3(0.f, _float(CCalculater::Random_Num_Double(0, 0.5)), 0.f), nullptr);

			fRandRange = _float(CCalculater::Random_Num_Double(0, 20));
			vRandPos = vDir * (fMinRange + fRandRange);
			g_pManagement->Create_Effect(L"MapDust_2", vPlayerPos + vRandPos + _v3(0.f, _float(CCalculater::Random_Num_Double(0, 0.5)), 0.f), nullptr);
		}
	}
}

void CScene_Stage_Training::Create_Snow(_double TimeDelta)
{
	CGameObject* pPlayer = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);
	if (!pPlayer)
		return;

	const _float SNOW_OFFSET = 0.3f;

	m_fMapSnowDelay += _float(TimeDelta);
	if (m_fMapSnowDelay > SNOW_OFFSET)
	{
		m_fMapSnowDelay = 0.f;

		CTransform* pPlayerTrans = TARGET_TO_TRANS(pPlayer);
		_v3 vPlayerPos = pPlayerTrans->Get_Pos();

		g_pManagement->Create_Effect(L"MapSnow", vPlayerPos + _v3(0.f, 10.f, 0.f), nullptr);
	}
}


HRESULT CScene_Stage_Training::Ready_LightDesc()
{
	NEW_LIGHT		LightDesc;
	ZeroMemory(&LightDesc, sizeof(NEW_LIGHT));

	_v3 vLightPos = _v3(5.f, -8.f, -5.f);
	V3_NORMAL_SELF(&vLightPos);

	LightDesc.Type = D3DLIGHT_DIRECTIONAL;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;
	// In.WorldSpace
	LightDesc.Direction = vLightPos;

	LightDesc.fAlpha = 1.f;

	if (FAILED(g_pManagement->Add_Light(m_pGraphic_Device, LightDesc, CLight_Manager::Static_Light)))
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

