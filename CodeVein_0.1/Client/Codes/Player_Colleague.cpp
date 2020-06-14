#include "stdafx.h"
#include "..\Headers\Player_Colleague.h"
#include "Player.h"
#include "Weapon.h"
#include "Colleague_Bullet.h"
#include "CollBullet_Heal.h"
#include "Colleague_UI.h"
#include "Colleague_Jack.h"

#include "ObjectPool_Manager.h"
#include "Haze.h"

#include "UI_Manager.h"


CPlayer_Colleague::CPlayer_Colleague(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CPlayer_Colleague::CPlayer_Colleague(const CPlayer_Colleague & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer_Colleague::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Colleague::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	JACK_INFO pInfo = *(JACK_INFO*)pArg;


	SetUp_Default();
	Ready_BoneMatrix();
	Ready_Collider();
	Ready_Weapon();

	m_pTransformCom->Set_Pos(pInfo.vPos);
	m_pTransformCom->Set_Angle(AXIS_Y, pInfo.fYAngle);

	//Teleport_ResetOptions(pArg);

	Check_Navi();

	m_pBattleAgentCom->Set_OriginRimAlpha(0.25f);
	m_pBattleAgentCom->Set_OriginRimValue(7.f);
	m_pBattleAgentCom->Set_RimAlpha(0.25f);
	m_pBattleAgentCom->Set_RimValue(7.f);

	m_bEnable = true;

	if (m_pNavMesh->Get_SubSetIndex() == -1)
		return E_FAIL;

	m_pCollJack = static_cast<CColleague_Jack*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Colleague_Jack", pArg));
	m_pCollJack->Set_Target(this);

	m_pColleagueUI = static_cast<CColleague_UI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Colleague_UI", pArg));
	m_pColleagueUI->Set_Target(this);

	return S_OK;
}

_int CPlayer_Colleague::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NOERROR;

	CPlayer* pPlayer = static_cast<CPlayer*>(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));
	if (nullptr != pPlayer && false == pPlayer->Get_Player_RigidBody())
		return NOERROR;

	if (nullptr != m_pTarget && true == m_pTarget->Get_Dead())
		return NOERROR;

	//====================================================================================================
	// 컬링
	//====================================================================================================
	m_bInFrustum = m_pOptimizationCom->Check_InFrustumforObject(&m_pTransformCom->Get_Pos(), 2.f);
	//====================================================================================================

	if (-1 == m_pNavMesh->Get_SubSetIndex())
		return E_FAIL;

	CGameObject::Update_GameObject(TimeDelta);

	m_pCollJack->Update_GameObject(TimeDelta);
	m_pColleagueUI->Update_GameObject(TimeDelta);

	if (m_eMovetype == CPlayer_Colleague::Coll_Dead)
		m_bIsDead = true;

	if (true == m_bNot_Recoverable && 0.f >= m_tObjParam.fHp_Cur)
		m_eMovetype = CPlayer_Colleague::Coll_Dead;

	if (false == m_bStage_LetsGo && m_eMovetype == CPlayer_Colleague::Coll_Move)
	{
		m_bStage_LetsGo = true;
		g_pSoundManager->Stop_Sound(CSoundManager::Jack_Voice);
		g_pSoundManager->Play_Sound(L"Jack_Let_Go.ogg", CSoundManager::Jack_Voice, CSoundManager::Voice_Sound);
	}


	Select_Heal();

	Check_MyHit();

	Check_Do_List(TimeDelta);
	Set_AniEvent();

	Function_Checking_AttCoolTime(m_fCoolTimer_limit);
	Function_Checking_SkilCoolTime(m_fCoolTimer_Skil_limit);
	Function_CoolTIme();

	m_pDynamicMesh->SetUp_Animation(m_eColleague_Ani);

	/*	cout << m_pSword->Get_Target_CanAttack() << endl;
	cout << "상태: " << m_eMovetype << " | " << "0 - Idle, 1 - Move, 3 - Att" << endl;
	cout << "공격상태: " << m_eColl_Sub_AttMoment << " | " << "Ani : " << m_eColleague_Ani << endl;*/

	//cout << "공격상태: " << m_eColl_DodgeMoment << " | " << m_iNormalAtt_Count << endl;

	if (false == m_bCheck_Be_Careful &&
		(true == m_bStart_Fighting && true == m_bNear_byMonster))
	{
		m_bCheck_Be_Careful = true;
		g_pSoundManager->Stop_Sound(CSoundManager::Jack_Voice);
		g_pSoundManager->Play_Sound(L"Be_Careful_For_Enemy.ogg", CSoundManager::Jack_Voice, CSoundManager::Voice_Sound);
	}
	//else if (false == m_bStart_Fighting && true == m_bCheck_Be_Careful)
	//{
	//	m_bCheck_Be_Careful = false;
	//}

	if (m_eMovetype != CPlayer_Colleague::Coll_Dead)
		Enter_Collision();

	IF_NOT_NULL(m_pSword)
		m_pSword->Update_GameObject(m_dTimeDelta);

	//}

	m_pNavMesh->Goto_Next_Subset(m_pTransformCom->Get_Pos(), nullptr);

	return S_OK;
}

_int CPlayer_Colleague::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NOERROR;

	IF_NULL_VALUE_RETURN(m_pRendererCom, E_FAIL);

	m_dTimeDelta = TimeDelta;

	m_pCollJack->Late_Update_GameObject(TimeDelta);
	m_pColleagueUI->Late_Update_GameObject(TimeDelta);

	if (!m_bDissolve)
	{
		if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
			return E_FAIL;
		if (FAILED(m_pRendererCom->Add_RenderList(RENDER_SHADOWTARGET, this)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pRendererCom->Add_RenderList(RENDER_ALPHA, this)))
			return E_FAIL;
	}

	if (m_bInFrustum && !m_bDissolve)
	{
		if (FAILED(m_pRendererCom->Add_RenderList(RENDER_MOTIONBLURTARGET, this)))
			return E_FAIL;
	}

	IF_NOT_NULL(m_pSword)
		m_pSword->Late_Update_GameObject(TimeDelta);

	return S_OK;
}

HRESULT CPlayer_Colleague::LateInit_GameObject()
{
	return S_OK;
}

HRESULT CPlayer_Colleague::Render_GameObject()
{
	IF_NULL_VALUE_RETURN(m_pShaderCom, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pDynamicMesh, E_FAIL);

	m_pDynamicMesh->Play_Animation(DELTA_60 * m_dPlayAni_Time);

	if (false == m_bEnable)
		return NOERROR;

	if (m_bInFrustum)
	{
		if (FAILED(g_pDissolveTexture->SetUp_OnShader("g_FXTexture", m_pShaderCom)))
			return E_FAIL;

		_mat matveiwView = g_pManagement->Get_Transform(D3DTS_VIEW);
		_mat matPro = g_pManagement->Get_Transform(D3DTS_PROJECTION);

		m_pShaderCom->Set_Value("g_matView", &matveiwView, sizeof(_mat));
		m_pShaderCom->Set_Value("g_matProj", &matPro, sizeof(_mat));

		if (FAILED(SetUp_ConstantTable(m_pShaderCom)))
			return E_FAIL;

		m_pShaderCom->Begin_Shader();

		_uint iNumMeshContainer = _uint(m_pDynamicMesh->Get_NumMeshContainer());

		for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
		{
			_uint iNumSubSet = (_uint)m_pDynamicMesh->Get_NumMaterials(i);

			m_pDynamicMesh->Update_SkinnedMesh(i);

			for (_uint j = 0; j < iNumSubSet; ++j)
			{
				if (CPlayer_Colleague::Coll_Dead != m_eMovetype)
					m_iPass = m_pDynamicMesh->Get_MaterialPass(i, j);

				if (m_bDissolve)
					m_iPass = 3;

				m_pShaderCom->Begin_Pass(m_iPass);

				m_pShaderCom->Set_DynamicTexture_Auto(m_pDynamicMesh, i, j);

				m_pShaderCom->Commit_Changes();

				m_pDynamicMesh->Render_Mesh(i, j);

				m_pShaderCom->End_Pass();
			}
		}
		m_pShaderCom->End_Shader();



	}

	Update_Collider();
	Render_Collider();

	return S_OK;
}

HRESULT CPlayer_Colleague::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	IF_NULL_VALUE_RETURN(pShader, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pDynamicMesh, E_FAIL);

	m_pDynamicMesh->Play_Animation(DELTA_60 * m_dPlayAni_Time);

	if (m_bInFrustum)
	{
		if (FAILED(SetUp_ConstantTable(pShader)))
			return E_FAIL;

		_uint iNumMeshContainer = _uint(m_pDynamicMesh->Get_NumMeshContainer());

		for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
		{
			_uint iNumSubSet = (_uint)m_pDynamicMesh->Get_NumMaterials(i);

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

	}

	Update_Collider();

	return S_OK;
}

HRESULT CPlayer_Colleague::Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur)
{
	if (false == m_bEnable)
		return S_OK;

	IF_NULL_VALUE_RETURN(pShader, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pDynamicMesh, E_FAIL);

	//============================================================================================
	// 공통 변수
	//============================================================================================
	_mat	ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat	ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);
	_mat	WorldMatrix = m_pTransformCom->Get_WorldMat();

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

		//_bool bMotionBlur = true;
		//if (FAILED(pShader->Set_Bool("g_bMotionBlur", bMotionBlur)))
		//	return E_FAIL;
		//_bool bDecalTarget = false;
		//if (FAILED(pShader->Set_Bool("g_bDecalTarget", bDecalTarget)))
		//	return E_FAIL;
		_float fBloomPower = 0.5f;
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
			_int tmpPass = m_pDynamicMesh->Get_MaterialPass(i, j);

			pShader->Begin_Pass(iPass);

			pShader->Commit_Changes();

			m_pDynamicMesh->Render_Mesh(i, j);

			pShader->End_Pass();
		}
	}

	//============================================================================================

	return S_OK;
}

void CPlayer_Colleague::Calling_Colleague(_bool _Calling_Colleague)
{
	if (true == _Calling_Colleague)
	{
		// 동료 소환

	}
	else
	{
		// 소환 해제
	}
}

void CPlayer_Colleague::Check_Navi()
{
	_tchar szNavData[STR_128] = L"";

	lstrcpy(szNavData, (
		g_eSceneID_Cur == 5 ? L"Navmesh_Training.dat" :
		g_eSceneID_Cur == 6 ? L"Navmesh_Stage_01.dat" :
		g_eSceneID_Cur == 7 ? L"Navmesh_Stage_02.dat" :
		g_eSceneID_Cur == 8 ? L"Navmesh_Stage_03.dat" : L"Navmesh_Stage_04.dat"));

	m_pTransformCom->Set_Pos(m_pTargetTransformCom->Get_Pos() - m_pTargetTransformCom->Get_Axis(AXIS_Z) * 2.f);

	m_pNavMesh->Reset_NaviMesh();
	m_pNavMesh->Ready_NaviMesh(m_pGraphic_Dev, szNavData);
	m_pNavMesh->Check_OnNavMesh(m_pTransformCom->Get_Pos());
}

HRESULT CPlayer_Colleague::Add_Component(void * pArg)
{
	_tchar MeshName[STR_128] = L"";

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_Buddy_Jack", L"Com_DynamicMesh", (CComponent**)&m_pDynamicMesh)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"NavMesh", L"Com_NavMesh", (CComponent**)&m_pNavMesh)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Optimization", L"Com_Optimization", (CComponent**)&m_pOptimizationCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"BattleAgent", L"Com_BattleAgent", (CComponent**)&m_pBattleAgentCom)))
		return E_FAIL;

	// 콜라이더 설정
	m_pCollider->Set_Radius(_v3(0.5f, 0.5f, 0.5f));
	m_pCollider->Set_Type(COL_SPHERE);
	m_pCollider->Set_Dynamic(true);		// 움직이는지
	m_pCollider->Set_CenterPos(m_pTransformCom->Get_Pos());

	return S_OK;
}

