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
	
private:
	typedef enum tagINVEN_TYPE
	{
		TYPE_HAIR,
		TYPE_FACE,
		TYPE_EYE,
		TYPE_MASK,
		TYPE_INNER,
		TYPE_END
	}Costume;

	typedef enum tagCustomType
	{
		Custom_Hair , Custom_Head , Custom_Mask, Custom_Body, Custom_ShowAll, Custom_end
	}CUSTOM;

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
	void Set_Costume_Head(CCostume_Head::CHAR_HEAD _eHeadType) { m_eHeadType = _eHeadType; }
	void Set_Costume_Mask(CCostume_Mask::MASK_TYPE _eMaskType) { m_eMaskType = _eMaskType; }
	void Set_Costume_Body(PLAYER_BODY _eBodyType) { Change_PlayerBody(_eBodyType); }

public:
	void Set_WeaponSlot(ACTIVE_WEAPON_SLOT eType, WPN_PARAM* pParam);
	void Set_ArmorSlot(ARMOR_All_DATA eType, ARMOR_PARAM* pParam);
	void Set_Target_UI(CGameObject* pGameObject);

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);
	virtual HRESULT Render_GameObject_SetPass(CShader* pShader, _int iPass, _bool _bIsForMotionBlur = false);

public:
	void Set_OnNPCUI(_bool _bOnNPCUI) { m_bOnUI_NPCTalk = _bOnNPCUI; }
	void Set_YakumoUI(_bool _bYakumoUI) { m_bOnYakumo_UI = _bYakumoUI; };
	void Set_YokumoUI(_bool _bYokumoUI) { m_bOnYokumo_UI = _bYokumoUI; };
	void Set_Jack(_bool _bJacKUI) { m_bOnJack_UI = _bJacKUI; };

	_bool Get_YakumoUI() { return m_bOnYakumo_UI; }
	_bool Get_YokumoUI() { return m_bOnYokumo_UI; }

public:
	CRigidBody* Get_Player_RigidBody() { return m_pRigid; }

public:
	void Reset_OldAnimations();

public:
	void Teleport_ResetOptions(_int _eSceneID, _int _eTeleportID);

private:
	void FootWalkTimerUpdate(_double _deltaTime);

private:
	ACTOR_INFO				m_tInfo = {};
	ACT_STATE				m_eActState = ACT_Summon;
	WEAPON_STATE			m_eMainWpnState = WEAPON_SSword;
	WEAPON_STATE			m_eSubWpnState = WEAPON_SSword;
	DRAIN_STATE				m_eDrainState = DRAIN_END;
	ACTIVE_WEAPON_SLOT		m_eActiveSlot = WPN_SLOT_A;

	_float					m_FallingTimer = 0.f;

	_bool					m_bCanPlayWalkSound = true;
	_float					m_fFootWalkTimer_Cur = 0.5f;
	_float					m_fFootWalkTimer_Max = 0.5f;

private:
	_bool					m_bOldMoveDierection[MOVE_End] = {};

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
	CShader*				m_pSkinnedShader = nullptr;
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
	_ulong					m_dwOldx = 999999;

private:
	vector<CCollider*>		m_vecPhsycColl;

private:
	vector<SKILL_INFO*>		m_vecFullSkillInfo;
	vector<SKILL_INFO*>		m_vecActiveSkillInfo;

private:
	CUSTOM					m_eCustomViewType = Custom_ShowAll;

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
	_bool					m_bOnInvenChange = false;

	_bool					m_bOnYakumo_UI = false;
	_bool					m_bOnYokumo_UI = false;
	_bool					m_bOnJack_UI = false;

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
	_float					m_fRightArmTimer = 1.f;
	_float					m_fCurMoveSpeed = 0.f;
	_v3						m_vMoveDir = {};

private:
	_float					m_fAtkEndTime = 0.f;

private:
	_float					m_fAmingRange = 20.f;

private: // For Dissolve
	_float					m_fDissolveY = 0.f;

private:
	_bool					m_bCheat_HP = false;
	_bool					m_bCheat_SP = false;
	_bool					m_bCheat_ST = false;

private:
	_float					m_fStaminaCost_Dodge = 0.f;
	_float					m_fStaminaCost_WeakAttack = 0.f;
	_float					m_fStaminaCost_HeavyAttack = 0.f;
	_float					m_fStamina_RecoverySpeed = 0.f;

private:
	_bool				m_arrbActiveSkillOn[8] = {};
	SKILL_INFO			m_arrSkillInfo[8] = {};

private:
	_bool				m_bWeaponActive[2] = {};

private:
	_bool				m_bOnCustomMode = false;
	

private:
	HRESULT Add_Component();
	HRESULT SetUp_Default();
	HRESULT SetUp_ConstantTable(CShader* pShader);

