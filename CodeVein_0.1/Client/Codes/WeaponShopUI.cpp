#include "stdafx.h"
#include "..\Headers\WeaponShopUI.h"
#include "Player.h"
#include "UI_Manager.h"
#include "CollisionMgr.h"

CWeaponShopUI::CWeaponShopUI(_Device pDevice)
	: CUI(pDevice)
{
}

CWeaponShopUI::CWeaponShopUI(const CWeaponShopUI & rhs)
	: CUI(rhs)
{
}

HRESULT CWeaponShopUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CWeaponShopUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);
	m_bIsActive = false;
	m_fAlpha = 1.f;

	CMistletoeOptionUI* pInstance = nullptr;
	//LOOP(3)
	//{
	//	pInstance = static_cast<CMistletoeOptionUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_MistletoeOptionUI", nullptr));
	//	pInstance->Set_Option(CMistletoeOptionUI::MISTLETOE_OPTION(i));
	//	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	//	m_vecOption.push_back(pInstance);
	//}
	//
	//m_pStageSelectUI = CUI_Manager::Get_Instance()->Get_StageSelectUI();
	//if (nullptr == m_pStageSelectUI)
	//	return E_FAIL;

	m_pTransformCom->Set_Scale(_v3(1.f, 1.3546f, 1.f));
	m_pTransformCom->Set_Angle(V3_NULL);
	m_pTransformCom->Set_Angle(AXIS_Y, D3DXToRadian(20.f));

	return NOERROR;
}

_int CWeaponShopUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	m_pRendererCom->Add_RenderList(RENDER_ALPHA, this);

	m_pTarget = static_cast<CPlayer*>(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));
	if (nullptr == m_pTarget)
		return NO_EVENT;

	CTransform* pTargetTrans = TARGET_TO_TRANS(m_pTarget);
	
	_v3 vLook = pTargetTrans->Get_Axis(AXIS_Z);
	_v3 vRight = pTargetTrans->Get_Axis(AXIS_X);
	
	// x 축 방향벡터
	V3_NORMAL_SELF(&vLook);
	V3_NORMAL_SELF(&vRight);

	// x 축 방향벡터 + 플레이어
	_v3 vPosition = pTargetTrans->Get_Pos() + vLook + vRight + (WORLD_UP * 1.5f);
	m_pTransformCom->Set_Pos(vPosition);
	
	_float fOriginYRadian = pTargetTrans->Get_Angle().y;
	_float fAdditionalYRadian = fOriginYRadian + D3DXToRadian(20.f);
	m_pTransformCom->Set_Angle(AXIS_Y, fAdditionalYRadian);

	
	//LOOP(3)
	//{
	//	TARGET_TO_TRANS(m_vecOption[i])->Set_Angle(m_pTransformCom->Get_Angle());
	//	TARGET_TO_TRANS(m_vecOption[i])->Set_Scale(_v3(0.87f, 0.1476f, 2.f));
	//	TARGET_TO_TRANS(m_vecOption[i])->Set_At(m_pTransformCom->Get_At());
	//	TARGET_TO_TRANS(m_vecOption[i])->Set_Pos(m_pTransformCom->Get_Pos() + _v3(0.f, _float(i) * -0.2f + 0.2f, 0.f) + *V3_NORMAL_SELF(&vLook) * -0.001f);
	//	
	//	m_vecOption[i]->Set_Active(m_bIsActive);
	//
	//}

	

	//if (!m_bIsActive)
	//{
	//	m_pStageSelectUI->Set_Active(false);
	//	CUI_Manager::Get_Instance()->Get_BloodCode_Menu()->Set_Active(false);
	//}
	
	_v3 vWorldPos;
	memcpy(vWorldPos, &m_pTransformCom->Get_WorldMat()._41, sizeof(_v3));
	Compute_ViewZ(&vWorldPos);

	Click_Option();
	return NO_EVENT;
}

_int CWeaponShopUI::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	m_matView = g_pManagement->Get_Transform(D3DTS_VIEW);
	m_matProj = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	return NO_EVENT;
}

HRESULT CWeaponShopUI::Render_GameObject()
{
	if (!m_bIsActive)
		return NOERROR;
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(6);

	m_pBufferCom->Render_VIBuffer();
	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CWeaponShopUI::Add_Component()
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_3dUI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CWeaponShopUI::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, _uint(0))))
		return E_FAIL;

	m_pShaderCom->Set_Texture("g_DepthTexture", g_pManagement->Get_Target_Texture(L"Target_DepthUI"));
	return NOERROR;
}

void CWeaponShopUI::Click_Option()
{
	if (!m_bIsActive)
		return;

	_int iIdx = 0;

	//for (auto& iter : m_vecOption)
	//{
	//	if (CCollisionMgr::Collision_Ray(iter, g_pInput_Device->Get_Ray(), &m_fCross))
	//	{
	//		Reset_Option();
	//		iter->Set_Select(true);
	//
	//		if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	//		{
	//			// 0번은 스테이지
	//			if (0 == iIdx)
	//			{
	//				static_cast<CPlayer*>(m_pTarget)->Active_UI_StageSelect(false);
	//			}
	//
	//			// 1번은 블러드 코드
	//			else if (1 == iIdx)
	//			{
	//				static_cast<CPlayer*>(m_pTarget)->Active_UI_BloodCode(false);
	//			}
	//
	//			// 2번은 레벨업
	//			else if (2 == iIdx)
	//			{
	//
	//			}
	//
	//			//Active_SubUI(iter);
	//		}
	//			
	//	}
	//	else
	//	{
	//		iter->Set_Select(false);
	//	}
	//
	//	++iIdx;
	//}
}

void CWeaponShopUI::Reset_Option()
{
	//for (auto& iter : m_vecOption)
	//{
	//		iter->Set_Select(false);
	//}
}

//void CWeaponShopUI::Active_SubUI(CMistletoeOptionUI* pSelectOption)
//{
//	if (!m_bIsActive ||
//		nullptr == pSelectOption)
//		return;
//
//	switch (pSelectOption->Get_Option())
//	{
//	case CMistletoeOptionUI::OPTION_STAGE: // 스테이지 이동
//		CUI_Manager::Get_Instance()->Get_StageSelectUI()->Set_Active(!CUI_Manager::Get_Instance()->Get_StageSelectUI()->Get_Active());
//		break;
//	case CMistletoeOptionUI::OPTION_INHERIT: // 연혈, 스킬
//		CUI_Manager::Get_Instance()->Get_BloodCode_Menu()->Set_Active(!CUI_Manager::Get_Instance()->Get_BloodCode_Menu()->Get_Active());
//		break;
//	case CMistletoeOptionUI::OPTION_LEVELUP: // 레벨 업
//		break;
//	}
//}

CWeaponShopUI * CWeaponShopUI::Create(_Device pGraphic_Device)
{
	CWeaponShopUI* pInstance = new CWeaponShopUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CWeaponShopUI::Clone_GameObject(void * pArg)
{
	CWeaponShopUI* pInstance = new CWeaponShopUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CWeaponShopUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
