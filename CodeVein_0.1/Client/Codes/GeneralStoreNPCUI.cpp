#include "stdafx.h"
#include "..\Headers\GeneralStoreNPCUI.h"
#include "UI_Manager.h"
#include "Player.h"
#include "CollisionMgr.h"

CGeneralStoreNPCUI::CGeneralStoreNPCUI(_Device pDevice)
	: CUI(pDevice)
{
}

CGeneralStoreNPCUI::CGeneralStoreNPCUI(const CGeneralStoreNPCUI & rhs)
	: CUI(rhs)
{
}

HRESULT CGeneralStoreNPCUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CGeneralStoreNPCUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_pTransformCom->Set_Scale(_v3(1.5f, 2.246f, 1.f));
	m_pTransformCom->Set_Angle(V3_NULL);
	m_pTransformCom->Set_Angle(AXIS_Y, D3DXToRadian(20.f));

	SetUp_Default();

	
	
	return NOERROR;
}

_int CGeneralStoreNPCUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI_BACK, this);

	_v3 vWorldPos;
	memcpy(vWorldPos, &m_pTransformCom->Get_WorldMat()._41, sizeof(_v3));
	Compute_ViewZ(&vWorldPos);


	if (nullptr == m_pTarget)
		return NO_EVENT;

	if (m_bIsActive && !m_bIsSubActive)
	{
		for (auto& iter : m_vecOption)
		{
			iter->Set_Active(true);
			m_bIsSubActive = true;
		}

		CUI_Manager::Get_Instance()->Get_NPC_InteractionUI()->Set_Active(false);
	}
	else if (!m_bIsActive && m_bIsSubActive)
	{
		for (auto& iter : m_vecOption)
		{
			iter->Set_Active(false);
			m_bIsSubActive = false;
		}
	}

	if (m_bIsActive)
	{
		CTransform* pTargetTrans = TARGET_TO_TRANS(m_pTarget);

		_v3 vLook = pTargetTrans->Get_Axis(AXIS_Z);
		_v3 vRight = pTargetTrans->Get_Axis(AXIS_X);

		// x 축 방향벡터
		V3_NORMAL_SELF(&vLook);
		V3_NORMAL_SELF(&vRight);

		// x 축 방향벡터 + 플레이어
		_v3 vPosition = pTargetTrans->Get_Pos() + (vLook * 0.25f) + (vRight * 0.8f) + (WORLD_UP * 1.5f);
		m_pTransformCom->Set_Pos(vPosition);

		_float fOriginYRadian = pTargetTrans->Get_Angle().y;
		_float fAdditionalYRadian = fOriginYRadian + D3DXToRadian(160.f);
		m_pTransformCom->Set_Angle(AXIS_Y, fAdditionalYRadian);

		LOOP(3)
		{
			TARGET_TO_TRANS(m_vecOption[i])->Set_Angle(m_pTransformCom->Get_Angle());
			TARGET_TO_TRANS(m_vecOption[i])->Set_Scale(_v3(0.87f, 0.2476f, 1.f));
			TARGET_TO_TRANS(m_vecOption[i])->Set_At(m_pTransformCom->Get_At());
			TARGET_TO_TRANS(m_vecOption[i])->Set_Pos(m_pTransformCom->Get_Pos() + _v3(0.f, _float(i) * -0.2f + 0.285f, 0.f) + *V3_NORMAL_SELF(&vLook) * +0.001f);
		}
	}
	
	
	Click_Option();
	return NO_EVENT;
}

_int CGeneralStoreNPCUI::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	m_matView = g_pManagement->Get_Transform(D3DTS_VIEW);
	m_matProj = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	return NO_EVENT;
}

HRESULT CGeneralStoreNPCUI::Render_GameObject()
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

HRESULT CGeneralStoreNPCUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_GeneralStoreNPCUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_3dUI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CGeneralStoreNPCUI::SetUp_ConstantTable()
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

	return NOERROR;
}

void CGeneralStoreNPCUI::SetUp_Default()
{
	CGeneralStoreOption* pOption = nullptr;

	LOOP(3)
	{
		pOption = static_cast<CGeneralStoreOption*>(g_pManagement->Clone_GameObject_Return(L"GameObject_GeneralStoreOption", nullptr));
		TARGET_TO_TRANS(pOption)->Set_Scale(_v3(0.87f, 0.2476f, 1.f));
		g_pManagement->Add_GameOject_ToLayer_NoClone(pOption, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
		m_vecOption.push_back(pOption);
	}

	m_vecOption[0]->Set_OptionType(CGeneralStoreOption::OPTION_PURCHASE);
	m_vecOption[1]->Set_OptionType(CGeneralStoreOption::OPTION_SELL);
	m_vecOption[2]->Set_OptionType(CGeneralStoreOption::OPTION_BYE);
}

void CGeneralStoreNPCUI::Click_Option()
{
	if (!m_bIsActive)
		return;

	
	if (CCollisionMgr::Collision_Ray(m_vecOption[0], g_pInput_Device->Get_Ray(), &m_fCross))
	{
		m_vecOption[0]->Set_Select(true);

		if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
		{
			CUI_Manager::Get_Instance()->Stop_Play_UISound(L"Slot_Regist.ogg", CSoundManager::CHANNELID::UI_Click, CSoundManager::Effect_Sound);

			CGeneralStoreUI* pStoreUI = CUI_Manager::Get_Instance()->Get_GeneralStoreUI();
			
			m_bIsActive = false;
			pStoreUI->Set_Active(true);
		}
		
	}
	else
		m_vecOption[0]->Set_Select(false);

	if (CCollisionMgr::Collision_Ray(m_vecOption[1], g_pInput_Device->Get_Ray(), &m_fCross))
	{
		CUI_Manager::Get_Instance()->Stop_Play_UISound(L"Slot_Regist.ogg", CSoundManager::CHANNELID::UI_Click, CSoundManager::Effect_Sound);

		m_vecOption[1]->Set_Select(true);

		if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
		{
			CGeneralStoreSellUI* pStoreSellUI = CUI_Manager::Get_Instance()->Get_GeneralStoreSellUI();
			
			m_bIsActive = false;
			pStoreSellUI->Set_Active(true);
		}	
	}
	else
		m_vecOption[1]->Set_Select(false);

	if (CCollisionMgr::Collision_Ray(m_vecOption[2], g_pInput_Device->Get_Ray(), &m_fCross))
	{
		m_vecOption[2]->Set_Select(true);

		if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
		{
			m_bIsActive = false;
			CUI_Manager::Get_Instance()->Stop_Play_UISound(L"UI_Open.ogg", CSoundManager::CHANNELID::UI_Open_Close, CSoundManager::Effect_Sound);
		}
			

	}
	else
		m_vecOption[2]->Set_Select(false);
}

CGeneralStoreNPCUI * CGeneralStoreNPCUI::Create(_Device pGraphic_Device)
{
	CGeneralStoreNPCUI* pInstance = new CGeneralStoreNPCUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CGeneralStoreNPCUI::Clone_GameObject(void * pArg)
{
	CGeneralStoreNPCUI* pInstance = new CGeneralStoreNPCUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CGeneralStoreNPCUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
