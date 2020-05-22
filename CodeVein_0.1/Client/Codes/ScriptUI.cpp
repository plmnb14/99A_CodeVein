#include "stdafx.h"
#include "..\Headers\ScriptUI.h"


CScriptUI::CScriptUI(_Device pDevice)
	: CUI(pDevice)
{
}

CScriptUI::CScriptUI(const CScriptUI & rhs)
	: CUI(rhs)
{
}

HRESULT CScriptUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CScriptUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fPosX = WINCX * 0.5f;
	m_fPosY = 55.f;
	m_fSizeX = 512.f * 0.7f;
	m_fSizeY = 128.f * 0.7f;

	m_fLifeTime = 5.f;
	
	return NOERROR;
}

_int CScriptUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	switch (m_eTalker)
	{
	case Talker_Player:
		m_iTalkerIndex = 2;
		break;
	case Talker_Jack:
		m_iTalkerIndex = 3;
		break;
	case Talker_Yakumo:
		m_iTalkerIndex = 4;
		break;
	case Talker_Yokumo:
		m_iTalkerIndex = 9;
		break;
	case Talker_End:
		break;
	}

	switch (m_eScriptNum)
	{
	case Jack_Player_Test1:
		m_iScriptIndex = 0;
		break;
	case Jack_Player_Test2:
		m_iScriptIndex = 1;
		break;
	case Yakumo_Hello_0_WhatsUp:
		m_iScriptIndex = 6;
		break;
	case Yakumo_Hello_1_WhatsUp:
		m_iScriptIndex = 8;
		break;
	case Yakumo_Bye_0_SeeYouAround:
		m_iScriptIndex = 5;
		break;
	case Yakumo_Bye_1_MakeSure:
		m_iScriptIndex = 7;
		break;
	case Script_End:
		break;
	}

	if (m_bIsActive && 1.f > m_fAlpha)
		m_fAlpha += _float(TimeDelta) * 2.f;
	if(!m_bIsActive && 0.f < m_fAlpha)
		m_fAlpha -= _float(TimeDelta) * 2.f;

	// 활성화 상태의 UI -> LifeTime 초과시 비활성화(Fade In / Out 으로 활성화 Or 비활성화)
	Calc_LifeTime(TimeDelta);
	
	return NO_EVENT;
}

_int CScriptUI::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CScriptUI::Render_GameObject()
{
	if (!m_bIsActive
		&& m_fAlpha <= 0.f)
		return NOERROR;

	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);
	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	_uint iIndex = 0;

	LOOP(2)
	{
		if (0 == i)
			iIndex = m_iTalkerIndex;
		else if (1 == i)
			iIndex = m_iScriptIndex;

		if (FAILED(SetUp_ConstantTable(iIndex)))
			return E_FAIL;

		m_pShaderCom->Begin_Shader();
		m_pShaderCom->Begin_Pass(5);
		m_pBufferCom->Render_VIBuffer();
		m_pShaderCom->End_Pass();
		m_pShaderCom->End_Shader();
	}


	return NOERROR;
}

HRESULT CScriptUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_ScriptUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CScriptUI::SetUp_ConstantTable(_uint iIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, iIndex)))
		return E_FAIL;

	return NOERROR;
}

void CScriptUI::SetUp_Default()
{
}

void CScriptUI::Calc_LifeTime(_double TimeDelta)
{
	if (!m_bIsActive)
		return;

	m_fDelta += _float(TimeDelta);

	if (m_fDelta >= m_fLifeTime)
	{
		m_fDelta = 0.f;
		m_bIsActive = false;
	}
}

CScriptUI * CScriptUI::Create(_Device pGraphic_Device)
{
	CScriptUI* pInstance = new CScriptUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create CScriptUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CScriptUI::Clone_GameObject(void * pArg)
{
	CScriptUI* pInstance = new CScriptUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Clone CScriptUI");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CScriptUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
