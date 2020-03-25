#include "stdafx.h"
#include "..\Headers\Player.h"
#include "Weapon.h"
#include "CameraMgr.h"
#include "Dummy_Target.h"

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
	Ready_Weapon();

	return NOERROR;
}

_int CPlayer::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	KeyInput();

	Parameter_YPos();
	Parameter_Movement();
	Parameter_State();
	Parameter_Atk();
	Parameter_HeavyCharging();
	Parameter_Collision();

	if (FAILED(m_pRenderer->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	m_pWeapon[m_eActiveSlot]->Update_GameObject(TimeDelta);

	return NO_EVENT;
}

_int CPlayer::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRenderer || 
		nullptr == m_pDynamicMesh)
		return E_FAIL;

	m_pDynamicMesh->SetUp_Animation_Lower(m_eAnim_Lower);
	m_pDynamicMesh->SetUp_Animation_Upper(m_eAnim_Upper);
	m_pDynamicMesh->SetUp_Animation_RightArm(m_eAnim_RightArm);

	m_pWeapon[m_eActiveSlot]->Late_Update_GameObject(TimeDelta);

	return NO_EVENT;
}

HRESULT CPlayer::Render_GameObject()
{
	if (nullptr == m_pShader ||
		nullptr == m_pDynamicMesh)
		return E_FAIL;

	m_pDynamicMesh->Play_Animation_Lower(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMutiply);
	m_pDynamicMesh->Play_Animation_Upper(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMutiply);
	m_pDynamicMesh->Play_Animation_RightArm(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMutiply, true);

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
			m_pShader->Begin_Pass(0);

			if (FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pDynamicMesh->Get_MeshTexture(i, j, MESHTEXTURE::TYPE_DIFFUSE))))
				return E_FAIL;

			m_pShader->Commit_Changes();

			m_pDynamicMesh->Render_Mesh(i, j);

			m_pShader->End_Pass();
		}		
	}

	m_pShader->End_Shader();

	g_pManagement->Gizmo_Draw_Capsule(m_pCollider->Get_CenterPos(), m_pCollider->Get_Radius());

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

	case ACT_BloodSucking:
	{
		Play_BloodSucking();
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
	case ACT_WeaponChange:
	{
		Play_WeaponChange();
		break;
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
	if (m_bOnDodge)
		return;

	_float fMoveSpeed = m_tInfo.fMoveSpeed_Cur * DELTA_60;
	_float fRotate = 720.f * DELTA_60;
	_float fAngle = CCameraMgr::Get_Instance()->Get_XAngle();
	_float fRadian = 0.f;
	_float fRecover = 720.f;

	if (m_bMove[MOVE_Front])
	{
		if (m_bMove[MOVE_Left] == true)
		{
			if (m_fAngle_Recover > -45.f)
			{
				m_fAngle_Recover -= fRecover * DELTA_60;

				if (m_fAngle_Recover <= -45.f)
				{
					m_fAngle_Recover = -45.f;
				}
			}

			else if (m_fAngle_Recover < -45.f)
			{
				m_fAngle_Recover += fRecover * DELTA_60;

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
				m_fAngle_Recover += fRecover * DELTA_60;

				if (m_fAngle_Recover >= 45.f)
				{
					m_fAngle_Recover = 45.f;
				}
			}

			else if (m_fAngle_Recover > 45.f)
			{
				m_fAngle_Recover -= fRecover * DELTA_60;

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
				m_fAngle_Recover += fRecover * DELTA_60;

				if (m_fAngle_Recover >= 360.f)
				{
					m_fAngle_Recover -= 360.f;
				}
			}

			else if (m_fAngle_Recover > 0.f)
			{
				m_fAngle_Recover -= fRecover * DELTA_60;

				if (m_fAngle_Recover <= 0.f)
				{
					m_fAngle_Recover = 0.f;
				}
			}

			else if (m_fAngle_Recover < 0.f)
			{
				m_fAngle_Recover += fRecover * DELTA_60;

				if (m_fAngle_Recover >= 0.f)
				{
					m_fAngle_Recover = 0.f;
				}
			}
		}

		fRadian = D3DXToRadian(fAngle + m_fAngle_Recover);

		m_pTransform->Set_Angle({ 0, fRadian, 0 });

		_v3 tmpLook = m_pTransform->Get_Axis(AXIS_Z);
		D3DXVec3Normalize(&tmpLook, &tmpLook);

		// 네비 메쉬 추가되면 바꿔야함
		//m_pTransform->Add_Pos(fMoveSpeed);
		m_pTransform->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransform->Get_Pos(), &tmpLook, fMoveSpeed)));
	}

	else if (m_bMove[MOVE_Left])
	{
		if (m_fAngle_Recover < -270)
		{
			m_fAngle_Recover += fRecover * DELTA_60;

			if (m_fAngle_Recover <= -360.f)
			{
				m_fAngle_Recover += 360.f;
			}
		}

		else if (m_fAngle_Recover <= 0)
		{
			m_fAngle_Recover -= fRecover * DELTA_60;

			if (m_fAngle_Recover <= -90.f)
			{
				m_fAngle_Recover = -90.f;
			}
		}

		else if (m_fAngle_Recover > 90)
		{
			m_fAngle_Recover += fRecover * DELTA_60;

			if (m_fAngle_Recover >= 270.f)
			{
				m_fAngle_Recover = 270.f;
			}
		}

		else if (m_fAngle_Recover > 0)
		{
			m_fAngle_Recover -= fRecover * DELTA_60;

			if (m_fAngle_Recover <= -90.f)
			{
				m_fAngle_Recover = -90.f;
			}
		}

		fRadian = D3DXToRadian(fAngle + m_fAngle_Recover);
		//_fRadian = D3DXToRadian((m_bAim ? _fAngle : _fAngle + m_fAngle_Recover));

		m_pTransform->Set_Angle({ 0, fRadian, 0 });

		_v3 tmpLook = m_pTransform->Get_Axis(AXIS_Z);
		D3DXVec3Normalize(&tmpLook, &tmpLook);

		// 네비 메쉬 추가되면 바꿔야함
		//m_pTransform->Add_Pos(fMoveSpeed);
		m_pTransform->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransform->Get_Pos(), &tmpLook, fMoveSpeed)));
	}

	else if (m_bMove[MOVE_Right])
	{
		if (m_fAngle_Recover < -90.f)
		{
			m_fAngle_Recover -= fRecover * DELTA_60;

			if (m_fAngle_Recover <= -270.f)
			{
				m_fAngle_Recover = 90.f;
			}
		}

		else if (m_fAngle_Recover < 0)
		{
			m_fAngle_Recover += fRecover * DELTA_60;

			if (m_fAngle_Recover >= 180.f)
			{
				m_fAngle_Recover = 180.f;
			}
		}

		else if (m_fAngle_Recover > 270.f)
		{
			m_fAngle_Recover += fRecover * DELTA_60;

			if (m_fAngle_Recover >= 360.f)
			{
				m_fAngle_Recover -= 360.f;
			}
		}

		else if (m_fAngle_Recover >= 0)
		{
			m_fAngle_Recover += fRecover * DELTA_60;

			if (m_fAngle_Recover >= 90.f)
			{
				m_fAngle_Recover = 90.f;
			}
		}

		fRadian = D3DXToRadian(fAngle + m_fAngle_Recover);

		m_pTransform->Set_Angle({ 0, fRadian, 0 });

		_v3 tmpLook = m_pTransform->Get_Axis(AXIS_Z);
		D3DXVec3Normalize(&tmpLook, &tmpLook);

		// 네비 메쉬 추가되면 바꿔야함
		//m_pTransform->Add_Pos(fMoveSpeed);
		m_pTransform->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransform->Get_Pos(), &tmpLook, fMoveSpeed)));
	}

	else if (m_bMove[MOVE_Back])
	{
		if (m_bMove[MOVE_Left] == true)
		{
			if (m_fAngle_Recover < 225.f)
			{
				m_fAngle_Recover += fRecover * DELTA_60;

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
				m_fAngle_Recover -= fRecover * DELTA_60;

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
				m_fAngle_Recover -= fRecover * DELTA_60;

				if (m_fAngle_Recover <= 180.f)
				{
					m_fAngle_Recover = 180.f;
				}
			}

			else if (m_fAngle_Recover < 0.f)
			{
				m_fAngle_Recover -= fRecover * DELTA_60;

				if (m_fAngle_Recover <= -180.f)
				{
					m_fAngle_Recover = -180.f;
				}
			}

			else if (m_fAngle_Recover < 180.f)
			{
				m_fAngle_Recover += fRecover * DELTA_60;

				if (m_fAngle_Recover >= 180.f)
				{
					m_fAngle_Recover = 180.f;
				}
			}
		}

		fRadian = D3DXToRadian(fAngle + m_fAngle_Recover);

		m_pTransform->Set_Angle({ 0, fRadian, 0 });

		_v3 tmpLook = m_pTransform->Get_Axis(AXIS_Z);
		D3DXVec3Normalize(&tmpLook, &tmpLook);

		// 네비 메쉬 추가되면 바꿔야함
		//m_pTransform->Add_Pos(fMoveSpeed);
		m_pTransform->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransform->Get_Pos(), &tmpLook, fMoveSpeed)));
	}
}

