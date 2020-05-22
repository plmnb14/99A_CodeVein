#include "stdafx.h"
#include "..\Headers\ExplainWeaponUI.h"
#include "UI_Manager.h"
#include "Player.h"

CExplainWeaponUI::CExplainWeaponUI(_Device pDevice)
	: CUI(pDevice)
{
}

CExplainWeaponUI::CExplainWeaponUI(const CExplainWeaponUI & rhs)
	: CUI(rhs)
{
}

HRESULT CExplainWeaponUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CExplainWeaponUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_tWeaponParam.iWeaponName = WPN_DATA_End;
	m_tWeaponParam.iWeaponType = WEAPON_End;
	m_tWeaponParam.iReinforce = 0;
	m_tWeaponParam.fDamage = 0.f;
	m_tWeaponParam.fRadius = 0.f;
	m_tWeaponParam.fTrail_Min = 0.f;
	m_tWeaponParam.fTrail_Max = 0.f;
	m_tWeaponParam.fCol_Height = 0.f;

	m_pAttackFont = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pAttackFont->Set_ViewZ(m_fViewZ - 0.1f);
	m_pAttackFont->Set_UI_Pos(m_fPosX + 110.f, m_fPosY + 235.f);
	m_pAttackFont->Set_UI_Size(15.f, 30.f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pAttackFont, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	
	m_pAddDamageFont = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pAddDamageFont->Set_ViewZ(m_fViewZ - 0.1f);
	m_pAddDamageFont->Set_UI_Pos(m_fPosX + 100.f, m_fPosY + 157.f);
	m_pAddDamageFont->Set_UI_Size(15.f, 30.f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pAddDamageFont, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pReinForceFont = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pReinForceFont->Set_ViewZ(m_fViewZ - 0.1f);
	m_pReinForceFont->Set_UI_Pos(m_fPosX + 100.f, m_fPosY + 130.f);
	m_pReinForceFont->Set_UI_Size(15.f, 30.f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pReinForceFont, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	return NOERROR;
}

_int CExplainWeaponUI::Update_GameObject(_double TimeDelta)
{
	if (m_bIsActive && !m_bIsSubActive)
	{
		SetUp_SubUI_Active(true);
		m_bIsSubActive = true;
	}
	else if (!m_bIsActive && m_bIsSubActive)
	{
		SetUp_SubUI_Active(false);
		m_bIsSubActive = false;
	}

	
	if (!m_bIsActive)
	{
		m_tWeaponParam.iWeaponName = WPN_DATA_End;
		m_tWeaponParam.iWeaponType = WEAPON_End;
		m_tWeaponParam.iReinforce = 0;
		m_tWeaponParam.fDamage = 0.f;
		m_tWeaponParam.fRadius = 0.f;
		m_tWeaponParam.fTrail_Min = 0.f;
		m_tWeaponParam.fTrail_Max = 0.f;
		m_tWeaponParam.fCol_Height = 0.f;

		SetUp_SubUI_Active(false);
		return NO_EVENT;
	}
		
	if (m_tWeaponParam.iWeaponName == WPN_DATA_End)
	{
		m_bIsActive = false;
	}
		

	CUI::Update_GameObject(TimeDelta);
	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	m_pTarget = static_cast<CPlayer*>(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));
	if (!m_pTarget)		
		return NO_EVENT;
	
	
	
	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);
	
	m_pAttackFont->Set_Active(m_bIsActive);
	m_pAddDamageFont->Set_Active(m_bIsActive);
	m_pReinForceFont->Set_Active(m_bIsActive);
	
	m_pAttackFont->Update_NumberValue(_float(m_tWeaponParam.fDamage));
	m_pAttackFont->Set_UI_Pos(630.f, 392.f);

	m_pAddDamageFont->Update_NumberValue(_float(m_tWeaponParam.fPlusDamage));
	m_pAddDamageFont->Set_UI_Pos(630.f, 288.f);
	
	m_pReinForceFont->Update_NumberValue(_float(m_tWeaponParam.iReinforce));
	m_pReinForceFont->Set_UI_Pos(630.f, 263.f);

	return NO_EVENT;
}

