#include "stdafx.h"
#include "..\Headers\Player.h"
#include "Weapon.h"
#include "Drain_Weapon.h"
#include "CameraMgr.h"
#include "Dummy_Target.h"

#include "ScriptManager.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
{

}

HRESULT CPlayer::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	SetUp_Default();

	Ready_BoneMatrix();
	Ready_Collider();

	Ready_Weapon();
	Ready_DrainWeapon();

	Ready_Skills();

	return NOERROR;
}

_int CPlayer::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	KeyInput();

	Parameter_HitCheck();
	Parameter_YPos();
	Parameter_Movement();
	Parameter_State();
	Parameter_HeavyCharging();
	Parameter_Collision();
	Parameter_Aiming();

	if (FAILED(m_pRenderer->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;
	//if (FAILED(m_pRenderer->Add_RenderList(RENDER_SHADOWTARGET, this)))
	//	return E_FAIL;

	IF_NOT_NULL(m_pWeapon[m_eActiveSlot])
		m_pWeapon[m_eActiveSlot]->Update_GameObject(TimeDelta);

	IF_NOT_NULL(m_pDrainWeapon)
		m_pDrainWeapon->Update_GameObject(TimeDelta);

	m_pNavMesh->Goto_Next_Subset(m_pTransform->Get_Pos(), nullptr);

	CScriptManager::Get_Instance()->Update_ScriptMgr(TimeDelta, m_pNavMesh->Get_SubSetIndex(), m_pNavMesh->Get_CellIndex());

	return NO_EVENT;
}

_int CPlayer::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRenderer ||
		nullptr == m_pDynamicMesh)
		return E_FAIL;

	Reset_BloodSuck_Options();
	Reset_Attack_Bool();

	m_pDynamicMesh->SetUp_Animation_Lower(m_eAnim_Lower);
	m_pDynamicMesh->SetUp_Animation_Upper(m_eAnim_Upper);
	m_pDynamicMesh->SetUp_Animation_RightArm(m_eAnim_RightArm);
	m_pDynamicMesh->SetUp_Animation_LeftArm(m_eAnim_RightArm);

	IF_NOT_NULL(m_pWeapon[m_eActiveSlot])
		m_pWeapon[m_eActiveSlot]->Late_Update_GameObject(TimeDelta);

	IF_NOT_NULL(m_pDrainWeapon)
		m_pDrainWeapon->Late_Update_GameObject(TimeDelta);

	return NO_EVENT;
}

HRESULT CPlayer::Render_GameObject()
{
	if (nullptr == m_pShader ||
		nullptr == m_pDynamicMesh)
		return E_FAIL;

	m_pDynamicMesh->Play_Animation_Lower(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMutiply);
	m_pDynamicMesh->Play_Animation_Upper(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMutiply);
	m_pDynamicMesh->Play_Animation_RightArm(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMutiply, m_bOneHand);
	m_pDynamicMesh->Play_Animation_LeftArm(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMutiply);

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShader->Begin_Shader();

	_uint iNumMeshContainer = _uint(m_pDynamicMesh->Get_NumMeshContainer());

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pDynamicMesh->Get_NumMaterials(i);

		// 메시를 뼈에 붙인다.
		m_pDynamicMesh->Update_SkinnedMesh(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			m_pShader->Begin_Pass(m_iPass);

			if (FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pDynamicMesh->Get_MeshTexture(i, j, MESHTEXTURE::TYPE_DIFFUSE))))
				return E_FAIL;

			if (FAILED(m_pShader->Set_Texture("g_NormalTexture", m_pDynamicMesh->Get_MeshTexture(i, j, MESHTEXTURE::TYPE_NORMAL))))
				return E_FAIL;

			if (FAILED(m_pShader->Set_Texture("g_SpecularTexture", m_pDynamicMesh->Get_MeshTexture(i, j, MESHTEXTURE::TYPE_SPECULAR))))
				return E_FAIL;

			m_pShader->Commit_Changes();

			m_pDynamicMesh->Render_Mesh(i, j);

			m_pShader->End_Pass();
		}
	}

	m_pShader->End_Shader();

	Draw_Collider();

	//IF_NOT_NULL(m_pNavMesh)
	m_pNavMesh->Render_NaviMesh();

	return NOERROR;
}

HRESULT CPlayer::Render_GameObject_SetPass(CShader* pShader, _int iPass)
{
	if (nullptr == pShader ||
		nullptr == m_pDynamicMesh)
		return E_FAIL;

	_mat		ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat		ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;
	g_pManagement->Set_LightPos(0, m_pTransform->Get_Pos() + _v3(0, 2, 2));

	_uint iNumMeshContainer = _uint(m_pDynamicMesh->Get_NumMeshContainer());

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pDynamicMesh->Get_NumMaterials(i);

		m_pDynamicMesh->Update_SkinnedMesh(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			pShader->Begin_Pass(iPass);

			m_pDynamicMesh->Render_Mesh(i, j);

			pShader->End_Pass();
		}
	}

	return NOERROR;
}

void CPlayer::Parameter_State()
{
	switch (m_eActState)
	{
	case ACT_Idle:
	{
		Play_Idle();
		break;
	}

	case ACT_Run:
	{
		Play_Run();
		break;
	}

	case ACT_Dash:
	{
		Play_Dash();
		break;
	}

	case ACT_MoveDelay:
	{
		Play_MoveDelay();
		break;
	}

	case ACT_Dodge:
	{
		Play_Dodge();
		break;
	}

	case ACT_WeakAtk:
	{
		Play_WeakAtk();
		break;
	}

	case ACT_HeavyAtk:
	{
		Play_HeavyAtk();
		break;
	}

	case ACT_Skill:
	{
		Play_Skills();
		break;
	}

	case ACT_BloodSuck:
	{
		Play_BloodSuck();
		break;
	}

	case ACT_BloodSuck_Count:
	{
		Play_BloodSuckCount();
		break;
	}

	case ACT_BloodSuck_Combo:
	{
		Play_BloodSuckCombo();
		break;
	}

	case ACT_Buff:
	{
		Play_Buff();
		break;
	}

	case ACT_Hit:
	{
		Play_Hit();
		break;
	}

	case ACT_Dead:
	{
		Play_Dead();
		break;
	}

	case ACT_WeaponChange:
	{
		Play_WeaponChange();
		break;
	}

	case ACT_PickUp:
	{
		Play_PickUp();
	}
	}
}

void CPlayer::Parameter_State_Upper()
{
}

void CPlayer::Parameter_Atk()
{
}

void CPlayer::Parameter_Movement()
{
	if (m_eActState == ACT_Hit)
		return;

	if (m_bOnDodge)
	{
		if (m_pTarget != nullptr)
		{
			_float fAngle = D3DXToDegree(m_pTransform->Chase_Target_Angle(&TARGET_TO_TRANS(m_pTarget)->Get_Pos()));
			m_pTransform->Set_Angle(AXIS_Y, D3DXToRadian(fAngle));
		}

		return;
	}

	if (m_bOnPickUp)
		return;

	_float fMoveSpeed = m_tInfo.fMoveSpeed_Cur * DELTA_60;
	_float fRadian = 0.f;
	_float fRecover = 720.f;
	_float fAngle = 0.f;
		
	if (false == m_bOnAttack)
		fAngle = (m_pTarget != nullptr ?
			D3DXToDegree(m_pTransform->Chase_Target_Angle(&TARGET_TO_TRANS(m_pTarget)->Get_Pos())) :
			CCameraMgr::Get_Instance()->Get_XAngle());

	else
		fAngle = D3DXToDegree(m_pTransform->Get_Angle().y);

	//if (m_bOnAiming)
	//	fMoveSpeed = 0.f;

	if (m_bOnAiming && m_pTarget != nullptr)
		Movement_Aiming(fAngle, fMoveSpeed);

	else
		Movement_NonAiming(fRecover, fAngle, fRadian, fMoveSpeed);
}

void CPlayer::Parameter_HeavyCharging()
{
	if (m_eActState == ACT_Hit)
		return;

	if (m_bCharging)
	{
		m_fChargeTimer_Cur += DELTA_60;
	}
}

void CPlayer::Parameter_YPos()
{
	m_pTransform->Set_Pos(m_pNavMesh->Axis_Y_OnNavMesh(m_pTransform->Get_Pos()));
}

void CPlayer::Parameter_Collision()
{
	// 콜라이더 위치 업데이트
	Update_Collider();

	// 밀리는거 관련
	OnCollisionEnter();
}

void CPlayer::Parameter_Aiming()
{
	if (true == m_bOnAiming)
	{
		if (nullptr != m_pTarget)
		{
			if (m_pTarget->Get_Target_Hp() <= 0.f)
			{
				m_bHaveAimingTarget = false;
				m_bOnAiming = false;

				CCameraMgr::Get_Instance()->Set_AimingTarget(nullptr);
				CCameraMgr::Get_Instance()->Set_OnAimingTarget(false);

				return;
			}
		}

		Target_AimChasing();

		m_pTransform->Set_Angle(AXIS_Y, m_pTransform->Chase_Target_Angle(&TARGET_TO_TRANS(m_pTarget)->Get_Pos()));
	}

	else if (false == m_bOnAiming)
	{
		//cout << "타겟팅 해제" << endl;

		if (nullptr != m_pTarget)
		{
			m_pTarget = nullptr;
			m_bHaveAimingTarget = false;

			CCameraMgr::Get_Instance()->Set_AimingTarget(m_pTarget);
			CCameraMgr::Get_Instance()->Set_OnAimingTarget(false);
		}
	}
}

void CPlayer::Parameter_HitCheck()
{
	// 플레이어가 외부에서 맞고
	if (false == m_tObjParam.bCanHit)
	{
		if (false == m_tObjParam.bIsHit)
		{
			if (m_tObjParam.fHp_Cur <= 0)
			{
				m_eActState = ACT_Dead;
			}

			else if (m_tObjParam.fHp_Cur > 0)
			{
				if (m_eActState != ACT_Hit)
				{
					m_bOnMoveDelay = false;

					m_eActState = ACT_Hit;
				}
			}
		}

		else if (true == m_tObjParam.bIsHit)
		{
			if (m_tObjParam.bHitAgain)
			{
				m_eActState = ACT_Hit;

				m_bOnMoveDelay = false;
				m_tObjParam.bHitAgain = false;
				m_pDynamicMesh->Reset_OldIndx();
			}
		}
	}
}

void CPlayer::Movement_Aiming(_float _fAngle, _float _fMovespeed)
{
	if (m_bMove[MOVE_Front] || m_bMove[MOVE_Back] || m_bMove[MOVE_Right] || m_bMove[MOVE_Left])
	{
		_v3 tmpLook;

		if (m_bMove[MOVE_Front])
		{
			if (m_bMove[MOVE_Left])
			{
				V3_NORMAL(&tmpLook, &(-m_pTransform->Get_Axis(AXIS_X) + m_pTransform->Get_Axis(AXIS_Z)));
			}

			else if (m_bMove[MOVE_Right])
			{
				V3_NORMAL(&tmpLook, &(m_pTransform->Get_Axis(AXIS_X) + m_pTransform->Get_Axis(AXIS_Z)));
			}

			else
			{
				tmpLook = m_pTransform->Get_Axis(AXIS_Z);
			}
		}

		else if (m_bMove[MOVE_Back])
		{
			if (m_bMove[MOVE_Left])
			{
				V3_NORMAL(&tmpLook, &(-m_pTransform->Get_Axis(AXIS_X) + -m_pTransform->Get_Axis(AXIS_Z)));
			}

			else if (m_bMove[MOVE_Right])
			{
				V3_NORMAL(&tmpLook, &(m_pTransform->Get_Axis(AXIS_X) + -m_pTransform->Get_Axis(AXIS_Z)));
			}

			else
			{
				tmpLook = -m_pTransform->Get_Axis(AXIS_Z);
			}
		}

		else if (m_bMove[MOVE_Left])
		{
			if (m_bMove[MOVE_Front])
			{
				V3_NORMAL(&tmpLook, &(-m_pTransform->Get_Axis(AXIS_X) + m_pTransform->Get_Axis(AXIS_Z)));
			}

			else if (m_bMove[MOVE_Back])
			{
				V3_NORMAL(&tmpLook, &(-m_pTransform->Get_Axis(AXIS_X) + -m_pTransform->Get_Axis(AXIS_Z)));
			}

			else
			{
				tmpLook = -m_pTransform->Get_Axis(AXIS_X);
			}
		}

		else if (m_bMove[MOVE_Right])
		{
			if (m_bMove[MOVE_Front])
			{
				V3_NORMAL(&tmpLook, &(m_pTransform->Get_Axis(AXIS_X) + m_pTransform->Get_Axis(AXIS_Z)));
			}

			else if (m_bMove[MOVE_Back])
			{
				V3_NORMAL(&tmpLook, &(m_pTransform->Get_Axis(AXIS_X) + -m_pTransform->Get_Axis(AXIS_Z)));
			}

			else
			{
				tmpLook = m_pTransform->Get_Axis(AXIS_X);
			}
		}

		D3DXVec3Normalize(&tmpLook, &tmpLook);

		m_pTransform->Set_Angle(AXIS_Y, D3DXToRadian(_fAngle));
		m_pTransform->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransform->Get_Pos(), &tmpLook, _fMovespeed)));
	}
}

