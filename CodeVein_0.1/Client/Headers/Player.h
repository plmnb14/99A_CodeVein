#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"
#include "CameraMgr.h"
#include "ScriptManager.h"

#include "UI_Manager.h"

#include "Costume_Body.h"
#include "Costume_Mask.h"
#include "Costume_Head.h"

#include "OrthoEffect.h"

#include "HitCheckUI.h"
#include "FadeCornerUI.h"

BEGIN(Client)

class CLockOn_UI;
class CCostume_Outer;
class CCostume_Hair;
class CStageAgent;
class CDrain_Weapon;
class CWeapon;
class CPlayer : public CGameObject
{
public:
	typedef struct tagMoveOptionReset
	{
		tagMoveOptionReset(SCENEID _eSceneID, Teleport_ID _eTeleportID)
			: eSceneID(_eSceneID), eTeleportID(_eTeleportID)
		{}

		Teleport_ID eTeleportID = TeleportID_End;
		SCENEID eSceneID = SCENE_END;

	}TELEPORT_RESET;
	
public:
	typedef enum tagPlayerBodyType
	{
		Player_Body_01, Player_Body_02, Player_Body_03, Player_Body_04,
		Player_Body_End
	}PLAYER_BODY;

public:
	enum DODGE_TYPE
	{
		DODGE_light, DODGE_Roll, DODGE_Heavy
	};

	enum MOVE_DIRECTION
	{
		MOVE_Front, MOVE_Back, MOVE_Left, MOVE_Right,
		MOVE_End
	};

	enum DRAIN_STATE
	{
		DRAIN_TAIL, DRAIN_DRAPE, DRAIN_IVY, DRAIN_END
	};

	enum ACT_STATE
	{
		ACT_Idle, ACT_Walk, ACT_Run, ACT_Dash, ACT_Dodge, ACT_MoveDelay,
		ACT_WeakAtk, ACT_HeavyAtk, ACT_Skill, ACT_Buff,
		ACT_BloodSuck_Count, ACT_BloodSuck, ACT_BloodSuck_Combo,
		ACT_BloodSuck_Execution,
		ACT_WeaponChange,
		ACT_Hit, ACT_Jump, ACT_Fall, ACT_Down,
		ACT_PickUp, ACT_OpenDoor, ACT_Cinematic,
		ACT_Mistoletoe,
		ACT_Summon, ACT_Disappear,
		ACT_Dead,
		ACT_STATE_END
	};

	enum ACTIVE_WEAPON_SLOT
	{
		WPN_SLOT_A, WPN_SLOT_B, /*WPN_SLOT_C, WPN_SLOT_D, WPN_SLOT_E,*/ WPN_SLOT_End
	};

private:
	enum BoneMatrix
	{
		Bone_Range, Bone_Body, Bone_Head, Bone_End
	};


protected:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	CCostume_Hair* Get_Costume_Hair() { return m_pHair; }
	CCostume_Head* Get_Costume_Head() { return m_pHead[m_eHeadType]; }
	CCostume_Mask* Get_Costume_Mask() { return m_pMask[m_eMaskType]; }

public:
	void Set_PlayerBody(PLAYER_BODY _eBodyType) { Change_PlayerBody(_eBodyType); }

public:
	void Set_WeaponSlot(ACTIVE_WEAPON_SLOT eType, WEAPON_DATA eData);
	void Set_ArmorSlot(ARMOR_All_DATA eType);
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);
	virtual HRESULT Render_GameObject_SetPass(CShader* pShader, _int iPass, _bool _bIsForMotionBlur = false);

public:
	virtual void Reset_OldAnimations();

public:
	virtual void Teleport_ResetOptions(_int _eSceneID, _int _eTeleportID);
private:
	ACTOR_INFO				m_tInfo = {};
	ACT_STATE				m_eActState = ACT_Summon;
	WEAPON_STATE			m_eMainWpnState = WEAPON_SSword;
	WEAPON_STATE			m_eSubWpnState = WEAPON_SSword;
	DRAIN_STATE				m_eDrainState = DRAIN_END;
	ACTIVE_WEAPON_SLOT		m_eActiveSlot = WPN_SLOT_A;

private:
	CStageAgent*			m_pStageAgent = nullptr;

private:
	CWeapon*				m_pWeapon[WPN_SLOT_End] = {};
	CDrain_Weapon*			m_pDrainWeapon = nullptr;
	CGameObject*			m_pCunterTarget = nullptr;

