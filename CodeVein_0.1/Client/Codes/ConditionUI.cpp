#include "stdafx.h"
#include "..\Headers\ConditionUI.h"
#include "Player.h"
#include "PlayerFontUI.h"

CConditionUI::CConditionUI(_Device pDevice)
	: CUI(pDevice)
{

}

CConditionUI::CConditionUI(const CConditionUI & rhs)
	: CUI(rhs)
{

}

void CConditionUI::Set_Condition_Info(_float fCurValue, _float fMaxValue)
{
	m_fCurValue = fCurValue;
	m_fMaxValue = fMaxValue;
}

HRESULT CConditionUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CConditionUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);
	

	m_pFontValue = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontValue, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	m_pFontValue->Set_ViewZ(m_fViewZ - 0.1f);
	
	return NOERROR;
}

_int CConditionUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pTarget = static_cast<CPlayer*>(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));
	if (nullptr == m_pTarget)
		return NO_EVENT;
	m_tObjParam = m_pTarget->Get_Target_Param();

	SetUp_State(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	switch (m_eType)
	{
	case CONDITION_HP:
	{
		m_iIndex = 0;
		m_fCurValue = m_tObjParam.fHp_Cur;
		m_fMaxValue = m_tObjParam.fHp_Max;	
	}		
		break;
	case CONDITION_ST:
	{
		m_iIndex = 1;
		m_fCurValue = m_tObjParam.fStamina_Cur;
		m_fMaxValue = m_tObjParam.fStamina_Max;
	}	
		break;
	case CONDITION_BLOOD:
	{
		m_iIndex = 2;
		m_fMaxValue = 100.f;

		if (g_pInput_Device->Get_DIKeyState(DIK_Y))
			m_fCurValue += (_float)TimeDelta * 20.f;
		if (g_pInput_Device->Get_DIKeyState(DIK_U))
			m_fCurValue -= (_float)TimeDelta * 20.f;
	}		
		break;
	case CONDITION_ATT:
	{
		m_iIndex = 3;
		m_fCurValue = m_tObjParam.fDamage;
	}
		break;
	case CONDITION_DEF:
	{
		m_iIndex = 4;
		m_fCurValue = m_tObjParam.fArmor_Cur;
		m_fMaxValue = m_tObjParam.fArmor_Max;
	}	
		break;
	}

	m_pFontValue->Set_UI_Pos(m_fPosX + 120.f, m_fPosY);
	m_pFontValue->Set_UI_Size(10.f, 10.f);
	return NO_EVENT;
}

_int CConditionUI::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CConditionUI::Render_GameObject()
{
	if (!m_bIsActive)
		return NOERROR;
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	_uint iPass = 0;
	_uint iIndex = 0;
	for (_uint i = 0; i < 2; ++i)
	{
		if (0 == i)
		{
			iPass = 3;
			iIndex = 5;	
		}
		else if (1 == i)
		{
			iPass = 1;
			iIndex = m_iIndex;
		}

		if (FAILED(SetUp_ConstantTable(iIndex)))
			return E_FAIL;

		m_pShaderCom->Begin_Shader();

		m_pShaderCom->Begin_Pass(iPass);

		m_pBufferCom->Render_VIBuffer();

		m_pShaderCom->End_Pass();

		m_pShaderCom->End_Shader();
	}

	return NOERROR;
}

HRESULT CConditionUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_StatusUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CConditionUI::SetUp_ConstantTable(_uint iIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (iIndex <= 4)
	{
		if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, iIndex)))
			return E_FAIL;
	}
	else if (5 == iIndex)
	{
		if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_fPosX", &m_fPosX, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_fSizeX", &m_fSizeX, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_fPercentage", &m_fPercentage, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, iIndex)))
			return E_FAIL;
	}
	
	
	return NOERROR;
}

void CConditionUI::SetUp_State(_double TimeDelta)
{
	if (m_fCurValue >= m_fMaxValue)
		m_fCurValue = m_fMaxValue;
	if (m_fCurValue <= 0.f)
		m_fCurValue = 0.f;
	
	m_fPercentage = m_fCurValue / m_fMaxValue;

	if (m_pFontValue)
	{
		//m_pFontValue->Set_UI_Pos(m_fPosX + 50.f, m_fPosY);
		m_pFontValue->Set_Active(m_bIsActive);
		m_pFontValue->Update_NumberValue(m_fCurValue);
	}
		
}

CConditionUI * CConditionUI::Create(_Device pGraphic_Device)
{
	CConditionUI* pInstance = new CConditionUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CConditionUI::Clone_GameObject(void * pArg)
{
	CConditionUI* pInstance = new CConditionUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CConditionUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
