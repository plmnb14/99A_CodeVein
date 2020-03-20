#include "stdafx.h"
#include "..\Headers\MenuBaseUI.h"



CMenuBaseUI::CMenuBaseUI(_Device pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CMenuBaseUI::CMenuBaseUI(const CMenuBaseUI & rhs)
	: CUI(rhs)
{
}

_uint CMenuBaseUI::Get_ItemIndex(_uint iSlotIndex)
{
	if (m_iSlotCnt <= iSlotIndex)
		return 0;

	

	return m_vecItemSlot[iSlotIndex].front()->Get_Index();
}

_uint CMenuBaseUI::Get_ItemCount(_uint iSlotIndex)
{
	_uint iCnt = _uint(m_vecItemSlot[iSlotIndex].size());
	return iCnt;
}

HRESULT CMenuBaseUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CMenuBaseUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_vecMenuBtn.reserve(9);
	

	m_fPosX = WINCX - 300.f;
	m_fPosY = WINCY * 0.5f;
	m_fSizeX = WINCX * 0.5f;
	m_fSizeY = WINCY;

	return NOERROR;
}

_int CMenuBaseUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	if (CInput_Device::Get_Instance()->Key_Up(DIK_C))
		Add_ItemIcon(L"GameObject_ItemIcon", SCENE_STAGE, L"Layer_ItemIcon", 1);
	if (CInput_Device::Get_Instance()->Key_Up(DIK_V))
		Add_ItemIcon(L"GameObject_ItemIcon", SCENE_STAGE, L"Layer_ItemIcon", 2);
	if (CInput_Device::Get_Instance()->Key_Up(DIK_B))
		Add_ItemIcon(L"GameObject_ItemIcon", SCENE_STAGE, L"Layer_ItemIcon", 3);

	SetUp_WindowPosition();

	
	return NO_EVENT;
}

_int CMenuBaseUI::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CMenuBaseUI::Render_GameObject()
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


	m_pBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();


	pManagement->Set_Transform(D3DTS_VIEW, m_matOldView);
	pManagement->Set_Transform(D3DTS_PROJECTION, m_matOldProj);

	Safe_Release(pManagement);

	return NOERROR;
}

HRESULT CMenuBaseUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Texture_MenuBase", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CMenuBaseUI::SetUp_ConstantTable()
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

void CMenuBaseUI::SetUp_WindowPosition()
{
	if (CInput_Device::Get_Instance()->Key_Up(DIK_ESCAPE))
		m_bIsOpenWindow = !m_bIsOpenWindow;

	if (true == m_bIsOpenWindow)
	{
		m_fPosX = WINCX - 300.f;
		m_fPosY = WINCY * 0.5f;
	}
	else
	{
		m_fPosX = -WINCX * 0.5f;
		m_fPosY = -WINCY * 0.5f;
	}

	
	for(_uint i = 0; i < m_iSlotCnt; ++i)
	{
		for (auto& iter : m_vecItemSlot[i])
		{
			iter->Set_UI_Pos(m_fPosX - 125.f + _float(m_iSlotCnt) * 45.f, m_fPosY + 200.f);
		}
	}
	
}

HRESULT CMenuBaseUI::SetUp_MenuButton()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	CUI::UI_DESC* pDesc = new CUI::UI_DESC;
	pDesc->fPosX = 500.f;
	pDesc->fPosY = 600.f;
	pDesc->fSizeX = 100.f;
	pDesc->fSizeY = 100.f;
	pManagement->Add_GameObject_ToLayer(L"GameObject_MenuBtn", SCENE_STAGE, L"Layer_MenuBtn", pDesc);

	Safe_Release(pManagement);

	return NOERROR;
}

_bool CMenuBaseUI::Find_Item(_uint iItemIndex)
{
	for (auto& iter : m_vecItemSlot)
	{
		for (auto& pItem : iter)
		{
			if (iItemIndex == pItem->Get_Index())
				return true;
		}
	}
	return false;
}

void CMenuBaseUI::Add_ItemIcon(const _tchar * pPrototypeTag, _uint iSceneID, const _tchar * pLayerTag, _uint iItemIndex)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;
	Safe_AddRef(pManagement);

	if (Find_Item(iItemIndex))
	{
		//카운트 증가
		/*CItemIcon* pItemIcon = static_cast<CItemIcon*>(pManagement->Get_GameObjectBack(pLayerTag, iSceneID));
		pItemIcon->Set_ItemCnt(pItemIcon->Get_ItemCnt() + 1);*/
	}
	else
	{
		CUI::UI_DESC* pDesc = new CUI::UI_DESC;
		pDesc->fPosX = m_fPosX - 125.f + _float(m_iSlotCnt) * 45.f;
		pDesc->fPosY = m_fPosY + 200.f;
		pDesc->fSizeX = 45.f;
		pDesc->fSizeY = 45.f;
		pManagement->Add_GameObject_ToLayer(pPrototypeTag, iSceneID, pLayerTag, pDesc);

		CItemIcon* pItemIcon = static_cast<CItemIcon*>(pManagement->Get_GameObjectBack(pLayerTag, iSceneID));
		pItemIcon->Set_Index(iItemIndex);
		m_vecItemSlot[m_iSlotCnt].push_back(pItemIcon);
		
		++m_iSlotCnt;
	}
	
	Safe_Release(pManagement);
}

CMenuBaseUI * CMenuBaseUI::Create(_Device pGraphic_Device)
{
	CMenuBaseUI* pInstance = new CMenuBaseUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("CMenuBaseUI Creating Fail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMenuBaseUI::Clone_GameObject(void * pArg)
{
	CMenuBaseUI* pInstance = new CMenuBaseUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CMenuBaseUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMenuBaseUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
