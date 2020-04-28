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
	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	m_pCollider->Update(m_pTransformCom->Get_Pos());

	m_fSpeed += _float(TimeDelta) * 1.f;

	switch (m_eSkillIndex)
	{
	case Skill_OneHand_Active_01:
		m_iIndex = 1;
		break;
	case Skill_OneHand_Active_02:
		m_iIndex = 2;
		break;
	case Skill_OneHand_Active_03:
		m_iIndex = 3;
		break;
	case Skill_OneHand_Active_04:
		m_iIndex = 4;
		break;
	case Skill_TwoHand_Active_01:
		m_iIndex = 5;
		break;
	case Skill_TwoHand_Active_02:
		m_iIndex = 6;
		break;
	case Skill_TwoHand_Active_03:
		m_iIndex = 7;
		break;
	case Skill_TwoHand_Active_04:
		m_iIndex = 8;
		break;
	case Skill_Halverd_Single:
		m_iIndex = 9;
		break;
	case Skill_Gun_Single:
		m_iIndex = 10;
		break;
	case Skill_Buff_Enchant_01:
		m_iIndex = 11;
		break;
	case Skill_Buff_Enchant_02:
		m_iIndex = 12;
		break;
	case Skill_Buff_Enchant_03:
		m_iIndex = 13;
		break;
	case Skill_End:
		m_iIndex = 0;
		break;
	}
	Compute_ViewZ(&m_pTransformCom->Get_Pos());

	m_pCursor->Set_Active(m_bIsActive && m_bIsSelect);
	TARGET_TO_TRANS(m_pCursor)->Set_Pos(m_pTransformCom->Get_Pos());
	TARGET_TO_TRANS(m_pCursor)->Set_Angle(m_pTransformCom->Get_Angle());
	
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
	if (!m_bIsActive)
		return NOERROR;
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	_uint iIndex = 0;
	_uint iPass = 0;
	
		
	iIndex = m_iIndex;
	iPass = 1;
	
	if (FAILED(SetUp_ConstantTable(iIndex)))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(iPass);

	m_pBufferCom->Render_VIBuffer();
	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
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

void CBloodSkillSlot::SetUp_Default()
{
	m_pCursor = static_cast<CBloodSkillCursor*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BloodSkillCursor", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pCursor, SCENE_STAGE, L"Layer_StageUI", nullptr);
	TARGET_TO_TRANS(m_pCursor)->Set_Scale(_v3(0.5f, 0.5f, 0.5f));
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
