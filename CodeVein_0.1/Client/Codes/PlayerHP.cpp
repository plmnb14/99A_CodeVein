#include "stdafx.h"
#include "..\Headers\PlayerHP.h"
#include "FontNumManager.h"
#include "FontNumUI.h"
#include "HPBack.h"

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

	m_fPosX = 202.f;
	m_fPosY = 658.f;
	m_fSizeX = 285.f;
	m_fSizeY = 30.1f;

	m_fViewZ = 1.f;
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

HRESULT CPlayerHP::SetUp_ConstantTable()
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

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 1)))
		return E_FAIL;

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

	// HP 숫자 폰트 사이의 슬래시
	UI_DESC* pDesc = new UI_DESC;
	pDesc->fPosX = m_fPosX + 80.f;
	pDesc->fPosY = m_fPosY;
	pDesc->fSizeX = 10.f;
	pDesc->fSizeY = 20.f;
	pDesc->iIndex = 10;
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_FontNumUI", SCENE_STAGE, L"Layer_FontNumUI", pDesc);
	static_cast<CFontNumUI*>(g_pManagement->Get_GameObjectBack(L"Layer_FontNumUI", SCENE_STAGE))->Set_ViewZ(m_fViewZ - 0.1f);

	g_pManagement->Add_GameObject_ToLayer(L"GameObject_FontNumManager", SCENE_STAGE, L"Layer_FontPlayerUI");
	m_pFontCurHP = static_cast<CFontNumManager*>(g_pManagement->Get_GameObjectBack(L"Layer_FontPlayerUI", SCENE_STAGE));
	m_pFontCurHP->Set_UI_Pos(m_fPosX + 35.f, m_fPosY);
	m_pFontCurHP->Set_UI_Size(15.f, 30.f);
	m_pFontCurHP->Set_ViewZ(m_fViewZ - 0.1f);

	g_pManagement->Add_GameObject_ToLayer(L"GameObject_FontNumManager", SCENE_STAGE, L"Layer_FontPlayerUI");
	m_pFontTotalHP = static_cast<CFontNumManager*>(g_pManagement->Get_GameObjectBack(L"Layer_FontPlayerUI", SCENE_STAGE));
	m_pFontTotalHP->Set_UI_Pos(m_fPosX + 100.f, m_fPosY);
	m_pFontTotalHP->Set_UI_Size(10.f, 20.f);
	m_pFontTotalHP->Set_ViewZ(m_fViewZ - 0.1f);


	pDesc = new UI_DESC;
	pDesc->fPosX = m_fPosX;
	pDesc->fPosY = m_fPosY - 0.8f;
	pDesc->fSizeX = m_fSizeX + 63.f;
	pDesc->fSizeY = m_fSizeY + 26.f;
	pDesc->iIndex = 0;
	//g_pManagement->Add_GameObject_ToLayer(L"GameObject_HPBack", SCENE_STAGE, L"Layer_HPBack", pDesc);
	//static_cast<CHPBack*>(g_pManagement->Get_GameObjectBack(L"Layer_HPBack", SCENE_STAGE))->Set_Active(true);
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


	// ----------임시(플레이어HP조절)------------------------------------------------
	/*if (GetAsyncKeyState(VK_UP) & 0x8000)
		m_fPlayerHP += 15.f * _float(TimeDelta);
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		m_fPlayerHP -= 15.f * _float(TimeDelta);*/
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
