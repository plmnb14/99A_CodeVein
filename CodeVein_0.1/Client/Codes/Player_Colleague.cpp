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

	//Ready_BoneMatrix();
	//Ready_Collider();

	return S_OK;
}

_int CPlayer_Colleague::Update_GameObject(_double TimeDelta)
{
	CGameObject::Late_Update_GameObject(TimeDelta);
	CGameObject::Update_GameObject(TimeDelta);

	Check_Do_List();
	Set_AniEvent();

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	//cout << "동료 위치: " << m_pTransformCom->Get_Pos().x << ", " << m_pTransformCom->Get_Pos().y << ", " << m_pTransformCom->Get_Pos().z << endl;
	//cout << "MoveType: " << m_eMovetype << ": Att는 2" << endl;

	return S_OK;
}

_int CPlayer_Colleague::Late_Update_GameObject(_double TimeDelta)
{

	return S_OK;
}

HRESULT CPlayer_Colleague::LateInit_GameObject()
{
	return S_OK;
}

HRESULT CPlayer_Colleague::Render_GameObject()
{
	if (nullptr == m_pShaderCom || nullptr == m_pStaticMesh)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();

	_ulong dwNumSubset = m_pStaticMesh->Get_NumMaterials();

	for (_ulong i = 0; i < dwNumSubset; ++i)
	{
		m_pShaderCom->Begin_Pass(0);
		m_pShaderCom->Set_StaticTexture_Auto(m_pStaticMesh, i);

		m_pShaderCom->Commit_Changes();

		m_pStaticMesh->Render_Mesh(i);

		m_pShaderCom->End_Pass();
	}

	m_pShaderCom->End_Shader();

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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_DefaultBox", L"Com_StaticMesh", (CComponent**)&m_pStaticMesh)))
		return E_FAIL;

	//// for.Com_Mesh
	//if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_Jack", L"Com_DynamicMesh", (CComponent**)&m_pDynamicMesh)))
	//	return E_FAIL;

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
	/*LPCSTR TempChar = "Head";

	D3DXFRAME_DERIVED* pFrame = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo(TempChar, 0);

	m_matBone[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	TempChar = "Spine";

	pFrame = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo(TempChar, 0);

	m_matBone[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBone[Bone_Body] = &pFrame->CombinedTransformationMatrix;*/

	return S_OK;
}

HRESULT CPlayer_Colleague::Ready_Collider()
{
	//m_vecPhysicCol;		// 충돌 체크용 콜라이더 벡터
	//m_vecAttackCol;		// 공격용 콜라이더 벡터
	m_vecPhysicCol.reserve(3);
	m_vecAttackCol.reserve(1);

	// 경계 체크용 Collider - 범위 내에 있는지? -> 있으면 바로 공격하고 멀리있으면 간 좀 보다가 가서 때린다던지 하는겅
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));
	IF_NULL_VALUE_RETURN(pCollider, E_FAIL);

	_float fRadius = 1.f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Range]->_41, m_matBone[Bone_Range]->_42, m_matBone[Bone_Range]->_43));

	return S_OK;
}

HRESULT CPlayer_Colleague::Ready_Weapon()
{
	return S_OK;
}

void CPlayer_Colleague::Update_Collider()
{
}

void CPlayer_Colleague::Render_Collider()
{
}

