#include "stdafx.h"
#include "Costume_Head.h"


CCostume_Head::CCostume_Head(_Device pGraphicDev)
	:CGameObject(pGraphicDev)
{
}

CCostume_Head::CCostume_Head(const CCostume_Head& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCostume_Head::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CCostume_Head::Ready_GameObject(void * pArg)
{
	_INFO pInfo = *(_INFO*)pArg;

	// 대상
	m_pTarget = pInfo.pTarget;

	// 부모 위치
	m_pmatParent = pInfo.pmatParent;

	// 부모 뼈 위치
	m_pmatBone = pInfo.pmatBone;

	// 색상
	m_vColorValue = pInfo.vColorValue;

	// 애니 속도
	m_fAnimMultiply = 1.f;

	_tchar szMeshName[STR_128] = L"";

	lstrcpy(szMeshName , 
		pInfo.eHeadTag == Chara_Head_01 ? L"Mesh_Face_01" :
		pInfo.eHeadTag == Chara_Head_02 ? L"Mesh_Face_02" :
		pInfo.eHeadTag == Chara_Head_03 ? L"Mesh_Face_03" :
		pInfo.eHeadTag == Chara_Head_04 ? L"Mesh_Face_04" :
		pInfo.eHeadTag == Chara_Head_05 ? L"Mesh_Face_05" :
		pInfo.eHeadTag == Chara_Head_06 ? L"Mesh_Face_06" :
		pInfo.eHeadTag == Chara_Head_07 ? L"Mesh_Face_07" :
		pInfo.eHeadTag == Chara_Head_08 ? L"Mesh_Face_08" :
		pInfo.eHeadTag == Chara_Head_09 ? L"Mesh_Face_09" : L"Mesh_Face_10");

	if (FAILED(Add_Components(szMeshName)))
		return E_FAIL;

	if (FAILED(Setup_Default()))
		return E_FAIL;

	m_pBattleAgent->Set_OriginRimAlpha(1.f);
	m_pBattleAgent->Set_OriginRimValue(5.f);
	m_pBattleAgent->Set_RimAlpha(1.f);
	m_pBattleAgent->Set_RimValue(5.f);

	return S_OK;
}

HRESULT CCostume_Head::Add_Components(const _tchar * pMeshName)
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, pMeshName, L"Com_StaticMesh", (CComponent**)&m_pStaticMesh)))
		return E_FAIL;

	// for.Com_BattleAgent
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"BattleAgent", L"Com_BattleAgent", (CComponent**)&m_pBattleAgent)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCostume_Head::Setup_Default()
{
	m_pTransform->Set_Pos(V3_NULL);
	m_pTransform->Set_Angle(V3_NULL);
	m_pTransform->Set_Scale(V3_ONE);
	m_pTransform->Set_Angle(AXIS_X, D3DXToRadian(-90.f));

	return S_OK;
}

HRESULT CCostume_Head::SetUp_ConstantTable(CShader* pShader)
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

void CCostume_Head::Calc_AttachBoneTransform()
{
	m_pTransform->Calc_ParentMat(&(*m_pmatBone * *m_pmatParent));
}

void CCostume_Head::SetUp_Animation(_uint iIndex, _bool _bOffLerp)
{
}

_int CCostume_Head::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	CGameObject::Update_GameObject(TimeDelta);

	Calc_AttachBoneTransform();

	return NO_EVENT;
}

_int CCostume_Head::Late_Update_GameObject(_double TimeDelta)
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

	return NO_EVENT;
}

HRESULT CCostume_Head::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	if (m_tObjParam.bInvisible)
		return S_OK;

	if (FAILED(SetUp_ConstantTable(pShader)))
		return E_FAIL;

	_uint iNumSubSet = (_uint)m_pStaticMesh->Get_NumMaterials();

	_bool bOnToonRimLight = true;
	pShader->Set_Value("g_bToonRimLight", &bOnToonRimLight, sizeof(_bool));

	for (_uint i = 0; i < iNumSubSet; ++i)
	{
		m_iPass = m_pStaticMesh->Get_MaterialPass(i);

		if (m_bDissolve)
			m_iPass = 3;

		pShader->Begin_Pass(m_iPass);

		pShader->Set_StaticTexture_Auto(m_pStaticMesh, i);

		pShader->Commit_Changes();

		m_pStaticMesh->Render_Mesh(i);

		pShader->End_Pass();
	}

	bOnToonRimLight = false;
	pShader->Set_Value("g_bToonRimLight", &bOnToonRimLight, sizeof(_bool));

	return S_OK;
}

