#include "stdafx.h"
#include "..\Headers\Material_Inven.h"

#include "Item_Manager.h"

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

	m_fPosX = WINCX * 0.75f;
	m_fPosY = WINCY * 0.5f;
	m_fSizeX = WINCX * 0.5f;
	m_fSizeY = WINCY;

	m_fViewZ = 2.f;

	m_bIsActive = false;

	// Slot Create
	CUI::UI_DESC* pDesc = nullptr;
	CMaterial_Slot* pSlot = nullptr;
	for (_uint i = 0; i < 2; ++i)
	{
		for (_uint j = 0; j < 5; ++j)
		{
			pDesc = new CUI::UI_DESC;
			pDesc->fPosX = m_fPosX - 180.f + 90.f * j;
			pDesc->fPosY = m_fPosY - 100.f + 90.f * i;
			pDesc->fSizeX = 80.f;
			pDesc->fSizeY = 80.f;
			pDesc->iIndex = 0;
			g_pManagement->Add_GameObject_ToLayer(L"GameObject_MaterialSlot", SCENE_STAGE, L"Layer_MaterialSlot", pDesc);
			pSlot = static_cast<CMaterial_Slot*>(g_pManagement->Get_GameObjectBack(L"Layer_MaterialSlot", SCENE_STAGE));
			m_vecMaterialSlot.push_back(pSlot);
		}

	}


	return NOERROR;
}

_int CMaterial_Inven::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	if (m_bIsDead)
		return DEAD_OBJ;
	if (g_pInput_Device->Key_Up(DIK_2))
		m_bIsActive = !m_bIsActive;

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);
	
	for (auto& pSlot : m_vecMaterialSlot)
		pSlot->Set_Active(m_bIsActive);

	if (g_pInput_Device->Key_Up(DIK_8))
		Add_Material(CMaterial::MATERIAL_1);
	if (g_pInput_Device->Key_Up(DIK_9))
		Sell_Material(3);
	cout << m_vecMaterialSlot[0]->Get_Size() << endl;

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
	m_matWorld._42 = 1.f;

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

	m_pShaderCom->Begin_Pass(0);

	// ���۸� �������Ѵ�.
	// (�ε�������(012023)�� �����ϰ��ִ� �ε����� ���� ������ �׸���.)
	// �ﰢ�� �ΰ��� �׸���.������ �ﰢ������ ��������, ������ ������ ���ؽ� ���̴��� ���ڷ� ������.
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
	if (iIndex > m_vecMaterialSlot.size())
		return;

	if (m_vecMaterialSlot[iIndex]->Get_Type() == pMaterial->Get_Type() ||
		m_vecMaterialSlot[iIndex]->Get_Size() == 0)
		m_vecMaterialSlot[iIndex]->Input_Item(pMaterial);
	else
		Load_Materials(pMaterial, iIndex + 1);
}

void CMaterial_Inven::Add_Material(CMaterial::MATERIAL_TYPE eType)
{
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_Material", SCENE_STAGE, L"Layer_Material");
	CMaterial* pMaterial = static_cast<CMaterial*>(g_pManagement->Get_GameObjectBack(L"Layer_Material", SCENE_STAGE));
	pMaterial->Set_Type(eType);
	
	Load_Materials(pMaterial, 0);
}

void CMaterial_Inven::Sell_Material(_uint iDelete)
{
	for (auto& pSlot : m_vecMaterialSlot)
	{
		if (pSlot->Pt_InRect())
		{
			for (_uint i = 0; i < iDelete; ++i)
				pSlot->Delete_Item();
			return;
		}
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
