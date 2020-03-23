#include "stdafx.h"
#include "..\Headers\RightArrow.h"

#include "MenuBaseUI.h"

CRightArrow::CRightArrow(_Device pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CRightArrow::CRightArrow(const CRightArrow & rhs)
	: CUI(rhs)
{
}

HRESULT CRightArrow::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CRightArrow::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_fPosX = 380.f;
	m_fPosY = 500.f;

	m_fSizeX = 25.f;
	m_fSizeY = 30.f;

	return NOERROR;
}

_int CRightArrow::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	//Click_RightArrow();

	Update_UIPos();

	return NO_EVENT;
}

_int CRightArrow::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CRightArrow::Render_GameObject()
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

HRESULT CRightArrow::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Texture_Arrow", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRightArrow::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))

		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 1)))
		return E_FAIL;

	return NOERROR;
}

void CRightArrow::Click_RightArrow()
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;

	Safe_AddRef(pManagement);

	CItemSlot* pItemSlot = static_cast<CItemSlot*>(pManagement->Get_GameObjectBack(L"Layer_ItemSlot", SCENE_STAGE));
	if (nullptr == pItemSlot)
		return;

	if (CInput_Device::Get_Instance()->MousePt_InRect(m_fPosX, m_fPosY, m_fSizeX, m_fSizeY, g_hWnd)
		&& CInput_Device::Get_Instance()->Get_DIMouseState(CInput_Device::DIM_LB))
	{		
		CItem_QuickSlot* pItemQuickSlot = static_cast<CItem_QuickSlot*>(pManagement->Get_GameObjectBack(L"Layer_ItemQuickSlot", SCENE_STAGE));
		CMenuBaseUI* pMenuBase = static_cast<CMenuBaseUI*>(pManagement->Get_GameObjectBack(L"Layer_MenuBase", SCENE_STAGE));
		if (pItemQuickSlot->Get_Index() < pMenuBase->Get_SlotCnt() - 1)
			pItemQuickSlot->Set_Index(pItemQuickSlot->Get_Index() + 1);
		else
			pItemQuickSlot->Set_Index(0);
	}

	Safe_Release(pManagement);
}

void CRightArrow::Update_UIPos()
{
	if (m_bIsOpen)
	{
		m_fPosX = 380.f;
		m_fPosY = 500.f;
	}
	else
	{
		m_fPosX = -WINCX + 380.f;
		m_fPosY = 500.f;
	}
}

CRightArrow * CRightArrow::Create(_Device pGraphic_Device)
{
	CRightArrow* pInstance = new CRightArrow(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("CRightArrow Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRightArrow::Clone_GameObject(void * pArg)
{
	CRightArrow* pInstance = new CRightArrow(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("CRightArrow Cloned Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRightArrow::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	CUI::Free();
}
