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

	m_fPosX = WINCX * 0.3f;
	m_fPosY = WINCY * 0.5f;
	m_fSizeX = 352.f;
	m_fSizeY = 471.f;

	m_fViewZ = 3.f;

	m_bIsActive = false;

	// Slot Create
	CUI::UI_DESC* pDesc = nullptr;
	CWeapon_Slot* pSlot = nullptr;
	LOOP(5)
	{
		pDesc = new CUI::UI_DESC;
		pDesc->fPosX = m_fPosX - 100.f;
		pDesc->fPosY = m_fPosY - 100.f + 70.f * i;
		pDesc->fSizeX = 60.f;
		pDesc->fSizeY = 60.f;
		pDesc->iIndex = i;
		g_pManagement->Add_GameObject_ToLayer(L"GameObject_WeaponSlot", SCENE_STAGE, L"Layer_WeaponSlot", pDesc);
		pSlot = static_cast<CWeapon_Slot*>(g_pManagement->Get_GameObjectBack(L"Layer_WeaponSlot", SCENE_STAGE));
		m_vecWeaponSlot.push_back(pSlot);
	}

	LOOP(2)
	{
		pDesc = new CUI::UI_DESC;
		pDesc->fPosX = m_fPosX + 100.f;
		pDesc->fPosY = m_fPosY - 50.f + 100.f * i;
		pDesc->fSizeX = 80.f;
		pDesc->fSizeY = 80.f;
		pDesc->iIndex = CWeapon::WPN_DATA_End;
		g_pManagement->Add_GameObject_ToLayer(L"GameObject_WeaponSlot", SCENE_STAGE, L"Layer_WeaponSlot", pDesc);
		pSlot = static_cast<CWeapon_Slot*>(g_pManagement->Get_GameObjectBack(L"Layer_WeaponSlot", SCENE_STAGE));
		pSlot->Set_UI_Index(CWeapon::WPN_DATA_End);
		m_pSelectSlot[i] = pSlot;
	}

	return NOERROR;
}

_int CWeapon_Inven::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	if (g_pInput_Device->Key_Up(DIK_3))
		m_bIsActive = !m_bIsActive;

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);


	

	if (GetAsyncKeyState(VK_LBUTTON) && 0x8000)
		Regist_Weapon();
	if (GetAsyncKeyState(VK_RBUTTON) && 0x8000)
		Unregist_Weapon();

	for (auto& pWeaponSlot : m_vecWeaponSlot)
	{
		pWeaponSlot->Set_Active(m_bIsActive);
		pWeaponSlot->Set_ViewZ(m_fViewZ - 0.1f);
	}

	LOOP(2)
	{
		m_pSelectSlot[i]->Set_Active(m_bIsActive);
		m_pSelectSlot[i]->Set_ViewZ(m_fViewZ - 0.1f);
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

void CWeapon_Inven::Regist_Weapon(CWeapon::WEAPON_DATA eData)
{
	if (CWeapon::WPN_DATA_End == m_pSelectSlot[0]->Get_Type())
	{
		m_pSelectSlot[0]->Set_UI_Index(eData);
		return;
	}

	else if (CWeapon::WPN_DATA_End == m_pSelectSlot[1]->Get_Type())
	{
		m_pSelectSlot[1]->Set_UI_Index(eData);
		return;
	}

	else
		return;
}

void CWeapon_Inven::Unregist_Weapon(CWeapon_Slot* pWeaponSlot)
{
	if (nullptr == pWeaponSlot)
		return;

	pWeaponSlot->Set_UI_Index(CWeapon::WPN_DATA_End);
}

void CWeapon_Inven::Regist_Weapon()
{
	for (auto& pWeaponSlot : m_vecWeaponSlot)
	{
		if (pWeaponSlot->Pt_InRect() && !pWeaponSlot->Get_Select())
		{
			if (CWeapon::WPN_DATA_End == m_pSelectSlot[0]->Get_Type())
			{
				m_pSelectSlot[0]->Set_UI_Index(pWeaponSlot->Get_Type());
				pWeaponSlot->Set_Select(true);
			}

			else if (CWeapon::WPN_DATA_End == m_pSelectSlot[1]->Get_Type())
			{
				m_pSelectSlot[1]->Set_UI_Index(pWeaponSlot->Get_Type());
				pWeaponSlot->Set_Select(true);
			}

			else
				return;
		}
	}
}

void CWeapon_Inven::Unregist_Weapon()
{
	for (auto& pWeaponSlot : m_vecWeaponSlot)
	{
		if (pWeaponSlot->Get_Type() == m_pSelectSlot[0]->Get_Type() &&
			pWeaponSlot->Get_Type() != CWeapon::WPN_DATA_End && pWeaponSlot->Pt_InRect() &&
			pWeaponSlot->Get_Select())
		{
			m_pSelectSlot[0]->Set_UI_Index(CWeapon::WPN_DATA_End);
			pWeaponSlot->Set_Select(false);
		}
		if (pWeaponSlot->Get_Type() == m_pSelectSlot[1]->Get_Type() &&
			pWeaponSlot->Get_Type() != CWeapon::WPN_DATA_End && pWeaponSlot->Pt_InRect() &&
			pWeaponSlot->Get_Select())
		{
			m_pSelectSlot[1]->Set_UI_Index(CWeapon::WPN_DATA_End);
			pWeaponSlot->Set_Select(false);
		}
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
