#include "stdafx.h"
#include "Scene_Stage_Base.h"

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

	// 메쉬 생성합니다.
	g_pManagement->LoadCreateObject_FromPath(m_pGraphic_Device, L"Object_Stage_00.dat");

	// 플레이어의 네비 메쉬도 바꿔줍니다.
	Ready_Player();

	return S_OK;
}

_int CScene_Stage_Base::Update_Scene(_double TimeDelta)
{
	CUI_Manager::Get_Instance()->Update_UI();

	return _int();
}

HRESULT CScene_Stage_Base::Render_Scene()
{

	return S_OK;
}

HRESULT CScene_Stage_Base::Ready_Player()
{
	CGameObject* pInstance = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);

	pInstance->Set_Enable(true);

	TARGET_TO_NAV(pInstance)->Reset_NaviMesh();
	TARGET_TO_NAV(pInstance)->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_StageBase.dat");
	TARGET_TO_TRANS(pInstance)->Set_Pos(V3_NULL);
	TARGET_TO_TRANS(pInstance)->Set_Angle(V3_NULL);

	pInstance = nullptr;;

	return S_OK;
}

HRESULT CScene_Stage_Base::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	// For.Sky
	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_Sky", SCENE_STAGE, pLayerTag)))
		return E_FAIL;

	return S_OK;
}


HRESULT CScene_Stage_Base::Ready_LightDesc()
{
	//디렉셔널 + 포인트 라이트로 바꿉니다.

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