void CPlayer::Movement_NonAiming(_float _fRecover, _float _fAngle, _float _fRadian, _float _fMoveSpeed)
{
	if (m_bMove[MOVE_Front])
	{
		if (m_bMove[MOVE_Left] == true)
		{
			if (m_fAngle_Recover > -45.f)
			{
				m_fAngle_Recover -= _fRecover * DELTA_60;

				if (m_fAngle_Recover <= -45.f)
				{
					m_fAngle_Recover = -45.f;
				}
			}

			else if (m_fAngle_Recover < -45.f)
			{
				m_fAngle_Recover += _fRecover * DELTA_60;

				if (m_fAngle_Recover >= -45.f)
				{
					m_fAngle_Recover = -45.f;
				}
			}
		}

		else if (m_bMove[MOVE_Right] == true)
		{
			if (m_fAngle_Recover < 45.f)
			{
				m_fAngle_Recover += _fRecover * DELTA_60;

				if (m_fAngle_Recover >= 45.f)
				{
					m_fAngle_Recover = 45.f;
				}
			}

			else if (m_fAngle_Recover > 45.f)
			{
				m_fAngle_Recover -= _fRecover * DELTA_60;

				if (m_fAngle_Recover <= 45.f)
				{
					m_fAngle_Recover = 45.f;
				}
			}
		}

		else
		{
			if (m_fAngle_Recover > 180.f)
			{
				m_fAngle_Recover += _fRecover * DELTA_60;

				if (m_fAngle_Recover >= 360.f)
				{
					m_fAngle_Recover -= 360.f;
				}
			}

			else if (m_fAngle_Recover > 0.f)
			{
				m_fAngle_Recover -= _fRecover * DELTA_60;

				if (m_fAngle_Recover <= 0.f)
				{
					m_fAngle_Recover = 0.f;
				}
			}

			else if (m_fAngle_Recover < 0.f)
			{
				m_fAngle_Recover += _fRecover * DELTA_60;

				if (m_fAngle_Recover >= 0.f)
				{
					m_fAngle_Recover = 0.f;
				}
			}
		}

		if(false == m_bOnAttack)
			_fRadian = D3DXToRadian(_fAngle + m_fAngle_Recover);

		else if (true == m_bOnAttack)
			_fRadian = D3DXToRadian(_fAngle);

		m_pTransform->Set_Angle({ 0, _fRadian, 0 });

		_v3 tmpLook = m_pTransform->Get_Axis(AXIS_Z);
		D3DXVec3Normalize(&tmpLook, &tmpLook);

		// 네비 메쉬 추가되면 바꿔야함
		//m_pTransform->Add_Pos(fMoveSpeed);
		m_pTransform->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransform->Get_Pos(), &tmpLook, _fMoveSpeed)));
	}

	else if (m_bMove[MOVE_Left])
	{
		if (m_fAngle_Recover < -270)
		{
			m_fAngle_Recover += _fRecover * DELTA_60;

			if (m_fAngle_Recover <= -360.f)
			{
				m_fAngle_Recover += 360.f;
			}
		}

		else if (m_fAngle_Recover <= 0)
		{
			m_fAngle_Recover -= _fRecover * DELTA_60;

			if (m_fAngle_Recover <= -90.f)
			{
				m_fAngle_Recover = -90.f;
			}
		}

		else if (m_fAngle_Recover > 90)
		{
			m_fAngle_Recover += _fRecover * DELTA_60;

			if (m_fAngle_Recover >= 270.f)
			{
				m_fAngle_Recover = 270.f;
			}
		}

		else if (m_fAngle_Recover > 0)
		{
			m_fAngle_Recover -= _fRecover * DELTA_60;

			if (m_fAngle_Recover <= -90.f)
			{
				m_fAngle_Recover = -90.f;
			}
		}

		if (false == m_bOnAttack)
			_fRadian = D3DXToRadian(_fAngle + m_fAngle_Recover);

		else if (true == m_bOnAttack)
			_fRadian = D3DXToRadian(_fAngle);

		m_pTransform->Set_Angle({ 0, _fRadian, 0 });

		_v3 tmpLook = m_pTransform->Get_Axis(AXIS_Z);
		D3DXVec3Normalize(&tmpLook, &tmpLook);

		// 네비 메쉬 추가되면 바꿔야함
		//m_pTransform->Add_Pos(fMoveSpeed);
		m_pTransform->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransform->Get_Pos(), &tmpLook, _fMoveSpeed)));
	}

	else if (m_bMove[MOVE_Right])
	{
		if (m_fAngle_Recover < -90.f)
		{
			m_fAngle_Recover -= _fRecover * DELTA_60;

			if (m_fAngle_Recover <= -270.f)
			{
				m_fAngle_Recover = 90.f;
			}
		}

		else if (m_fAngle_Recover < 0)
		{
			m_fAngle_Recover += _fRecover * DELTA_60;

			if (m_fAngle_Recover >= 180.f)
			{
				m_fAngle_Recover = 180.f;
			}
		}

		else if (m_fAngle_Recover > 270.f)
		{
			m_fAngle_Recover += _fRecover * DELTA_60;

			if (m_fAngle_Recover >= 360.f)
			{
				m_fAngle_Recover -= 360.f;
			}
		}

		else if (m_fAngle_Recover >= 0)
		{
			m_fAngle_Recover += _fRecover * DELTA_60;

			if (m_fAngle_Recover >= 90.f)
			{
				m_fAngle_Recover = 90.f;
			}
		}

		if (false == m_bOnAttack)
			_fRadian = D3DXToRadian(_fAngle + m_fAngle_Recover);

		else if (true == m_bOnAttack)
			_fRadian = D3DXToRadian(_fAngle);

		m_pTransform->Set_Angle({ 0, _fRadian, 0 });

		_v3 tmpLook = m_pTransform->Get_Axis(AXIS_Z);
		D3DXVec3Normalize(&tmpLook, &tmpLook);

		// 네비 메쉬 추가되면 바꿔야함
		//m_pTransform->Add_Pos(fMoveSpeed);
		m_pTransform->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransform->Get_Pos(), &tmpLook, _fMoveSpeed)));
	}

	else if (m_bMove[MOVE_Back])
	{
		if (m_bMove[MOVE_Left] == true)
		{
			if (m_fAngle_Recover < 225.f)
			{
				m_fAngle_Recover += _fRecover * DELTA_60;

				if (m_fAngle_Recover >= 225.f)
				{
					m_fAngle_Recover = 225.f;
				}
			}
		}

		else if (m_bMove[MOVE_Right] == true)
		{
			if (m_fAngle_Recover > 135.f)
			{
				m_fAngle_Recover -= _fRecover * DELTA_60;

				if (m_fAngle_Recover <= 135.f)
				{
					m_fAngle_Recover = 135.f;
				}
			}
		}

		else
		{
			if (m_fAngle_Recover > 180.f)
			{
				m_fAngle_Recover -= _fRecover * DELTA_60;

				if (m_fAngle_Recover <= 180.f)
				{
					m_fAngle_Recover = 180.f;
				}
			}

			else if (m_fAngle_Recover < 0.f)
			{
				m_fAngle_Recover -= _fRecover * DELTA_60;

				if (m_fAngle_Recover <= -180.f)
				{
					m_fAngle_Recover = -180.f;
				}
			}

			else if (m_fAngle_Recover < 180.f)
			{
				m_fAngle_Recover += _fRecover * DELTA_60;

				if (m_fAngle_Recover >= 180.f)
				{
					m_fAngle_Recover = 180.f;
				}
			}
		}

		if (false == m_bOnAttack)
			_fRadian = D3DXToRadian(_fAngle + m_fAngle_Recover);

		else if (true == m_bOnAttack)
			_fRadian = D3DXToRadian(_fAngle);

		m_pTransform->Set_Angle({ 0, _fRadian, 0 });

		_v3 tmpLook = m_pTransform->Get_Axis(AXIS_Z);
		D3DXVec3Normalize(&tmpLook, &tmpLook);

		// 네비 메쉬 추가되면 바꿔야함
		//m_pTransform->Add_Pos(fMoveSpeed);
		m_pTransform->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransform->Get_Pos(), &tmpLook, _fMoveSpeed)));
	}
}

void CPlayer::Target_AimChasing()
{
	if (m_bHaveAimingTarget)
		return;

	for (auto& iter : g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE))
	{
		if(true == iter->Get_Dead())
			continue;

		if (false == iter->Get_Enable())
			continue;

		_float fLength = D3DXVec3Length(&(TARGET_TO_TRANS(iter)->Get_Pos() - m_pTransform->Get_Pos()));

		if (fLength > m_fAmingRange)
			continue;

		m_bHaveAimingTarget = true;

		m_pTarget = iter;

		CCameraMgr::Get_Instance()->Set_AimingTarget(m_pTarget);
		CCameraMgr::Get_Instance()->Set_OnAimingTarget(true);

		m_pTransform->Set_Angle(AXIS_Y, m_pTransform->Chase_Target_Angle(&TARGET_TO_TRANS(m_pTarget)->Get_Pos()));

		return;
	}

	m_bOnAiming = false;

	return;
}

void CPlayer::KeyInput()
{
	if (CCameraMgr::Get_Instance()->Get_CamView() == TOOL_VIEW)
		return;

	if (m_eActState == ACT_Hit)
		return;

	if (m_eActState == ACT_Dead)
		return;

	KeyDown();
	KeyUp();
}

void CPlayer::KeyDown()
{
	if (false == m_bStopMovementKeyInput)
	{
		// 이동관련
		Key_Movement_Down();

		// 공격관련
		Key_Attack();

		// 무기 변경
		Key_ChangeWeapon();

		// 스킬
		Key_Skill();

		// 흡혈
		Key_BloodSuck();

		// 상호작용
		Key_InterAct();
	}

	// 특수동작
	Key_Special();
}

void CPlayer::KeyUp()
{
	// 이동관련
	Key_Movement_Up();

	//
	if (false == m_bCharging)
		return;

	if (g_pInput_Device->Get_DIMouseState_Up(CInput_Device::DIM_RB))
	{
		m_bCharging = false;
		m_fChargeTimer_Cur = 0.f;
	}
}

void CPlayer::Key_Movement_Down()
{
	if (m_eActState == ACT_Hit)
		return;

	if (true == m_bOnSkill)
		return;

	if (g_pInput_Device->Key_Down(DIK_W))
	{
		m_bMove[MOVE_Front] = true;
	}

	else if (g_pInput_Device->Key_Down(DIK_S))
	{
		m_bMove[MOVE_Back] = true;
	}

	if (g_pInput_Device->Key_Down(DIK_A))
	{
		m_bMove[MOVE_Left] = true;
	}

	else if (g_pInput_Device->Key_Down(DIK_D))
	{
		m_bMove[MOVE_Right] = true;
	}

	if (g_pInput_Device->Key_Down(DIK_LSHIFT))
	{
		m_bSprint = true;
	}


	if (m_bMove[MOVE_Front] || m_bMove[MOVE_Back] || m_bMove[MOVE_Left] || m_bMove[MOVE_Right])
	{
		if (true == m_bOnDodge)
			return;

		if (true == m_bOnAttack)
			return;

		if (false == m_bChangeWeapon && false == m_bOnBuff)
		{
			m_eActState = (m_bSprint ? ACT_Dash : ACT_Run);

			if (m_tInfo.fMoveSpeed_Max > m_tInfo.fMoveSpeed_Cur)
			{
				m_tInfo.fMoveSpeed_Cur += 1.f + m_tInfo.fMoveAccel_Cur * m_tInfo.fMoveAccel_Cur * DELTA_60;
				m_tInfo.fMoveAccel_Cur += DELTA_60;
			}
		}

		else if (true == m_bOnAiming)
		{
			_float tmpSpeedValue = 0.75f;

			if (m_bMove[MOVE_Back])
				tmpSpeedValue = 0.33f;

			if (m_tInfo.fMoveSpeed_Max * tmpSpeedValue > m_tInfo.fMoveSpeed_Cur)
			{
				m_tInfo.fMoveSpeed_Cur += 0.1f + m_tInfo.fMoveAccel_Cur * m_tInfo.fMoveAccel_Cur * DELTA_60;
				m_tInfo.fMoveAccel_Cur += DELTA_60;
			}

			else
			{
				m_tInfo.fMoveSpeed_Cur = m_tInfo.fMoveSpeed_Max * tmpSpeedValue;
			}
		}

		else
		{
			if (m_tInfo.fMoveSpeed_Max * 0.25f > m_tInfo.fMoveSpeed_Cur)
			{
				m_tInfo.fMoveSpeed_Cur += 0.1f + m_tInfo.fMoveAccel_Cur * m_tInfo.fMoveAccel_Cur * DELTA_60;
				m_tInfo.fMoveAccel_Cur += DELTA_60;
			}

			else
			{
				m_tInfo.fMoveSpeed_Cur = m_tInfo.fMoveSpeed_Max * 0.25f;
			}
		}
	}

	else
	{
		if (m_eActState == ACT_Skill)
			return;

		if (m_eActState == ACT_Dead)
			return;

		if (m_eActState == ACT_Hit)
			return;

		if (m_eActState == ACT_Dodge)
			return;

		if (m_eActState == ACT_WeakAtk)
			return;

		if (m_eActState == ACT_HeavyAtk)
			return;

		if (m_eActState == ACT_MoveDelay)
			return;

		if (m_eActState == ACT_WeaponChange)
			return;

		if (m_eActState == ACT_Buff)
			return;

		if (m_eActState == ACT_BloodSuck)
			return;

		if (m_eActState == ACT_BloodSuck_Count)
			return;

		if (m_eActState == ACT_PickUp)
			return;

		if (m_eActState == ACT_Idle)
			return;

		if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.001f))
		{
			m_eActState = (m_eActState == ACT_Run ? ACT_MoveDelay : ACT_Idle);

			m_bOnDodge = false;
		}
	}


}

