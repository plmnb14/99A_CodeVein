#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CWeapon;
class CMonsterUI;
class CMonkey final : public CGameObject
{
public:
	enum FBLR { FRONT, FRONTLEFT, FRONTRIGHT, BACK, BACKLEFT, BACKRIGHT, LEFT, RIGHT };
	enum MONSTER_ANITYPE { IDLE, MOVE, ATTACK, HIT, CC, DEAD };
	
	enum MONKEY_IDLETYPE { IDLE_IDLE, IDLE_SIT };
	enum MONKEY_MOVETYPE { MOVE_RUN, MOVE_WALK, MOVE_DODGE };
	enum MONKEY_ATKTYPE { ATK_NORMAL, ATK_COMBO };
	enum MONKEY_HITTYPE { HIT_STRONG, HIT_NORMAL }; //s->cc n->hit
	enum MONKEY_CCTYPE { CC_DOWN_P, CC_DOWN_S };
	enum MONKEY_DEADTYPE { DEAD_DEAD, DEAD_DEAD_F, DEAD_DEAD_B};

	enum ATK_NORMAL_TYPE 
	{
		NORMAL_ATK_ROTBODY,
		NORMAL_RDIGONAL,
		NORMAL_JUMPDOWN,
		NORMAL_JUMPLHAND,
		NORMAL_JUMP_ROTBODY,
		NORMAL_FANGSHOOT 
	};
	enum ATK_COMBO_TYPE { COMBO_NORMAL, COMBO_JUMP_CLOCK, COMBO_RUNATK};

	enum MONKEY_ANI 
	{
		Idle,
		NF_Sit,
		NF_Sit_End,

		Run,
		Walk,
		NF_Walk,

		Dodge,
		Dodge_L,
		Dodge_R,

		Down_S_Start,
		Down_S_Loop,
		Down_S_End,

		Down_P_Start,
		Down_P_Loop,
		Down_P_End,

		Dmg_F,
		Dmg_B,

		Death,
		Death_F,
		Death_B,

		Atk_Sp_Start,
		Atk_Sp_Loop,
		Atk_Sp_End,		
		Atk_N02,
		Atk_N01,
		Atk_Jump03,
		Atk_Jump02,
		Atk_Jump01,
		Atk_FangShoot

	};

	enum BONE_TYPE { Bone_Range, Bone_Body, Bone_Head, Bone_LeftHand, Bone_End };

public:
	typedef struct tagMonkeyInfo
	{
		tagMonkeyInfo(_float _fDMG, _float _fHpMax,
			_float _fKnowRange, _float _fAtkRange,
			_float _ShotRange, _int iDodgeMax)
			:
			fKonwingRange(_fKnowRange),
			fCanAttackRange(_fAtkRange),
			fCanShotRangeIfGunChooose(_ShotRange),
			fDodgeCountMax(iDodgeMax)
		{
			tMonterStatus.fDamage = _fDMG;
			tMonterStatus.fHp_Max = _fHpMax;
		}
		OBJECT_PARAM		tMonterStatus;
		_float				fKonwingRange = 20.f;
		_float				fCanShotRangeIfGunChooose = 10.f;
		_float				fCanAttackRange = 5.f;
		_int				fDodgeCountMax = 3;
	}MONKEY_INFO;

protected:
	explicit CMonkey(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CMonkey(const CMonkey& rhs);
	virtual ~CMonkey() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_SetPass(CShader* pShader, _int iPass);

private:
	void Update_Collider();
	void Render_Collider();
	void Enter_Collision();
	void Check_CollisionPush();
	void Check_CollisionEvent(list<CGameObject*> plistGameObject);

	void Function_RotateBody();
	void Function_CoolDown();
	void Function_Movement(_float _fspeed, _v3 _vDir = { V3_NULL });
	void Function_DecreMoveMent(_float _fMutiply = 1.f);
	void Function_ResetAfterAtk();

	void Checkk_PosY();
	void Check_Hit();
	void Check_FBLR();
	void Check_Dist();
	void Check_AniEvent();

	void Play_RandomAtkNormal();
	void Play_RandomAtkCombo();

	void Play_FangShot();
	void Play_Jump_RotBody(); //Jump1
	void Play_JumpLHand(); //jump2
	void Play_JumpDown(); //jump3
	void Play_RDiagonal(); //n1
	void Play_Atk_RotBody(); //n2
	void Play_Combo_Normal(); //n1 n2 0.85 0.95
	void Play_Combo_Jump_Clock(); //jump1 n2 0.85 0.95
	void Play_Combo_RunAtk();  //0.8 0.9 0.92

	void Play_Idle(); 
	void Play_Move(); 
	void Play_Hit();  
	void Play_CC(); 
	void Play_Dead(); 

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT Ready_Status(void* pArg);
	HRESULT Ready_Weapon();
	HRESULT Ready_Collider();
	HRESULT Ready_BoneMatrix();

public:
	static CMonkey* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CNavMesh*			m_pNavMesh = nullptr;
	CCollider*			m_pCollider = nullptr;
	CWeapon*			m_pWeapon = nullptr;

	CTransform*			m_pTargetTransform = nullptr;

	_v3					m_vBirthPos;
	_mat*				m_matBone[Bone_End];
	_double				m_dTimeDelta = 0;
	_double				m_dAniPlayMul = 1;

	_float				m_fSkillMoveSpeed_Cur = 0.f;
	_float				m_fSkillMoveSpeed_Max = 0.f;
	_float				m_fSkillMoveAccel_Cur = 0.5f;
	_float				m_fSkillMoveAccel_Max = 0.f;
	_float				m_fSkillMoveMultiply = 1.f;

	MONSTER_ANITYPE		m_eFirstCategory;
	MONKEY_IDLETYPE		m_eSecondCategory_IDLE;
	MONKEY_MOVETYPE		m_eSecondCategory_MOVE;
	MONKEY_ATKTYPE		m_eSecondCategory_ATK;
	MONKEY_HITTYPE		m_eSecondCategory_HIT;
	MONKEY_CCTYPE		m_eSecondCategory_CC;
	MONKEY_DEADTYPE		m_eSecondCategory_DEAD;

	ATK_COMBO_TYPE		m_eAtkCombo;
	MONKEY_ANI			m_eState;
	FBLR				m_eFBLR;

	_bool				m_bEventTrigger[20] = {};

	_bool				m_bCanPlayDead = false;
	_bool				m_bCanDissolve = false;

	_bool				m_bInRecognitionRange = false;
	_bool				m_bInAtkRange = false;

	_bool				m_bCanChase = false;

	_bool				m_bCanCoolDown = false;
	_bool				m_bIsCoolDown = false;

	_bool				m_bAtkCategory = true;
	_bool				m_bCanInterrupt = true;
	_bool				m_bCanCombo = true;
	_bool				m_bIsCombo = false;

	_bool				m_bCanIdle = true;
	_bool				m_bIsIdle = false;

	_float				m_fRecognitionRange = 20.f;
	_float				m_fShotRange = 10.f;
	_float				m_fAtkRange = 5.f;
	_float				m_fCoolDownMax = 0.f;
	_float				m_fCoolDownCur = 0.f;
	_float				m_fSpeedForCollisionPush = 2.f;

	_int				m_iRandom = 0;
	_int				m_iDodgeCountMax = 3; //3회 피격시 회피
	_int				m_iDodgeCount = 0; //n회 피격시 회피

	_float				m_fShotDelay = 0.f;

	//////////// 채유미
	// 몬스터 HP바 UI
	CMonsterUI*			m_pMonsterUI = nullptr;

};

END