private:
	CLockOn_UI*				m_pLockOn_UI = nullptr;

private:
	CTransform*				m_pTransform = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CShader*				m_pShader = nullptr;
	CMesh_Dynamic*			m_pDynamicMesh = nullptr;
	CNavMesh*				m_pNavMesh = nullptr;
	CCollider*				m_pCollider = nullptr;
	CBattleAgent*			m_pBattleAgent = nullptr;
	CRigidBody*				m_pRigid = nullptr;

private:
	CCostume_Hair*			m_pHair = nullptr;
	CCostume_Outer*			m_pOuter = nullptr;
	CCostume_Mask*			m_pMask[CCostume_Mask::Mask_End] = {};
	CCostume_Head*			m_pHead[CCostume_Head::Chara_Head_End] = {};		// 머리 미리 준비
	CCostume_Body*			m_pBody[CCostume_Body::Chara_Body_End] = {};

private:
	CCostume_Mask::MASK_TYPE	m_eMaskType = CCostume_Mask::Mask_11;
	CCostume_Head::CHAR_HEAD	m_eHeadType = CCostume_Head::Chara_Head_02;
	CCostume_Body::CHAR_BODY	m_eBodyType = CCostume_Body::Chara_Body_01;

private:
	CUI_Manager*			m_pUIManager = nullptr;
	CCameraMgr*				m_pCamManager = nullptr;
	CScriptManager*			m_pScriptManager = nullptr;

private:
	COrthoEffect*			m_pScreenBloodEffect = nullptr;
	COrthoEffect*			m_pScreenFadeEffect = nullptr;

	CHitCheckUI*			m_pScreenCornerEffect = nullptr;
	CFadeCornerUI*			m_pScreenCornerFade = nullptr;

private:
	vector<CCollider*>		m_vecPhsycColl;

private:
	vector<SKILL_INFO*>		m_vecFullSkillInfo;
	vector<SKILL_INFO*>		m_vecActiveSkillInfo;

private:
	_mat*					m_matBones[Bone_End];
	_mat*					m_matHandBone = nullptr;
	_mat*					m_matTailBone = nullptr;

private:
	_bool					m_bMove[MOVE_End] = {};
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
	_bool					m_bOnAiming = false;
	_bool					m_bHaveAimingTarget = false;
	_bool					m_bEventTrigger[32] = {};
	_bool					m_bOneHand = true;
	_bool					m_bOnSkill = false;
	_bool					m_bOnBloodSuck = false;
	_bool					m_bReadyChargeSuck = false;
	_bool					m_bOnChargeSuck = false;
	_bool					m_bOnPickUp = false;
	_bool					m_bCanSprintAtk = false;
	_bool					m_bActiveUI = false;		// 현재 UI 활성 중
	_bool					m_bCanSummon = true;		// 재소환 가능한지
	_bool					m_bCanDisappear = true;		// 사라질 수 있는지
	_bool					m_bOnSummon = false;		// 재소환
	_bool					m_bOnDisappear = false;		// 사라짐
	_bool					m_bOnDown = false;			// 다운 됬냐
	_bool					m_bOffLerp = false;			// 애니 보간 끄기
	_bool					m_bCanExecution = true;	// 처형 가능
	_bool					m_bIsExecution = false;		// 처형 중
	_bool					m_bOnUI_Mistletoe = false;
	_bool					m_bOnUI_Inventory = false;
	_bool					m_bOnUI_BloodCode = false;
	_bool					m_bOnUI_StageSelect = false;
	_bool					m_bOnUI_NPCTalk = false;
	_bool					m_bCanPickUp = false;	// 아이템 줍기
	_bool					m_bCanDialouge = false; // 대화
	_bool					m_bCanInterAct = false;	// 상호작용

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
	_float					m_fCurMoveSpeed = 0.f;
	_v3						m_vMoveDir = {};

private:
	_float					m_fAtkEndTime = 0.f;

private:
	_float					m_fAmingRange = 20.f;

private: // For Dissolve
	_float					m_fDissolveY = 0.f;

private:
	_bool				m_arrbActiveSkillOn[8] = {};
	SKILL_INFO			m_arrSkillInfo[8] = {};

private:
	_bool				m_bWeaponActive[2] = {};

private:
	HRESULT Add_Component();
	HRESULT SetUp_Default();
	HRESULT SetUp_ConstantTable(CShader* pShader);

