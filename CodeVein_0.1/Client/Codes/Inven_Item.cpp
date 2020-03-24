#include "stdafx.h"
#include "..\Headers\Inven_Item.h"


CInven_Item::CInven_Item(_Device pDevice)
	: CUI(pDevice)
{
}

CInven_Item::CInven_Item(const CInven_Item & rhs)
	: CUI(rhs)
{
}

HRESULT CInven_Item::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	
	return NOERROR;
}

HRESULT CInven_Item::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_fPosX = WINCX * 0.5f;
	m_fPosY = WINCY * 0.5f;
	m_fSizeX = WINCX;
	m_fSizeY = WINCY;

	SetUp_Slot(CItem::CONSUME);

	return NOERROR;
}

_int CInven_Item::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	/*if (GetAsyncKeyState(VK_MULTIPLY) & 0x8000)
		Add_Item_ToInven(CItem::CONSUME, 0, L"GameObject_ConsumeItem", L"Layer_ConsumeItem");
	cout << m_SlotList[CItem::CONSUME].front()->Get_SlotSize() << endl;*/
	return NO_EVENT;
}

_int CInven_Item::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CInven_Item::Render_GameObject()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	m_matOldView = pManagement->Get_Transform(D3DTS_VIEW);
	m_matOldProj = pManagement->Get_Transform(D3DTS_PROJECTION);

	pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);


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

	pManagement->Set_Transform(D3DTS_VIEW, m_matOldView);
	pManagement->Set_Transform(D3DTS_PROJECTION, m_matOldProj);

	Safe_Release(pManagement);

	return NOERROR;
}

HRESULT CInven_Item::Add_Component()
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

HRESULT CInven_Item::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))

		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 0)))
		return E_FAIL;

	return NOERROR;
}

void CInven_Item::SetUp_Slot(CItem::ITEM_TYPE eType, void* pArg)
{
	CItemSlot* pSlot = nullptr;


	LOOP(5)
	{
		g_pManagement->Add_GameObject_ToLayer(L"GameObject_ItemSlot", SCENE_STAGE, L"Layer_ItemSlot");
		pSlot = static_cast<CItemSlot*>(g_pManagement->Get_GameObjectBack(L"Layer_ItemSlot", SCENE_STAGE));
		
		m_SlotList[eType].push_back(pSlot);
	}
}

void CInven_Item::Add_Item_ToInven(CItem::ITEM_TYPE eType, _uint iItemNum, const _tchar * pPrototypeTag, const _tchar* pLayerTag, void * pArg)
{
	/*CItem* pItem = nullptr;
	g_pManagement->Add_GameObject_ToLayer(pPrototypeTag, SCENE_STAGE, pLayerTag, pArg);
	pItem = static_cast<CItem*>(g_pManagement->Get_GameObjectBack(pLayerTag, SCENE_STAGE));*/

	m_SlotList[eType].front()->Add_Item(pPrototypeTag);
}

CInven_Item * CInven_Item::Create(_Device pGraphic_Device)
{
	CInven_Item* pInstance = new CInven_Item(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("CInven_Item Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CInven_Item::Clone_GameObject(void * pArg)
{
	CInven_Item* pInstance = new CInven_Item(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("CInven_Item Cloned Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInven_Item::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	for (_uint i = 0; i < CItem::ITEM_END; ++i)
	{
		for (auto& pSlot : m_SlotList[i])
		{
			Safe_Release(pSlot);
		}
		m_SlotList[i].clear();
	}

	CUI::Free();
}
