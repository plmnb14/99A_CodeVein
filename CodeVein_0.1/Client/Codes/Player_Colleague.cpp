#include "stdafx.h"
#include "..\Headers\Player_Colleague.h"
#include "Player.h"
#include "Weapon.h"
#include "Colleague_Bullet.h"

#include "ObjectPool_Manager.h"



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
	/*if (false == m_bEnable)
		return E_FAIL;*/

	CGameObject::Update_GameObject(TimeDelta);

	if (g_pInput_Device->Key_Down(DIK_P))
	{
		m_tObjParam.fHp_Cur += 1000.f;
	}

	if (100.f >= m_tObjParam.fHp_Cur)
	{
		m_eMovetype = CPlayer_Colleague::Coll_Heal;
		m_eColl_HealMoment = CPlayer_Colleague::My_Heal;
	}

	Check_MyHit();
	//CollHeal_ForPlayer();

	Check_Do_List();
	Set_AniEvent();

	Function_Checking_AttCoolTime(m_fCoolTimer_limit);
	Function_CoolTIme();

	m_pDynamicMesh->SetUp_Animation(m_eColleague_Ani);

	if (m_eMovetype != CPlayer_Colleague::Coll_Dead)
		Enter_Collision();

	cout << "현재 HP: " << m_tObjParam.fHp_Cur << endl;
	cout << "공격상태: " << m_eColl_Sub_AttMoment << endl;
	cout << "4 - Att_ThreeCombo, 5 - Att_CenterDown" << endl;

	/*
	cout << "애니상태: " << m_eColleague_Ani << " || " << "Run - 5, Walk - 3, Idle - 0" << endl;
	cout << "공격중인지: " << m_tObjParam.bCanAttack << " || 0 - false, 1 - true" << endl;
	cout << "몇타인지: " << m_iNormalAtt_Count << endl;
	*/

	// 지금은 몬스터 발견하자마자 바로 튀어가서 공격하는 형식이지만,
	// 몬스터 쪽으로 가서 떄리지만 가드나 구르기 같은 걸 추가해서 역동적으로 싸울 수 있게
	// 스킬을 먼저 다 하고 가드나 구르기같은 회피기를 추가하자
	// 1번, 2번 추가 완료
	// 3번 총쏘기
	// 총알 추가해야 함!
	// 추가하자 총알

	return S_OK;
}

_int CPlayer_Colleague::Late_Update_GameObject(_double TimeDelta)
{
	/*if (false == m_bEnable)
		return E_FAIL;*/

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

	Update_Collider();
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
	m_pCollider->Set_Radius(_v3(0.5f, 0.5f, 0.5f));
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

	// 왼손
	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo("LeftHand", 0), E_FAIL);
	m_matBone[Bone_LHand] = &pFrame->CombinedTransformationMatrix;

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
	m_pSword->Change_WeaponData(CWeapon::Wpn_SSword_Slave);

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

void CPlayer_Colleague::Check_MyHit()
{
	if (m_eMovetype == CPlayer_Colleague::Coll_Dead)
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
	else if (0 >= m_tObjParam.fHp_Cur && 0 >= m_iMyHeal_Count)
		m_eMovetype = CPlayer_Colleague::Coll_Dead;
	else if (0 >= m_tObjParam.fHp_Cur && 0 < m_iMyHeal_Count)
		m_eMovetype = CPlayer_Colleague::Coll_Heal;
}