HRESULT CPlayer_Colleague::SetUp_Default()
{
	m_pTarget = static_cast<CPlayer*>(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));
	m_pTargetTransformCom = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));
	IF_NULL_VALUE_RETURN(m_pTargetTransformCom, E_FAIL);
	if (nullptr != m_pTargetTransformCom)
		Safe_AddRef(m_pTargetTransformCom);

	m_eMovetype = CPlayer_Colleague::Coll_Idle;
	m_tObjParam.fHp_Cur = m_tObjParam.fHp_Max;

	m_pTransformCom->Set_Scale(_v3(1.2f, 1.1f, 1.2f));

	m_List_pMonTarget[0] = &(g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE));
	m_List_pMonTarget[1] = &(g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE));

	m_tObjParam.fHp_Cur = 9500.f;
	m_tObjParam.fHp_Max = 9500.f;
	m_tObjParam.fDamage = 100.f;

	m_tObjParam.bCanHit = true;		// 맞을 수 있는지
	m_tObjParam.bIsHit = false;		// 맞는 도중인지
	m_tObjParam.bHitAgain = false;

	m_tObjParam.bCanAttack = true;	// 공격 가능한지
	m_tObjParam.bIsAttack = false;	// 공격 중인지

	m_tObjParam.bCanDodge = true;	// 회피 가능한지
	m_tObjParam.bIsDodge = false;	// 회피 중인지

	m_tObjParam.bCanGuard = true;	// 가드 가능?
	m_tObjParam.bIsGuard = false;	// 가드 중인지

	IF_NOT_NULL(m_pSword)
		m_pSword->Set_Target_CanAttack(false);

	return S_OK;
}

HRESULT CPlayer_Colleague::SetUp_ConstantTable(CShader * pShader)
{
	IF_NULL_VALUE_RETURN(pShader, E_FAIL);

	//=============================================================================================
	// 기본 메트릭스
	//=============================================================================================

	if (FAILED(pShader->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

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
	m_pBattleAgentCom->Update_RimParam_OnShader(m_pShaderCom);
	//=============================================================================================

	/*if (nullptr == m_pShaderCom)
	return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
	return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &g_pManagement->Get_Transform(D3DTS_VIEW), sizeof(_mat))))
	return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &g_pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_mat))))
	return E_FAIL;
	if (FAILED(g_pDissolveTexture->SetUp_OnShader("g_FXTexture", m_pShaderCom)))
	return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fFxAlpha", &m_fFXAlpha, sizeof(_float))))
	return E_FAIL;*/

	return S_OK;
}

HRESULT CPlayer_Colleague::Ready_BoneMatrix()
{
	// 0은 몬스터, 1은 플레이어

	D3DXFRAME_DERIVED*	pFrame = nullptr;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo("Spine", 0), E_FAIL);
	m_matBone[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBone[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo("Head", 0), E_FAIL);
	m_matBone[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	// 왼손
	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo("LeftHand", 0), E_FAIL);
	m_matBone[Bone_LHand] = &pFrame->CombinedTransformationMatrix;

	// 오른손
	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo("RightHand", 0), E_FAIL);
	m_matBone[Bone_RHand] = &pFrame->CombinedTransformationMatrix;

	return S_OK;
}

HRESULT CPlayer_Colleague::Ready_Collider()
{
	m_vecPhysicCol.reserve(3);

	// 경계 체크용 Collider
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));
	IF_NULL_VALUE_RETURN(pCollider, E_FAIL);

	_float fRadius = 1.3f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Range]->_41, m_matBone[Bone_Range]->_42, m_matBone[Bone_Range]->_43));
	pCollider->Set_Enabled(true);
	pCollider->Set_Type(COL_SPHERE);
	m_vecPhysicCol.push_back(pCollider);

	//=====================================

	// 맞는 용도 구
	// 몸통
	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 0.7f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Body]->_41, m_matBone[Bone_Body]->_42, m_matBone[Bone_Body]->_43));
	pCollider->Set_Enabled(true);
	pCollider->Set_Type(COL_SPHERE);
	m_vecPhysicCol.push_back(pCollider);

	//=====================================
	// 머리
	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 0.2f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Head]->_41, m_matBone[Bone_Head]->_42, m_matBone[Bone_Head]->_43));
	pCollider->Set_Enabled(true);
	pCollider->Set_Type(COL_SPHERE);
	m_vecPhysicCol.push_back(pCollider);


	return S_OK;
}

HRESULT CPlayer_Colleague::Ready_Weapon()
{
	m_pSword = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pSword->Change_WeaponData(Wpn_SSword_Slave);

	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo("RightHandAttach");
	m_pSword->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	m_pSword->Set_ParentMatrix(&m_pTransformCom->Get_WorldMat());
	m_pSword->Set_Friendly(true);		// 무기의 아군인지 적군인지 체크한다. true는 아군

	return S_OK;
}

void CPlayer_Colleague::Update_Collider()
{
	_ulong matrixIdx = 0;

	for (auto& iter : m_vecPhysicCol)
	{
		_mat tmpMat;
		tmpMat = *m_matBone[matrixIdx] * m_pTransformCom->Get_WorldMat();

		_v3 ColPos = _v3(tmpMat._41, tmpMat._42, tmpMat._43);

		iter->Update(ColPos);

		++matrixIdx;
	}

	m_pCollider->Update(m_pTransformCom->Get_Pos() + _v3(0.f, m_pCollider->Get_Radius().y, 0.f));
}

void CPlayer_Colleague::Render_Collider()
{
	for (auto& iter : m_vecPhysicCol)
	{
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}
}

void CPlayer_Colleague::Check_DeadEffect(_double TimeDelta)
{
	m_fDeadEffect_Delay -= _float(TimeDelta);
	if (m_fDeadEffect_Delay > 0.f)
		return;

	m_fDeadEffect_Offset -= _float(TimeDelta);
	if (m_fDeadEffect_Offset > 0.f)
		return;

	m_fDeadEffect_Offset = 0.1f;

	_v3 vPos = m_pTransformCom->Get_Pos();
	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo("Head");
	_v3 vHeadPos = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	pFamre = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo("Hips");
	_v3 vHipPos = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);

	CParticleMgr::Get_Instance()->Create_Effect_FinishPos(L"SpawnParticle", 0.1f, vPos, vHeadPos);
	CParticleMgr::Get_Instance()->Create_Effect_FinishPos(L"SpawnParticle_Sub", 0.1f, vPos, vHeadPos);

	CParticleMgr::Get_Instance()->Create_Effect(L"Monster_DeadSmoke_0", vHeadPos);
	CParticleMgr::Get_Instance()->Create_Effect(L"Monster_DeadSmoke_0", vHipPos);
	CParticleMgr::Get_Instance()->Create_Effect(L"Monster_DeadSmoke_0", vPos);

	return;
}

void CPlayer_Colleague::Check_Do_List(_double TimeDelta)
{
	if (m_eMovetype == CPlayer_Colleague::Coll_Hit ||
		m_eMovetype == CPlayer_Colleague::Coll_Dead ||
		m_eMovetype == CPlayer_Colleague::Coll_Heal ||
		m_eMovetype == CPlayer_Colleague::Coll_Dodge)
		return;

	//if (g_pInput_Device->Key_Down(DIK_F1))
	//	m_tObjParam.fHp_Cur += 500.f;
	//if (g_pInput_Device->Key_Down(DIK_F2))
	//	m_tObjParam.fHp_Cur -= 500.f;

	_float	fPlayerLength = V3_LENGTH(&(m_pTransformCom->Get_Pos() - m_pTargetTransformCom->Get_Pos()));

	// Player와의 최대 거리 체크
	m_fPlayer_DistDifference >= fPlayerLength ? m_bCheck_PlayerDist = true : m_bCheck_PlayerDist = false;

	
	if (true == m_bCheck_PlayerDist)		// 최대 거리 안에 있을 때
	{
		// 목표가 있을 때
		if (nullptr != m_pObject_Mon)
		{
			if (0.f >= m_pObject_Mon->Get_Target_Hp() ||
				false == m_pObject_Mon->Get_Enable() ||
				true == m_pObject_Mon->Get_Dead())
			{
				m_pObject_Mon = nullptr;

				Function_Reset_State();
				Function_Find_Target();

				if (nullptr != m_pObject_Mon)
					Check_Action_List(TimeDelta);
			}
			else
				Check_Action_List(TimeDelta);
		}
		else if (nullptr == m_pObject_Mon)
		{
			Function_Find_Target();

			if (nullptr != m_pObject_Mon)
			{
				if (0.f >= m_pObject_Mon->Get_Target_Hp() ||
					false == m_pObject_Mon->Get_Enable() ||
					true == m_pObject_Mon->Get_Dead())
				{
					m_pObject_Mon = nullptr;

					Function_Reset_State();
					Function_Find_Target();

					if (nullptr != m_pObject_Mon)
						Check_Action_List(TimeDelta);
				}
				else
					Check_Action_List(TimeDelta);
			}
		}

		if (false == m_bStart_Fighting && false == m_bNear_byMonster)
		{
			Select_Moving();
		}
	}
	else									// 최대 거리 밖에 있을 때
	{
		if (nullptr != m_pObject_Mon)
		{
			m_pObject_Mon = nullptr;
			m_bStart_Fighting = false;
			m_bNear_byMonster = false;
		}

		Check_Navi();
		if (false == m_bCheck_Dont_Go)
		{
			m_bCheck_Dont_Go = true;
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_Voice);
			g_pSoundManager->Play_Sound(L"Don_t_GoAway.ogg", CSoundManager::Jack_Voice, CSoundManager::Voice_Sound);
		}
	}
}

void CPlayer_Colleague::Check_Action_List(_double TimeDelta)
{
	_float	fMonTarget_Dist = V3_LENGTH(&(TARGET_TO_TRANS(m_pObject_Mon)->Get_Pos() - m_pTransformCom->Get_Pos()));

	m_fDodge_CoolTime += (_float)TimeDelta;

	if ((true == m_bStart_Fighting && true == m_bNear_byMonster) && m_fDodge_CoolTime >= 10.f)
	{
		_float fAngle = 0.f;
		if (nullptr != m_pObject_Mon)
			fAngle = D3DXToDegree(m_pTransformCom->Chase_Target_Angle(&TARGET_TO_TRANS(m_pObject_Mon)->Get_Pos()));

		// 전투 상태일 때, 10초마다 구르게
		//cout << "데굴데굴" << endl;
		m_eMovetype = CPlayer_Colleague::Coll_Dodge;

		if (0.f <= fAngle && 30.f > fAngle)
			m_eColl_DodgeMoment = CPlayer_Colleague::Dodge_BackRoll;
		else if (30.f <= fAngle && 90.f > fAngle)
			m_eColl_DodgeMoment = CPlayer_Colleague::Dodge_LeftRoll;
		else if (-150.f <= fAngle && -90.f > fAngle)
			m_eColl_DodgeMoment = CPlayer_Colleague::Dodge_BackRoll;
		else if (90.f <= fAngle && 150.f > fAngle)
			m_eColl_DodgeMoment = CPlayer_Colleague::Dodge_BackRoll;
		else if (150.f <= fAngle && 180.f > fAngle)
			m_eColl_DodgeMoment = CPlayer_Colleague::Dodge_FrontRoll;
		else if (-180.f <= fAngle && -150.f > fAngle)
			m_eColl_DodgeMoment = CPlayer_Colleague::Dodge_FrontRoll;
		else if (-90.f <= fAngle && -30.f > fAngle)
			m_eColl_DodgeMoment = CPlayer_Colleague::Dodge_RightRoll;
		else if (-30.f <= fAngle && 0 > fAngle)
			m_eColl_DodgeMoment = CPlayer_Colleague::Dodge_BackRoll;

		m_fDodge_CoolTime = 0;
	}

	switch (m_eTarget)
	{
	case CPlayer_Colleague::Coll_Target_Boss:
	{
		m_bNear_byMonster = true;

		m_eMovetype = CPlayer_Colleague::Coll_Attack;
		m_eColl_AttackMoment = CPlayer_Colleague::Att_Normal;
		break;
	}
	case CPlayer_Colleague::Coll_Target_Monster:
	{
		m_bNear_byMonster = true;

		m_eMovetype = CPlayer_Colleague::Coll_Attack;
		m_eColl_AttackMoment = CPlayer_Colleague::Att_Normal;
		break;
	}
	}
}

void CPlayer_Colleague::Check_MyHit()
{
	if (m_eMovetype == CPlayer_Colleague::Coll_Dead ||
		m_eMovetype == CPlayer_Colleague::Coll_Heal)
		return;

	if (0 < m_tObjParam.fHp_Cur)
	{
		if (false == m_tObjParam.bCanHit)
		{
			if (true == m_tObjParam.bIsHit)
			{
				if (true == m_tObjParam.bHitAgain)
				{
					m_eMovetype = CPlayer_Colleague::Coll_Hit;
					Function_FBRL();
					m_tObjParam.bHitAgain = false;
					m_pDynamicMesh->Reset_OldIndx();
				}
			}
			else
			{
				m_eMovetype = Colleague_Type::Coll_Hit;
				Function_FBRL();
			}
		}
	}
	else
		return;
}

