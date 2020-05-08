#include "stdafx.h"
#include "..\Headers\BloodSkillSlot.h"
#include "UI_Manager.h"

CBloodSkillSlot::CBloodSkillSlot(_Device pDevice)
	: CUI(pDevice)
{
}

CBloodSkillSlot::CBloodSkillSlot(const CBloodSkillSlot & rhs)
	: CUI(rhs)
{
}

HRESULT CBloodSkillSlot::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CBloodSkillSlot::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	SetUp_Default();
	return NOERROR;
}

_int CBloodSkillSlot::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	m_pRendererCom->Add_RenderList(RENDER_3DUI, this);

	m_pCollider->Update(m_pTransformCom->Get_Pos());

	m_fSpeed += _float(TimeDelta) * 1.f;

	switch (m_eSkillID)
	{
	case Dragon_Lunge:
		m_iIndex = 1;
		break;
	case Vanishing_Hollow:
		m_iIndex = 2;
		break;
	case Circulating_Pulse:
		m_iIndex = 3;
		break;
	case Triple_Annihilator:
		m_iIndex = 4;
		break;
	case Shadow_Assault:
		m_iIndex = 5;
		break;
	case Severing_Abyss:
		m_iIndex = 6;
		break;
	case Swallow_Cutter:
		m_iIndex = 7;
		break;
	case Tormenting_Blast:
		m_iIndex = 8;
		break;
	case Phantom_Assault:
		m_iIndex = 9;
		break;
	case Legion_Punisher:
		m_iIndex = 10;
		break;
	case Chariot_Rush:
		m_iIndex = 11;
		break;
	case Fusillade_Rondo:
		m_iIndex = 12;
		break;
	case SkillID_End:
		break;
	}

	_v3 vLookZ = m_pTransformCom->Get_Axis(AXIS_Z);
	
	if (m_eSkillID == SkillID_End)
		m_pCursor->Set_Active(false);
	else
		m_pCursor->Set_Active(m_bIsActive && m_bIsSelect);

	TARGET_TO_TRANS(m_pCursor)->Set_Pos(m_pTransformCom->Get_Pos()/* + *V3_NORMAL_SELF(&vLookZ) * -0.01f*/);
	TARGET_TO_TRANS(m_pCursor)->Set_Angle(m_pTransformCom->Get_Angle());

	_v3 vWorldPos;
	memcpy(vWorldPos, &m_pTransformCom->Get_WorldMat()._41, sizeof(_v3));
	Compute_ViewZ(&vWorldPos);

	return NO_EVENT;
}

_int CBloodSkillSlot::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	m_matView = g_pManagement->Get_Transform(D3DTS_VIEW);
	m_matProj = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	return NO_EVENT;
}

HRESULT CBloodSkillSlot::Render_GameObject()
{
	if (!m_bIsActive || SkillID_End == m_eSkillID)
		return NOERROR;
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	_uint iIndex = 0;
	_uint iPass = 0;
	
	if (!m_bIsRelease)
	{
		//LOOP(2)
		{
			/*(0 == i) ? (iIndex = 0) && (iPass = 1) : */(iIndex = m_iIndex) && (iPass = 5);

			if (FAILED(SetUp_ConstantTable(iIndex)))
				return E_FAIL;

			m_pShaderCom->Begin_Shader();
			m_pShaderCom->Begin_Pass(iPass);

			m_pBufferCom->Render_VIBuffer();
			m_pShaderCom->End_Pass();
			m_pShaderCom->End_Shader();
		}
	}
	else
	{
		//LOOP(2)
		{
			/*(0 == i) ? (iIndex = 0) && (iPass = 1) : */(iIndex = m_iIndex) && (iPass = 1);

			if (FAILED(SetUp_ConstantTable(iIndex)))
				return E_FAIL;

			m_pShaderCom->Begin_Shader();
			m_pShaderCom->Begin_Pass(iPass);

			m_pBufferCom->Render_VIBuffer();
			m_pShaderCom->End_Pass();
			m_pShaderCom->End_Shader();
		}
	}
	
	
	return S_OK;
}

HRESULT CBloodSkillSlot::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_BloodSkillSlot", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_3dUI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	// for.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;
	m_pCollider->Set_Radius(_v3{ 0.15f, 0.15f, 0.15f });
	m_pCollider->Set_Dynamic(true);
	m_pCollider->Set_Type(COL_SPHERE);
	m_pCollider->Set_CenterPos(m_pTransformCom->Get_Pos());

	return NOERROR;
}

HRESULT CBloodSkillSlot::SetUp_ConstantTable(_uint iIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	
	/*if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, iIndex)))
		return E_FAIL;*/
	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, _uint(iIndex))))
		return E_FAIL;

	m_pShaderCom->Set_Texture("g_DepthTexture", g_pManagement->Get_Target_Texture(L"Target_DepthUI"));

	return NOERROR;
}

void CBloodSkillSlot::SetUp_Default()
{
	m_pCursor = static_cast<CBloodSkillCursor*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BloodSkillCursor", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pCursor, SCENE_STAGE, L"Layer_StageUI", nullptr);
	TARGET_TO_TRANS(m_pCursor)->Set_Scale(_v3(0.5f, 0.5f, 1.f));

}

CBloodSkillSlot * CBloodSkillSlot::Create(_Device pGraphic_Device)
{
	CBloodSkillSlot* pInstance = new CBloodSkillSlot(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CBloodSkillSlot::Clone_GameObject(void * pArg)
{
	CBloodSkillSlot* pInstance = new CBloodSkillSlot(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CBloodSkillSlot::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pCollider);

	CUI::Free();
}