void CPlayer_Colleague::Check_Do_List()
{
	if (m_eMovetype == CPlayer_Colleague::Coll_Hit || 
		m_eMovetype == CPlayer_Colleague::Coll_Dead ||
		m_eMovetype == CPlayer_Colleague::Coll_Heal)
		return;

	_float	fMinPos = 0.f;
	_float	fMonLength = 0.f;

	_float	fPlayerLength = V3_LENGTH(&(m_pTransformCom->Get_Pos() - m_pTargetTransformCom->Get_Pos()));

	if (fPlayerLength > 30.f)
	{
		m_pTransformCom->Set_Pos(_v3(m_pTargetTransformCom->Get_Pos().x - 3.f, m_pTargetTransformCom->Get_Pos().y, m_pTargetTransformCom->Get_Pos().z - 2.f));
	}


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
		fMinPos = 2000000.f;

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

		fMonLength = D3DXVec3Length(&(m_pTransformCom->Get_Pos() - TARGET_TO_TRANS(iter)->Get_Pos()));

		if (fMinPos > fMonLength)
		{
			if (true == iter->Get_Dead())
				if (m_pObject_Mon == iter)
				{
					m_pObject_Mon = nullptr;
					fMonLength = 0.f;
					m_bStart_Fighting = false;
					m_bNear_byMonster = false;
					m_bMonDead = true;
					continue;
				}

			/*if (false == iter->Get_Enable())
			continue;*/

			if (nullptr == iter)
				continue;

			fMinPos = fMonLength;

			m_pObject_Mon = iter;

			if (false == iter->Get_Dead())
			{
				if (m_pObject_Mon == iter)
				{
					m_bMonDead = false;
				}
			}


			if (nullptr == m_pObject_Mon)
				continue;
		}
		else if (!(fMinPos > fMonLength) && false == iter->Get_Dead())
		{
			if (true == iter->Get_Dead())
				if (m_pObject_Mon == iter)
				{
					m_pObject_Mon = nullptr;
					fMonLength = 0.f;
					m_bStart_Fighting = false;
					m_bNear_byMonster = false;
					m_bMonDead = true;
					continue;
				}

			/*	if (false == iter->Get_Enable())
			continue;*/

			if (nullptr == iter)
				continue;

			fMinPos = fMonLength;

			m_pObject_Mon = iter;

			if (nullptr != m_pObject_Mon)
				m_bMonDead = false;

			if (nullptr == m_pObject_Mon)
				continue;
		}
	}
	// Hit, Att, Dodge 이면 return 시킨다
	

	if (true == m_tObjParam.bIsHit || true == m_tObjParam.bIsDodge)
		return;

	_float	fMyPlayerLength = V3_LENGTH(&(m_pTransformCom->Get_Pos() - m_pTargetTransformCom->Get_Pos()));

	if (fMyPlayerLength < 30.f)		// 플레이어가 범위 내에 있는지 체크
	{
		if (fMinPos < 30.f && 0 != fMinPos)
			m_bStart_Fighting = true;
		if (fMinPos > 30.f || 0 == fMinPos)
			m_bStart_Fighting = false;

		if (true == m_bStart_Fighting)
		{
			if (fMinPos < 30.f)
			{
				m_bNear_byMonster = true;

				if (true == m_bAvailable_Skil)
				{
					m_eMovetype = CPlayer_Colleague::Coll_Attack;
					m_eColl_AttackMoment = CPlayer_Colleague::Att_Skil;
				}
				else
				{
					m_eMovetype = CPlayer_Colleague::Coll_Attack;
					m_eColl_AttackMoment = CPlayer_Colleague::Att_Normal;
				}
			}
			else
			{
				m_eMovetype = CPlayer_Colleague::Coll_Idle;
				m_eColl_IdleMoment = CPlayer_Colleague::Idle_Waiting;
				m_bStart_Fighting = false;
				m_bNear_byMonster = false;
			}
			if (fMinPos > 30.f || nullptr == m_pObject_Mon || true == m_bMonDead || 0 == fMinPos/* || true == m_bNot_AttcaingMon*/)
			{
				m_bStart_Fighting = false;
				m_bNear_byMonster = false;
			}
		}
		
		

		if (false == m_bStart_Fighting && false == m_bNear_byMonster)
		{
			if (fMyPlayerLength >= 5.f)
			{
				m_eMovetype = CPlayer_Colleague::Coll_Move;
				m_eColl_MoveMent = CPlayer_Colleague::Move_Run;
			}
			else if (fMyPlayerLength > 2.f && fMyPlayerLength < 5.f)
			{
				m_eMovetype = CPlayer_Colleague::Coll_Move;
				m_eColl_MoveMent = CPlayer_Colleague::Move_Walk;
			}
		}
		if (false == m_bStart_Fighting && false == m_bNear_byMonster && fMyPlayerLength < 2.f)
		{
			m_eMovetype = CPlayer_Colleague::Coll_Idle;
			m_eColl_IdleMoment = CPlayer_Colleague::Idle_Waiting;
		}

	}
	if (fMyPlayerLength > 30.f)
	{
		m_pTransformCom->Set_Pos(_v3(m_pTargetTransformCom->Get_Pos().x - 3.f, m_pTargetTransformCom->Get_Pos().y, m_pTargetTransformCom->Get_Pos().z - 2.f));
	}


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
		switch (m_eColl_MoveMent)
		{
		case Client::CPlayer_Colleague::Move_Walk:
		{
			// 플레이어와 일정 거리 이상으로 가까워졌을 때 걷는다
			CollMove_Walk();
			break;
		}
		case CPlayer_Colleague::Move_BackWalk:
		{
			CollMove_BackWalk();
			break;
		}
		case CPlayer_Colleague::Move_Run:
		{
			// 플레이어와 일정 거리 이상 떨어져 있을 때 뛰어서 쫒아간다
			CollMove_Run();
			break;
		}
		case CPlayer_Colleague::Move_BackRun:
		{
			CollMove_BackRun();
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
			break;
		}
		case CPlayer_Colleague::Att_Base2:
		{
			CollAtt_Base2();
			break;
		}
		case CPlayer_Colleague::Att_Base3:
		{
			CollAtt_Base3();
			break;
		}
		case CPlayer_Colleague::Att_Base4:
		{
			CollAtt_Base4();
			break;
		}
		case CPlayer_Colleague::Att_ThreeCombo:
		{
			CollAtt_ThreeCombo();
			break;
		}
		case CPlayer_Colleague::Att_CenterDown:
		{
			CollAtt_CenterDown();
			break;
		}
		case CPlayer_Colleague::Att_SlowGun:
		{
			CollAtt_SlowGun();
		}
		break;
		}
	}
	case CPlayer_Colleague::Coll_Dodge:
	{
		switch (m_eColl_DodgeMoment)
		{
		case Client::CPlayer_Colleague::Dodge_FrontRoll:
		{
			CollDodge_FrontRoll();
			break;
		}
		case Client::CPlayer_Colleague::Dodge_BackRoll:
		{

			break;
		}
		}
		break;
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
			CollHeal_ForMe();
			break;
		}
		case Client::CPlayer_Colleague::Player_Heal:
		{
			CollHeal_ForPlayer();
			break;
		}
		}
		break;
	}
	case CPlayer_Colleague::Coll_Dead:
	{
		Play_Dead();
		break;
	}

	}
}

