#include "stdafx.h"
#include "..\Headers\Armor_Inven.h"
#include "Armor_Slot.h"

CArmor_Inven::CArmor_Inven(_Device pDevice)
	: CUI(pDevice)
{
}

CArmor_Inven::CArmor_Inven(const CArmor_Inven & rhs)
	: CUI(rhs)
{
}

ARMOR_TYPE CArmor_Inven::Get_UseArmorType()
{
	return m_eRegistArmor;
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

	m_fPosX = 229.5f;
	m_fPosY = 325.5f;
	m_fSizeX = 280.f;
	m_fSizeY = 471.f;
	m_fViewZ = 4.f;

	m_bIsActive = false;

	SetUp_Default();

	Add_Armor(ARMOR_Drape);
	Add_Armor(ARMOR_Gauntlet);
	Add_Armor(ARMOR_LongCoat);

	return NOERROR;
}

_int CArmor_Inven::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	
	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);

	for (auto& pSlot : m_vecArmorSlot)
	{
		pSlot->Set_ViewZ(m_fViewZ - 0.1f);
		pSlot->Set_Active(m_bIsActive);
	}

	Click_Inven();
		
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

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();

	m_pShaderCom->Begin_Pass(1);


	m_pBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();

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
	
}

void CArmor_Inven::Click_Inven()
{
	if (!m_bIsActive)
		return;

	for (auto& pSlot : m_vecArmorSlot)
	{
		if (pSlot->Pt_InRect())
		{
			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB) &&
				!pSlot->Get_Select())
				Regist_Armor(pSlot);
			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_RB) &&
				pSlot->Get_Select())
				UnRegist_Armor(pSlot);
		}
	}
}

void CArmor_Inven::Regist_Armor(CArmor_Slot * pArmorSlot)
{
	if (pArmorSlot->Get_Type() == ARMOR_End)
		return;
	if (m_eRegistArmor == ARMOR_End)
	{
		m_eRegistArmor = pArmorSlot->Get_Type();
		pArmorSlot->Set_Select(true);
	}
	else
		return;
}

void CArmor_Inven::UnRegist_Armor(CArmor_Slot * pArmorSlot)
{
	if (pArmorSlot->Get_Type() == ARMOR_End)
		return;
	if (pArmorSlot->Get_Type() == m_eRegistArmor)
	{
		m_eRegistArmor = ARMOR_End;
		pArmorSlot->Set_Select(false);
	}
}

void CArmor_Inven::Add_Armor(ARMOR_TYPE eType)
{
	CUI::UI_DESC* pDesc = nullptr;
	CArmor_Slot* pSlot = nullptr;


	pDesc = new CUI::UI_DESC;

	pDesc->fSizeX = 50.f;
	pDesc->fSizeY = 50.f;
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_ArmorSlot", SCENE_MORTAL, L"Layer_PlayerUI", pDesc);
	pSlot = static_cast<CArmor_Slot*>(g_pManagement->Get_GameObjectBack(L"Layer_PlayerUI", SCENE_MORTAL));
	pSlot->Set_Type(eType);
	m_vecArmorSlot.push_back(pSlot);

	// 슬롯 생성시 위치 조정
	for (_uint i = 0; i < m_vecArmorSlot.size(); ++i)
	{
		m_vecArmorSlot[i]->Set_Active(m_bIsActive);
		m_vecArmorSlot[i]->Set_ViewZ(m_fViewZ - 0.1f);
		m_vecArmorSlot[i]->Set_UI_Pos(m_fPosX - 103.f + 52.f * (i % 5), m_fPosY - 140.f + 52.f * (i / 5));
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
