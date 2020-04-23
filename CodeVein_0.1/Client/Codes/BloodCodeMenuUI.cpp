#include "stdafx.h"
#include "..\Headers\BloodCodeMenuUI.h"
#include "Player.h"

CBloodCodeMenuUI::CBloodCodeMenuUI(_Device pDevice)
	: CUI(pDevice)
{
}

CBloodCodeMenuUI::CBloodCodeMenuUI(const CBloodCodeMenuUI & rhs)
	: CUI(rhs)
{
}

HRESULT CBloodCodeMenuUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CBloodCodeMenuUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fAlpha = 1.f;
	
	return NOERROR;
}

_int CBloodCodeMenuUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	
	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	m_pTarget = static_cast<CPlayer*>(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));
	if (nullptr == m_pTarget)
		return NO_EVENT;
	
	m_pTransformCom->Set_Angle(TARGET_TO_TRANS(m_pTarget)->Get_Angle());
	_v3 vLookZ = TARGET_TO_TRANS(m_pTarget)->Get_Axis(AXIS_Z);
	_v3 vLookX = TARGET_TO_TRANS(m_pTarget)->Get_Axis(AXIS_X);
	m_pTransformCom->Set_Pos(TARGET_TO_TRANS(m_pTarget)->Get_Pos() + (*V3_NORMAL_SELF(&vLookZ) + _v3(0.f, 2.f, 0.f) + *V3_NORMAL_SELF(&vLookX)));
	m_pTransformCom->Set_Scale(_v3(3.f, 3.f, 0.f));

	Compute_ViewZ(&m_pTransformCom->Get_Pos());

	return NO_EVENT;
}

_int CBloodCodeMenuUI::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	m_matView = g_pManagement->Get_Transform(D3DTS_VIEW);
	m_matProj = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	return NO_EVENT;
}

HRESULT CBloodCodeMenuUI::Render_GameObject()
{
	if (!m_bIsActive)
		return NOERROR;

	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	_uint iIndex = 0;

	LOOP(2)
	{
		if (0 == i)
			iIndex = 0;
		else if (1 == i)
			iIndex = 1;

		if (FAILED(SetUp_ConstantTable(iIndex)))
			return E_FAIL;

		m_pShaderCom->Begin_Shader();
		m_pShaderCom->Begin_Pass(0);

		m_pBufferCom->Render_VIBuffer();
		m_pShaderCom->End_Pass();
		m_pShaderCom->End_Shader();
	}
	

	return NOERROR;
}

HRESULT CBloodCodeMenuUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_BloodCodeMenuUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CBloodCodeMenuUI::SetUp_ConstantTable(_uint iIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, iIndex)))
		return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_Value("g_fAlpha", &m_fAlpha, sizeof(_float))))
	//	return E_FAIL;

	return NOERROR;
}

CBloodCodeMenuUI * CBloodCodeMenuUI::Create(_Device pGraphic_Device)
{
	CBloodCodeMenuUI* pInstance = new CBloodCodeMenuUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CBloodCodeMenuUI::Clone_GameObject(void * pArg)
{
	CBloodCodeMenuUI* pInstance = new CBloodCodeMenuUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CBloodCodeMenuUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