void CPlayer::Key_Movement_Up()
{
	if (g_pInput_Device->Key_Up(DIK_W))
	{
		m_bMove[MOVE_Front] = false;
	}

	else if (g_pInput_Device->Key_Up(DIK_S))
	{
		m_bMove[MOVE_Back] = false;
	}

	if (g_pInput_Device->Key_Up(DIK_A))
	{
		m_bMove[MOVE_Left] = false;
	}

	else if (g_pInput_Device->Key_Up(DIK_D))
	{
		m_bMove[MOVE_Right] = false;
	}

	if (g_pInput_Device->Key_Up(DIK_LSHIFT))
	{
		m_bSprint = false;
	}
}

void CPlayer::Key_ChangeWeapon()
{
	if (g_pInput_Device->Key_Down(DIK_UPARROW))
	{
		Change_Weapon();
	}
}

void CPlayer::Key_Special()
{
	if (g_pInput_Device->Key_Down(DIK_SPACE))
	{
		if (m_bCanDodge)
		{
			m_bOnDodge = false;

			LOOP(3)
				m_pDynamicMesh->Reset_OldIndx(i);

			m_eActState = ACT_Dodge;
		}
	}

	if (g_pInput_Device->Key_Down(DIK_Q))
	{
		m_bOnAiming = (m_bOnAiming == false ? true : false);

		if (false == m_bOnAiming)
		{
			CCameraMgr::Get_Instance()->Set_LockAngleX(D3DXToDegree(m_pTransform->Get_Angle(AXIS_Y)));
		}
	}
}

void CPlayer::Key_Attack()
{
	if (m_eActState == ACT_BloodSuck)
		return;

	if (m_eActState == ACT_BloodSuck_Combo)
		return;

	if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		if (m_bReadyChargeSuck == true)
		{
			if (m_bOnAttack && m_bSprint)
			{
				m_eActState = ACT_BloodSuck_Combo;
			}

			else if (false == m_bOnAttack)
			{
				m_eActState = ACT_BloodSuck;
			}

			return;
		}

		if (false == m_bOnCombo)
		{
			if (m_bCanAttack == false)
			{
				if (m_pDynamicMesh->Is_Finish_Animation_Lower(m_fAtkEndTime))
				{
					m_bCanAttack = (m_sWeakAtkCnt < m_sWeakAtkCnt_Max ? true : false);
					m_bOnAttack = (m_sWeakAtkCnt < m_sWeakAtkCnt_Max ? false : true);
				}
			}

			if (m_bCanAttack == true)
			{
				LOOP(16)
					m_bEventTrigger[i] = false;

				m_tInfo.fMoveSpeed_Cur = 0.f;
				m_tInfo.fMoveAccel_Cur = 0.f;

				if (m_bSprint)
				{
					m_eActState = ACT_WeakAtk;
					m_bCanAttack = false;

					m_fAtkEndTime = 0.75f;
				}

				else if (m_sWeakAtkCnt < m_sWeakAtkCnt_Max)
				{
					m_eActState = ACT_WeakAtk;
					m_bCanAttack = false;

					m_sWeakAtkCnt += 1;
					switch (m_eMainWpnState)
					{
					case WEAPON_None:
					{
						m_fAtkEndTime = (m_sWeakAtkCnt == 3 ? 0.18f : 0.2f);
						break;
					}

					case WEAPON_Ssword:
					{
						m_fAtkEndTime = (m_sWeakAtkCnt == 3 ? 0.18f : 0.2f);
						break;
					}

					case WEAPON_LSword:
					{
						break;
					}

					case WEAPON_Halverd:
					{
						break;
					}

					case WEAPON_Hammer:
					{
						m_fAtkEndTime = 0.35f;
						break;
					}

					case WEAPON_Gun:
					{
						m_fAtkEndTime = 0.2f;
						break;
					}
					}
				}
			}
		}
	}

	if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_RB))
	{
		if (false == m_bOnCombo)
		{
			if (m_bCanAttack == false)
			{
				if (m_pDynamicMesh->Is_Finish_Animation_Lower(m_fAtkEndTime))
				{
					m_bCanAttack = (m_sHeavyAtkCnt < m_sHeavyAtkCnt_Max ? true : false);
					m_bOnAttack = (m_sHeavyAtkCnt < m_sHeavyAtkCnt_Max ? false : true);
				}
			}

			if (m_bCanAttack == true)
			{
				if (m_bSprint)
				{
					m_eActState = ACT_HeavyAtk;
					m_bCanAttack = false;

					m_fAtkEndTime = 0.75f;
				}

				else if (m_sHeavyAtkCnt < m_sHeavyAtkCnt_Max)
				{
					m_eActState = ACT_HeavyAtk;
					m_bCanAttack = false;

					m_sHeavyAtkCnt += 1;
					m_fAtkEndTime = 0.75f;

					m_bCharging = true;
				}
			}
		}
	}
}

void CPlayer::Key_Skill()
{
	// 1번 스킬
	if (g_pInput_Device->Key_Down(DIK_1))
	{
		if (true == m_bOnSkill)
			return;

		m_eActState = ACT_Skill;

		if (true == m_bOneHand)
			m_eAnim_Lower = P_ANI(m_vecFullSkillInfo[0]->dwAnimationIdx);

		else if (false == m_bOneHand)
			m_eAnim_Lower = P_ANI(m_vecFullSkillInfo[4]->dwAnimationIdx);
	}

	// 2번 스킬
	else if (g_pInput_Device->Key_Down(DIK_2))
	{
		if (true == m_bOnSkill)
			return;

		m_eActState = ACT_Skill;

		if (true == m_bOneHand)
			m_eAnim_Lower = P_ANI(m_vecFullSkillInfo[1]->dwAnimationIdx);

		else if (false == m_bOneHand)
			m_eAnim_Lower = P_ANI(m_vecFullSkillInfo[5]->dwAnimationIdx);
	}

	// 3번 스킬
	else if (g_pInput_Device->Key_Down(DIK_3))
	{
		if (true == m_bOnSkill)
			return;

		m_eActState = ACT_Skill;

		if(true == m_bOneHand)
			m_eAnim_Lower = P_ANI(m_vecFullSkillInfo[2]->dwAnimationIdx);

		else if (false == m_bOneHand)
			m_eAnim_Lower = P_ANI(m_vecFullSkillInfo[6]->dwAnimationIdx);
	}

	// 4번 스킬
	else if (g_pInput_Device->Key_Down(DIK_4))
	{
		if (true == m_bOnSkill)
			return;

		m_eActState = ACT_Skill;

		if (true == m_bOneHand)
			m_eAnim_Lower = P_ANI(m_vecFullSkillInfo[3]->dwAnimationIdx);

		else if (false == m_bOneHand)
			m_eAnim_Lower = P_ANI(m_vecFullSkillInfo[7]->dwAnimationIdx);
	}

	// 1번 버프
	else if (g_pInput_Device->Key_Down(DIK_5))
	{
		if (true == m_bOnSkill)
			return;

		m_eActState = ACT_Buff;

		m_eAnim_Lower = P_ANI(m_vecFullSkillInfo[8]->dwAnimationIdx);
	}
}

void CPlayer::Key_Utility()
{
}

void CPlayer::Key_InterAct()
{
	if (g_pInput_Device->Key_Down(DIK_E))
	{
		m_eActState = ACT_PickUp;

		//===========================================
		// 아이템 줍기
		//===========================================
	}
}

void CPlayer::Key_BloodSuck()
{
	if (g_pInput_Device->Key_Down(DIK_LCONTROL))
	{
		if (m_bReadyChargeSuck == false)
			m_bReadyChargeSuck = true;
	}

	else if (g_pInput_Device->Key_Up(DIK_LCONTROL))
	{
		m_bReadyChargeSuck = false;
	}

	if (g_pInput_Device->Key_Down(DIK_C))
	{
		m_eActState = ACT_BloodSuck_Count;
	}
}

void CPlayer::Play_Idle()
{
	switch (m_eMainWpnState)
	{
	case WEAPON_None:
	{
		m_eAnim_Lower = Cmn_Idle;
		break;
	}

	case WEAPON_Ssword:
	{
		m_eAnim_Lower = Cmn_Idle;
		break;
	}

	case WEAPON_LSword:
	{
		m_eAnim_Lower = Cmn_Idle;
		break;
	}

	case WEAPON_Hammer:
	{
		m_eAnim_Lower = Hammer_Idle_Loop;
		break;
	}

	case WEAPON_Halverd:
	{
		m_eAnim_Lower = Cmn_Idle;
		break;
	}
	case WEAPON_Gun:
	{
		m_eAnim_Lower = Gun_Idle_Loop;
		break;
	}
	}

	m_eAnim_Upper = m_eAnim_Lower;
	m_eAnim_RightArm = m_eAnim_Lower;
}

void CPlayer::Play_Run()
{
	if (false == m_bOnAiming)
	{
		switch (m_eMainWpnState)
		{
		case WEAPON_None:
		{
			m_eAnim_Lower = Cmn_Run_F;
			break;
		}

		case WEAPON_Ssword:
		{
			m_eAnim_Lower = Cmn_Run_F;
			break;
		}

		case WEAPON_LSword:
		{
			m_eAnim_Lower = Cmn_Run_F;
			break;
		}

		case WEAPON_Hammer:
		{
			m_eAnim_Lower = Cmn_Run_F;
			break;
		}

		case WEAPON_Halverd:
		{
			m_eAnim_Lower = Cmn_Run_F;
			break;
		}
		case WEAPON_Gun:
		{
			m_eAnim_Lower = Cmn_Run_F;
			break;
		}
		}
	}

	else if (true == m_bOnAiming)
	{
		switch (m_eMainWpnState)
		{
		case WEAPON_None:
		{
			if (m_bMove[MOVE_Front])
			{
				if (m_bMove[MOVE_Left])
				{
					m_eAnim_Lower = Cmn_Run_FL_L;
				}

				else if (m_bMove[MOVE_Right])
				{
					m_eAnim_Lower = Cmn_Run_FR;
				}

				else
				{
					m_eAnim_Lower = Cmn_Run_F;
				}
			}

			else if (m_bMove[MOVE_Back])
			{
				if (m_bMove[MOVE_Left])
				{
					m_eAnim_Lower = Cmn_Run_FL_L;
				}

				else if (m_bMove[MOVE_Right])
				{
					m_eAnim_Lower = Cmn_Run_FR;
				}

				else
				{
					m_eAnim_Lower = Cmn_Run_B;
				}
			}

			else if (m_bMove[MOVE_Left])
			{
				if (m_bMove[MOVE_Front])
				{
					m_eAnim_Lower = Cmn_Run_FL_L;
				}

				else if (m_bMove[MOVE_Back])
				{
					m_eAnim_Lower = Cmn_Run_BL;
				}

				else
				{
					m_eAnim_Lower = Cmn_Run_L;
				}
			}

			else if (m_bMove[MOVE_Right])
			{
				if (m_bMove[MOVE_Front])
				{
					m_eAnim_Lower = Cmn_Run_FR;
				}

				else if (m_bMove[MOVE_Back])
				{
					m_eAnim_Lower = Cmn_Run_BR_L;
				}

				else
				{
					m_eAnim_Lower = Cmn_Run_R;
				}
			}

			break;
		}

		case WEAPON_Ssword:
		{
			m_eAnim_Lower = Cmn_Run_F;
			break;
		}

		case WEAPON_LSword:
		{
			m_eAnim_Lower = Cmn_Run_F;
			break;
		}

		case WEAPON_Hammer:
		{
			m_eAnim_Lower = Cmn_Run_F;
			break;
		}

		case WEAPON_Halverd:
		{
			m_eAnim_Lower = Cmn_Run_F;
			break;
		}
		case WEAPON_Gun:
		{
			m_eAnim_Lower = Cmn_Run_F;
			break;
		}
		}
	}

	if (false == m_bChangeWeapon)
	{
		m_eAnim_Upper = m_eAnim_Lower;

		m_eAnim_RightArm =
			(m_eMainWpnState == WEAPON_Hammer ? Hammer_Blend_Run :
				m_eMainWpnState == WEAPON_Gun ? Gun_Blend_Run :
				m_eMainWpnState == WEAPON_LSword ? Lsword_Blend_Run : m_eAnim_Lower);

		m_eAnim_LeftArm = (
			m_eMainWpnState == WEAPON_Gun ? Gun_Blend_Run :
			m_eMainWpnState == WEAPON_Halverd ? Halverd_Run_Blend : m_eAnim_Lower);
	}

	g_pManagement->Create_FootSmoke_Effect(m_pTransform->Get_Pos(), 0.5f);
}

