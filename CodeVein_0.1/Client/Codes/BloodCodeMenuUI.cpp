#include "stdafx.h"
#include "..\Headers\BloodCodeMenuUI.h"
#include "Player.h"
#include "UI_Manager.h"

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

	SetUp_Default();

	
	return NOERROR;
}

_int CBloodCodeMenuUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	Compute_ViewZ(&m_pTransformCom->Get_Pos());

	m_pTarget = static_cast<CPlayer*>(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));
	if (nullptr == m_pTarget)
		return NO_EVENT;

	_v3 vAngle = TARGET_TO_TRANS(m_pTarget)->Get_Angle();
	
	m_pTransformCom->Set_Angle(vAngle);
	_v3 vLookZ = TARGET_TO_TRANS(m_pTarget)->Get_Axis(AXIS_Z);
	_v3 vLookX = TARGET_TO_TRANS(m_pTarget)->Get_Axis(AXIS_X);

	_v3 vDir = *V3_NORMAL_SELF(&(-*V3_NORMAL_SELF(&vLookX) + *V3_NORMAL_SELF(&vLookZ) + _v3(0.f, 2.f, 0.f))) * 2.f;
	_v3 vPosition = TARGET_TO_TRANS(m_pTarget)->Get_Pos() + vDir;
	m_pTransformCom->Set_Pos(vPosition);
	m_pTransformCom->Set_Scale(_v3(3.555555f, 2.f, 0.f));

	_v3 vMyAxisX = m_pTransformCom->Get_Axis(AXIS_X);
	
	_v3 vLength = TARGET_TO_TRANS(m_pTarget)->Get_Pos() + vDir * 0.95f - *V3_NORMAL_SELF(&vMyAxisX) * m_pTransformCom->Get_Size().x * 0.22f;


	TARGET_TO_TRANS(m_pOwnerUI)->Set_Angle(m_pTransformCom->Get_Angle());
	TARGET_TO_TRANS(m_pOwnerUI)->Set_Scale(_v3(2.f, 2.f, 0.f));
	TARGET_TO_TRANS(m_pOwnerUI)->Set_Pos(vLength);
	m_pOwnerUI->Set_ViewZ(m_fViewZ - 0.1f);
	m_pOwnerUI->Set_Active(m_bIsActive);

		
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


	if (FAILED(SetUp_ConstantTable(0)))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(1);

	m_pBufferCom->Render_VIBuffer();
	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

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
	
	return NOERROR;
}

void CBloodCodeMenuUI::SetUp_Default()
{
	m_pOwnerUI = static_cast<CCodeOwnerUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_CodeOwnerUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pOwnerUI, SCENE_STAGE, L"Layer_StageUI", nullptr);
	TARGET_TO_TRANS(m_pOwnerUI)->Set_Scale(_v3(2.f, 2.f, 0.f));

	CBloodCodeSlot* pInstance = nullptr;
	LOOP(5)
	{
		pInstance = static_cast<CBloodCodeSlot*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BloodCodeSlot", nullptr));
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_StageUI", nullptr);
		m_vecBloodCodeSlot.push_back(pInstance);
	}
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
