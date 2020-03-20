#include "stdafx.h"
#include "..\Headers\ItemPalette.h"





CItemPalette::CItemPalette(_Device pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CItemPalette::CItemPalette(const CItemPalette & rhs)
	: CUI(rhs)
{
}

_uint CItemPalette::Get_ItemIndex(_uint iPaletteIndex)
{
	if (iPaletteIndex >= m_vecItemIcon.size())
		return 0;

	return m_vecItemIcon[iPaletteIndex]->Get_Index();
}

HRESULT CItemPalette::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}


HRESULT CItemPalette::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_vecItemIcon.reserve(m_iMaxCnt);

	
	m_fPosX = WINCX * 0.5f;
	m_fPosY = WINCY * 0.5f;

	m_fSizeX = 600.f;
	m_fSizeY = 400.f;

	
	return NOERROR;
}

_int CItemPalette::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);


	//SetUp_WindowPosition();
	////////�ӽ� �ȷ�Ʈ ������ ��ȯ///////////////////////
	if (CInput_Device::Get_Instance()->Key_Up(DIK_C))
		Add_ItemIcon(L"GameObject_ItemIcon", SCENE_STAGE, L"Layer_ItemIcon", 1);
	if (CInput_Device::Get_Instance()->Key_Up(DIK_V))
		Add_ItemIcon(L"GameObject_ItemIcon", SCENE_STAGE, L"Layer_ItemIcon", 2);
	if (CInput_Device::Get_Instance()->Key_Up(DIK_B))
		Add_ItemIcon(L"GameObject_ItemIcon", SCENE_STAGE, L"Layer_ItemIcon", 3);

	return NO_EVENT;
}

_int CItemPalette::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CItemPalette::Render_GameObject()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	m_matOldView = pManagement->Get_Transform(D3DTS_VIEW);
	m_matOldProj = pManagement->Get_Transform(D3DTS_PROJECTION);

	pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);


	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();

	m_pShaderCom->Begin_Pass(1);

	// ���۸� �������Ѵ�.
	// (�ε�������(012023)�� �����ϰ��ִ� �ε����� ���� ������ �׸���.)
	// �ﰢ�� �ΰ��� �׸���.������ �ﰢ������ ��������, ������ ������ ���ؽ� ���̴��� ���ڷ� ������.
	m_pBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();

	pManagement->Set_Transform(D3DTS_VIEW, m_matOldView);
	pManagement->Set_Transform(D3DTS_PROJECTION, m_matOldProj);

	Safe_Release(pManagement);

	return NOERROR;
}

HRESULT CItemPalette::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Texture_ItemPalette", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CItemPalette::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))

		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 0)))
		return E_FAIL;

	return NOERROR;
}

void CItemPalette::SetUp_WindowPosition()
{
	

	if (CInput_Device::Get_Instance()->Key_Up(DIK_ESCAPE))
		m_bIsOpenWindow = !m_bIsOpenWindow;

	if (true == m_bIsOpenWindow)
	{
		m_fPosX = WINCX * 0.5f;
		m_fPosY = WINCY * 0.5f;

		
	}
	else
	{
		m_fPosX = -WINCX * 0.5f;
		m_fPosY = -WINCY * 0.5f;

		
	}

	LOOP(m_vecItemIcon.size())
	{
		m_vecItemIcon[i]->Set_UI_Pos(m_fPosX - 100.f + 50.f * i, m_fPosY);
	}
	
}

void CItemPalette::Add_ItemIcon(const _tchar * pPrototypeTag, _uint iSceneID, const _tchar * pLayerTag, _uint iItemIndex)
{
	if (m_vecItemIcon.size() >= m_iMaxCnt)
		return;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;
	Safe_AddRef(pManagement);

	CUI::UI_DESC* pDesc = new CUI::UI_DESC;
	pDesc->fPosX = m_fPosX - 100.f + 50.f * _float(m_vecItemIcon.size());
	pDesc->fPosY = m_fPosY;
	pDesc->fSizeX = 50.f;
	pDesc->fSizeY = 50.f;
	pManagement->Add_GameObject_ToLayer(pPrototypeTag, iSceneID, pLayerTag, pDesc);

	CItemIcon* pItemIcon = static_cast<CItemIcon*>(pManagement->Get_GameObjectBack(pLayerTag, iSceneID));
	pItemIcon->Set_Index(iItemIndex);
	m_vecItemIcon.push_back(pItemIcon);

	Safe_Release(pManagement);
	
}

CItemPalette * CItemPalette::Create(_Device pGraphic_Device)
{
	CItemPalette* pInstance = new CItemPalette(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("CItemPalette Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CItemPalette::Clone_GameObject(void * pArg)
{
	CItemPalette* pInstance = new CItemPalette(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("CItemPalette Cloned Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItemPalette::Free()
{
	for_each(m_vecItemIcon.begin(), m_vecItemIcon.end(), Safe_Release<CItemIcon*>);
	m_vecItemIcon.shrink_to_fit();
	m_vecItemIcon.clear();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	CUI::Free();
}
