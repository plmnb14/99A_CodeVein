#include "stdafx.h"
#include "..\Headers\Player_Colleague.h"
#include "Player.h"
#include "Weapon.h"


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
	if (FAILED(Add_Component()))
		return E_FAIL;

	SetUp_Default();

	Ready_BoneMatrix();
	Ready_Collider();
	Ready_Weapon();

	return S_OK;
}

_int CPlayer_Colleague::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	Check_Do_List();
	Set_AniEvent();

	m_pDynamicMesh->SetUp_Animation(m_eColleague_Ani);

	Update_Collider();

	return S_OK;
}

_int CPlayer_Colleague::Late_Update_GameObject(_double TimeDelta)
{
	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	m_dTimeDelta = TimeDelta;

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
	if (nullptr == m_pShaderCom || nullptr == m_pDynamicMesh)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pDynamicMesh->Play_Animation(m_dPlayAni_Time * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"));

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

			m_pShaderCom->Begin_Pass(m_iPass);

			m_pShaderCom->Set_DynamicTexture_Auto(m_pDynamicMesh, i, j);

			m_pShaderCom->Commit_Changes();

			m_pDynamicMesh->Render_Mesh(i, j);

			m_pShaderCom->End_Pass();
		}
	}
	m_pShaderCom->End_Shader();

	IF_NOT_NULL(m_pSword)
		m_pSword->Update_GameObject(m_dTimeDelta);


	Render_Collider();

	return S_OK;
}

HRESULT CPlayer_Colleague::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_Test_Jack", L"Com_DynamicMesh", (CComponent**)&m_pDynamicMesh)))
		return E_FAIL;

	// for.Com_NavMesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"NavMesh", L"Com_NavMesh", (CComponent**)&m_pNavMesh)))
		return E_FAIL;

	// for.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	// 콜라이더 설정
	m_pCollider->Set_Radius(_v3(0.35f, 0.35f, 0.35f));
	m_pCollider->Set_Type(COL_SPHERE);
	m_pCollider->Set_Dynamic(true);		// 움직이는지
	m_pCollider->Set_CenterPos(m_pTransformCom->Get_Pos());

	return S_OK;
}


HRESULT CPlayer_Colleague::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
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
		return E_FAIL;

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

	return S_OK;
}

HRESULT CPlayer_Colleague::Ready_Collider()
{
	m_vecPhysicCol.reserve(3);

	// 경계 체크용 Collider - 밀어내기 구
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));
	IF_NULL_VALUE_RETURN(pCollider, E_FAIL);

	_float fRadius = 1.f;
	 
	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Range]->_41, m_matBone[Bone_Range]->_42, m_matBone[Bone_Range]->_43));
	pCollider->Set_Enabled(true);
	pCollider->Set_Type(COL_SPHERE);
	m_vecPhysicCol.push_back(pCollider);

	//=====================================

	// 맞는 용도 구
	// 몸통
	fRadius = 0.4f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Body]->_41, m_matBone[Bone_Body]->_42, m_matBone[Bone_Body]->_43));
	pCollider->Set_Enabled(true);
	pCollider->Set_Type(COL_SPHERE);
	m_vecPhysicCol.push_back(pCollider);

	//=====================================
	// 머리
	fRadius = 0.4f;

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
	m_pSword->Change_WeaponData(CWeapon::WPN_SSword_Normal);

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

