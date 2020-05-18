#include "stdafx.h"
#include "Costume_Body.h"


CCostume_Body::CCostume_Body(_Device pGraphicDev)
	:CGameObject(pGraphicDev)
{
}

CCostume_Body::CCostume_Body(const CCostume_Body& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCostume_Body::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CCostume_Body::Ready_GameObject(void * pArg)
{
	_INFO pInfo = *(_INFO*)pArg;

	m_pTarget = pInfo.pTarget;
	m_pmatParent = pInfo.pmatParent;
	m_pmatBone = pInfo.pmatBone;
	m_vColorValue = pInfo.vColorValue;
	m_eBodyType = pInfo.eBodyTag;

	m_fAnimMultiply = 1.f;

	_tchar szMeshName[STR_128] = L"";

	lstrcpy(szMeshName,
		pInfo.eBodyTag == Chara_Body_01 ? L"Mesh_BodyInner_01" :
		pInfo.eBodyTag == Chara_Body_02 ? L"Mesh_BodyInner_02" :
		pInfo.eBodyTag == Chara_Body_03 ? L"Mesh_BodyInner_03" :
		pInfo.eBodyTag == Chara_Body_04 ? L"Mesh_BodyInner_04" : L"Mesh_BodyInner_01");

	if (FAILED(Add_Components(szMeshName)))
		return E_FAIL;

	if (FAILED(Setup_Default()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCostume_Body::Add_Components(const _tchar * pMeshName)
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, pMeshName, L"Com_MeshDynamic", (CComponent**)&m_pDynamicMesh)))
		return E_FAIL;

	// for.Com_BattleAgent
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"BattleAgent", L"Com_BattleAgent", (CComponent**)&m_pBattleAgent)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCostume_Body::Setup_Default()
{
	m_pTransform->Set_Pos(V3_NULL);
	m_pTransform->Set_Angle(V3_NULL);
	m_pTransform->Set_Scale(V3_ONE);

	return S_OK;
}

