#include "stdafx.h"
#include "..\Headers\BossHP.h"
#include "HPBack.h"
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
	m_bIsActive = false;

	m_fPosX = WINCX * 0.5f;
	m_fPosY = 80.f + 10.f;

	m_fSizeX = WINCX * 0.5f - 10.f;
	m_fSizeY = 20.f;
	
	UI_DESC* pDesc = new UI_DESC;
	pDesc->fPosX = m_fPosX + 23.f;
	pDesc->fPosY = m_fPosY + 1.7f;
	pDesc->fSizeX = WINCX - 17.f;
	pDesc->fSizeY = 55.f;
	pDesc->iIndex = 2;
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_HPBack", SCENE_STAGE, L"Layer_HPBack", pDesc);
	m_pHPBack = static_cast<CHPBack*>(g_pManagement->Get_GameObjectBack(L"Layer_HPBack", SCENE_STAGE));
	m_pHPBack->Set_ViewZ(m_fViewZ + 0.1f);
	
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_BossDecoUI", SCENE_STAGE, L"Layer_BossDecoUI");
	m_pDecoUI = static_cast<CBossDecoUI*>(g_pManagement->Get_GameObjectBack(L"Layer_BossDecoUI", SCENE_STAGE));
	m_pDecoUI->Set_ViewZ(m_fViewZ + 0.2f);

	// 보스와 연동
	m_pTarget = g_pManagement->Get_GameObjectBack(L"Layer_Boss", SCENE_STAGE);
	m_fBossHP = m_pTarget->Get_Target_Hp();
	m_fTotalHP = m_fBossHP;
	
	return NOERROR;
}

_int CBossHP::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	SetUp_State(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	m_pHPBack->Set_Active(m_bIsActive);
	m_pDecoUI->Set_Active(m_bIsActive);

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

HRESULT CBossHP::SetUp_ConstantTable()
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

void CBossHP::SetUp_State(_double TimeDelta)
{
	if (m_fBossHP >= m_fTotalHP)
		m_fBossHP = m_fTotalHP;
	if (m_fBossHP <= 0.f)
		m_fBossHP = 0.f;

	m_fBossHP = m_pTarget->Get_Target_Hp();

	// Texture UV 흐르는 속도
	m_fSpeed += -0.05f * _float(TimeDelta);

	m_fPercentage = m_fBossHP / m_fTotalHP;


	// ----------임시(Boss HP조절)------------------------------------------------
	/*if (GetAsyncKeyState(VK_DIVIDE) & 0x8000)
		m_fBossHP += 15.f * _float(TimeDelta);
	if (GetAsyncKeyState(VK_MULTIPLY) & 0x8000)
		m_fBossHP -= 15.f * _float(TimeDelta);*/
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
