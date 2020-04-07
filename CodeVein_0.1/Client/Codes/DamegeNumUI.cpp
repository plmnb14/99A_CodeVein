#include "stdafx.h"
#include "..\Headers\DamegeNumUI.h"

#include "MonsterUI.h"

CDamegeNumUI::CDamegeNumUI(_Device Graphic_Device)
	: CUI(Graphic_Device)
{
}

CDamegeNumUI::CDamegeNumUI(const CDamegeNumUI & rhs)
	: CUI(rhs)
{
}

HRESULT CDamegeNumUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CDamegeNumUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_fSizeX = m_pTransformCom->Get_Size().x;
	m_fSizeY = m_pTransformCom->Get_Size().y;

	return S_OK;
}

_int CDamegeNumUI::Update_GameObject(_double TimeDelta)
{
	CGameObject::LateInit_GameObject();
	CUI::Update_GameObject(TimeDelta);

	SetUp_State(TimeDelta);

	_mat matBill, matWorld, matView;
	matWorld = m_pTransformCom->Get_WorldMat();

	matView = g_pManagement->Get_Transform(D3DTS_VIEW);
	D3DXMatrixIdentity(&matBill);

	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, NULL, &matBill);

	Compute_ViewZ(&m_pTransformCom->Get_Pos());

	m_pTransformCom->Set_WorldMat((matBill * matWorld));


	m_fPosX = m_pTransformCom->Get_Pos().x;
	m_fPosY = m_pTransformCom->Get_Pos().y;

	matWorld._41 = m_fPosX - WINCX * 0.5f;
	matWorld._42 = -m_fPosY + WINCY * 0.5f;

	//m_pTransformCom->Set_Pos(_v3(TARGET_TO_TRANS(m_pTarget)->Get_Pos()) + (WORLD_UP * 2.f) + (WORLD_RIGHT * -0.5f));
	m_pTransformCom->Set_Scale(_v3(0.1f, 0.1f, 0.1f));

	// �������� ������ ���� �����ؾ� ��
	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	return S_OK;
}

_int CDamegeNumUI::Late_Update_GameObject(_double TimeDelta)
{
	return S_OK;
}

HRESULT CDamegeNumUI::LateInit_GameObject()
{
	m_iNowHP = (_uint)m_pTarget->Get_Target_Param().fHp_Cur;
	m_iMaxHP = (_uint)m_pTarget->Get_Target_Param().fHp_Max;
	
	return S_OK;
}

HRESULT CDamegeNumUI::Render_GameObject()
{
	if (nullptr == m_pShaderCom || nullptr == m_pBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable(0)))
		return E_FAIL;

	m_pGraphic_Dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pShaderCom->Begin_Shader();

	if (0 <= m_iSave_Damage && 9 >= m_iSave_Damage)
	{
		for (_uint i = 0; i < m_iSave_Damage; ++i)
		{
			m_pShaderCom->Begin_Pass(1);

			m_pTransformCom->Set_Pos(_v3(TARGET_TO_TRANS(m_pTarget)->Get_Pos()) + (WORLD_UP * 2.f) + (WORLD_RIGHT * -1.f));

			if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, Temp_GetDamage[2])))
				return E_FAIL;

			m_pShaderCom->Commit_Changes();
			m_pBufferCom->Render_VIBuffer();
			m_pShaderCom->End_Pass();
		}

		for (_uint i = 0; i < m_iSave_Damage; ++i)
		{
			m_pShaderCom->Begin_Pass(1);

			m_pTransformCom->Set_Pos(_v3(TARGET_TO_TRANS(m_pTarget)->Get_Pos()) + (WORLD_UP * 2.f) + (WORLD_RIGHT * -2.f));

			if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, Temp_GetDamage[1])))
				return E_FAIL;

			m_pShaderCom->Commit_Changes();
			m_pBufferCom->Render_VIBuffer();
			m_pShaderCom->End_Pass();
		}

		for (_uint i = 0; i < m_iSave_Damage; ++i)
		{
			m_pShaderCom->Begin_Pass(1);

			m_pTransformCom->Set_Pos(_v3(TARGET_TO_TRANS(m_pTarget)->Get_Pos()) + (WORLD_UP * 2.f) + (WORLD_RIGHT * -3.f));

			if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, Temp_GetDamage[0])))
				return E_FAIL;

			m_pShaderCom->Commit_Changes();
			m_pBufferCom->Render_VIBuffer();
			m_pShaderCom->End_Pass();
		}
	}

	m_pShaderCom->End_Shader();

	return S_OK;
}

HRESULT CDamegeNumUI::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_DamegeNum", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDamegeNumUI::SetUp_ConstantTable(_uint TextureIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matView", &g_pManagement->Get_Transform(D3DTS_VIEW), sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &g_pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_mat))))

	if (FAILED(m_pShaderCom->Set_Value("g_fPosX", &m_fPosX, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_fSizeX", &m_fSizeX, sizeof(_float))))
		return E_FAIL;

	/*if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, TextureIndex)))
		return E_FAIL;*/

	return S_OK;
}

void CDamegeNumUI::SetUp_State(_double TimeDelta)
{	
	if (m_pTarget == nullptr)
		return;

		if (true == m_pTarget->Get_Target_IsHit())
		{
			m_iGet_Damege = m_iMaxHP - (_uint)m_pTarget->Get_Target_Hp();
			m_iNowHP -= m_iGet_Damege;
			m_iMaxHP = m_iNowHP;
		}

		if (true == m_pTarget->Get_Target_IsHit())
		{
			if(3 < m_iSave_Damage)
				m_iSave_Damage = 0;

			while (0 != m_iGet_Damege)
			{
				Temp_GetDamage[m_iSave_Damage] = m_iGet_Damege % 10;
				m_iGet_Damege /= 10;
				m_iSave_Damage += 1;
			}
		}
}

CDamegeNumUI* CDamegeNumUI::Create(_Device pGraphic_Device)
{
	CDamegeNumUI* pInstance = new CDamegeNumUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CDamegeNumUI::Clone_GameObject(void * pArg)
{
	CDamegeNumUI* pInstance = new CDamegeNumUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CDamegeNumUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