void CPlayer_Colleague::Set_AniEvent()
{
	switch (m_eMovetype)
	{
	case CPlayer_Colleague::Coll_Idle:
	{
		switch (m_eColl_IdleMoment)
		{
		case CPlayer_Colleague::Idle_Waiting:
		{
			CollIdle_Waiting();
			m_eColleague_Ani = CPlayer_Colleague::Ani_Idle;
			// Idle_Waiting - 플레이어가 범위 안에 있고 주위에 몬스터도 없을 때
			break;
		}
		case CPlayer_Colleague::Idle_Guard:
		{
			// Idle_Guard - 플레이어와 적이 범위 내 존재하고 가드 중일 때
			break;
		}
		}
		break;
	}
	case CPlayer_Colleague::Coll_Move:
	{
		switch (m_eColl_MoveMent)
		{
		case Client::CPlayer_Colleague::Move_Walk:
		{
			// 플레이어와 일정 거리 이상으로 가까워졌을 때 걷는다
			CollMove_Walk();
			m_eColleague_Ani = CPlayer_Colleague::Ani_Front_Walk;
			break;
		}
		case CPlayer_Colleague::Move_BackWalk:
		{
			CollMove_BackWalk();
			m_eColleague_Ani = CPlayer_Colleague::Ani_Back_Walk;
			break;
		}
		case CPlayer_Colleague::Move_Run:
		{
			// 플레이어와 일정 거리 이상 떨어져 있을 때 뛰어서 쫒아간다
			CollMove_Run();
			m_eColleague_Ani = CPlayer_Colleague::Ani_Front_Run;
			break;
		}
		case CPlayer_Colleague::Move_BackRun:
		{
			CollMove_BackRun();
			m_eColleague_Ani = CPlayer_Colleague::Ani_Back_Run;
			break;
		}
		}
		break;
	}
	case CPlayer_Colleague::Coll_Guard:
	{
		switch (m_eColl_GuardMoment)
		{
		case Client::CPlayer_Colleague::Guard_Idle:
		{
			CollGuard_Idle();
			break;
		}
		case Client::CPlayer_Colleague::Gurad_Walk:
		{
			CollGuard_Walk();
			break;
		}
		case Client::CPlayer_Colleague::Gurad_Hit:
		{
			CollGuard_Hit();
			break;
		}
		}
		break;
	}
	case CPlayer_Colleague::Coll_Attack:
	{
		switch (m_eColl_AttackMoment)
		{
		case CPlayer_Colleague::Att_Skil:
		{
			// 전투 대기 상태 - 간보는 상태?
			CollAtt_Skil();
			break;
		}
		case CPlayer_Colleague::Att_Normal:
		{
			// 일반 공격
			CollAtt_Normal();
			break;
		}
		break;
		}

		switch (m_eColl_Sub_AttMoment)
		{
		case CPlayer_Colleague::Att_Base1:
		{
			CollAtt_Base1();
			m_eColleague_Ani = CPlayer_Colleague::One_Att;
			break;
		}
		case CPlayer_Colleague::Att_Base2:
		{
			CollAtt_Base2();
			m_eColleague_Ani = CPlayer_Colleague::Two_Att;
			break;
		}
		case CPlayer_Colleague::Att_Base3:
		{
			CollAtt_Base3();
			m_eColleague_Ani = CPlayer_Colleague::Three_Att;
			break;
		}
		case CPlayer_Colleague::Att_Base4:
		{
			CollAtt_Base4();
			m_eColleague_Ani = CPlayer_Colleague::Four_Att;
			break;
		}
		case CPlayer_Colleague::Att_ThreeCombo:
		{
			CollAtt_ThreeCombo();
			m_eColleague_Ani = CPlayer_Colleague::Ani_Trun_Center_Att_Skil;
			break;
		}
		case CPlayer_Colleague::Att_CenterDown:
		{
			CollAtt_CenterDown();
			m_eColleague_Ani = CPlayer_Colleague::Ani_Jump_CenterAtt_Skil;
			break;
		}
		case CPlayer_Colleague::Att_SlowGun:
		{
			CollAtt_SlowGun();
			m_eColleague_Ani = CPlayer_Colleague::Ani_PlayerHeal_or_Gun;
			break;
		}
		case CPlayer_Colleague::Att_MonWalk:
		{
			CollMove_MonWalk();
			m_eColleague_Ani = CPlayer_Colleague::Ani_Front_Walk;
			break;
		}
		case CPlayer_Colleague::Att_MonRun:
		{
			CollMove_MonRun();
			m_eColleague_Ani = CPlayer_Colleague::Ani_Front_Run;
			break;
		}
		case CPlayer_Colleague::Att_MonBackWalk:
		{
			CollMove_BackWalk();
			m_eColleague_Ani = CPlayer_Colleague::Ani_Back_Walk;
		}
		break;
		}
		break;
	}
	case CPlayer_Colleague::Coll_Dodge:
	{
		switch (m_eColl_DodgeMoment)
		{
		case CPlayer_Colleague::Dodge_FrontRoll:
		{
			CollDodge_FrontRoll();
			m_eColleague_Ani = CPlayer_Colleague::Ani_Front_Roll;
			break;
		}
		case CPlayer_Colleague::Dodge_BackRoll:
		{
			CollDodge_BackRoll();
			m_eColleague_Ani = CPlayer_Colleague::Ani_Back_Roll;
			break;
		}
		case CPlayer_Colleague::Dodge_LeftRoll:
		{
			CollDodge_LeftRoll();
			m_eColleague_Ani = CPlayer_Colleague::Ani_Left_Roll;
			break;
		}
		case CPlayer_Colleague::Dodge_RightRoll:
		{
			CollDodge_RightRoll();
			m_eColleague_Ani = CPlayer_Colleague::Ani_Right_Roll;
			break;
		}
		case CPlayer_Colleague::Dodge_BLeftRoll:
		{
			CollDodge_B_LeftRoll();
			m_eColleague_Ani = CPlayer_Colleague::Ani_BLeft_Roll;
			break;
		}
		case CPlayer_Colleague::Dodge_BRightRoll:
		{
			CollDodge_B_RightRoll();
			m_eColleague_Ani = CPlayer_Colleague::Ani_BRight_Roll;
			break;
		}
		}
	}
	case CPlayer_Colleague::Coll_Hit:
	{
		Play_Hit();
		break;
	}
	case CPlayer_Colleague::Coll_Heal:
	{
		switch (m_eColl_HealMoment)
		{
		case Client::CPlayer_Colleague::My_Heal:
		{
			if (m_eMovetype != CPlayer_Colleague::Coll_Dead)
			{
				m_eColleague_Ani = CPlayer_Colleague::Ani_Heal;
				CollHeal_ForMe();
			}
			break;
		}
		case Client::CPlayer_Colleague::Player_Heal:
		{
			m_eColleague_Ani = CPlayer_Colleague::Ani_PlayerHeal_or_Gun;
			CollHeal_ForPlayer();
			break;
		}
		}
		break;
	}
	case CPlayer_Colleague::Coll_Dead:
	{
		Play_Dead();
		m_eColleague_Ani = CPlayer_Colleague::Ani_Dead;
		if (false == m_bJack_Death)
		{
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_Voice);
			g_pSoundManager->Play_Sound(L"Jack_Death.ogg", CSoundManager::Jack_Voice, CSoundManager::Voice_Sound);
			m_bJack_Death = true;
		}

		break;
	}
	break;
	}
}

void CPlayer_Colleague::Colleague_Movement(_float fSpeed, _v3 vDir)
{
	V3_NORMAL(&vDir, &vDir);
	m_pTransformCom->Set_Pos(m_pNavMesh->Axis_Y_OnNavMesh(m_pTransformCom->Get_Pos()));
	m_pTransformCom->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransformCom->Get_Pos(), &vDir, fSpeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));
}

void CPlayer_Colleague::Colleague_SkilMovement(_float Multiply)
{
	m_fAtt_MoveSpeed_Cur -= (0.3f * m_fAtt_MoveAccel_Cur * m_fAtt_MoveAccel_Cur * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60")) * Multiply;
	m_fAtt_MoveAccel_Cur += g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60");

	if (m_fAtt_MoveSpeed_Cur < 0)
	{
		m_fAtt_MoveAccel_Cur = 0.5f;
		m_fAtt_MoveSpeed_Cur = 0.f;
	}

	return;
}

void CPlayer_Colleague::Select_Moving()
{
	_float	fPlayerLength = V3_LENGTH(&(m_pTransformCom->Get_Pos() - m_pTargetTransformCom->Get_Pos()));

	if(5.f <= fPlayerLength)
	{
		m_eMovetype = CPlayer_Colleague::Coll_Move;
		m_eColl_MoveMent = CPlayer_Colleague::Move_Run;
	}
	else if (2.f < fPlayerLength && 5.f > fPlayerLength)
	{
		m_eMovetype = CPlayer_Colleague::Coll_Move;
		m_eColl_MoveMent = CPlayer_Colleague::Move_Walk;
	}
	else if (2.f >= fPlayerLength)
	{
		m_eMovetype = CPlayer_Colleague::Coll_Idle;
		m_eColl_IdleMoment = CPlayer_Colleague::Idle_Waiting;
	}
}

void CPlayer_Colleague::Select_Heal()
{
	if (m_eMovetype == CPlayer_Colleague::Coll_Dead ||
		m_eMovetype == CPlayer_Colleague::Coll_Dodge)
		return;

	_float	fHPPercent = (m_tObjParam.fHp_Max * 30.f) / 100.f;
	_float	fTarget_HPPercent = (m_pTarget->Get_Target_Param().fHp_Max * 30.f) / 100.f;

	

	// 전체 HP에서 30% 남았을 때 자힐을 한다
	if ((fHPPercent >= m_tObjParam.fHp_Cur && false == m_bNot_Recoverable) 
		&& true == m_bNest_Skil_CoolTImer)
	{
		m_eMovetype = CPlayer_Colleague::Coll_Heal;
		m_eColl_HealMoment = CPlayer_Colleague::My_Heal;
		m_fCoolTimer_Skil_limit = 5.f;
		return;
	}

	// Player의 전체 HP에서 30% 남았을 때 힐을 해준다.
	if ((fTarget_HPPercent >= m_pTarget->Get_Target_Param().fHp_Cur) && 
		true == m_bNest_Skil_CoolTImer)
	{
		m_eMovetype = CPlayer_Colleague::Coll_Heal;
		m_fCoolTimer_Skil_limit = 5.f;
		m_eColl_HealMoment = CPlayer_Colleague::Player_Heal;
		return;
	}
}

void CPlayer_Colleague::Play_Start_Game()
{
	_double	AniTime = m_pDynamicMesh->Get_TrackInfo().Position;

	if (false == m_bCheck_SEndGame)
	{
		if (false == m_bCheck_StartGame)
		{
			m_bCheck_StartGame = true;
			m_eMovetype = CPlayer_Colleague::Coll_Start;
		}
		else
		{
			if (m_pDynamicMesh->Is_Finish_Animation(0.71f))
			{
				Function_Reset_State();
				m_eMovetype = CPlayer_Colleague::Coll_Idle;
				m_bCheck_SEndGame = true;
			}
			else
			{
				if (0.f <= AniTime)
				{
					if (false == m_bEventTrigger[0])
					{
						m_bEventTrigger[0] = true;
						Start_Dissolve(0.8f, false, true, 0.0f);
						m_fDeadEffect_Delay = 0.f;
						CObjectPool_Manager::Get_Instance()->Create_Object(L"GameObject_Haze",
							(void*)&CHaze::HAZE_INFO(_float(CCalculater::Random_Num(100, 300)), m_pTransformCom->Get_Pos(), 0.f));
					}
				}
			}
		}
	}
}

void CPlayer_Colleague::Play_Dead()
{
	// 죽었음
	_double	AniTime = m_pDynamicMesh->Get_TrackInfo().Position;

	if (false == m_bColleagueDead)
	{
		Function_Reset_State();
		m_bColleagueDead = true;
	}
	else
	{
		if (m_pDynamicMesh->Is_Finish_Animation(0.9f))
		{
			//Check_DeadEffect();
			m_bEnable = true;
			m_dPlayAni_Time = 0;
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_Voice);
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_01);
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_02);
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_03);
		}
	}
}

