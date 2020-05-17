#include "stdafx.h"
#include "..\Headers\Weapon_Slot.h"

CWeapon_Slot::CWeapon_Slot(_Device pDevice)
	: CUI(pDevice)
{
	ZeroMemory(&m_tWeaponParam, sizeof(WPN_PARAM));
}

CWeapon_Slot::CWeapon_Slot(const CWeapon_Slot & rhs)
	: CUI(rhs)
{
}

HRESULT CWeapon_Slot::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CWeapon_Slot::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_bIsActive = false;

	m_tWeaponParam.iWeaponName = WPN_DATA_End;

	return NOERROR;
}

_int CWeapon_Slot::Update_GameObject(_double TimeDelta)
{
	if(m_bIsDead)
		return DEAD_OBJ;

	CUI::Update_GameObject(TimeDelta);
	m_pRendererCom->Add_RenderList(RENDER_UI, this);
	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	/*
	Wpn_SSword, Wpn_SSword_Black, Wpn_SSword_Military, Wpn_SSword_Slave,
	Wpn_LSword, Wpn_LSword_Black, Wpn_LSword_Military, Wpn_LSword_Slave,
	Wpn_Halberd, Wpn_Halberd_Black, Wpn_Halberd_Military, Wpn_Halberd_Slave,
	Wpn_Hammer, Wpn_Hammer_Black, Wpn_Hammer_Military, Wpn_Hammer_Slave,
	Wpn_Gun, Wpn_Gun_Black, Wpn_Gun_Military, Wpn_Gun_Slave,
	WPN_Shield_Normal, WPN_QueenShield, WPN_QueenLance, WPN_DeerKingShield, WPN_FrostBlood_IceGirl,
	WPN_DATA_End*/
	switch (m_tWeaponParam.iWeaponName)
	{
	case Wpn_SSword:
		m_iIndex = 4;
		break;
	case Wpn_SSword_Black:
		m_iIndex = 5;
		break;
	case Wpn_SSword_Military:
		m_iIndex = 6;
		break;
	case Wpn_SSword_Slave:
		m_iIndex = 7;
		break;
	case Wpn_LSword:
		m_iIndex = 8;
		break;
	case Wpn_LSword_Black:
		m_iIndex = 9;
		break;
	case Wpn_LSword_Military:
		m_iIndex = 10;
		break;
	case Wpn_LSword_Slave:
		m_iIndex = 11;
		break;
	case Wpn_Halberd:
		m_iIndex = 12;
		break;
	case Wpn_Halberd_Black:
		m_iIndex = 13;
		break;
	case Wpn_Halberd_Military:
		m_iIndex = 14;
		break;
	case Wpn_Halberd_Slave:
		m_iIndex = 15;
		break;
	case Wpn_Hammer:
		m_iIndex = 16;
		break;
	case Wpn_Hammer_Black:
		m_iIndex = 17;
		break;
	case Wpn_Hammer_Military:
		m_iIndex = 18;
		break;
	case Wpn_Hammer_Slave:
		m_iIndex = 19;
		break;
	case Wpn_Gun:
		m_iIndex = 20;
		break;
	case Wpn_Gun_Black:
		m_iIndex = 21;
		break;
	case Wpn_Gun_Military:
		m_iIndex = 22;
		break;
	case Wpn_Gun_Slave:
		m_iIndex = 23;
		break;
	default:
		m_iIndex = 0;
		break;
	}

	m_bIsCollMouse = Pt_InRect();

	return NO_EVENT;
}

_int CWeapon_Slot::Late_Update_GameObject(_double TimeDelta)
{
	if (m_bIsDead)
		return DEAD_OBJ;

	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);

	m_matWorld._11 = m_fSizeX;
	m_matWorld._22 = m_fSizeY;
	m_matWorld._33 = 1.f;
	m_matWorld._41 = m_fPosX - WINCX * 0.5f;
	m_matWorld._42 = -m_fPosY + WINCY * 0.5f;

	return NO_EVENT;
}

HRESULT CWeapon_Slot::Render_GameObject()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	if (!m_bIsActive)
		return NOERROR;

	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);
	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	_uint iIndex = 0;

	if (!m_bIsSelect)
	{
		if (m_bIsCollMouse)
		{
			LOOP(3)
			{
				if (0 == i)
					iIndex = 0;
				else if (1 == i)
					iIndex = m_iIndex;
				else if (2 == i)
					iIndex = 2;

				if (FAILED(SetUp_ConstantTable(iIndex)))
					return E_FAIL;
				m_pShaderCom->Begin_Shader();
				m_pShaderCom->Begin_Pass(1);
				m_pBufferCom->Render_VIBuffer();
				m_pShaderCom->End_Pass();
				m_pShaderCom->End_Shader();
			}
		}
		else
		{
			LOOP(2)
			{
				if (0 == i)
					iIndex = 0;
				else if (1 == i)
					iIndex = m_iIndex;
				
				if (FAILED(SetUp_ConstantTable(iIndex)))
					return E_FAIL;
				m_pShaderCom->Begin_Shader();
				m_pShaderCom->Begin_Pass(1);
				m_pBufferCom->Render_VIBuffer();
				m_pShaderCom->End_Pass();
				m_pShaderCom->End_Shader();
			}
		}
	}
	else
	{
		if (m_bIsCollMouse)
		{
			LOOP(4)
			{
				if (0 == i)
					iIndex = 0;
				else if (1 == i)
					iIndex = m_iIndex;
				else if (2 == i)
					iIndex = 1;
				else if (3 == i)
					iIndex = 2;

				if (FAILED(SetUp_ConstantTable(iIndex)))
					return E_FAIL;
				m_pShaderCom->Begin_Shader();
				m_pShaderCom->Begin_Pass(1);
				m_pBufferCom->Render_VIBuffer();
				m_pShaderCom->End_Pass();
				m_pShaderCom->End_Shader();
			}
		}
		else
		{
			LOOP(3)
			{
				if (0 == i)
					iIndex = 0;
				else if (1 == i)
					iIndex = m_iIndex;
				else if (2 == i)
					iIndex = 1;

				if (FAILED(SetUp_ConstantTable(iIndex)))
					return E_FAIL;
				m_pShaderCom->Begin_Shader();
				m_pShaderCom->Begin_Pass(1);
				m_pBufferCom->Render_VIBuffer();
				m_pShaderCom->End_Pass();
				m_pShaderCom->End_Shader();
			}
		}
	}
	

	return NOERROR;
}


_bool CWeapon_Slot::Pt_InRect()
{
	return g_pInput_Device->MousePt_InRect(m_fPosX, m_fPosY, m_fSizeX, m_fSizeY, g_hWnd);
}

HRESULT CWeapon_Slot::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_WeaponSlot", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CWeapon_Slot::SetUp_ConstantTable(_uint iIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, iIndex)))
		return E_FAIL;

	return NOERROR;
}

CWeapon_Slot * CWeapon_Slot::Create(_Device pGraphic_Device)
{
	CWeapon_Slot* pInstance = new CWeapon_Slot(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("CWeaponSlot Creating Fail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWeapon_Slot::Clone_GameObject(void * pArg)
{
	CWeapon_Slot* pInstance = new CWeapon_Slot(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("CWeaponSlot Cloned Fail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Slot::Free()
{
	
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}

