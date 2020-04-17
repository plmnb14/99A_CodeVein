#include "stdafx.h"
#include "..\Headers\PlayerST.h"
#include "HPBack.h"

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

	m_fPosX = 202.f;
	m_fPosY = 620.f;
	m_fSizeX = 280.f;
	m_fSizeY = 10.f;
	m_fViewZ = 0.f;

	m_bIsActive = true;

	
	m_pTarget = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);

	m_fPlayerST = m_pTarget->Get_Target_Stamina();
	m_fTotalST = m_fPlayerST;

	UI_DESC* pDesc = new UI_DESC;
	pDesc->fPosX = 202.f;
	pDesc->fPosY = m_fPosY;
	pDesc->fSizeX = 345.f;
	pDesc->fSizeY = 30.f;
	pDesc->iIndex = 0;

	//g_pManagement->Add_GameObject_ToLayer(L"GameObject_HPBack", SCENE_STAGE, L"Layer_HPBack", pDesc);
	//static_cast<CHPBack*>(g_pManagement->Get_GameObjectBack(L"Layer_HPBack", SCENE_STAGE))->Set_Active(true);

	return NOERROR;
}

_int CPlayerST::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

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

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	m_matOldView = pManagement->Get_Transform(D3DTS_VIEW);
	m_matOldProj = pManagement->Get_Transform(D3DTS_PROJECTION);

	pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);


	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();

	m_pShaderCom->Begin_Pass(2);

	// 버퍼를 렌더링한다.
	// (인덱스버퍼(012023)에 보관하고있는 인덱스를 가진 정점을 그리낟.)
	// 삼각형 두개를 그리낟.각각의 삼각형마다 정점세개, 각각의 정점을 버텍스 셰이더의 인자로 던진다.
	m_pBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();


	pManagement->Set_Transform(D3DTS_VIEW, m_matOldView);
	pManagement->Set_Transform(D3DTS_PROJECTION, m_matOldProj);

	Safe_Release(pManagement);

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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_STBar", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CPlayerST::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

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

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 0)))
		return E_FAIL;

	return NOERROR;
}

void CPlayerST::SetUp_State(_double TimeDelta)
{
	if (m_fPlayerST >= m_fTotalST)
		m_fPlayerST = m_fTotalST;
	if (m_fPlayerST <= 0.f)
		m_fPlayerST = 0.f;

	m_fPlayerST = m_pTarget->Get_Target_Stamina();

	// Texture UV 흐르는 속도
	m_fSpeed += 0.f;
	m_fPercentage = m_fPlayerST / m_fTotalST;


	// ----------임시(플레이어 ST조절)------------------------------------------------
	/*if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		m_fPlayerST += 15.f * _float(TimeDelta);
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		m_fPlayerST -= 15.f * _float(TimeDelta);*/
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
