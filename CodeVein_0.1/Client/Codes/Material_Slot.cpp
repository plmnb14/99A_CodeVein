#include "stdafx.h"
#include "..\Headers\Material_Slot.h"
#include "PlayerFontUI.h"


CMaterial_Slot::CMaterial_Slot(_Device pDevice)
	: CUI(pDevice)
{
}

CMaterial_Slot::CMaterial_Slot(const CMaterial_Slot & rhs)
	: CUI(rhs)
{
}

HRESULT CMaterial_Slot::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CMaterial_Slot::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_bIsActive = false;

	SetUp_Default();

	return NOERROR;
}

_int CMaterial_Slot::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	if (m_bIsDead)
		return DEAD_OBJ;

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	if (m_vecMaterial.size() > 0)
		m_eType = m_vecMaterial.front()->Get_Type();
	else
		m_eType = CMaterial::MATERIAL_END;

	switch (m_eType)
	{
	case CMaterial::Queen_Steel:
		m_iIndex = 3;
		break;
	case CMaterial::Queen_Titanium:
		m_iIndex = 4;
		break;
	case CMaterial::Queen_Tungsten:
		m_iIndex = 5;
		break;
	}

	if (m_pItemCntFont)
	{
		m_pItemCntFont->Set_Active(m_bIsActive);

		if (m_vecMaterial.size() == 0)
			m_pItemCntFont->Set_Active(false);

		m_pItemCntFont->Update_NumberValue(_float(m_vecMaterial.size()));
		
	}

	m_bIsCollMouse = Pt_InRect();

	return NO_EVENT;
}

_int CMaterial_Slot::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CMaterial_Slot::Render_GameObject()
{
	if (!m_bIsActive ||
		m_eType == CMaterial::MATERIAL_END)
		return NOERROR;

	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);
	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	_uint iIndex = 0;

	if (!m_bIsSelect)
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
				if (0 == i)
					iIndex = 0;
				else if (1 == i)
					iIndex = m_iIndex;

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
	



	return NOERROR;
}

HRESULT CMaterial_Slot::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_Materials", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CMaterial_Slot::SetUp_ConstantTable(_uint iIndex)
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

void CMaterial_Slot::SetUp_Default()
{
	m_pItemCntFont = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pItemCntFont->Set_UI_Pos(m_fPosX - m_fSizeX * 0.25f, m_fPosY + m_fSizeY * 0.25f);
	m_pItemCntFont->Set_UI_Size(10.4f, 20.f);
	m_pItemCntFont->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pItemCntFont, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	m_pItemCntFont->Set_ViewZ(m_fViewZ - 0.1f);
}


void CMaterial_Slot::Input_Item(CMaterial * pMaterial)
{
	if (nullptr == pMaterial)
		return;

	m_vecMaterial.push_back(pMaterial);
}

_bool CMaterial_Slot::Pt_InRect()
{
	return g_pInput_Device->MousePt_InRect(m_fPosX, m_fPosY, m_fSizeX, m_fSizeY, g_hWnd);
}

_uint CMaterial_Slot::Get_Size()
{
	return _uint(m_vecMaterial.size());
}

_uint CMaterial_Slot::Get_Type()
{
	if (m_vecMaterial.size() == 0)
		return CMaterial::MATERIAL_END;
	return _uint(m_vecMaterial.front()->Get_Type());
}

void CMaterial_Slot::Delete_Item()
{
	if (m_vecMaterial.size() == 0)
		return;
	m_vecMaterial.pop_back();
}

CMaterial_Slot * CMaterial_Slot::Create(_Device pGraphic_Device)
{
	CMaterial_Slot* pInstance = new CMaterial_Slot(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("CMaterialSlot Creating Fail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMaterial_Slot::Clone_GameObject(void * pArg)
{
	CMaterial_Slot* pInstance = new CMaterial_Slot(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("CMaterialSlot Cloned Fail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMaterial_Slot::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
