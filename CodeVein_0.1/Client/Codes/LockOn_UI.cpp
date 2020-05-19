#include "stdafx.h"
#include "LockOn_UI.h"


CLockOn_UI::CLockOn_UI(_Device Graphic_Device)
	: CUI(Graphic_Device)
{
}

CLockOn_UI::CLockOn_UI(const CLockOn_UI& rhs)
	: CUI(rhs)
{
}

HRESULT CLockOn_UI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CLockOn_UI::Ready_GameObject(void* pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	D3DXMatrixIdentity(&m_matWorld);

	return S_OK;
}

HRESULT CLockOn_UI::LateInit_GameObject()
{
	return S_OK;
}

_int CLockOn_UI::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	CUI::Update_GameObject(TimeDelta);

	Caculate_World();
	m_pTransform->Set_WorldMat(Caculate_Billboard());

	Compute_ViewZ(&Caculate_Pos(m_matWorld));

	m_pRenderer->Add_RenderList(RENDER_UI, this);

	return S_OK;
}

_int CLockOn_UI::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	return S_OK;
}


HRESULT CLockOn_UI::Render_GameObject()
{
	if (nullptr == m_pShader || 
		nullptr == m_pBuffer)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable(0)))
		return E_FAIL;

	m_pShader->Begin_Shader();
	m_pShader->Begin_Pass(11);

	m_pShader->Commit_Changes();

	m_pBuffer->Render_VIBuffer();

	m_pShader->End_Pass();
	m_pShader->End_Shader();

	return S_OK;
}

void CLockOn_UI::Caculate_World()
{
	_mat targetWorldMat , targetBoneMat;
	D3DXMatrixIdentity(&targetWorldMat);
	D3DXMatrixIdentity(&targetBoneMat);

	memcpy(&targetBoneMat._41, &m_pTargetBonemat->_41, sizeof(_v3));
	memcpy(&targetWorldMat._41, &m_pTargetWorldmat->_41, sizeof(_v3));


	m_matWorld = targetBoneMat * targetWorldMat;

	// 위치 보정치
	m_matWorld._42 += 0.5f;
}

_mat CLockOn_UI::Caculate_Billboard()
{
	_mat matBill, matView;

	matView = g_pManagement->Get_Transform(D3DTS_VIEW);
	D3DXMatrixIdentity(&matBill);

	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, NULL, &matBill);

	return matBill * m_matWorld;
}

_v3 CLockOn_UI::Caculate_Pos(_mat _matrix)
{
	_v3 vPos = {};

	memcpy(vPos, &_matrix._41, sizeof(_v3));

	return vPos;
}

void CLockOn_UI::Reset_TargetMatrix()
{
	D3DXMatrixIdentity(&m_matWorld);
	m_pTargetBonemat = nullptr;
	m_pTargetWorldmat = nullptr;
}

HRESULT CLockOn_UI::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_LockOnSite", L"Com_Texture", (CComponent**)&m_pTexture)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBuffer)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLockOn_UI::SetUp_ConstantTable(_uint TextureIndex)
{
	if (nullptr == m_pShader)
		return E_FAIL;

	if (FAILED(m_pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_matView", &g_pManagement->Get_Transform(D3DTS_VIEW), sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_matProj", &g_pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_mat))))
		return E_FAIL;

	m_pTexture->SetUp_OnShader("g_DiffuseTexture", m_pShader, 0);

	return S_OK;
}

CLockOn_UI* CLockOn_UI::Create(_Device pGraphic_Device)
{
	CLockOn_UI* pInstance = new CLockOn_UI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CLockOn_UI::Clone_GameObject(void * pArg)
{
	CLockOn_UI* pInstance = new CLockOn_UI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CLockOn_UI::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pRenderer);

	CGameObject::Free();
}