HRESULT CPlayer_Colleague::SetUp_Default()
{
	m_pTarget = static_cast<CPlayer*>(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));
	IF_NULL_VALUE_RETURN(m_pTarget, E_FAIL);
	if (nullptr != m_pTarget)
	{
		Safe_AddRef(m_pTarget);

		m_pTargetTransformCom = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));
		Safe_AddRef(m_pTargetTransformCom);
	}

	m_eMovetype = CPlayer_Colleague::Coll_Idle;
	m_tObjParam.fHp_Cur = m_tObjParam.fHp_Max;

	m_pTransformCom->Set_Scale(_v3(1.2f, 1.1f, 1.2f));

	m_List_pMonTarget[0] = &(g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE));
	m_List_pMonTarget[1] = &(g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE));

	m_tObjParam.fHp_Cur = 5000.f;
	m_tObjParam.fHp_Max = 5000.f;
	m_tObjParam.fDamage = 10.f;

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

void CPlayer_Colleague::Play_Dead()
{
	// 죽었음
	// 겨우살이 갔다오지 않는 이상, 더이상 동료로 쓸 수 없다
	_double AniTime = m_pDynamicMesh->Get_TrackInfo().Position;

	if (false == m_bColleagueDead)
	{
		Funtion_Reset_State();
		m_bColleagueDead = true;
		m_eColleague_Ani = CPlayer_Colleague::Ani_Dead;
	}
	else
	{
		if (m_pDynamicMesh->Is_Finish_Animation(0.9f))
		{
			m_bEnable = true;
			m_dPlayAni_Time = 0;
		}
		//if (14.1f <= AniTime)
		//{
		//	if (false == m_bEventTrigger[0])
		//	{
		//		m_bEventTrigger[0] = true;

		//		Start_Dissolve(0.8f, false, true, 0.0f);
		//		m_fDeadEffect_Delay = 0.f;
		//	//	CObjectPool_Manager::Get_Instance()->Create_Object(L"GameObject_Haze", (void*)&CHaze::HAZE_INFO(100.f, m_pTransformCom->Get_Pos(), 0.f));
		//	}
		//}
	}
	
}

void CPlayer_Colleague::Colleague_Guard()
{
}

