#include "stdafx.h"
#include "..\Headers\SubQuickSlot.h"




CSubQuickSlot::CSubQuickSlot(_Device pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CSubQuickSlot::CSubQuickSlot(const CSubQuickSlot & rhs)
	: CUI(rhs)
{
}

void CSubQuickSlot::Set_Index(_bool bIsLeft)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;
	Safe_AddRef(pManagement);

	m_iIndex = static_cast<CItem_QuickSlot*>(pManagement->Get_GameObjectBack(L"Layer_ItemQuickSlot", SCENE_STAGE))->Get_Index();
	m_iMaxIndex = static_cast<CMenuBaseUI*>(pManagement->Get_GameObjectBack(L"Layer_MenuBase", SCENE_STAGE))->Get_SlotCnt();

	if (bIsLeft == true)
	{
		if (m_iIndex > 0)
			m_eType = static_cast<CMenuBaseUI*>(pManagement->Get_GameObjectBack(L"Layer_MenuBase", SCENE_STAGE))->Get_SlotItemType(m_iIndex - 1);
		else
			m_eType = static_cast<CMenuBaseUI*>(pManagement->Get_GameObjectBack(L"Layer_MenuBase", SCENE_STAGE))->Get_SlotItemType(m_iMaxIndex - 1);
	}
	
	else
	{
		if(m_iIndex >= m_iMaxIndex - 1)
			m_eType = static_cast<CMenuBaseUI*>(pManagement->Get_GameObjectBack(L"Layer_MenuBase", SCENE_STAGE))->Get_SlotItemType(0);
		else
			m_eType = static_cast<CMenuBaseUI*>(pManagement->Get_GameObjectBack(L"Layer_MenuBase", SCENE_STAGE))->Get_SlotItemType(m_iIndex + 1);
	}

	

	Safe_Release(pManagement);
}

HRESULT CSubQuickSlot::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CSubQuickSlot::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_vOldPos.x = m_fPosX;
	m_vOldPos.y = m_fPosY;

	return NOERROR;
}

_int CSubQuickSlot::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);


	Update_UIPos();

	

	return NO_EVENT;
}

_int CSubQuickSlot::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CSubQuickSlot::Render_GameObject()
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

	// ���۸� �������Ѵ�.
	// (�ε�������(012023)�� �����ϰ��ִ� �ε����� ���� ������ �׸���.)
	// �ﰢ�� �ΰ��� �׸���.������ �ﰢ������ ��������, ������ ������ ���ؽ� ���̴��� ���ڷ� ������.
	m_pBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();

	pManagement->Set_Transform(D3DTS_VIEW, m_matOldView);
	pManagement->Set_Transform(D3DTS_PROJECTION, m_matOldProj);

	Safe_Release(pManagement);

	return NOERROR;
}

HRESULT CSubQuickSlot::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Texture_Item", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;

}

HRESULT CSubQuickSlot::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))

		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, m_eType)))
		return E_FAIL;

	return NOERROR;
}

void CSubQuickSlot::Update_UIPos()
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
}

CSubQuickSlot * CSubQuickSlot::Create(_Device pGraphic_Device)
{
	CSubQuickSlot* pInstance = new CSubQuickSlot(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("CSubQuickSlot Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSubQuickSlot::Clone_GameObject(void * pArg)
{
	CSubQuickSlot* pInstance = new CSubQuickSlot(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("CSubQuickSlot Cloned Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSubQuickSlot::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	CUI::Free();
}