void CPlayer_Colleague::Play_Hit()
{
	if (m_eMovetype == CPlayer_Colleague::Coll_Dead || m_eMovetype == CPlayer_Colleague::Coll_Heal)
		return;

	if (false == m_tObjParam.bIsHit)
	{
		Function_Reset_State();
		m_tObjParam.bIsHit = true;

		// 히트 관련 동작 재생
		switch (m_eFBLR)
		{
		case CPlayer_Colleague::Coll_Front:
		{
			m_eColleague_Ani = CPlayer_Colleague::Ani_Front_Hit;
			Function_FBRL();
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_Voice);
			g_pSoundManager->Play_Sound(L"Jack_Damage_Ugh2.ogg", CSoundManager::Jack_Voice, CSoundManager::Voice_Sound);
			break;
		}
		case CPlayer_Colleague::Coll_Back:
		{
			m_eColleague_Ani = CPlayer_Colleague::Ani_Back_Hit;
			Function_FBRL();
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_Voice);
			g_pSoundManager->Play_Sound(L"Jack_Damage_Ugh.ogg", CSoundManager::Jack_Voice, CSoundManager::Voice_Sound);
			break;
		}
		}
	}
	else
	{
		if (m_pDynamicMesh->Is_Finish_Animation(0.86f))
		{
			m_tObjParam.bIsHit = false;
			m_tObjParam.bCanHit = true;

			m_fCoolTimer_limit = 0.5f;

			m_eMovetype = CPlayer_Colleague::Coll_Idle;
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_Voice);
		}
		else if (m_pDynamicMesh->Is_Finish_Animation(0.2f))
		{
			if (false == m_tObjParam.bCanHit)
			{
				m_tObjParam.bCanHit = true;
				Function_FBRL();
			}
		}
	}

	return;
}

void CPlayer_Colleague::Colleague_Guard()
{
}

void CPlayer_Colleague::CollMove_Walk()
{
	Function_RotateBody();
	Colleague_Movement(2.f, m_pTransformCom->Get_Axis(AXIS_Z));
}

void CPlayer_Colleague::CollMove_BackWalk()
{
	Function_RotateBody();
	Colleague_Movement(2.f, -(m_pTransformCom->Get_Axis(AXIS_Z)));
}

void CPlayer_Colleague::CollMove_Run()
{
	Function_RotateBody();
	Colleague_Movement(5.f, m_pTransformCom->Get_Axis(AXIS_Z));
}

void CPlayer_Colleague::CollMove_BackRun()
{
	Function_RotateBody();
	Colleague_Movement(5.f, -(m_pTransformCom->Get_Axis(AXIS_Z)));
}

void CPlayer_Colleague::CollMove_MonWalk()
{
	Function_RotateBody();
	Colleague_Movement(2.f, m_pTransformCom->Get_Axis(AXIS_Z));
}

void CPlayer_Colleague::CollMove_MonRun()
{
	Function_RotateBody();
	Colleague_Movement(5.f, m_pTransformCom->Get_Axis(AXIS_Z));
}

void CPlayer_Colleague::CollDodge_FrontRoll()
{
	_double AniTime = m_pDynamicMesh->Get_TrackInfo().Position;

	if (true == m_tObjParam.bCanDodge)
	{
		m_tObjParam.bCanDodge = false;
		m_tObjParam.bIsDodge = true;
		g_pSoundManager->Stop_Sound(CSoundManager::Jack_Voice);
		g_pSoundManager->Play_Sound(L"Jack_Dodge.ogg", CSoundManager::Jack_Voice, CSoundManager::Voice_Sound);
	}
	else
	{
		if (m_pDynamicMesh->Is_Finish_Animation(0.81f) && m_eColleague_Ani == CPlayer_Colleague::Ani_Front_Roll)
		{
			Function_Reset_State();
			m_bChecking_MyHit = true;
			m_fCoolTimer_limit = 5.f;
			m_eMovetype = CPlayer_Colleague::Coll_Idle;
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_Voice);
			return;
		}
		else if (0.067f <= AniTime && 1.5f >= AniTime)
		{
			// 여기서는 빠르게 해서 속도 확 높여서 구르는 구간
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_fAtt_MoveSpeed_Cur = 7.f;
				m_fAtt_MoveAccel_Cur = 0.f;
				m_fAni_Multiply = 0.5f;
			}
			Colleague_Movement(m_fAtt_MoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Colleague_SkilMovement(m_fAni_Multiply);
		}
	}
}

void CPlayer_Colleague::CollDodge_BackRoll()
{
	_double AniTime = m_pDynamicMesh->Get_TrackInfo().Position;

	if (true == m_tObjParam.bCanDodge)
	{
		m_tObjParam.bCanDodge = false;
		m_tObjParam.bIsDodge = true;
		g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_03);
		g_pSoundManager->Play_Sound(L"Jack_Dodge.ogg", CSoundManager::Jack_SFX_03, CSoundManager::Effect_Sound);
	}
	else
	{
		if (m_pDynamicMesh->Is_Finish_Animation(0.81f) && m_eColleague_Ani == CPlayer_Colleague::Ani_Back_Roll)
		{
			Function_Reset_State();
			m_bChecking_MyHit = true;
			m_fCoolTimer_limit = 5.f;
			m_eMovetype = CPlayer_Colleague::Coll_Idle;
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_Voice);
			return;
		}
		else if (0.3f <= AniTime && 1.467f >= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_fAtt_MoveSpeed_Cur = 7.f;
				m_fAtt_MoveAccel_Cur = 0.f;
				m_fAni_Multiply = 1.f;
			}
			Colleague_Movement(m_fAtt_MoveSpeed_Cur, -m_pTransformCom->Get_Axis(AXIS_Z));
			Colleague_SkilMovement(m_fAni_Multiply);
		}
	}
}

void CPlayer_Colleague::CollDodge_LeftRoll()
{
	_double AniTime = m_pDynamicMesh->Get_TrackInfo().Position;

	if (true == m_tObjParam.bCanDodge)
	{
		m_tObjParam.bCanDodge = false;
		m_tObjParam.bIsDodge = true;
		g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_03);
		g_pSoundManager->Play_Sound(L"Jack_Dodge.ogg", CSoundManager::Jack_SFX_03, CSoundManager::Effect_Sound);
	}
	else
	{
		if (m_pDynamicMesh->Is_Finish_Animation(0.81f) && m_eColleague_Ani == CPlayer_Colleague::Ani_Left_Roll)
		{
			Function_Reset_State();
			m_bChecking_MyHit = true;
			m_fCoolTimer_limit = 5.f;
			m_eMovetype = CPlayer_Colleague::Coll_Idle;
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_Voice);
			return;
		}
		else if (0.033f <= AniTime && 1.133f >= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_fAtt_MoveSpeed_Cur = 5.f;
				m_fAtt_MoveAccel_Cur = 0.f;	// 엑셀 값은 항상 0 초기화
				m_fAni_Multiply = 1.f;	// 감폭 수치. 값이 클수록 빨리 감소. 0일시 등속운동(원래는 감속) // 보통은 1 ~ 0.5사이
			}
			Colleague_Movement(m_fAtt_MoveSpeed_Cur, -m_pTransformCom->Get_Axis(AXIS_X));
			Colleague_SkilMovement(m_fAni_Multiply);
		}
	}
}

void CPlayer_Colleague::CollDodge_RightRoll()
{
	_double AniTime = m_pDynamicMesh->Get_TrackInfo().Position;

	if (true == m_tObjParam.bCanDodge)
	{
		m_tObjParam.bCanDodge = false;
		m_tObjParam.bIsDodge = true;
		g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_03);
		g_pSoundManager->Play_Sound(L"Jack_Dodge.ogg", CSoundManager::Jack_SFX_03, CSoundManager::Effect_Sound);
	}
	else
	{
		if (m_pDynamicMesh->Is_Finish_Animation(0.81f) && m_eColleague_Ani == CPlayer_Colleague::Ani_Right_Roll)
		{
			Function_Reset_State();
			m_bChecking_MyHit = true;
			m_fCoolTimer_limit = 5.f;
			m_eMovetype = CPlayer_Colleague::Coll_Idle;
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_Voice);
			return;
		}
		else if (0.067f <= AniTime && 1.267f >= AniTime)
		{
			// 여기서는 빠르게 해서 속도 확 높여서 구르는 구간
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_fAtt_MoveSpeed_Cur = 5.f;
				m_fAtt_MoveAccel_Cur = 0.f;	// 엑셀 값은 항상 0 초기화
				m_fAni_Multiply = 1.f;	// 감폭 수치. 값이 클수록 빨리 감소. 0일시 등속운동(원래는 감속) // 보통은 1 ~ 0.5사이
			}
			Colleague_Movement(m_fAtt_MoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_X));
			Colleague_SkilMovement(m_fAni_Multiply);
		}
	}
}

void CPlayer_Colleague::CollDodge_B_LeftRoll()
{
}

void CPlayer_Colleague::CollDodge_B_RightRoll()
{
}

void CPlayer_Colleague::CollIdle_Waiting()
{
	Function_RotateBody();
	Colleague_Movement(0.f, m_pTransformCom->Get_Axis(AXIS_Z));
}

void CPlayer_Colleague::CollAtt_Skil()
{
}