void CPlayer::Parameter_HeavyCharging()
{
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
	Update_Collider();
	OnCollisionEnter();
}

void CPlayer::KeyInput()
{
	if (CCameraMgr::Get_Instance()->Get_CamView() == TOOL_VIEW)
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
		cout << "마우스에서 손땜" << endl;

		m_bCharging = false;
		m_fChargeTimer_Cur = 0.f;
	}
}

void CPlayer::Key_Movement_Down()
{
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
				m_tInfo.fMoveSpeed_Cur += 0.1f + m_tInfo.fMoveAccel_Cur * m_tInfo.fMoveAccel_Cur * DELTA_60;
				m_tInfo.fMoveAccel_Cur += DELTA_60;
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

		if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.1f))
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
}

void CPlayer::Key_Attack()
{
	if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		if (false == m_bOnCombo)
		{
			if (m_bCanAttack == false)
			{
				if(m_pDynamicMesh->Is_Finish_Animation_Lower(m_fAtkEndTime))
				{
					m_bCanAttack = (m_sWeakAtkCnt < m_sWeakAtkCnt_Max ? true : false);
					m_bOnAttack = (m_sWeakAtkCnt < m_sWeakAtkCnt_Max ? false : true);
				}
			}

			if (m_bCanAttack == true)
			{
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
					m_eActState =  ACT_WeakAtk;
					m_bCanAttack = false;

					m_sWeakAtkCnt += 1;
					m_fAtkEndTime = (m_sWeakAtkCnt == 3 ? 0.18f : 0.2f);
				}
			}
		}
	}

	else if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_RB))
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
	if (g_pInput_Device->Key_Down(DIK_1))
	{
		m_eActState = ACT_Buff;
	}

	else if (g_pInput_Device->Key_Down(DIK_2))
	{

	}

	else if (g_pInput_Device->Key_Down(DIK_3))
	{

	}

	else if (g_pInput_Device->Key_Down(DIK_3))
	{

	}
}