void CPlayer_Colleague::Check_Do_List()
{
	//1. 몬스터 타겟을 찾을 시 리스트 중에서 거리가 가장 가까운 것으로 한다.

	// 우선 순위
	// - 플레이어가 30.f 안에 있는지
	// - 전투상황이 아니라면 가장 가까운 몬스터 한 마리를 찾는다
	// - 범위 내에 몬스터가 있는지
	// - 없으면 플레이어가 있는지
	_float	fMinPos = 0.f;
	_float	fLength = 0.f;

	m_List_pMonTarget[0] = &(g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE));
	m_List_pMonTarget[1] = &(g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE));


	for (auto& iter_Mon : *m_List_pMonTarget[0])
	{
		if (false == iter_Mon->Get_Enable())
			continue;

		if (m_List_pMonTarget[0]->empty())
			continue;
	}
	for (auto& iter_Boss : *m_List_pMonTarget[1])
	{
		if (false == iter_Boss->Get_Enable())
			continue;

		if (m_List_pMonTarget[1]->empty())
			continue;
	}

	if (!(m_List_pMonTarget[0]->empty()))
		fMinPos = D3DXVec3Length(&(m_pTransformCom->Get_Pos() - TARGET_TO_TRANS((*m_List_pMonTarget)->front())->Get_Pos()));

	for (auto& iter : *m_List_pMonTarget[0])
	{
		if (false == iter->Get_Enable())
		{
			m_bStart_Fighting = false;
			m_bNear_byMonster = false;
			continue;
		}

		if (true == iter->Get_Dead())
		{
			if (m_pObject_Mon == iter)
			{
				m_pObject_Mon = nullptr;
				continue;
			}
		}

	
		fLength = D3DXVec3Length(&(m_pTransformCom->Get_Pos() - TARGET_TO_TRANS(iter)->Get_Pos()));

		if (fMinPos > fLength)
		{
			if (true == iter->Get_Dead())
				if (m_pObject_Mon == iter)
				{
					m_pObject_Mon = nullptr;
					fLength = 0.f;
					m_bStart_Fighting = false;
					m_bNear_byMonster = false;
					m_bMonDead = true;
					continue;
				}

			if (false == iter->Get_Enable())
				continue;

			if (nullptr == iter)
				continue;

			fMinPos = fLength;

			m_pObject_Mon = iter;

			if (nullptr != m_pObject_Mon)
				m_bMonDead = false;

			if (nullptr == m_pObject_Mon)
				continue;
		}
	}

	cout << "몬스터 거리: " << fLength << endl;

	_float	MyLength = V3_LENGTH(&(m_pTransformCom->Get_Pos() - m_pTargetTransformCom->Get_Pos()));

	if (MyLength < 30.f)		// 플레이어가 범위 내에 있는지 체크
	{

		//fLength < 30.f? ;
		if (fLength < 30.f && 0 != fLength)
			m_bStart_Fighting = true;
		if (fLength > 30.f)
			m_bStart_Fighting = false;

		if (true == m_bStart_Fighting)
		{
			if (fLength < 30.f)
			{
				m_bNear_byMonster = true;

				/*if (fLength < 2.3f)
				{
				m_eMovetype = CPlayer_Colleague::Coll_Attack;
				m_eColl_AttackMoment = CPlayer_Colleague::Att_Normal;
				}
				if (fLength > 2.3f)
				{
				m_eMovetype = CPlayer_Colleague::Coll_Move;
				m_eColl_Movement = CPlayer_Colleague::Move_MonRun;
				}*/
				m_eMovetype = CPlayer_Colleague::Coll_Attack;
				m_eColl_AttackMoment = CPlayer_Colleague::Att_Normal;

			}
			else
			{
				m_eMovetype = CPlayer_Colleague::Coll_Idle;
				m_eColl_IdleMoment = CPlayer_Colleague::Idle_Waiting;
				m_bStart_Fighting = false;
				m_bNear_byMonster = false;
			}
			if (fLength > 30.f || nullptr == m_pObject_Mon || true == m_bMonDead || 0 == fLength)
			{

				m_bStart_Fighting = false;
				m_bNear_byMonster = false;
			}
		}

		if (false == m_bStart_Fighting && false == m_bNear_byMonster)
		{
			if (MyLength > 2.f && MyLength < 5.f)
			{
				m_eMovetype = CPlayer_Colleague::Coll_Move;
				m_eColl_Movement = CPlayer_Colleague::Move_Walk;
			}
			if (MyLength >= 5.f)
			{
				m_eMovetype = CPlayer_Colleague::Coll_Move;
				m_eColl_Movement = CPlayer_Colleague::Move_Run;
			}
		}
		if (false == m_bStart_Fighting && false == m_bNear_byMonster && MyLength < 2.f)
		{
			m_eMovetype = CPlayer_Colleague::Coll_Idle;
			m_eColl_IdleMoment = CPlayer_Colleague::Idle_Waiting;
		}

	}

	if (MyLength > 30.f)
	{
		m_pTransformCom->Set_Pos(_v3(m_pTargetTransformCom->Get_Pos().x - 3.f, m_pTargetTransformCom->Get_Pos().y, m_pTargetTransformCom->Get_Pos().z - 2.f));
	}

	// Hit, Att, Dead 이면 return 시킨다
	if (m_eMovetype == CPlayer_Colleague::Coll_Hit || m_eMovetype == CPlayer_Colleague::Coll_Attack
		|| m_eMovetype == CPlayer_Colleague::Coll_Dead)
		return;

	if (true == m_tObjParam.bIsHit || true == m_tObjParam.bIsAttack || true == m_tObjParam.bIsDodge)
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
		switch (m_eColl_Movement)
		{
		case Client::CPlayer_Colleague::Move_Walk:
		{
			// 플레이어와 일정 거리 이상으로 가까워졌을 때 걷는다
			CollMove_Walk();
			break;
		}
		case CPlayer_Colleague::Move_Run:
		{
			// 플레이어와 일정 거리 이상 떨어져 있을 때 뛰어서 쫒아간다
			CollMove_Run();
			break;
		}
		case CPlayer_Colleague::Move_MonWalk:
		{
			CollMove_MonWalk();
			break;
		}
		case CPlayer_Colleague::Move_MonRun:
		{
			CollMove_MonRun();
			break;
		}
		case CPlayer_Colleague::Move_Dodge:
		{
			// 이건 회피
			CollMove_Dodge();
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
			break;
		}
		case Client::CPlayer_Colleague::Gurad_Walk:
		{
			break;
		}
		case Client::CPlayer_Colleague::Gurad_Hit:
		{
			break;
		}
		}
		break;
	}
	case CPlayer_Colleague::Coll_Attack:
	{
		switch (m_eColl_AttackMoment)
		{
		case CPlayer_Colleague::Att_waiting:
		{
			// 전투 대기 상태 - 간보는 상태?
			CollAtt_Waiting();
			break;
		}
		case CPlayer_Colleague::Att_Normal:
		{
			// 일반 공격
			CollAtt_Normal();
			break;
		}
		case CPlayer_Colleague::Att_Skill:
		{

			break;
		}
		}
		break;
	}
	case CPlayer_Colleague::Coll_Hit:
	{
		Colleague_Hit();
		break;
	}
	case CPlayer_Colleague::Coll_Dead:
	{
		Colleague_Dead();
		break;
	}
	}
}

