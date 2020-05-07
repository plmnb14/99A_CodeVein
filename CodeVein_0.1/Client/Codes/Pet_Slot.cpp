#include "stdafx.h"
#include "..\Headers\Pet_Slot.h"

CPet_Slot::CPet_Slot(_Device pDevice)
	:CUI(pDevice)
{
}

CPet_Slot::CPet_Slot(const CPet_Slot & rhs)
	:CUI(rhs)
{
}

HRESULT CPet_Slot::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CPet_Slot::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return DEAD_OBJ;

	CUI::Ready_GameObject(pArg);

	SetUp_Default();

	m_bIsActive = false;

	return S_OK;
}

_int CPet_Slot::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	if (true == m_bIsDead)
		return DEAD_OBJ;

	if (nullptr != m_pSelectUI)
	{
		m_pSelectUI->Set_UI_Pos(m_fPosX, m_fPosY);
		m_pSelectUI->Set_UI_Size(m_fSizeX, m_fSizeY);
		m_pSelectUI->Set_ViewZ(m_fViewZ - 0.1f);
		m_pSelectUI->Set_Active(m_bIsActive);
	}

	if (nullptr != m_pCursorUI)
	{
		m_pCursorUI->Set_UI_Pos(m_fPosX, m_fPosY);
		m_pCursorUI->Set_UI_Size(m_fSizeX, m_fSizeY);
		m_pCursorUI->Set_ViewZ(m_fViewZ - 0.2f);

		if (m_vecPet.size() > 0)
			m_pCursorUI->Set_Active(m_bIsActive);
		else
			m_pCursorUI->Set_Active(false);

		m_pCursorUI->Set_CursorColl(Pt_InRect());
	}

	if (nullptr != m_pNumberUI)
	{
		m_pNumberUI->Set_Active(m_bIsActive);

		if (m_vecPet.size() == 0)
			m_pNumberUI->Set_Active(false);

		m_pNumberUI->Set_UI_Index(_uint(m_vecPet.size()));
		m_pNumberUI->Set_UI_Pos(m_fPosX - m_fSizeX * 0.25f, m_fPosY + m_fSizeY * 0.25f);
	}

	m_pRenderer->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	if (m_vecPet.size() > 0)
		m_ePetType = m_vecPet.front()->Get_Type();
	else
		m_ePetType = CPet::PET_TYPE::PET_TYPE_END;


	if (m_vecPet.size() > 0)
		m_pSelectUI->Set_Select(m_bIsSelect);
	else
		m_pSelectUI->Set_Select(false);

	//강화수치= +숫자 붙이고
	//등급 테두리 색상
	//종류 아이콘
	switch (m_ePetType)
	{
	case CPet::PET_TYPE::PET_DEERKING:
		m_iIndex = 0;
		break;
	case CPet::PET_TYPE::PET_POISONBUTTERFLY:
		m_iIndex = 1;
		break;
	case CPet::PET_TYPE::PET_TYPE_END:
		m_iIndex = 2;
		break;
	}

	return NO_EVENT;
}

_int CPet_Slot::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CPet_Slot::Render_GameObject()
{
	if (!m_bIsActive)
		return S_OK;

	IF_NULL_VALUE_RETURN(m_pShader, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pBuffer, E_FAIL);

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	m_matOldView = g_pManagement->Get_Transform(D3DTS_VIEW);
	m_matOldProj = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);


	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShader->Begin_Shader();

	m_pShader->Begin_Pass(1);

	m_pBuffer->Render_VIBuffer();

	m_pShader->End_Pass();

	m_pShader->End_Shader();


	g_pManagement->Set_Transform(D3DTS_VIEW, m_matOldView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matOldProj);


	return S_OK;
}

_uint CPet_Slot::Get_Size()
{
	return _uint(m_vecPet.size());
}

_uint CPet_Slot::Get_Plus()
{
	if (m_vecPet.size() == 0)
		return CPet::PET_PLUS_TYPE::PET_PLUS_END;

	return _uint(m_vecPet.front()->Get_Plus());
}

