#include "stdafx.h"
#include "..\Headers\PlayerHP.h"
#include "PlayerFontUI.h"
#include "FontNumUI.h"

CPlayerHP::CPlayerHP(_Device pGraphic_Device)
	: CUI(pGraphic_Device)
{
	ZeroMemory(&m_vecNoiseDir, sizeof(_v2));
}

CPlayerHP::CPlayerHP(const CPlayerHP & rhs)
	: CUI(rhs)
{
	ZeroMemory(&m_vecNoiseDir, sizeof(_v2));
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
	m_fSizeX = 320.f;
	m_fSizeY = 64.f;

	m_bIsActive = true;

	SetUp_Default();
	
	return NOERROR;
}

_int CPlayerHP::Update_GameObject(_double TimeDelta)
{
	if (g_eSceneID_Cur == SCENE_STAGE_BASE)
	{
		Set_Active(false);
		m_pFontCurHP->Set_Active(false);
		m_pFontTotalHP->Set_Active(false);
		return S_OK;
	}
	else
	{
		Set_Active(true);
		m_pFontCurHP->Set_Active(true);
		m_pFontTotalHP->Set_Active(true);
	}

	CUI::Update_GameObject(TimeDelta);

	m_pTarget = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);
	if (!m_pTarget)
		return 0;

	SetUp_State(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	if(m_fPlayerHP <= 0.f)
		m_pFontCurHP->Update_NumberValue(0);
	else
		m_pFontCurHP->Update_NumberValue(m_fPlayerHP);

	m_pFontTotalHP->Update_NumberValue(m_fTotalHP);

	
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
	_uint iIndex = 0;
	LOOP(6)
	{
		if (0 == i)
		{
			m_fSizeX = 320.f;
			m_fSizeY = 64.f;
			iIndex = 0;
			iPass = 1;
		}
		else if (1 == i)
		{
			m_fSizeX = 300.f;
			m_fSizeY = 64.f;
			iIndex = 6;
			iPass = 3;
		}
		else if (2 == i)
		{
			m_fSizeX = 300.f;
			m_fSizeY = 64.f;
			iIndex = 1;
			iPass = 2;
		}
		else if (3 == i)
		{
			m_fSizeX = 300.f;
			m_fSizeY = 32.f;
			iIndex = 5;
			iPass = 10;
		}
		else if (4 == i)
		{
			m_fSizeX = 300.f;
			m_fSizeY = 64.f;
			iIndex = 3;
			iPass = 1;
		}
		else
		{
			m_fSizeX = 320.f;
			m_fSizeY = 64.f;
			iIndex = 2;
			iPass = 1;
		}

		m_matWorld._11 = m_fSizeX;
		m_matWorld._22 = m_fSizeY;
		m_matWorld._33 = 1.f;
		m_matWorld._41 = m_fPosX - WINCX * 0.5f;
		m_matWorld._42 = -m_fPosY + WINCY * 0.5f;

		
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

	if (6 == iIndex)
	{
		m_fPercentage = m_fOldHP / m_fTotalHP;
	}
	else
	{
		m_fPercentage = m_fPlayerHP / m_fTotalHP;
	}
	
	{
		if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))

			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_fSpeed", &m_fSpeed, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_Value("g_fPercentage", &m_fPercentage, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_Value("g_vNoiseDir", &m_vecNoiseDir, sizeof(_v2))))
			return E_FAIL;

		if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, iIndex)))
			return E_FAIL;
	}
	

	return NOERROR;
}

void CPlayerHP::SetUp_Default()
{
	// Player 현재 체력 폰트
	m_pFontCurHP = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontCurHP->Set_UI_Pos(m_fPosX + 68.f, m_fPosY);
	m_pFontCurHP->Set_UI_Size(15.f, 30.f);
	m_pFontCurHP->Set_ViewZ(m_fViewZ - 0.1f);
	m_pFontCurHP->Set_Active(true);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontCurHP, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	// 플레이어 전체 체력 폰트
	m_pFontTotalHP = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pFontTotalHP->Set_UI_Pos(m_fPosX + 123.f, m_fPosY);
	m_pFontTotalHP->Set_UI_Size(10.f, 20.f);
	m_pFontTotalHP->Set_ViewZ(m_fViewZ - 0.1f);
	m_pFontTotalHP->Set_Active(true);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFontTotalHP, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

}

void CPlayerHP::SetUp_State(_double TimeDelta)
{
	// 수정
	m_fTotalHP = m_pTarget->Get_Target_Param().fHp_Max;
	m_fPlayerHP = m_pTarget->Get_Target_Param().fHp_Cur;

	if (m_fOldHP <= m_fPlayerHP)
		m_fOldHP = m_fPlayerHP;
	else
		m_fOldHP -= TimeDelta * 20.f;

	if (m_fOldHP >= m_fTotalHP)
		m_fOldHP = m_fTotalHP;

	if (m_fPlayerHP <= 0.f)
		m_fPlayerHP = 0.f;
	else if (m_fPlayerHP >= m_fTotalHP)
		m_fPlayerHP = m_fTotalHP;
	
	// Texture UV 흐르는 속도
	m_fSpeed += -0.2f * _float(TimeDelta);

	m_vecNoiseDir += -0.08f * _float(TimeDelta) * V2_ONE;

	//m_fPercentage = m_fPlayerHP / m_fTotalHP;
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
