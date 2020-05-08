#include "stdafx.h"
#include "..\Headers\Colleague_UI.h"

CColleague_UI::CColleague_UI(_Device Graphic_Device)
	: CUI(Graphic_Device)
{
}

CColleague_UI::CColleague_UI(const CColleague_UI& rhs)
	: CUI(rhs)
{
}

HRESULT CColleague_UI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CColleague_UI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_fPosX = 119.50f;
	m_fPosY = 421.50f;
	m_fSizeX = 155.f;
	m_fSizeY = 13.f;

	//m_bIsActive = true;

	SetUp_Default();

	return S_OK;
}

_int CColleague_UI::Update_GameObject(_double TimeDelta)
{
	CGameObject::LateInit_GameObject();
	CUI::Update_GameObject(TimeDelta);

	if (true == m_pTarget->Get_Dead())
		return DEAD_OBJ;

	SetUp_State(TimeDelta);

	if (false == m_pTarget->Get_Dead())	// 동료가 살아있을 때만 랜더해야 함
		m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	return S_OK;
}

_int CColleague_UI::Late_Update_GameObject(_double TimeDelta)
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);

	m_matWorld._11 = m_fSizeX;
	m_matWorld._22 = m_fSizeY;
	m_matWorld._33 = 1.f;
	m_matWorld._41 = m_fPosX - WINCX * 0.5f;
	m_matWorld._42 = -m_fPosY + WINCY * 0.5f;

	return S_OK;
}

HRESULT CColleague_UI::LateInit_GameObject()
{


	return S_OK;
}

HRESULT CColleague_UI::Render_GameObject()
{
	if (nullptr == m_pShaderCom || nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	

	m_pShaderCom->Begin_Shader();

 	if (false == m_pTarget->Get_Dead())	// 동료가 살아있을 때만 랜더해야 함
 	{
	_uint Temp[5] = { 1, 1, 2, 2 };

	for (_uint i = 0; i < 4; ++i)
	{
		m_pShaderCom->Begin_Pass(Temp[i]);

		if (0 == i)
		{
			if (FAILED(SetUp_ConstantTable(0)))
				return E_FAIL;

			m_pShaderCom->Commit_Changes();
			m_pBufferCom->Render_VIBuffer();
			m_pShaderCom->End_Pass();
		}
 		if (1 == i)
 		{
 			if (FAILED(SetUp_ConstantTable(1)))
				return E_FAIL;
			m_fPosX = 192.f;
			m_fPosY = 421.50f;
			m_fSizeX = 155.f;
			m_fSizeY = 13.f;
 			
			m_pShaderCom->Commit_Changes();
			m_pBufferCom->Render_VIBuffer();
			m_pShaderCom->End_Pass();
 		}
		if (2 == i)
		{
			if (FAILED(SetUp_ConstantTable(2)))
				return E_FAIL;
			m_fPosX = 192.f;
			m_fPosY = 421.50f;
			m_fSizeX = 155.f;
			m_fSizeY = 13.f;

			m_pShaderCom->Commit_Changes();
			m_pBufferCom->Render_VIBuffer();
			m_pShaderCom->End_Pass();
		}
		if (3 == i)
		{
			if (FAILED(SetUp_ConstantTable(3)))
				return E_FAIL;
			m_fPosX = 192.f;
			m_fPosY = 421.50f;
			m_fSizeX = 155.f;
			m_fSizeY = 13.f;

			m_pShaderCom->Commit_Changes();
			m_pBufferCom->Render_VIBuffer();
			m_pShaderCom->End_Pass();
		}
		
	}

		m_pShaderCom->End_Shader();
	}

	return S_OK;
}

HRESULT CColleague_UI::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_Colleague_UI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CColleague_UI::SetUp_ConstantTable(_uint iIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	/*if (0 == iIndex)
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

		if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 0)))
			return E_FAIL;
	}*/
	if (1 == iIndex)
	{
		if(FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_Value("g_fPosX", &m_fPosX, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_Value("g_fSizeX", &m_fSizeX, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 1)))
			return E_FAIL;
	}
	else if (2 == iIndex)
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

		if (FAILED(m_pShaderCom->Set_Value("g_fSpeed", &m_fSpeed, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_Value("g_fPercentage", &m_fWhite_Percentage, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 2)))
			return E_FAIL;
	}
	else if (3 == iIndex)
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

		if (FAILED(m_pShaderCom->Set_Value("g_fSpeed", &m_fSpeed, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_Value("g_fPercentage", &m_fPercentage, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 3)))
			return E_FAIL;
	}
	

	return S_OK;
}

void CColleague_UI::SetUp_State(_double TimeDelta)
{
	m_fColleagueHp = m_pTarget->Get_Target_Param().fHp_Cur;
	m_fTotalHP = m_pTarget->Get_Target_Param().fHp_Max;

	m_fPercentage = m_fColleagueHp / m_fTotalHP;

	if (m_fTotalHP > m_fColleagueHp)
	{
		m_fOldHP = m_fColleagueHp;
		//m_fWhite_Percentage = m_fOldHP / m_fTotalHP;
	}


	if (m_fPercentage < m_fWhite_Percentage)
	{
		_float fGap = m_fPercentage - m_fWhite_Percentage;

		if (fGap < 0)
			fGap = 0.05f;

		m_fWhite_Percentage -= fGap * 5.f * (_float)TimeDelta;

		if (m_fWhite_Percentage < m_fPercentage)
			m_fWhite_Percentage = m_fPercentage;
	}

	// 줄어들 때, 빨간색 먼저 줄어든 뒤, 약 0.05초 후 하얀색이 천천히 줄어든다.
	// 만약 동료가 죽으면 랜더를 종료한다.
	if (m_fColleagueHp >= m_fTotalHP)
		m_fColleagueHp = m_fTotalHP;
	if (m_fColleagueHp <= 0.f)
		m_fColleagueHp = 0.f;
}

void CColleague_UI::SetUp_RenderIndex()
{
	if (0 == m_iRenderCheck_Index)
	{
		m_fPosX = 70.f;
		m_fPosY = 400.f;
		m_fSizeX = 90.f;
		m_fSizeY = 90.f;
	}
	if (1 == m_iRenderCheck_Index)
	{
		m_fPosX = 80.f;
		m_fPosY = 400.f;
		m_fSizeX = 37.f;
		m_fSizeY = 17.f;
	}
	/*if (2 == m_iRenderCheck_Index)
	{
		m_fSizeX = 92.f;
		m_fSizeY = 430.f;

		m_fPosX = 100.f;
		m_fPosY = 30.f;
	}
	if (3 == m_iRenderCheck_Index)
	{
		m_fSizeX = 92.f;
		m_fSizeY = 430.f;

		m_fPosX = 100.f;
		m_fPosY = 30.f;
	}
	if (4 == m_iRenderCheck_Index)
	{
		m_fSizeX = 92.f;
		m_fSizeY = 430.f;

		m_fPosX = 100.f;
		m_fPosY = 30.f;
	}*/
}

void CColleague_UI::SetUp_Default()
{
}

CColleague_UI * CColleague_UI::Create(_Device pGraphic_Device)
{
	CColleague_UI* pInstance = new CColleague_UI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("CPlayerHP Creating ColleagueUI Fail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CColleague_UI::Clone_GameObject(void * pArg)
{
	CColleague_UI* pInstance = new CColleague_UI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("CPlayerHP Creating ColleagueUI Fail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CColleague_UI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBufferCom);

	CGameObject::Free();
}