_uint CPet_Slot::Get_Type()
{
	if (m_vecPet.size() == 0)
		return CPet::PET_TYPE::PET_TYPE_END;

	return _uint(m_vecPet.front()->Get_Type());
}

_uint CPet_Slot::Get_Grade()
{
	if (m_vecPet.size() == 0)
		return CPet::PET_TYPE::PET_TYPE_END;

	return _uint(m_vecPet.front()->Get_Type());
}

void CPet_Slot::Set_Select(_bool bIsSelect)
{
	m_bIsSelect = bIsSelect;

	return;
}

_bool CPet_Slot::Pt_InRect()
{
	return g_pInput_Device->MousePt_InRect(m_fPosX, m_fPosY, m_fSizeX, m_fSizeY, g_hWnd);
}

void CPet_Slot::Input_Item(CPet * pPet)
{
	IF_NULL_RETURN(pPet);

	m_vecPet.push_back(pPet);

	return;
}

void CPet_Slot::Delete_Item()
{
	if (m_vecPet.size() == 0)
		return;

	m_vecPet.pop_back();

	return;
}

void CPet_Slot::Delete_Items()
{
	for (_uint i = 0; i < m_vecPet.size(); ++i)
		m_vecPet.pop_back();

	return;
}

HRESULT CPet_Slot::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Texture //현재는 이렇지만 펫 아이콘으로 교체
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_Expendables", L"Com_Texture", (CComponent**)&m_pTexture)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBuffer)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPet_Slot::SetUp_ConstantTable()
{
	IF_NULL_VALUE_RETURN(m_pShader, E_FAIL);

	if (FAILED(m_pShader->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_matView", &m_matView, sizeof(_mat))))

		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTexture->SetUp_OnShader("g_DiffuseTexture", m_pShader, m_iIndex)))
		return E_FAIL;

	return S_OK;
}

void CPet_Slot::SetUp_Default()
{
	CUI::UI_DESC* pDesc = nullptr;

	pDesc = new CUI::UI_DESC;
	pDesc->fPosX = m_fPosX;
	pDesc->fPosY = m_fPosY;
	pDesc->fSizeX = m_fSizeX;
	pDesc->fSizeY = m_fSizeY;
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_SelectUI", SCENE_STAGE, L"Layer_SelectUI", pDesc);
	m_pSelectUI = static_cast<CSelect_UI*>(g_pManagement->Get_GameObjectBack(L"Layer_SelectUI", SCENE_STAGE));

	pDesc = new CUI::UI_DESC;
	pDesc->fPosX = m_fPosX - m_fSizeX * 0.25f;
	pDesc->fPosY = m_fPosY + m_fSizeY * 0.25f;
	pDesc->fSizeX = m_fSizeX * 0.25f;
	pDesc->fSizeY = m_fSizeY * 0.25f;
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_NumberUI", SCENE_STAGE, L"Layer_NumberUI", pDesc);
	m_pNumberUI = static_cast<CNumberUI*>(g_pManagement->Get_GameObjectBack(L"Layer_NumberUI", SCENE_STAGE));

	pDesc = new CUI::UI_DESC;
	pDesc->fPosX = m_fPosX;
	pDesc->fPosY = m_fPosY;
	pDesc->fSizeX = m_fSizeX;
	pDesc->fSizeY = m_fSizeY;
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_CursorUI", SCENE_STAGE, L"Layer_CursorUI", pDesc);
	m_pCursorUI = static_cast<CCursorUI*>(g_pManagement->Get_GameObjectBack(L"Layer_CursorUI", SCENE_STAGE));

	return;
}

CPet_Slot* CPet_Slot::Create(_Device pGraphic_Device)
{
	CPet_Slot* pInstance = new CPet_Slot(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CPet_Slot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPet_Slot::Clone_GameObject(void * pArg)
{
	CPet_Slot* pInstance = new CPet_Slot(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CPet_Slot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPet_Slot::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pRenderer);

	CUI::Free();

	return;
}