void CPlayer::Play_Dash()
{
	switch (m_eMainWpnState)
	{
	case WEAPON_None:
	{
		m_eAnim_Lower = Cmn_Dash;
		break;
	}

	case WEAPON_Ssword:
	{
		m_eAnim_Lower = Cmn_Dash;
		break;
	}

	case WEAPON_LSword:
	{
		m_eAnim_Lower = Cmn_Dash;
		break;
	}

	case WEAPON_Hammer:
	{
		m_eAnim_Lower = Cmn_Dash;
		break;
	}

	case WEAPON_Halverd:
	{
		m_eAnim_Lower = Cmn_Dash;
		break;
	}
	case WEAPON_Gun:
	{
		m_eAnim_Lower = Cmn_Dash;
		break;
	}
	}

	m_eAnim_Upper = m_eAnim_Lower;
	m_eAnim_RightArm = m_eAnim_Lower;
}

void CPlayer::Play_MoveDelay()
{
	if (false == m_bOnMoveDelay)
	{
		m_bOnMoveDelay = true;

		if (m_bOnAiming)
		{
			if (m_bMove[MOVE_Front])
			{
				if (m_bMove[MOVE_Left])
				{
					m_eAnim_Lower = Cmn_Run_FL_L_End;
				}

				else if (m_bMove[MOVE_Right])
				{
					m_eAnim_Lower = Cmn_Run_FR_End_L;
				}

				else
				{
					m_eAnim_Lower = Cmn_Run_F_End_L;
				}
			}

			else if (m_bMove[MOVE_Back])
			{
				if (m_bMove[MOVE_Left])
				{
					m_eAnim_Lower = Cmn_Run_FL_L_End;
				}

				else if (m_bMove[MOVE_Right])
				{
					m_eAnim_Lower = Cmn_Run_FR_End_L;
				}

				else
				{
					m_eAnim_Lower = Cmn_Run_B_End_L;
				}
			}

			else if (m_bMove[MOVE_Left])
			{
				if (m_bMove[MOVE_Front])
				{
					m_eAnim_Lower = Cmn_Run_FL_L_End;
				}

				else if (m_bMove[MOVE_Back])
				{
					m_eAnim_Lower = Cmn_Run_BL_End_R;
				}

				else
				{
					m_eAnim_Lower = Cmn_Run_L_End_L;
				}
			}

			else if (m_bMove[MOVE_Right])
			{
				if (m_bMove[MOVE_Front])
				{
					m_eAnim_Lower = Cmn_Run_FR_End_L;
				}

				else if (m_bMove[MOVE_Back])
				{
					m_eAnim_Lower = Cmn_Run_BR_End_L;
				}

				else
				{
					m_eAnim_Lower = Cmn_Run_R_End_L;
				}
			}
		}
		else
			m_eAnim_Lower = Cmn_Run_F_End_L;

		m_eAnim_Upper = m_eAnim_Lower;

		if (m_bOneHand)
		{
			if (m_eMainWpnState == WEAPON_Gun)
			{
				m_eAnim_RightArm = Gun_Blend_Walk;
				m_eAnim_LeftArm = Gun_Blend_Walk;
			}
			else
			{
				m_eAnim_RightArm = m_eAnim_Lower;
				m_eAnim_LeftArm = m_eAnim_RightArm;
			}
		}

		m_fSkillMoveAccel_Cur = 0.f;
		m_fSkillMoveSpeed_Cur = m_tInfo.fMoveSpeed_Cur;
	}

	else if (true == m_bOnMoveDelay)
	{
		Decre_Skill_Movement(0.75f);
		Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));

		if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.8f))
		{
			m_eActState = ACT_Idle;
			m_bOnMoveDelay = false;
		}
	}
}

void CPlayer::Play_Dodge()
{
	if (false == m_bOnDodge)
	{
		m_bDodgeBack = false;

		switch (m_eMainWpnState)
		{
		case WEAPON_None:
		{
			if (m_bMove[MOVE_Front] || m_bMove[MOVE_Back] || m_bMove[MOVE_Right] || m_bMove[MOVE_Left])
			{
				m_bDodgeBack = false;

				if (false == m_bOnAiming)
				{
					m_eAnim_Lower = Cmn_RealLightDodge_F;
					g_pManagement->Create_AutoFindEffect(L"Player_SpaceBar_StepParticle", 1.f, m_pTransform, _v3(0.f, 1.f, 0.f));
					break;
				}

				if (m_bMove[MOVE_Front])
				{
					if (m_bMove[MOVE_Left])
					{
						m_eAnim_Lower = Cmn_RealLightDodge_FL;
					}

					else if (m_bMove[MOVE_Right])
					{
						m_eAnim_Lower = Cmn_RealLightDodge_FR;
					}

					else
					{
						m_eAnim_Lower = Cmn_RealLightDodge_F;
					}
				}

				else if (m_bMove[MOVE_Back])
				{
					if (m_bMove[MOVE_Left])
					{
						m_eAnim_Lower = Cmn_RealLightDodge_BL;
					}

					else if (m_bMove[MOVE_Right])
					{
						m_eAnim_Lower = Cmn_RealLightDodge_BR;
					}

					else
					{
						m_eAnim_Lower = Cmn_RealLightDodge_B;
					}
				}

				else if (m_bMove[MOVE_Left])
				{
					if (m_bMove[MOVE_Front])
					{
						m_eAnim_Lower = Cmn_RealLightDodge_FL;
					}

					else if (m_bMove[MOVE_Back])
					{
						m_eAnim_Lower = Cmn_RealLightDodge_BL;
					}

					else
					{
						m_eAnim_Lower = Cmn_RealLightDodge_L;
					}
				}

				else if (m_bMove[MOVE_Right])
				{
					if (m_bMove[MOVE_Front])
					{
						m_eAnim_Lower = Cmn_RealLightDodge_FR;
					}

					else if (m_bMove[MOVE_Back])
					{
						m_eAnim_Lower = Cmn_RealLightDodge_BR;
					}

					else
					{
						m_eAnim_Lower = Cmn_RealLightDodge_R;
					}
				}
			}
			else
			{
				m_bDodgeBack = true;
				m_eAnim_Lower = Cmn_RealLightDodge_B;
				g_pManagement->Create_Effect(L"Player_FootSmoke_Jump", m_pTransform->Get_Pos());
				g_pManagement->Create_Effect(L"Player_FootSmoke_DodgeBack", V3_NULL, m_pTransform);
			}

			break;
		}

		case WEAPON_Ssword:
		{
			if (m_bMove[MOVE_Front] || m_bMove[MOVE_Back] || m_bMove[MOVE_Right] || m_bMove[MOVE_Left])
			{
				m_bDodgeBack = false;

				if (false == m_bOnAiming)
				{
					m_eAnim_Lower = Cmn_RealLightDodge_F;
					break;
				}

				if (m_bMove[MOVE_Front])
				{
					if (m_bMove[MOVE_Left])
					{
						m_eAnim_Lower = Cmn_RealLightDodge_FL;
					}

					else if (m_bMove[MOVE_Right])
					{
						m_eAnim_Lower = Cmn_RealLightDodge_FR;
					}

					else
					{
						m_eAnim_Lower = Cmn_RealLightDodge_F;
					}
				}

				else if (m_bMove[MOVE_Back])
				{
					if (m_bMove[MOVE_Left])
					{
						m_eAnim_Lower = Cmn_RealLightDodge_BL;
					}

					else if (m_bMove[MOVE_Right])
					{
						m_eAnim_Lower = Cmn_RealLightDodge_BR;
					}

					else
					{
						m_eAnim_Lower = Cmn_RealLightDodge_B;
					}
				}

				else if (m_bMove[MOVE_Left])
				{
					if (m_bMove[MOVE_Front])
					{
						m_eAnim_Lower = Cmn_RealLightDodge_FL;
					}

					else if (m_bMove[MOVE_Back])
					{
						m_eAnim_Lower = Cmn_RealLightDodge_BL;
					}

					else
					{
						m_eAnim_Lower = Cmn_RealLightDodge_L;
					}
				}

				else if (m_bMove[MOVE_Right])
				{
					if (m_bMove[MOVE_Front])
					{
						m_eAnim_Lower = Cmn_RealLightDodge_FR;
					}

					else if (m_bMove[MOVE_Back])
					{
						m_eAnim_Lower = Cmn_RealLightDodge_BR;
					}

					else
					{
						m_eAnim_Lower = Cmn_RealLightDodge_R;
					}
				}

			}
			else
			{
				m_bDodgeBack = true;
				m_eAnim_Lower = Cmn_RealLightDodge_B;
				g_pManagement->Create_Effect(L"Player_FootSmoke_Jump", m_pTransform->Get_Pos());
				g_pManagement->Create_Effect(L"Player_FootSmoke_DodgeBack", V3_NULL, m_pTransform);
			}
			break;
		}

		case WEAPON_LSword:
		{
			if (m_bMove[MOVE_Front] || m_bMove[MOVE_Back] || m_bMove[MOVE_Right] || m_bMove[MOVE_Left])
			{
				m_bDodgeBack = false;

				if (false == m_bOnAiming)
				{
					m_eAnim_Lower = Lsword_Dodge_F;
					break;
				}

				if (m_bMove[MOVE_Front])
				{
					if (m_bMove[MOVE_Left])
					{
						m_eAnim_Lower = Lsword_Dodge_FL;
					}

					else if (m_bMove[MOVE_Right])
					{
						m_eAnim_Lower = Lsword_Dodge_FR;
					}

					else
					{
						m_eAnim_Lower = Lsword_Dodge_F;
					}
				}

				else if (m_bMove[MOVE_Back])
				{
					if (m_bMove[MOVE_Left])
					{
						m_eAnim_Lower = Lsword_Dodge_BL;
					}

					else if (m_bMove[MOVE_Right])
					{
						m_eAnim_Lower = Lsword_Dodge_BR;
					}

					else
					{
						m_eAnim_Lower = Lsword_Dodge_B;
					}
				}

				else if (m_bMove[MOVE_Left])
				{
					if (m_bMove[MOVE_Front])
					{
						m_eAnim_Lower = Lsword_Dodge_FL;
					}

					else if (m_bMove[MOVE_Back])
					{
						m_eAnim_Lower = Lsword_Dodge_BL;
					}

					else
					{
						m_eAnim_Lower = Lsword_Dodge_L;
					}
				}

				else if (m_bMove[MOVE_Right])
				{
					if (m_bMove[MOVE_Front])
					{
						m_eAnim_Lower = Lsword_Dodge_FR;
					}

					else if (m_bMove[MOVE_Back])
					{
						m_eAnim_Lower = Lsword_Dodge_BR;
					}

					else
					{
						m_eAnim_Lower = Lsword_Dodge_R;
					}
				}

			}
			else
			{
				m_bDodgeBack = true;
				m_eAnim_Lower = Lsword_Dodge_B;
			}
			break;
		}

		case WEAPON_Hammer:
		{
			if (m_bMove[MOVE_Front] || m_bMove[MOVE_Back] || m_bMove[MOVE_Right] || m_bMove[MOVE_Left])
			{
				m_bDodgeBack = false;

				if (false == m_bOnAiming)
				{
					m_eAnim_Lower = Hammer_Dodge_F;
					break;
				}

				if (m_bMove[MOVE_Front])
				{
					if (m_bMove[MOVE_Left])
					{
						m_eAnim_Lower = Hammer_Dodge_FL;
					}

					else if (m_bMove[MOVE_Right])
					{
						m_eAnim_Lower = Hammer_Dodge_FR;
					}

					else
					{
						m_eAnim_Lower = Hammer_Dodge_F;
					}
				}

				else if (m_bMove[MOVE_Back])
				{
					if (m_bMove[MOVE_Left])
					{
						m_eAnim_Lower = Hammer_Dodge_BL;
					}

					else if (m_bMove[MOVE_Right])
					{
						m_eAnim_Lower = Hammer_Dodge_BR;
					}

					else
					{
						m_eAnim_Lower = Hammer_Dodge_B;
					}
				}

				else if (m_bMove[MOVE_Left])
				{
					if (m_bMove[MOVE_Front])
					{
						m_eAnim_Lower = Hammer_Dodge_FL;
					}

					else if (m_bMove[MOVE_Back])
					{
						m_eAnim_Lower = Hammer_Dodge_BL;
					}

					else
					{
						m_eAnim_Lower = Hammer_Dodge_L;
					}
				}

				else if (m_bMove[MOVE_Right])
				{
					if (m_bMove[MOVE_Front])
					{
						m_eAnim_Lower = Hammer_Dodge_FR;
					}

					else if (m_bMove[MOVE_Back])
					{
						m_eAnim_Lower = Hammer_Dodge_BR;
					}

					else
					{
						m_eAnim_Lower = Hammer_Dodge_R;
					}
				}
			}

			else
			{
				m_bDodgeBack = true;
				m_eAnim_Lower = Hammer_Dodge_B;
			}
			break;
		}

		case WEAPON_Halverd:
		{
			if (m_bMove[MOVE_Front] || m_bMove[MOVE_Back] || m_bMove[MOVE_Right] || m_bMove[MOVE_Left])
			{
				m_bDodgeBack = false;

				if (false == m_bOnAiming)
				{
					m_eAnim_Lower = Halverd_Dodge_F;
					break;
				}

				if (m_bMove[MOVE_Front])
				{
					if (m_bMove[MOVE_Left])
					{
						m_eAnim_Lower = Halverd_Dodge_LF;
					}

					else if (m_bMove[MOVE_Right])
					{
						m_eAnim_Lower = Halverd_Dodge_RF;
					}

					else
					{
						m_eAnim_Lower = Halverd_Dodge_F;
					}
				}

				else if (m_bMove[MOVE_Back])
				{
					if (m_bMove[MOVE_Left])
					{
						m_eAnim_Lower = Halverd_Dodge_LB;
					}

					else if (m_bMove[MOVE_Right])
					{
						m_eAnim_Lower = Halverd_Dodge_RB;
					}

					else
					{
						m_eAnim_Lower = Halverd_Dodge_B;
					}
				}

				else if (m_bMove[MOVE_Left])
				{
					if (m_bMove[MOVE_Front])
					{
						m_eAnim_Lower = Halverd_Dodge_LF;
					}

					else if (m_bMove[MOVE_Back])
					{
						m_eAnim_Lower = Halverd_Dodge_LB;
					}

					else
					{
						m_eAnim_Lower = Halverd_Dodge_L;
					}
				}

				else if (m_bMove[MOVE_Right])
				{
					if (m_bMove[MOVE_Front])
					{
						m_eAnim_Lower = Halverd_Dodge_RF;
					}

					else if (m_bMove[MOVE_Back])
					{
						m_eAnim_Lower = Halverd_Dodge_RB;
					}

					else
					{
						m_eAnim_Lower = Halverd_Dodge_R;
					}
				}
			}

			else
			{
				m_bDodgeBack = true;
				m_eAnim_Lower = Halverd_Dodge_B;
			}
			break;
		}

		case WEAPON_Gun:
		{
			if (m_bMove[MOVE_Front] || m_bMove[MOVE_Back] || m_bMove[MOVE_Right] || m_bMove[MOVE_Left])
			{
				m_bDodgeBack = false;

				if (false == m_bOnAiming)
				{
					m_eAnim_Lower = Gun_DodgeRoll_F;
					break;
				}

				if (m_bMove[MOVE_Front])
				{
					if (m_bMove[MOVE_Left])
					{
						m_eAnim_Lower = Gun_DodgeRoll_FL;
					}

					else if (m_bMove[MOVE_Right])
					{
						m_eAnim_Lower = Gun_DodgeRoll_FR;
					}

					else
					{
						m_eAnim_Lower = Gun_DodgeRoll_F;
					}
				}

				else if (m_bMove[MOVE_Back])
				{
					if (m_bMove[MOVE_Left])
					{
						m_eAnim_Lower = Gun_DodgeRoll_BL;
					}

					else if (m_bMove[MOVE_Right])
					{
						m_eAnim_Lower = Gun_DodgeRoll_BR;
					}

					else
					{
						m_eAnim_Lower = Gun_DodgeRoll_B;
					}
				}

				else if (m_bMove[MOVE_Left])
				{
					if (m_bMove[MOVE_Front])
					{
						m_eAnim_Lower = Gun_DodgeRoll_FL;
					}

					else if (m_bMove[MOVE_Back])
					{
						m_eAnim_Lower = Gun_DodgeRoll_BL;
					}

					else
					{
						m_eAnim_Lower = Gun_DodgeRoll_L;
					}
				}

				else if (m_bMove[MOVE_Right])
				{
					if (m_bMove[MOVE_Front])
					{
						m_eAnim_Lower = Gun_DodgeRoll_FR;
					}

					else if (m_bMove[MOVE_Back])
					{
						m_eAnim_Lower = Gun_DodgeRoll_BR;
					}

					else
					{
						m_eAnim_Lower = Gun_DodgeRoll_R;
					}
				}
			}

			else
			{
				m_bDodgeBack = true;
				m_eAnim_Lower = Gun_DodgeRoll_B;
			}
			break;
		}
		}

		m_eAnim_Upper = m_eAnim_Lower;
		m_eAnim_RightArm = m_eAnim_Lower;

		m_bOnDodge = true;
		m_bCanDodge = false;
		m_bStopMovementKeyInput = true;

		m_fSkillMoveSpeed_Cur =
			(m_eMainWpnState == WEAPON_LSword ? 12.f :
				m_eMainWpnState == WEAPON_Hammer ? 12.f :
				m_eMainWpnState == WEAPON_Halverd ? 12.f :
				m_eMainWpnState == WEAPON_Ssword ? 12.f :
				m_eMainWpnState == WEAPON_Gun ? 12.f : 15.f);

		m_fSkillMoveMultiply =
			(m_eMainWpnState == WEAPON_LSword ? 0.8f :
				m_eMainWpnState == WEAPON_Hammer ? 0.8f :
				m_eMainWpnState == WEAPON_Halverd ? 0.8f :
				m_eMainWpnState == WEAPON_Ssword ? 2.f :
				m_eMainWpnState == WEAPON_Gun ? 0.6f : 1.2f);

		m_fSkillMoveAccel_Cur = 0.f;
	}

	else
	{
		_v3 vDir;

		if (true == m_bOnAiming)
		{
			if (m_bMove[MOVE_Front])
			{
				if (m_bMove[MOVE_Right])
				{
					V3_NORMAL(&vDir, &(m_pTransform->Get_Axis(AXIS_X) + m_pTransform->Get_Axis(AXIS_Z)));
				}

				else if (m_bMove[MOVE_Left])
				{
					V3_NORMAL(&vDir, &(-m_pTransform->Get_Axis(AXIS_X) + m_pTransform->Get_Axis(AXIS_Z)));
				}

				else
					vDir = m_pTransform->Get_Axis(AXIS_Z);
			}

			else if (m_bMove[MOVE_Back])
			{
				if (m_bMove[MOVE_Right])
				{
					V3_NORMAL(&vDir, &(m_pTransform->Get_Axis(AXIS_X) + -m_pTransform->Get_Axis(AXIS_Z)));
				}

				else if (m_bMove[MOVE_Left])
				{
					V3_NORMAL(&vDir, &(-m_pTransform->Get_Axis(AXIS_X) + -m_pTransform->Get_Axis(AXIS_Z)));
				}

				else
					vDir = -m_pTransform->Get_Axis(AXIS_Z);
			}

			else if (m_bMove[MOVE_Right])
			{
				if (m_bMove[MOVE_Front])
				{
					V3_NORMAL(&vDir, &(m_pTransform->Get_Axis(AXIS_X) + m_pTransform->Get_Axis(AXIS_Z)));
				}

				else if (m_bMove[MOVE_Back])
				{
					V3_NORMAL(&vDir, &(m_pTransform->Get_Axis(AXIS_X) + -m_pTransform->Get_Axis(AXIS_Z)));
				}

				else
					vDir = m_pTransform->Get_Axis(AXIS_X);
			}

			else if (m_bMove[MOVE_Left])
			{
				if (m_bMove[MOVE_Front])
				{
					V3_NORMAL(&vDir, &(-m_pTransform->Get_Axis(AXIS_X) + m_pTransform->Get_Axis(AXIS_Z)));
				}

				else if (m_bMove[MOVE_Back])
				{
					V3_NORMAL(&vDir, &(-m_pTransform->Get_Axis(AXIS_X) + -m_pTransform->Get_Axis(AXIS_Z)));
				}

				else
					vDir = -m_pTransform->Get_Axis(AXIS_X);
			}
		}

		else
			vDir = (m_bDodgeBack ? -m_pTransform->Get_Axis(AXIS_Z) : m_pTransform->Get_Axis(AXIS_Z));

		Decre_Skill_Movement(m_fSkillMoveMultiply);
		Skill_Movement(m_fSkillMoveSpeed_Cur, vDir);

		if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.75f))
		{
			m_bDodgeBack = false;
			m_bAFK = false;
			m_bOnDodge = false;
			m_bCanDodge = true;
			m_bStopMovementKeyInput = false;

			if (m_bMove[MOVE_Front] || m_bMove[MOVE_Back] || m_bMove[MOVE_Right] || m_bMove[MOVE_Left])
			{
				m_eActState = (m_bSprint ? ACT_Dash : ACT_Run);
			}

			else
				m_eActState = ACT_Idle;

			Reset_BattleState();
		}

		else if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.25f))
		{
			m_bCanDodge = true;
			m_bStopMovementKeyInput = false;
		}
	}
}

