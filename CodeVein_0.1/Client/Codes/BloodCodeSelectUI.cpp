#include "stdafx.h"
#include "..\Headers\BloodCodeSelectUI.h"
#include "UI_Manager.h"
#include "CollisionMgr.h"

CBloodCodeSelectUI::CBloodCodeSelectUI(_Device pDevice)
	: CUI(pDevice)
{
	ZeroMemory(&m_vSlotPosition, sizeof(_v3));
}

CBloodCodeSelectUI::CBloodCodeSelectUI(const CBloodCodeSelectUI & rhs)
	: CUI(rhs)
{
	ZeroMemory(&m_vSlotPosition, sizeof(_v3));
}

HRESULT CBloodCodeSelectUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CBloodCodeSelectUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	SetUp_Default();

	
	
	return NOERROR;
}

_int CBloodCodeSelectUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	m_pRendererCom->Add_RenderList(RENDER_UI, this);
	

	_v3 vLookX = m_pTransformCom->Get_Axis(AXIS_X);
	_v3 vLookY = m_pTransformCom->Get_Axis(AXIS_Y);
	_v3 vLookZ = m_pTransformCom->Get_Axis(AXIS_Z);

	_uint idx = 0;
	for (auto& iter : m_vecBloodCodeSlot)
	{
		TARGET_TO_TRANS(iter)->Set_Pos(m_vSlotPosition + *V3_NORMAL_SELF(&vLookX) * -0.25f + *V3_NORMAL_SELF(&vLookY) * 0.3f
		+ *V3_NORMAL_SELF(&vLookX) * _float(idx) * 0.3f + *V3_NORMAL_SELF(&vLookZ) * -0.001f);
		TARGET_TO_TRANS(iter)->Set_Angle(m_pTransformCom->Get_Angle());
		iter->Set_Active(m_bIsActive);
	
		++idx;
	}


	switch (m_eType)
	{
	case BloodCode_Fighter:
		m_iIndex = 0;
		break;
	case BloodCode_Caster:
		m_iIndex = 1;
		break;
	case BloodCode_Berserker:
		m_iIndex = 2;
		break;
	case BloodCode_Prometheus:
		m_iIndex = 3;
		break;
	case BloodCode_Eos:
		m_iIndex = 4;
		break;
	case BloodCode_End:
		m_iIndex = 5;
		break;
	}
	Compute_ViewZ(&m_pTransformCom->Get_Pos());

	Click_BloodCodeSlot();
	
	return NO_EVENT;
}

_int CBloodCodeSelectUI::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	m_matView = g_pManagement->Get_Transform(D3DTS_VIEW);
	m_matProj = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	return NO_EVENT;
}

HRESULT CBloodCodeSelectUI::Render_GameObject()
{
	if (!m_bIsActive)
		return NOERROR;
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable(m_iIndex)))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(1);

	m_pBufferCom->Render_VIBuffer();
	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	for (auto& iter : m_vecPhysicCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	for (auto& iter : m_vecAttackCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	return S_OK;
}

HRESULT CBloodCodeSelectUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_BloodCodeSelectUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	
	return NOERROR;
}

HRESULT CBloodCodeSelectUI::SetUp_ConstantTable(_uint iIndex)
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

void CBloodCodeSelectUI::Click_BloodCodeSlot()
{
	if (!m_bIsActive)
		return;

	for (auto& iter : m_vecBloodCodeSlot)
	{
		if (CCollisionMgr::Collision_Ray(iter, g_pInput_Device->Get_Ray(), &m_fCross))
		{
			iter->Set_Select(true);
			m_eType = iter->Get_Type();

			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
				CUI_Manager::Get_Instance()->Get_BloodCode_Menu()->Set_IsChoise(true);
		}
		else
		{
			iter->Set_Select(false);
		}
	}
}


BloodCode_ID CBloodCodeSelectUI::Get_Type()
{
	return m_eType;
}

void CBloodCodeSelectUI::SetUp_Default()
{
	CBloodCodeSlot* pInstance = nullptr;

	LOOP(BloodCode_End)
	{
		pInstance = static_cast<CBloodCodeSlot*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BloodCodeSlot", nullptr));
		pInstance->Set_Type(BloodCode_ID(i));
		TARGET_TO_TRANS(pInstance)->Set_Scale(_v3(0.3f, 0.223f, 1.f));
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_StageUI", nullptr);
		m_vecBloodCodeSlot.push_back(pInstance);
	}

	
}


CBloodCodeSelectUI * CBloodCodeSelectUI::Create(_Device pGraphic_Device)
{
	CBloodCodeSelectUI* pInstance = new CBloodCodeSelectUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CBloodCodeSelectUI::Clone_GameObject(void * pArg)
{
	CBloodCodeSelectUI* pInstance = new CBloodCodeSelectUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CBloodCodeSelectUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	

	CUI::Free();
}
