#include "stdafx.h"
#include "Armor_Inven.h"


CArmor_Inven::CArmor_Inven(_Device pDevice)
	: CUI(pDevice)
{
}

CArmor_Inven::CArmor_Inven(const CArmor_Inven & rhs)
	: CUI(rhs)
{
}

HRESULT CArmor_Inven::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CArmor_Inven::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);
	m_fPosX = WINCX * 0.75f;
	m_fPosY = WINCY * 0.5f;
	m_fSizeX = WINCX * 0.5f;
	m_fSizeY = WINCY;

	m_fViewZ = 1.f;

	m_bIsActive = false;

	SetUp_Default();
	return NOERROR;
}

_int CArmor_Inven::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	if (g_pInput_Device->Key_Up(DIK_4))
		m_bIsActive = !m_bIsActive;

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);

	for (auto& pSlot : m_vecArmorSlot)
	{
		pSlot->Set_ViewZ(m_fViewZ - 0.1f);
		pSlot->Set_Active(m_bIsActive);
	}

	if (m_bIsActive)
	{
		//if(g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
		if (GetAsyncKeyState(VK_LBUTTON) && 0x8000)
			Regist_Armor();
		//if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_RB))
		if (GetAsyncKeyState(VK_RBUTTON) && 0x8000)
			Unregist_Armor();
	}

	return NO_EVENT;
}

_int CArmor_Inven::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CArmor_Inven::Render_GameObject()
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


	m_pBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matOldView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matOldProj);

	return NOERROR;
}

HRESULT CArmor_Inven::Add_Component()
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

HRESULT CArmor_Inven::SetUp_ConstantTable()
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

void CArmor_Inven::SetUp_Default()
{
	CUI::UI_DESC* pDesc = nullptr;
	CArmor_Slot* pSlot = nullptr;
	LOOP(CArmor::ARMOR_END)
	{
		pDesc = new CUI::UI_DESC;
		pDesc->fPosX = m_fPosX - 100.f;
		pDesc->fPosY = m_fPosY - 100.f + 90.f * i;
		pDesc->fSizeX = 80.f;
		pDesc->fSizeY = 80.f;
		pDesc->iIndex = i;
		g_pManagement->Add_GameObject_ToLayer(L"GameObject_ArmorSlot", SCENE_STAGE, L"Layer_ArmorSlot", pDesc);
		pSlot = static_cast<CArmor_Slot*>(g_pManagement->Get_GameObjectBack(L"Layer_ArmorSlot", SCENE_STAGE));
		m_vecArmorSlot.push_back(pSlot);
	}
}


void CArmor_Inven::Regist_Armor()
{
	for (auto& pSlot : m_vecArmorSlot)
		pSlot->Set_Select(false);
	for (auto& pSlot : m_vecArmorSlot)
	{
		if (pSlot->Pt_InRect())
		{
			m_pRegistArmor = pSlot;
			pSlot->Set_Select(true);
		}
	}
}

void CArmor_Inven::Unregist_Armor()
{
	for (auto& pSlot : m_vecArmorSlot)
	{
		if (pSlot->Pt_InRect())
		{
			pSlot->Set_Select(false);
			m_pRegistArmor = nullptr;
		}
	}
}

CArmor_Inven * CArmor_Inven::Create(_Device pGraphic_Device)
{
	CArmor_Inven* pInstance = new CArmor_Inven(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CArmor_Inven::Clone_GameObject(void * pArg)
{
	CArmor_Inven* pInstance = new CArmor_Inven(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CArmor_Inven::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	CUI::Free();
}

