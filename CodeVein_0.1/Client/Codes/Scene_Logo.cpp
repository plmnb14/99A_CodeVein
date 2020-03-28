#include "stdafx.h"
#include "..\Headers\Scene_Logo.h"
#include "Scene_Stage.h"
#include "BackGround.h"
#include "Management.h"
#include "CameraMgr.h"
#include "LogoBtn.h"

#include "UI_Manager.h"

CScene_Logo::CScene_Logo(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CScene(pGraphic_Device)
	, m_pLoading(nullptr)
{

}

HRESULT CScene_Logo::Ready_Scene()
{
	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Layer_LogoBtn(L"Layer_LogoBtn")))
		return E_FAIL;

	m_pLoading = CLoading::Create(m_pGraphic_Device, SCENE_STAGE);
	if (nullptr == m_pLoading)
		return E_FAIL;

	return S_OK;
}

_int CScene_Logo::Update_Scene(_double TimeDelta)
{
	_bool Coll_ToButton = static_cast<CLogoBtn*>(g_pManagement->Get_GameObjectBack(L"Layer_LogoBtn", SCENE_LOGO))->Get_CollMose();

	if(true == m_pLoading->Get_Finish() && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		if (false == Coll_ToButton)
			return 0;

		if (FAILED(g_pManagement->SetUp_CurrentScene(CScene_Stage::Create(m_pGraphic_Device))))
			return -1;

		if (FAILED(g_pManagement->Clear_Instance(SCENE_LOGO)))
			return -1;

		return 0;
	}
	
	
	return _int();
}

HRESULT CScene_Logo::Render_Scene()
{
	
	return S_OK;
}

HRESULT CScene_Logo::Ready_Prototype_GameObject()
{
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_LogoBtn", CLogoBtn::Create(m_pGraphic_Device))))
		return E_FAIL;

	CCameraMgr::Get_Instance()->Reserve_ContainerSize(2);
	CCameraMgr::Get_Instance()->Ready_Camera(m_pGraphic_Device, DYNAMIC_CAM, L"Tool_FreeCam", TOOL_VIEW, DEFAULT_MODE);
	CCameraMgr::Get_Instance()->Set_MainCamera(DYNAMIC_CAM, L"Tool_FreeCam");
	CCameraMgr::Get_Instance()->Set_MainPos(_v3{ 0,3,-5 });
	
	return S_OK;
}

HRESULT CScene_Logo::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	return S_OK;
}

HRESULT CScene_Logo::Ready_Layer_LogoBtn(const _tchar * pLayerTag)
{
	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_LogoBtn", SCENE_LOGO, pLayerTag)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CScene_Logo::Temp_Stage_Loader(const _tchar * _DatPath)
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