void CPlayer::Play_WeakAtk()
{
	switch (m_eMainWpnState)
	{
	case WEAPON_None:
	{
		Play_Ssword_WeakAtk();
		break;
	}

	case WEAPON_Ssword:
	{
		Play_Ssword_WeakAtk();
		break;
	}

	case WEAPON_LSword:
	{
		Play_Lsword_WeakAtk();
		break;
	}

	case WEAPON_Hammer:
	{
		Play_Hammer_WeakAtk();
		break;
	}

	case WEAPON_Halverd:
	{
		Play_Halverd_WeakAtk();
		break;
	}
	case WEAPON_Gun:
	{
		Play_Gun_WeakAtk();
		break;
	}
	}
}

void CPlayer::Play_HeavyAtk()
{
	if (false == m_bOnAttack)
	{
		switch (m_eMainWpnState)
		{
		case WEAPON_None:
		{
			Play_Ssword_HeavyAtk();
			break;
		}

		case WEAPON_Ssword:
		{
			Play_Ssword_HeavyAtk();
			break;
		}

		case WEAPON_LSword:
		{
			Play_Lsword_HeavyAtk();
			break;
		}

		case WEAPON_Hammer:
		{
			Play_Hammer_HeavyAtk();
			break;
		}

		case WEAPON_Halverd:
		{
			Play_Halverd_HeavyAtk();
			break;
		}
		case WEAPON_Gun:
		{
			Play_Gun_HeavyAtk();
			break;
		}
		}

		m_eAnim_Upper = m_eAnim_Lower;
		m_eAnim_RightArm = m_eAnim_Lower;

		m_bOnAttack = true;
	}

	else if (true == m_bOnAttack)
	{
		switch (m_eMainWpnState)
		{
		case WEAPON_None:
		{
			Play_Ssword_HeavyAtk();
			break;
		}

		case WEAPON_Ssword:
		{
			Play_Ssword_HeavyAtk();
			break;
		}

		case WEAPON_LSword:
		{
			Play_Lsword_HeavyAtk();
			break;
		}

		case WEAPON_Hammer:
		{
			Play_Hammer_HeavyAtk();
			break;
		}

		case WEAPON_Halverd:
		{
			Play_Halverd_HeavyAtk();
			break;
		}
		case WEAPON_Gun:
		{
			Play_Gun_HeavyAtk();
			break;
		}
		}
	}
}

void CPlayer::Play_Buff()
{
	if (false == m_bOnBuff)
	{
		m_bOnBuff = true;

		m_eAnim_Upper = Cmn_UseItem;
		m_eAnim_Lower = m_eAnim_Upper;
		m_eAnim_RightArm = m_eAnim_Upper;
		m_eAnim_LeftArm = m_eAnim_RightArm;

		m_fAnimMutiply = 1.f;
	}

	else if (true == m_bOnBuff)
	{
		if (m_bMove[MOVE_Front] || m_bMove[MOVE_Back] || m_bMove[MOVE_Left] || m_bMove[MOVE_Right])
		{
			m_eAnim_Lower = Cmn_Walk_F;
			m_eAnim_Upper = m_eAnim_Lower;
		}

		else
		{
			m_eAnim_Lower = Cmn_UseEnchant;
			m_eAnim_Upper = m_eAnim_Lower;
		}

		if (m_pDynamicMesh->Is_Finish_Animation_RightArm(0.95f))
		{
			m_bOnBuff = false;
			m_eActState = ACT_Idle;
		}
	}
}

void CPlayer::Play_Hit()
{
	if (false == m_tObjParam.bIsHit)
	{
		m_tObjParam.bIsHit = true;

		m_eAnim_Upper = Cmn_Hit01_F;
		m_eAnim_Lower = m_eAnim_Upper;
		m_eAnim_RightArm = m_eAnim_Upper;
		m_eAnim_LeftArm = m_eAnim_RightArm;

		// 뒤로 밀리는거는 확인해보고
	}

	else if (true == m_tObjParam.bIsHit)
	{
		if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
		{
			m_eActState = ACT_Idle;

			m_tObjParam.bIsHit = false;
			m_tObjParam.bCanHit = true;

			Reset_BattleState();

			//cout << "여긴 탑니까" << endl;
		}

		else if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.2f))
		{
			if (m_tObjParam.bCanHit == false)
			{
				m_tObjParam.bCanHit = true;
			}
		}
	}
}

void CPlayer::Play_Dead()
{
	if (false == m_bIsDead)
	{
		m_bIsDead = true;

		m_eAnim_Upper = Cmn_Dying_Loop;
		m_eAnim_Lower = m_eAnim_Upper;
		m_eAnim_RightArm = m_eAnim_Upper;
		m_eAnim_LeftArm = m_eAnim_RightArm;
	}

	else if (true == m_bIsDead)
	{
		if (m_eAnim_Upper == Cmn_Dying_Loop)
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.95f))
			{
				m_eAnim_Upper = Cmn_Dying_End;
				m_eAnim_Lower = m_eAnim_Upper;
				m_eAnim_RightArm = m_eAnim_Upper;
				m_eAnim_LeftArm = m_eAnim_RightArm;

				m_eActState = ACT_Dead;
			}
		}

		else if (m_eAnim_Upper == Cmn_Dying_End)
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.99f))
			{
				m_fAnimMutiply = 0.f;
				m_eActState = ACT_Dead;
			}
		}
	}
}

