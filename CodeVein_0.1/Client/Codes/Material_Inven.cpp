#include "stdafx.h"
#include "..\Headers\Material_Inven.h"

#include "Item_Manager.h"
#include "UI_Manager.h"

CMaterial_Inven::CMaterial_Inven(_Device pDevice)
	: CUI(pDevice)
{
}

CMaterial_Inven::CMaterial_Inven(const CMaterial_Inven & rhs)
	: CUI(rhs)
{
}

HRESULT CMaterial_Inven::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CMaterial_Inven::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fPosX = 229.5f;
	m_fPosY = 325.5f;
	m_fSizeX = 280.f;
	m_fSizeY = 471.f;
	m_fViewZ = 4.f;

	m_bIsActive = false;

	SetUp_Default();

	// Slot Create
	CUI::UI_DESC* pDesc = nullptr;
	CMaterial_Slot* pSlot = nullptr;
	for (_uint i = 0; i < 6; ++i)
	{
		for (_uint j = 0; j < 5; ++j)
		{
			pDesc = new CUI::UI_DESC;

			pDesc->fPosX = m_fPosX - 103.f + 52.f * j;
			pDesc->fPosY = m_fPosY - 140.f + 52.f * i;
			pDesc->fSizeX = 50.f;
			pDesc->fSizeY = 50.f;
			g_pManagement->Add_GameObject_ToLayer(L"GameObject_MaterialSlot", SCENE_MORTAL, L"Layer_PlayerUI", pDesc);
			pSlot = static_cast<CMaterial_Slot*>(g_pManagement->Get_GameObjectBack(L"Layer_PlayerUI", SCENE_MORTAL));
			pSlot->Set_ViewZ(m_fViewZ - 0.1f);
			m_vecMaterialSlot.push_back(pSlot);
			m_vecUI_DESC.push_back(pDesc);
		}

	}
	
	Add_MultiMaterial(CMaterial::Queen_Steel, 8);
	Add_MultiMaterial(CMaterial::Queen_Titanium, 9);
	Add_MultiMaterial(CMaterial::Queen_Tungsten, 10);
	return NOERROR;
}

_int CMaterial_Inven::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);


	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);

	Click_Inven();

	for (auto& pSlot : m_vecMaterialSlot)
		pSlot->Set_Active(m_bIsActive);
	
	if (m_pExplainUI)
		m_pExplainUI->Set_Active(m_bIsActive);
	
	return NO_EVENT;
}

_int CMaterial_Inven::Late_Update_GameObject(_double TimeDelta)
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);

	m_matWorld._11 = m_fSizeX;
	m_matWorld._22 = m_fSizeY;
	m_matWorld._33 = 1.f;
	m_matWorld._41 = m_fPosX - WINCX * 0.5f;
	m_matWorld._42 = -m_fPosY + WINCY * 0.5f;

	return NO_EVENT;
}

