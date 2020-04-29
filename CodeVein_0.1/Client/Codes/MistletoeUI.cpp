#include "stdafx.h"
#include "..\Headers\MistletoeUI.h"
#include "Player.h"
#include "MistletoeOptionUI.h"
#include "StageSelectUI.h"
#include "UI_Manager.h"

CMistletoeUI::CMistletoeUI(_Device pDevice)
	: CUI(pDevice)
{
}

CMistletoeUI::CMistletoeUI(const CMistletoeUI & rhs)
	: CUI(rhs)
{
}

HRESULT CMistletoeUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CMistletoeUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);
	m_bIsActive = false;
	m_fAlpha = 1.f;

	CMistletoeOptionUI* pInstance = nullptr;
	LOOP(3)
	{
		pInstance = static_cast<CMistletoeOptionUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_MistletoeOptionUI", nullptr));
		pInstance->Set_UI_Index(i + 1);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_StageUI", nullptr);
		m_vecOption.push_back(pInstance);
	}
	
	m_pStageSelectUI = CUI_Manager::Get_Instance()->Get_StageSelectUI();
	if (nullptr == m_pStageSelectUI)
		return E_FAIL;
	m_pTransformCom->Set_Scale(_v3(0.87f, 1.f, 0.f));
	return NOERROR;
}

_int CMistletoeUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	m_pRendererCom->Add_RenderList(RENDER_UI, this);
	

	m_pTarget = static_cast<CPlayer*>(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));
	if (nullptr == m_pTarget)
		return NO_EVENT;

	
	_v3 vLookZ = TARGET_TO_TRANS(m_pTarget)->Get_Axis(AXIS_Z);
	_v3 vLookX = TARGET_TO_TRANS(m_pTarget)->Get_Axis(AXIS_X);
	
	_v3 vDir = *V3_NORMAL_SELF(&(*V3_NORMAL_SELF(&vLookX)+ /**V3_NORMAL_SELF(&vLookZ) + */_v3(0.f, 1.5f, 0.f))) * 1.5f;
	_v3 vPosition = TARGET_TO_TRANS(m_pTarget)->Get_Pos() + vDir;
	m_pTransformCom->Set_Pos(vPosition);
	
	m_pTransformCom->Set_Angle(TARGET_TO_TRANS(m_pTarget)->Get_Angle());
	_v3 vLength = TARGET_TO_TRANS(m_pTarget)->Get_Pos() + vDir * 0.9f;

	
	LOOP(3)
	{
		TARGET_TO_TRANS(m_vecOption[i])->Set_Angle(m_pTransformCom->Get_Angle());
		TARGET_TO_TRANS(m_vecOption[i])->Set_Scale(_v3(0.87f, 0.1476f, 0.f));
		TARGET_TO_TRANS(m_vecOption[i])->Set_At(m_pTransformCom->Get_At());
		TARGET_TO_TRANS(m_vecOption[i])->Set_Pos(m_pTransformCom->Get_Pos() + _v3(0.f, _float(i) * -0.2f + 0.2f, 0.f) + *V3_NORMAL_SELF(&vLookZ) * -0.1f);
		
		m_vecOption[i]->Set_Active(m_bIsActive);

		(i == m_iSelectIndex) ? (m_vecOption[i]->Set_Select(true)) : (m_vecOption[i]->Set_Select(false));
	}

	

	if (!m_bIsActive)
	{
		m_pStageSelectUI->Set_Active(false);
	}
	
	Compute_ViewZ(&m_pTransformCom->Get_Pos());
	return NO_EVENT;
}

_int CMistletoeUI::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	m_matView = g_pManagement->Get_Transform(D3DTS_VIEW);
	m_matProj = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	return NO_EVENT;
}

HRESULT CMistletoeUI::Render_GameObject()
{
	if (!m_bIsActive)
		return NOERROR;
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(1);

	m_pBufferCom->Render_VIBuffer();
	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CMistletoeUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_MistletoeUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CMistletoeUI::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 0)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;
	return NOERROR;
}

void CMistletoeUI::Active_SubUI()
{
	if (!m_bIsActive)
		return;
	//if (g_pInput_Device->Key_Up(DIK_P) && m_bIsActive)
	//{
	//	switch (m_iSelectIndex)
	//	{
	//	case 0:
	//		m_pStageSelectUI->Set_Active(!m_pStageSelectUI->Get_Active());
	//		break;
	//	}
	//
	//}

	if (0 == m_iSelectIndex)
		m_pStageSelectUI->Set_Active(!m_pStageSelectUI->Get_Active());
	else
		m_pStageSelectUI->Set_Active(false);

	if (1 == m_iSelectIndex)
		CUI_Manager::Get_Instance()->Get_BloodCode_Menu()->Set_Active(!CUI_Manager::Get_Instance()->Get_BloodCode_Menu()->Get_Active());
	else
		CUI_Manager::Get_Instance()->Get_BloodCode_Menu()->Set_Active(false);
}

void CMistletoeUI::Move_Up()
{
	if (!m_bIsActive)
		return;
	if (m_iSelectIndex > 0)
		m_iSelectIndex--;
}

void CMistletoeUI::Move_Down()
{
	if (!m_bIsActive)
		return;
	if (m_iSelectIndex < m_vecOption.size() - 1)
			m_iSelectIndex++;
}

CMistletoeUI * CMistletoeUI::Create(_Device pGraphic_Device)
{
	CMistletoeUI* pInstance = new CMistletoeUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CMistletoeUI::Clone_GameObject(void * pArg)
{
	CMistletoeUI* pInstance = new CMistletoeUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CMistletoeUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
