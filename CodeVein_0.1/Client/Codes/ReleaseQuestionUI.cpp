#include "stdafx.h"
#include "..\Headers\ReleaseQuestionUI.h"
#include "UI_Manager.h"
#include "CollisionMgr.h"

CReleaseQuestionUI::CReleaseQuestionUI(_Device pDevice)
	: CUI(pDevice)
{
}

CReleaseQuestionUI::CReleaseQuestionUI(const CReleaseQuestionUI & rhs)
	: CUI(rhs)
{
}

HRESULT CReleaseQuestionUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CReleaseQuestionUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	SetUp_Default();
	return NOERROR;
}

_int CReleaseQuestionUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	m_pRendererCom->Add_RenderList(RENDER_3DUI, this);

	_v3 vWorldPos;
	memcpy(vWorldPos, &m_pTransformCom->Get_WorldMat()._41, sizeof(_v3));
	Compute_ViewZ(&vWorldPos);

	if (m_bIsActive &&
		1.f > m_fAlpha)
		m_fAlpha += _float(TimeDelta) * 1.2f;
	
	if (!m_bIsActive &&
		0.f < m_fAlpha)
		m_fAlpha -= _float(TimeDelta) * 1.2f;

	_v3 vLookY = m_pTransformCom->Get_Axis(AXIS_Y);
	_v3 vLookZ = m_pTransformCom->Get_Axis(AXIS_Z);
	
	LOOP(2)
	{
		TARGET_TO_TRANS(m_vecOption[i])->Set_Pos(m_pTransformCom->Get_Pos() - *V3_NORMAL_SELF(&vLookY) * 0.15f * _float(i) + *V3_NORMAL_SELF(&vLookZ) * 0.001f);
		TARGET_TO_TRANS(m_vecOption[i])->Set_Angle(m_pTransformCom->Get_Angle());
		m_vecOption[i]->Set_Active(m_bIsActive);
	}

	Click_Option();

	return NO_EVENT;
}

_int CReleaseQuestionUI::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	m_matView = g_pManagement->Get_Transform(D3DTS_VIEW);
	m_matProj = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	return NO_EVENT;
}

HRESULT CReleaseQuestionUI::Render_GameObject()
{
	if (!m_bIsActive && 0.f >= m_fAlpha)
		return NOERROR;
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable(1)))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(3);

	m_pBufferCom->Render_VIBuffer();
	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return S_OK;
}

HRESULT CReleaseQuestionUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_ReleaseQuestionUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_3dUI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CReleaseQuestionUI::SetUp_ConstantTable(_uint iIndex)
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

void CReleaseQuestionUI::SetUp_Default()
{
	CReleaseOption* pInstance = nullptr;

	LOOP(2)
	{
		pInstance = static_cast<CReleaseOption*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ReleaseOption", nullptr));
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_MORTAL, L"Layer_StageUI", nullptr);
		TARGET_TO_TRANS(pInstance)->Set_Scale(_v3(1.2f, 0.3f, 1.f));
		pInstance->Set_UI_Index(i);
		m_vecOption.push_back(pInstance);
	}
}

void CReleaseQuestionUI::Click_Option()
{
	if (!m_bIsActive)
		return;
	LOOP(2)
	{
		if (CCollisionMgr::Collision_Ray(m_vecOption[i], g_pInput_Device->Get_Ray(), &m_fCross))
		{
			m_vecOption[i]->Set_Select(true);

			if ((0 == m_vecOption[i]->Get_UI_Index()) && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
			{
				// 습득한다
				m_bIsActive = false;
				CUI_Manager::Get_Instance()->Get_Skill_Inven()->Add_Skill_Data(m_eReleaseSkill);
				CUI_Manager::Get_Instance()->Get_Skill_AcquisitionUI()->Set_Skill(m_eReleaseSkill);
				CUI_Manager::Get_Instance()->Get_Skill_AcquisitionUI()->Set_Active(true);

				/*switch (m_eReleaseBloodCode)
				{
				case BloodCode_Fighter:
				{
					CUI_Manager::Get_Instance()->Get_FigherBlood()->Add_FighterBlood(m_eReleaseSkill);
				}
					break;
				case BloodCode_Prometheus:
				{
					CUI_Manager::Get_Instance()->Get_PrometheusBlood()->Add_PrometheusBlood(m_eReleaseSkill);
				}
					break;
				}*/
				
				m_bIsRelease = true;

				g_pSoundManager->Play_Sound(L"UI_SkillRelease.ogg", CSoundManager::CHANNELID::Skill_Release_OK, CSoundManager::Effect_Sound);
			}
			else if ((1 == m_vecOption[i]->Get_UI_Index()) && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
			{
				// 취소
				m_bIsActive = false;
				m_bIsRelease = false;

				g_pSoundManager->Play_Sound(L"UI_UpgradeFailed.wav", CSoundManager::CHANNELID::Skill_Release_Cancel, CSoundManager::Effect_Sound);
			}
		}
		else
			m_vecOption[i]->Set_Select(false);
	}
}

CReleaseQuestionUI * CReleaseQuestionUI::Create(_Device pGraphic_Device)
{
	CReleaseQuestionUI* pInstance = new CReleaseQuestionUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CReleaseQuestionUI::Clone_GameObject(void * pArg)
{
	CReleaseQuestionUI* pInstance = new CReleaseQuestionUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CReleaseQuestionUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
