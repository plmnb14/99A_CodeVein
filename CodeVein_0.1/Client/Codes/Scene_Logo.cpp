#include "stdafx.h"
#include "Scene_Logo.h"
#include "Scene_Title.h"

#include "Management.h"

CScene_Logo::CScene_Logo(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CScene(pGraphic_Device)
	, m_pLoading(nullptr)
{

}

HRESULT CScene_Logo::Ready_Scene()
{
	if (FAILED(Ready_Essential_Prototype_GameObject()))
		return E_FAIL;

	m_pLoading = CLoading::Create(m_pGraphic_Device, SCENE_TITLE);
	if (nullptr == m_pLoading)
		return E_FAIL;

	return S_OK;
}

_int CScene_Logo::Update_Scene(_double TimeDelta)
{
	Logo_KeyInput();

	if (true == m_pLoading->Get_Finish() && g_pInput_Device->Key_Down(DIK_SPACE))
	{
		if (FAILED(g_pManagement->SetUp_CurrentScene(CScene_Title::Create(m_pGraphic_Device, m_eSceneChange, m_bLoadStaticMesh))))
			return -1;

		if (FAILED(g_pManagement->Clear_Instance(SCENE_LOGO)))
			return -1;

		return 0;
	}

	return NO_EVENT;
}

HRESULT CScene_Logo::Render_Scene()
{

	return S_OK;
}

CScene_Logo * CScene_Logo::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CScene_Logo*	pInstance = new CScene_Logo(pGraphic_Device);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MSG_BOX("Failed To Creating CScene_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScene_Logo::Free()
{
	Safe_Release(m_pLoading);

	CScene::Free();
}

HRESULT CScene_Logo::Ready_Essential_Prototype_GameObject()
{
	return S_OK;
}

HRESULT CScene_Logo::Ready_Layer_Logo(const _tchar * pLayerTag)
{
	return S_OK;
}

void CScene_Logo::Update_DebugStage_Console()
{
	system("cls");
	cout << "===============================================================================" << endl;
	cout << " 1. 위 숫자 키를 누르면 옵션이 활성화 됩니다." << endl;
	cout << " 2. (Load_StaticMesh) 가 (false) 이면 스테이지 들어가도 생성 안됩니다." << endl;
	cout << " 3. Space 를 누르면 다음 스테이지로 넘어갑니다." << endl;
	cout << " 4. 트레이닝 맵은 별도로 Load_StaticMesh 안해도 넘어갑니다." << endl;
	cout << " 5. 기본 설정된 맵은 Stage_Traing 입니다." << endl;

	cout << "-------------------------------------------------------------------------------" << endl;
	cout << "[1] Stage_Base = ";
	cout << (m_eSceneChange == CScene_Logo::Stage_Base ? "true" : "false") << endl;

	cout << "[2] Stage_Training = ";
	cout << (m_eSceneChange == CScene_Logo::Stage_Training ? "true" : "false") << endl;

	cout << "[3] Stage_01 = ";
	cout << (m_eSceneChange == CScene_Logo::Stage_01 ? "true" : "false") << endl;

	cout << "[4] Stage_02 = ";
	cout << (m_eSceneChange == CScene_Logo::Stage_02 ? "true" : "false") << endl;

	cout << "[5] Stage_03 = ";
	cout << (m_eSceneChange == CScene_Logo::Stage_03 ? "true" : "false") << endl;
	cout << "-------------------------------------------------------------------------------" << endl;
	cout << "[6] Load_StaticMesh = ";
	cout << (m_bLoadStaticMesh ? "true" : "false") << endl;
	cout << "-------------------------------------------------------------------------------" << endl;
}

void CScene_Logo::Logo_KeyInput()
{
	if (g_pInput_Device->Key_Down(DIK_1))
	{
		m_eSceneChange = Stage_Base;
		Update_DebugStage_Console();
	}

	if (g_pInput_Device->Key_Down(DIK_2))
	{
		m_eSceneChange = Stage_Training;
		Update_DebugStage_Console();
	}

	else if (g_pInput_Device->Key_Down(DIK_3))
	{
		m_eSceneChange = Stage_01;
		Update_DebugStage_Console();
	}

	else if (g_pInput_Device->Key_Down(DIK_4))
	{
		m_eSceneChange = Stage_02;
		Update_DebugStage_Console();
	}

	else if (g_pInput_Device->Key_Down(DIK_5))
	{
		m_eSceneChange = Stage_03;
		Update_DebugStage_Console();
	}

	else if (g_pInput_Device->Key_Down(DIK_6))
	{
		m_bLoadStaticMesh = (m_bLoadStaticMesh ? false : true);

		Update_DebugStage_Console();
	}
}
