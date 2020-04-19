#include "stdafx.h"
#include "..\Headers\StageUI.h"
#include "Player.h"

CStageUI::CStageUI(_Device pDevice)
	: CUI(pDevice)
{
}

CStageUI::CStageUI(const CStageUI & rhs)
	: CUI(rhs)
{
}

HRESULT CStageUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CStageUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);
	m_fAlpha = 0.f;
	m_bIsActive = false;
	m_eTeleportMenu = Teleport_Home;
	
	return NOERROR;
}

_int CStageUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	if (m_bIsDead)
		return DEAD_OBJ;
	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	if (1.f > m_fAlpha && m_bIsActive)
		m_fAlpha += _float(TimeDelta);
	m_pTarget = static_cast<CPlayer*>(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));

	if (m_pTarget)
	{
		m_pTransformCom->Set_Angle(TARGET_TO_TRANS(m_pTarget)->Get_Angle());
		_v3 vLook = TARGET_TO_TRANS(m_pTarget)->Get_Axis(AXIS_Z);
		m_pTransformCom->Set_Pos(TARGET_TO_TRANS(m_pTarget)->Get_Pos() + (*V3_NORMAL_SELF(&vLook) * 2.f + _v3(0.f, 1.5f, 0.f)));
		m_pTransformCom->Set_Scale(_v3(2.f, 1.f, 0.f));
	}
	
	
	switch (m_eTeleportMenu)
	{
	case Teleport_Home:
		m_iIndex = 0;
		m_iMaxSubStage = 2;
		break;
	case Teleport_St01:
	{
		if (0 == m_iSubStage)
			m_iIndex = 1;
		else if (1 == m_iSubStage)
			m_iIndex = 12;
		else if (2 == m_iSubStage)
			m_iIndex = 13;
	}	
		break;
	case Teleport_St02:
	{
		if (0 == m_iSubStage)
			m_iIndex = 2;
		else if (1 == m_iSubStage)
			m_iIndex = 14;
		else if (2 == m_iSubStage)
			m_iIndex = 15;
	}	
		break;
	case Teleport_St03:
	{
		if (0 == m_iSubStage)
			m_iIndex = 3;
		else if (1 == m_iSubStage)
			m_iIndex = 16;
	}
		break;
	case Teleport_St04:
	{
		if (0 == m_iSubStage)
			m_iIndex = 4;
		else if (1 == m_iSubStage)
			m_iIndex = 17;
	}
		break;
	}

	if (!m_bIsActive && m_fAlpha > 0.f)
	{
		m_fAlpha -= _float(TimeDelta) * 2.f;
	}
	
	Compute_ViewZ(&m_pTransformCom->Get_Pos());
	return NO_EVENT;
}

_int CStageUI::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	m_matView = g_pManagement->Get_Transform(D3DTS_VIEW);
	m_matProj = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	return NO_EVENT;
}

HRESULT CStageUI::Render_GameObject()
{
	if (m_fAlpha <= 0.f)
		return NOERROR;
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	_uint iIndex = 0;
	if (!m_bIsSelect)
	{
		if (FAILED(SetUp_ConstantTable(m_iIndex)))
			return E_FAIL;

		m_pShaderCom->Begin_Shader();
		m_pShaderCom->Begin_Pass(5);

		m_pBufferCom->Render_VIBuffer();
		m_pShaderCom->End_Pass();
		m_pShaderCom->End_Shader();
	}
	else
	{
		if (m_eTeleportMenu == Teleport_Home)
		{
			LOOP(3)
			{
				if (i == 0)
					iIndex = 10;
				else if (i == 1)
					iIndex = 5;
				else if (i == 2)
					iIndex = m_iIndex;
				//(i == 0) ? (iIndex = 10) : (iIndex = m_iIndex);

				if (FAILED(SetUp_ConstantTable(iIndex)))
					return E_FAIL;

				m_pShaderCom->Begin_Shader();
				m_pShaderCom->Begin_Pass(5);

				m_pBufferCom->Render_VIBuffer();
				m_pShaderCom->End_Pass();
				m_pShaderCom->End_Shader();
			}
			
		}
		else if (m_eTeleportMenu == Teleport_End - 1)
		{
			LOOP(3)
			{
				if (i == 0)
					iIndex = 11;
				else if (i == 1)
					iIndex = 9;
				else if (i == 2)
					iIndex = m_iIndex;
				//(i == 0) ? (iIndex = 11) : (iIndex = m_iIndex);

				if (FAILED(SetUp_ConstantTable(iIndex)))
					return E_FAIL;

				m_pShaderCom->Begin_Shader();
				m_pShaderCom->Begin_Pass(5);

				m_pBufferCom->Render_VIBuffer();
				m_pShaderCom->End_Pass();
				m_pShaderCom->End_Shader();
			}
			
		}
		else
		{
			LOOP(4)
			{
				if (i == 0) iIndex = 10;
				else if (i == 1) iIndex = 11;
				else if (i == 2)
				{
					switch (m_eTeleportMenu)
					{
					case Teleport_St01:
						iIndex = 6;
						break;
					case Teleport_St02:
						iIndex = 7;
						break;
					case Teleport_St03:
						iIndex = 8;
						break;
					case Teleport_St04:
						iIndex = 9;
						break;
					}
				}
				else if (i == 3) iIndex = m_iIndex;

				if (FAILED(SetUp_ConstantTable(iIndex)))
					return E_FAIL;

				m_pShaderCom->Begin_Shader();
				m_pShaderCom->Begin_Pass(5);

				m_pBufferCom->Render_VIBuffer();
				m_pShaderCom->End_Pass();
				m_pShaderCom->End_Shader();
			}
			
		}
	}

	return NOERROR;
}

HRESULT CStageUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_StageUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CStageUI::SetUp_ConstantTable(_uint iIndex)
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
	if (FAILED(m_pShaderCom->Set_Value("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;
	return NOERROR;
}

CStageUI * CStageUI::Create(_Device pGraphic_Device)
{
	CStageUI* pInstance = new CStageUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed Create StageUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CStageUI::Clone_GameObject(void * pArg)
{
	CStageUI* pInstance = new CStageUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed Clone StageUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStageUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