HRESULT CPlayer_Colleague::SetUp_Default()
{
	m_pTarget = static_cast<CPlayer*>(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));
	IF_NULL_VALUE_RETURN(m_pTarget, E_FAIL);

	// 플레이어에서 10.f 떨어진 위치에서 최초 생성
	//m_pTransformCom->Set_Pos(_v3(TARGET_TO_TRANS(m_pTarget)->Get_Pos().x - 1.f, TARGET_TO_TRANS(m_pTarget)->Get_Pos().y, TARGET_TO_TRANS(m_pTarget)->Get_Pos().z - 1.f));
	m_pTransformCom->Set_Scale(_v3(1.2f, 1.1f, 1.2f));
	m_pTargetTransformCom = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));

	m_List_pMonTarget[0] = &(g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE));
	m_List_pMonTarget[1] = &(g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE));

	m_tObjParam.fHp_Cur = 1000.f;
	m_tObjParam.fHp_Max = 1000.f;
	m_tObjParam.fDamage = 200.f;

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

void CPlayer_Colleague::Colleague_Movement(_float fSpeed, _v3 vDir/*, _v3 YDir*/)
{
	V3_NORMAL(&vDir, &vDir);
	m_pTransformCom->Set_Pos(m_pNavMesh->Axis_Y_OnNavMesh(m_pTransformCom->Get_Pos()));
	m_pTransformCom->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransformCom->Get_Pos(), &vDir, fSpeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));
}