void CPlayer::Play_WeaponChange()
{
	if (false == m_bChangeWeapon)
	{
		m_bChangeWeapon = true;

		m_eAnim_Upper = Cmn_WeaponChange;
		m_eAnim_Lower = m_eAnim_Upper;
		m_eAnim_RightArm = m_eAnim_Upper;

		m_fAnimMutiply = 1.f;
	}

	else if (true == m_bChangeWeapon)
	{
		if (m_bMove[MOVE_Front] || m_bMove[MOVE_Back] || m_bMove[MOVE_Left] || m_bMove[MOVE_Right])
		{
			m_eAnim_Lower = Cmn_Walk_F;
		}

		else
			m_eAnim_Lower = Cmn_WeaponChange;

		if (m_pDynamicMesh->Is_Finish_Animation_Upper(0.95f))
		{
			m_eActiveSlot =
				(m_eActiveSlot == WPN_SLOT_A ? WPN_SLOT_B :
					m_eActiveSlot == WPN_SLOT_B ? WPN_SLOT_C : WPN_SLOT_A);

			m_eMainWpnState = m_pWeapon[m_eActiveSlot]->Get_WeaponType();

			m_bChangeWeapon = false;

			m_bOneHand = (m_eMainWpnState == WEAPON_LSword ? false :
				m_eMainWpnState == WEAPON_Gun ? false :
				m_eMainWpnState == WEAPON_Hammer ? false : true);

			m_eActState = ACT_Idle;
		}
	}
}

void CPlayer::Play_Spawn()
{
	const _float SPEED = 0.3f;
	Start_Dissolve(SPEED, true);

	for (auto& iter : m_pWeapon)
		iter->Start_Dissolve(SPEED, true);
	//m_pDrainWeapon->Start_Dissolve(0.7f, true);

	const _float END_VALUE = 0.6f;
	if (m_iPass == 3 && m_fFXAlpha > END_VALUE)
	{
		_v3 vPos = m_pTransform->Get_Pos();
		vPos.y += m_fDissolveY;
		g_pManagement->Create_Spawn_Effect(vPos);
		m_fDissolveY += DELTA_60 * 1.3f;
	}
	else
		m_fDissolveY = 0.f;
}


void CPlayer::Play_PickUp()
{
	if (false == m_bOnPickUp)
	{
		// 픽업일 땐 못움직임
		m_bOnPickUp = true;
		m_bStopMovementKeyInput = true;

		m_eAnim_Upper = Cmn_PickItem;
		m_eAnim_Lower = m_eAnim_Upper;
		m_eAnim_RightArm = m_eAnim_Upper;
		m_eAnim_LeftArm = m_eAnim_Upper;
	}

	else if (true == m_bOnPickUp)
	{
		if (m_pDynamicMesh->Is_Finish_Animation_Lower())
		{
			m_eActState = ACT_Idle;

			m_bStopMovementKeyInput = false;
			m_bOnPickUp = false;

			// 아이템 줍는 이벤트가 있다면 여기 추가
		}
	}
}

void CPlayer::Play_BloodSuck()
{
	if (false == m_bOnBloodSuck)
	{
		m_bOnBloodSuck = true;

		m_eAnim_Upper = LongCoat_ChargeSuck_Start;
		m_eAnim_Lower = m_eAnim_Upper;
		m_eAnim_RightArm = m_eAnim_Upper;

		IF_NOT_NULL(m_pDrainWeapon)
		{
			m_pDrainWeapon->Set_AnimIdx(m_eAnim_Upper);
			m_pDrainWeapon->Set_Active(true);
		}
	}

	else if (true == m_bOnBloodSuck)
	{
		if (false == m_bOnChargeSuck)
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.95f))
			{
				m_eAnim_Upper = LongCoat_ChargeSuck_End;
				m_eAnim_Lower = m_eAnim_Upper;
				m_eAnim_RightArm = m_eAnim_Upper;

				m_bOnChargeSuck = true;

				IF_NOT_NULL(m_pDrainWeapon)
				{
					m_pDrainWeapon->Set_AnimIdx(m_eAnim_Upper);
					m_pDrainWeapon->Set_ActiveCollider(true);
					m_pDrainWeapon->Set_Active(true);
				}
			}

			else if (false == m_bReadyChargeSuck)
			{

				if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.f)
				{
					m_eAnim_Upper = LongCoat_ChargeSuck_End;
					m_eAnim_Lower = m_eAnim_Upper;
					m_eAnim_RightArm = m_eAnim_Upper;

					m_bOnChargeSuck = true;

					IF_NOT_NULL(m_pDrainWeapon)
					{
						m_pDrainWeapon->Set_AnimIdx(m_eAnim_Upper);
						m_pDrainWeapon->Set_ActiveCollider(true);
						m_pDrainWeapon->Set_Active(true);
					}
				}

				else if (m_pDynamicMesh->Get_TrackInfo().Position < 3.f)
				{
					m_eActState = ACT_Idle;

					m_bOnBloodSuck = false;
					m_bOnChargeSuck = false;
					m_bReadyChargeSuck = false;

					IF_NOT_NULL(m_pDrainWeapon)
					{
						m_pDrainWeapon->Set_AnimIdx(m_eAnim_Upper);
						m_pDrainWeapon->Set_ActiveCollider(false);
						m_pDrainWeapon->Set_Active(false);
						m_pDrainWeapon->Set_ResetOldAnimIdx();
					}
				}
			}
		}

		else if (true == m_bOnChargeSuck)
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				m_eActState = ACT_Idle;

				m_bOnChargeSuck = false;
				m_bReadyChargeSuck = false;

				IF_NOT_NULL(m_pDrainWeapon)
				{
					m_pDrainWeapon->Set_AnimIdx(m_eAnim_Upper);
					m_pDrainWeapon->Set_ActiveCollider(false);
					m_pDrainWeapon->Set_Active(false);
				}
			}
		}
	}
}

void CPlayer::Play_BloodSuckCount()
{
	if (false == m_bOnBloodSuck)
	{
		m_bOnBloodSuck = true;

		m_eAnim_Upper = LongCoat_Parry;
		m_eAnim_Lower = m_eAnim_Upper;
		m_eAnim_RightArm = m_eAnim_Upper;

		IF_NOT_NULL(m_pDrainWeapon)
		{
			m_pDrainWeapon->Set_AnimIdx(m_eAnim_Upper);
			m_pDrainWeapon->Set_ActiveCollider(true);
			m_pDrainWeapon->Set_Active(true);
			m_pDrainWeapon->Set_ResetOldAnimIdx();
		}
	}

	else if (true == m_bOnBloodSuck)
	{
		if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
		{
			m_eActState = ACT_Idle;

			m_bOnBloodSuck = false;

			IF_NOT_NULL(m_pDrainWeapon)
			{
				m_pDrainWeapon->Set_ActiveCollider(false);
				m_pDrainWeapon->Set_Active(false);
			}
		}
	}
}

void CPlayer::Play_BloodSuckCombo()
{
	if (false == m_bOnBloodSuck)
	{
		m_bOnBloodSuck = true;

		m_eAnim_Upper = LongCoat_ComboSuck;
		m_eAnim_Lower = m_eAnim_Upper;
		m_eAnim_RightArm = m_eAnim_Upper;

		IF_NOT_NULL(m_pDrainWeapon)
		{
			m_pDrainWeapon->Set_AnimIdx(m_eAnim_Upper);
			m_pDrainWeapon->Set_Active(true);
		}
	}

	else if (true == m_bOnBloodSuck)
	{
		if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.95f))
		{
			m_eActState = ACT_Idle;

			m_bOnBloodSuck = false;
			m_bReadyChargeSuck = false;

			IF_NOT_NULL(m_pDrainWeapon)
			{
				m_pDrainWeapon->Set_AnimIdx(m_eAnim_Upper);
				m_pDrainWeapon->Set_ActiveCollider(true);
				m_pDrainWeapon->Set_Active(true);
			}
		}
	}
}

void CPlayer::Play_Skills()
{
	if (false == m_bOnSkill)
	{
		m_bOnSkill = true;

		m_eAnim_Upper = m_eAnim_Lower;
		m_eAnim_RightArm = m_eAnim_Lower;
		m_eAnim_LeftArm = m_eAnim_Lower;
	}

	else
	{
		switch (m_eAnim_Lower)
		{
		case Renketsu_StrongAtk_01:
		{
			//스킬 추가하고있었고, 회피 중에 맞으면 이상해지는 오류 수정해야함.
			break;
		}
		case Renketsu_StrongAtk_02:
		{
			break;
		}
		case Renketsu_StrongAtk_03:
		{
			break;
		}
		case Renketsu_StrongAtk_11:
		{
			break;
		}
		}
		if (m_pDynamicMesh->Is_Finish_Animation(0.95f))
		{
			m_eActState = ACT_Idle;
			m_bOnSkill = false;
		}
	}
}

void CPlayer::Play_Buffs()
{
}

void CPlayer::Play_Enchants()
{
}

void CPlayer::Play_OneHandSkill_01()
{
}

void CPlayer::Play_OneHandSkill_02()
{
}

void CPlayer::Play_OneHandskill_03()
{
}

void CPlayer::Play_TwoHandSkill_01()
{
}

void CPlayer::Play_TwoHandSkill_02()
{
}

void CPlayer::Play_TwoHandSkill_03()
{
}

void CPlayer::Play_GunSkill()
{
}

void CPlayer::Play_Ssword_WeakAtk()
{
	if (false == m_bOnAttack)
	{
		if (m_bSprint)
		{
			m_eAnim_Lower = Ssword_DownAtk_01;

			m_fSkillMoveSpeed_Cur = 20.f;
			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveMultiply = 4.f;
		}
		else
		{
			m_eAnim_Lower = Player_Anim(Ssword_WeakAtk_01 + (m_sWeakAtkCnt - 1));

			m_fSkillMoveSpeed_Cur = (m_sWeakAtkCnt == 3 ? 8.5f :
									m_sWeakAtkCnt == 4 ? 7.5f : 5.5f);
			m_fSkillMoveAccel_Cur = 0.f;

			m_fSkillMoveMultiply = (m_sWeakAtkCnt == 1 ? 1.f :
									m_sWeakAtkCnt == 3 ? 1.5f :
									m_sWeakAtkCnt == 4 ? 1.35f : 0.75f);
		}

		m_bOnAttack = true;

		m_eAnim_Upper = m_eAnim_Lower;
		m_eAnim_RightArm = m_eAnim_Lower;
		m_eAnim_LeftArm = m_eAnim_Lower;
	}

	else if (true == m_bOnAttack)
	{
		switch (m_eAnim_Lower)
		{
		case Ssword_DownAtk_01:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.1f && m_pDynamicMesh->Get_TrackInfo().Position < 2.53f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}

				if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.95f && m_pDynamicMesh->Get_TrackInfo().Position < 2.53f)
				{
					if (m_bEventTrigger[2] == false)
					{
						m_bEventTrigger[2] = true;

						m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
					}
				}

				else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.73f && m_pDynamicMesh->Get_TrackInfo().Position < 2.53f)
				{
					if (m_bEventTrigger[1] == false)
					{
						m_bEventTrigger[1] = true;

						m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					}
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;
		}

		case Ssword_WeakAtk_01:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.3f && m_pDynamicMesh->Get_TrackInfo().Position < 3.43f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_fSkillMoveSpeed_Cur = 0.6f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.05f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.95f && m_pDynamicMesh->Get_TrackInfo().Position < 1.f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.83f && m_pDynamicMesh->Get_TrackInfo().Position < 0.95f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
				}
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.53f && m_pDynamicMesh->Get_TrackInfo().Position < 0.83f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.1f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;
		}

		case Ssword_WeakAtk_02:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.8f && m_pDynamicMesh->Get_TrackInfo().Position < 2.4f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_fSkillMoveSpeed_Cur = 0.6f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.05f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 1.1f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 1.03f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 0.43f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}

			Decre_Skill_Movement(m_fSkillMoveMultiply);
			Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));

			break;
		}

		case Ssword_WeakAtk_03:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.6f && m_pDynamicMesh->Get_TrackInfo().Position < 3.23f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_fSkillMoveSpeed_Cur = 0.8f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.05f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 0.95f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.2f && m_pDynamicMesh->Get_TrackInfo().Position < 0.9f)
			{
				if (m_pDynamicMesh->Get_TrackInfo().Position > 0.86f)
				{
					if (m_bEventTrigger[1] == false)
					{
						m_bEventTrigger[1] = true;

						m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					}
				}

				else if (m_pDynamicMesh->Get_TrackInfo().Position > 0.53f)
				{
					if (m_bEventTrigger[0] == false)
					{
						m_bEventTrigger[0] = true;

						m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
						m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
					}
				}


				if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.3f && m_pDynamicMesh->Get_TrackInfo().Position < 0.7f)
				{
					Decre_Skill_Movement(m_fSkillMoveMultiply);
					Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
				}
			}

			break;
		}

		case Ssword_WeakAtk_04:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 0.95f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 0.7f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 0.3f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.3f && m_pDynamicMesh->Get_TrackInfo().Position < 0.9f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;
		}

		default:
		{
			Decre_Skill_Movement();
			Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));

			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			break;
		}
		}
	}
}