HRESULT CCostume_Body::SetUp_ConstantTable(CShader* pShader)
{
	if (nullptr == pShader)
		return E_FAIL;

	_mat		ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat		ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	//=============================================================================================
	// 기본 메트릭스
	//=============================================================================================

	if (FAILED(pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;

	//=============================================================================================
	// 디졸브용 상수
	//=============================================================================================
	if (FAILED(g_pDissolveTexture->SetUp_OnShader("g_FXTexture", pShader)))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fFxAlpha", &m_fFXAlpha, sizeof(_float))))
		return E_FAIL;

	//=============================================================================================
	// 쉐이더 재질정보 수치 입력
	//=============================================================================================
	_float	fEmissivePower = 0.f;	// 이미시브 : 높을 수록, 자체 발광이 강해짐.
	_float	fSpecularPower = 1.f;	// 메탈니스 : 높을 수록, 정반사가 강해짐.
	_float	fRoughnessPower = 1.f;	// 러프니스 : 높을 수록, 빛 산란이 적어짐(빛이 응집됨).
	_float	fMinSpecular = 1.f;	// 최소 빛	: 최소 단위의 빛을 더해줌.
	_float	fID_R = 1.0f;	// ID_R : R채널 ID 값 , 1이 최대
	_float	fID_G = 0.5f;	// ID_G : G채널 ID 값 , 1이 최대
	_float	fID_B = 0.1f;	// ID_B	: B채널 ID 값 , 1이 최대

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

	m_pBattleAgent->Update_RimParam_OnShader(pShader);

	return NOERROR;
}

void CCostume_Body::Calc_AttachBoneTransform()
{
	_mat tmpMat;
	D3DXMatrixIdentity(&tmpMat);

	//memcpy(&tmpMat._41, &(*m_pmatBone)._41, sizeof(_v3));
	//memcpy(&tmpMat, m_pmatBone, sizeof(_mat));

	m_pTransform->Calc_ParentMat(&(tmpMat * *m_pmatParent));
}

void CCostume_Body::SetUp_Animation(_uint iIndex, _bool _bOffLerp)
{
	m_pDynamicMesh->SetUp_Animation(iIndex, _bOffLerp);
}

_int CCostume_Body::Update_GameObject(_double TimeDelta)
{
	//if (false == m_bEnable)
	//	return NO_EVENT;

	//CGameObject::Update_GameObject(TimeDelta);
	//Calc_AttachBoneTransform();

	return NO_EVENT;
}

_int CCostume_Body::Update_GameObject(_double TimeDelta, _bool bSkill)
{
	if (false == m_bEnable)
		return NO_EVENT;

	CGameObject::Update_GameObject(TimeDelta);
	Calc_AttachBoneTransform();


	return NO_EVENT;
}

_int CCostume_Body::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	if (!m_tObjParam.bInvisible)
	{
		if (FAILED(m_pRenderer->Add_RenderList(RENDER_NONALPHA, this)))
			return E_FAIL;
	}

	else if (m_tObjParam.bInvisible)
	{
		if (FAILED(m_pRenderer->Add_RenderList(RENDER_ALPHA, this)))
			return E_FAIL;
	}

	if (false == m_bDissolve)
	{
		if (!m_tObjParam.bInvisible)
		{
			if (FAILED(m_pRenderer->Add_RenderList(RENDER_MOTIONBLURTARGET, this)))
				return E_FAIL;

			if (FAILED(m_pRenderer->Add_RenderList(RENDER_SHADOWTARGET, this)))
				return E_FAIL;
		}
	}

	if (false == m_bEnable)
		return NO_EVENT;

	if (nullptr == m_pRenderer ||
		nullptr == m_pDynamicMesh)
		return E_FAIL;

	//m_pDynamicMesh->SetUp_Animation_Lower(m_eAnimLower, m_bLerpOff);
	//m_pDynamicMesh->SetUp_Animation_Upper(m_eAnimUpper, m_bLerpOff);
	//m_pDynamicMesh->SetUp_Animation_RightArm(m_eAnimRight, m_bLerpOff);
	//m_pDynamicMesh->SetUp_Animation_LeftArm(m_eAnimLeft, m_bLerpOff);

	return NO_EVENT;
}

HRESULT CCostume_Body::Render_GameObject()
{
	if (false == m_bEnable)
		return S_OK;

	if (nullptr == m_pShader ||
		nullptr == m_pDynamicMesh)
		return E_FAIL;

	//m_pDynamicMesh->SetUp_Animation(0);

	//m_pDynamicMesh->Play_Animation_Lower(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMultiply);
	//m_pDynamicMesh->Play_Animation_Upper(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMultiply);
	//m_pDynamicMesh->Play_Animation_RightArm(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMultiply, false);
	//m_pDynamicMesh->Play_Animation_LeftArm(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMultiply);

	m_pDynamicMesh->Play_Animation(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMultiply);

	if (m_tObjParam.bInvisible)
		return S_OK;

	if (FAILED(SetUp_ConstantTable(m_pShader)))
		return E_FAIL;

	m_pShader->Begin_Shader();

	_uint iNumMeshContainer = _uint(m_pDynamicMesh->Get_NumMeshContainer());
	// 메쉬 컨테이너는 3개

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pDynamicMesh->Get_NumMaterials(i);
		// 서브셋은 5개

		// 메시를 뼈에 붙인다.
		m_pDynamicMesh->Update_SkinnedMesh(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			m_iPass = m_pDynamicMesh->Get_MaterialPass(i, j);

			if (m_bDissolve)
				m_iPass = 3;

			m_pShader->Begin_Pass(m_iPass);

			m_pShader->Set_DynamicTexture_Auto(m_pDynamicMesh, i, j);

			if (13 == m_iPass)
			{
				_float fSpec = 0.1f;

				if (FAILED(m_pShader->Set_Value("g_fSpecularPower", &fSpec, sizeof(_float))))
					return E_FAIL;
			}

			m_pShader->Commit_Changes();

			m_pDynamicMesh->Render_Mesh(i, j);

			m_pShader->End_Pass();
		}
	}

	m_pShader->End_Shader();

	return NOERROR;
}

