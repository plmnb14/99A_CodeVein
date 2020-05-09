#include "stdafx.h"
#include "..\Headers\Weapon_Inven.h"


CWeapon_Inven::CWeapon_Inven(_Device pDevice)
	: CUI(pDevice)
{
}

CWeapon_Inven::CWeapon_Inven(const CWeapon_Inven & rhs)
	: CUI(rhs)
{
}

HRESULT CWeapon_Inven::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CWeapon_Inven::Ready_GameObject(void * pArg)
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

	// Slot Create
	CUI::UI_DESC* pDesc = nullptr;
	CWeapon_Slot* pSlot = nullptr;
	
	/*for (_uint i = 0; i < 6; ++i)
	{

		for (_uint j = 0; j < 5; ++j)
		{
			pDesc = new CUI::UI_DESC;
			pDesc->fPosX = m_fPosX - 103.f + 52.f * j;
			pDesc->fPosY = m_fPosY - 130.f + 52.f * i;
			pDesc->fSizeX = 50.f;
			pDesc->fSizeY = 50.f;
			g_pManagement->Add_GameObject_ToLayer(L"GameObject_WeaponSlot", SCENE_STAGE, L"Layer_WeaponSlot", pDesc);
			pSlot = static_cast<CWeapon_Slot*>(g_pManagement->Get_GameObjectBack(L"Layer_WeaponSlot", SCENE_STAGE));
			m_vecWeaponSlot.push_back(pSlot);
		}

	}*/

	LOOP(2)
		m_UseWeapon[i] = WEAPON_None;

	Add_Weapon(WEAPON_Hammer);
	Add_Weapon(WEAPON_Gun);
		
	return NOERROR;
}

_int CWeapon_Inven::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);


	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);

	Click_Inven();
	


	for (auto& pWeaponSlot : m_vecWeaponSlot)
	{
		pWeaponSlot->Set_Active(m_bIsActive);
		pWeaponSlot->Set_ViewZ(m_fViewZ - 0.1f);
	}

	
	return NO_EVENT;
}

_int CWeapon_Inven::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CWeapon_Inven::Render_GameObject()
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

	m_pShaderCom->Begin_Pass(1);

	m_pBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matOldView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matOldProj);

	return NOERROR;
}

HRESULT CWeapon_Inven::Add_Component()
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

HRESULT CWeapon_Inven::SetUp_ConstantTable()
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

void CWeapon_Inven::Click_Inven()
{
	if (!m_bIsActive)
		return;

	for (auto& pSlot : m_vecWeaponSlot)
	{
		if (pSlot->Pt_InRect())
		{
			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB) &&
				!pSlot->Get_Select())
			{
				Regist_Weapon(pSlot);
			}
			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_RB) &&
				pSlot->Get_Select())
			{
				UnRegist_Weapon(pSlot);
			}
		}
	}
}


void CWeapon_Inven::Regist_Weapon(CWeapon_Slot* pWeaponSlot)
{
	if (pWeaponSlot->Get_Type() == WEAPON_None)
		return;
	if (m_UseWeapon[0] == WEAPON_None)
	{
		m_UseWeapon[0] = pWeaponSlot->Get_Type();
		pWeaponSlot->Set_Select(true);
	}
	else if (m_UseWeapon[1] == WEAPON_None)
	{
		m_UseWeapon[1] = pWeaponSlot->Get_Type();
		pWeaponSlot->Set_Select(true);
	}
	else
		return;
}

void CWeapon_Inven::UnRegist_Weapon(CWeapon_Slot * pWeaponSlot)
{
	if (pWeaponSlot->Get_Type() == WEAPON_None)
		return;
	if (pWeaponSlot->Get_Type() == m_UseWeapon[0])
	{
		m_UseWeapon[0] = WEAPON_None;
		pWeaponSlot->Set_Select(false);
	}
	else if (pWeaponSlot->Get_Type() == m_UseWeapon[1])
	{
		m_UseWeapon[1] = WEAPON_None;
		pWeaponSlot->Set_Select(false);
	}
	else
		return;
}

void CWeapon_Inven::Add_Weapon(WEAPON_STATE eType)
{
	/*for (auto& pSlot : m_vecWeaponSlot)
	{
		if (pSlot->Get_Type() == WEAPON_None)
		{
			pSlot->Set_Type(eType);
			return;
		}
		else
			continue;
	}*/
	CUI::UI_DESC* pDesc = nullptr;
	CWeapon_Slot* pSlot = nullptr;

	
	pDesc = new CUI::UI_DESC;
	
	pDesc->fSizeX = 50.f;
	pDesc->fSizeY = 50.f;
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_WeaponSlot", SCENE_MORTAL, L"Layer_PlayerUI", pDesc);
	pSlot = static_cast<CWeapon_Slot*>(g_pManagement->Get_GameObjectBack(L"Layer_PlayerUI", SCENE_MORTAL));
	pSlot->Set_Type(eType);
	m_vecWeaponSlot.push_back(pSlot);

	// 슬롯 생성시 위치 조정
	for (_uint i = 0; i < m_vecWeaponSlot.size(); ++i)
	{
		m_vecWeaponSlot[i]->Set_Active(m_bIsActive);
		m_vecWeaponSlot[i]->Set_ViewZ(m_fViewZ - 0.1f);
		m_vecWeaponSlot[i]->Set_UI_Pos(m_fPosX - 103.f + 52.f * (i % 5), m_fPosY - 100.f + 52.f * (i / 5));
	}
}


CWeapon_Inven * CWeapon_Inven::Create(_Device pGraphic_Device)
{
	CWeapon_Inven* pInstance = new CWeapon_Inven(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CWeapon_Inven::Clone_GameObject(void * pArg)
{
	CWeapon_Inven* pInstance = new CWeapon_Inven(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CWeapon_Inven::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	CUI::Free();
}