void CPlayer_Colleague::Colleague_Dead()
{
	// 데드 애니메이션 모션

}

void CPlayer_Colleague::Colleague_Guard()
{
}

void CPlayer_Colleague::Colleague_Hit()
{
	// 맞았다!
}

void CPlayer_Colleague::CollMove_Walk()
{
	Funtion_RotateBody();
	Colleague_Movement(2.f, m_pTransformCom->Get_Axis(AXIS_Z));
	// 지금은 앞으로만 걷게
	m_eColleague_Ani = CPlayer_Colleague::Ani_Front_Walk;
}

void CPlayer_Colleague::CollMove_Run()
{
	// 뛸 때 속도조절하는건데 일시적으로 이렇게 두고 나중에 꼭 수정해야 함
	Funtion_RotateBody();
	Colleague_Movement(5.f, m_pTransformCom->Get_Axis(AXIS_Z));
	// 지금은 앞으로만 뛰게
	m_eColleague_Ani = CPlayer_Colleague::Ani_Front_Run;
}

void CPlayer_Colleague::CollMove_MonWalk()
{
	if (nullptr == m_pObject_Mon)
		return;

	Funtion_RotateBody();

	_v3 MonDir = TARGET_TO_TRANS(m_pObject_Mon)->Get_Pos() - m_pTransformCom->Get_Pos();

	D3DXVec3Normalize(&MonDir, &MonDir);

	Colleague_Movement(4.f, MonDir);
	m_eColleague_Ani = CPlayer_Colleague::Ani_Front_Run;
}

void CPlayer_Colleague::CollMove_MonRun()
{
	if (nullptr == m_pObject_Mon)
		return;

	Funtion_RotateBody();

	_v3 MonDir = m_pTransformCom->Get_Pos() - TARGET_TO_TRANS(m_pObject_Mon)->Get_Pos();

	D3DXVec3Normalize(&MonDir, &MonDir);

	Colleague_Movement(4.f, MonDir);
	m_eColleague_Ani = CPlayer_Colleague::Ani_Front_Run;
}

void CPlayer_Colleague::CollMove_Dodge()
{
	// 구르기나 피할 수 있는 요소를 담을 함수
}

void CPlayer_Colleague::CollIdle_Waiting()
{
	Funtion_RotateBody();
	Colleague_Movement(0.f, m_pTransformCom->Get_Axis(AXIS_Z));
	m_eColleague_Ani = CPlayer_Colleague::Ani_Idle;
}

void CPlayer_Colleague::CollAtt_Waiting()
{
}

