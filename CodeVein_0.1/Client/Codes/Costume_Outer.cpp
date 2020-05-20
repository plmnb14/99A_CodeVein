#include "stdafx.h"
#include "Costume_Outer.h"


CCostume_Outer::CCostume_Outer(_Device pGraphicDev)
	:CGameObject(pGraphicDev)
{
}

CCostume_Outer::CCostume_Outer(const CCostume_Outer& rhs)
	: CGameObject(rhs)
{
}

void CCostume_Outer::Reset_OldAniIdx(_ulong _dwNumber)
{
	m_pDynamicMesh->Reset_OldIndx(1);
}

HRESULT CCostume_Outer::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CCostume_Outer::Ready_GameObject(void * pArg)
{
	_INFO pInfo = *(_INFO*)pArg;

	m_pTarget = pInfo.pTarget;
	m_pmatParent = pInfo.pmatParent;
	m_pmatBone = pInfo.pmatBone;
	m_vColorValue = pInfo.vColorValue;

	m_fAnimMultiply = 1.f;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Setup_Default()))
		return E_FAIL;

	m_pBattleAgent->Set_OriginRimAlpha(0.f);
	m_pBattleAgent->Set_OriginRimValue(7.f);
	m_pBattleAgent->Set_RimAlpha(0.f);
	m_pBattleAgent->Set_RimValue(7.f);

	return S_OK;
}

HRESULT CCostume_Outer::Add_Components()
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_Drape_01", L"Com_MeshDynamic", (CComponent**)&m_pDynamicMesh)))
		return E_FAIL;

	// for.Com_BattleAgent
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"BattleAgent", L"Com_BattleAgent", (CComponent**)&m_pBattleAgent)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCostume_Outer::Setup_Default()
{
	m_pTransform->Set_Pos(V3_NULL);
	m_pTransform->Set_Angle(V3_NULL);
	m_pTransform->Set_Scale(V3_ONE);

	return S_OK;
}