void CPlayer::Play_Ssword_HeavyAtk()
{
	if (false == m_bOnAttack)
	{
		if (m_bSprint)
		{
			m_fSkillMoveMultiply = 1.2f;

			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveSpeed_Cur = 11.f;

			m_eAnim_Lower = Ssword_SpecialLaunch;
		}
		else
		{
			m_fSkillMoveMultiply = 2.f;

			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveSpeed_Cur = 7.f;

			m_eAnim_Lower = Ssword_Charge;
		}

		m_eAnim_Upper = m_eAnim_Lower;
		m_eAnim_RightArm = m_eAnim_Lower;
		m_eAnim_LeftArm = m_eAnim_Lower;

		m_bOnAttack = true;
	}

	else if (true == m_bOnAttack)
	{
		switch (m_eAnim_Lower)
		{
		case Ssword_Charge:
		{
			Decre_Skill_Movement(m_fSkillMoveMultiply);
			Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));

			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.65f))
			{
				m_eAnim_Lower = (m_fChargeTimer_Cur > 0.2f ? Ssword_Charge_Attack_01_B : Ssword_Charge_Attack_01_A);
				m_eAnim_Upper = m_eAnim_Lower;
				m_eAnim_RightArm = m_eAnim_Lower;

				m_fChargeTimer_Cur = 0.f;

				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveSpeed_Cur = (m_eAnim_Lower == Ssword_Charge_Attack_01_B ? 12.f : 8.f);

				m_fSkillMoveMultiply = 1.f;
			}
			break;
		}
		case Ssword_Charge_Attack_01_B:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.8f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.66f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.3f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.86f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.4f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
				}
			}


			if (m_pDynamicMesh->Get_TrackInfo().Position > 1.35f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;
		}

		case Ssword_Charge_Attack_01_A:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.95f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.83f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.33f && m_pDynamicMesh->Get_TrackInfo().Position < 0.83f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
				}
			}

			Decre_Skill_Movement(m_fSkillMoveMultiply);
			Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));

			break;
		}

		case Ssword_SpecialLaunch:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.4f && m_pDynamicMesh->Get_TrackInfo().Position < 2.1f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
				}

				if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.95f)
				{
					if (m_bEventTrigger[2] == false)
					{
						m_bEventTrigger[2] = true;

						m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
					}
				}

				else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.866f)
				{
					if (m_bEventTrigger[1] == false)
					{
						m_bEventTrigger[1] = true;

						m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					}
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));

				m_fAnimMutiply = 1.f;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 1.3f)
			//else if (m_pDynamicMesh->Get_TrackInfo().Position > 1.35f)
			{
				if (m_fAnimMutiply > 0.5f)
				{
					m_fAnimMutiply -= DELTA_60;

					if (m_fAnimMutiply <= 0.5f)
					{
						m_fAnimMutiply = 0.5f;
					}
				}
			}

			break;
		}
		}
	}
}

void CPlayer::Play_Ssword_DownAtk()
{
}

void CPlayer::Play_Ssword_DashAtk()
{
}

void CPlayer::Play_Gun_WeakAtk()
{
	if (false == m_bOnAttack)
	{
		if (m_bSprint)
		{
			m_eAnim_Lower = Gun_WeakAtk_10;

			m_fSkillMoveSpeed_Cur = 13.f;
			m_fSkillMoveAccel_Cur = 0.f;
		}
		else
		{
			m_eAnim_Lower = Player_Anim(Gun_WeakAtk_01 + (m_sWeakAtkCnt - 1));

			m_fSkillMoveSpeed_Cur = (
				m_sWeakAtkCnt == 1 ? 6.f :
				m_sWeakAtkCnt == 2 ? 10.f :
				m_sWeakAtkCnt == 3 ? 8.f : 12.5f);

			m_fSkillMoveAccel_Cur = 0.f;

			m_fSkillMoveMultiply = (
				m_sWeakAtkCnt == 2 ? 1.3f :
				m_sWeakAtkCnt == 2 ? 1.8f :
				m_sWeakAtkCnt == 3 ? 1.1f : 1.3f);
		}

		m_bOnAttack = true;

		m_eAnim_Upper = m_eAnim_Lower;
		m_eAnim_RightArm = m_eAnim_Lower;
	}

	else if (true == m_bOnAttack)
	{
		switch (m_eAnim_Lower)
		{

		case Gun_WeakAtk_01:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position < 0.88f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;
		}

		case Gun_WeakAtk_02:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.83f && m_pDynamicMesh->Get_TrackInfo().Position < 2.56f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 1.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.3f && m_pDynamicMesh->Get_TrackInfo().Position < 0.8f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;
		}

		case Gun_WeakAtk_03:
		{

			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 1.92f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 0.5f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position < 1.2f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;
		}

		case Gun_WeakAtk_04:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.55f && m_pDynamicMesh->Get_TrackInfo().Position < 3.3f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 1.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.05f && m_pDynamicMesh->Get_TrackInfo().Position < 2.f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));
			}

			break;
		}

		}
	}
}

void CPlayer::Play_Gun_HeavyAtk()
{
	if (false == m_bOnAttack)
	{
		if (m_bSprint)
		{
			m_fSkillMoveMultiply = 1.f;

			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveSpeed_Cur = 7.f;

			m_eAnim_Lower = Gun_WeakAtk_05;
		}
		else
		{
			m_eAnim_Lower = Gun_Charge;
		}

		m_bOnAttack = true;
	}

	else if (true == m_bOnAttack)
	{
		switch (m_eAnim_Lower)
		{
		case Gun_Charge:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.65f))
			{
				m_eAnim_Lower = (m_fChargeTimer_Cur > 0.2f ? Gun_Charge_Attack_01_B : Gun_Charge_Attack_01_A);
				m_eAnim_Upper = m_eAnim_Lower;
				m_eAnim_RightArm = m_eAnim_Lower;

				m_fChargeTimer_Cur = 0.f;

				m_fSkillMoveAccel_Cur = (m_eAnim_Lower == Gun_HeavyAtk_01_B ? 3.f : 1.8f);
				m_fSkillMoveSpeed_Cur = 0.f;

				m_fSkillMoveMultiply = 1.f;
			}
			break;
		}
		case Gun_Charge_Attack_01_A:
		{
			Decre_Skill_Movement(m_fSkillMoveMultiply);
			Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));

			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.92f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}
			break;
		}

		case Gun_Charge_Attack_01_B:
		{
			Decre_Skill_Movement(m_fSkillMoveMultiply);
			Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));

			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.92f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}
			break;
		}
		}
	}
}

void CPlayer::Play_Gun_DownAtk()
{
}

void CPlayer::Play_Gun_DashAtk()
{
}

void CPlayer::Play_Halverd_WeakAtk()
{
}

void CPlayer::Play_Halverd_HeavyAtk()
{
}

void CPlayer::Play_Halverd_DownAtk()
{
}

void CPlayer::Play_Halverd_DashAtk()
{
}

void CPlayer::Play_Hammer_WeakAtk()
{
	if (false == m_bOnAttack)
	{
		if (m_bSprint)
		{
			m_eAnim_Lower = Hammer_Down_02;

			m_fSkillMoveMultiply = 0.1f;
			m_fSkillMoveSpeed_Cur = 3.5f;
			m_fSkillMoveAccel_Cur = 0.f;
		}

		else
		{
			_float fSpeed = 0.f;
			_float fMultiply = 0.f;

			if (m_sWeakAtkCnt == 1)
			{
				m_eAnim_Lower = Hammer_WeakAtk_07;
				fSpeed = 1.8f;
				fMultiply = 0.01f;
			}

			else if (m_sWeakAtkCnt == 2)
			{
				m_eAnim_Lower = Hammer_WeakAtk_04;
				fSpeed = 3.f;
				fMultiply = 0.2f;
			}

			else if (m_sWeakAtkCnt == 3)
			{
				m_eAnim_Lower = Hammer_WeakAtk_02;
				fSpeed = 2.f;
				fMultiply = 0.1f;
			}

			m_fSkillMoveMultiply = fMultiply;
			m_fSkillMoveSpeed_Cur = fSpeed;
			m_fSkillMoveAccel_Cur = 0.f;
		}

		m_bOnAttack = true;

		m_eAnim_Upper = m_eAnim_Lower;
		m_eAnim_RightArm = m_eAnim_Lower;
	}

	else if (true == m_bOnAttack)
	{
		switch (m_eAnim_Lower)
		{
		case Hammer_Down_02:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.233f && m_pDynamicMesh->Get_TrackInfo().Position < 2.4f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;

		}

		case Hammer_WeakAtk_07:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.33f && m_pDynamicMesh->Get_TrackInfo().Position < 1.98f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;

		}

		case Hammer_WeakAtk_02:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 0.f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

		}

		case Hammer_WeakAtk_04:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 0.3f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

		}
		}
	}
}

void CPlayer::Play_Hammer_HeavyAtk()
{
	if (false == m_bOnAttack)
	{
		if (m_bSprint)
		{
			m_fSkillMoveMultiply = 1.f;

			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveSpeed_Cur = 7.f;

			m_eAnim_Lower = Hammer_Down_01;
		}
		else
		{
			m_fSkillMoveMultiply = 0.2f;

			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveSpeed_Cur = 4.f;

			m_eAnim_Lower = Hammer_HeavyAtk_04_B;
		}

	}
	else if (true == m_bOnAttack)
	{
		switch (m_eAnim_Lower)
		{
		case Hammer_HeavyAtk_04_B:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 8.5f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_fSkillMoveMultiply = 0.1f;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 0.9f;

					m_bEventTrigger[1] = true;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 5.5f && m_pDynamicMesh->Get_TrackInfo().Position < 6.35f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_fSkillMoveMultiply = 0.75f;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 6.f;

					m_bEventTrigger[0] = true;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.6f && m_pDynamicMesh->Get_TrackInfo().Position < 5.f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}
			break;
		}

		case Hammer_Down_01:
		{
			if (m_pDynamicMesh->Get_TrackInfo().Position > 1.45f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));

				if (m_fAnimMutiply <= 0.5f)
				{
					m_fAnimMutiply = 2.f;
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 1.35f)
			{
				if (m_fAnimMutiply > 0.5f)
				{
					m_fAnimMutiply -= DELTA_60;

					if (m_fAnimMutiply <= 0.5f)
					{
						m_fAnimMutiply = 0.5f;
					}
				}
			}

			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			break;
		}
		}
	}
}

void CPlayer::Play_Hammer_DownAtk()
{
}

void CPlayer::Play_Hammer_DashAtk()
{
}

void CPlayer::Play_Lsword_WeakAtk()
{
}

void CPlayer::Play_Lsword_HeavyAtk()
{
}

void CPlayer::Play_Lsword_DownAtk()
{
}

void CPlayer::Play_Lsword_DashAtk()
{
}

void CPlayer::Ready_Weapon()
{
	m_pWeapon[WPN_SLOT_A] = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pWeapon[WPN_SLOT_A]->Change_WeaponData(CWeapon::WPN_SSword_Normal);
	m_pWeapon[WPN_SLOT_A]->Set_Friendly(true);
	LPCSTR tmpChar = "RightHandAttach";
	_mat   matAttach;

	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo(tmpChar, 2);

	m_pWeapon[WPN_SLOT_A]->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	m_pWeapon[WPN_SLOT_A]->Set_ParentMatrix(&m_pTransform->Get_WorldMat());


	m_pWeapon[WPN_SLOT_B] = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pWeapon[WPN_SLOT_B]->Change_WeaponData(CWeapon::WPN_Hammer_Normal);

	m_pWeapon[WPN_SLOT_B]->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	m_pWeapon[WPN_SLOT_B]->Set_ParentMatrix(&m_pTransform->Get_WorldMat());
	m_pWeapon[WPN_SLOT_B]->Set_Friendly(true);

	//========================================================================================================================
	// 여기서 부터 디버그용 무기슬롯
	//========================================================================================================================

	m_pWeapon[WPN_SLOT_C] = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pWeapon[WPN_SLOT_C]->Change_WeaponData(CWeapon::WPN_Gun_Normal);

	m_pWeapon[WPN_SLOT_C]->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	m_pWeapon[WPN_SLOT_C]->Set_ParentMatrix(&m_pTransform->Get_WorldMat());
	m_pWeapon[WPN_SLOT_C]->Set_Friendly(true);

	//========================================================================================================================
}

void CPlayer::Ready_DrainWeapon()
{
	LPCSTR tmpChar = "Hips";
	_mat   matAttach;

	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo(tmpChar, 0);

	m_pDrainWeapon = static_cast<CDrain_Weapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_DrainWeapon", NULL));
	m_pDrainWeapon->Set_ParentMatrix(&m_pTransform->Get_WorldMat());
	m_pDrainWeapon->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
}

void CPlayer::Ready_BoneMatrix()
{
	LPCSTR tmpChar = "Head";

	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo(tmpChar, 1);

	m_matBones[Bone_Head] = &pFamre->CombinedTransformationMatrix;

	tmpChar = "Spine";

	pFamre = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo(tmpChar, 1);

	m_matBones[Bone_Range] = &pFamre->CombinedTransformationMatrix;
	m_matBones[Bone_Body] = &pFamre->CombinedTransformationMatrix;
}