void CPlayer_Colleague::CollAtt_Normal()
{
	_float		fMonLenght = 0.f;
	/*_v3			vMonDir = V3_NULL;
	_v3			vMonDirrun = V3_NULL;*/

	_double		AniTime = m_pDynamicMesh->Get_TrackInfo().Position;

	for (auto& iter : *m_List_pMonTarget[0])
	{
		if (iter == m_pObject_Mon && true == iter->Get_Dead())
		{
			m_pObject_Mon = nullptr;
			continue;
		}

		if (iter == m_pObject_Mon)
		{
			CTransform* MonTransCom = TARGET_TO_TRANS(iter);
			fMonLenght = V3_LENGTH(&(MonTransCom->Get_Pos() - m_pTransformCom->Get_Pos()));
			/*vMonDir = MonTransCom->Get_Pos() - m_pTransformCom->Get_Pos();
			vMonDirrun = MonTransCom->Get_Pos() - m_pTransformCom->Get_Pos();*/
		}
	}

	if (nullptr == m_pObject_Mon)
		return;

	Funtion_RotateBody();

	//if (true == m_tObjParam.bCanAttack)
	//{
	//	m_tObjParam.bCanAttack = false;
	//	m_tObjParam.bIsAttack = true;
	//}

	//if (m_pDynamicMesh->Is_Finish_Animation(0.95f))
	//{
	//	// 애니메이션이 끝나면 초기화해주는 부분
	//}

	if (false == m_bEventTrigger[0])
	{

	}

	if (fMonLenght > 8.f)
	{
		Colleague_Movement(4.f, m_pTransformCom->Get_Axis(AXIS_Z));
		m_eColleague_Ani = CPlayer_Colleague::Ani_Front_Run;
	}
	else if (fMonLenght <= 8.f && fMonLenght > 2.3f)
	{
		//Colleague_Movement(2.f, TARGET_TO_TRANS(m_pObject_Mon)->Get_Axis(AXIS_Z));

		//D3DXVec3Normalize(&vMonDirrun, &vMonDirrun);

		Colleague_Movement(2.f, m_pTransformCom->Get_Axis(AXIS_Z));
		m_eColleague_Ani = CPlayer_Colleague::Ani_Front_Walk;
	}
	if (fMonLenght <= 2.3f)
	{
		m_eColleague_Ani = CPlayer_Colleague::One_Att;

		if (m_eColleague_Ani == CPlayer_Colleague::One_Att &&  AniTime >= 0.6f)
			m_pSword->Set_Target_CanAttack(true);
		else
			m_pSword->Set_Target_CanAttack(false);

		if (0.6f <= AniTime && 0.8f >= AniTime)
		{
			m_pSword->Set_Target_CanAttack(true);
			Colleague_Movement(8.f, m_pTransformCom->Get_Axis(AXIS_Z));
		}
		else   
			m_pSword->Set_Target_CanAttack(false);
	}

	if (m_tObjParam.bIsAttack)
	{
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = true;
	}
}

void CPlayer_Colleague::Funtion_RotateBody()
{
	_float fTargetAngle = 0.f;

	if (true == m_bStart_Fighting && true == m_bNear_byMonster)
	{
		if (nullptr == m_pObject_Mon)
			return;
	}

	for (auto& iter : *m_List_pMonTarget[0])
	{
		if (iter == m_pObject_Mon && true == iter->Get_Dead())
		{
			m_pObject_Mon = nullptr;
			continue;
		}

		if (iter == m_pObject_Mon)
		{
			if (true == m_bStart_Fighting && true == m_bNear_byMonster)		// 전투 시 시선을 몬스터로 고정
				fTargetAngle = m_pTransformCom->Chase_Target_Angle(&TARGET_TO_TRANS(m_pObject_Mon)->Get_Pos());
			else
				fTargetAngle = m_pTransformCom->Chase_Target_Angle(&m_pTargetTransformCom->Get_Pos());	// 없으면 플레이어
		}
	}

	if (nullptr == m_pObject_Mon)
		return;


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

CPlayer_Colleague* CPlayer_Colleague::Create(_Device pGraphic_Device)
{
	CPlayer_Colleague*	pInstance = new CPlayer_Colleague(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Colleague::Clone_GameObject(void* pArg)
{
	CPlayer_Colleague*	pInstance = new CPlayer_Colleague(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CPlayer_Colleague");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Colleague::Free()
{
	/*for (auto& Mon_iter : *m_List_pMonTarget[0])
	Safe_Release(Mon_iter);

	for (auto& Boss_iter : *m_List_pMonTarget[1])
	Safe_Release(Boss_iter);*/

	/*for (auto& iter : m_List_pMonTarget)
	Safe_Delete_Array(iter);*/

	Safe_Release(m_pSword);
	Safe_Release(m_pCollider);

	// 동료 Transfrom
	Safe_Release(m_pTransformCom);


	Safe_Release(m_pDynamicMesh);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pNavMesh);

	CGameObject::Free();
}