void CPlayer_Colleague::Play_Hit()
{
	if (m_eMovetype == CPlayer_Colleague::Coll_Dead)
		return;

	if (false == m_tObjParam.bIsHit)
	{
		Funtion_Reset_State();
		m_tObjParam.bIsHit = true;

		// 히트 관련 동작 재생
		switch (m_eFBLR)
		{
		case CPlayer_Colleague::Coll_Front:
		{
			m_eColleague_Ani = CPlayer_Colleague::Ani_Front_Hit;
			Function_FBRL();
			break;
		}
		case CPlayer_Colleague::Coll_Back:
		{
			m_eColleague_Ani = CPlayer_Colleague::Ani_Back_Hit;
			Function_FBRL();
			break;
		}
		}
	}
	else
	{
		if (m_pDynamicMesh->Is_Finish_Animation(0.86f))
		{
			m_tObjParam.bCanHit = true;
			m_tObjParam.bIsHit = false;

			m_bCanCoolDown = true;
			m_fCoolTime_Max = 0.5f;

			m_eMovetype = CPlayer_Colleague::Coll_Idle;
			m_eColleague_Ani = CPlayer_Colleague::Ani_Idle;
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

void CPlayer_Colleague::CollMove_Walk()
{
	Funtion_RotateBody();
	Colleague_Movement(2.f, m_pTransformCom->Get_Axis(AXIS_Z));
	// 지금은 앞으로만 걷게
	m_eColleague_Ani = CPlayer_Colleague::Ani_Front_Walk;
}

void CPlayer_Colleague::CollMove_BackWalk()
{
	// 뒤로 걷나?
	Funtion_RotateBody();
	Colleague_Movement(2.f, -(m_pTransformCom->Get_Axis(AXIS_Z)));
	m_eColleague_Ani = CPlayer_Colleague::Ani_Back_Walk;
}

void CPlayer_Colleague::CollMove_Run()
{
	// 뛸 때 속도조절하는건데 일시적으로 이렇게 두고 나중에 꼭 수정해야 함
	Funtion_RotateBody();
	Colleague_Movement(5.f, m_pTransformCom->Get_Axis(AXIS_Z));
	// 지금은 앞으로만 뛰게
	m_eColleague_Ani = CPlayer_Colleague::Ani_Front_Run;
}

void CPlayer_Colleague::CollMove_BackRun()
{
	Funtion_RotateBody();
	Colleague_Movement(5.f, -(m_pTransformCom->Get_Axis(AXIS_Z)));
	// 지금은 앞으로만 뛰게
	m_eColleague_Ani = CPlayer_Colleague::Ani_Back_Run;
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

void CPlayer_Colleague::CollDodge_FrontRoll()
{
	// 구르기나 피할 수 있는 요소를 담을 함수
}

void CPlayer_Colleague::CollIdle_Waiting()
{
	Funtion_RotateBody();
	Colleague_Movement(0.f, m_pTransformCom->Get_Axis(AXIS_Z));
	m_eColleague_Ani = CPlayer_Colleague::Ani_Idle;
}

void CPlayer_Colleague::CollAtt_Skil()
{
	// 각 스킬마다 쿨타임이 있어야 한다.
	// 스킬 사용 후에 쿨타임 시작하게
	// 거리 별로 스킬을 사용하는 것이 아니라 랜덤으로? 사용하게 해야 한다
	// 지금 나오는거 보면 쿨타임이 잘 안 되는 것 같기도?
	// 아주 멀다면 손에서 구 발사하는 건 그대로 해야 함
	_float		fMonLenght = 0.f;

	for (auto& iter : *m_List_pMonTarget[0])
	{
		if (iter == m_pObject_Mon && false == iter->Get_Dead())
		{
			CTransform* MonTransCom = TARGET_TO_TRANS(iter);
			fMonLenght = V3_LENGTH(&(m_pTransformCom->Get_Pos() - MonTransCom->Get_Pos()));
		}
		if (iter == m_pObject_Mon && true == iter->Get_Dead())
		{
			m_pObject_Mon = iter;
			continue;
		}
		if (false == iter->Get_Enable())
			continue;
		if (m_pObject_Mon != iter)
		{
			m_pObject_Mon = iter;
			fMonLenght = V3_LENGTH(&(m_pTransformCom->Get_Pos() - TARGET_TO_TRANS(iter)->Get_Pos()));
		}


		if (0 != fMonLenght)
			m_bNot_AttcaingMon = false;
		if (0 == fMonLenght)
		{
			m_bNot_AttcaingMon = true;
			if (m_pObject_Mon != iter)
			{
				m_pObject_Mon = iter;
				fMonLenght = V3_LENGTH(&(m_pTransformCom->Get_Pos() - TARGET_TO_TRANS(iter)->Get_Pos()));
			}
			else
				return;
		}
	}

	if (nullptr == m_pObject_Mon)
		return;

	_double		AniTime = m_pDynamicMesh->Get_TrackInfo().Position;

	if (false == m_bCheck_Attcing)
		Funtion_RotateBody();

	// 총 스킬을 그냥 노말로 빼야 할 것 같다
	// 손에서 총은 거리가 있을 때 바로바로 나와야 하고 
	// 음...

	if (true == m_bIsCoolDown)
	{
		if (true == m_bTestRendom)
		{
			// 여기서 중앙베기
			m_eMovetype = CPlayer_Colleague::Coll_Attack;
			m_eColl_AttackMoment = CPlayer_Colleague::Att_Skil;
			m_eColl_Sub_AttMoment = CPlayer_Colleague::Att_CenterDown;
			m_eColleague_Ani = CPlayer_Colleague::Ani_Jump_CenterAtt_Skil;
		}
		else if (false == m_bTestRendom)
		{
			// 여기서 삼단베기
			m_eMovetype = CPlayer_Colleague::Coll_Attack;
			m_eColl_AttackMoment = CPlayer_Colleague::Att_Skil;
			m_eColl_Sub_AttMoment = CPlayer_Colleague::Att_ThreeCombo;
			m_eColleague_Ani = CPlayer_Colleague::Ani_Trun_Center_Att_Skil;
		}
	}
	else
	{
		m_eMovetype = CPlayer_Colleague::Coll_Attack;
		m_eColl_AttackMoment = CPlayer_Colleague::Att_Normal;
	}
}

void CPlayer_Colleague::CollAtt_Normal()
{
	// 평타를 몇번째 평타를 칠 지, 얼마나 다가가야 하는지 구분해주는 함수

	_float		fMonLenght = 0.f;

	for (auto& iter : *m_List_pMonTarget[0])
	{
		if (iter == m_pObject_Mon && false == iter->Get_Dead())
		{
			CTransform* MonTransCom = TARGET_TO_TRANS(iter);
			fMonLenght = V3_LENGTH(&(m_pTransformCom->Get_Pos() - MonTransCom->Get_Pos()));
		}
		if (iter == m_pObject_Mon && true == iter->Get_Dead())
		{
			m_pObject_Mon = iter;
			continue;
		}
		if (false == iter->Get_Enable())
			continue;
		if (m_pObject_Mon != iter)
		{
			m_pObject_Mon = iter;
			fMonLenght = V3_LENGTH(&(m_pTransformCom->Get_Pos() - TARGET_TO_TRANS(iter)->Get_Pos()));
		}


		if (0 != fMonLenght)
			m_bNot_AttcaingMon = false;
		if (0 == fMonLenght)
		{
			m_bNot_AttcaingMon = true;
			if (m_pObject_Mon != iter)
			{
				m_pObject_Mon = iter;
				fMonLenght = V3_LENGTH(&(m_pTransformCom->Get_Pos() - TARGET_TO_TRANS(iter)->Get_Pos()));
			}
			else
				return;
		}
	}

	if (nullptr == m_pObject_Mon)
		return;

	_double		AniTime = m_pDynamicMesh->Get_TrackInfo().Position;

	if (false == m_bCheck_Attcing /*|| false == m_bMyHiting*/)
		Funtion_RotateBody();


	if (fMonLenght > 8.f)		// 4.f
	{
		if (false == m_bEventTrigger[4])
		{
			m_bEventTrigger[4] = true;
			m_fAtt_MoveSpeed_Cur = 4.f;
			m_fAtt_MoveAccel_Cur = 0.f;	// 엑셀 값은 항상 0 초기화
			m_fAni_Multiply = 0.f;	// 감폭 수치. 값이 클수록 빨리 감소. 0일시 등속운동(원래는 감속) // 보통은 1 ~ 0.5사이
		}
		Colleague_Movement(m_fAtt_MoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
		/*Colleague_SkilMovement(m_fAtt_MoveMultiply);*/
		m_eMovetype = CPlayer_Colleague::Coll_Move;
		m_eColl_MoveMent = CPlayer_Colleague::Move_MonRun;
		m_eColleague_Ani = CPlayer_Colleague::Ani_Front_Run;
	}
	else if (8.f >= fMonLenght && 5.f < fMonLenght)
	{
		// 노멀에서 총을 쏜다
		m_eMovetype = CPlayer_Colleague::Coll_Attack;
		m_eColl_AttackMoment = CPlayer_Colleague::Att_Normal;
		m_eColl_Sub_AttMoment = CPlayer_Colleague::Att_SlowGun;
		m_eColleague_Ani = CPlayer_Colleague::Ani_PlayerHeal_or_Gun;
	}
	else if ((fMonLenght <= 4.f && fMonLenght > 3.f) && false == m_bCheck_Attcing)	// <= 4.f / > 3.f
	{
		// 공격 중에 걷는 애니 시 트리거 사용
		// 순수하게 걷기만 한다면 트리거 사용 x
		if (false == m_bEventTrigger[5])
		{
			m_bEventTrigger[5] = true;
			m_fAtt_MoveSpeed_Cur = 2.f;
			m_fAtt_MoveAccel_Cur = 0.f;
			m_fAni_Multiply = 0.f;
		}
		Colleague_Movement(2.f, m_pTransformCom->Get_Axis(AXIS_Z));
		//Colleague_SkilMovement(m_fAni_Multiply);
		m_eMovetype = CPlayer_Colleague::Coll_Move;
		m_eColl_MoveMent = CPlayer_Colleague::Move_MonWalk;
		m_eColleague_Ani = CPlayer_Colleague::Ani_Front_Walk;
	}
	else if (fMonLenght <= 3.f && false == m_bCheck_Attcing)		// <= 3.f
	{
		// 여기서 스킬을 쓸지 공격을 할지 정한다
		// 쿨타임이 5.f이면 사용 가능, 아니면 불가능하다
		// 만약 스킬의 쿨타임이 다 차있다면 스킬을 우선적으로 사용하며, 없을 경우 일반 공격을 한다
		if (m_iNormalAtt_Count == 3 && true == m_bNest_Att_CoolTimer)
		{
			m_eMovetype = CPlayer_Colleague::Coll_Attack;
			m_eColl_Sub_AttMoment = CPlayer_Colleague::Att_Base4;
			m_eColleague_Ani = CPlayer_Colleague::Four_Att;
		}
		else if (m_iNormalAtt_Count == 2 && true == m_bNest_Att_CoolTimer)
		{
			m_eMovetype = CPlayer_Colleague::Coll_Attack;
			m_eColl_Sub_AttMoment = CPlayer_Colleague::Att_Base3;
			m_eColleague_Ani = CPlayer_Colleague::Three_Att;
		}
		else if (m_iNormalAtt_Count == 1 && true == m_bNest_Att_CoolTimer)
		{
			m_eMovetype = CPlayer_Colleague::Coll_Attack;
			m_eColl_Sub_AttMoment = CPlayer_Colleague::Att_Base2;
			m_eColleague_Ani = CPlayer_Colleague::Two_Att;
		}
		else if (m_iNormalAtt_Count == 0)
		{
			m_eMovetype = CPlayer_Colleague::Coll_Attack;
			m_eColl_Sub_AttMoment = CPlayer_Colleague::Att_Base1;
			m_eColleague_Ani = CPlayer_Colleague::One_Att;
		}
		else if (m_iNormalAtt_Count >= 4)
			m_iNormalAtt_Count = 0;

		else if (m_eMovetype == CPlayer_Colleague::Coll_Move)
			return;
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
		if (m_pDynamicMesh->Is_Finish_Animation(0.45f) && m_eMovetype == CPlayer_Colleague::Coll_Attack)
		{
			Funtion_Reset_State();
			m_eMovetype = CPlayer_Colleague::Coll_Idle;
			//m_eColleague_Ani = CPlayer_Colleague::Ani_Idle;
			m_bMyHiting = false;
			m_bCheck_Attcing = false;

			m_fCoolTimer_limit = 0.045f;
			m_bBase_Att[0] = true;
			m_bChecking_MyHit = true;

			++m_iNormalAtt_Count;
			return;
		}
			
		else if (0.8f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pSword->Set_Target_CanAttack(false);
				m_pSword->Set_Enable_Record(false);
			}
		}
		else if (0.6f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pSword->Set_Target_CanAttack(true);
				m_pSword->Set_Enable_Record(true);
			}
		}

		if (0.6f <= AniTime && 0.8f >= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_fAtt_MoveSpeed_Cur = 4.f;
				m_fAtt_MoveAccel_Cur = 0.f;	// 엑셀 값은 항상 0 초기화
				m_fAni_Multiply = 0.45f;	// 감폭 수치. 값이 클수록 빨리 감소. 0일시 등속운동(원래는 감속) // 보통은 1 ~ 0.5사이
			}
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
		if (m_pDynamicMesh->Is_Finish_Animation(0.45f) && m_eMovetype == CPlayer_Colleague::Coll_Attack)
		{
			
			Funtion_Reset_State();
			m_eMovetype = CPlayer_Colleague::Coll_Idle;
			//m_eColleague_Ani = CPlayer_Colleague::Ani_Idle;
			m_bMyHiting = false;
			m_bCheck_Attcing = false;

			m_fCoolTimer_limit = 0.045f;
			m_bBase_Att[0] = true;
			m_bChecking_MyHit = true;

			++m_iNormalAtt_Count;
			return;
		}
		
		else if (0.3f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_pSword->Set_Target_CanAttack(false);
				m_pSword->Set_Enable_Record(false);
			}
		}
		else if (0.1f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_pSword->Set_Target_CanAttack(true);
				m_pSword->Set_Enable_Record(true);
			}
		}
		if (0.f <= AniTime && 0.833f >= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_fAtt_MoveSpeed_Cur = 5.1f;
				m_fAtt_MoveAccel_Cur = 0.f;	// 엑셀 값은 항상 0 초기화
				m_fAni_Multiply = 0.6f;	// 감폭 수치. 값이 클수록 빨리 감소. 0일시 등속운동(원래는 감속) // 보통은 1 ~ 0.5사이
			}
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
			
			Funtion_Reset_State();
			//m_eMovetype = CPlayer_Colleague::Coll_Idle;
			//m_eColleague_Ani = CPlayer_Colleague::Ani_Idle;
			m_bMyHiting = false;
			m_bCheck_Attcing = false;

			m_fCoolTimer_limit = 0.045f;
			m_bBase_Att[0] = true;
			m_bChecking_MyHit = true;

			++m_iNormalAtt_Count;
			return;
		}
		else if (0.867f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_pSword->Set_Target_CanAttack(false);
				m_pSword->Set_Enable_Record(false);
			}
		}
		else if (0.6f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_pSword->Set_Target_CanAttack(true);
				m_pSword->Set_Enable_Record(true);
			}
		}
		if (0.167f <= AniTime && 0.833f >= AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
				m_fAtt_MoveSpeed_Cur = 4.f;
				m_fAtt_MoveAccel_Cur = 0.f;	// 엑셀 값은 항상 0 초기화
				m_fAni_Multiply = 0.6f;	// 감폭 수치. 값이 클수록 빨리 감소. 0일시 등속운동(원래는 감속) // 보통은 1 ~ 0.5사이
			}
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
			
			Funtion_Reset_State();
			//m_eMovetype = CPlayer_Colleague::Coll_Idle;
			//m_eColleague_Ani = CPlayer_Colleague::Ani_Idle;
			m_bMyHiting = false;
			m_bCheck_Attcing = false;

			m_fCoolTimer_limit = 0.045f;
			m_bBase_Att[0] = true;
			m_bChecking_MyHit = true;

			++m_iNormalAtt_Count;
			return;
		}
		else if (0.833f <= AniTime)
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
			}
		}
		if (0.333f <= AniTime && 0.867f >= AniTime)
		{
			if (false == m_bEventTrigger[11])
			{
				m_bEventTrigger[11] = true;
				m_fAtt_MoveSpeed_Cur = 4.f;
				m_fAtt_MoveAccel_Cur = 0.f;	// 엑셀 값은 항상 0 초기화
				m_fAni_Multiply = 0.6f;	// 감폭 수치. 값이 클수록 빨리 감소. 0일시 등속운동(원래는 감속) // 보통은 1 ~ 0.5사이
			}
			Colleague_Movement(m_fAtt_MoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Colleague_SkilMovement(m_fAni_Multiply);
		}
		/*for (auto& iter : *m_List_pMonTarget[0])
		{
		if (iter == m_pObject_Mon && true == iter->Get_Dead())
		{
		m_pObject_Mon = nullptr;
		continue;
		}
		else if (iter == m_pObject_Mon)
		{
		if (true == iter->Get_Target_IsHit())
		{
		++m_iNormalAtt_Count;
		m_bMyHiting = true;
		m_bBase_Att[3] = true;
		m_bChecking_MyHit = true;
		m_fCoolTimer_limit = 0.05f;
		Function_Checking_AttCoolTime(m_fCoolTimer_limit);
		}
		else
		{
		m_bChecking_MyHit = false;
		m_bMyHiting = false;
		m_bBase_Att[0] = false;
		m_bBase_Att[1] = false;
		m_bBase_Att[2] = false;
		m_bBase_Att[3] = false;
		}
		}
		}*/
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

void CPlayer_Colleague::CollAtt_ThreeCombo()		// 4번
{
	_double		AniTime = m_pDynamicMesh->Get_TrackInfo().Position;

	// 3단 베기
	if (true == m_tObjParam.bCanAttack)
	{
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = true;
	}
	else
	{
		if (m_pDynamicMesh->Is_Finish_Animation(0.88f) && m_eColl_AttackMoment == CPlayer_Colleague::Att_Skil)
		{
			Funtion_Reset_State();
			m_eMovetype = CPlayer_Colleague::Coll_Idle;
			m_eColleague_Ani = CPlayer_Colleague::Ani_Idle;
			m_bCanCoolDown = true;
			m_fCoolTime_Max = 15.f;
			m_bTestRendom = true;
			return;
		}
		else if (1.433f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pSword->Set_Target_CanAttack(false);
				m_pSword->Set_Enable_Record(false);
			}
		}
		else if (1.067f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pSword->Set_Target_CanAttack(true);
				m_pSword->Set_Enable_Record(true);
			}
		}
		if (1.067f <= AniTime && 1.433f >= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_fAtt_MoveSpeed_Cur = 4.f;
				m_fAtt_MoveAccel_Cur = 0.5f;	// 엑셀 값은 항상 0 초기화
				m_fAni_Multiply = 0.5f;	// 감폭 수치. 값이 클수록 빨리 감소. 0일시 등속운동(원래는 감속) // 보통은 1 ~ 0.5사이
			}
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
}

