#include "stdafx.h"
#include "..\Headers\Inventory_Icon.h"

CInventory_Icon::CInventory_Icon(_Device pDevice)
	: CUI(pDevice)
{
}

CInventory_Icon::CInventory_Icon(const CInventory_Icon & rhs)
	: CUI(rhs)
{
}

HRESULT CInventory_Icon::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CInventory_Icon::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_bIsActive = false;

	SetUp_Default();

	return NOERROR;
}

_int CInventory_Icon::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	switch (m_eType)
	{
	case ICON_EXPEND:
		m_iIndex = 0;
		break;
	case ICON_MTRL:
		m_iIndex = 1;
		break;
	case ICON_WEAPON:
		m_iIndex = 2;
		break;
	case ICON_ARMOR:
		m_iIndex = 3;
		break;
	case ICON_ALL:
		m_iIndex = 4;
		break;
	case ICON_STATUS:
		m_iIndex = 5;
		break;
	case ICON_EXIT:
		m_iIndex = 6;
		break;
	case ICON_PET:
		m_iIndex = 7;
		break;
	case ICON_SUMMONS:
		m_iIndex = 9;
		break;
	}
	
	return NO_EVENT;
}

_int CInventory_Icon::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CInventory_Icon::Render_GameObject()
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
	if (Pt_InRect())
	{
		LOOP(2)
		{
			if (0 == i)
				iIndex = m_iIndex;
			else if (1 == i)
				iIndex = 8;

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
		iIndex = m_iIndex;
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

HRESULT CInventory_Icon::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_Inven_Icon", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CInventory_Icon::SetUp_ConstantTable(_uint iIndex)
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

void CInventory_Icon::SetUp_Default()
{
	
}

_bool CInventory_Icon::Pt_InRect()
{
	return g_pInput_Device->MousePt_InRect(m_fPosX, m_fPosY, m_fSizeX, m_fSizeY, g_hWnd);
}

CInventory_Icon * CInventory_Icon::Create(_Device pGraphic_Device)
{
	CInventory_Icon* pInstance = new CInventory_Icon(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CInventory_Icon::Clone_GameObject(void * pArg)
{
	CInventory_Icon* pInstance = new CInventory_Icon(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CInventory_Icon::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
