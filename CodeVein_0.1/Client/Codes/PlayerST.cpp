#include "stdafx.h"
#include "..\Headers\PlayerST.h"

CPlayerST::CPlayerST(_Device pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CPlayerST::CPlayerST(const CPlayerST & rhs)
	: CUI(rhs)
{
}

HRESULT CPlayerST::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CPlayerST::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_fPosX = 200.f;
	m_fPosY = 607.f;
	m_fSizeX = 301.f;
	m_fSizeY = 64.f;
	m_fViewZ = 1.f;

	m_bIsActive = true;

	return NOERROR;
}

_int CPlayerST::Update_GameObject(_double TimeDelta)
{
	if (g_eSceneID_Cur == SCENE_STAGE_BASE)
	{
		Set_Active(false);
		return S_OK;
	}
	else
	{
		Set_Active(true);
	}

	CUI::Update_GameObject(TimeDelta);
	m_pTarget = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);
	if (!m_pTarget)
		return 0;

	SetUp_State(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	return NO_EVENT;
}

_int CPlayerST::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CPlayerST::Render_GameObject()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	_uint iPass = 0;
	for (_uint i = 0; i < 3; ++i)
	{
		if (FAILED(SetUp_ConstantTable(i)))
			return E_FAIL;

		(i == 1) ? (iPass = 3) : (iPass = 1);

		m_pShaderCom->Begin_Shader();

		m_pShaderCom->Begin_Pass(iPass);

		m_pBufferCom->Render_VIBuffer();

		m_pShaderCom->End_Pass();

		m_pShaderCom->End_Shader();
	}

	return NOERROR;
}

HRESULT CPlayerST::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_PlayerST", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CPlayerST::SetUp_ConstantTable(_uint iIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (iIndex == 0 || iIndex == 2)
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
	else if (iIndex == 1)
	{
		if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_fSpeed", &m_fSpeed, sizeof(_float))))
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

void CPlayerST::SetUp_State(_double TimeDelta)
{
	m_fTotalST = m_pTarget->Get_Target_Param().fStamina_Max;
	m_fPlayerST = m_pTarget->Get_Target_Param().fStamina_Cur;

	if (m_fPlayerST <= 0.f)
		m_fPlayerST = 0.f;

	if (m_fPlayerST >= m_fTotalST)
		m_fPlayerST = m_fTotalST;
	

	// Texture UV Èå¸£´Â ¼Óµµ
	m_fSpeed += 0.f;
	m_fPercentage = m_fPlayerST / m_fTotalST;
}

CPlayerST * CPlayerST::Create(_Device pGraphic_Device)
{
	CPlayerST* pInstance = new CPlayerST(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("CPlayerST Creating Fail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayerST::Clone_GameObject(void * pArg)
{
	CPlayerST* pInstance = new CPlayerST(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CPlayerST");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerST::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