void CPlayer_Colleague::CollAtt_ThreeCombo()
{
	if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.8f) && m_eColleague_Ani == CPlayer_Colleague::Ani_Trun_Center_Att_Skil)
	{
		Function_Reset_State();
		m_eMovetype = CPlayer_Colleague::Coll_Idle;
		m_bTestRendom = true;
		m_bMyHiting = false;
		m_bCheck_Attcing = false;
		m_fCoolTimer_limit = 5.f;
		m_bChecking_MyHit = true;
		++m_iNormalAtt_Count;
		++m_iCenter_Count;
		g_pSoundManager->Stop_Sound(CSoundManager::Jack_Voice);
		g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_02);
		m_bCheck_Skil_Voice = false;
		return;
	}
	else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.7f)
	{
		if (m_bEventTrigger[6] == false)
		{
			m_bEventTrigger[6] = true;
			m_pSword->Set_Enable_Trail(false);
		}
	}

	else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.5f)
	{
		if (m_bEventTrigger[5] == false)
		{
			m_bEventTrigger[5] = true;

			m_pSword->Set_Target_CanAttack(false);
			m_pSword->Set_Enable_Record(false);
		}
	}

	else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.167f)
	{
		if (m_bEventTrigger[4] == false)
		{
			m_bEventTrigger[4] = true;
			m_pSword->Set_Target_CanAttack(true);
			m_pSword->Set_Enable_Record(true);
		}
	}

	else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.0f)
	{
		if (m_bEventTrigger[3] == false)
		{
			m_bEventTrigger[3] = true;

			m_pSword->Set_Enable_Trail(true);
		}
	}


	if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.6f && m_pDynamicMesh->Get_TrackInfo().Position < 3.367f)
	{
		if (m_bEventTrigger[2] == false)
		{
			m_bEventTrigger[2] = true;

			m_fAtt_MoveAccel_Cur = 0.f;
			m_fAtt_MoveSpeed_Cur = 0.2f;
			m_fAni_Multiply = 0.0f;
		}
		Function_RotateBody();
		Colleague_SkilMovement(m_fAni_Multiply);
		Colleague_Movement(m_fAtt_MoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
	}

	else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.366f && m_pDynamicMesh->Get_TrackInfo().Position < 2.067f)
	{
		if (m_bEventTrigger[1] == false)
		{
			m_bEventTrigger[1] = true;

			m_fAtt_MoveAccel_Cur = 0.f;
			m_fAtt_MoveSpeed_Cur = 0.2f;
			m_fAni_Multiply = 0.0f;
		}
		Function_RotateBody();
		Colleague_SkilMovement(m_fAni_Multiply);
		Colleague_Movement(m_fAtt_MoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
	}

	else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.f && m_pDynamicMesh->Get_TrackInfo().Position < 1.366f)
	{
		if (m_bEventTrigger[0] == false)
		{
			m_bEventTrigger[0] = true;

			m_fAtt_MoveAccel_Cur = 0.f;
			m_fAtt_MoveSpeed_Cur = 4.f;
			m_fAni_Multiply = 0.f;
		}
		Function_RotateBody();
		Colleague_SkilMovement(m_fAni_Multiply);
		Colleague_Movement(m_fAtt_MoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
	}

	_v3 vEffPos = _v3(0.f, 1.5f, 0.f) + m_pTransformCom->Get_Axis(AXIS_Z) * 1.5f;

	if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.7f)
	{
		if (m_bEventTrigger[9] == false)
		{
			m_bEventTrigger[9] = true;

			cout << "Skill_ShadowAssault_ScratchBlur_2226 : " << m_pDynamicMesh->Get_TrackInfo().Position << endl;

			g_pManagement->Create_Effect_Delay(L"Player_Skill_ShadowAssault_ScratchBlur", 0.f, vEffPos, m_pTransformCom);
			g_pManagement->Create_Effect_Delay(L"Player_Skill_ShadowAssault_Scratch", 0.f, vEffPos, m_pTransformCom);
			g_pManagement->Create_Effect_Delay(L"Hit_Slash_0", 0.f, vEffPos, m_pTransformCom);
			g_pManagement->Create_Effect_Delay(L"Hit_Slash_1", 0.f, vEffPos, m_pTransformCom);
			g_pManagement->Create_Effect_Delay(L"Hit_Slash_2", 0.f, vEffPos, m_pTransformCom);
			g_pManagement->Create_Effect_Delay(L"Hit_Slash_3", 0.f, vEffPos, m_pTransformCom);
			g_pManagement->Create_Effect_Delay(L"Hit_Slash_Particle_0", 0.f, vEffPos, m_pTransformCom);
			g_pManagement->Create_ParticleEffect_Delay(L"Player_Skill_RedParticle_Explosion", 0.1f, 0.f, vEffPos, m_pTransformCom);
		}
	}

	else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.133f)
	{
		if (m_bEventTrigger[8] == false)
		{
			m_bEventTrigger[8] = true;

			g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.f, m_pTransformCom->Get_Pos() + _v3(0, 0.5f, 0.f), nullptr);
		}
	}

	else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.1f)
	{
		if (m_bEventTrigger[7] == false)
		{
			m_bEventTrigger[7] = true;

			_v3 vEffPos = m_pTransformCom->Get_Pos() + _v3(0.f, 1.3f, 0.f);
			g_pManagement->Create_ParticleEffect_Delay(L"Player_Skill_Floor_BlackRing", 0.1f, 0.f, V3_NULL, m_pTransformCom);
			g_pManagement->Create_ParticleEffect_Delay(L"Player_Skill_Floor_RedRing", 0.1f, 0.f, V3_NULL, m_pTransformCom);
			g_pManagement->Create_ParticleEffect_Delay(L"Player_Skill_RotYRing_Red", 0.3f, 0.f, V3_NULL, m_pTransformCom);
			g_pManagement->Create_ParticleEffect_Delay(L"Player_Skill_RotYRing_Black", 0.3f, 0.f, V3_NULL, m_pTransformCom);
			g_pManagement->Create_ParticleEffect_Delay(L"Player_Skill_DarkSmokeAura", 0.5f, 0.f, _v3(0, 1.1f, 0.f), m_pTransformCom);
			g_pManagement->Create_Effect_Delay(L"Player_Skill_RedOnion_3", 0.15f, vEffPos);
			g_pManagement->Create_Effect_Delay(L"Player_Skill_RedOnion_3", 0.16f, vEffPos);
			g_pManagement->Create_Effect_Delay(L"Player_Skill_RedOnion_3", 0.17f, vEffPos);
			g_pManagement->Create_Effect_Delay(L"Player_Skill_RedOnion_3", 0.18f, vEffPos);
			g_pManagement->Create_Effect_Delay(L"Player_Skill_BloodTornadeMesh", 0.05f, V3_NULL, m_pTransformCom);
			g_pManagement->Create_Effect_Delay(L"Player_Skill_BloodTornadeMesh_2", 0.12f, V3_NULL, m_pTransformCom);
			g_pManagement->Create_Effect_Delay(L"Player_Skill_BloodTornadeMesh", 0.2f, V3_NULL, m_pTransformCom);
			g_pManagement->Create_Effect_Delay(L"Player_Skill_BloodTornadeMesh_2", 0.27f, V3_NULL, m_pTransformCom);
			g_pManagement->Create_Effect_Delay(L"Player_Skill_BloodTornadeMesh_3", 0.31f, V3_NULL, m_pTransformCom);
			g_pManagement->Create_Effect_Delay(L"Player_Skill_BloodTornadeMesh", 0.35f, V3_NULL, m_pTransformCom);
			g_pManagement->Create_Effect_Delay(L"Player_Skill_BloodTornadeMesh_2", 0.42f, V3_NULL, m_pTransformCom);
			g_pManagement->Create_Effect_Delay(L"Player_Skill_BloodTornadeMesh_3", 0.46f, V3_NULL, m_pTransformCom);
			g_pManagement->Create_Effect_Delay(L"Player_Skill_BloodTornadeMesh", 0.5f, V3_NULL, m_pTransformCom);
			g_pManagement->Create_Effect_Delay(L"Player_Skill_BloodTornadeMesh_2", 0.62f, V3_NULL, m_pTransformCom);
		}
	}
	//}

	return;
}

void CPlayer_Colleague::CollAtt_CenterDown()
{
	_double		AniTime = m_pDynamicMesh->Get_TrackInfo().Position;
	_v3 vEffPos = _v3(0.f, 1.5f, 0.f) + m_pTransformCom->Get_Axis(AXIS_Z) * 1.5f;


	if (true == m_tObjParam.bCanAttack)
	{
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = true;
	}
	else
	{
		if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f) && m_eColleague_Ani == CPlayer_Colleague::Ani_Jump_CenterAtt_Skil)
		{
			Function_Reset_State();
			m_eMovetype = CPlayer_Colleague::Coll_Idle;
			m_bTestRendom = true;
			m_bMyHiting = false;
			m_bCheck_Attcing = false;
			m_fCoolTimer_limit = 8.f;
			m_bChecking_MyHit = true;
			++m_iNormalAtt_Count;
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_Voice);
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_02);
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_01);
			m_bCheck_Skil_Voice = false;
			m_iCenter_Count = 0;
			return;
		}
		else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.7f)
		{
			if (m_bEventTrigger[6] == false)
			{
				m_bEventTrigger[6] = true;

				m_pSword->Set_Enable_Trail(false);
			}
		}

		else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.5f)
		{
			if (m_bEventTrigger[5] == false)
			{
				m_bEventTrigger[5] = true;

				m_pSword->Set_Target_CanAttack(false);
				m_pSword->Set_Enable_Record(false);
			}
		}

		else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.167f)
		{
			if (m_bEventTrigger[4] == false)
			{
				m_bEventTrigger[4] = true;

				m_pSword->Set_Target_CanAttack(true);
				m_pSword->Set_Enable_Record(true);
			}
		}

		else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.0f)
		{
			if (m_bEventTrigger[3] == false)
			{
				m_bEventTrigger[3] = true;

				m_pSword->Set_Enable_Trail(true);
			}
		}


		if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.6f && m_pDynamicMesh->Get_TrackInfo().Position < 3.367f)
		{
			if (m_bEventTrigger[2] == false)
			{
				m_bEventTrigger[2] = true;

				m_fAtt_MoveAccel_Cur = 0.f;
				m_fAtt_MoveSpeed_Cur = 0.2f;
				m_fAni_Multiply = 0.0f;
			}
			Colleague_SkilMovement(m_fAni_Multiply);
			Colleague_Movement(m_fAtt_MoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
		}

		else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.366f && m_pDynamicMesh->Get_TrackInfo().Position < 2.067f)
		{
			if (m_bEventTrigger[1] == false)
			{
				m_bEventTrigger[1] = true;

				m_fAtt_MoveAccel_Cur = 0.f;
				m_fAtt_MoveSpeed_Cur = 0.2f;
				m_fAni_Multiply = 0.0f;
			}
			Colleague_SkilMovement(m_fAni_Multiply);
			Colleague_Movement(m_fAtt_MoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
		}

		else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.f && m_pDynamicMesh->Get_TrackInfo().Position < 1.366f)
		{
			if (m_bEventTrigger[0] == false)
			{
				m_bEventTrigger[0] = true;

				m_fAtt_MoveAccel_Cur = 0.f;
				m_fAtt_MoveSpeed_Cur = 0.2f;
				m_fAni_Multiply = 0.0f;
			}
			Function_RotateBody();
			Colleague_SkilMovement(m_fAni_Multiply);
			Colleague_Movement(m_fAtt_MoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
		}

		_v3 vEffPos = _v3(0.f, 1.5f, 0.f) + m_pTransformCom->Get_Axis(AXIS_Z) * 1.5f;

		if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.333f)
		{
			if (m_bEventTrigger[9] == false)
			{
				m_bEventTrigger[9] = true;

				g_pManagement->Create_Effect_Delay(L"Player_Skill_ShadowAssault_ScratchBlur", 0.f, vEffPos, m_pTransformCom);
				g_pManagement->Create_Effect_Delay(L"Player_Skill_ShadowAssault_Scratch", 0.f, vEffPos, m_pTransformCom);
				g_pManagement->Create_Effect_Delay(L"Hit_Slash_0", 0.f, vEffPos, m_pTransformCom);
				g_pManagement->Create_Effect_Delay(L"Hit_Slash_1", 0.f, vEffPos, m_pTransformCom);
				g_pManagement->Create_Effect_Delay(L"Hit_Slash_2", 0.f, vEffPos, m_pTransformCom);
				g_pManagement->Create_Effect_Delay(L"Hit_Slash_3", 0.f, vEffPos, m_pTransformCom);
				g_pManagement->Create_Effect_Delay(L"Hit_Slash_Particle_0", 0.f, vEffPos, m_pTransformCom);
				g_pManagement->Create_ParticleEffect_Delay(L"Player_Skill_RedParticle_Explosion", 0.1f, 0.f, vEffPos, m_pTransformCom);

				g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_01);
				g_pSoundManager->Play_Sound(L"SE_WEAPON_IMPACT_001.ogg", CSoundManager::Jack_SFX_01, CSoundManager::Effect_Sound);
				SHAKE_CAM_lv2;
			}
		}

		else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.133f)
		{
			if (m_bEventTrigger[8] == false)
			{
				m_bEventTrigger[8] = true;

				g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.f, m_pTransformCom->Get_Pos() + _v3(0, 0.5f, 0.f), nullptr);

				g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_02);
				g_pSoundManager->Play_Sound(L"SE_BLACK_KNIGHT_SWING_005.ogg", CSoundManager::Jack_SFX_02, CSoundManager::Effect_Sound);
			}
		}

		else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.1f)
		{
			if (m_bEventTrigger[7] == false)
			{
				m_bEventTrigger[7] = true;

				_v3 vEffPos = m_pTransformCom->Get_Pos() + _v3(0.f, 1.3f, 0.f);
				CParticleMgr::Get_Instance()->Create_Skill_Start_Effect(V3_NULL, vEffPos, m_pTransformCom);
				g_pManagement->Create_ParticleEffect_Delay(L"Player_Skill_DarkSmokeAura", 0.5f, 0.f, _v3(0, 1.1f, 0.f), m_pTransformCom);
			}
		}

	}
	return;
}

