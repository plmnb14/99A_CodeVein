#include "stdafx.h"
#include "..\Headers\MonsterUI.h"

#include "CameraMgr.h"

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

	// 몬스터의 머리 위에 띄워야 함
	// 몬스터의 좌표를 받아와서 플레이어에게 빌보드로
	// 몬스터의 Y좌표에서 조금 더 위에다가 띄운다
	///////////////////////////
	// HP바 옆에 데미지를 입었을 때 입은 데미지가 뜬다
	// 데미지를 입거나 락온 시에 HP바가 출력된다.
	// 데미지 입을 시 빨간색 바는 데미지 들어가자마자 줄어든다.
	// 줄어들 데미지만큼 흰색이 표시된 후에 흰색이 줄어든다.
	// 흰색은 빨간색 바가 줄어들고 나서 시간 차를 두고 천천히 줄어든다.
	// 공격하거나 데미지 입을 때 파란색으로 바에 칠해지는데 어떤 조건이 있어야 함
	// 하얀색 위에 빨간색 덮어두고 데미지 들어갈 때 바로 HP바 줄이고 그 다음에 천천히 하얀색 바 줄어들게
	// 몬스터가 데미지를 입었을 떄 HP Bar가 생겨야 함

	// 몬스터마다 HP가 달라야


	m_fMonsterHp = 200.f;
	m_fTotalHP = 200.f;

	return S_OK;
}

HRESULT CMonsterUI::LateInit_GameObject()
{
	m_pTarget = g_pManagement->Get_Instance()->Get_GameObjectBack(L"Layer_Monster", SCENE_STAGE);

	m_pTransformCom->Set_Pos(_v3(TARGET_TO_TRANS(m_pTarget)->Get_Pos() + (WORLD_UP * 2.f)));
	m_pTransformCom->Set_Scale(_v3(0.8f, 0.08f, 1.f));

	m_fSizeX = m_pTransformCom->Get_Size().x;
	m_fSizeY = m_pTransformCom->Get_Size().y;

	return S_OK;
}

_int CMonsterUI::Update_GameObject(_double TimeDelta)
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
	
	//_v3 CamDes = g_pManagement->Get_CamPosition() - m_pTransformCom->Get_Pos();
	//_float fCamDir = V3_LENGTH(&CamDes);
	//if (fCamDir > 5.f && fCamDir <= 10.f)
	//{
	//	m_fAlpha = (fCamDir + 5.f) ;
	//}
	//if(fCamDir < 5.f)
	//	m_fAlpha = 1.f;



	if (m_fMonsterHp > 0)	// HP 변경이 있거나 플레이어가 락온했거나
	{
		m_pGraphic_Dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		m_pShaderCom->Begin_Shader();

		_uint Temp[3] = { 0, 2, 2 };

		for (_uint i = 0; i < 3; ++i)
		{
			m_pShaderCom->Begin_Pass(Temp[i]);

			if (i == 1)
			{
				if (FAILED(m_pShaderCom->Set_Value("g_fPercentage", &m_fWhite_Percentage, sizeof(_float))))
					return E_FAIL;
			}
			if (i == 0 || i == 2)
			{
				if (FAILED(m_pShaderCom->Set_Value("g_fPercentage", &m_fPercentage, sizeof(_float))))
					return E_FAIL;
			}
			if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, i)))
				return E_FAIL;
			
			
			m_pShaderCom->Commit_Changes();
			m_pBufferCom->Render_VIBuffer();
			m_pShaderCom->End_Pass();
		}

		m_pShaderCom->End_Shader();
	}


	//cout << m_fPercentage << ", " << m_fWhite_Percentage << endl;
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

	// 마지막에 숫자는 몇번째 사진으로 할건지
	/*if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, TextureIndex)))
		return E_FAIL;*/

	return S_OK;
}

void CMonsterUI::SetUp_State(_double TimeDelta)
{
	m_fPercentage = m_fMonsterHp / m_fTotalHP;
	
	if (m_fPercentage < m_fWhite_Percentage)
	{
		_float fGap = m_fPercentage - m_fWhite_Percentage;

		if (fGap < 0)
			fGap = 0.05f;

		m_fWhite_Percentage -= fGap * 6.f * (_float)TimeDelta;

		if (m_fWhite_Percentage < m_fPercentage)
			m_fWhite_Percentage = m_fPercentage;
	}
	

	// 임시로 늘어들고 줄게 해보자

	if (GetAsyncKeyState(VK_NUMPAD4) & 0x8000)
	{
		m_fOldHP = m_fMonsterHp;
		m_fWhite_Percentage = m_fOldHP / m_fTotalHP;
		m_fMonsterHp += 20.f;
	}
	if (GetAsyncKeyState(VK_NUMPAD2) & 0x8000)
	{
		m_fOldHP = m_fMonsterHp;
		m_fWhite_Percentage = m_fOldHP / m_fTotalHP;
		m_fMonsterHp -= 20.f;
	}
		
	
	// 만약 빨간색바가 줄어들었다면 빨간색이 줄어든만큼만 하얀색바도 줄어들어야 한다.
	// 빨간색 바의 크기를 가져와서 그 크기만큼 줄인다면?
	//if (/*빨간색 바가 줄어들었다면 &&*/ 3 == m_iHPIndex)
	//	m_fMonsterHp -= m_fDamage * TimeDelta;
	

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