void CPlayer_Colleague::Check_Do_List()
{
	// 플레이어 쪽으로 몸 돌릴 때, 4.f 이상 떨어져있으면 플레이어가 보는 쪽으로 돌린다.

	// 우선 순위
	// - 플레이어가 30.f 안에 있는지
	// - 전투상황이 아니라면 가장 가까운 몬스터 한 마리를 찾는다
	// - 범위 내에 몬스터가 있는지
	// - 없으면 플레이어가 있는지

	_v3 pPlayerPos = m_pTargetTransformCom->Get_Pos();
	_float MyLength = V3_LENGTH(&(m_pTransformCom->Get_Pos() - m_pTargetTransformCom->Get_Pos()));


	if (MyLength < 30.f)	// - 플레이어가 30.f 안에 있는지 먼저 체크
	{
		//	// 몬스터가 근처에 있을 때 몬스터 쪽으로 이동하여 공격한다.
		//	// 루이스의 경우 플레이어보다 먼저 튀어나간다
		//	// 지금은 기본 공격만
		//	// 후에 거리 계산해서 스킬 사용 여부도 정해야 함
		if (false == m_bStart_Fighting)
		{
			// 전투 상황이 아니라면 가장 가까운 몬스터를 찾는다

			m_List_pMonTarget[0] = g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE);
			m_List_pMonTarget[1] = g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE);
			
			if (m_List_pMonTarget[0].empty())
				m_bMonExistence = true;
			else
				m_bMonExistence = false;

			if (false == m_bMonExistence)
			{
				_v3 vecMonpos = V3_NULL;

				for (auto& iter : m_List_pMonTarget[0])
					vecMonpos = TARGET_TO_TRANS(iter)->Get_Pos();

				m_fAll_Length = V3_LENGTH(&(m_pTransformCom->Get_Pos() - vecMonpos));

				if (m_fAll_Length < 15.f)
				{
					m_bStart_Fighting = true;
					
				}
			}
		}
		if (true == m_bStart_Fighting)
		{
			_float MonHP;
			for (auto& iter : m_List_pMonTarget[0])
				MonHP = iter->Get_Target_Hp();

			// 여기서 전투 시작
			if (m_fAll_Length < 15.f)
			{
				m_eMovetype = CPlayer_Colleague::Coll_Attack;
				m_eColl_AttackMoment = CPlayer_Colleague::Att_Idle;
			}
				

			if (0 >= MonHP)
				m_bStart_Fighting = false;
		}

		// - 범위 내 몬스터가 없으면 플레이어가 있는지
		if (MyLength > 3.f && false == m_bStart_Fighting)
		{
			// no
			// 플레이어와 얼마나 거리가 떨어져있는가?
			// 7.f 이하면 걷고, 이상이면 뛰어라
			if (MyLength > 3.f && MyLength < 7.f)
			{
				m_eMovetype = CPlayer_Colleague::Coll_Move;
				m_eColl_Movement = CPlayer_Colleague::Move_Walk;
			}
			if (MyLength > 7.f)
			{
				m_eMovetype = CPlayer_Colleague::Coll_Move;
				m_eColl_Movement = CPlayer_Colleague::Move_Run;
			}
		}
		if (MyLength < 3.f && false == m_bNear_byMonster)
		{
			// 이 조건일 때 주위에 몬스터가 있는지 체크, 없을 시 Idle_Waiting
			// 플레이어 HP도 여기서 고려 해볼 법도 한가?
			// yes
			m_eMovetype = CPlayer_Colleague::Coll_Idle;
			m_eColl_IdleMoment = CPlayer_Colleague::Idle_Waiting;
		}
	}
	if (MyLength > 30.f)
	{
		// 플레이어가 범위 내를 벗어났으니까 플레이어 근처로 강제 소환해야 함
		// 위치를 플레이어에서 한 3.f 정도 떨어진 곳으로 설정해주자
		m_pTargetTransformCom->Set_Pos(_v3(m_pTargetTransformCom->Get_Pos().x - 3.f, m_pTargetTransformCom->Get_Pos().y, m_pTargetTransformCom->Get_Pos().z - 2.f));
	}

	// 기본 조건 먼저
	// Hit, Att, Dead 이면 return 시킨다
	if (m_eMovetype == CPlayer_Colleague::Coll_Hit || m_eMovetype == CPlayer_Colleague::Coll_Attack
		|| m_eMovetype == CPlayer_Colleague::Coll_Dead)
		return;

	if (true == m_tObjParam.bIsHit || true == m_tObjParam.bIsAttack || true == m_tObjParam.bIsDodge)
		return;
}

