#include "stdafx.h"
#include "..\Headers\PlayerHP.h"
#include "PlayerFontUI.h"
#include "FontNumUI.h"

CPlayerHP::CPlayerHP(_Device pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CPlayerHP::CPlayerHP(const CPlayerHP & rhs)
	: CUI(rhs)
{
}

HRESULT CPlayerHP::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CPlayerHP::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_fPosX = 200.f;
	m_fPosY = 637.f;
	m_fSizeX = 301.f;
	m_fSizeY = 64.f;

	m_bIsActive = true;

	SetUp_Default();
	
	return NOERROR;
}

_int CPlayerHP::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	SetUp_State(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	if(m_fPlayerHP <= 0.f)
		m_pFontCurHP->Set_Number(0);
	else
		m_pFontCurHP->Set_Number(_uint(m_fPlayerHP));

	m_pFontTotalHP->Set_Number(_uint(m_fTotalHP));

	return NO_EVENT;
}

_int CPlayerHP::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CPlayerHP::Render_GameObject()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	
	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	_uint iPass = 0;
	for (_uint i = 0; i < 4; ++i)
	{
		if (FAILED(SetUp_ConstantTable(i)))
			return E_FAIL;
		(i == 1) ? (iPass = 2) : (iPass = 1);
		m_pShaderCom->Begin_Shader();

		m_pShaderCom->Begin_Pass(iPass);

		m_pBufferCom->Render_VIBuffer();

		m_pShaderCom->End_Pass();

		m_pShaderCom->End_Shader();
	}
	

	return NOERROR;
}

HRESULT CPlayerHP::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_PlayerHPUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CPlayerHP::SetUp_ConstantTable(_uint iIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (iIndex == 1)
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
	else
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

	return NOERROR;
}

void CPlayerHP::SetUp_Default()
{
	m_pTarget = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);

	if (nullptr != m_pTarget)
		m_fPlayerHP = m_pTarget->Get_Target_Hp();

	else
		m_fPlayerHP = 0.f;

	m_fTotalHP = m_fPlayerHP;

	// Player 현재 체력 폰트
	m_pFontCurHP = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontCurHP->Set_UI_Pos(m_fPosX + 30.f, m_fPosY);
	m_pFontCurHP->Set_UI_Size(50.f, 50.f);
	m_pFontCurHP->Set_ViewZ(m_fViewZ - 0.1f);
	m_pFontCurHP->Set_Active(true);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontCurHP, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	// 플레이어 전체 체력 폰트
	m_pFontTotalHP = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontTotalHP->Set_UI_Pos(m_fPosX + 115.f, m_fPosY - 3.f);
	m_pFontTotalHP->Set_UI_Size(25.f, 25.f);
	m_pFontTotalHP->Set_ViewZ(m_fViewZ - 0.1f);
	m_pFontTotalHP->Set_Active(true);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontTotalHP, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
}

void CPlayerHP::SetUp_State(_double TimeDelta)
{
	if (m_fPlayerHP >= m_fTotalHP)
		m_fPlayerHP = m_fTotalHP;
	if (m_fPlayerHP <= 0.f)
		m_fPlayerHP = 0.f;

	m_fPlayerHP = m_pTarget->Get_Target_Hp();

	// Texture UV 흐르는 속도
	m_fSpeed += -0.2f * _float(TimeDelta);

	m_fPercentage = m_fPlayerHP / m_fTotalHP;
}

CPlayerHP * CPlayerHP::Create(_Device pGraphic_Device)
{
	CPlayerHP* pInstance = new CPlayerHP(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("CPlayerHP Creating Fail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayerHP::Clone_GameObject(void * pArg)
{
	CPlayerHP* pInstance = new CPlayerHP(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CPlayerHP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerHP::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
