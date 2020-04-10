#include "stdafx.h"
#include "Get_ItemUI.h"
#include "..\Headers\Get_ItemUI.h"

#include "UI_Manager.h"


CGet_ItemUI::CGet_ItemUI(_Device Graphic_Device)
	: CUI(Graphic_Device)
{
}

CGet_ItemUI::CGet_ItemUI(const CGet_ItemUI& rhs)
	: CUI(rhs)

{
}

HRESULT CGet_ItemUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CGet_ItemUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	// ������ ȹ�� �� �˸��� 1�� ����
	// ������ ���� ������ ȹ���� ���̻� ������ n�� �� ����.
	// n�� ���� �ð��� ���� �帥 ���¿���, �� �������� ������ �� n�ʴ� ����, 
	// ������ ȹ�� �޼��� 1�� �� �߰��� ���


	m_fPosX = WINCX * 0.5f;
	m_fPosY = WINCY - 100.f;

	m_fSizeX = 269.f;
	m_fSizeY = 81.f;

	return S_OK;
}

_int CGet_ItemUI::Update_GameObject(_double TimeDelta)
{
	CGameObject::LateInit_GameObject();
	CUI::Update_GameObject(TimeDelta);

	SetUp_State(TimeDelta);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	if (true == m_bShow_Ask_Pickup /*&& false == m_bSparkle_Box*/)
		m_pRendererCom->Add_RenderList(RENDER_UI, this);

	return S_OK;
}

_int CGet_ItemUI::Late_Update_GameObject(_double TimeDelta)
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);

	m_matWorld._11 = m_fSizeX;
	m_matWorld._22 = m_fSizeY;
	m_matWorld._33 = 1.f;
	m_matWorld._41 = m_fPosX - WINCX * 0.5f;
	m_matWorld._42 = -m_fPosY + WINCY * 0.5f;

	return S_OK;
}

HRESULT CGet_ItemUI::LateInit_GameObject()
{

	return S_OK;
}

HRESULT CGet_ItemUI::Render_GameObject()
{
	if (nullptr == m_pShaderCom || nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);
	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	CUI_Manager* pUIMgr = CUI_Manager::Get_Instance();

	if (3 < m_iCount_PickUpitem)
		m_iCount_PickUpitem = 0;

	if (FAILED(SetUp_ConstantTable(0)))
		return E_FAIL;

	
	m_pGraphic_Dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pShaderCom->Begin_Shader();

	if (3 == m_iUINumber)
		m_pShaderCom->Begin_Pass(2);
	if (4 == m_iUINumber)
		m_pShaderCom->Begin_Pass(5);
	else
		m_pShaderCom->Begin_Pass(1);
	
	if (true == m_bCheck_Click && false == m_bSparkle_Box)
	{
		if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 1)))
			return E_FAIL;

		m_pShaderCom->Commit_Changes();

		if (g_pInput_Device->Key_Up(DIK_P))
		{
			m_bSparkle_Box = true;
		
			++m_iCount_PickUpitem;
			pUIMgr->Set_CoundItem(m_iCount_PickUpitem);
		}
	}
	m_pBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();
	//else
	//{

	//}
	//// ���� ���̿� ������ �ڽ� ��¦
	//if (true == m_bSparkle_Box && false == m_bShow_GetItemName)
	//{
	//	m_iUINumber = 4;

	//	m_fSparkleBox -= 0.01f;

	//	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 4)))
	//		return E_FAIL;

	//	m_pShaderCom->Commit_Changes();

	//	if (0.f >= m_fSparkleBox)
	//	{
	//		m_bShow_GetItemName = true;
	//	}
	//}
	//else
	//{
	//	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 2)))
	//		return E_FAIL;
	//}

	//if (true == m_bShow_GetItemName)
	//{
	//	m_iUINumber = 3;
	//	m_bShow_Ask_Pickup = true;

	//	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 3)))
	//		return E_FAIL;

	//	m_pShaderCom->Commit_Changes();
	//}


	return S_OK;
}

HRESULT CGet_ItemUI::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_Get_ItemUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGet_ItemUI::SetUp_ConstantTable(_uint TextureIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_fPosX", &m_fPosX, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_fSizeX", &m_fSizeX, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_fPercentage", &m_fPercentage, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_fSparkle", &m_fSparkleBox, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, TextureIndex)))
		return E_FAIL;

	m_iUINumber = TextureIndex;

	return S_OK;
}

void CGet_ItemUI::SetUp_State(_double TimeDelta)
{
	m_fPercentage = m_fNowItemBar_Size / m_fSizeX;

	// �÷��̾�� �������� �Ÿ���
	// �Ÿ������ؼ� ���� �Ÿ� �ȿ� ���� ��� UI�� ���� �÷��̾ AŰ�� ������ ��
	// �������� �ݴ´�.
	// AŰ�� ������ �� ������ ����(������) ���ϰ� �޹���� ����
	// Ű�� ���� ��(Key_Down), ȭ�� ���� ��ܿ� �ֿ� ������ �̸��� ����.
	// �� �˸��� �� 1.2�� ���� �����Ŵ

	// ������ ���̷� ������ ���߿� ������ �����ǰ� ���� ������ �����ۿ� ���� ������ �̸���(enum����?)
	// �������ٰ� �޾ƿͼ� �ش� �̸��� ����

	CGameObject* pPlayer = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE);

	_v3 Player_D3 = TARGET_TO_TRANS(m_pTarget)->Get_Pos() - TARGET_TO_TRANS(pPlayer)->Get_Pos();

	if (V3_LENGTH(&Player_D3) <= 2.f)
	{
		m_bShow_Ask_Pickup = true;
		if (g_pInput_Device->Key_Down(DIK_P))
		{
			m_bCheck_Click = true;
		}
	}
	else
	{
		m_bCheck_Click = false;
		m_bShow_Ask_Pickup = false;
	}
	

	if (3 == m_iUINumber || 4 == m_iUINumber)
	{
		m_fSizeX = 298.f;
		m_fSizeY = 38.f;

		m_fPosX = 1100.f;
		m_fPosY = 200.f;
	}


	if (3 == m_iUINumber)
		m_fNowItemBar_Size += 5.f;

	if (m_fNowItemBar_Size >= m_fSizeX)
	{
		m_fNowItemBar_Size = m_fSizeX;
		//m_fEndTimer += (_float)TimeDelta;
	}
	if (true == m_bSparkle_Box)
	{
		m_bCheck_Click = false;
		m_bSparkle_Box = false;
		m_bShow_GetItemName = false;
		m_bShow_Ask_Pickup = false;

		m_fSparkleBox = 0.f;
		m_fTimer = 0.f;
		m_fEndTimer = 0.f;
		m_fNowItemBar_Size = 0.f;

		m_iUINumber = 0;

		m_fPosX = WINCX * 0.5f;
		m_fPosY = WINCY - 100.f;

		m_fSizeX = 269.f;
		m_fSizeY = 81.f;
	}
}

CGet_ItemUI * CGet_ItemUI::Create(_Device pGraphic_Device)
{
	CGet_ItemUI* pInstance = new CGet_ItemUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CGet_ItemUI::Clone_GameObject(void * pArg)
{
	CGet_ItemUI* pInstance = new CGet_ItemUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CGet_ItemUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
