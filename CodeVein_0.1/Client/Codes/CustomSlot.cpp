#include "stdafx.h"
#include "..\Headers\CustomSlot.h"

CCustomSlot::CCustomSlot(_Device pDevice)
	: CUI(pDevice)
{
	
}

CCustomSlot::CCustomSlot(const CCustomSlot & rhs)
	: CUI(rhs)
{
	
}

HRESULT CCustomSlot::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CCustomSlot::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);
	
	SetUp_Default();

	m_bIsActive = false;
	return NOERROR;
}

_int CCustomSlot::Update_GameObject(_double TimeDelta)
{
	if (m_bIsDead)
		return DEAD_OBJ;

	Check_LateInit();
	CUI::Update_GameObject(TimeDelta);
	
	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	m_bIsCollMouse = Pt_InRect();

	return NO_EVENT;
}

_int CCustomSlot::Late_Update_GameObject(_double TimeDelta)
{
	if (m_bIsDead)
		return DEAD_OBJ;

	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);

	m_matWorld._11 = m_fSizeX;
	m_matWorld._22 = m_fSizeY;
	m_matWorld._33 = 1.f;
	m_matWorld._41 = m_fPosX - WINCX * 0.5f;
	m_matWorld._42 = -m_fPosY + WINCY * 0.5f;

	return NO_EVENT;
}

HRESULT CCustomSlot::Render_GameObject()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	if (!m_bIsActive)
		return NOERROR;

	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);
	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	_uint iIndex = 0;
	if (m_bIsCollMouse)
	{
		LOOP(3)
		{
			if (0 == i)
			{
				iIndex = 9;
				m_fSizeX = 50.f;
				m_fSizeY = 50.f;
				Late_Update_GameObject(0.f);

				if (FAILED(SetUp_ConstantTable(iIndex)))
					return E_FAIL;
			}
			else if (1 == i)
			{
				iIndex = m_iSlotIdx;
				m_fSizeX = 50.f;
				m_fSizeY = 50.f;
				Late_Update_GameObject(0.f);

				if (FAILED(SetUp_ConstantTable(iIndex, true)))
					return E_FAIL;
			}
			else if (2 == i)
			{
				iIndex = 8;
				m_fSizeX = 110.f;
				m_fSizeY = 110.f;
				Late_Update_GameObject(0.f);

				if (FAILED(SetUp_ConstantTable(iIndex)))
					return E_FAIL;
			}

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
			if (0 == i)
			{
				iIndex = 9;
				m_fSizeX = 50.f;
				m_fSizeY = 50.f;
				Late_Update_GameObject(0.f);

				if (FAILED(SetUp_ConstantTable(iIndex)))
					return E_FAIL;
			}
			else if (1 == i)
			{
				iIndex = m_iSlotIdx;
				m_fSizeX = 50.f;
				m_fSizeY = 50.f;
				Late_Update_GameObject(0.f);

				if (FAILED(SetUp_ConstantTable(iIndex, true)))
					return E_FAIL;
			}

			m_pShaderCom->Begin_Shader();
			m_pShaderCom->Begin_Pass(1);
			m_pBufferCom->Render_VIBuffer();
			m_pShaderCom->End_Pass();
			m_pShaderCom->End_Shader();
		}
	}

	if (m_bIsSelect)
	{
		iIndex = 7;
		m_fSizeX = 60.f;
		m_fSizeY = 60.f;
		Late_Update_GameObject(0.f);

		if (FAILED(SetUp_ConstantTable(iIndex)))
			return E_FAIL;
		m_pShaderCom->Begin_Shader();
		m_pShaderCom->Begin_Pass(1);
		m_pBufferCom->Render_VIBuffer();
		m_pShaderCom->End_Pass();
		m_pShaderCom->End_Shader();
	}
	
	return NOERROR;
}

_bool CCustomSlot::Pt_InRect()
{
	_float fHoverSize = 50.f;
	return g_pInput_Device->MousePt_InRect(m_fPosX, m_fPosY, fHoverSize, fHoverSize, g_hWnd);
}

HRESULT CCustomSlot::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"DefaultTex_Custom_UI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_ElemTexture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"DefaultTex_Custom_UI", L"Com_ElemTexture", (CComponent**)&m_pElemTextureCom)))
		return E_FAIL;
	
	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CCustomSlot::SetUp_ConstantTable(_int iIdx, _bool bElem)
{
	if (!m_bIsActive)
		return NOERROR;

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))

		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (bElem)
	{
		if (FAILED(m_pElemTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, iIdx)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, iIdx)))
			return E_FAIL;
	}

	return NOERROR;
}

void CCustomSlot::SetUp_Default()
{
	
}

void CCustomSlot::Check_LateInit()
{
	if (m_bLateInit)
		return;

	m_bLateInit = true;
	Check_SlotTexture();
}

void CCustomSlot::Check_SlotTexture()
{
	switch (m_eType)
	{
	case Client::CCustomSlot::TYPE_HAIR:
		Change_Texture(L"DefaultTex_Custom_Hair");
		break;
	case Client::CCustomSlot::TYPE_FACE:
		Change_Texture(L"DefaultTex_Custom_Face");
		break;
	case Client::CCustomSlot::TYPE_EYE:
		Change_Texture(L"DefaultTex_Custom_Eye");
		break;
	case Client::CCustomSlot::TYPE_MASK:
		Change_Texture(L"DefaultTex_Custom_ToxicGuard");
		break;
	case Client::CCustomSlot::TYPE_INNER:
		Change_Texture(L"DefaultTex_Custom_Inner");
		break;
	}
}

void CCustomSlot::Change_Texture(const _tchar * _Name)
{
	auto& iter = m_pmapComponents.find(L"Com_ElemTexture" );

	Safe_Release(m_pElemTextureCom);
	Safe_Release(iter->second);

	iter->second = m_pElemTextureCom = static_cast<CTexture*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _Name));
	Safe_AddRef(iter->second);
}

CCustomSlot * CCustomSlot::Create(_Device pGraphic_Device)
{
	CCustomSlot* pInstance = new CCustomSlot(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CCustomSlot::Clone_GameObject(void * pArg)
{
	CCustomSlot* pInstance = new CCustomSlot(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CCustomSlot::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pElemTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
