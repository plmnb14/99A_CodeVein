#include "stdafx.h"
#include "Scene_Stage_Base.h"
#include "Scene_Stage_Training.h"
#include "Scene_Stage_01.h"

#include "UI_Manager.h"

CScene_Stage_Base::CScene_Stage_Base(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CScene(pGraphic_Device)
{

}

HRESULT CScene_Stage_Base::Ready_Scene()
{
	// 빛 세팅
	if (FAILED(Ready_LightDesc()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(L"Layer_Sky")))
		return E_FAIL;

	// 메쉬 생성합니다.
	g_pManagement->LoadCreateObject_FromPath(m_pGraphic_Device, L"Object_Stage_00.dat");

	// 플레이어의 네비 메쉬도 바꿔줍니다.
	Ready_Player();

	if (false == g_bOnStage[6])
	{
		m_pLoading = CLoading::Create(m_pGraphic_Device, SCENE_PREPARE_ALL);

		g_bOnStage[6] = true;
	}

	return S_OK;
}

_int CScene_Stage_Base::Update_Scene(_double TimeDelta)
{
	CUI_Manager::Get_Instance()->Update_UI();

	return _int();
}

HRESULT CScene_Stage_Base::Render_Scene()
{
	if (m_pLoading != nullptr)
	{
		if (true == m_pLoading->Get_Finish() && g_pInput_Device->Key_Down(DIK_U))
		{
			CGameObject* pInstance = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);

			pInstance->Set_Enable(false);

			g_pManagement->Clear_LightList();

			if (FAILED(g_pManagement->Clear_Instance(SCENE_STAGE)))
				return -1;

			CScene* pScene = CScene_Stage_01::Create(m_pGraphic_Device, m_bLoadStaticMesh);

			if (FAILED(g_pManagement->SetUp_CurrentScene(pScene)))
				return -1;
		}
	}

	else
	{
		cout << " 으음~?" << endl;

		if (g_pInput_Device->Key_Down(DIK_U))
		{
			CGameObject* pInstance = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);

			pInstance->Set_Enable(false);

			g_pManagement->Clear_LightList();

			if (FAILED(g_pManagement->Clear_Instance(SCENE_STAGE)))
				return -1;

			CScene* pScene = CScene_Stage_01::Create(m_pGraphic_Device, m_bLoadStaticMesh);

			if (FAILED(g_pManagement->SetUp_CurrentScene(pScene)))
				return -1;
		}
	}
	return S_OK;
}

HRESULT CScene_Stage_Base::Ready_Player()
{
	CGameObject* pInstance = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);

	pInstance->Set_Enable(true);

	TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Stage_00.dat");
	TARGET_TO_NAV(pInstance)->Set_SubsetIndex(0);
	TARGET_TO_NAV(pInstance)->Set_Index(14);
	TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(-0.519f, 0.120f, 23.810f));
	TARGET_TO_TRANS(pInstance)->Set_Angle(V3_NULL);

	pInstance = nullptr;;

	return S_OK;
}

HRESULT CScene_Stage_Base::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	// For.Sky
	CGameObject* pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_Sky", nullptr);

	TARGET_TO_TRANS(pInstance)->Set_Angle(AXIS_Y, D3DXToRadian(220.f));

	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Sky", nullptr);

	return S_OK;
}


HRESULT CScene_Stage_Base::Ready_LightDesc()
{
	//디렉셔널 + 포인트 라이트로 바꿉니다.

	D3DLIGHT9		LightDesc;
	ZeroMemory(&LightDesc, sizeof(D3DLIGHT9));

	//_v3 tmpDir = _v3(-1.f, -1.f, 0.f);
	//
	//LightDesc.Type = D3DLIGHT_DIRECTIONAL;
	//LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//LightDesc.Ambient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f);
	//LightDesc.Specular = LightDesc.Diffuse;
	//LightDesc.Direction = *V3_NORMAL_SELF(&tmpDir);
	//
	//if (FAILED(g_pManagement->Add_Light(m_pGraphic_Device, LightDesc)))
	//	return E_FAIL;

	float fAmbient = 0.01f;

	LightDesc.Type = D3DLIGHT_POINT;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(fAmbient, fAmbient, fAmbient, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;
	LightDesc.Position = _v3(-0.f, 3.f, 0.f);
	LightDesc.Range = 8.f;

	if (FAILED(g_pManagement->Add_Light(m_pGraphic_Device, LightDesc)))
		return E_FAIL;


	LightDesc.Type = D3DLIGHT_POINT;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(fAmbient, fAmbient, fAmbient, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;
	LightDesc.Position = _v3(-0.f, 3.f, 5.f);
	LightDesc.Range = 8.f;

	if (FAILED(g_pManagement->Add_Light(m_pGraphic_Device, LightDesc)))
		return E_FAIL;

	LightDesc.Type = D3DLIGHT_POINT;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(fAmbient, fAmbient, fAmbient, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;
	LightDesc.Position = _v3(-0.f, 4.f, 10.f);
	LightDesc.Range = 8.f;

	if (FAILED(g_pManagement->Add_Light(m_pGraphic_Device, LightDesc)))
		return E_FAIL;

	LightDesc.Type = D3DLIGHT_POINT;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(fAmbient, fAmbient, fAmbient, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;
	LightDesc.Position = _v3(-0.f, 4.f, 15.f);
	LightDesc.Range = 8.f;

	if (FAILED(g_pManagement->Add_Light(m_pGraphic_Device, LightDesc)))
		return E_FAIL;

	LightDesc.Type = D3DLIGHT_POINT;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(fAmbient, fAmbient, fAmbient, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;
	LightDesc.Position = _v3(-0.f, 4.f, 20.f);
	LightDesc.Range = 8.f;

	if (FAILED(g_pManagement->Add_Light(m_pGraphic_Device, LightDesc)))
		return E_FAIL;

	return NOERROR;
}

CScene_Stage_Base * CScene_Stage_Base::Create(LPDIRECT3DDEVICE9 pGraphic_Device, _bool _bLoadStatic)
{
	CScene_Stage_Base*	pInstance = new CScene_Stage_Base(pGraphic_Device);

	pInstance->m_bLoadStaticMesh = _bLoadStatic;

	if (FAILED(pInstance->Ready_Scene()))
	{
		MSG_BOX("Failed To Creating CScene_Stage_Base");
		Safe_Release(pInstance);
	}

	pInstance->m_bLoadStaticMesh = _bLoadStatic;

	return pInstance;
}

void CScene_Stage_Base::Free()
{
	Safe_Release(m_pNavMesh);

	CScene::Free();
}

