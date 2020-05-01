#include "stdafx.h"
#include "..\Headers\SkillReleaseUI.h"
#include "UI_Manager.h"
#include "CollisionMgr.h"

CSkillReleaseUI::CSkillReleaseUI(_Device pDevice)
	: CUI(pDevice)
{
	ZeroMemory(&m_vSlotPosition, sizeof(_v3));
}

CSkillReleaseUI::CSkillReleaseUI(const CSkillReleaseUI & rhs)
	: CUI(rhs)
{
	ZeroMemory(&m_vSlotPosition, sizeof(_v3));
}

HRESULT CSkillReleaseUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CSkillReleaseUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	SetUp_Default();

	return NOERROR;
}

_int CSkillReleaseUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	m_pRendererCom->Add_RenderList(RENDER_3DUI, this);

	

	_v3 vLookX = m_pTransformCom->Get_Axis(AXIS_X);
	_v3 vLookY = m_pTransformCom->Get_Axis(AXIS_Y);
	_v3 vLookZ = m_pTransformCom->Get_Axis(AXIS_Z);

	_uint idx = 0;
	for (auto& iter : m_vecSkillSlot)
	{
		TARGET_TO_TRANS(iter)->Set_Pos(m_pTransformCom->Get_Pos() + *V3_NORMAL_SELF(&vLookX) * -0.25f + *V3_NORMAL_SELF(&vLookY) * 0.3f
			+ *V3_NORMAL_SELF(&vLookX) * _float(idx) * 0.5f + *V3_NORMAL_SELF(&vLookZ) * -0.001f);
		TARGET_TO_TRANS(iter)->Set_Angle(m_pTransformCom->Get_Angle());
		iter->Set_Active(m_bIsActive);
		++idx;
	}
	TARGET_TO_TRANS(m_pQuestionUI)->Set_Pos(m_pTransformCom->Get_Pos() + *V3_NORMAL_SELF(&vLookY) * -0.3f);
	TARGET_TO_TRANS(m_pQuestionUI)->Set_Angle(m_pTransformCom->Get_Angle());
	
	switch (m_eBloodCodeType)
	{
	case BloodCode_Fighter:
	{
		m_vecSkillSlot[0]->Set_SkillIndex(Skill_OneHand_Active_01);
		m_vecSkillSlot[1]->Set_SkillIndex(Skill_OneHand_Active_02);
		m_iIndex = 0;
	}
		break;
	case BloodCode_Caster:
	{
		m_vecSkillSlot[0]->Set_SkillIndex(Skill_OneHand_Active_03);
		m_vecSkillSlot[1]->Set_SkillIndex(Skill_OneHand_Active_04);
		m_iIndex = 1;
	}
		break;
	case BloodCode_Berserker:
	{
		m_vecSkillSlot[0]->Set_SkillIndex(Skill_TwoHand_Active_01);
		m_vecSkillSlot[1]->Set_SkillIndex(Skill_TwoHand_Active_02);
		m_iIndex = 2;
	}
		break;
	case BloodCode_Prometheus:
	{
		m_vecSkillSlot[0]->Set_SkillIndex(Skill_TwoHand_Active_03);
		m_vecSkillSlot[1]->Set_SkillIndex(Skill_TwoHand_Active_04);
		m_iIndex = 3;
	}
		break;
	case BloodCode_Eos:
	{
		m_vecSkillSlot[0]->Set_SkillIndex(Skill_Halverd_Single);
		m_vecSkillSlot[1]->Set_SkillIndex(Skill_Gun_Single);
		m_iIndex = 4;
	}
		break;
	}

	_v3 vWorldPos;
	memcpy(vWorldPos, &m_pTransformCom->Get_WorldMat()._41, sizeof(_v3));
	Compute_ViewZ(&vWorldPos);


	Click_SkillSlot();

	if (m_bIsActive && 
		1.f > m_fAlpha)
	{
		m_fAlpha += _float(TimeDelta) * 1.f;
	}
	if (!m_bIsActive &&
		0.f < m_fAlpha)
	{
		m_fAlpha -= _float(TimeDelta) * 1.f;
	}

	return NO_EVENT;
}

_int CSkillReleaseUI::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	m_matView = g_pManagement->Get_Transform(D3DTS_VIEW);
	m_matProj = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	return NO_EVENT;
}

HRESULT CSkillReleaseUI::Render_GameObject()
{
	if (!m_bIsActive || 0.f >= m_fAlpha)
		return NOERROR;
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable(m_iIndex)))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(3);

	m_pBufferCom->Render_VIBuffer();
	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return S_OK;
}

HRESULT CSkillReleaseUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_SkillReleaseUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_3dUI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CSkillReleaseUI::SetUp_ConstantTable(_uint iIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, _uint(iIndex))))
		return E_FAIL;

	m_pShaderCom->Set_Texture("g_DepthTexture", g_pManagement->Get_Target_Texture(L"Target_DepthUI"));
	return NOERROR;
}

void CSkillReleaseUI::SetUp_Default()
{
	CBloodSkillSlot* pInstance = nullptr;

	LOOP(2)
	{
		pInstance = static_cast<CBloodSkillSlot*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BloodSkillSlot", nullptr));
		TARGET_TO_TRANS(pInstance)->Set_Scale(_v3(0.3f, 0.3f, 1.5f));
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_StageUI", nullptr);
		m_vecSkillSlot.push_back(pInstance);
	}

	m_pQuestionUI = static_cast<CReleaseQuestionUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ReleaseQuestionUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pQuestionUI, SCENE_STAGE, L"Layer_StageUI", nullptr);
	TARGET_TO_TRANS(m_pQuestionUI)->Set_Scale(_v3(2.f, 1.f, 1.6f));
}

void CSkillReleaseUI::Click_SkillSlot()
{
	if (!m_bIsActive)
		return;

	for (auto& iter : m_vecSkillSlot)
	{
		if (CCollisionMgr::Collision_Ray(iter, g_pInput_Device->Get_Ray(), &m_fCross))
		{
			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
			{
				Reset_Select();
				iter->Set_Select(true);
				m_pQuestionUI->Set_Active(true);
			}
				
		}
		
	}
}

void CSkillReleaseUI::Reset_Select()
{
	for (auto& iter : m_vecSkillSlot)
		iter->Set_Select(false);
}

CSkillReleaseUI * CSkillReleaseUI::Create(_Device pGraphic_Device)
{
	CSkillReleaseUI* pInstance = new CSkillReleaseUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CSkillReleaseUI::Clone_GameObject(void * pArg)
{
	CSkillReleaseUI* pInstance = new CSkillReleaseUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CSkillReleaseUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