HRESULT CCostume_Outer::SetUp_ConstantTable(CShader* pShader)
{
	if (nullptr == pShader)
		return E_FAIL;

	//=============================================================================================
	// 기본 메트릭스
	//=============================================================================================

	if (FAILED(pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;
	//=============================================================================================
	// 디졸브용 상수
	//=============================================================================================
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

void CCostume_Outer::Calc_AttachBoneTransform()
{
	_mat tmpMat;
	D3DXMatrixIdentity(&tmpMat);

	//memcpy(&tmpMat._41, &(*m_pmatBone)._41, sizeof(_v3));
	//memcpy(&tmpMat, m_pmatBone, sizeof(_mat));

	m_pTransform->Calc_ParentMat(&(tmpMat * *m_pmatParent));
}

void CCostume_Outer::Change_OuterMesh(CClothManager::Cloth_Dynamic _eOuterType)
{
	if (_eOuterType == m_eOuterType)
		return;

	_tchar szMeshName[STR_128] = L"";

	switch (_eOuterType)
	{
	case CClothManager::None:
	{
		lstrcpy(szMeshName, L"Mesh_Drape_01");
		m_eOuterType = CClothManager::Cloth_Dynamic::None;
		break;
	}
	case CClothManager::Drape_01:
	{
		lstrcpy(szMeshName, L"Mesh_Drape_01");
		m_eOuterType = CClothManager::Drape_01;
		break;

	}
	case CClothManager::Gauntlet_01:
	{
		lstrcpy(szMeshName, L"Mesh_Gauntlet_01");
		m_eOuterType = CClothManager::Gauntlet_01;
		break;
	}
	case CClothManager::Gauntlet_03:
	{
		lstrcpy(szMeshName, L"Mesh_Gauntlet_03");
		m_eOuterType = CClothManager::Gauntlet_03;
		break;
	}
	case CClothManager::LongCoat_01:
	{
		lstrcpy(szMeshName, L"Mesh_LongCoat_01");
		m_eOuterType = CClothManager::LongCoat_01;
		break;
	}
	case CClothManager::LongCoat_02:
	{
		lstrcpy(szMeshName, L"Mesh_LongCoat_02");
		m_eOuterType = CClothManager::LongCoat_02;
		break;
	}
	case CClothManager::LongCoat_03:
	{
		lstrcpy(szMeshName, L"Mesh_LongCoat_03");
		m_eOuterType = CClothManager::LongCoat_03;
		break;
	}
	case CClothManager::Muffler_01:
	{
		lstrcpy(szMeshName, L"Mesh_Muffler_01");
		m_eOuterType = CClothManager::Muffler_01;
		break;
	}
	case CClothManager::Muffler_02:
	{
		lstrcpy(szMeshName, L"Mesh_Muffler_02");
		m_eOuterType = CClothManager::Muffler_02;
		break;
	}
	}

	// 컴포넌트에 있는 매쉬 찾아서
	auto& iter = m_pmapComponents.find(L"Com_MeshDynamic");

	// Static 멤버변수는 처음에 Clone 할때 AddRef 해준다., 
	Safe_Release(m_pDynamicMesh);
	Safe_Release(iter->second);

	// Release 한 컴포넌트에 새로이 Clone 받음
	iter->second = m_pDynamicMesh = static_cast<CMesh_Dynamic*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, szMeshName));
	Safe_AddRef(iter->second);

	g_pClothManager->Set_Sleep_AllDynamic();
}

void CCostume_Outer::Change_Vertex()
{
	physx::PxSceneWriteLock scopedLock(*g_pPhysx->Get_Scene());

	physx::PxCloth* pCloth = g_pClothManager->Get_Cloth_Dynamic(m_eOuterType);

	physx::PxClothFabric* pFabric = pCloth->getFabric();
	physx::PxClothParticleData* pData = pCloth->lockParticleData();

	LPD3DXMESH	pMesh = m_pDynamicMesh->Get_MeshContainer()[0]->pOriginalMesh;
	//LPD3DXMESH	pMesh = m_pDynamicMesh->Get_MeshContainer()[0]->MeshData.pMesh;
	_ulong dwStride = pMesh->GetNumBytesPerVertex();

	_byte* pVertices = nullptr;

	pMesh->LockVertexBuffer(0, (void**)&pVertices); 


	_ulong adsfas = _ulong(pFabric->getNbParticles());

	for (_ulong i = 0; i < _ulong(pFabric->getNbParticles()); ++i)
	{
		*(_v3*)(pVertices + (i * dwStride)) = *(_v3*)(pData->particles + i);
	}

	pMesh->UnlockVertexBuffer();
}

void CCostume_Outer::SetUp_Animation(_uint iIndex, _bool _bOffLerp)
{
	m_pDynamicMesh->SetUp_Animation(iIndex, _bOffLerp);
}

_int CCostume_Outer::Update_GameObject(_double TimeDelta)
{
	//if (false == m_bEnable)
	//	return NO_EVENT;

	//CGameObject::Update_GameObject(TimeDelta);
	//Calc_AttachBoneTransform();

	return NO_EVENT;
}

_int CCostume_Outer::Update_GameObject(_double TimeDelta, _bool bClearCol)
{
	if (false == m_bEnable)
		return NO_EVENT;

	CGameObject::Update_GameObject(TimeDelta);
	Calc_AttachBoneTransform();

	static _byte iCount = 0;

	if (g_pInput_Device->Key_Down(DIK_G))
	{
		++iCount;

		Change_OuterMesh(CClothManager::Cloth_Dynamic(iCount));

		if (CClothManager::Cloth_Dynamic::Dynamic_End - 1 == iCount)
			iCount = CClothManager::Cloth_Dynamic::None;
	}


	if(g_pClothManager->Is_Valid_Dynamic(m_eOuterType))
		g_pClothManager->Update_Cloth_Dynamic(m_eOuterType, bClearCol);



	return NO_EVENT;
}

_int CCostume_Outer::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	// 옷 없을 때 예외처리
	if (m_eOuterType == CClothManager::Cloth_Dynamic::None || false == g_pClothManager->Is_Valid_Dynamic(m_eOuterType))
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

HRESULT CCostume_Outer::Render_GameObject()
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

HRESULT CCostume_Outer::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	IF_NULL_VALUE_RETURN(pShader, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pDynamicMesh, E_FAIL);

	//// 버텍스 교체
	if (m_eOuterType != CClothManager::Cloth_Dynamic::None)
		Change_Vertex();

	m_pDynamicMesh->Play_Animation_Lower(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMultiply);
	m_pDynamicMesh->Play_Animation_Upper(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMultiply);
	m_pDynamicMesh->Play_Animation_RightArm(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMultiply, false);
	m_pDynamicMesh->Play_Animation_LeftArm(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMultiply);

	//m_pDynamicMesh->Play_Animation(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMultiply);

	if (m_tObjParam.bInvisible)
		return S_OK;

	if (FAILED(SetUp_ConstantTable(pShader)))
		return E_FAIL;

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

			pShader->Begin_Pass(m_iPass);

			pShader->Set_DynamicTexture_Auto(m_pDynamicMesh, i, j);

			pShader->Commit_Changes();

			m_pDynamicMesh->Render_Mesh(i, j);

			pShader->End_Pass();
		}
	}

	return S_OK;
}

HRESULT CCostume_Outer::Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur)
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

void CCostume_Outer::Set_LowerAnimation(_ulong _dwAnimIdx, _bool _bOffLerp)
{
	m_pDynamicMesh->SetUp_Animation_Lower(_dwAnimIdx, _bOffLerp);
}

void CCostume_Outer::Set_UpperAnimation(_ulong _dwAnimIdx, _bool _bOffLerp)
{
	m_pDynamicMesh->SetUp_Animation_Upper(_dwAnimIdx, _bOffLerp);

}

void CCostume_Outer::Set_LeftArmAnimation(_ulong _dwAnimIdx, _bool _bOffLerp)
{
	m_pDynamicMesh->SetUp_Animation_LeftArm(_dwAnimIdx, _bOffLerp);

}

void CCostume_Outer::Set_RightArmAnimation(_ulong _dwAnimIdx, _bool _bOffLerp)
{
	m_pDynamicMesh->SetUp_Animation_RightArm(_dwAnimIdx, _bOffLerp);

}

CCostume_Outer * CCostume_Outer::Create(_Device pGraphicDev)
{
	CCostume_Outer* pInstance = new CCostume_Outer(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create Hair");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCostume_Outer::Clone_GameObject(void * pArg)
{
	CCostume_Outer* pInstance = new CCostume_Outer(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned Hair");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCostume_Outer::free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pDynamicMesh);
	Safe_Release(m_pBattleAgent);

	CGameObject::Free();
}