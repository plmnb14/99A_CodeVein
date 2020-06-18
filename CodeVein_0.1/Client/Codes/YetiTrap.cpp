#include "stdafx.h"
#include "..\Headers\YetiTrap.h"

CYetiTrap::CYetiTrap(_Device _pGraphicDev)
	:CGameObject(_pGraphicDev)
{
}

CYetiTrap::CYetiTrap(const CYetiTrap & rhs)
	:CGameObject(rhs)
{
}

CYetiTrap::~CYetiTrap()
{
}

HRESULT CYetiTrap::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CYetiTrap::Ready_GameObject(void * pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("Nullptr Default Seting YetiTrap");
		return E_FAIL;
	}

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Default(pArg)))
		return E_FAIL;

	return S_OK;
}

_int CYetiTrap::Update_GameObject(_double _TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	CGameObject::Update_GameObject(_TimeDelta);

	Check_Dist();

	return NO_EVENT;
}

_int CYetiTrap::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	IF_NULL_VALUE_RETURN(m_pRenderer, E_FAIL);

	if (false == m_pOptimization->Check_InFrustumforObject(&m_pTransform->Get_Pos(), 3.f))
		return NO_EVENT;
	
	m_pRenderer->Add_RenderList(RENDER_NONALPHA, this);
	m_pRenderer->Add_RenderList(RENDER_MOTIONBLURTARGET, this);
	m_pRenderer->Add_RenderList(RENDER_SHADOWTARGET, this);

	return NO_EVENT;
}

HRESULT CYetiTrap::Render_GameObject()
{
	_mat matveiwView = CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW);
	_mat matPro = CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION);

	m_pShader->Set_Value("g_matView", &matveiwView, sizeof(_mat));
	m_pShader->Set_Value("g_matProj", &matPro, sizeof(_mat));

	m_pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMat(), sizeof(_mat));

	m_pShader->Begin_Shader();

	_ulong dwNum = m_pMesh_Static->Get_NumMaterials();

	for (_ulong i = 0; i < dwNum; ++i)
	{
		m_iPass = m_pMesh_Static->Get_MaterialPass(i);

		m_pShader->Begin_Pass(m_iPass);

		m_pShader->Set_StaticTexture_Auto(m_pMesh_Static, i);

		m_pShader->Commit_Changes();

		m_pMesh_Static->Render_Mesh(i);

		m_pShader->End_Pass();
	}

	m_pShader->End_Shader();

	return S_OK;
}

HRESULT CYetiTrap::Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur)
{
	//============================================================================================
	// 공통 변수
	//============================================================================================
	_mat	WorldMatrix = m_pTransform->Get_WorldMat();

	if (FAILED(pShader->Set_Value("g_matWorld", &WorldMatrix, sizeof(_mat))))
		return E_FAIL;

	//============================================================================================
	// 모션 블러 상수
	//============================================================================================
	if (_bIsForMotionBlur)
	{
		_mat	ViewMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW);
		_mat	ProjMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION);

		if (FAILED(pShader->Set_Value("g_matLastWVP", &m_matLastWVP, sizeof(_mat))))
			return E_FAIL;

		m_matLastWVP = WorldMatrix * ViewMatrix * ProjMatrix;

		_float fBloomPower = 0.f;
		if (FAILED(pShader->Set_Value("g_fBloomPower", &fBloomPower, sizeof(_float))))
			return E_FAIL;
	}
	//============================================================================================
	// 쉐이더 시작
	//============================================================================================

	_ulong dwNumSubSet = m_pMesh_Static->Get_NumMaterials();

	for (_ulong i = 0; i < dwNumSubSet; ++i)
	{
		pShader->Begin_Pass(iPass);

		pShader->Commit_Changes();

		m_pMesh_Static->Render_Mesh(i);

		pShader->End_Pass();
	}

	//============================================================================================

	return S_OK;
}

HRESULT CYetiTrap::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	if (FAILED(SetUp_ConstantTable(m_pShader)))
		return E_FAIL;

	_v4 vRimcolor = _v4(0.f, 0.f, 1.f, 1.f);

	_ulong dwNum = m_pMesh_Static->Get_NumMaterials();

	if (FAILED(pShader->Set_Value("g_vRimColor", &vRimcolor, sizeof(_v4))))
		return E_FAIL;

	for (_ulong i = 0; i < dwNum; ++i)
	{
		m_iPass = m_pMesh_Static->Get_MaterialPass(i);

		pShader->Begin_Pass(m_iPass);

		pShader->Set_StaticTexture_Auto(m_pMesh_Static, i);

		pShader->Commit_Changes();

		m_pMesh_Static->Render_Mesh(i);

		pShader->End_Pass();
	}

	vRimcolor = _v4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pShader->Set_Value("g_vRimColor", &vRimcolor, sizeof(_v4))))
		return E_FAIL;

	return S_OK;
}