HRESULT CCostume_Head::Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur)
{
	//============================================================================================
	// 공통 변수
	//============================================================================================

	_mat	ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat	ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);
	_mat	WorldMatrix = m_pTransform->Get_WorldMat();

	_mat matWVP = WorldMatrix * ViewMatrix * ProjMatrix;

	if (FAILED(pShader->Set_Value("g_matWorld", &WorldMatrix, sizeof(_mat))))
		return E_FAIL;

	//============================================================================================
	// 모션 블러 상수
	//============================================================================================
	if (_bIsForMotionBlur)
	{
		if (FAILED(pShader->Set_Value("g_matLastWVP", &m_matLastWVP, sizeof(_mat))))
			return E_FAIL;

		m_matLastWVP = matWVP;

		_float fBloomPower = 20.f;
		if (FAILED(pShader->Set_Value("g_fBloomPower", &fBloomPower, sizeof(_float))))
			return E_FAIL;
	}

	//============================================================================================
	// 쉐이더 시작
	//============================================================================================

	_uint dwNumSubSet = m_pStaticMesh->Get_NumMaterials();
	_uint tmpPass = iPass;

	for (_uint i = 0; i < dwNumSubSet; ++i)
	{
		if (_bIsForMotionBlur)
		{
			tmpPass = m_pStaticMesh->Get_MaterialPass(i);

			if (14 == tmpPass || 15 == tmpPass)
			{
				// 눈썹 , 눈
				tmpPass = 3;
			}

			else if (22 == tmpPass)
			{
				// 얼굴
				tmpPass = 2;
			}

			else
				tmpPass = iPass;
		}

		pShader->Begin_Pass(tmpPass);

		pShader->Commit_Changes();

		m_pStaticMesh->Render_Mesh(i);

		pShader->End_Pass();
	}

	//============================================================================================


	return S_OK;
}

void CCostume_Head::Change_HeadMesh(CHAR_HEAD _eHeadType)
{
	if (_eHeadType == m_eHeadType)
		return;

	_tchar szMeshName[STR_128] = L"";

	switch (_eHeadType)
	{
	case Chara_Head_01:
	{
		lstrcpy(szMeshName, L"Mesh_Face_01");
		m_eHeadType = Chara_Head_01;
		break;
	}
	case Chara_Head_02:
	{
		lstrcpy(szMeshName, L"Mesh_Face_02");
		m_eHeadType = Chara_Head_02;
		break;
	}
	case Chara_Head_03:
	{
		lstrcpy(szMeshName, L"Mesh_Face_03");
		m_eHeadType = Chara_Head_03;
		break;
	}
	case Chara_Head_04:
	{
		lstrcpy(szMeshName, L"Mesh_Face_04");
		m_eHeadType = Chara_Head_04;
		break;
	}
	case Chara_Head_05:
	{
		lstrcpy(szMeshName, L"Mesh_Face_05");
		m_eHeadType = Chara_Head_05;
		break;
	}
	case Chara_Head_06:
	{
		lstrcpy(szMeshName, L"Mesh_Face_06");
		m_eHeadType = Chara_Head_06;
		break;
	}
	case Chara_Head_07:
	{
		lstrcpy(szMeshName, L"Mesh_Face_07");
		m_eHeadType = Chara_Head_07;
		break;
	}

	case Chara_Head_08:
	{
		lstrcpy(szMeshName, L"Mesh_Face_08");
		m_eHeadType = Chara_Head_08;
		break;
	}

	case Chara_Head_09:
	{
		lstrcpy(szMeshName, L"Mesh_Face_09");
		m_eHeadType = Chara_Head_09;
		break;
	}

	case Chara_Head_10:
	{
		lstrcpy(szMeshName, L"Mesh_Face_10");
		m_eHeadType = Chara_Head_10;
		break;
	}

	}

	// 컴포넌트에 있는 매쉬 찾아서
	auto& iter = m_pmapComponents.find(L"Com_StaticMesh");

	// Static 멤버변수는 처음에 Clone 할때 AddRef 해준다., 
	Safe_Release(m_pStaticMesh);
	Safe_Release(iter->second);

	// Release 한 컴포넌트에 새로이 Clone 받음
	iter->second = m_pStaticMesh = static_cast<CMesh_Static*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, szMeshName));
	Safe_AddRef(iter->second);
}

CCostume_Head * CCostume_Head::Create(_Device pGraphicDev)
{
	CCostume_Head* pInstance = new CCostume_Head(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create Hair");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCostume_Head::Clone_GameObject(void * pArg)
{
	CCostume_Head* pInstance = new CCostume_Head(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned Head");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCostume_Head::free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pStaticMesh);
	Safe_Release(m_pBattleAgent);

	CGameObject::Free();
}