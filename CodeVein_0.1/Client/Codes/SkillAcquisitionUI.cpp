#include "stdafx.h"
#include "..\Headers\SkillAcquisitionUI.h"
#include "UI_Manager.h"
#include "CollisionMgr.h"

CSkillAcquisitionUI::CSkillAcquisitionUI(_Device pDevice)
	: CUI(pDevice)
{
}

CSkillAcquisitionUI::CSkillAcquisitionUI(const CSkillAcquisitionUI & rhs)
	: CUI(rhs)
{
}

HRESULT CSkillAcquisitionUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CSkillAcquisitionUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	SetUp_Default();

	return NOERROR;
}

_int CSkillAcquisitionUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	m_pRendererCom->Add_RenderList(RENDER_3DUI, this);

	_v3 vWorldPos;
	memcpy(vWorldPos, &m_pTransformCom->Get_WorldMat()._41, sizeof(_v3));
	Compute_ViewZ(&vWorldPos);

	if (m_bIsActive &&
		1.f > m_fAlpha)
		m_fAlpha += _float(TimeDelta) * 2.f;

	if (!m_bIsActive &&
		0.f < m_fAlpha)
		m_fAlpha -= _float(TimeDelta) * 2.f;

	_v3 vAxisY = m_pTransformCom->Get_Axis(AXIS_Y);
	_v3 vAxisZ = m_pTransformCom->Get_Axis(AXIS_Z);
	TARGET_TO_TRANS(m_pOKMsg)->Set_Pos(m_pTransformCom->Get_Pos() + *V3_NORMAL_SELF(&vAxisZ) * -0.001f + *V3_NORMAL_SELF(&vAxisY) * -0.2f);
	TARGET_TO_TRANS(m_pOKMsg)->Set_Angle(m_pTransformCom->Get_Angle());
	m_pOKMsg->Set_Active(m_bIsActive);

	Click_OKMsgUI();

	switch (m_eSkillType)
	{
	case Dragon_Lunge:
		m_iIndex = 0;
		break;
	case Vanishing_Hollow:
		m_iIndex = 1;
		break;
	case Circulating_Pulse:
		m_iIndex = 2;
		break;
	case Triple_Annihilator:
		m_iIndex = 3;
		break;
	case Shadow_Assault:
		m_iIndex = 4;
		break;
	case Severing_Abyss:
		m_iIndex = 5;
		break;
	case Swallow_Cutter:
		m_iIndex = 6;
		break;
	case Tormenting_Blast:
		m_iIndex = 7;
		break;
	case Phantom_Assault:
		m_iIndex = 8;
		break;
	case Legion_Punisher:
		m_iIndex = 9;
		break;
	}

	return NO_EVENT;
}

_int CSkillAcquisitionUI::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	m_matView = g_pManagement->Get_Transform(D3DTS_VIEW);
	m_matProj = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	return NO_EVENT;
}

HRESULT CSkillAcquisitionUI::Render_GameObject()
{
	if (!m_bIsActive && 0.f >= m_fAlpha)
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
	
	return NOERROR;
}

HRESULT CSkillAcquisitionUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_SkillAcquisitionUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_3dUI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CSkillAcquisitionUI::SetUp_ConstantTable(_uint iIndex)
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

	return NOERROR;
}

void CSkillAcquisitionUI::SetUp_Default()
{
	m_pOKMsg = static_cast<COKMessageUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_OKMessageUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pOKMsg, SCENE_MORTAL, L"Layer_StageUI", nullptr);
	TARGET_TO_TRANS(m_pOKMsg)->Set_Scale(_v3(0.6f, 0.3f, 1.f));
}

void CSkillAcquisitionUI::Click_OKMsgUI()
{
	if (!m_bIsActive)
		return;

	if (CCollisionMgr::Collision_Ray(m_pOKMsg, g_pInput_Device->Get_Ray(), &m_fCross))
	{
		if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
		{
			m_bIsActive = false;
		}
	}
}

CSkillAcquisitionUI * CSkillAcquisitionUI::Create(_Device pGraphic_Device)
{
	CSkillAcquisitionUI* pInstance = new CSkillAcquisitionUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CSkillAcquisitionUI::Clone_GameObject(void * pArg)
{
	CSkillAcquisitionUI* pInstance = new CSkillAcquisitionUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CSkillAcquisitionUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