void CYetiTrap::Check_Dist()
{
	if (nullptr != m_pPlayer)
	{
		_float fDist = V3_LENGTH(&(TARGET_TO_TRANS(m_pPlayer)->Get_Pos() - m_pTransform->Get_Pos()));

		if (fDist <= 5.5f)
		{
			m_bCanSummonYeti = true;
			if (!m_bEffect)
			{
				m_bEffect = true;

				_v3 vPos = m_pTransform->Get_Pos();
				CParticleMgr::Get_Instance()->Create_Effect(L"DeerKing_Snow_Up_Particle_0", vPos);
				CParticleMgr::Get_Instance()->Create_Effect(L"DeerKing_Snow_Up_Particle_0", vPos);
				CParticleMgr::Get_Instance()->Create_Effect(L"DeerKing_IceSmoke_0", vPos);
				CParticleMgr::Get_Instance()->Create_Effect(L"DeerKing_IceSmoke_1", vPos);
				CParticleMgr::Get_Instance()->Create_Effect(L"DeerKing_IceSmoke_Mid_0", vPos);
				CParticleMgr::Get_Instance()->Create_Effect(L"DeerKing_IceSmoke_Mid_1", vPos);
			}
		}
	}

	return;
}

HRESULT CYetiTrap::Add_Component(void* pArg)
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_SM_Snowpack03_st09a1", L"Com_StaticMesh", (CComponent**)&m_pMesh_Static)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"BattleAgent", L"Com_BattleAgent", (CComponent**)&m_pBattleAgent)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Optimization", L"Com_Opimaization", (CComponent**)& m_pOptimization)))
		return E_FAIL;

	return S_OK;
}

HRESULT CYetiTrap::SetUp_ConstantTable(CShader * pShader)
{
	if (FAILED(pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMat(), sizeof(_mat))))

	//=============================================================================================
	// 디졸브용 상수
	//=============================================================================================
	if (FAILED(pShader->Set_Value("g_fFxAlpha", &m_fFXAlpha, sizeof(_float))))
		return E_FAIL;

	//=============================================================================================
	// 쉐이더 재질정보 수치 입력
	//=============================================================================================
	_float	fEmissivePower = 5.f;	// 이미시브 : 높을 수록, 자체 발광이 강해짐.
	_float	fSpecularPower = 1.f;	// 메탈니스 : 높을 수록, 정반사가 강해짐.
	_float	fRoughnessPower = 1.f;	// 러프니스 : 높을 수록, 빛 산란이 적어짐(빛이 응집됨).
	_float	fMinSpecular = 0.1f;	// 최소 빛	: 높을 수록 빛이 퍼짐(림라이트의 범위가 넓어지고 , 밀집도가 낮아짐).
	_float	fID_R = 1.0f;	// ID_R : R채널 ID 값 , 1이 최대
	_float	fID_G = 0.5f;	// ID_G : G채널 ID 값 , 1이 최대
	_float	fID_B = 0.2f;	// ID_B	: B채널 ID 값 , 1이 최대

	if (FAILED(pShader->Set_Value("g_fEmissivePower", &fEmissivePower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fSpecularPower", &fSpecularPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fRoughnessPower", &fRoughnessPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fMinSpecular", &fMinSpecular, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fID_R_Power", &fID_R, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fID_G_Power", &fID_G, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fID_B_Power", &fID_B, sizeof(_float))))
		return E_FAIL;

	//=============================================================================================
	// 림라이트 값들을 쉐이더에 등록시킴
	//=============================================================================================
	m_pBattleAgent->Update_RimParam_OnShader(pShader);
	//=============================================================================================

	return S_OK;
}

HRESULT CYetiTrap::Ready_Default(void * pArg)
{
	YETITRAPINFO Info = *(YETITRAPINFO*)pArg;

	m_pTransform->Set_Scale(V3_ONE);
	m_pTransform->Set_Angle(Info.vAngle);
	m_pTransform->Set_Pos(Info.vPos);

	m_bCanSummonYeti = false;
	m_iIndex = Info.iIndex;

	m_pBattleAgent->Set_RimColor(_v4(0.f, 0.f, 1.f, 0.f));
	m_pBattleAgent->Set_OriginRimAlpha(1.f); //0.5 미묘하게 푸르댕댕
	m_pBattleAgent->Set_OriginRimValue(5.f);
	m_pBattleAgent->Set_RimAlpha(1.f);
	m_pBattleAgent->Set_RimValue(5.f);

	m_pPlayer = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);

	if (nullptr == m_pPlayer)
	{
		MSG_BOX("Can Find Player In YetiTrap Ready_Default");
		return E_FAIL;
	}

	return S_OK;
}

CYetiTrap* CYetiTrap::Create(_Device _pGraphicDev)
{
	CYetiTrap* pInstance = new CYetiTrap(_pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CYetiTrap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CYetiTrap::Clone_GameObject(void * pArg)
{
	CYetiTrap* pInstance = new CYetiTrap(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CYetiTrap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CYetiTrap::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pCollider);
	Safe_Release(m_pMesh_Static);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pBattleAgent);
	Safe_Release(m_pOptimization);

	CGameObject::Free();

	return;
}
