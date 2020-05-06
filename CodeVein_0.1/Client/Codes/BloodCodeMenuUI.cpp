#include "stdafx.h"
#include "..\Headers\BloodCodeMenuUI.h"
#include "Player.h"
#include "UI_Manager.h"
#include "CollisionMgr.h"

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

	//SetUp_Default();

	m_pTransformCom->Set_Scale(_v3(3.555555f, 2.f, 1.f));

	return NOERROR;
}

_int CBloodCodeMenuUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_ALPHA, this);

	_v3 vWorldPos;
	memcpy(vWorldPos, &m_pTransformCom->Get_WorldMat()._41, sizeof(_v3));
	Compute_ViewZ(&vWorldPos);
	
	m_pTarget = static_cast<CPlayer*>(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));
	if (nullptr == m_pTarget)
		return NO_EVENT;
	
	

	m_pTransformCom->Set_Angle(TARGET_TO_TRANS(m_pTarget)->Get_Angle());
	_v3 vLookX = TARGET_TO_TRANS(m_pTarget)->Get_Axis(AXIS_X);
	_v3 vLookY = TARGET_TO_TRANS(m_pTarget)->Get_Axis(AXIS_Y);
	_v3 vLookZ = TARGET_TO_TRANS(m_pTarget)->Get_Axis(AXIS_Z);
	
	_v3 vDir = *V3_NORMAL_SELF(&(-*V3_NORMAL_SELF(&vLookX) + *V3_NORMAL_SELF(&vLookZ) + _v3(0.f, 2.f, 0.f))) * 2.f;
	_v3 vPosition = TARGET_TO_TRANS(m_pTarget)->Get_Pos() + vDir;
	m_pTransformCom->Set_Pos(vPosition);
	

	if (!m_bIsActive) // 비활성화
	{
		CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI()->Set_Active(false);
		CUI_Manager::Get_Instance()->Get_Skill_ReleaseUI()->Set_Active(false);
		CUI_Manager::Get_Instance()->Get_BloodCode_Owner()->Set_CodeID(BloodCode_End);
		CUI_Manager::Get_Instance()->Get_BloodCode_Owner()->Set_Select(false);
		CUI_Manager::Get_Instance()->Get_Skill_AcquisitionUI()->Set_Active(false);
		m_bIsChoiseBloodCode = false;
	}
	else // 활성화
	{
		if (!m_bIsChoiseBloodCode)
		{
			CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI()->Set_Active(true);
			CUI_Manager::Get_Instance()->Get_Skill_ReleaseUI()->Set_Active(false);
			CUI_Manager::Get_Instance()->Get_BloodCode_Owner()->Set_Select(false);
		}
		else if(m_bIsChoiseBloodCode)
		{
			CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI()->Set_Active(false);
			CUI_Manager::Get_Instance()->Get_Skill_ReleaseUI()->Set_Active(true);
			CUI_Manager::Get_Instance()->Get_BloodCode_Owner()->Set_Select(true);
		}
	}
	
	TARGET_TO_TRANS(CUI_Manager::Get_Instance()->Get_BloodCode_Owner())->Set_Pos(m_pTransformCom->Get_Pos()
		+ *V3_NORMAL_SELF(&vLookX) * -m_pTransformCom->Get_Size().x * 0.23f + *V3_NORMAL_SELF(&vLookZ) * -0.01f);
	TARGET_TO_TRANS(CUI_Manager::Get_Instance()->Get_BloodCode_Owner())->Set_Angle(m_pTransformCom->Get_Angle());
	TARGET_TO_TRANS(CUI_Manager::Get_Instance()->Get_BloodCode_Owner())->Set_Scale(_v3(1.894f, 2.f, 1.f));
	CUI_Manager::Get_Instance()->Get_BloodCode_Owner()->Set_Active(m_bIsActive);

	TARGET_TO_TRANS(CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI())->Set_Pos(m_pTransformCom->Get_Pos() + *V3_NORMAL_SELF(&vLookZ) * -0.02f);
	TARGET_TO_TRANS(CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI())->Set_Angle(m_pTransformCom->Get_Angle());
	TARGET_TO_TRANS(CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI())->Set_Scale(_v3(3.555555f, 2.f, 1.f));

	TARGET_TO_TRANS(CUI_Manager::Get_Instance()->Get_Skill_ReleaseUI())->Set_Pos(m_pTransformCom->Get_Pos() + *V3_NORMAL_SELF(&vLookZ) * -0.02f);
	TARGET_TO_TRANS(CUI_Manager::Get_Instance()->Get_Skill_ReleaseUI())->Set_Angle(m_pTransformCom->Get_Angle());
	TARGET_TO_TRANS(CUI_Manager::Get_Instance()->Get_Skill_ReleaseUI())->Set_Scale(_v3(3.555555f, 2.f, 1.f));

	TARGET_TO_TRANS(CUI_Manager::Get_Instance()->Get_Skill_AcquisitionUI())->Set_Pos(m_pTransformCom->Get_Pos() + *V3_NORMAL_SELF(&vLookZ) * -0.02f);
	TARGET_TO_TRANS(CUI_Manager::Get_Instance()->Get_Skill_AcquisitionUI())->Set_Angle(m_pTransformCom->Get_Angle());
	TARGET_TO_TRANS(CUI_Manager::Get_Instance()->Get_Skill_AcquisitionUI())->Set_Scale(_v3(3.555555f, 2.f, 1.f));
	
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_3dUI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
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

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, _uint(iIndex))))
		return E_FAIL;

	//m_pShaderCom->Set_Texture("g_DepthTexture", g_pManagement->Get_Target_Texture(L"Target_DepthUI"));
	return NOERROR;
}

void CBloodCodeMenuUI::SetUp_Default()
{
	/*m_pBloodCodeSelectUI = static_cast<CBloodCodeSelectUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BloodCodeSelectUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBloodCodeSelectUI, SCENE_STAGE, L"Layer_StageUI", nullptr);
	TARGET_TO_TRANS(m_pBloodCodeSelectUI)->Set_Scale(_v3(3.555555f, 2.f, 2.f));
	if (nullptr == m_pBloodCodeSelectUI)
	{
		MSG_BOX("SelectUI is null");
		return;
	}

	m_pSkillReleaseUI = static_cast<CSkillReleaseUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_SkillReleaseUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pSkillReleaseUI, SCENE_STAGE, L"Layer_StageUI", nullptr);
	TARGET_TO_TRANS(m_pSkillReleaseUI)->Set_Scale(_v3(3.555555f, 2.f, 3.f));
	if (nullptr == m_pSkillReleaseUI)
	{
		MSG_BOX("m_pSkillReleaseUI is null");
		return;
	}

	m_pBloodCodeOwner = static_cast<CBloodCodeOwner*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BloodCodeOwner", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBloodCodeOwner, SCENE_STAGE, L"Layer_StageUI", nullptr);
	TARGET_TO_TRANS(m_pBloodCodeOwner)->Set_Scale(_v3(1.894f, 2.f, 1.f));
	if (nullptr == m_pBloodCodeOwner)
	{
		MSG_BOX("m_pBloodCodeOwner is null");
		return;
	}*/
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