void CPlayer::Key_Utility()
{
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
		m_eAnim_Lower = Cmn_Idle;
		break;
	}
	}

	m_eAnim_Upper = m_eAnim_Lower;
	m_eAnim_RightArm = m_eAnim_Lower;
}

void CPlayer::Play_Run()
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

	if (false == m_bChangeWeapon)
	{
		m_eAnim_Upper = m_eAnim_Lower;
		m_eAnim_RightArm = m_eAnim_Lower;
	}
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

		m_eAnim_Lower = Cmn_Run_F_End_L;
		m_eAnim_Upper = m_eAnim_Lower;
		m_eAnim_RightArm = m_eAnim_Lower;

		m_fSkillMoveAccel_Cur = 0.f;
		m_fSkillMoveSpeed_Cur = m_tInfo.fMoveSpeed_Cur;
	}
	
	else if ( true == m_bOnMoveDelay)
	{
		//cout << m_fSkillMoveSpeed_Cur << endl;

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
		switch (m_eMainWpnState)
		{
		case WEAPON_None:
		{
			if (m_bMove[MOVE_Front] || m_bMove[MOVE_Back] || m_bMove[MOVE_Right] || m_bMove[MOVE_Left])
			{
				m_bDodgeBack = false;
				m_eAnim_Lower = Cmn_DodgeLight_F;
			}

			else
			{
				m_bDodgeBack = true;
				m_eAnim_Lower = Cmn_DodgeLight_B;
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

		m_eAnim_Upper = m_eAnim_Lower;
		m_eAnim_RightArm = m_eAnim_Lower;

		m_bOnDodge = true;
		m_bCanDodge = false;
		m_bStopMovementKeyInput = true;

		m_fSkillMoveSpeed_Cur = 6.f;
	}

	else
	{
		_v3 vDir = (m_bDodgeBack ? -m_pTransform->Get_Axis(AXIS_Z) : m_pTransform->Get_Axis(AXIS_Z));

		Decre_Skill_Movement(0.4f);
		Skill_Movement(m_fSkillMoveSpeed_Cur, vDir);

		if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.6f))
		{
			m_bDodgeBack = false;
			m_bAFK = false;
			m_bOnDodge = false;
			m_bCanDodge = true;
			m_bStopMovementKeyInput = false;

			if (m_bMove[MOVE_Front]|| m_bMove[MOVE_Back]|| m_bMove[MOVE_Right]|| m_bMove[MOVE_Left])
			{
				m_eActState = (m_bSprint ? ACT_Dash : ACT_Run);
			}

			else
				m_eActState = ACT_Idle;

			Reset_BattleState();
		}

		else if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.5f))
		{
			m_bDodgeBack = false;
			m_bCanDodge = true;
			m_bStopMovementKeyInput = false;
		}
	}
}