void CPlayer_Colleague::CollAtt_SlowGun()
{
	_double		AniTime = m_pDynamicMesh->Get_TrackInfo().Position;
	_v3			vBirth, vLook;
	_float		fLenght = 1.f;

	if (m_eColl_Sub_AttMoment != CPlayer_Colleague::Att_SlowGun)
		return;

	if (true == m_tObjParam.bCanAttack)
	{
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = true;
	}
	else
	{
		if (m_pDynamicMesh->Is_Finish_Animation(0.88f) && m_eColl_Sub_AttMoment == CPlayer_Colleague::Att_SlowGun)
		{
			Function_Reset_State();
			m_eMovetype = CPlayer_Colleague::Coll_Idle;
			m_bMyHiting = false;
			m_bCheck_Attcing = false;
			m_bChecking_MyHit = true;

			m_fCoolTimer_limit = 5.f;

			m_bChecking_Gun = false;

			return;
		}
		else if ((0.967f <= AniTime && 1.167f >= AniTime) && m_eColleague_Ani == CPlayer_Colleague::Ani_PlayerHeal_or_Gun)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				_mat matBone = *m_matBone[Bone_LHand] * m_pTransformCom->Get_WorldMat();
				memcpy(&vBirth, &matBone._41, sizeof(_v3));
				memcpy(&vLook, &matBone._21, sizeof(_v3));
				vBirth += (vLook*fLenght);

				CObjectPool_Manager::Get_Instance()->Create_Object(L"GameObject_ColleagueBullet",
					&BULLET_INFO(vBirth, m_pTransformCom->Get_Axis(AXIS_Z), 2.f, 1.f));
				g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_01);
				g_pSoundManager->Play_Sound(L"Gun_Attack.ogg", CSoundManager::Jack_SFX_01, CSoundManager::Effect_Sound);
			}
		}
		else if ((0.5f <= AniTime) && m_eColleague_Ani == CPlayer_Colleague::Ani_PlayerHeal_or_Gun)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;

				_mat matBone = *m_matBone[Bone_LHand] * m_pTransformCom->Get_WorldMat();
				memcpy(&vBirth, &matBone._41, sizeof(_v3));

				//g_pManagement->Create_Effect_Delay(L"Colleague_Skill_HandLight_Red_0", 0.f, vBirth);
				g_pManagement->Create_Effect_Delay(L"Colleague_Skill_HandLight_Pink_0", 0.3f, vBirth);
				g_pManagement->Create_Effect_Delay(L"Colleague_Skill_HandSmoke_Black_0", 0.f, vBirth);
			}
		}

		if (0.f <= AniTime && 0.861f >= AniTime)
			m_bChecking_Gun = true;
	}
}

void CPlayer_Colleague::CollAtt_Normal()
{
	if (m_eMovetype == CPlayer_Colleague::Coll_Hit ||
		m_eMovetype == CPlayer_Colleague::Coll_Dead ||
		m_eMovetype == CPlayer_Colleague::Coll_Heal)
		return;

	if (nullptr == m_pObject_Mon)
	{
		m_bStart_Fighting = false;
		m_bNear_byMonster = false;
		return;
	}

	_float		fMonLenght = D3DXVec3Length(&(TARGET_TO_TRANS(m_pObject_Mon)->Get_Pos() - m_pTransformCom->Get_Pos()));

	if (false == m_bCheck_Attcing /*|| false == m_bMyHiting*/)
		Function_RotateBody();


	if (fMonLenght > 4.f)
	{
		m_eMovetype = CPlayer_Colleague::Coll_Attack;
		m_eColl_Sub_AttMoment = CPlayer_Colleague::Att_MonRun;
	}
	if (true == m_bNest_Att_CoolTimer && (7.3f >= fMonLenght && 4.f < fMonLenght))
	{
		// 노멀에서 총을 쏜다
		m_eMovetype = CPlayer_Colleague::Coll_Attack;
		m_eColl_Sub_AttMoment = CPlayer_Colleague::Att_SlowGun;
	}
	else if ((fMonLenght <= 4.f && fMonLenght > 3.f))
	{
		m_eMovetype = CPlayer_Colleague::Coll_Attack;
		m_eColl_Sub_AttMoment = CPlayer_Colleague::Att_MonWalk;
	}
	/*else if ((1.5f >= fMonLenght && 0.f <= fMonLenght) && )
	{
	m_eMovetype = CPlayer_Colleague::Coll_Attack;
	m_eColl_Sub_AttMoment = CPlayer_Colleague::Att_MonBackWalk;
	}*/
	else if ((fMonLenght <= 3.f/* && false == m_bCheck_Attcing*/))
	{
		if (m_iNormalAtt_Count > 4)
			m_iNormalAtt_Count = 0;

		if (2 <= m_iCenter_Count)
		{
			m_eMovetype = CPlayer_Colleague::Coll_Attack;
			m_eColl_Sub_AttMoment = CPlayer_Colleague::Att_CenterDown;
			if (false == m_bCheck_Skil_Voice)
			{
				m_bCheck_Skil_Voice = true;
				g_pSoundManager->Stop_Sound(CSoundManager::Jack_Voice);
				g_pSoundManager->Play_Sound(L"Skil_Three.ogg", CSoundManager::Jack_Voice, CSoundManager::Voice_Sound);
				g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_02);
				g_pSoundManager->Play_Sound(L"Jack_Sword_Swing.ogg", CSoundManager::Jack_SFX_02, CSoundManager::Voice_Sound);
			}
		}
		else if (m_iNormalAtt_Count == 4)
		{
			// 여기서 삼단베기
			m_eMovetype = CPlayer_Colleague::Coll_Attack;
			m_eColl_Sub_AttMoment = CPlayer_Colleague::Att_ThreeCombo;

			if (false == m_bCheck_Skil_Voice)
			{
				m_bCheck_Skil_Voice = true;
				g_pSoundManager->Stop_Sound(CSoundManager::Jack_Voice);
				g_pSoundManager->Play_Sound(L"Letheargy_CooA.ogg", CSoundManager::Jack_Voice, CSoundManager::Voice_Sound);
				g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_02);
				g_pSoundManager->Play_Sound(L"Jack_Sword_Swing.ogg", CSoundManager::Jack_SFX_02, CSoundManager::Voice_Sound);
			}
		}
		else if (m_iNormalAtt_Count == 3)
		{
			m_eMovetype = CPlayer_Colleague::Coll_Attack;
			m_eColl_Sub_AttMoment = CPlayer_Colleague::Att_Base4;
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_Voice);
			g_pSoundManager->Play_Sound(L"Letheargy_Hap2.ogg", CSoundManager::Jack_Voice, CSoundManager::Voice_Sound);
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_02);
			g_pSoundManager->Play_Sound(L"Jack_Sword_Swing.ogg", CSoundManager::Jack_SFX_02, CSoundManager::Voice_Sound);
		}
		else if (m_iNormalAtt_Count == 2)
		{
			m_eMovetype = CPlayer_Colleague::Coll_Attack;
			m_eColl_Sub_AttMoment = CPlayer_Colleague::Att_Base3;
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_Voice);
			g_pSoundManager->Play_Sound(L"Letheargy_Hmm.ogg", CSoundManager::Jack_Voice, CSoundManager::Voice_Sound);
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_02);
			g_pSoundManager->Play_Sound(L"Jack_Sword_Swing.ogg", CSoundManager::Jack_SFX_02, CSoundManager::Voice_Sound);
		}
		else if (m_iNormalAtt_Count == 1)
		{
			m_eMovetype = CPlayer_Colleague::Coll_Attack;
			m_eColl_Sub_AttMoment = CPlayer_Colleague::Att_Base2;
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_Voice);
			g_pSoundManager->Play_Sound(L"Letheargy_Hmm.ogg", CSoundManager::Jack_Voice, CSoundManager::Voice_Sound);
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_02);
			g_pSoundManager->Play_Sound(L"Jack_Sword_Swing.ogg", CSoundManager::Jack_SFX_02, CSoundManager::Voice_Sound);
		}
		else if (m_iNormalAtt_Count == 0)
		{
			m_eMovetype = CPlayer_Colleague::Coll_Attack;
			m_eColl_Sub_AttMoment = CPlayer_Colleague::Att_Base1;
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_Voice);
			g_pSoundManager->Play_Sound(L"Letheargy_Hmm.ogg", CSoundManager::Jack_Voice, CSoundManager::Voice_Sound);
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_02);
			g_pSoundManager->Play_Sound(L"Jack_Sword_Swing.ogg", CSoundManager::Jack_SFX_02, CSoundManager::Voice_Sound);
		}
	}

	if (0 == fMonLenght || 30.f < fMonLenght)
	{
		m_bNear_byMonster = false;
		m_bStart_Fighting = false;
	}

	return;
}

void CPlayer_Colleague::CollAtt_Base1()
{
	_double		AniTime = m_pDynamicMesh->Get_TrackInfo().Position;

	if (true == m_tObjParam.bCanAttack)
	{
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = true;
	}
	else
	{
		if (m_pDynamicMesh->Is_Finish_Animation(0.45f) && m_eColleague_Ani == CPlayer_Colleague::One_Att)
		{
			Function_Reset_State();
			//			m_eMovetype = CPlayer_Colleague::Coll_Idle;
			m_bChecking_MyHit = true;
			++m_iNormalAtt_Count;
			m_bMyHiting = false;
			m_bCheck_Attcing = false;
			m_fCoolTimer_limit = 0.045f;
			m_bBase_Att[0] = true;
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_Voice);
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_01);
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_02);
			return;
		}
		else if (1.167f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pSword->Set_Enable_Trail(false);
				m_pSword->Set_Target_CanAttack(false);
				m_pSword->Set_Enable_Record(false);
			}
		}
		else if (0.6f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pSword->Set_Enable_Record(true);
			}
		}
		else if (0.5f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_pSword->Set_Target_CanAttack(true);
				m_pSword->Set_Enable_Trail(true);
				g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_01);
				g_pSoundManager->Play_Sound(L"Jack_Hit_Attack00.ogg", CSoundManager::Jack_SFX_01, CSoundManager::Voice_Sound);
			}
		}

		if (0.6f <= AniTime && 0.8f >= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_fAtt_MoveSpeed_Cur = 4.f;
				m_fAtt_MoveAccel_Cur = 0.f;	// 엑셀 값은 항상 0 초기화
				m_fAni_Multiply = 0.45f;	// 감폭 수치. 값이 클수록 빨리 감소. 0일시 등속운동(원래는 감속) // 보통은 1 ~ 0.5사이
			}
			Function_RotateBody();
			Colleague_Movement(m_fAtt_MoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Colleague_SkilMovement(m_fAni_Multiply);
		}
		if (0.f <= AniTime && 1.833f >= AniTime)
		{
			if (true == m_bNest_Att_CoolTimer)
				m_bCheck_Attcing = true;
		}
		else
			m_bCheck_Attcing = false;
	}
	return;
}

void CPlayer_Colleague::CollAtt_Base2()
{
	_double		AniTime = m_pDynamicMesh->Get_TrackInfo().Position;

	if (true == m_tObjParam.bCanAttack)
	{
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = true;
	}
	else
	{
		if (m_pDynamicMesh->Is_Finish_Animation(0.45f) &&
			(m_eColl_Sub_AttMoment == CPlayer_Colleague::Att_Base2 && m_eColl_AttackMoment == CPlayer_Colleague::Att_Normal))
		{

			Function_Reset_State();
			m_eMovetype = CPlayer_Colleague::Coll_Idle;
			++m_iNormalAtt_Count;
			m_bMyHiting = false;
			m_bCheck_Attcing = false;
			m_fCoolTimer_limit = 0.045f;
			m_bBase_Att[0] = true;
			m_bChecking_MyHit = true;
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_Voice);
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_01);
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_02);
			return;
		}
		/*else if (0.833f <= AniTime)
		{
		if (false == m_bEventTrigger[3])
		{
		m_bEventTrigger[3] = true;
		m_pSword->Set_Enable_Trail(false);
		}
		}*/
		else if (0.933f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_pSword->Set_Target_CanAttack(false);
				m_pSword->Set_Enable_Record(false);
				m_pSword->Set_Enable_Trail(false);
			}
		}
		/*else if (0.267f <= AniTime)
		{
		if (false == m_bEventTrigger[5])
		{
		m_bEventTrigger[5] = true;
		m_pSword->Set_Enable_Trail(true);
		}
		}*/
		else if (0.367f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_pSword->Set_Target_CanAttack(true);
				m_pSword->Set_Enable_Record(true);
				m_pSword->Set_Enable_Trail(true);
				g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_01);
				g_pSoundManager->Play_Sound(L"Jack_Hit_Attack01.ogg", CSoundManager::Jack_SFX_01, CSoundManager::Voice_Sound);
			}
		}
		if (0.f <= AniTime && 0.833f >= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_fAtt_MoveSpeed_Cur = 5.1f;
				m_fAtt_MoveAccel_Cur = 0.f;	// 엑셀 값은 항상 0 초기화
				m_fAni_Multiply = 0.6f;	// 감폭 수치. 값이 클수록 빨리 감소. 0일시 등속운동(원래는 감속) // 보통은 1 ~ 0.5사이
			}
			Function_RotateBody();
			Colleague_Movement(m_fAtt_MoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Colleague_SkilMovement(m_fAni_Multiply);
		}
		if (0.f <= AniTime && 1.933f >= AniTime)
		{
			if (true == m_bNest_Att_CoolTimer)
				m_bCheck_Attcing = true;
		}
		else
			m_bCheck_Attcing = false;
	}
	return;
}

