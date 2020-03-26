#include "stdafx.h"
#include "..\Headers\Item_QuickSlot.h"

CItem_QuickSlot::CItem_QuickSlot(_Device pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CItem_QuickSlot::CItem_QuickSlot(const CItem_QuickSlot & rhs)
	: CUI(rhs)
{
}

HRESULT CItem_QuickSlot::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CItem_QuickSlot::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_fPosX = 300.f;
	m_fPosY = 500.f;

	m_vOldPos =_v2(m_fPosX, m_fPosY);
	
	m_fSizeX = 50.f;
	m_fSizeY = 50.f;

	SetUp_ItemCntUI();

	return NOERROR;
}

_int CItem_QuickSlot::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;
	Safe_AddRef(pManagement);
	
	m_eType = static_cast<CMenuBaseUI*>(pManagement->Get_GameObjectBack(L"Layer_MenuBase", SCENE_STAGE))->Get_SlotItemType(m_iIndex);
	m_iMaxIndex = static_cast<CMenuBaseUI*>(pManagement->Get_GameObjectBack(L"Layer_MenuBase", SCENE_STAGE))->Get_SlotCnt();


	Safe_Release(pManagement);

	/*if (m_iIndex >= m_iMaxIndex - 1)
		m_iIndex = m_iMaxIndex - 1;
	if (m_iIndex <= 0)
		m_iIndex = 0;*/
	
	Update_Count();

	if (CInput_Device::Get_Instance()->Key_Up(DIK_ADD))
	{
		if (m_iIndex >= m_iMaxIndex - 1)
			m_iIndex = 0;
		else
			++m_iIndex;
	}
		
	if (CInput_Device::Get_Instance()->Key_Up(DIK_SUBTRACT))
	{
		if (m_iIndex <= 0)
			m_iIndex = m_iMaxIndex - 1;
		else
			--m_iIndex;
	}

	Update_UIPos();
		
	return NO_EVENT;
}

_int CItem_QuickSlot::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CItem_QuickSlot::Render_GameObject()
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

HRESULT CItem_QuickSlot::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_ItemIcon", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CItem_QuickSlot::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))

		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, _uint(m_eType))))
		return E_FAIL;

	return NOERROR;
}

void CItem_QuickSlot::SetUp_ItemCntUI()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;
	Safe_AddRef(pManagement);

	CUI::UI_DESC* pDesc = new CUI::UI_DESC;
	pDesc->fPosX = m_fPosX;
	pDesc->fPosY = m_fPosY - 35.f;
	pDesc->fSizeX = 20.f;
	pDesc->fSizeY = 20.f;
	pManagement->Add_GameObject_ToLayer(L"GameObject_CntSlotUI", SCENE_STAGE, L"Layer_QuickSlotCnt", pDesc);

	Safe_Release(pManagement);
}

void CItem_QuickSlot::Update_Count()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;
	Safe_AddRef(pManagement);

	static_cast<CSlotCnt_UI*>(pManagement->Get_GameObjectBack(L"Layer_QuickSlotCnt", SCENE_STAGE))->Set_Count(
	static_cast<CMenuBaseUI*>(pManagement->Get_GameObjectBack(L"Layer_MenuBase", SCENE_STAGE))->Get_SlotSize(m_iIndex));

	Safe_Release(pManagement);
}

void CItem_QuickSlot::Update_UIPos()
{
	if (m_bIsOpen)
	{
		m_fPosX = m_vOldPos.x;
		m_fPosY = m_vOldPos.y;
	}
	else
	{
		m_fPosX = -WINCX + m_vOldPos.x;
		m_fPosY = m_vOldPos.y;
	}

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;
	Safe_AddRef(pManagement);

	static_cast<CSlotCnt_UI*>(pManagement->Get_GameObjectBack(L"Layer_QuickSlotCnt", SCENE_STAGE))->Set_UI_Pos(m_fPosX, m_fPosY + 35.f);

	Safe_Release(pManagement);
}

void CItem_QuickSlot::Use_Item()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;
	Safe_AddRef(pManagement);

	CMenuBaseUI* pMenu = static_cast<CMenuBaseUI*>(pManagement->Get_GameObjectBack(L"Layer_MenuBase", SCENE_STAGE));
	pMenu->Delete_Item(m_iIndex);

	Safe_Release(pManagement);
}


CItem_QuickSlot * CItem_QuickSlot::Create(_Device pGraphic_Device)
{
	CItem_QuickSlot* pInstance = new CItem_QuickSlot(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Item_QuickSlot Creating Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CItem_QuickSlot::Clone_GameObject(void * pArg)
{
	CItem_QuickSlot* pInstance = new CItem_QuickSlot(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("CItem_QuickSlot Cloned Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem_QuickSlot::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	CUI::Free();
}
