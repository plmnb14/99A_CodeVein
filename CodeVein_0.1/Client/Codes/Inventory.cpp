#include "stdafx.h"
#include "..\Headers\Inventory.h"

#include "UI_Manager.h"
#include "Inven_Icon.h"

#include "Tab_Consume.h"
#include "QuickSlot.h"
#include "Item_Slot.h"

CInventory::CInventory(_Device pDevice)
	: CUI(pDevice)
{
	ZeroMemory(&m_vOldPos, sizeof(_v2));
}

CInventory::CInventory(const CInventory & rhs)
	: CUI(rhs)
{
	ZeroMemory(&m_vOldPos, sizeof(_v2));
}

HRESULT CInventory::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CInventory::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_fPosX = 400.f;
	m_fPosY = WINCY * 0.5f;
	m_fSizeX = 500.f;
	m_fSizeY = 600.f;

	m_vOldPos = _v2(m_fPosX, m_fPosY);

	if (FAILED(SetUp_Icon()))
		return E_FAIL;

	if (FAILED(SetUp_Default()))
		return E_FAIL;

	return NOERROR;
}

_int CInventory::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	
	/*if (g_pInput_Device->Key_Up(DIK_ESCAPE))
		return DEAD_OBJ;*/
	if (m_bIsDead)
		return DEAD_OBJ;
	
	if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
		Click_Icon();
	
	/*if (m_bIsMove)
		Move_Inven(TimeDelta);*/
	
	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);

	
	for (_uint i = 0; i < _uint(m_vecRegist.size()); ++i)
	{
		m_vecRegist[i]->Set_UI_Pos(200.f, 400.f);
		m_vecRegist[i]->Set_UI_Size(50.f, 50.f);
	}
	
	
	return NO_EVENT;
}

_int CInventory::Late_Update_GameObject(_double TimeDelta)
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);

	m_matWorld._11 = m_fSizeX;
	m_matWorld._22 = m_fSizeY;
	m_matWorld._33 = 1.f;
	m_matWorld._41 = m_fPosX - WINCX * 0.5f;
	m_matWorld._42 = -m_fPosY + WINCY * 0.5f;
	m_matWorld._42 = 1.f;

	
	m_fViewZ = 1.f;


	m_pConsumeTab->Get_Slot(0);

	return NO_EVENT;
}

HRESULT CInventory::Render_GameObject()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	
	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	m_matOldView = g_pManagement->Get_Transform(D3DTS_VIEW);
	m_matOldProj = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);


	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();

	m_pShaderCom->Begin_Pass(1);

	// 버퍼를 렌더링한다.
	// (인덱스버퍼(012023)에 보관하고있는 인덱스를 가진 정점을 그리낟.)
	// 삼각형 두개를 그리낟.각각의 삼각형마다 정점세개, 각각의 정점을 버텍스 셰이더의 인자로 던진다.
	m_pBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matOldView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matOldProj);

	

	return NOERROR;
}

HRESULT CInventory::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_MenuWindow", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CInventory::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))

		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 2)))
		return E_FAIL;

	return NOERROR;
}

void CInventory::Move_Inven(_double TimeDelta)
{
	m_fPosX += (_float)TimeDelta * 20.f;

	// 아이콘 인벤 따라서 움직임
	for (_uint i = 0; i < _uint(m_vecIcon.size()); ++i)
	{
		m_vecIcon[i]->Set_UI_Pos(m_fPosX - 130.f + i * 70.f, m_fPosY - 250.f);
	}

	_v2 vLength = m_vOldPos - _v2(m_fPosX, m_fPosY);

	if (D3DXVec2Length(&vLength) >= 200.f)
		m_bIsMove = false;
}

void CInventory::Click_Icon()
{
	for (auto& pIcon : m_vecIcon)
	{
		if (pIcon->Coll_Mouse())
		{
			switch (pIcon->Get_IconType())
			{
			case CInven_Icon::ICON_STATUS:
				CUI_Manager::Get_Instance()->Open_Status();
				break;
			case CInven_Icon::ICON_ITEM:
				CUI_Manager::Get_Instance()->Open_Item();
				break;
			case CInven_Icon::ICON_SKILL:
				CUI_Manager::Get_Instance()->Open_Skill();
				break;
			case CInven_Icon::ICON_EXIT:
				m_bIsDead = true;
				break;
			}
		}
	}
}


HRESULT CInventory::SetUp_Default()
{
	CUI::UI_DESC* pDesc = nullptr;
	
	// 소모품탭 생성
	pDesc = new CUI::UI_DESC;
	pDesc->fPosX = 1000.f;
	pDesc->fPosY = WINCY * 0.5f;
	pDesc->fSizeX = 400.f;
	pDesc->fSizeY = 600.f;
	pDesc->iIndex = 0;
	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_TabConsume", SCENE_STAGE, L"Layer_TabConsume", pDesc)))
		return E_FAIL;
	m_pConsumeTab = static_cast<CTab_Consume*>(g_pManagement->Get_GameObjectBack(L"Layer_TabConsume", SCENE_STAGE));

	// 퀵슬롯 생성
	pDesc = new CUI::UI_DESC;
	pDesc->fPosX = 100.f;
	pDesc->fPosY = 600.f;
	pDesc->fSizeX = 100.f;
	pDesc->fSizeY = 100.f;
	pDesc->iIndex = 0;
	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_QuickSlot", SCENE_STAGE, L"Layer_QuickSlot", pDesc)))
		return E_FAIL;
	m_pQuickSlot = static_cast<CQuickSlot*>(g_pManagement->Get_GameObjectBack(L"Layer_QuickSlot", SCENE_STAGE));
	// 재료탭 생성

	// 아장탭 생성

	// 무기탭 생성
	return NOERROR;
}

HRESULT CInventory::SetUp_Icon()
{
	UI_DESC* pDesc = nullptr;
	
	
	LOOP(5)
	{
		pDesc = new UI_DESC;
		pDesc->fPosX = m_fPosX - 130.f + i * 70.f;
		pDesc->fPosY = m_fPosY - 250.f;
		pDesc->fSizeX = 50.f;
		pDesc->fSizeY = 50.f;

		if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_InvenIcon", SCENE_STAGE, L"Layer_InvenIcon", pDesc)))
			return E_FAIL;

		CInven_Icon* pIcon = static_cast<CInven_Icon*>(g_pManagement->Get_GameObjectBack(L"Layer_InvenIcon", SCENE_STAGE));
		pIcon->Set_IconType(CInven_Icon::ICON_TYPE(i));

		m_vecIcon.push_back(pIcon);		
	}
	

	return NOERROR;
}

void CInventory::Add_ToQuickSlot(CItem_Slot* pSlot)
{
	//m_vecRegist.push_back(pSlot);
}

CInventory * CInventory::Create(_Device pGraphic_Device)
{
	CInventory* pInstance = new CInventory(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CInventory::Clone_GameObject(void * pArg)
{
	CInventory* pInstance = new CInventory(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("CInventory Cloned Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInventory::Free()
{
	for (auto& pIcon : m_vecIcon)
		pIcon->Set_Dead();
	m_vecIcon.clear();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	
	
	CUI::Free();
}