void CPlayer_Colleague::CollAtt_CenterDown()		// 2번
{
	_double		AniTime = m_pDynamicMesh->Get_TrackInfo().Position;

	// Center Down
	if (true == m_tObjParam.bCanAttack)
	{
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = true;
	}
	else
	{
		if (m_pDynamicMesh->Is_Finish_Animation(0.84f) && 
			m_eColl_AttackMoment == CPlayer_Colleague::Att_Skil)
		{
			Funtion_Reset_State();
			m_eMovetype = CPlayer_Colleague::Coll_Idle;
			m_eColleague_Ani = CPlayer_Colleague::Ani_Idle;
			m_bCanCoolDown = true;
			m_fCoolTime_Max = 15.f;
			m_bTestRendom = false;
			return;
		}
		else if (2.967f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pSword->Set_Target_CanAttack(false);
				m_pSword->Set_Enable_Record(false);
			}
		}
		else if (2.833f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pSword->Set_Target_CanAttack(true);
				m_pSword->Set_Enable_Record(true);
			}
		}
		else if (2.067f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pSword->Set_Target_CanAttack(false);
				m_pSword->Set_Enable_Record(false);
				m_fCoolTime_Max = 15.f;
			}
		}
		else if (1.833f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_pSword->Set_Target_CanAttack(true);
				m_pSword->Set_Enable_Record(true);
			}
		}
		if (3.033f <= AniTime && 3.333f >= AniTime)		// 2타 움직임
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_fAtt_MoveSpeed_Cur = 3.4f;
				m_fAtt_MoveAccel_Cur = 0.8f;	// 엑셀 값은 항상 0 초기화
				m_fAni_Multiply = 0.5f;	// 감폭 수치. 값이 클수록 빨리 감소. 0일시 등속운동(원래는 감속) // 보통은 1 ~ 0.5사이
			}
			Colleague_Movement(m_fAtt_MoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Colleague_SkilMovement(m_fAni_Multiply);
		}
		else if (0.1f <= AniTime && 1.567f >= AniTime)	// 1타 움직임
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_fAtt_MoveSpeed_Cur = 1.5f;
				m_fAtt_MoveAccel_Cur = 0.5f;	// 엑셀 값은 항상 0 초기화
				m_fAni_Multiply = 0.5f;	// 감폭 수치. 값이 클수록 빨리 감소. 0일시 등속운동(원래는 감속) // 보통은 1 ~ 0.5사이
			}
			Colleague_Movement(m_fAtt_MoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Colleague_SkilMovement(m_fAni_Multiply);
		}
		
		if (0.f <= AniTime && 3.033f >= AniTime)
		{
			
			if (true == m_bNest_Att_CoolTimer)
				m_bCheck_Attcing = true;
		}
		else
			m_bCheck_Attcing = false;
	}
}

