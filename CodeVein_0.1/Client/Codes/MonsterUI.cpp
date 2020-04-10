#include "stdafx.h"
#include "..\Headers\MonsterUI.h"

#include "GunGenji.h"


CMonsterUI::CMonsterUI(_Device Graphic_Device)
	: CUI(Graphic_Device)
{
}

CMonsterUI::CMonsterUI(const CMonsterUI& rhs)
	: CUI(rhs)
{
}

HRESULT CMonsterUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CMonsterUI::Ready_GameObject(void* pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_fSizeX = m_pTransformCom->Get_Size().x;
	m_fSizeY = m_pTransformCom->Get_Size().y;

	return S_OK;
}

HRESULT CMonsterUI::LateInit_GameObject()
{
	return S_OK;
}

_int CMonsterUI::Update_GameObject(_double TimeDelta)
{
	CGameObject::LateInit_GameObject();
	CUI::Update_GameObject(TimeDelta);

	if (true == m_pTarget->Get_Dead())
		return DEAD_OBJ;

	
	//if (true == m_bIsDead)
	//	return DEAD_OBJ;

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

	//_mat tmpMat;
	//tmpMat = (*m_matMonsterBon) * (m_pTransformCom->Get_WorldMat());

	//
	////m_pTransformCom->Set_Pos(_v3(TARGET_TO_TRANS(m_pTarget)->Get_Pos()) + (WORLD_UP * 1.5f));
	m_pTransformCom->Set_Pos(_v3(m_matMonsterBon->_41, m_matMonsterBon->_42, m_matMonsterBon->_43));

	/*if (0 == m_iCheck_Renderindex)
		m_pTransformCom->Set_Pos((_v3(TARGET_TO_TRANS(m_pTarget)->Get_Pos().x, TARGET_TO_TRANS(m_pTarget)->Get_Pos().y, (TARGET_TO_TRANS(m_pTarget)->Get_Pos().z - 0.06f)) + (WORLD_UP * 2.f)));
	if (1 == m_iCheck_Renderindex)
		m_pTransformCom->Set_Pos((_v3(TARGET_TO_TRANS(m_pTarget)->Get_Pos().x, TARGET_TO_TRANS(m_pTarget)->Get_Pos().y, (TARGET_TO_TRANS(m_pTarget)->Get_Pos().z - 0.02f)) + (WORLD_UP * 2.f)));
	if (2 == m_iCheck_Renderindex)
		m_pTransformCom->Set_Pos((_v3(TARGET_TO_TRANS(m_pTarget)->Get_Pos()) + (WORLD_UP * 2.f)));
*/
	if (0 == m_iCheck_Renderindex)
		m_pTransformCom->Set_Pos((_v3(m_matMonsterBon->_41, m_matMonsterBon->_42 * 1.5f, (m_matMonsterBon->_43 - 0.06f))));
	if (1 == m_iCheck_Renderindex)
		m_pTransformCom->Set_Pos((_v3(m_matMonsterBon->_41, m_matMonsterBon->_42, (m_matMonsterBon->_43 - 0.02f))));
	if (2 == m_iCheck_Renderindex)
		m_pTransformCom->Set_Pos((_v3(m_matMonsterBon->_41, m_matMonsterBon->_42 * 1.5f, m_matMonsterBon->_43)));

	m_pTransformCom->Set_Scale(_v3(0.8f, 0.08f, 0.8f));

	m_fMonsterHp = m_pTarget->Get_Target_Param().fHp_Cur;
	m_fTotalHP = m_pTarget->Get_Target_Param().fHp_Max;

	CGameObject* pPlayer = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE);

	_v3 Player_D3 = TARGET_TO_TRANS(pPlayer)->Get_Pos() - TARGET_TO_TRANS(m_pTarget)->Get_Pos();

	if (V3_LENGTH(&Player_D3) <= 10.f)
		m_bCheck_Dir = true;
	else
		m_bCheck_Dir = false;


	if (true == m_bCheck_Dir)
		m_pRendererCom->Add_RenderList(RENDER_UI, this);


	return S_OK;
}

_int CMonsterUI::Late_Update_GameObject(_double TimeDelta)
{
	return S_OK;
}


HRESULT CMonsterUI::Render_GameObject()
{
	if (nullptr == m_pShaderCom || nullptr == m_pBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable(0)))
		return E_FAIL;

	if (m_fMonsterHp > 0)	// HP 변경이 있거나 플레이어가 락온했거나
	{
		m_pGraphic_Dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		m_pShaderCom->Begin_Shader();

		_uint Temp[3] = { 0, 2, 2 };

		for (_uint i = 0; i < 3; ++i)
		{
			m_pShaderCom->Begin_Pass(Temp[i]);

			if (i == 0)
				m_iCheck_Renderindex = 0;
			if (i == 1)
			{
				if (FAILED(m_pShaderCom->Set_Value("g_fPercentage", &m_fWhite_Percentage, sizeof(_float))))
					return E_FAIL;
				m_iCheck_Renderindex = 1;
			}
			if (i == 2)
			{
				if (FAILED(m_pShaderCom->Set_Value("g_fPercentage", &m_fPercentage, sizeof(_float))))
					return E_FAIL;
				m_iCheck_Renderindex = 2;
			}
			if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, i)))
				return E_FAIL;


			m_pShaderCom->Commit_Changes();
			m_pBufferCom->Render_VIBuffer();
			m_pShaderCom->End_Pass();
		}

		m_pShaderCom->End_Shader();
	}

	return S_OK;
}

HRESULT CMonsterUI::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_MonsterHPBar", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterUI::SetUp_ConstantTable(_uint TextureIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &g_pManagement->Get_Transform(D3DTS_VIEW), sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &g_pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fSpeed", &m_fSpeed, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_fPercentage", &m_fPercentage, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CMonsterUI::SetUp_State(_double TimeDelta)
{
	m_fPercentage = m_fMonsterHp / m_fTotalHP;

	if (m_fTotalHP > m_fMonsterHp)
	{
		m_fOldHP = m_fMonsterHp;
		//m_fWhite_Percentage = m_fOldHP / m_fTotalHP;
	}


	if (m_fPercentage < m_fWhite_Percentage)
	{
		_float fGap = m_fPercentage - m_fWhite_Percentage;

		if (fGap < 0)
			fGap = 0.05f;

		m_fWhite_Percentage -= fGap * 5.f * (_float)TimeDelta;

		if (m_fWhite_Percentage < m_fPercentage)
			m_fWhite_Percentage = m_fPercentage;
	}

	// 줄어들 때, 빨간색 먼저 줄어든 뒤, 약 0.1초 후 하얀색이 천천히 줄어든다.
	// 만약 몬스터가 죽는다면 랜더를 종료한다.
	if (m_fMonsterHp >= m_fTotalHP)
		m_fMonsterHp = m_fTotalHP;
	if (m_fMonsterHp <= 0.f)
		m_fMonsterHp = 0.f;
}

CMonsterUI* CMonsterUI::Create(_Device pGraphic_Device)
{
	CMonsterUI* pInstance = new CMonsterUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CMonsterUI::Clone_GameObject(void * pArg)
{
	CMonsterUI* pInstance = new CMonsterUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CMonsterUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
