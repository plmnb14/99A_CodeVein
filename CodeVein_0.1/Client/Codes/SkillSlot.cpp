#include "stdafx.h"
#include "..\Headers\SkillSlot.h"


CSkillSlot::CSkillSlot(_Device pDevice)
	: CUI(pDevice)
{
}

CSkillSlot::CSkillSlot(const CSkillSlot & rhs)
	: CUI(rhs)
{
}

HRESULT CSkillSlot::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CSkillSlot::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);
	return NOERROR;
}

_int CSkillSlot::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	SetUp_SkillIndex();

	m_bIsCollMouse = Pt_InRect();

	return NO_EVENT;
}

_int CSkillSlot::Late_Update_GameObject(_double TimeDelta)
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);

	m_matWorld._11 = m_fSizeX;
	m_matWorld._22 = m_fSizeY;
	m_matWorld._33 = 1.f;
	m_matWorld._41 = m_fPosX - WINCX * 0.5f;
	m_matWorld._42 = -m_fPosY + WINCY * 0.5f;

	return NO_EVENT;
}

HRESULT CSkillSlot::Render_GameObject()
{
	if (!m_bIsActive)
		return NOERROR;

	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	_uint iIndex = 0;

	if (m_bIsSelect)
	{
		if (m_bIsCollMouse)
		{
			LOOP(4)
			{
				if (0 == i)
					iIndex = 0;
				else if (1 == i)
					iIndex = m_iIndex;
				else if (2 == i)
					iIndex = 1;
				else if (3 == i)
					iIndex = 2;

				if (FAILED(SetUp_ConstantTable(iIndex)))
					return E_FAIL;

				m_pShaderCom->Begin_Shader();
				m_pShaderCom->Begin_Pass(1);
				m_pBufferCom->Render_VIBuffer();
				m_pShaderCom->End_Pass();
				m_pShaderCom->End_Shader();
			}
		}
		else
		{
			LOOP(3)
			{
				if (0 == i)
					iIndex = 0;
				else if (1 == i)
					iIndex = m_iIndex;
				else if (2 == i)
					iIndex = 1;

				if (FAILED(SetUp_ConstantTable(iIndex)))
					return E_FAIL;

				m_pShaderCom->Begin_Shader();
				m_pShaderCom->Begin_Pass(1);
				m_pBufferCom->Render_VIBuffer();
				m_pShaderCom->End_Pass();
				m_pShaderCom->End_Shader();
			}
		}
	}
	else
	{
		if (m_bIsCollMouse)
		{
			LOOP(3)
			{
				if (0 == i)
					iIndex = 0;
				else if (1 == i)
					iIndex = m_iIndex;
				else if (2 == i)
					iIndex = 2;

				if (FAILED(SetUp_ConstantTable(iIndex)))
					return E_FAIL;

				m_pShaderCom->Begin_Shader();
				m_pShaderCom->Begin_Pass(1);
				m_pBufferCom->Render_VIBuffer();
				m_pShaderCom->End_Pass();
				m_pShaderCom->End_Shader();
			}
		}
		else
		{
			LOOP(2)
			{
				(0 == i) ? (iIndex = 0) : (iIndex = m_iIndex);
				if (FAILED(SetUp_ConstantTable(iIndex)))
					return E_FAIL;

				m_pShaderCom->Begin_Shader();
				m_pShaderCom->Begin_Pass(1);
				m_pBufferCom->Render_VIBuffer();
				m_pShaderCom->End_Pass();
				m_pShaderCom->End_Shader();
			}

		}
	}
	
		
	

	return NOERROR;
}

HRESULT CSkillSlot::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_SkillSlot", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CSkillSlot::SetUp_ConstantTable(_uint iIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))

		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, iIndex)))
		return E_FAIL;

	return NOERROR;
}

void CSkillSlot::SetUp_SkillIndex()
{
	switch (m_eSkillID)
	{
	case Dragon_Lunge:
		m_iIndex = 3;
		break;
	case Vanishing_Hollow:
		m_iIndex = 4;
		break;
	case Circulating_Pulse:
		m_iIndex = 5;
		break;
	case Triple_Annihilator:
		m_iIndex = 6;
		break;
	case Shadow_Assault:
		m_iIndex = 7;
		break;
	case Severing_Abyss:
		m_iIndex = 8;
		break;
	case Swallow_Cutter:
		m_iIndex = 9;
		break;
	case Tormenting_Blast:
		m_iIndex = 10;
		break;
	case Phantom_Assault:
		m_iIndex = 11;
		break;
	case Legion_Punisher:
		m_iIndex = 12;
		break;	
	}
}

_bool CSkillSlot::Pt_InRect()
{
	return g_pInput_Device->MousePt_InRect(m_fPosX, m_fPosY, m_fSizeX, m_fSizeY, g_hWnd);
}

CSkillSlot * CSkillSlot::Create(_Device pGraphic_Device)
{
	CSkillSlot* pInstance = new CSkillSlot(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CSkillSlot::Clone_GameObject(void * pArg)
{
	CSkillSlot* pInstance = new CSkillSlot(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CSkillSlot::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