_int CExplainWeaponUI::Late_Update_GameObject(_double TimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;

	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);

	m_matWorld._11 = m_fSizeX;
	m_matWorld._22 = m_fSizeY;
	m_matWorld._33 = 1.f;
	m_matWorld._41 = m_fPosX - WINCX * 0.5f;
	m_matWorld._42 = -m_fPosY + WINCY * 0.5f;

	return NO_EVENT;
}

HRESULT CExplainWeaponUI::Render_GameObject()
{
	if (!m_bIsActive || m_tWeaponParam.iWeaponName == WPN_DATA_End)
		return NOERROR;

	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	SetUp_TexIdx();

	_uint iIndex = 0;
	LOOP(4)
	{
		if (0 == i)
		{
			iIndex = 0;
			m_fSizeX = 720.f;
			m_fSizeY = 720.f;
			m_fPosX = WINCX * 0.5f + 30.f;
			m_fPosY = WINCY * 0.5f - 30.f;
		}
		else if (1 == i)
		{
			m_fSizeX = 400.f;
			m_fSizeY = 225.f;
			m_fPosX = WINCX * 0.5f + 70.f;
			m_fPosY = WINCY * 0.5f - 245.f;
			iIndex = m_iInfoIdx;
		}
		else if (2 == i)
		{
			m_fSizeX = 80.f;
			m_fSizeY = 80.f;
			m_fPosX = WINCX * 0.5f - 135.f;
			m_fPosY = WINCY * 0.5f - 238.f;
			iIndex = m_iWeaponIconIdx;
		}
		else if (3 == i)
		{
			m_fSizeX = 256.f;
			m_fSizeY = 64.f;
			m_fPosX = WINCX * 0.5f;
			m_fPosY = WINCY * 0.5f + 120.f;
			iIndex = m_iWeaponMoveIdx;
		}

		m_matWorld._11 = m_fSizeX;
		m_matWorld._22 = m_fSizeY;
		m_matWorld._33 = 1.f;
		m_matWorld._41 = m_fPosX - WINCX * 0.5f;
		m_matWorld._42 = -m_fPosY + WINCY * 0.5f;

		if (FAILED(SetUp_ConstantTable(iIndex)))
			return E_FAIL;

		m_pShaderCom->Begin_Shader();
		m_pShaderCom->Begin_Pass(1);
		m_pBufferCom->Render_VIBuffer();
		m_pShaderCom->End_Pass();
		m_pShaderCom->End_Shader();
	}


	return NOERROR;
}

HRESULT CExplainWeaponUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_ExplainWeaponUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CExplainWeaponUI::SetUp_ConstantTable(_uint iIndex)
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

void CExplainWeaponUI::SetUp_SubUI_Active(_bool bIsActive)
{
	if (m_tWeaponParam.iWeaponName = WPN_DATA_End)
	{
		m_pAttackFont->Set_Active(false);
		m_pAddDamageFont->Set_Active(false);
		m_pReinForceFont->Set_Active(false);
	}
	
	m_pAttackFont->Set_Active(bIsActive);
	m_pAddDamageFont->Set_Active(bIsActive);
	m_pReinForceFont->Set_Active(bIsActive);
}

