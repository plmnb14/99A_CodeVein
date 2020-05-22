#include "stdafx.h"
#include "..\Headers\SkillPointUI.h"
#include "PlayerFontUI.h"

CSkillPointUI::CSkillPointUI(_Device pDevice)
	: CUI(pDevice)
{
}

CSkillPointUI::CSkillPointUI(const CSkillPointUI & rhs)
	: CUI(rhs)
{
}

HRESULT CSkillPointUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CSkillPointUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_fPosX = 930.f;
	m_fPosY = 642.;
	m_fSizeX = 256.f;
	m_fSizeY = 64.f;
	m_bIsActive = true;

	m_pCurPointFont = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pCurPointFont->Set_UI_Size(18.23f, 35.f);
	m_pCurPointFont->Set_UI_Pos(m_fPosX - 20.f, m_fPosY);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pCurPointFont, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pMaxPointFont = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pMaxPointFont->Set_UI_Size(18.23f, 35.f);
	m_pMaxPointFont->Set_UI_Pos(m_fPosX + 40.f, m_fPosY);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pMaxPointFont, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	return NOERROR;
}

_int CSkillPointUI::Update_GameObject(_double TimeDelta)
{
	Late_Init();

	m_pTarget = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);
	if (!m_pTarget)
		return S_OK;
	m_tObjParam = m_pTarget->Get_Target_Param();

	if (g_eSceneID_Cur == SCENE_STAGE_BASE)
	{
		Set_Active(false);
		m_pCurPointFont->Set_Active(false);
		m_pMaxPointFont->Set_Active(false);
		return S_OK;
	}
	else
	{
		Set_Active(true);
		m_pCurPointFont->Set_Active(false);
		m_pMaxPointFont->Set_Active(false);
	}

	CUI::Update_GameObject(TimeDelta);
	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	if (m_bIsActive)
	{
		m_pCurPointFont->Update_NumberValue(_float(m_tObjParam.sMana_Cur));
		m_pMaxPointFont->Update_NumberValue(_float(m_tObjParam.sMana_Max));
		m_pCurPointFont->Set_Active(m_bIsActive);
		m_pMaxPointFont->Set_Active(m_bIsActive);	
	}
	else
	{
		m_pCurPointFont->Set_Active(false);
		m_pMaxPointFont->Set_Active(false);
	}
	return NO_EVENT;
}

_int CSkillPointUI::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CSkillPointUI::Render_GameObject()
{
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

HRESULT CSkillPointUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_SkillPointUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CSkillPointUI::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 0)))
		return E_FAIL;

	return NOERROR;
}

void CSkillPointUI::Late_Init()
{
	if (m_bLateInit)
		return;
	m_bLateInit = true;

}

CSkillPointUI * CSkillPointUI::Create(_Device pGraphic_Device)
{
	CSkillPointUI* pInstance = new CSkillPointUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CSkillPointUI::Clone_GameObject(void * pArg)
{
	CSkillPointUI* pInstance = new CSkillPointUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CSkillPointUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