void CPlayer_Colleague::Set_AniEvent()
{
	// enum의 멤버에 따라 동작을 설정해주는 함수

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
			case CPlayer_Colleague::Move_Dodge:
			{
				// 이건 회피
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
				break;
			}
			case CPlayer_Colleague::Att_Idle:
			{
				CollAtt_Idle();
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
	m_pTransformCom->Set_Scale(V3_ONE);
	m_pTargetTransformCom = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));

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

	return S_OK;
}

void CPlayer_Colleague::Colleague_Movement(_float fSpeed, _v3 vDir)
{
	V3_NORMAL(&vDir, &vDir);

	m_pTransformCom->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransformCom->Get_Pos(), &vDir, fSpeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));
	m_pTransformCom->Set_Pos_Axis(m_pNavMesh->Axis_Y_OnNavMesh(m_pTargetTransformCom->Get_Pos()), AXIS_Y);
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
	m_eColl_Movement = CPlayer_Colleague::Move_Walk;
	Funtion_RotateBody();
	Colleague_Movement(2.f, m_pTransformCom->Get_Axis(AXIS_Z));
}

void CPlayer_Colleague::CollMove_Run()
{
	// 뛸 때 속도조절하는건데 일시적으로 이렇게 두고 나중에 꼭 수정해야 함
	m_eColl_Movement = CPlayer_Colleague::Move_Run;
	Funtion_RotateBody();
	Colleague_Movement(4.f, m_pTransformCom->Get_Axis(AXIS_Z));
}

void CPlayer_Colleague::CollMove_Dodge()
{
	// 구르기나 피할 수 있는 요소를 담을 함수
}

void CPlayer_Colleague::CollIdle_Waiting()
{
	m_eColl_IdleMoment = CPlayer_Colleague::Idle_Waiting;
	Funtion_RotateBody();
	Colleague_Movement(0.f, m_pTransformCom->Get_Axis(AXIS_Z));
}

void CPlayer_Colleague::CollAtt_Idle()
{
	// 기본 공격만 하는 함수
	// 루이 기본 공격
	// 기본 4타
	// 몬스터가 공격 범위 밖이면 공격을 잠깐 중지한다.
	// 보스도 생각해야 함
	CTransform* pMonTrans = nullptr;
	CGameObject* pMonster = nullptr;

	for (auto& Moniter : m_List_pMonTarget[0])
		pMonster = Moniter;

	for (auto& iter : m_List_pMonTarget[0])
		pMonTrans = TARGET_TO_TRANS(iter);

	if (nullptr == pMonTrans)
		return;


	Funtion_RotateBody();

	_float fMonLenght = V3_LENGTH(&(m_pTransformCom->Get_Pos() - pMonTrans->Get_Pos()));

	// 떨어져있으면 근접공격을 위해 가까이 간다
	if (fMonLenght > 2.3f)
	{
		// 근접공격을 위해 쫒아간다
		Colleague_Movement(4.f, pMonTrans->Get_Axis(AXIS_Z));
	}
		
	if (fMonLenght < 2.3f)
	{
		// 범위 내에 들어왔으니 간보다가 슉퍽
		if (true == pMonster->Get_Target_Param().bIsAttack)
		{
			// 몬스터가 공격 시 가드를 올림
			m_eMovetype = CPlayer_Colleague::Coll_Guard;
		}
	}
}

void CPlayer_Colleague::Funtion_RotateBody()
{
	_float fTargetAngle = m_pTransformCom->Chase_Target_Angle(&m_pTargetTransformCom->Get_Pos());

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
	Safe_Release(m_pSword);
	Safe_Release(m_pCollider);

	// 동료 Transfrom
	Safe_Release(m_pTransformCom);

	// 플레이어 Ransform
	//Safe_Release(m_pTargetTransformCom);
	Safe_Release(m_pStaticMesh);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pNavMesh);

	CGameObject::Free();
}