void CExplainWeaponUI::SetUp_TexIdx()
{
	switch (m_tWeaponParam.iWeaponName)
	{
	case Wpn_SSword:
	{
		m_iInfoIdx = 1;
		m_iWeaponIconIdx = 21;
		m_iWeaponMoveIdx = 43;
	}
	break;
	case Wpn_SSword_Black:
	{
		m_iInfoIdx = 2;
		m_iWeaponIconIdx = 22;
		m_iWeaponMoveIdx = 43;
	}
	break;
	case Wpn_SSword_Military:
	{
		m_iInfoIdx = 3;
		m_iWeaponIconIdx = 23;
		m_iWeaponMoveIdx = 43;
	}
	break;
	case Wpn_SSword_Slave:
	{
		m_iInfoIdx = 4;
		m_iWeaponIconIdx = 24;
		m_iWeaponMoveIdx = 43;
	}
	break;
	case Wpn_LSword:
	{
		m_iInfoIdx = 5;
		m_iWeaponIconIdx = 25;
		m_iWeaponMoveIdx = 44;
	}
	break;
	case Wpn_LSword_Black:
	{
		m_iInfoIdx = 6;
		m_iWeaponIconIdx = 26;
		m_iWeaponMoveIdx = 44;
	}
	break;
	case Wpn_LSword_Military:
	{
		m_iInfoIdx = 7;
		m_iWeaponIconIdx = 27;
		m_iWeaponMoveIdx = 44;
	}
	break;
	case Wpn_LSword_Slave:
	{
		m_iInfoIdx = 8;
		m_iWeaponIconIdx = 28;
		m_iWeaponMoveIdx = 44;
	}
	break;
	case Wpn_Halberd:
	{
		m_iInfoIdx = 9;
		m_iWeaponIconIdx = 29;
		m_iWeaponMoveIdx = 42;
	}
	break;
	case Wpn_Halberd_Black:
	{
		m_iInfoIdx = 10;
		m_iWeaponIconIdx = 30;
		m_iWeaponMoveIdx = 42;
	}
	break;
	case Wpn_Halberd_Military:
	{
		m_iInfoIdx = 11;
		m_iWeaponIconIdx = 31;
		m_iWeaponMoveIdx = 42;
	}
	break;
	case Wpn_Halberd_Slave:
	{
		m_iInfoIdx = 12;
		m_iWeaponIconIdx = 32;
		m_iWeaponMoveIdx = 42;
	}
	break;
	case Wpn_Hammer:
	{
		m_iInfoIdx = 13;
		m_iWeaponIconIdx = 33;
		m_iWeaponMoveIdx = 44;
	}
	break;
	case Wpn_Hammer_Black:
	{
		m_iInfoIdx = 14;
		m_iWeaponIconIdx = 34;
		m_iWeaponMoveIdx = 44;
	}
	break;
	case Wpn_Hammer_Military:
	{
		m_iInfoIdx = 15;
		m_iWeaponIconIdx = 35;
		m_iWeaponMoveIdx = 44;
	}
	break;
	case Wpn_Hammer_Slave:
	{
		m_iInfoIdx = 16;
		m_iWeaponIconIdx = 36;
		m_iWeaponMoveIdx = 44;
	}
	break;
	case Wpn_Gun:
	{
		m_iInfoIdx = 17;
		m_iWeaponIconIdx = 37;
		m_iWeaponMoveIdx = 43;
	}
	break;
	case Wpn_Gun_Black:
	{
		m_iInfoIdx = 18;
		m_iWeaponIconIdx = 38;
		m_iWeaponMoveIdx = 43;
	}
	break;
	case Wpn_Gun_Military:
	{
		m_iInfoIdx = 19;
		m_iWeaponIconIdx = 39;
		m_iWeaponMoveIdx = 43;
	}
	break;
	case Wpn_Gun_Slave:
	{
		m_iInfoIdx = 20;
		m_iWeaponIconIdx = 40;
		m_iWeaponMoveIdx = 43;
	}
	break;
	default:
	{
		m_iInfoIdx = 41;
		m_iWeaponIconIdx = 41;
		m_iWeaponMoveIdx = 41;
	}
	break;
	}
}

CExplainWeaponUI * CExplainWeaponUI::Create(_Device pGraphic_Device)
{
	CExplainWeaponUI* pInstance = new CExplainWeaponUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CExplainWeaponUI::Clone_GameObject(void * pArg)
{
	CExplainWeaponUI* pInstance = new CExplainWeaponUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CExplainWeaponUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	CUI::Free();
}