void CPlayer_Colleague::CollAtt_Base3()
{
	_double		AniTime = m_pDynamicMesh->Get_TrackInfo().Position;

	if (true == m_tObjParam.bCanAttack)
	{
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = true;
	}
	else
	{
		if (m_pDynamicMesh->Is_Finish_Animation(0.45f) && m_eMovetype == CPlayer_Colleague::Coll_Attack)
		{

			Function_Reset_State();
			m_eMovetype = CPlayer_Colleague::Coll_Idle;
			++m_iNormalAtt_Count;
			m_bMyHiting = false;
			m_bCheck_Attcing = false;
			m_fCoolTimer_limit = 0.04f;
			m_bBase_Att[0] = true;
			m_bChecking_MyHit = true;
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_Voice);
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_01);
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_02);
			return;
		}
		else if (0.9f <= AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
				m_pSword->Set_Enable_Trail(false);
			}
		}
		else if (0.867f <= AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_pSword->Set_Target_CanAttack(false);
				m_pSword->Set_Enable_Record(false);
			}
		}
		else if (0.6f <= AniTime)
		{
			if (false == m_bEventTrigger[10])
			{
				m_bEventTrigger[10] = true;
				m_pSword->Set_Target_CanAttack(true);
				m_pSword->Set_Enable_Record(true);
				g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_01);
				g_pSoundManager->Play_Sound(L"Jack_Hit_Attack02.ogg", CSoundManager::Jack_SFX_01, CSoundManager::Voice_Sound);
			}
		}
		else if (0.5f <= AniTime)
		{
			if (false == m_bEventTrigger[11])
			{
				m_bEventTrigger[11] = true;
				m_pSword->Set_Enable_Trail(true);
			}
		}
		if (0.167f <= AniTime && 0.833f >= AniTime)
		{
			if (false == m_bEventTrigger[12])
			{
				m_bEventTrigger[12] = true;
				m_fAtt_MoveSpeed_Cur = 4.f;
				m_fAtt_MoveAccel_Cur = 0.f;	// 엑셀 값은 항상 0 초기화
				m_fAni_Multiply = 0.6f;	// 감폭 수치. 값이 클수록 빨리 감소. 0일시 등속운동(원래는 감속) // 보통은 1 ~ 0.5사이
			}
			Function_RotateBody();
			Colleague_Movement(m_fAtt_MoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Colleague_SkilMovement(m_fAni_Multiply);
		}
		if (0.f <= AniTime && 2.367f >= AniTime)
		{
			if (true == m_bNest_Att_CoolTimer)
				m_bCheck_Attcing = true;
		}
		else
			m_bCheck_Attcing = false;
	}
	return;
}

void CPlayer_Colleague::CollAtt_Base4()
{
	_double		AniTime = m_pDynamicMesh->Get_TrackInfo().Position;

	if (true == m_tObjParam.bCanAttack)
	{
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = true;
	}
	else
	{
		if (m_pDynamicMesh->Is_Finish_Animation(0.45f) && m_eMovetype == CPlayer_Colleague::Coll_Attack)
		{
			Function_Reset_State();
			m_eMovetype = CPlayer_Colleague::Coll_Idle;
			++m_iNormalAtt_Count;
			m_bMyHiting = false;
			m_bCheck_Attcing = false;
			m_fCoolTimer_limit = 0.04f;
			m_bBase_Att[0] = true;
			m_bChecking_MyHit = true;
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_Voice);
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_01);
			g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_02);
			return;
		}
		else if (1.1f <= AniTime)
		{
			if (false == m_bEventTrigger[12])
			{
				m_bEventTrigger[12] = true;
				m_pSword->Set_Enable_Trail(false);
			}
		}
		else if (0.833f <= AniTime)
		{
			if (false == m_bEventTrigger[13])
			{
				m_bEventTrigger[13] = true;
				m_pSword->Set_Target_CanAttack(false);
				m_pSword->Set_Enable_Record(false);
			}
		}
		else if (0.6f <= AniTime)
		{
			if (false == m_bEventTrigger[14])
			{
				m_bEventTrigger[14] = true;
				m_pSword->Set_Target_CanAttack(true);
				m_pSword->Set_Enable_Record(true);
				g_pSoundManager->Stop_Sound(CSoundManager::Jack_SFX_01);
				g_pSoundManager->Play_Sound(L"Jack_Hit_Attack03.ogg", CSoundManager::Jack_SFX_01, CSoundManager::Voice_Sound);
			}
		}
		else if (0.533f <= AniTime)
		{
			if (false == m_bEventTrigger[15])
			{
				m_bEventTrigger[15] = true;
				m_pSword->Set_Enable_Trail(true);
			}
		}
		if (0.333f <= AniTime && 0.867f >= AniTime)
		{
			if (false == m_bEventTrigger[16])
			{
				m_bEventTrigger[16] = true;
				m_fAtt_MoveSpeed_Cur = 4.f;
				m_fAtt_MoveAccel_Cur = 0.f;	// 엑셀 값은 항상 0 초기화
				m_fAni_Multiply = 0.6f;	// 감폭 수치. 값이 클수록 빨리 감소. 0일시 등속운동(원래는 감속) // 보통은 1 ~ 0.5사이
			}
			Function_RotateBody();
			Colleague_Movement(m_fAtt_MoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Colleague_SkilMovement(m_fAni_Multiply);
		}
		if (0.f <= AniTime && 2.233f >= AniTime)
		{
			if (true == m_bNest_Att_CoolTimer)
				m_bCheck_Attcing = true;
		}
		else
			m_bCheck_Attcing = false;
	}
	return;
}

void CPlayer_Colleague::CollGuard_Idle()
{
}

void CPlayer_Colleague::CollGuard_Walk()
{
}

void CPlayer_Colleague::CollGuard_Hit()
{
}

void CPlayer_Colleague::CollHeal_ForMe()
{
	_double		AniTime = m_pDynamicMesh->Get_TrackInfo().Position;
	_float		fMinusHP = m_tObjParam.fHp_Max - m_tObjParam.fHp_Cur;

	if (0 < m_iMyHeal_Count && 4 > m_iMyHeal_Count)
	{
		m_bNot_Recoverable = false;

		if (m_pDynamicMesh->Is_Finish_Animation(0.9f) && m_eColleague_Ani == CPlayer_Colleague::Ani_Heal)
		{
			Function_Reset_State();
			m_eMovetype = CPlayer_Colleague::Coll_Idle;
			return;
		}
		else if (2.267f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				--m_iMyHeal_Count;

				m_bEventTrigger[0] = true;
				m_bCheck_HealMyHp = true;
				m_tObjParam.fHp_Cur += (fMinusHP * 30.f) / 100.f;
				if (m_tObjParam.fHp_Cur >= m_tObjParam.fHp_Max)
					m_tObjParam.fHp_Cur = m_tObjParam.fHp_Max;
				else if (m_tObjParam.fHp_Cur < 0.f)
					m_tObjParam.fHp_Cur = 0.f;

				_mat matBone = *m_matBone[Bone_RHand] * m_pTransformCom->Get_WorldMat();
				_v3 vEffPos;
				memcpy(&vEffPos, &matBone._41, sizeof(_v3));

				g_pManagement->Create_Effect_Delay(L"Player_Skill_Distortion_Circle", 0.2f, vEffPos, nullptr);
				g_pManagement->Create_Effect_Delay(L"Player_Heal_RedLight", 0.2f, vEffPos, nullptr);
				g_pManagement->Create_Effect_Delay(L"Player_Heal_Particle", 0.2f, vEffPos, nullptr);
				g_pManagement->Create_ParticleEffect_Delay(L"Player_Buff_HandSmoke", 0.2f, 0.2f, vEffPos, nullptr);
			}
		}
	}
	else if(0 >= m_iMyHeal_Count)
	{
		m_bNot_Recoverable = true;
		m_eMovetype = CPlayer_Colleague::Coll_Idle;
		return;
	}
	else
	{
		m_eMovetype = CPlayer_Colleague::Coll_Idle;
		return;
	}
		
}

void CPlayer_Colleague::CollHeal_ForPlayer()
{
	_double	AniTime = m_pDynamicMesh->Get_TrackInfo().Position;
	_v3		vBirth, vLook;
	_float	fLenght = 1.f;

	if (true == m_bNest_Skil_CoolTImer)
	{
		_float	PlusHP = (m_tObjParam.fHp_Cur / 10.f);

		if (m_pDynamicMesh->Is_Finish_Animation(0.9f) && m_eColl_HealMoment == CPlayer_Colleague::Player_Heal)
		{
			Function_Reset_State();
			m_eMovetype = CPlayer_Colleague::Coll_Idle;
			m_fCoolTimer_Skil_limit = 5.f;
			return;
		}
		else if (1.033f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_tObjParam.fHp_Cur -= PlusHP;

				_mat matBone = *m_matBone[Bone_LHand] * m_pTransformCom->Get_WorldMat();
				memcpy(&vBirth, &matBone._41, sizeof(_v3));
				memcpy(&vLook, &matBone._21, sizeof(_v3));
				vBirth += (vLook*fLenght);

				CObjectPool_Manager::Get_Instance()->Create_Object(L"GameObject_CollBullet_Heal",
					&BULLET_INFO(vBirth, m_pTransformCom->Get_Axis(AXIS_Z), 3.f, 1.f));
			}
		}
	}
}

void CPlayer_Colleague::Function_RotateBody()
{
	_float fTargetAngle = 0.f;

	if (true == m_bStart_Fighting && true == m_bNear_byMonster)
	{
		if (nullptr == m_pObject_Mon)
			return;
	}
	if (false == m_bStart_Fighting)
		m_bLook_Monster = false;

	// 	if (true == m_tObjParam.bIsAttack)
	// 		return;

	for (auto& Bossiter : *m_List_pMonTarget[1])
	{
		if (true == (*m_List_pMonTarget[1]).empty())
			break;

		if (Bossiter == m_pObject_Mon && true == Bossiter->Get_Dead())
		{
			m_pObject_Mon = nullptr;
			break;
		}

		if (m_List_pMonTarget[1]->empty())
			break;

		if (Bossiter == m_pObject_Mon)
		{
			if (true == m_bStart_Fighting && true == m_bNear_byMonster)		// 전투 시 시선을 몬스터로 고정
			{
				m_bLook_Monster = true;
				fTargetAngle = m_pTransformCom->Chase_Target_Angle(&TARGET_TO_TRANS(Bossiter)->Get_Pos());
			}
			else
			{
				m_bLook_Monster = false;
				fTargetAngle = m_pTransformCom->Chase_Target_Angle(&m_pTargetTransformCom->Get_Pos());	// 없으면 플레이어
			}
		}
	}

	for (auto& iter : *m_List_pMonTarget[0])
	{
		if (true == (*m_List_pMonTarget[0]).empty())
			continue;

		if (false == iter->Get_Enable())
			continue;

		if (iter == m_pObject_Mon && true == iter->Get_Dead())
		{
			m_pObject_Mon = nullptr;
			continue;
		}

		if (iter == m_pObject_Mon)
		{
			if (true == m_bStart_Fighting && true == m_bNear_byMonster)		// 전투 시 시선을 몬스터로 고정
			{
				m_bLook_Monster = true;
				fTargetAngle = m_pTransformCom->Chase_Target_Angle(&TARGET_TO_TRANS(m_pObject_Mon)->Get_Pos());
				break;
			}
			else
			{
				m_bLook_Monster = false;
				fTargetAngle = m_pTransformCom->Chase_Target_Angle(&m_pTargetTransformCom->Get_Pos());	// 없으면 플레이어
				break;
			}
		}
		/*else if (iter != m_pObject_Mon)
		{
		m_pObject_Mon = iter;
		m_bLook_Monster = true;
		break;
		}*/
	}

	if (false == m_bLook_Monster)
		fTargetAngle = m_pTransformCom->Chase_Target_Angle(&m_pTargetTransformCom->Get_Pos());	// 없으면 플레이어

	_float fYAngle = m_pTransformCom->Get_Angle().y;

	_v3 vTargetDir = m_pTransformCom->Get_Axis(AXIS_Z);
	V3_NORMAL_SELF(&vTargetDir);

	if (fTargetAngle > 0)
	{
		if (fYAngle < 0)
		{
			if (-D3DXToRadian(90.f) > fYAngle && -D3DXToRadian(180.f) < fYAngle)
			{
				fYAngle -= DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle <= -D3DXToRadian(180.f)) fYAngle = D3DXToRadian(180.f);
			}
			else fYAngle += DELTA_60 * D3DXToRadian(360.f);
		}
		else
		{
			if (fYAngle < fTargetAngle)
			{
				fYAngle += DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle >= fTargetAngle) fYAngle = fTargetAngle;
			}
			else
			{
				fYAngle -= DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle <= fTargetAngle) fYAngle = fTargetAngle;
			}
		}
	}
	else if (fTargetAngle < 0)
	{
		if (fYAngle > 0)
		{
			if (D3DXToRadian(90.f) < fYAngle && D3DXToRadian(180.f) > fYAngle)
			{
				fYAngle += DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle >= D3DXToRadian(180.f)) fYAngle = -D3DXToRadian(180.f);
			}
			else fYAngle -= DELTA_60 * D3DXToRadian(360.f);
		}
		else
		{
			if (fYAngle > fTargetAngle)
			{
				fYAngle -= DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle <= fTargetAngle) fYAngle = fTargetAngle;
			}
			else
			{
				fYAngle += DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle >= fTargetAngle) fYAngle = fTargetAngle;
			}
		}
	}

	m_pTransformCom->Set_Angle(AXIS_Y, fYAngle);

	return;
}