void CPlayer_Colleague::CollAtt_SlowGun()
{
	_double		AniTime = m_pDynamicMesh->Get_TrackInfo().Position;
	_v3			vBirth, vLook;
	_float		fLenght = 1.f;

	// 천천히 가는 총 발사
	// 총을 쏘긴 하지만 생성이 안 되는 것 같고
	// 너무 여러번 쏘아서 일반 공격이 늦어진다.
	if (true == m_tObjParam.bCanAttack)
	{
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = true;
	}
	else
	{
		if (m_pDynamicMesh->Is_Finish_Animation(0.88f) && 
			m_eColl_AttackMoment == CPlayer_Colleague::Att_Normal)
		{
			Funtion_Reset_State();
			m_eMovetype = CPlayer_Colleague::Coll_Idle;
			m_eColleague_Ani = CPlayer_Colleague::Ani_Idle;
			m_fCoolTime_Max = 15.f;
			m_bCanCoolDown = true;
			return;
		}
		else if (false == m_bEventTrigger[0])
		{
			m_bEventTrigger[0] = true;
		}
		else if (1.033f <= AniTime)
		{
			// 총알 만들어야 함
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				_mat matBone = *m_matBone[Bone_LHand] * m_pTransformCom->Get_WorldMat();
				memcpy(&vBirth, &matBone._41, sizeof(_v3));
				memcpy(&vLook, &matBone._21, sizeof(_v3));
				vBirth += (vLook*fLenght);

				/*g_pManagement->Add_GameObject_ToLayer(L"GameObject_ColleagueBullet", 
					SCENE_STAGE, L"Layer_CollBullet", &BULLET_INFO(vBirth, m_pTransformCom->Get_Axis(AXIS_Z), 8.f, 1.5));*/
				CObjectPool_Manager::Get_Instance()->Create_Object(L"GameObject_ColleagueBullet", 
					&BULLET_INFO(vBirth, m_pTransformCom->Get_Axis(AXIS_Z), 3.f, 1.f));
				cout << "총총" << endl;
			}
		}
	}
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
	if (m_iMyHeal_Count <= 4)
	{
		if (false == m_bEventTrigger[0])
		{
			m_bEventTrigger[0] = true;
			m_eMovetype = CPlayer_Colleague::Coll_Heal;
			m_eColl_HealMoment = CPlayer_Colleague::My_Heal;
			m_eColleague_Ani = CPlayer_Colleague::Ani_Heal;
			m_bCheck_HealMyHp = true;
			m_tObjParam.fHp_Cur += 1000.f;
			--m_iMyHeal_Count;
		}
		else if (m_pDynamicMesh->Is_Finish_Animation(0.9f))
		{
			Funtion_Reset_State();
			m_eMovetype = CPlayer_Colleague::Coll_Idle;
			m_eColleague_Ani = CPlayer_Colleague::Ani_Idle;
		}
	}
}