void CPlayer::Play_WeakAtk()
{
	if (false == m_bOnAttack)
	{
		m_bOnAttack = true;

		switch (m_eMainWpnState)
		{
		case WEAPON_None:
		{
			if (m_bSprint)
			{
				m_eAnim_Lower = Ssword_DownAtk_01;

				m_fSkillMoveSpeed_Cur = 13.f;
				m_fSkillMoveAccel_Cur = 0.f;
			}
			else
			{
				m_eAnim_Lower = Player_Anim(Ssword_WeakAtk_01 + (m_sWeakAtkCnt - 1));

				m_fSkillMoveSpeed_Cur = (m_sWeakAtkCnt == 1 ? 5.f : 6.5f);
				m_fSkillMoveAccel_Cur = 0.f;
			}
			break;
		}

		case WEAPON_Ssword:
		{
			m_eAnim_Lower = Ssword_WeakAtk_01;
			break;
		}

		case WEAPON_LSword:
		{
			m_eAnim_Lower = Cmn_Idle;
			break;
		}

		case WEAPON_Hammer:
		{
			m_eAnim_Lower = Cmn_Idle;
			break;
		}

		case WEAPON_Halverd:
		{
			m_eAnim_Lower = Cmn_Idle;
			break;
		}
		case WEAPON_Gun:
		{
			m_eAnim_Lower = Cmn_Idle;
			break;
		}
		}

		m_eAnim_Upper = m_eAnim_Lower;
		m_eAnim_RightArm = m_eAnim_Lower;
	}

	else if (true == m_bOnAttack)
	{
		switch (m_eAnim_Lower)
		{
		case Ssword_DownAtk_01:
		{
			if (m_pDynamicMesh->Get_TrackInfo().Position > 1.05f)
			{
				Decre_Skill_Movement(2.5f);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
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

void CPlayer::Play_HeavyAtk()
{
	if (false == m_bOnAttack)
	{
		m_bOnAttack = true;

		switch (m_eMainWpnState)
		{
		case WEAPON_None:
		{
			if (m_bSprint)
			{
				m_fSkillMoveMultiply = 1.f;

				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveSpeed_Cur = 7.f;

				m_eAnim_Lower = Ssword_SpecialLaunch;
			}
			else
			{
				m_fSkillMoveMultiply = 2.f;

				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveSpeed_Cur = 7.f;

				m_eAnim_Lower = Ssword_Charge;
			}
			break;
		}

		case WEAPON_Ssword:
		{
			m_eAnim_Lower = Ssword_WeakAtk_01;
			break;
		}

		case WEAPON_LSword:
		{
			m_eAnim_Lower = Cmn_Idle;
			break;
		}

		case WEAPON_Hammer:
		{
			m_eAnim_Lower = Cmn_Idle;
			break;
		}

		case WEAPON_Halverd:
		{
			m_eAnim_Lower = Cmn_Idle;
			break;
		}
		case WEAPON_Gun:
		{
			m_eAnim_Lower = Cmn_Idle;
			break;
		}
		}

		m_eAnim_Upper = m_eAnim_Lower;
		m_eAnim_RightArm = m_eAnim_Lower;
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
			if (m_pDynamicMesh->Get_TrackInfo().Position > 1.35f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}
			break;
		}

		case Ssword_Charge_Attack_01_A:
		{
			Decre_Skill_Movement(m_fSkillMoveMultiply);
			Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));

			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}
			break;
		}

		case Ssword_SpecialLaunch:
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

		default:
		{
			//Decre_Skill_Movement(m_fSkillMoveMultiply);
			//Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));

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

void CPlayer::Play_Skill()
{
}

void CPlayer::Play_BloodSucking()
{
}

void CPlayer::Play_Buff()
{
	if (false == m_bOnBuff)
	{
		m_bOnBuff = true;

		m_eAnim_Upper = Cmn_UseItem;
		m_eAnim_Lower = m_eAnim_Upper;
		m_eAnim_RightArm = m_eAnim_Upper;

		m_fAnimMutiply = 1.f;
	}

	else if (true == m_bOnBuff)
	{
		if (m_bMove[MOVE_Front] || m_bMove[MOVE_Back] || m_bMove[MOVE_Left] || m_bMove[MOVE_Right])
		{
			m_eAnim_Lower = Cmn_Walk_F;
		}

		else
			m_eAnim_Lower = Cmn_UseEnchant;

		if (m_pDynamicMesh->Is_Finish_Animation_Upper(0.95f))
		{
			m_bOnBuff = false;
			m_eActState = ACT_Idle;
		}
	}
}

void CPlayer::Play_Hit()
{
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
			m_eActiveSlot = (m_eActiveSlot == WPN_SLOT_A ? WPN_SLOT_B : WPN_SLOT_A);
				 
			m_eMainWpnState = m_pWeapon[m_eActiveSlot]->Get_WeaponType();

			m_bChangeWeapon = false;

			m_eActState = ACT_Idle;
		}
	}
}

void CPlayer::Play_Skills()
{
}

void CPlayer::Play_Buffs()
{
}

void CPlayer::Play_Enchants()
{
}

void CPlayer::Ready_Weapon()
{
	m_pWeapon[WPN_SLOT_A] = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pWeapon[WPN_SLOT_A]->Change_WeaponData(CWeapon::WPN_SSword_Normal);

	LPCSTR tmpChar = "RightHandAttach";
	_mat   matAttach;

	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo(tmpChar);

	m_pWeapon[WPN_SLOT_A]->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	m_pWeapon[WPN_SLOT_A]->Set_ParentMatrix(&m_pTransform->Get_WorldMat());


	m_pWeapon[WPN_SLOT_B] = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pWeapon[WPN_SLOT_B]->Change_WeaponData(CWeapon::WPN_Hammer_Normal);
	
	m_pWeapon[WPN_SLOT_B]->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	m_pWeapon[WPN_SLOT_B]->Set_ParentMatrix(&m_pTransform->Get_WorldMat());
}

void CPlayer::Skill_Movement(_float _fspeed, _v3 _vDir)
{
	_v3 tmpLook;
	_float fSpeed = _fspeed;

	tmpLook = _vDir;
	D3DXVec3Normalize(&tmpLook, &tmpLook);

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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_Player", L"Mesh_Dynamic", (CComponent**)&m_pDynamicMesh)))
		return E_FAIL;

	// for.Com_NavMesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"NavMesh", L"NavMesh", (CComponent**)&m_pNavMesh)))
		return E_FAIL;

	m_pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	m_pCollider->Set_Radius(_v3{ 0.4f, 0.5f, 0.4f});
	m_pCollider->Set_Dynamic(true);
	m_pCollider->Set_Type(COL_CAPSULE);
	m_pCollider->Set_CapsuleLength(1.8f);
	m_pCollider->Set_CenterPos(m_pTransform->Get_Pos() + _v3{0.f , m_pCollider->Get_Radius().y , 0.f});

	return NOERROR;
}

HRESULT CPlayer::SetUp_Default()
{
	ZeroMemory(&m_tInfo, sizeof(ACTOR_INFO));

	// Transform
	m_pTransform->Set_Pos(_v3(-0.487f , 0.f , 23.497f));
	m_pTransform->Set_Scale(V3_ONE);

	// Mesh
	m_pDynamicMesh->SetUp_Animation(Cmn_Run_F);

	// Info
	m_tInfo.fHP = 100.f;
	m_tInfo.fArmor = 0.f;
	m_tInfo.fStamina = 100.f;
	m_tInfo.fDamage = 0.f;
	m_tInfo.fMoveAccel_Cur = 0.f;
	m_tInfo.fMoveSpeed_Max = 1.5f;
	m_tInfo.fMoveAccel_Cur = 0.f;
	m_tInfo.fMoveSpeed_Max = 5.f;

	// Anim
	m_fAnimMutiply = 1.f;

	// Navi
	m_pNavMesh->Ready_NaviMesh(m_pGraphic_Dev, L"Navmesh_Test.dat");
	m_pNavMesh->Set_SubsetIndex(0);
	m_pNavMesh->Set_Index(14);

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

	return NOERROR;
}

void CPlayer::OnCollisionEnter()
{
	for (auto& iter : g_pManagement->Get_GameObjectList(L"Layer_Dummy", SCENE_STAGE))
	{
		CDummy_Target* pDummy = static_cast<CDummy_Target*>(iter);

		for (auto& iterCollider : pDummy->Get_ColliderVector())
		{
			if (m_pCollider->Check_Capsule(iterCollider))
			{
				cout << "부딪혀요" << endl;
			}
		}
	}
}

void CPlayer::Update_Collider()
{
	//cout << m_pCollider->Get_ColInfo()->vBegin.x << endl;

	m_pCollider->Update(m_pTransform->Get_WorldMat());
}

void CPlayer::Reset_BattleState()
{
	m_bOnAttack = false;
	m_bCanAttack = true;
	m_bCharging = false;

	m_sWeakAtkCnt = 0;
	m_sHeavyAtkCnt = 0;

	m_fChargeTimer_Cur = 0.f;

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
	LOOP(2)
	{
		Safe_Release(m_pWeapon[i]);
	}

	Safe_Release(m_pCollider);
	Safe_Release(m_pTransform);
	Safe_Release(m_pDynamicMesh);
	Safe_Release(m_pShader);	
	Safe_Release(m_pRenderer);
	Safe_Release(m_pNavMesh);

	CGameObject::Free();
}