HRESULT CMaterial_Inven::Render_GameObject()
{
	if (!m_bIsActive)
		return NOERROR;

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

HRESULT CMaterial_Inven::Add_Component()
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

HRESULT CMaterial_Inven::SetUp_ConstantTable()
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

void CMaterial_Inven::Load_Materials(CMaterial * pMaterial, _uint iIndex)
{
	/*if (iIndex > m_vecMaterialSlot.size())
		return;

	if ((m_vecMaterialSlot[iIndex]->Get_Type() == pMaterial->Get_Type() ||
		m_vecMaterialSlot[iIndex]->Get_Size() == 0) && m_vecMaterialSlot[iIndex]->Get_Size() < 9)
		m_vecMaterialSlot[iIndex]->Input_Item(pMaterial);
	else
		Load_Materials(pMaterial, iIndex + 1);*/

	

	if (nullptr == pMaterial)
		return;
	if (m_vecMaterialSlot.size() <= iIndex)
		return;

	if ((m_vecMaterialSlot[iIndex]->Get_Type() == pMaterial->Get_Type() ||
		m_vecMaterialSlot[iIndex]->Get_Size() == 0) && m_vecMaterialSlot[iIndex]->Get_Size() < 5)
		m_vecMaterialSlot[iIndex]->Input_Item(pMaterial);
	else
		Load_Materials(pMaterial, iIndex + 1);
}

void CMaterial_Inven::Click_Inven()
{
	if (!m_bIsActive)
		return;

	for (auto& pSlot : m_vecMaterialSlot)
	{
		if (pSlot->Pt_InRect() && pSlot->Get_Type() != CMaterial::MATERIAL_END)
		{
			m_pExplainUI->Set_Type(CMaterial::MATERIAL_TYPE(pSlot->Get_Type()));
			m_pExplainUI->Set_CurHaveCnt(pSlot->Get_Size());
			m_pExplainUI->Set_MaximumCnt(5);

			if (!pSlot->Get_Select() && pSlot->Get_Size() > 0 && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
			{
				// 마우스 왼쪽 버튼 눌렀을 때
				
			}
			else if (pSlot->Get_Select() && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_RB))
			{
				// 마우스 오른쪽 버튼 눌렀을 때

			}
		}
	}
}

void CMaterial_Inven::SetUp_Default()
{
	m_pExplainUI = static_cast<CExplainMaterialUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ExplainMaterialUI", nullptr));
	m_pExplainUI->Set_UI_Pos(WINCX * 0.5f, WINCY * 0.5f);
	m_pExplainUI->Set_UI_Size(1024.f * 0.6f, 720.f * 0.6f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pExplainUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
}

void CMaterial_Inven::Add_Material(CMaterial::MATERIAL_TYPE eType)
{
	/*g_pManagement->Add_GameObject_ToLayer(L"GameObject_Material", SCENE_STAGE, L"Layer_Material");
	CMaterial* pMaterial = static_cast<CMaterial*>(g_pManagement->Get_GameObjectBack(L"Layer_Material", SCENE_STAGE));
	pMaterial->Set_Type(eType);
	
	Load_Materials(pMaterial, 0);*/

	g_pManagement->Add_GameObject_ToLayer(L"GameObject_Material", SCENE_STAGE, L"Layer_Material");
	CMaterial* pMaterial = static_cast<CMaterial*>(g_pManagement->Get_GameObjectBack(L"Layer_Material", SCENE_STAGE));
	if (nullptr == pMaterial)
	{
		MSG_BOX("pMaterial is null");
		return;
	}
	pMaterial->Set_Type(eType);

	Load_Materials(pMaterial, 0);
}



void CMaterial_Inven::Add_MultiMaterial(CMaterial::MATERIAL_TYPE eType, _uint iCnt)
{
	LOOP(_int(iCnt))
		Add_Material(eType);
}

void CMaterial_Inven::Sell_Material(_uint iDelete)
{
	_ulong idx = 0;
	for (auto& pSlot : m_vecMaterialSlot)
	{
		if (pSlot->Pt_InRect())
		{
			// 삭제하려는 수가 소지 아이템 수보다 많을 경우 예외처리
			if (iDelete > pSlot->Get_Size())
				return;
			for (_uint i = 0; i < iDelete; ++i)
			{
				pSlot->Delete_Item();
			}
			
			if (pSlot->Get_Size() == 0)
			{
				m_vecMaterialSlot.erase(m_vecMaterialSlot.begin() + idx);
				m_vecMaterialSlot.shrink_to_fit();
				break;
			}
		}
		
		++idx;
	}
	
	
	_ulong Idx = 0;
	for (auto& pSlot : m_vecMaterialSlot)
	{
		
		pSlot->Set_UI_Pos(m_vecUI_DESC[Idx]->fPosX, m_vecUI_DESC[Idx]->fPosY);
		++Idx;
	}
}

CMaterial_Inven * CMaterial_Inven::Create(_Device pGraphic_Device)
{
	CMaterial_Inven* pInstance = new CMaterial_Inven(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CMaterial_Inven::Clone_GameObject(void * pArg)
{
	CMaterial_Inven* pInstance = new CMaterial_Inven(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CMaterial_Inven::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	CUI::Free();
}