void CPlayer_Colleague::Function_Reset_State()
{
	m_fAni_Multiply = 1.f;

	m_tObjParam.bIsDodge = false;
	m_tObjParam.bCanAttack = true;
	m_tObjParam.bIsAttack = false;

	m_tObjParam.bCanHit = true;
	m_tObjParam.bIsHit = false;

	m_tObjParam.bCanDodge = true;
	m_tObjParam.bIsDodge = false;

	for (auto& vetor_iter : m_vecAttackCol)
		vetor_iter->Set_Enabled(false);


	m_pSword->Set_Target_CanAttack(false);
	m_pSword->Set_Enable_Record(false);

	LOOP(20)
	{
		m_bEventTrigger[i] = false;
	}

	m_bChecking_MyHit = false;
	m_bMyHiting = false;
	//m_bBase_Att[0] = false;
	//m_bBase_Att[1] = false;
	//m_bBase_Att[2] = false;
	//m_bBase_Att[3] = false;

	m_bChecking_SkilHit = false;
}

void CPlayer_Colleague::Enter_Collision()
{
	if (m_eMovetype != CPlayer_Colleague::Coll_Dead)
	{
		Check_Collision_PushOut();
		Check_Collision_Event(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL));
	}
}

void CPlayer_Colleague::Check_Collision_PushOut()
{
	list<CGameObject*> tmpList[3];

	tmpList[0] = g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL);
	tmpList[1] = g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE);
	tmpList[2] = g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE);

	for (auto& list_iter : tmpList)
	{
		if (list_iter.empty())
			continue;

		for (auto& Obj_iter : list_iter)
		{
			CCollider* pCollider = TARGET_TO_COL(Obj_iter);

			if (m_pCollider->Check_Sphere(pCollider, m_pTransformCom->Get_Axis(AXIS_Z), m_fAtt_MoveSpeed_Cur * DELTA_60))
			{
				CTransform* pTrans = TARGET_TO_TRANS(Obj_iter);
				CNavMesh*   pNav = TARGET_TO_NAV(Obj_iter);

				_v3 vDir = m_pTransformCom->Get_Pos() - pTrans->Get_Pos();
				V3_NORMAL_SELF(&vDir);

				vDir.y = 0;

				pTrans->Set_Pos(pNav->Move_OnNaviMesh(NULL, &pTrans->Get_Pos(), &vDir, m_pCollider->Get_Length().x));
			}
		}
	}

	return;
}

void CPlayer_Colleague::Check_Collision_Event(list<CGameObject*> plistGameObject)
{
	// 공격 불가능이면 체크 안함
	if (false == m_tObjParam.bCanAttack)
		return;

	_bool bFirst = true;
	//게임 오브젝트를 받아와서
	for (auto& iter : plistGameObject)
	{
		// 맞을 수 없다면 리턴
		if (false == iter->Get_Target_CanHit())
			continue;

		// 내가 가진 Vec 콜라이더와 비교한다.
		for (auto& vecIter : m_vecAttackCol)
		{
			bFirst = true;

			// 피직콜라이더랑 비교
			for (auto& vecCol : iter->Get_PhysicColVector())
			{
				// 물체 전체를 대표할 콜라이더.
				if (vecIter->Check_Sphere(vecCol))
				{
					// 첫번째는 경계구 콜라이더니까 다음 콜라이더와 충돌처리 한다.
					if (bFirst)
					{
						bFirst = false;
						continue;
					}

					if (false == iter->Get_Target_IsDodge())
					{
						iter->Set_Target_CanHit(false);

						if (iter->Get_Target_IsHit())
							iter->Set_HitAgain(true);

						iter->Hit_Target(m_tObjParam.fDamage);
					}

					m_tObjParam.bCanAttack = false;

					break;

				}

				else
				{
					if (bFirst)
					{
						break;
					}
				}
			}
		}
	}
}

void CPlayer_Colleague::Function_CoolTIme()
{
	if (true == m_bCanCoolDown)
	{
		m_fCoolTime_Cur += DELTA_60;

		if (m_fCoolTime_Cur >= m_fCoolTime_Max)
		{
			m_fCoolTime_Cur = 0.f;
			m_bCanCoolDown = false;
			m_bIsCoolDown = true;
			m_tObjParam.bCanAttack = true;
			m_bAvailable_Skil = true;
		}
		else
			m_bAvailable_Skil = false;
	}
}

_bool CPlayer_Colleague::Function_Checking_AttCoolTime(_float fTImer)
{
	if (true == m_bChecking_MyHit)
	{
		m_fCoolTImer_NomalAtt += DELTA_60;

		if (fTImer <= m_fCoolTImer_NomalAtt)
		{
			m_bNest_Att_CoolTimer = true;
			m_fCoolTImer_NomalAtt = 0.f;
		}

		else
			m_bNest_Att_CoolTimer = false;
	}

	return m_bNest_Att_CoolTimer;
}

_bool CPlayer_Colleague::Function_Checking_SkilCoolTime(_float fSkilTimer)
{
	m_fCoolTimer_Skil_limit += DELTA_60;

	if (fSkilTimer <= m_fCoolTimer_Skil_limit && fSkilTimer != 0.f)
	{
		m_bNest_Skil_CoolTImer = true;
		m_fCoolTimer_Skil_limit = 0.f;
	}
	else
		m_bNest_Skil_CoolTImer = false;

	return m_bNest_Skil_CoolTImer;
}

void CPlayer_Colleague::Function_FBRL()
{
	_float fAngle = D3DXToDegree(m_pTransformCom->Chase_Target_Angle(&m_pTargetTransformCom->Get_Pos()));

	if (Coll_Hit == m_eMovetype)
	{
		if (0.f <= fAngle && 90.f > fAngle)
			m_eFBLR = Coll_FBLR::Coll_Front;
		else if (-90.f <= fAngle && 0.f > fAngle)
			m_eFBLR = Coll_FBLR::Coll_Front;
		else if (90.f <= fAngle && 180.f > fAngle)
			m_eFBLR = Coll_FBLR::Coll_Back;
		else if (-180.f <= fAngle && -90.f > fAngle)
			m_eFBLR = Coll_FBLR::Coll_Back;
	}
}

void CPlayer_Colleague::Function_Find_Target()
{
	_float	fMinPos = 2000000.f;

	if (nullptr != m_pObject_Mon)
		m_pObject_Mon = nullptr;

	//if ((m_List_pMonTarget[0]->empty()))
	//{
	//	
	//}
	for (auto& Bossiter : *m_List_pMonTarget[1])
	{
		if (true == Bossiter->Get_Dead())
		{
			m_bStart_Fighting = false;
			m_bNear_byMonster = false;
			continue;
		}
		else if (false == Bossiter->Get_Enable())
		{
			m_bStart_Fighting = false;
			m_bNear_byMonster = false;
			continue;
		}
		else if (nullptr == Bossiter)
		{
			m_bStart_Fighting = false;
			m_bNear_byMonster = false;
			continue;
		}
		else if (Bossiter->Get_Target_Hp() <= 0.f)
		{
			m_bStart_Fighting = false;
			m_bNear_byMonster = false;
			continue;
		}

		_float	fBossLength = V3_LENGTH(&(TARGET_TO_TRANS(Bossiter)->Get_Pos() - m_pTransformCom->Get_Pos()));

		if (fMinPos <= fBossLength)
			continue;

		if (fBossLength > m_fEnemiesPerceive_Length)
			continue;

		fMinPos = fBossLength;
		m_pObject_Mon = Bossiter;

		if (nullptr != m_pObject_Mon)
			m_bStart_Fighting = true;

		m_eTarget = CPlayer_Colleague::Coll_Target_Boss;

		return;
	}
	IF_NOT_NULL_RETURN(m_pObject_Mon);


	for (auto& Moniter : *m_List_pMonTarget[0])
	{
		if (true == Moniter->Get_Dead())
		{
			m_bStart_Fighting = false;
			m_bNear_byMonster = false;
			continue;
		}
		else if (false == Moniter->Get_Enable())
		{
			m_bStart_Fighting = false;
			m_bNear_byMonster = false;
			continue;
		}
		else if (nullptr == Moniter)
		{
			m_bStart_Fighting = false;
			m_bNear_byMonster = false;
			continue;
		}
		else if (Moniter->Get_Target_Hp() <= 0.f)
		{
			m_bStart_Fighting = false;
			m_bNear_byMonster = false;
			continue;
		}

		_float	fMonLength = V3_LENGTH(&(TARGET_TO_TRANS(Moniter)->Get_Pos() - m_pTransformCom->Get_Pos()));

		if (fMinPos <= fMonLength)
			continue;

		if (fMonLength > m_fEnemiesPerceive_Length)
			continue;

		fMinPos = fMonLength;
		m_pObject_Mon = Moniter;

		if (nullptr != m_pObject_Mon)
			m_bStart_Fighting = true;

		m_eTarget = CPlayer_Colleague::Coll_Target_Monster;

		return;
	}
	IF_NOT_NULL_RETURN(m_pObject_Mon);

	m_pObject_Mon = nullptr;
	m_eTarget = CPlayer_Colleague::Coll_Target_Non;

	return;
}

HRESULT CPlayer_Colleague::Teleport_ResetOptions(void * pArg)
{
	_tchar szNavMeshName[STR_128] = L"";

	lstrcpy(szNavMeshName,
		(g_eSceneID_Cur == SCENE_STAGE_BASE ? L"Navmesh_Stage_00.dat" :
			g_eSceneID_Cur == SCENE_STAGE_01 ? L"Navmesh_Stage_01.dat" :
			g_eSceneID_Cur == SCENE_STAGE_02 ? L"Navmesh_Stage_02.dat" :
			g_eSceneID_Cur == SCENE_STAGE_03 ? L"Navmesh_Stage_03.dat" :
			g_eSceneID_Cur == SCENE_STAGE_04 ? L"Navmesh_Stage_04.dat" : L"Navmesh_Training.dat"));

	m_pNavMesh->Reset_NaviMesh();
	m_pNavMesh->Ready_NaviMesh(m_pGraphic_Dev, szNavMeshName);
	m_pNavMesh->Check_OnNavMesh(m_pTransformCom->Get_Pos());

	if (m_pNavMesh->Get_SubSetIndex() == -1)
		return E_FAIL;
	else
		return S_OK;
}

CPlayer_Colleague * CPlayer_Colleague::Create(_Device pGraphic_Device)
{
	CPlayer_Colleague* pInstance = new CPlayer_Colleague(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CPlayer_Colleague");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer_Colleague::Clone_GameObject(void * pArg)
{
	CPlayer_Colleague* pInstance = new CPlayer_Colleague(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CPlayer_Colleague");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Colleague::Free()
{
	Safe_Release(m_pSword);
	Safe_Release(m_pCollider);
	Safe_Release(m_pNavMesh);
	Safe_Release(m_pColleagueUI);
	Safe_Release(m_pCollJack);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pDynamicMesh);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTargetTransformCom);


	for (auto& iter : m_vecPhysicCol)
	{
		Safe_Release(iter);
	}

	//for (auto& Boneiter : m_matBone)
	//{
	//	Safe_Release(Boneiter);
	//}

	Safe_Release(m_pBattleAgentCom);

	CGameObject::Free();
}