private:
	void Movement_Aiming(_float _fAngle, _float _fMoveSpeed);
	void Movement_NonAiming(_float _fRecover, _float _fAngle, _float _fRadian, _float _fMoveSpeed);

private:
	void Target_AimChasing();

private:
	void OnCollisionEnter();

private:
	void Update_Collider();

private:
	void Reset_BattleState();

private:
	void Check_NPC();
	void Check_Mistletoe();
	_int Check_HitDirection();
	_bool Check_CunterAngle(CGameObject* pObj);
	_bool Check_CunterTarget();
	void Check_Stamina(_double dTimeDelta);
	void Check_Cheat();

private:
	void Change_PlayerBody(PLAYER_BODY _eBodyType);
	void Update_OuterAnim();

private:
	void Parameter_State();
	void Parameter_State_Upper();
	void Parameter_Atk();
	void Parameter_Movement();
	void Parameter_HeavyCharging();
	void Parameter_YPos(_double dTimeDelta);
	void Parameter_Collision();
	void Parameter_Aiming();
	void Parameter_HitCheck();
	void Parameter_CheckActiveSkill();
	void Parameter_CheckActiveWeapon();

private:
	void KeyInput();
	void KeyDown();
	void KeyUp();
	void Key_Movement_Down();
	void Key_Movement_Up();
	void Key_ChangeWeapon();
	void Key_Special();
	void Key_Attack();
	void Key_Skill();
	void Key_Utility();
	void Key_InterAct();
	void Key_BloodSuck();
	void Key_UI_n_Utiliy(_bool _bActiveUI);

private:
	void Play_Idle();
	void Play_Run();
	void Play_Dash();
	void Play_MoveDelay();
	void Play_Dodge();
	void Play_WeakAtk();
	void Play_HeavyAtk();
	void Play_Buff();
	void Play_Hit();
	void Play_Dead();
	void Play_WeaponChange();
	void Play_Spawn();
	void Play_PickUp();
	void Play_Down();

private:
	void Play_Summon();
	void Play_Disappear();

private:
	void Play_BloodSuck();
	void Play_BloodSuckCount();
	void Play_BloodSuckCombo();
	void Play_BloodSuckExecution();

private:
	void Play_Skills();
	void Play_Buffs();
	void Play_Enchants();

private:
	void Play_OneHandSkill_01();
	void Play_OneHandSkill_02();
	void Play_OneHandskill_03();

private:
	void Play_TwoHandSkill_01();
	void Play_TwoHandSkill_02();
	void Play_TwoHandSkill_03();

private:
	virtual void Play_GunSkill();

private:
	void Play_Ssword_WeakAtk();
	void Play_Ssword_HeavyAtk();
	void Play_Ssword_DownAtk();
	void Play_Ssword_DashAtk();

private:
	void Play_Gun_WeakAtk();
	void Play_Gun_HeavyAtk();
	void Play_Gun_DownAtk();
	void Play_Gun_DashAtk();

private:
	void Play_Halverd_WeakAtk();
	void Play_Halverd_HeavyAtk();
	void Play_Halverd_DownAtk();
	void Play_Halverd_DashAtk();

private:
	void Play_Hammer_WeakAtk();
	void Play_Hammer_HeavyAtk();
	void Play_Hammer_DownAtk();
	void Play_Hammer_DashAtk();

private:
	void Play_Lsword_WeakAtk();
	void Play_Lsword_HeavyAtk();
	void Play_Lsword_DownAtk();
	void Play_Lsword_DashAtk();

private:
	void Ready_Bodys();
	void Ready_Heads();
	void Ready_Masks();
	void Ready_Weapon();
	void Ready_DrainWeapon();
	void Ready_BoneMatrix();
	void Ready_Collider();
	void Ready_Skills();
	void Ready_Rigid();
	void Temp_ActiveSkills();

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
	void Active_UI_Mistletoe(_bool _bResetUI = false);		// 겨우살이
	void Active_UI_Inventory(_bool _bResetUI = false);		// 인벤토리
	void Active_UI_StageSelect(_bool _bResetUI = false);	// 스테이지 선택
	void Active_UI_NPC(_bool _bResetUI = false);			// NPC 와의 대화
	void Active_UI_BloodCode(_bool _bResetUI = false);			// NPC 와의 대화
	void Active_UI_WeaponShop_Yakumo(_bool _bResetUI = false);
	void Active_UI_MaterialShop_Yokumo(_bool _bResetUI = false);

public:
	void Active_UI_LockOn(_bool _bResetUI = false);

	void UI_Check();

public:
	static	CPlayer* Create(_Device pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);

public:
	virtual void Free();

private:
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