private:
	virtual void Movement_Aiming(_float _fAngle, _float _fMoveSpeed);
	virtual void Movement_NonAiming(_float _fRecover, _float _fAngle, _float _fRadian, _float _fMoveSpeed);

private:
	virtual void Target_AimChasing();

private:
	virtual void OnCollisionEnter();

private:
	virtual void Update_Collider();

private:
	virtual void Reset_BattleState();

private:
	virtual void Check_Mistletoe();
	virtual _int Check_HitDirection();
	virtual _bool Check_CunterAngle(CGameObject* pObj);
	virtual _bool Check_CunterTarget();

private:
	virtual void Change_PlayerBody(PLAYER_BODY _eBodyType);
	virtual void Update_OuterAnim();

private:
	virtual void Parameter_State();
	virtual void Parameter_State_Upper();
	virtual void Parameter_Atk();
	virtual void Parameter_Movement();
	virtual void Parameter_HeavyCharging();
	virtual void Parameter_YPos(_double dTimeDelta);
	virtual void Parameter_Collision();
	virtual void Parameter_Aiming();
	virtual void Parameter_HitCheck();
	virtual void Parameter_CheckActiveSkill();
	virtual void Parameter_CheckActiveWeapon();

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
	virtual void Key_InterAct();
	virtual void Key_BloodSuck();
	virtual void Key_UI_n_Utiliy(_bool _bActiveUI);

private:
	virtual void Play_Idle();
	virtual void Play_Run();
	virtual void Play_Dash();
	virtual void Play_MoveDelay();
	virtual void Play_Dodge();
	virtual void Play_WeakAtk();
	virtual void Play_HeavyAtk();
	virtual void Play_Buff();
	virtual void Play_Hit();
	virtual void Play_Dead();
	virtual void Play_WeaponChange();
	virtual void Play_Spawn();
	virtual void Play_PickUp();
	virtual void Play_Down();

private:
	virtual void Play_Summon();
	virtual void Play_Disappear();

private:
	virtual void Play_BloodSuck();
	virtual void Play_BloodSuckCount();
	virtual void Play_BloodSuckCombo();
	virtual void Play_BloodSuckExecution();

private:
	virtual void Play_Skills();
	virtual void Play_Buffs();
	virtual void Play_Enchants();

private:
	virtual void Play_OneHandSkill_01();
	virtual void Play_OneHandSkill_02();
	virtual void Play_OneHandskill_03();

private:
	virtual void Play_TwoHandSkill_01();
	virtual void Play_TwoHandSkill_02();
	virtual void Play_TwoHandSkill_03();

private:
	virtual void Play_GunSkill();

private:
	virtual void Play_Ssword_WeakAtk();
	virtual void Play_Ssword_HeavyAtk();
	virtual void Play_Ssword_DownAtk();
	virtual void Play_Ssword_DashAtk();

private:
	virtual void Play_Gun_WeakAtk();
	virtual void Play_Gun_HeavyAtk();
	virtual void Play_Gun_DownAtk();
	virtual void Play_Gun_DashAtk();

private:
	virtual void Play_Halverd_WeakAtk();
	virtual void Play_Halverd_HeavyAtk();
	virtual void Play_Halverd_DownAtk();
	virtual void Play_Halverd_DashAtk();

private:
	virtual void Play_Hammer_WeakAtk();
	virtual void Play_Hammer_HeavyAtk();
	virtual void Play_Hammer_DownAtk();
	virtual void Play_Hammer_DashAtk();

private:
	virtual void Play_Lsword_WeakAtk();
	virtual void Play_Lsword_HeavyAtk();
	virtual void Play_Lsword_DownAtk();
	virtual void Play_Lsword_DashAtk();

private:
	virtual void Ready_Bodys();
	virtual void Ready_Heads();
	virtual void Ready_Masks();
	virtual void Ready_Weapon();
	virtual void Ready_DrainWeapon();
	virtual void Ready_BoneMatrix();
	virtual void Ready_Collider();
	virtual void Ready_Skills();
	virtual void Ready_Rigid();
	virtual void Temp_ActiveSkills();

private:
	virtual void Reset_BloodSuck_Options();
	virtual void Reset_Attack_Bool();

private:
	void Skill_Movement(_float _fspeed, _v3 _vDir = { 0.f , 0.f , 0.f });
	void Decre_Skill_Movement(_float _fMutiply = 1.f);

private:
	virtual void Draw_Collider();

