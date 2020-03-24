#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CWeapon;
class CPlayer : public CGameObject
{
public:
	enum MOVE_DIRECTION
	{
		MOVE_Front, MOVE_Back, MOVE_Left, MOVE_Right,
		MOVE_End
	};

	enum ACT_STATE
	{
		ACT_Idle, ACT_Walk, ACT_Run, ACT_Dash, ACT_Dodge, ACT_MoveDelay,
		ACT_WeakAtk, ACT_HeavyAtk, ACT_Skill, ACT_BloodSucking, ACT_Buff,
		ACT_WeaponChange,
		ACT_Hit, ACT_Jump, ACT_Fall, ACT_Down,
		ACT_PickUp, ACT_OpenDoor, ACT_Cinematic,
		ACT_STATE_END
	};

	enum ACTIVE_WEAPON_SLOT
	{
		WPN_SLOT_A, WPN_SLOT_B, WPN_SLOT_End
	};


protected:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	
private:
	ACTOR_INFO				m_tInfo				= {};
	ACT_STATE				m_eActState			= ACT_Idle;
	ACT_STATE				m_eActState_Upper	= ACT_Idle;
	WEAPON_STATE			m_eMainWpnState		= WEAPON_None;
	WEAPON_STATE			m_eSubWpnState	= WEAPON_None;
	ACTIVE_WEAPON_SLOT		m_eActiveSlot	= WPN_SLOT_A;

private:
	CWeapon*				m_pWeapon[WPN_SLOT_End] = {};

private:
	CTransform*				m_pTransform		= nullptr;
	CRenderer*				m_pRenderer			= nullptr;	
	CShader*				m_pShader			= nullptr;
	CMesh_Dynamic*			m_pDynamicMesh		= nullptr;
	CNavMesh*				m_pNavMesh			= nullptr;

private:
	vector<CCollider*>		m_vecPhsycColl;

private:
	_bool					m_bMove[MOVE_End]	= {};
	_bool					m_bSprint = false;
	_bool					m_bLockOn = false;
	_bool					m_bCanHit = false; 
	_bool					m_bCanAttack = true;
	_bool					m_bOnAttack = false;
	_bool					m_bOnAtk = false;
	_bool					m_bNoDmg = false;
	_bool					m_bAFK = false;
	_bool					m_bStopMovementKeyInput = false;
	_bool					m_bOnCombo = false;
	_bool					m_bOnDodge = false;
	_bool					m_bCanDodge = true;
	_bool					m_bDodgeBack = false;
	_bool					m_bCharging = false;
	_bool					m_bOnMoveDelay = false;
	_bool					m_bChangeWeapon = false;
	_bool					m_bOnBuff = false;
	_bool					m_bEventTrigger[32] = {};

private:
	_short					m_sHeavyAtkCnt = 0;
	_short					m_sWeakAtkCnt = 0;
	_short					m_sHeavyAtkCnt_Max = 10;
	_short					m_sWeakAtkCnt_Max = 4;

private:
	_float					m_fAngle_Recover = 0.f;

private:
	_float					m_fAnimMutiply = 1.f;

private:
	_float					m_fAtkCoolTimer_Cur = 0.f;
	_float					m_fAtkCoolTimer_Max = 0.f;
	_float					m_fChargeTimer_Cur = 0.f;
	_float					m_fChargeTimer_Max = 0.f;

private:
	_float					m_fSkillMoveSpeed_Cur = 0.f;
	_float					m_fSkillMoveSpeed_Max = 0.f;
	_float					m_fSkillMoveAccel_Cur = 0.5f;
	_float					m_fSkillMoveAccel_Max = 0.f;
	_float					m_fSkillMoveMultiply = 1.f;

private:
	_float					m_fAtkEndTime = 0.f;

private:
	HRESULT Add_Component();
	HRESULT SetUp_Default();
	HRESULT SetUp_ConstantTable();

private:
	virtual void Reset_BattleState();

private:
	virtual void Parameter_State();
	virtual void Parameter_State_Upper();
	virtual void Parameter_Atk();
	virtual void Parameter_Movement();
	virtual void Parameter_HeavyCharging();
	virtual void Parameter_YPos();

private:
	virtual void KeyInput();
	virtual void KeyDown();
	virtual void KeyUp();
	virtual void Key_Movement_Down();
	virtual void Key_Movement_Up();
	virtual void Key_ChangeWeapon();
	virtual void Key_Special();
	virtual void Key_Attack();
	virtual void Key_Skill();
	virtual void Key_Utility();

private:
	virtual void Play_Idle();
	virtual void Play_Run();
	virtual void Play_Dash();
	virtual void Play_MoveDelay();
	virtual void Play_Dodge();
	virtual void Play_WeakAtk();
	virtual void Play_HeavyAtk();
	virtual void Play_Skill();
	virtual void Play_BloodSucking();
	virtual void Play_Buff();
	virtual void Play_Hit();
	virtual void Play_WeaponChange();

private:
	virtual void Play_Skills();
	virtual void Play_Buffs();
	virtual void Play_Enchants();

private:
	virtual void Ready_Weapon();

private:
	void Skill_Movement(_float _fspeed, _v3 _vDir = { 0.f , 0.f , 0.f });
	void Decre_Skill_Movement(_float _fMutiply = 1.f);

private:
	virtual void Change_Weapon();

public:
	static	CPlayer* Create(_Device pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);

public:
	virtual void Free();

private:
	typedef enum Player_Anim
	{
		Cmn_Idle,
		Cmn_Idle_Look,
		Cmn_Walk_F,
		Cmn_Walk_FL_LockOn,
		Cmn_Walk_FL,
		Cmn_Walk_L,
		Cmn_Walk_R,
		Cmn_Walk_BR_LockOn,
		Cmn_Walk_BR,
		Cmn_Walk_B,
		Cmn_Run_F,
		Cmn_Run_F_End_L,
		Cmn_Run_F_End_R,
		Cmn_Run_FL_L,
		Cmn_Run_FL_L_End,
		Cmn_Run_FL_R,
		Cmn_Run_FL_R_End_L,
		Cmn_Run_FL_R_End_R,
		Cmn_Run_FR,
		Cmn_Run_FR_End_L,
		Cmn_Run_FR_End_R,
		Cmn_Run_L,
		Cmn_Run_L_End_L,
		Cmn_Run_L_End_R,
		Cmn_Run_R,
		Cmn_Run_R_End_L,
		Cmn_Run_R_End_R,
		Cmn_Run_B,
		Cmn_Run_B_End_L,
		Cmn_Run_B_End_R,
		Cmn_Run_BL,
		Cmn_Run_BL_End_R,
		Cmn_Run_BR_L,
		Cmn_Run_BR_L_End_L,
		Cmn_Run_BR_L_End_R,
		Cmn_Run_BR_R,
		Cmn_Run_BR_End_L,
		Cmn_UseItem,
		Cmn_UseEnchant,
		Cmn_UseInject,
		Cmn_UseThrow,
		Cmn_Stun,
		Cmn_Stun_End,
		Cmn_PickItem,
		Cmn_CheckPoint,
		Cmn_Hit01_F,
		Cmn_Hit01_FL,
		Cmn_Hit01_FR,
		Cmn_Hit01_L,
		Cmn_Hit01_R,
		Cmn_Hit01_B,
		Cmn_Hit01_BL,
		Cmn_Hit01_BR,
		Cmn_Hit_Ground,
		Cmn_Hit02_F,
		Cmn_Hit02_L,
		Cmn_Hit02_R,
		Cmn_Hit02_B,
		Cmn_Hit03_F,
		Cmn_Hit03_L,
		Cmn_Hit03_R,
		Cmn_Hit03_B,
		Cmn_Hit04_F,
		Cmn_Hit04_L,
		Cmn_Hit04_R,
		Cmn_Hit04_B,
		Cmn_HitStun,
		Cmn_HitBlow_F,
		Cmn_HitBlow_L,
		Cmn_HitBlow_R,
		Cmn_HitBlow_B,
		Cmn_Hit_RaiseUp,
		Cmn_Hit_RaiseDown,
		Cmn_Hit_Strike_F,
		Cmn_Hit_Strike_B,
		Cmn_Dash,
		Cmn_Dash_End_L,
		Cmn_Dash_End_R,
		Cmn_DeadIdle,
		Cmn_DodgeLight_F,
		Cmn_DodgeLight_FL,
		Cmn_DodgeLight_FR,
		Cmn_DodgeLight_L,
		Cmn_DodgeLight_R,
		Cmn_DodgeLight_B,
		Cmn_DodgeLight_BL,
		Cmn_DodgeLight_BR,
		Cmn_DoorOpen_Lift,
		Cmn_DoorOpen_Pull,
		Cmn_DoorOpen_Push,
		Cmn_DoorOpen_Pull_H,
		Cmn_DoorOpen_Push_H,
		Cmn_Down_01,
		Cmn_Down_01_End,
		Cmn_Down_02,
		Cmn_Down_02_End,
		Cmn_Down_Death_01,
		Cmn_Down_Death_02,
		Cmn_Down_WakeUp_01,
		Cmn_Down_WakeUp_02,
		Cmn_Drop_Item,
		Cmn_Dying_Loop,
		Cmn_Dying_End,
		Cmn_Dying_Live,
		Cmn_Fall,
		Cmn_Fall_End,
		Cmn_GameStart,
		Cmn_WeaponChange,
		Ssword_WeakAtk_01,
		Ssword_WeakAtk_02,
		Ssword_WeakAtk_03,
		Ssword_WeakAtk_04,
		Ssword_WeakAtk_05,
		Ssword_WeakAtk_06,
		Ssword_WeakAtk_07,
		Ssword_WeakAtk_08,
		Ssword_WeakAtk_09,
		Ssword_WeakAtk_10,
		Ssword_WeakAtk_11,
		Ssword_WeakAtk_12,
		Ssword_WeakAtk_13,
		Ssword_WeakAtk_14,
		Ssword_WeakAtk_15,
		Ssword_WeakAtk_16,
		Ssword_Atk_Repel,
		Ssword_DownAtk_01,
		Ssword_DownAtk_02,
		Ssword_DownAtk_03,
		Ssword_DownAtk_04,
		Ssword_SpecialLaunch,
		Ssword_Charge,
		Ssword_Charge_Attack_01_A,
		Ssword_Charge_Attack_01_B,
		Ssword_HeavyAtk_01_A,
		Ssword_HeavyAtk_01_B,
		Ssword_HeavyAtk_02_A,
		Ssword_HeavyAtk_04_B,
		Ssword_HeavyAtk_06_A,
		Ssword_HeavyAtk_06_B,
		Ssword_HeavyAtk_07_A,
		Ssword_HeavyAtk_07_B,
		Ssword_HeavyAtk_08_B,
		Ssword_HeavyAtk_09_A,
		Ssword_HeavyAtk_10_B,
		Ssword_HeavyAtk_12_B,
		Ssword_HeavyAtk_14_A,
		Ssword_HeavyAtk_14_B,
		Ssword_HeavyAtk_16_A,
		Ssword_Guard_Start,
		Ssword_Guard_Loop,
		Ssword_Guard_End,
		Ssword_Guard_Break,
		Ssword_Guard_Hit,
		Ssword_Attack_Charging,
		Halberd_Idle_Loop,
		LSowrd_Idle_Loop,
		Gun_Idle_Loop,
		Hammer_Idle_Loop,
		LSword_Run_Blend,
		LSword_Dash_Blend


/*
LSword_Dash_Blend,
LSword_Run_Blend,
Hammer_Idle_Loop,
Gun_Idle_Loop,
LSowrd_Idle_Loop,
Halberd_Idle_Loop,
Ssword_Attack_Charging,
Ssword_Guard_Hit,
Ssword_Guard_Break,
Ssword_Guard_End,
Ssword_Guard_Loop,
Ssword_Guard_Start,
Ssword_HeavyAtk_16_A,
Ssword_HeavyAtk_14_B,
Ssword_HeavyAtk_14_A,
Ssword_HeavyAtk_12_B,	// 차징 내려찍기
Ssword_HeavyAtk_10_B,	// 차징 올려베기
Ssword_HeavyAtk_09_A,	// 차징 강공격 2단 종베기
Ssword_HeavyAtk_08_B,	// 차징 강공격 1단 종베기
Ssword_HeavyAtk_07_B,	// 뒤로갔다가 앞베기
Ssword_HeavyAtk_07_A,	// 차징 앞베기
Ssword_HeavyAtk_06_B,	// 차징
Ssword_HeavyAtk_06_A,	// 제자리 3단 강공격
Ssword_HeavyAtk_04_B,	// 찌르고 강공격
Ssword_HeavyAtk_02_A,	// Shift 강공격
Ssword_HeavyAtk_01_B,	// 차지 강공격
Ssword_HeavyAtk_01_A,	// 강공격
Ssword_SpecialLaunch,
Ssword_DownAtk_04,
Ssword_DownAtk_03,
Ssword_DownAtk_02,
Ssword_DownAtk_01,
Ssword_Atk_Repel,
Ssword_WeakAtk_16,
Ssword_WeakAtk_15,
Ssword_WeakAtk_14,
Ssword_WeakAtk_13,
Ssword_WeakAtk_12,
Ssword_WeakAtk_11,
Ssword_WeakAtk_10,
Ssword_WeakAtk_09,
Ssword_WeakAtk_08,
Ssword_WeakAtk_07,
Ssword_WeakAtk_06,
Ssword_WeakAtk_05,
Ssword_WeakAtk_04,
Ssword_WeakAtk_03,
Ssword_WeakAtk_02,
Ssword_WeakAtk_01,
Cmn_WeaponChange,
Cmn_GameStart,
Cmn_Fall_End,
Cmn_Fall,
Cmn_Dying_Live,
Cmn_Dying_End,
Cmn_Dying_Loop,
Cmn_Drop_Item,
Cmn_Down_WakeUp_02,
Cmn_Down_WakeUp_01,
Cmn_Down_Death_02,
Cmn_Down_Death_01,
Cmn_Down_02_End,
Cmn_Down_02,
Cmn_Down_01_End,
Cmn_Down_01,
Cmn_DoorOpen_Push_H,
Cmn_DoorOpen_Pull_H,
Cmn_DoorOpen_Push,
Cmn_DoorOpen_Pull,
Cmn_DoorOpen_Lift,
Cmn_DodgeLight_BR,
Cmn_DodgeLight_BL,
Cmn_DodgeLight_B,
Cmn_DodgeLight_R,
Cmn_DodgeLight_L,
Cmn_DodgeLight_FR,
Cmn_DodgeLight_FL,
Cmn_DodgeLight_F,
Cmn_DeadIdle,
Cmn_Dash_End_R,
Cmn_Dash_End_L,
Cmn_Dash,
Cmn_Hit_Strike_B,
Cmn_Hit_Strike_F,
Cmn_Hit_RaiseDown,
Cmn_Hit_RaiseUp,
Cmn_HitBlow_B,
Cmn_HitBlow_R,
Cmn_HitBlow_L,
Cmn_HitBlow_F,
Cmn_HitStun,
Cmn_Hit04_B,
Cmn_Hit04_R,
Cmn_Hit04_L,
Cmn_Hit04_F,
Cmn_Hit03_B,
Cmn_Hit03_R,
Cmn_Hit03_L,
Cmn_Hit03_F,
Cmn_Hit02_B,
Cmn_Hit02_R,
Cmn_Hit02_L,
Cmn_Hit02_F,
Cmn_Hit_Ground,
Cmn_Hit01_BR,
Cmn_Hit01_BL,
Cmn_Hit01_B,
Cmn_Hit01_R,
Cmn_Hit01_L,
Cmn_Hit01_FR,
Cmn_Hit01_FL,
Cmn_Hit01_F,
Cmn_CheckPoint,
Cmn_PickItem,
Cmn_Stun_End,
Cmn_Stun,
Cmn_UseThrow,
Cmn_UseInject,
Cmn_UseEnchant,
Cmn_UseItem,
Cmn_Run_BR_End_L,
Cmn_Run_BR_R,
Cmn_Run_BR_L_End_R,
Cmn_Run_BR_L_End_L,
Cmn_Run_BR_L,
Cmn_Run_BL_End_R,
Cmn_Run_BL,
Cmn_Run_B_End_R,
Cmn_Run_B_End_L,
Cmn_Run_B,
Cmn_Run_R_End_R,
Cmn_Run_R_End_L,
Cmn_Run_R,
Cmn_Run_L_End_R,
Cmn_Run_L_End_L,
Cmn_Run_L,
Cmn_Run_FR_End_R,
Cmn_Run_FR_End_L,
Cmn_Run_FR,
Cmn_Run_FL_R_End_R,
Cmn_Run_FL_R_End_L,
Cmn_Run_FL_R,
Cmn_Run_FL_L_End,
Cmn_Run_FL_L,
Cmn_Run_F_End_R,
Cmn_Run_F_End_L,
Cmn_Run_F,
Cmn_Walk_B,
Cmn_Walk_BR,
Cmn_Walk_BR_LockOn,
Cmn_Walk_R,
Cmn_Walk_L,
Cmn_Walk_FL,
Cmn_Walk_FL_LockOn,
Cmn_Walk_F,
Cmn_Idle_Look,
Cmn_Idle
*/

	}P_ANI;

private:
	P_ANI	m_eAnim_Upper = Cmn_Idle;
	P_ANI	m_eAnim_Lower = Cmn_Idle;
	P_ANI	m_eAnim_RightArm = Cmn_Idle;
};

END