void CPlayer::Ready_Collider()
{
	//==============================================================================================================
	m_vecPhysicCol.reserve(10);
	//==============================================================================================================
	// 콜라이더 받아오고
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 1.2f;

	// 첫번째 콜라이더는 경계 체크용 콜라이더
	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBones[Bone_Range]->_41, m_matBones[Bone_Range]->_42, m_matBones[Bone_Range]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	//==============================================================================================================

	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 0.4f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBones[Bone_Body]->_41, m_matBones[Bone_Body]->_42, m_matBones[Bone_Body]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	//==============================================================================================================

	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 0.2f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBones[Bone_Head]->_41, m_matBones[Bone_Head]->_42, m_matBones[Bone_Head]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	//==============================================================================================================
}

void CPlayer::Ready_Skills()
{
	SKILL_INFO* pSkillInfo = new SKILL_INFO();

	// 1 - 스플릿 어비스
	pSkillInfo->bOneHand = true;
	pSkillInfo->dwAnimationIdx = Renketsu_StrongAtk_01;
	pSkillInfo->eCurSkillIdx = Skill_OneHand_Active_01;
	pSkillInfo->dwSkillCost = 2;

	LOOP(4)
		pSkillInfo->ePreSkillIdx[i] = Skill_End;

	m_vecFullSkillInfo.push_back(pSkillInfo);

	//====================================================================================
	pSkillInfo = new SKILL_INFO();

	// 2 - 팬텀 어썰트
	pSkillInfo->bOneHand = true;
	pSkillInfo->dwAnimationIdx = Renketsu_StrongAtk_02;
	pSkillInfo->eCurSkillIdx = Skill_OneHand_Active_02;
	pSkillInfo->dwSkillCost = 2;

	LOOP(4)
		pSkillInfo->ePreSkillIdx[i] = Skill_End;

	m_vecFullSkillInfo.push_back(pSkillInfo);

	//====================================================================================
	pSkillInfo = new SKILL_INFO();

	// 3 - 블러드 서큘러
	pSkillInfo->bOneHand = true;
	pSkillInfo->dwAnimationIdx = Renketsu_StrongAtk_03;
	pSkillInfo->eCurSkillIdx = Skill_OneHand_Active_03;
	pSkillInfo->dwSkillCost = 2;

	LOOP(4)
		pSkillInfo->ePreSkillIdx[i] = Skill_End;

	m_vecFullSkillInfo.push_back(pSkillInfo);

	//====================================================================================
	pSkillInfo = new SKILL_INFO();

	// 4 - 쉐도우 어썰트
	pSkillInfo->bOneHand = true;
	pSkillInfo->dwAnimationIdx = Renketsu_StrongAtk_12;
	pSkillInfo->eCurSkillIdx = Skill_OneHand_Active_04;
	pSkillInfo->dwSkillCost = 2;

	LOOP(4)
		pSkillInfo->ePreSkillIdx[i] = Skill_End;

	m_vecFullSkillInfo.push_back(pSkillInfo);

	//====================================================================================
	pSkillInfo = new SKILL_INFO();

	// 5 - 리전 피니쉬
	pSkillInfo->bOneHand = true;
	pSkillInfo->dwAnimationIdx = Renketsu_StrongAtk_04;
	pSkillInfo->eCurSkillIdx = Skill_TwoHand_Active_01;
	pSkillInfo->dwSkillCost = 2;

	LOOP(4)
		pSkillInfo->ePreSkillIdx[i] = Skill_End;

	m_vecFullSkillInfo.push_back(pSkillInfo);

	//====================================================================================
	pSkillInfo = new SKILL_INFO();

	// 6 - 삼멸살
	pSkillInfo->bOneHand = true;
	pSkillInfo->dwAnimationIdx = Renketsu_StrongAtk_05;
	pSkillInfo->eCurSkillIdx = Skill_TwoHand_Active_02;
	pSkillInfo->dwSkillCost = 2;

	LOOP(4)
		pSkillInfo->ePreSkillIdx[i] = Skill_End;

	m_vecFullSkillInfo.push_back(pSkillInfo);

	//====================================================================================
	pSkillInfo = new SKILL_INFO();

	// 7 - 강룡복호
	pSkillInfo->bOneHand = true;
	pSkillInfo->dwAnimationIdx = Renketsu_StrongAtk_06;
	pSkillInfo->eCurSkillIdx = Skill_TwoHand_Active_03;
	pSkillInfo->dwSkillCost = 2;

	LOOP(4)
		pSkillInfo->ePreSkillIdx[i] = Skill_End;

	m_vecFullSkillInfo.push_back(pSkillInfo);

	//====================================================================================
	pSkillInfo = new SKILL_INFO();

	// 8 - 토먼트 블레이드
	pSkillInfo->bOneHand = true;
	pSkillInfo->dwAnimationIdx = Renketsu_StrongAtk_11;
	pSkillInfo->eCurSkillIdx = Skill_TwoHand_Active_04;
	pSkillInfo->dwSkillCost = 2;

	LOOP(4)
		pSkillInfo->ePreSkillIdx[i] = Skill_End;

	m_vecFullSkillInfo.push_back(pSkillInfo);

	//====================================================================================

	pSkillInfo = new SKILL_INFO();

	// 1 - 버프
	pSkillInfo->bOneHand = true;
	pSkillInfo->dwAnimationIdx = Renketsu_Buff;
	pSkillInfo->eCurSkillIdx = Skill_Buff_Enchant_01;
	pSkillInfo->dwSkillCost = 2;

	LOOP(4)
		pSkillInfo->ePreSkillIdx[i] = Skill_End;

	m_vecFullSkillInfo.push_back(pSkillInfo);

	//====================================================================================

}

void CPlayer::Temp_ActiveSkills()
{
}

void CPlayer::Reset_BloodSuck_Options()
{
	if (ACT_BloodSuck != m_eActState && ACT_BloodSuck_Combo != m_eActState && ACT_BloodSuck_Count != m_eActState)
	{
		if (m_bOnBloodSuck == true || m_bOnChargeSuck == true)
		{
			m_bOnBloodSuck = false;
			m_bOnChargeSuck = false;

			m_pDrainWeapon->Set_Active(false);
			m_pDrainWeapon->Set_ActiveCollider(false);
			m_pDrainWeapon->Set_AnimIdx(0);
			m_pDrainWeapon->Set_ResetOldAnimIdx();
		}
	}
}

void CPlayer::Reset_Attack_Bool()
{
	if (m_eActState != ACT_WeakAtk && m_eActState != ACT_HeavyAtk)
	{
		if (false == m_bEventTrigger[0])
		{
			LOOP(16)
				m_bEventTrigger[i] = false;
		}
	}
}

void CPlayer::Skill_Movement(_float _fspeed, _v3 _vDir)
{
	_v3 tmpLook;
	_float fSpeed = _fspeed;

	tmpLook = _vDir;
	D3DXVec3Normalize(&tmpLook, &tmpLook);

	m_fCurMoveSpeed = _fspeed;

	// 네비게이션 적용하면 
	m_pTransform->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransform->Get_Pos(), &tmpLook, fSpeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));
}

void CPlayer::Decre_Skill_Movement(_float _fMutiply)
{
	m_fSkillMoveSpeed_Cur -= (0.3f - m_fSkillMoveAccel_Cur * m_fSkillMoveAccel_Cur * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60")) * _fMutiply;
	m_fSkillMoveAccel_Cur += g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60");

	if (m_fSkillMoveSpeed_Cur < 0.f)
	{
		m_fSkillMoveAccel_Cur = 0.5f;
		m_fSkillMoveSpeed_Cur = 0.f;
	}
}

void CPlayer::Draw_Collider()
{
	for (auto& iter : m_vecPhysicCol)
	{
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}
}

void CPlayer::Change_Weapon()
{
	// 무기가 없으면 교체 못함
	IF_NULL_RETURN(m_pWeapon[WPN_SLOT_A]);

	// 무기가 없으면 교체 못함
	IF_NULL_RETURN(m_pWeapon[WPN_SLOT_B]);

	if (m_eActState == ACT_WeaponChange)
		return;
	// 바꾼 무기로부터 타입을 받아옴

	// 무기 바꾸는 애니메이션
	m_eActState = ACT_WeaponChange;
	// 여기 무기 바꾸는 코드를 추후에 작성해야 합니다.
}

HRESULT CPlayer::Add_Component()
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_Player", L"Com_MeshDynamic", (CComponent**)&m_pDynamicMesh)))
		return E_FAIL;

	// for.Com_NavMesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"NavMesh", L"Com_NavMesh", (CComponent**)&m_pNavMesh)))
		return E_FAIL;

	// for.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	m_pCollider->Set_Radius(_v3{ 0.5f, 0.5f, 0.5f });
	m_pCollider->Set_Dynamic(true);
	m_pCollider->Set_Type(COL_SPHERE);
	m_pCollider->Set_CenterPos(m_pTransform->Get_Pos() + _v3{ 0.f , m_pCollider->Get_Radius().y , 0.f });

	return NOERROR;
}

HRESULT CPlayer::SetUp_Default()
{
	ZeroMemory(&m_tInfo, sizeof(ACTOR_INFO));

	// Transform
	m_pTransform->Set_Pos(_v3(150.484f, -18.08f, 70.417f));
	//m_pTransform->Set_Pos(_v3(0.f, 0.f, 0.f));
	m_pTransform->Set_Scale(V3_ONE);

	// Mesh
	m_pDynamicMesh->SetUp_Animation(Cmn_Run_F);

	// Info
	//m_tInfo.fHP = 100.f;
	//m_tInfo.fArmor = 0.f;
	//m_tInfo.fStamina = 100.f;
	//m_tInfo.fDamage = 0.f;
	m_tInfo.fMoveAccel_Cur = 0.f;
	m_tInfo.fMoveSpeed_Max = 1.5f;
	m_tInfo.fMoveAccel_Cur = 0.f;
	m_tInfo.fMoveSpeed_Max = 5.f;

	// Parameter
	m_tObjParam.bCanHit = true;
	m_tObjParam.fHp_Cur = 100.f;
	m_tObjParam.fHp_Max = 100.f;

	// Anim
	m_fAnimMutiply = 1.f;

	// Navi
	m_pNavMesh->Ready_NaviMesh(m_pGraphic_Dev, L"Navmesh_Stage_01.dat");
	m_pNavMesh->Set_SubsetIndex(0);
	//m_pNavMesh->Set_Index(14);

	return S_OK;
}

HRESULT CPlayer::SetUp_ConstantTable()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	if (FAILED(m_pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	_mat		ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat		ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(m_pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(g_pDissolveTexture->SetUp_OnShader("g_FXTexture", m_pShader)))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_fFxAlpha", &m_fFXAlpha, sizeof(_float))))
		return E_FAIL;

	return NOERROR;
}

void CPlayer::OnCollisionEnter()
{
	//list<CGameObject*> tmpList =  g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE);
	//
	//for (auto& iter : tmpList)
	//{
	//	CCollider* pCollider = TARGET_TO_COL(iter);
	//
	//	if (m_pCollider->Check_Sphere(pCollider, m_pTransform->Get_Axis(AXIS_Z), m_tInfo.fMoveSpeed_Cur * DELTA_60))
	//	{
	//		CTransform* pTrans = TARGET_TO_TRANS(iter);
	//		CNavMesh*   pNav = TARGET_TO_NAV(iter);
	//
	//		// 방향 구해주고
	//		_v3 vDir = m_pTransform->Get_Pos() - pTrans->Get_Pos();
	//		V3_NORMAL_SELF(&vDir);
	//
	//		// y축 이동은 하지말자
	//		vDir.y = 0;
	//
	//		// 네비 메쉬타게 끔 세팅
	//		pTrans->Set_Pos(pNav->Move_OnNaviMesh(NULL, &pTrans->Get_Pos(), &vDir, m_pCollider->Get_Length().x));
	//	}
	//}
}

void CPlayer::Update_Collider()
{
	_ulong matrixIdx = 0;

	for (auto& iter : m_vecPhysicCol)
	{
		_mat tmpMat;
		tmpMat = *m_matBones[matrixIdx] * m_pTransform->Get_WorldMat();

		_v3 ColPos = _v3(tmpMat._41, tmpMat._42, tmpMat._43);

		iter->Update(ColPos);

		++matrixIdx;
	}

	m_pCollider->Update(m_pTransform->Get_Pos() + _v3(0.f, m_pCollider->Get_Radius().y, 0.f));
}

void CPlayer::Reset_BattleState()
{
	m_fAnimMutiply = 1.f;

	m_bOnAttack = false;
	m_bCanAttack = true;
	m_bCharging = false;
	m_bStopMovementKeyInput = false;

	m_sWeakAtkCnt = 0;
	m_sHeavyAtkCnt = 0;

	m_fChargeTimer_Cur = 0.f;

	m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
	m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);

	LOOP(16)
		m_bEventTrigger[i] = false;
}

CPlayer * CPlayer::Create(_Device pGraphic_Device)
{
	CPlayer*	pInstance = new CPlayer(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject * CPlayer::Clone_GameObject(void * pArg)
{
	CPlayer*	pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	LOOP(3)
	{
		Safe_Release(m_pWeapon[i]);
	}

	for (auto& iter : m_vecFullSkillInfo)
	{
		Safe_Delete(iter);
	}

	Safe_Release(m_pDrainWeapon);

	Safe_Release(m_pCollider);
	Safe_Release(m_pTransform);
	Safe_Release(m_pDynamicMesh);
	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pNavMesh);

	for (auto& iter : m_vecPhysicCol)
	{
		Safe_Release(iter);
	}

	for (auto& iter : m_matBones)
	{
		iter = nullptr;
	}

	CGameObject::Free();
}
