#include "stdafx.h"
#include "..\Headers\MassageUI.h"

CMassageUI::CMassageUI(_Device Graphic_Device)
	: CUI(Graphic_Device)
{
}

CMassageUI::CMassageUI(const CMassageUI& rhs)
	: CUI(rhs)

{
}

HRESULT CMassageUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CMassageUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_fPosX = WINCX * 0.5f;
	m_fPosY = WINCY * 0.5f;

	m_fSizeX = 1280.f;
	m_fSizeY = 720.f;

	m_fNow_BarSize = 0.f;

	return S_OK;
}

_int CMassageUI::Update_GameObject(_double TimeDelta)
{
	//cout << m_fAlpha << endl;

	CGameObject::LateInit_GameObject();
	CUI::Update_GameObject(TimeDelta);

	SetUp_State(TimeDelta);

	if (true == m_bCheck_Play_BossnameUI)
	{
		m_pRendererCom->Add_RenderList(RENDER_UI, this);

		/*if (m_fTimer >= 3.f)
		{
			m_bCheck_MassageStart = true;
			m_bCheck_Play_BossnameUI = false;
		}*/
			
	}

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	return S_OK;
}

_int CMassageUI::Late_Update_GameObject(_double TimeDelta)
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);

	m_matWorld._11 = m_fSizeX;
	m_matWorld._22 = m_fSizeY;
	m_matWorld._33 = 1.f;
	/*m_matWorld._41 = m_fPosX - WINCX * 0.5f;
	m_matWorld._42 = -m_fPosY + WINCY * 0.5f;*/

	return S_OK;
}

HRESULT CMassageUI::LateInit_GameObject()
{

	return S_OK;
}

HRESULT CMassageUI::Render_GameObject()
{
	if (nullptr == m_pShaderCom || nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	m_matOldView = g_pManagement->Get_Transform(D3DTS_VIEW);
	m_matOldProj = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	if (FAILED(SetUp_ConstantTable(0)))
		return E_FAIL;

	//_uint TempNumber = 0;


	m_pShaderCom->Begin_Shader();

	m_pShaderCom->Begin_Pass(5);

	/*	if (m_bCheck_Alpha0 == false)
		{
			if (FAILED(m_pShaderCom->Set_Value("g_fAlpha", &m_fAlpha0, sizeof(_float))))
				return E_FAIL;
			TempNumber = 0;
		}
		if(m_bCheck_Alpha0 == true)
		{
			if (FAILED(m_pShaderCom->Set_Value("g_fAlpha", &m_fAlpha1, sizeof(_float))))
				return E_FAIL;
			TempNumber = 1;
		}
		if (m_bCheck_Alpha1 == true)
		{
			if (FAILED(m_pShaderCom->Set_Value("g_fAlpha", &m_fAlpha2, sizeof(_float))))
				return E_FAIL;
			TempNumber = 2;
		}
		if (m_bCheck_Alpha2 == true)
		{
			if (FAILED(m_pShaderCom->Set_Value("g_fAlpha", &m_fAlpha3, sizeof(_float))))
				return E_FAIL;
			TempNumber = 3;
		}
		if (false == m_bCheck_Play_BossnameUI && true == m_bCheck_Alpha0 && true == m_bCheck_Alpha1 &&
			true == m_bCheck_Alpha2)
		{
			if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, TempNumber)))
				return E_FAIL;
		}
		if (true == m_bCheck_Alpha0 && true == m_bCheck_Alpha1 &&
			true == m_bCheck_Alpha2 && true == m_bCheck_Alpha3)
			{
			}
				*/
	for (_uint i = 0; i < 4; ++i)
	{
		if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, i)))
			return E_FAIL;

		m_pShaderCom->Commit_Changes();

		m_pBufferCom->Render_VIBuffer();
		
	}

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();

	return S_OK;
}

HRESULT CMassageUI::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_Start_BossUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMassageUI::SetUp_ConstantTable(_uint TextureIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

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

	if (FAILED(m_pShaderCom->Set_Value("g_fPercentage", &m_fBarPercentage, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	//if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, TextureIndex)))
	//	return E_FAIL;

	return S_OK;
}

void CMassageUI::SetUp_State(_double TimeDelta)
{
	//m_fBarPercentage = m_fNow_BarSize / m_fSizeY;
	// 키를 누르면 불값으로 받아서 해당 불값이 true이면 계속 증가하게 하고
	// 일정 값에 다다르면 불값을 false로 바꿔서 멈추게 한다.
	// 전체 출력 시간이 약 3초 가량 되면 랜더를 멈춘다.

	if (g_pInput_Device->Key_Down(DIK_O))
	{
		m_bCheck_Play_BossnameUI = true;
	}
	/*if (false == m_bPullAlpha)
	{
		if (true == m_bTestbool)
			m_fAlpha0 += 0.01f;
		if (m_fAlpha0 >= 0.5f)
		{
			m_bCheck_Alpha0 = true;
			if (m_fAlpha0 >= 1.f)
				m_fAlpha0 = 1.f;

			m_fAlpha1 += 0.01f;
		}
		if (m_fAlpha1 >= 0.5f)
		{
			if (m_fAlpha1 >= 1.f)
				m_fAlpha1 = 1.f;
			m_bCheck_Alpha1 = true;
			m_fAlpha2 += 0.01f;
		}
		if (m_fAlpha2 >= 0.5f)
		{
			if (m_fAlpha2 >= 1.f)
				m_fAlpha2 = 1.f;
			m_bCheck_Alpha2 = true;
			m_fAlpha3 += 0.01f;
		}
		if (m_fAlpha3 >= 0.5f)
		{
			if (m_fAlpha3 >= 1.f)
				m_fAlpha3 = 1.f;
			m_bCheck_Alpha3 = true;
			m_bCheck_Play_BossnameUI = true;
		}
	}*/
	

	if (true == m_bCheck_Play_BossnameUI && false == m_bPullAlpha)
	{
		
		m_fAlpha += 0.01f;
		if (1.f <= m_fAlpha)
		{
			m_bPullAlpha = true;
			m_fTimer += (_float)TimeDelta;
		}

		/*if (1.f <= m_fAlpha3)
		{
			m_bPullAlpha = true;
			m_fTimer += (_float)TimeDelta;
		}*/
			
	}
	if (true == m_bPullAlpha && m_fAlpha >= 0.f)
	{
		m_fTimer += (_float)TimeDelta;

		if (m_fTimer >= 2.f)
		{
			/*m_fAlpha0 -= 0.01f;
			m_fAlpha1 -= 0.01f;
			m_fAlpha2 -= 0.01f;
			m_fAlpha3 -= 0.01f;*/
			m_fAlpha -= 0.01f;
		}
			

		if (0.f >= m_fAlpha)
			m_bReset = true;
	}

	if (true == m_bReset && 0.f >= m_fAlpha)
	{
		m_bPullAlpha = false;
		m_bCheck_Play_BossnameUI = false;
		m_fTimer = 0.f;
		m_fAlpha = 0.f;
		m_bReset = false;
	}
		
}

CMassageUI* CMassageUI::Create(_Device pGraphic_Device)
{
	CMassageUI* pInstance = new CMassageUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CMassageUI::Clone_GameObject(void * pArg)
{
	CMassageUI* pInstance = new CMassageUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CMassageUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
