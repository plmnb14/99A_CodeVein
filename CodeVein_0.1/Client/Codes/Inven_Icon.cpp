#include "stdafx.h"
#include "..\Headers\Inven_Icon.h"

#include "UI_Manager.h"
#include "Inventory.h"
#include "Inven_Status.h"


CInven_Icon::CInven_Icon(_Device pDevice)
	: CUI(pDevice)
{
}

CInven_Icon::CInven_Icon(const CInven_Icon & rhs)
	: CUI(rhs)
{
}

HRESULT CInven_Icon::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	m_eIconType = ICON_END;

	return NOERROR;
}

HRESULT CInven_Icon::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	
	return NOERROR;
}

_int CInven_Icon::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	if (m_bIsDead)
		return DEAD_OBJ;

	
	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 0.1f);

	return NO_EVENT;
}

_int CInven_Icon::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CInven_Icon::Render_GameObject()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;


	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	m_matOldView = g_pManagement->Get_Transform(D3DTS_VIEW);
	m_matOldProj = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);


	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();

	m_pShaderCom->Begin_Pass(1);

	// 버퍼를 렌더링한다.
	// (인덱스버퍼(012023)에 보관하고있는 인덱스를 가진 정점을 그리낟.)
	// 삼각형 두개를 그리낟.각각의 삼각형마다 정점세개, 각각의 정점을 버텍스 셰이더의 인자로 던진다.
	m_pBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matOldView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matOldProj);

	return NOERROR;
}

HRESULT CInven_Icon::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_Menu_Icon", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CInven_Icon::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))

		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, m_eIconType)))
		return E_FAIL;
	
	return NOERROR;
}



_bool CInven_Icon::Coll_Mouse()
{
	
	return g_pInput_Device->MousePt_InRect(m_fPosX, m_fPosY, m_fSizeX, m_fSizeY, g_hWnd);
}

CInven_Icon * CInven_Icon::Create(_Device pGraphic_Device)
{
	CInven_Icon* pInstance = new CInven_Icon(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("CInven_Icon Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CInven_Icon::Clone_GameObject(void * pArg)
{
	CInven_Icon* pInstance = new CInven_Icon(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("CInven_Icon Cloned Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CInven_Icon::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	CUI::Free();
}
