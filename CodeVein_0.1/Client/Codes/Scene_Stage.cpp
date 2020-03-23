#include "stdafx.h"
#include "..\Headers\Scene_Stage.h"
#include "Management.h"
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

	if (FAILED(Ready_Layer_Player(L"Layer_Player")))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Camera(L"Layer_Camera")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(L"Layer_Monster")))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Effect(L"Layer_Effect")))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(L"Layer_BackGround")))
		return E_FAIL;

	if (FAILED(CUI_Manager::Get_Instance()->SetUp_UILayer()))
		return E_FAIL;

	return S_OK;
}

_int CScene_Stage::Update_Scene(_double TimeDelta)
{
	// 임시
	//static _bool bCreate = false;
	static _float fStartPosY = 0.f;
	if (GetAsyncKeyState('Z') & 0x8000)
	{
		CManagement*		pManagement = CManagement::Get_Instance();
		if (nullptr == pManagement)
			return E_FAIL;
		
		Safe_AddRef(pManagement);

		LOOP(3)
		{
			CEffect::EFFECT_DESC* pEffDesc = new CEffect::EFFECT_DESC;
			pEffDesc->pTargetTrans = nullptr;
			pEffDesc->vWorldPos = {0.f, fStartPosY, 0.f};
			if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_EffectLineParticle", SCENE_STAGE, L"Layer_Effect", pEffDesc)))
				return E_FAIL;
		}
		
		Safe_Release(pManagement);
	//	bCreate = true;
		fStartPosY += 0.01f;
	}
	else
		fStartPosY = 0.f;
	
	

	// -------------- UI Manager ----------------------
	CUI_Manager::Get_Instance()->Update_UI();

	if (CInput_Device::Get_Instance()->Key_Up(DIK_NUMPAD3))
		CUI_Manager::Get_Instance()->Add_Item(CItem::REGEN_POWER);
	if (CInput_Device::Get_Instance()->Key_Up(DIK_NUMPAD4))
		CUI_Manager::Get_Instance()->Add_Item(CItem::ITEM_2);
	if (CInput_Device::Get_Instance()->Key_Up(DIK_NUMPAD5))
		CUI_Manager::Get_Instance()->Use_Item();
	return _int();
}

HRESULT CScene_Stage::Render_Scene()
{

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Player(const _tchar * pLayerTag)
{
	// 이미 오브젝트 매니져에 추가되어있는 객체를 찾아서 복제한다음. 
	// 적절한 레이어에 보관해라.

	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	// For.Terrain
	if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_Player", SCENE_STAGE, pLayerTag)))
		return E_FAIL;

	Safe_Release(pManagement);

	return S_OK;
}
HRESULT CScene_Stage::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	// 여기서 카메라 준비

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	// For. TestMonster
	//if (FAILED(pManagement->Add_GameObject_ToLayer(L"Monster_TestMonster", SCENE_STAGE, pLayerTag)))
	//	return E_FAIL;

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	// 이미 오브젝트 매니져에 추가되어있는 객체를 찾아서 복제한다음. 
	// 적절한 레이어에 보관해라.

	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	// For.Terrain
	//if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_Terrain", SCENE_STAGE, pLayerTag)))
	//	return E_FAIL;

	// For.Sky
	if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_Sky", SCENE_STAGE, pLayerTag)))
		return E_FAIL;

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	// For.Effect


	Safe_Release(pManagement);


	return S_OK;
}

HRESULT CScene_Stage::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	

	CUI_Manager::Get_Instance()->SetUp_UILayer();
	

	Safe_Release(pManagement);

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_ItemSlot(const _tchar * pLayerTag)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	

	Safe_Release(pManagement);

	return NOERROR;
}

HRESULT CScene_Stage::Ready_LightDesc()
{
	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	

	D3DLIGHT9		LightDesc;
	ZeroMemory(&LightDesc, sizeof(D3DLIGHT9));

	// Direction
	//LightDesc.Type = D3DLIGHT_DIRECTIONAL;
	//LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//LightDesc.Ambient = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.f);
	//LightDesc.Specular = LightDesc.Diffuse;
	//// In.WorldSpace
	//LightDesc.Direction = _v3(1.f, -1.f, 1.f);

	//if (FAILED(pManagement->Add_Light(m_pGraphic_Device, LightDesc)))
	//	return E_FAIL;

	// Point
	LightDesc.Type = D3DLIGHT_POINT;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 0.0f, 0.0f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;	
	LightDesc.Position = _v3(5.f, 5.0f, 5.f);
	LightDesc.Range = 10.0f;

	if (FAILED(pManagement->Add_Light(m_pGraphic_Device, LightDesc)))
		return E_FAIL;

	// Point
	LightDesc.Type = D3DLIGHT_POINT;
	LightDesc.Diffuse = D3DXCOLOR(0.0f, 1.f, 0.0f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;
	LightDesc.Position = _v3(15.f, 5.0f, 5.f);
	LightDesc.Range = 10.0f;

	if (FAILED(pManagement->Add_Light(m_pGraphic_Device, LightDesc)))
		return E_FAIL;


	Safe_Release(pManagement);

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
	CUI_Manager::Get_Instance()->Destroy_Instance();
	CScene::Free();
}

