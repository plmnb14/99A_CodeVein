#include "stdafx.h"
#include "..\Headers\BossHP.h"
#include "BossDecoUI.h"


CBossHP::CBossHP(_Device pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CBossHP::CBossHP(const CBossHP & rhs)
	: CUI(rhs)
{
}

HRESULT CBossHP::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CBossHP::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	/*m_fPosX = WINCX * 0.5f;*/
	/*m_fPosY = WINCY * 0.1f;*/
	m_fSizeX = 840;
	m_fSizeY = 64.f;

	
	m_pDecoUI = static_cast<CBossDecoUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BossDecoUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pDecoUI, SCENE_STAGE, L"Layer_StageUI", nullptr);
	
	m_pDecoUI->Set_UI_Size(WINCX, 64.f);
	m_pDecoUI->Set_ViewZ(m_fViewZ + 0.1f);
	
	m_pBossNameUI = static_cast<CBossNameUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BossNameUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBossNameUI, SCENE_STAGE, L"Layer_StageUI", nullptr);
	
	m_pBossNameUI->Set_UI_Size(200.f, 25.f);
	m_pBossNameUI->Set_ViewZ(m_fViewZ + 0.1f);
	
	
	return NOERROR;
}

_int CBossHP::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	if (nullptr == m_pTarget)
		return NO_EVENT;
	
	SetUp_State(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);
	
	if (m_pDecoUI)
	{
		m_pDecoUI->Set_UI_Pos(m_fPosX, m_fPosY);
		m_pDecoUI->Set_Active(m_bIsActive);
	}
		
	if (m_pBossNameUI)
	{
		m_pBossNameUI->Set_UI_Pos(m_fPosX, m_fPosY - 30.f);
		m_pBossNameUI->Set_Active(m_bIsActive);
		m_pBossNameUI->Set_UI_Index(m_eBossNameIdx);
	}	

	return NO_EVENT;
}

_int CBossHP::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CBossHP::Render_GameObject()
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

	for (_uint i = 0; i < 3; ++i)
	{
		if (i == 0)
		{
			iIndex = 0;
			iPass = 1;
		}
			
		else if (i == 1)
		{
			iIndex = 1;
			iPass = 2;
		}
			
		else if (i == 2)
		{
			iIndex = 4;
			iPass = 3;
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

HRESULT CBossHP::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_BossUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CBossHP::SetUp_ConstantTable(_uint iIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if(1 == iIndex)
		m_fPercentage = m_fBossHP / m_fTotalHP;
	else if(4 == iIndex)
		m_fPercentage = m_fOldHP / m_fTotalHP;

	
	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fSpeed", &m_fSpeed, sizeof(_float))))
		return E_FAIL;
	/*if (FAILED(m_pShaderCom->Set_Value("g_fPosX", &m_fPosX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fSizeX", &m_fSizeX, sizeof(_float))))
		return E_FAIL;*/
	if (FAILED(m_pShaderCom->Set_Value("g_fPercentage", &m_fPercentage, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, iIndex)))
		return E_FAIL;
	
	

	return NOERROR;
}

void CBossHP::SetUp_State(_double TimeDelta)
{
	/*if (m_fBossHP >= m_fTotalHP)
		m_fBossHP = m_fTotalHP;
	if (m_fBossHP <= 0.f)
		m_fBossHP = 0.f;*/
	if (!m_bIsActive)
		return;
	m_fBossHP = m_pTarget->Get_Target_Hp();
	m_fTotalHP = m_pTarget->Get_Target_Param().fHp_Max;

	if (m_fOldHP <= m_fBossHP)
		m_fOldHP = m_fBossHP;
	else
		m_fOldHP -= _float(TimeDelta) * 150.f;

	if (m_fOldHP >= m_fTotalHP)
		m_fOldHP = m_fTotalHP;

	if (m_fBossHP <= 0.f)
		m_fBossHP = 0.f;
	else if (m_fBossHP >= m_fTotalHP)
		m_fBossHP = m_fTotalHP;

	// Texture UV Èå¸£´Â ¼Óµµ
	m_fSpeed += -0.05f * _float(TimeDelta);

	
}

CBossHP * CBossHP::Create(_Device pGraphic_Device)
{
	CBossHP* pInstance = new CBossHP(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("CBossHP Creating Fail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBossHP::Clone_GameObject(void * pArg)
{
	CBossHP* pInstance = new CBossHP(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CBossHP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossHP::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