private:
	virtual void Change_Weapon();

private:
	virtual void Reset_All();

public:
	virtual void Active_UI_Mistletoe(_bool _bResetUI = false);		// 겨우살이
	virtual void Active_UI_Inventory(_bool _bResetUI = false);		// 인벤토리
	virtual void Active_UI_StageSelect(_bool _bResetUI = false);	// 스테이지 선택
	virtual void Active_UI_NPC(_bool _bResetUI = false);			// NPC 와의 대화
	virtual void Active_UI_BloodCode(_bool _bResetUI = false);			// NPC 와의 대화

public:
	virtual void Active_UI_LockOn(_bool _bResetUI = false);

public:
	static	CPlayer* Create(_Device pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);

public:
	virtual void Free();

private:
	/*
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
		Halverd_Idle_Loop,
		Halverd_Dash_Blend,
		Halverd_Run_Blend,
		Halverd_Walk_Blend,
		Halverd_Dodge_F,
		Halverd_Dodge_LF,
		Halverd_Dodge_RF,
		Halverd_Dodge_L,
		Halverd_Dodge_R,
		Halverd_Dodge_B,
		Halverd_Dodge_LB,
		Halverd_Dodge_RB,
		Halverd_WeakAtk_01,
		Halverd_WeakAtk_02,
		Halverd_WeakAtk_03,
		Halverd_WeakAtk_04,
		Halverd_WeakAtk_05,
		Halverd_WeakAtk_06,
		Halverd_WeakAtk_07,
		Halverd_WeakAtk_08,
		//Halverd_Charge,
		//Halverd_Charge_Attack_01_A,
		//Halverd_Charge_Attack_01_B,
		Halverd_HeavyAtk_01_A,
		Halverd_HeavyAtk_01_B,
		Halverd_HeavyAtk_03_A,
		Halverd_HeavyAtk_04_A,
		Halverd_HeavyAtk_05_A,
		Halverd_HeavyAtk_06_A,
		Halverd_HeavyAtk_07_A,
		Halverd_HeavyAtk_08_A,
		Halverd_HeavyAtk_09_A,
		Halverd_HeavyAtk_10_A,
		Halverd_HeavyAtk_11_A,
		Halverd_HeavyAtk_12_A,
		Halverd_HeavyAtk_13_A,
		Halverd_HeavyAtk_14_A,
		Halverd_HeavyAtk_16_A,
		Halverd_Guard_Start,
		Halverd_Guard_Loop,
		Halverd_Guard_End,
		Halverd_Guard_Hit,
		Halverd_Guard_Break,
		Halverd_SpecialDown_01,
		Halverd_SpecialDown_02,
		Halverd_SpecialDown_03,
		Halverd_SpecialLaunch,
		Halverd_Skill_Heavy_01,
		Halverd_Fall_Start,
		Halverd_Fall_End,
		Halverd_Atk_Dodge_F,
		Halverd_Atk_Dodge_B,
		Halverd_Atk_Dodge_R,
		Hammer_Idle_Loop,
		Hammer_Blend_Walk,
		Hammer_Blend_Run,
		Hammer_Blend_Dash,
		Hammer_Atk_Dodge_F,
		Hammer_Atk_Dodge_B,
		Hammer_Atk_Dodge_R,
		Hammer_Fall_Start,
		Hammer_Fall_End,
		Hammer_Dodge_F,
		Hammer_Dodge_FL,
		Hammer_Dodge_FR,
		Hammer_Dodge_L,
		Hammer_Dodge_R,
		Hammer_Dodge_B,
		Hammer_Dodge_BL,
		Hammer_Dodge_BR,
		Hammer_WeakAtk_01,
		Hammer_WeakAtk_02,
		Hammer_WeakAtk_04,
		Hammer_WeakAtk_05,
		Hammer_WeakAtk_06,
		Hammer_WeakAtk_07,
		//Hammer_Charge,
		//Hammer_Charge_Attack_01_A,
		//Hammer_Charge_Attack_01_B,
		Hammer_HeavyAtk_01_A,
		Hammer_HeavyAtk_01_B,
		Hammer_HeavyAtk_03_A,
		Hammer_HeavyAtk_04_B,
		Hammer_HeavyAtk_05_A,
		Hammer_HeavyAtk_06_B,
		Hammer_HeavyAtk_08_A,
		Hammer_HeavyAtk_08_B,
		Hammer_HeavyAtk_10_A,
		Hammer_HeavyAtk_10_B,
		Hammer_HeavyAtk_12_B,
		Hammer_HeavyAtk_14_B,
		Hammer_Guard_Start,
		Hammer_Guard_Loop,
		Hammer_Guard_End,
		Hammer_Guard_Hit,
		Hammer_Guard_Break,
		Gun_Idle_Loop,
		Gun_Blend_Walk,
		Gun_Blend_Run,
		Gun_Blend_Dash,
		Gun_DodgeAtk_F,
		Gun_DodgeAtk_B,
		Gun_DodgeAtk_R,
		Gun_DodgeRoll_F,
		Gun_DodgeRoll_FL,
		Gun_DodgeRoll_FR,
		Gun_DodgeRoll_L,
		Gun_DodgeRoll_R,
		Gun_DodgeRoll_B,
		Gun_DodgeRoll_BL,
		Gun_DodgeRoll_BR,
		Gun_Fall_Start,
		Gun_Fall_End,
		Gun_WeakAtk_01,
		Gun_WeakAtk_02,
		Gun_WeakAtk_03,
		Gun_WeakAtk_04,
		Gun_WeakAtk_05,
		Gun_WeakAtk_06,
		Gun_WeakAtk_07,
		Gun_WeakAtk_08,
		Gun_WeakAtk_09,
		Gun_WeakAtk_10,
		Gun_Charge,
		Gun_Charge_Attack_01_A,
		Gun_Charge_Attack_01_B,
		Gun_HeavyAtk_01_A,
		Gun_HeavyAtk_01_B,
		Gun_HeavyAtk_02_B,
		Gun_HeavyAtk_03_A,
		Gun_HeavyAtk_04_B,
		Gun_HeavyAtk_05_B,
		Gun_HeavyAtk_06_A,
		Gun_HeavyAtk_06_B,
		Gun_HeavyAtk_07_A,
		Gun_HeavyAtk_08_A,
		Gun_Guard_Start,
		Gun_Guard_Loop,
		Gun_Guard_End,
		Gun_Guard_Hit,
		Gun_Guard_Break,
		Lsword_Idle_Loop,
		Lsword_Blend_Walk,
		Lsword_Blend_Run,
		Lsword_Blend_Dash,
		Lsword_Dodge_Atk_F,
		Lsword_Dodge_Atk_B,
		Lsword_Dodge_Atk_R,
		Lsword_Dodge_F,
		Lsword_Dodge_FL,
		Lsword_Dodge_FR,
		Lsword_Dodge_L,
		Lsword_Dodge_R,
		Lsword_Dodge_B,
		Lsword_Dodge_BL,
		Lsword_Dodge_BR,  //
		Lsword_WeakAtk_01,
		Lsword_WeakAtk_02,
		Lsword_WeakAtk_03,
		Lsword_WeakAtk_04,
		Lsword_WeakAtk_05,
		Lsword_WeakAtk_07,
		Lsword_WeakAtk_08,
		Lsword_WeakAtk_09,
		Lsword_DownAtk_01,
		Lsword_DownAtk_02,
		Lsword_DownAtk_03,
		Lsword_DownAtk_Launch,
		Lsword_Charge,
		Lsword_Charge_Attack_01_A,
		Lsword_Charge_Attack_01_B,
		Lsword_HeavyAtk_01_A,
		Lsword_HeavyAtk_01_B,
		Lsword_HeavyAtk_03_A,
		Lsword_HeavyAtk_04_B,
		Lsword_HeavyAtk_05_A,
		Lsword_HeavyAtk_06_B,
		Lsword_HeavyAtk_07_A,
		Lsword_HeavyAtk_08_B,
		Lsword_HeavyAtk_09_A,
		Lsword_HeavyAtk_10_B,
		Lsword_HeavyAtk_11_A,
		Lsword_HeavyAtk_12_B,
		Lsword_HeavyAtk_13_B,
		Lsword_HeavyAtk_14_B,
		Lsword_HeavyAtk_16_B,
		Lsword_HeavyAtk_18_B,
		Lsword_Guard_Start,
		Lsword_Guard_Loop,
		Lsword_Guard_End,
		Lsword_Guard_Hit,
		Lsword_Guard_Break,
		Renketsu_PowerfulBuff,
		Renketsu_Buff,
		Renketsu_Blade,
		Renketsu_StrongAtk_01,
		Renketsu_StrongAtk_02,	// 검 날리고 휘두르기 (대검)
		Renketsu_StrongAtk_03,	// 검 날리고 휘두르기 (대검)
		Renketsu_StrongAtk_04,	// 2타 후 공중 내려찍기
		Renketsu_StrongAtk_05,	// 차지 크러쉬 (잠룡복호)
		Renketsu_StrongAtk_06,	// 발로 내려치기
		Renketsu_StrongAtk_07,	// 제비 떨어뜨리기? (대검)
		Renketsu_StrongAtk_09,  // 제비 떨어뜨리기? (대검)
		Renketsu_StrongAtk_10,	// 낙엽 떨어뜨리기
		Renketsu_StrongAtk_11,  // 회전 2회
		Renketsu_StrongAtk_12,	// 회전 1회
		Renketsu_Wave,
		Renketsu_Throw,
		Renketsu_Search,
		Renketsu_Mid_Shoot,
		Renketsu_Max_Shoot,
		Renketsu_High_Shoot,
		Renketsu_GroundUpper,
		Renketsu_Ground,
		Renketsu_Enchant,
		Renketsu_DodgeShot,
		Renketsu_Create_01,
		Renketsu_Create_02,
		Renketsu_Barrier,
		Renketsu_AllRangeShoot,
		Renketsu_AimShoot_00,
		Renketsu_AimShoot_01,
		Cmn_BackStab,
		Cmc_CheckPoint_Start,
		Cmc_CheckPoint_Loop,
		Cmc_CheckPoint_End,
		Cmn_BoxOpen,
		LongCoat_Parry,
		LongCoat_ChargeSuck_Start,
		LongCoat_ChargeSuck_End,
		LongCoat_ComboSuck,
		LongCoat_GroundSuck,
		LongCoat_SpecialSuck_01,
		LongCoat_SpecialSuck_02,
		Drape_Parry,
		Drape_ChargeSDuck_Start,
		Drape_ChargeSDuck_End,
		Drape_ComboSuck,
		Drape_Ground,
		Drape_SpecialSuck_01,
		Cmn_Renketsu_Dodge_F,
		Cmn_Renketsu_Dodge_FL,
		Cmn_Renketsu_Dodge_FR,
		Cmn_Renketsu_Dodge_L,
		Cmn_Renketsu_Dodge_R,
		Cmn_Renketsu_Dodge_B,
		Cmn_Renketsu_Dodge_BL,
		Cmn_Renketsu_Dodge_BR,
		Cmn_Renketsu_Dodge_Start,
		Cmn_RealLightDodge_F,
		Cmn_RealLightDodge_FL,
		Cmn_RealLightDodge_FR,
		Cmn_RealLightDodge_L,
		Cmn_RealLightDodge_R,
		Cmn_RealLightDodge_B,
		Cmn_RealLightDodge_BL,
		Cmn_RealLightDodge_BR,
		Hammer_Launch,
		Hammer_Down_01,
		Hammer_Down_02

	}P_ANI;
	*/

	typedef enum tagPlayerNewAni
	{
		Cmn_Active_Mistletoe,
		Cmn_PickItemStand,
		Cmn_UseItem,
		Cmn_CheckPoint_Start,
		Cmn_CheckPoint_Loop,
		Cmn_CheckPoint_End,
		Cmn_UseItemEnchant,
		Cmn_UseItemInjection,
		Cmn_UseItemRecovery,
		Cmn_UseItemThrow,
		Cmn_WeaponChange,
		Cmn_BoxOpen,
		Cmn_Idle,
		Cmn_Walk_F,
		Cmn_Walk_FL,
		Cmn_Walk_FR,
		Cmn_Walk_L,
		Cmn_Walk_R,
		Cmn_Walk_B,
		Cmn_Walk_BL,
		Cmn_Walk_BR,
		Cmn_Run_F,
		Cmn_Run_FL,
		Cmn_Run_FR,
		Cmn_Run_L,
		Cmn_Run_R,
		Cmn_Run_B,
		Cmn_Run_BL,
		Cmn_Run_BR,
		Cmn_Damage_01_F,
		Cmn_Damage_01_FL,
		Cmn_Damage_01_FR,
		Cmn_Damage_01_L,
		Cmn_Damage_01_R,
		Cmn_Damage_01_B,
		Cmn_Damage_01_BL,
		Cmn_Damage_01_BR,
		Cmn_Damage_02_F,
		Cmn_Damage_02_L,
		Cmn_Damage_02_R,
		Cmn_Damage_02_B,
		Cmn_Damage_03_F,
		Cmn_Damage_03_L,
		Cmn_Damage_03_R,
		Cmn_Damage_03_B,
		Cmn_Damage_04_F,
		Cmn_Damage_04_L,
		Cmn_Damage_04_R,
		Cmn_Damage_04_B,
		Cmn_Damage_Blow_F,
		Cmn_Damage_Blow_B,
		Cmn_Dash,
		Cmn_Dash_End,
		Cmn_Down_F_Loop,
		Cmn_Down_F_End,
		Cmn_Down_F_Death,
		Cmn_Down_F_Alive,
		Cmn_Down_B_Loop,
		Cmn_Down_B_End,
		Cmn_Down_B_Death,
		Cmn_Down_B_Alive,
		Cmn_Dying_Loop,
		Cmn_Dying_Alive,
		Cmn_Dying_Death,
		Cmn_NoBuddy_Death,
		Cmn_Damage_Blow_L,
		Cmn_Damage_Blow_R,
		Cmn_Fall_Loop,
		Cmn_Fall_End,
		Cmn_Run_F_End_L,
		Cmn_Run_FL_End,
		Cmn_Run_FR_End,
		Cmn_Run_L_LEnd,
		Cmn_Run_R_End,
		Cmn_Run_B_End,
		Cmn_Run_BL_End,
		Cmn_Run_BR_End,
		Ssword_BlendWalk,
		Ssword_BlendRun,
		Ssword_BlendDash,
		Ssword_WeakAtk_01,
		Ssword_WeakAtk_02,
		Ssword_WeakAtk_03,
		Ssword_WeakAtk_04,
		Ssword_WeakAtk_05,
		Ssword_Down_02,
		Ssword_Down_01,
		Ssword_Charge,
		Ssword_HeavyAtk_01,
		Ssword_HeavyAtk_02,
		Ssword_DodgeAtk_F,
		Ssword_DodgeAtk_B,
		Lsword_BlendWalk,
		Lsword_BlendRun,
		Lsword_BlendDash,
		LSword_BlendIdle,
		LSword_WeakAtk_01,
		LSword_WeakAtk_02,
		LSword_WeakAtk_03,
		LSword_WeakAtk_04,
		LSword_WeakAtk_05,
		Lsword_Down,
		Lsword_SpecialLaunch,
		Lsword_Charge,
		Lsword_HeavyAtk_01,
		Lsword_HeavyAtk_02,
		Lsword_DodgeAtk_F,
		Lsword_DodgeAtk_B,
		Hammer_BlendWalk,
		Hammer_BlendRun,
		Hammer_BlendDash,
		Hammer_BlendIdle,
		Hammer_WeakAtk_01,
		Hammer_WeakAtk_02,
		Hammer_WeakAtk_04,
		Hammer_WeakAtk_07,
		Hammer_WeakAtk_Down_01,
		Hammer_WeakAtk_Down_02,
		Hammer_Charge,
		Hammer_HeavyAtk_01,
		Hammer_HeavyAtk_02,
		Hammer_DodgeAtk_F,
		Hammer_DodgeAtk_B,
		Halberd_BlendWalk,
		Halberd_BlendRun,
		Halberd_BlendDash,
		Halberd_BlendIdle,
		Halberd_WeakAtk_03,
		Halberd_WeakAtk_06,
		Halberd_WeakAtk_07,
		Halberd_WeakAtk_08,
		Halberd_SpecialLaunch,
		Halberd_DownAtk_03,
		Halberd_DownAtk_02,
		Halberd_Charge,
		Halberd_HeavyAtk_06,
		Halberd_HeavyAtk_07,
		Halberd_DodgeAtk_F,
		Halberd_DodgeAtk_B,
		Gun_BlendWalk,
		Gun_BlendRun,
		Gun_BlendDash,
		Gun_BlendIdle,
		Gun_WeakAtk_01,
		Gun_WeakAtk_02,
		Gun_WeakAtk_03,
		Gun_WeakAtk_04,
		Gun_WeakAtk_05,
		Gun_WeakAtk_10,
		Gun_Charge,
		Gun_HeavyAtk_01,
		Gun_HeavyAtk_02,
		Gun_DodgeAtk_F,
		Gun_DodgeAtk_B,
		Gun_Dodge_F,
		Gun_Dodge_FL,
		Gun_Dodge_FR,
		Gun_Dodge_L,
		Gun_Dodge_R,
		Gun_Dodge_B,
		Gun_Dodge_BL,
		Gun_Dodge_BR,
		Ssword_Dodge_F,
		Ssword_Dodge_FL,
		Ssword_Dodge_FR,
		Ssword_Dodge_L,
		Ssword_Dodge_R,
		Ssword_Dodge_B,
		Ssword_Dodge_BL,
		Ssword_Dodge_BR,
		Lsword_Dodge_F,
		Lsword_Dodge_FL,
		Lsword_Dodge_FR,
		Lsword_Dodge_L,
		Lsword_Dodge_R,
		Lsword_Dodge_B,
		Lsword_Dodge_BL,
		Lsword_Dodge_BR,
		Halberd_Dodge_F,
		Halberd_Dodge_FL,
		Halberd_Dodge_FR,
		Halberd_Dodge_L,
		Halberd_Dodge_R,
		Halberd_Dodge_B,
		Halberd_Dodge_BL,
		Halberd_Dodge_BR,
		Hammer_Dodge_F,
		Hammer_Dodge_FL,
		Hammer_Dodge_FR,
		Hammer_Dodge_L,
		Hammer_Dodge_R,
		Hammer_Dodge_B,
		Hammer_Dodge_BL,
		Hammer_Dodge_BR,
		Renketsu_01,
		Renketsu_02,
		Renketsu_03,
		Renketsu_04,
		Renketsu_05,
		Renketsu_06,
		Renketsu_07,
		Renketsu_09,
		Renketsu_10,
		Renketsu_11,
		Renketsu_12,
		Renketsu_Spear,
		Renketsu_Gun,
		Renketsu_Buff,
		Renketsu_Wave,
		Renketsu_Search,
		Renketsu_Enchant,
		Renketsu_Throw,
		Renketsu_AimShot,
		LongCoat_Parry,
		LongCoat_Charge_Start,
		LongCoat_Charge_End,
		LongCoat_Combo,
		LongCoat_GroundCombo,
		LongCoat_Exicution_Cinema,
		LongCoat_Exicution,
		Guantlet_Parry,
		Gauntlet_Charge_Start,
		Gauntlet_Charge_End,
		Gauntlet_Combo,
		Gauntlet_GroundCombo,
		Drape_Parry,
		Drape_Charge_Start,
		Drape_Charge_End,
		Drape_GroundCombo,
		Drape_Exicution_Cinema,
		Muffler_Parry,
		Muffler_Charge_Start,
		Muffler_Charge_End,
		Muffler_Combo,
		Muffler_GroundCombo,
		Gun_Flame_A,
		Gun_Flame_B,
		Cinema_01_Walk,
		Cinema_01_BattleReady,
		Cinema_02_Stand,
		Guard_Walk_F,
		Guard_Walk_FL,
		Guard_Walk_FR,
		Guard_Walk_L,
		Guard_Walk_R,
		Guard_Walk_B,
		Guard_Walk_BL,
		Guard_Walk_BR,
		Guard_Ssword_Start,
		Guard_Ssword_Loop,
		Guard_Ssword_End,
		Guard_Ssword_Break,
		Guard_Ssword_Hit,
		Guard_Gun_Start,
		Guard_Gun_Loop,
		Guard_Gun_End,
		Guard_Gun_Break,
		Guard_Gun_Hit,
		Guard_Halberd_Start,
		Guard_Halberd_Loop,
		Guard_Halberd_End,
		Guard_Halberd_Break,
		Guard_Halberd_Hit,
		Guard_Hammer_Start,
		Guard_Hammer_Loop,
		Guard_Hammer_End,
		Guard_Hammer_Break,
		Guard_Hammer_Hit,
		Guard_LSword_Start,
		Guard_LSword_Loop,
		Guard_LSword_End,
		Guard_LSword_Break,
		Guard_LSword_Hit,
	}P_NEWANI;

private:
	P_NEWANI	m_eAnim_Upper = Cmn_CheckPoint_End;
	P_NEWANI	m_eAnim_Lower = Cmn_CheckPoint_End;
	P_NEWANI	m_eAnim_RightArm = Cmn_CheckPoint_End;
	P_NEWANI	m_eAnim_LeftArm = Cmn_CheckPoint_End;

private:
	PLAYER_BODY	m_ePlayerBody = Player_Body_01;

private:
	_long	m_lDebugValue = 0;
};

END