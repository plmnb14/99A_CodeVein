#include "stdafx.h"
#include "PickUp_ItemUIManager.h"
#include "UI_Manager.h"

#include "PickUp_ItemUI.h"
#include "DropItem.h"

CPickUp_ItemUIManager::CPickUp_ItemUIManager(_Device Graphic_Device)
	: CUI(Graphic_Device)
{
}

CPickUp_ItemUIManager::CPickUp_ItemUIManager(const CPickUp_ItemUIManager & rhs)
	: CUI(rhs)
{
}

HRESULT CPickUp_ItemUIManager::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CPickUp_ItemUIManager::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_fSizeX = 298.f;
	m_fSizeY = 38.f;

	m_fPosX = 1100.f;
	m_fPosY = 200.f;

	return S_OK;
}

_int CPickUp_ItemUIManager::Update_GameObject(_double TimeDelta)
{
	CGameObject::LateInit_GameObject();
	CUI::Update_GameObject(TimeDelta);

	SetUp_State(TimeDelta);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	for (auto& pPickUP : m_vecPickUp_Item)
		pPickUP->Set_Active(m_bIsActive);

	_uint iIndex = 0;


	auto& iter_begin = m_vecPickUp_Item.begin();
	auto& iter_end = m_vecPickUp_Item.end();

	for (; iter_begin != iter_end;)
	{
		//(*iter_begin)->Set_UI_Pos(m_fPosX, m_fPosY + 50.f * iIndex);
		iIndex++;

		if (false == (*iter_begin)->Get_Active())
		{
			iter_begin = m_vecPickUp_Item.erase(iter_begin);
			//(*iter_begin)->Set_UI_Pos(1100.f, 200.f);
		}
			
		else
			iter_begin++;
	}

	return NO_EVENT;
}

_int CPickUp_ItemUIManager::Late_Update_GameObject(_double TimeDelta)
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);

	m_matWorld._11 = m_fSizeX;
	m_matWorld._22 = m_fSizeY;
	m_matWorld._33 = 1.f;
	m_matWorld._41 = m_fPosX - WINCX * 0.5f;
	m_matWorld._42 = -m_fPosY + WINCY * 0.5f;

	return S_OK;
}

HRESULT CPickUp_ItemUIManager::LateInit_GameObject()
{
	return S_OK;
}

HRESULT CPickUp_ItemUIManager::Render_GameObject()
{
	if (nullptr == m_pShaderCom || nullptr == m_pBufferCom)
		return E_FAIL;

	if (false == m_bIsActive)
		return NOERROR;

	CUI_Manager* pUIManager = CUI_Manager::Get_Instance();
	CItem_Manager* pItemManager = CItem_Manager::Get_Instance();

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	//m_pShaderCom->Begin_Shader();

	//m_pShaderCom->Begin_Pass(3);

	//m_pShaderCom->Commit_Changes();
	//m_pBufferCom->Render_VIBuffer();

	//m_pShaderCom->End_Pass();

	//m_pShaderCom->End_Shader();

	//_uint iIndex = 0;

	//for (_uint i = 0; i < m_iCount_GetItem; ++i)
	//{
	//	if (0 == i)
	//		iIndex = 0;
	//	else
	//		iIndex = m_iCount_GetItem;

	//	if (FAILED(SetUp_ConstantTable(pItemManager->Get_PickUp_Number())))
	//		return E_FAIL;

	//	m_pShaderCom->Begin_Shader();

	//	m_pShaderCom->Begin_Pass(3);

	//	m_pShaderCom->Commit_Changes();
	//	m_pBufferCom->Render_VIBuffer();

	//	m_pShaderCom->End_Pass();

	//	m_pShaderCom->End_Shader();
	//}

	return S_OK;
}

void CPickUp_ItemUIManager::Set_vecPickUp_Item(CPickUp_ItemUI * _vecPickUP)
{
	m_vecPickUp_Item.push_back(_vecPickUP);

	return;
}

HRESULT CPickUp_ItemUIManager::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_PickUp_Item", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPickUp_ItemUIManager::SetUp_ConstantTable(_uint TextureIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_fPosX", &m_fPosX, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_fSizeX", &m_fSizeX, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, TextureIndex)))
		return E_FAIL;

	return S_OK;
}

void CPickUp_ItemUIManager::SetUp_State(_double TimeDelta)
{
}

void CPickUp_ItemUIManager::SetUp_Rendering_ItemTextrue()
{
}

CPickUp_ItemUIManager * CPickUp_ItemUIManager::Create(_Device pGraphic_Device)
{
	CPickUp_ItemUIManager* pInstance = new CPickUp_ItemUIManager(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CPickUp_ItemUIManager::Clone_GameObject(void * pArg)
{
	CPickUp_ItemUIManager* pInstance = new CPickUp_ItemUIManager(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CPickUp_ItemUIManager::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