void CPlayer_Colleague::CollHeal_ForPlayer()
{
	if (0 < m_tObjParam.fHp_Cur)
	{
		_float PlusHP = (m_tObjParam.fHp_Cur / 10.f);
		if (false == m_bEventTrigger[0])
		{
			m_bEventTrigger[0] = true;
			m_eMovetype = CPlayer_Colleague::Coll_Hit;
			m_eColl_HealMoment = CPlayer_Colleague::Player_Heal;
			m_eColleague_Ani = CPlayer_Colleague::Ani_PlayerHeal_or_Gun;
			m_tObjParam.fHp_Cur -= PlusHP;
			m_pTarget->Add_Target_Hp(PlusHP);
		}
		else if (m_pDynamicMesh->Is_Finish_Animation(0.9f))
		{

		}
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
	if (false == m_bStart_Fighting)
		m_bLook_Monster = false;

	for (auto& iter : *m_List_pMonTarget[0])
	{
		if (iter == m_pObject_Mon && true == iter->Get_Dead())
		{
			m_pObject_Mon = nullptr;
			continue;
		}

		if (m_List_pMonTarget[0]->empty())
			continue;

		if (iter == m_pObject_Mon)
		{
			if (true == m_bStart_Fighting && true == m_bNear_byMonster)		// 전투 시 시선을 몬스터로 고정
			{
				m_bLook_Monster = true;
				fTargetAngle = m_pTransformCom->Chase_Target_Angle(&TARGET_TO_TRANS(m_pObject_Mon)->Get_Pos());
			}
			else
			{
				m_bLook_Monster = false;
				fTargetAngle = m_pTransformCom->Chase_Target_Angle(&m_pTargetTransformCom->Get_Pos());	// 없으면 플레이어
			}
		}
	}

	/*if (nullptr == m_pObject_Mon)
	return;*/

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

void CPlayer_Colleague::Funtion_Reset_State()
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
	m_bBase_Att[0] = false;
	m_bBase_Att[1] = false;
	m_bBase_Att[2] = false;
	m_bBase_Att[3] = false;
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
	list<CGameObject*> TempList[3];
	TempList[0] = g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE);
	TempList[1] = g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL);
	TempList[2] = g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE);


	for (auto& list_iter : TempList)
	{
		for (auto& Obj_iter : list_iter)
		{
			CCollider* pCollider = TARGET_TO_COL(Obj_iter);

			if (m_pCollider->Check_Sphere(pCollider, m_pTransformCom->Get_Axis(AXIS_Z), m_fAni_Multiply * DELTA_60))
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
}

void CPlayer_Colleague::Check_Collision_Event(list<CGameObject*> plistGameObject)
{
	if (false == m_tObjParam.bIsAttack)
		return;

	_bool bFirst = true;

	for (auto& iter : plistGameObject)
	{
		if (false == iter->Get_Target_CanHit())
			continue;

		for (auto& vecIter : m_vecAttackCol)
		{
			if (false == vecIter->Get_Enabled())
				continue;

			bFirst = true;

			for (auto& vecCol : iter->Get_PhysicColVector())
			{
				if (vecIter->Check_Sphere(vecCol))
				{
					if (bFirst)
					{
						bFirst = false;
						continue;
					}

					if (false == iter->Get_Target_IsDodge())
					{
						iter->Set_Target_CanHit(false);
						iter->Add_Target_Hp(m_tObjParam.fDamage);

						if (iter->Get_Target_IsHit())
						{
							iter->Set_HitAgain(true);
						}
					}

					vecIter->Set_Enabled(false);

					g_pManagement->Create_Hit_Effect(vecIter, vecCol, TARGET_TO_TRANS(iter));

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

	return;
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
			m_bNest_Att_CoolTimer = true;
		else
			m_bNest_Att_CoolTimer = false;
	}

	return m_bNest_Att_CoolTimer;
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

CPlayer_Colleague* CPlayer_Colleague::Create(_Device pGraphic_Device)
{
	CPlayer_Colleague*	pInstance = new CPlayer_Colleague(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CPlayer_Colleague");
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
	Safe_Release(m_pNavMesh);

	// 동료 Transfrom
	Safe_Release(m_pTransformCom);

	Safe_Release(m_pDynamicMesh);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTargetTransformCom);

	for (auto& iter : m_vecPhysicCol)
	{
		Safe_Release(iter);
	}

	for (auto& iter : m_matBone)
	{
		iter = nullptr;
	}

	CGameObject::Free();
}