HRESULT CCostume_Body::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	if (false == m_bEnable)
		return S_OK;

	if (nullptr == pShader ||
		nullptr == m_pDynamicMesh)
		return E_FAIL;

	m_pDynamicMesh->Play_Animation_Lower(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMutiply);
	m_pDynamicMesh->Play_Animation_Upper(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMutiply);
	m_pDynamicMesh->Play_Animation_RightArm(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMutiply, false);
	m_pDynamicMesh->Play_Animation_LeftArm(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMutiply);


	if (m_tObjParam.bInvisible)
		return S_OK;

	if (FAILED(SetUp_ConstantTable(pShader)))
		return E_FAIL;


	_uint iNumMeshContainer = _uint(m_pDynamicMesh->Get_NumMeshContainer());
	// 메쉬 컨테이너는 3개

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		if (i == 1)
			continue;

		_uint iNumSubSet = (_uint)m_pDynamicMesh->Get_NumMaterials(i);
		// 서브셋은 5개

		// 메시를 뼈에 붙인다.
		m_pDynamicMesh->Update_SkinnedMesh(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			m_iPass = m_pDynamicMesh->Get_MaterialPass(i, j);

			if (m_bDissolve)
				m_iPass = 3;

			pShader->Begin_Pass(m_iPass);

			pShader->Set_DynamicTexture_Auto(m_pDynamicMesh, i, j);

			pShader->Commit_Changes();

			m_pDynamicMesh->Render_Mesh(i, j);

			pShader->End_Pass();
		}
	}


	return NOERROR;
}

HRESULT CCostume_Body::Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur)
{
	if (false == m_bEnable)
		return S_OK;

	if (nullptr == pShader ||
		nullptr == m_pDynamicMesh)
		return E_FAIL;

	//============================================================================================
	// 공통 변수
	//============================================================================================

	_mat	ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat	ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);
	_mat	WorldMatrix = m_pTransform->Get_WorldMat();

	if (FAILED(pShader->Set_Value("g_matWorld", &WorldMatrix, sizeof(_mat))))
		return E_FAIL;

	//============================================================================================
	// 모션 블러 상수
	//============================================================================================
	if (_bIsForMotionBlur)
	{
		if (FAILED(pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(pShader->Set_Value("g_matLastWVP", &m_matLastWVP, sizeof(_mat))))
			return E_FAIL;

		m_matLastWVP = WorldMatrix * ViewMatrix * ProjMatrix;

		_bool bMotionBlur = true;
		if (FAILED(pShader->Set_Bool("g_bMotionBlur", bMotionBlur)))
			return E_FAIL;
		_bool bDecalTarget = false;
		if (FAILED(pShader->Set_Bool("g_bDecalTarget", bDecalTarget)))
			return E_FAIL;
		_float fBloomPower = 0.f;
		if (FAILED(pShader->Set_Value("g_fBloomPower", &fBloomPower, sizeof(_float))))
			return E_FAIL;
	}

	//============================================================================================
	// 기타 상수
	//============================================================================================
	else
	{
		_mat matWVP = WorldMatrix * ViewMatrix * ProjMatrix;

		if (FAILED(pShader->Set_Value("g_matWVP", &matWVP, sizeof(_mat))))
			return E_FAIL;
	}

	//============================================================================================
	// 쉐이더 실행
	//============================================================================================
	_uint iNumMeshContainer = _uint(m_pDynamicMesh->Get_NumMeshContainer());

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pDynamicMesh->Get_NumMaterials(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			pShader->Begin_Pass(iPass);

			pShader->Commit_Changes();

			m_pDynamicMesh->Render_Mesh(i, j);

			pShader->End_Pass();
		}
	}

	//============================================================================================

	return S_OK;
}

CCostume_Body * CCostume_Body::Create(_Device pGraphicDev)
{
	CCostume_Body* pInstance = new CCostume_Body(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create Hair");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCostume_Body::Clone_GameObject(void * pArg)
{
	CCostume_Body* pInstance = new CCostume_Body(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned Hair");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCostume_Body::free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pDynamicMesh);
	Safe_Release(m_pBattleAgent);

	CGameObject::Free();
}