#include "stdafx.h"
#include "Calling_Colleague.h"

#include "Player_Colleague.h"

#include "UI_Manager.h"


CCalling_Colleague::CCalling_Colleague(_Device Graphic_Device)
	: CUI(Graphic_Device)
{
}

CCalling_Colleague::CCalling_Colleague(const CCalling_Colleague& rhs)
	: CUI(rhs)
{
}

HRESULT CCalling_Colleague::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CCalling_Colleague::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_fPosX = 80.f;
	m_fPosY = 80.f;
	m_fSizeY = 80.f;
	m_fSizeX = 80.f;

	m_bIsActive = true;

	SetUp_Default();

	return S_OK;
}

_int CCalling_Colleague::Update_GameObject(_double TimeDelta)
{
	CGameObject::LateInit_GameObject();
	CUI::Update_GameObject(TimeDelta);

	SetUp_State(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	//if (g_pInput_Device->Key_Down(DIK_F2))
	//{
	//	//m_bIsSelect = true;
	//	Check_Calling_Colleague(true);
	//}
	//	
	//else if (g_pInput_Device->Key_Down(DIK_F3))
	//{
	//	//m_bIsSelect = false;
	//	Check_Calling_Colleague(false);
	//}
		

	return S_OK;
}

_int CCalling_Colleague::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CCalling_Colleague::LateInit_GameObject()
{
	return S_OK;
}

HRESULT CCalling_Colleague::Render_GameObject()
{
	if (nullptr == m_pShaderCom || nullptr == m_pBufferCom)
		return E_FAIL;

	if (false == m_bIsActive)
		return NOERROR;

	CUI_Manager* pUIManager = CUI_Manager::Get_Instance();

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);


	
	// 플레이어가 마우스를 UI 위에 올렸을 때 불이 들어와야 한다.
	/*m_pShaderCom->Begin_Shader();

	m_pShaderCom->Begin_Pass(1);

	if (FAILED(SetUp_ConstantTable(m_iRender_Index)))
		return E_FAIL;

	m_pShaderCom->Commit_Changes();
	m_pBufferCom->Render_VIBuffer();
	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();*/


	return S_OK;
}

HRESULT CCalling_Colleague::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_Calling_Colleague", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCalling_Colleague::SetUp_ConstantTable(_uint iIndex)
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

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, iIndex)))
		return E_FAIL;

	return S_OK;
}

void CCalling_Colleague::SetUp_State(_double TimeDelta)
{

}

void CCalling_Colleague::SetUp_RenderIndex()
{
	// 마우스를 아이콘 위에 올려두면 인덱스가 변한다
	//if()
}

void CCalling_Colleague::SetUp_Default()
{
}

void CCalling_Colleague::Check_Calling_Colleague(_bool Check_Call_Colleague)
{
	CGameObject* pTempColleague = nullptr;

 	if (true == Check_Call_Colleague)
	{
		if (nullptr == pTempColleague)
		{
			pTempColleague = g_pManagement->Clone_GameObject_Return(L"GameObject_Colleague",
				&CPlayer_Colleague::JACK_INFO(_v3(10.f, 0.f, 5.f), 0.f, 0));
			g_pManagement->Add_GameOject_ToLayer_NoClone(pTempColleague, SCENE_STAGE, L"Layer_Colleague", nullptr);

			
		}
		else
		{
			static_cast<CPlayer_Colleague*>(pTempColleague)->Check_Navi();
			//pTempColleague = nullptr;
		}
	}
	else if (false == Check_Call_Colleague)
	{
		if (nullptr == pTempColleague)
			return;
		else
		{
			pTempColleague->Set_Enable(false);
			pTempColleague = nullptr;
			return;
		}
	}
}

CCalling_Colleague * CCalling_Colleague::Create(_Device pGraphic_Device)
{
	CCalling_Colleague* pInstance = new CCalling_Colleague(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("CPlayerHP Creating ColleagueUI Fail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCalling_Colleague::Clone_GameObject(void * pArg)
{
	CCalling_Colleague* pInstance = new CCalling_Colleague(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("CPlayerHP Creating ColleagueUI Fail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCalling_Colleague::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBufferCom);

	CGameObject::Free();
}
