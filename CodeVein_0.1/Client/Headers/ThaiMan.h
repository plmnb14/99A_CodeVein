#pragma once

#include "Info_Monster.h"

BEGIN(Client)

class CThaiMan final : public CGameObject
{
public:
	enum THAIMAN_IDLETYPE { IDLE_IDLE, IDLE_SIT, IDLE_SCRATCH, IDLE_CROUCH };
	enum THAIMAN_MOVETYPE { MOVE_RUN, MOVE_WALK, MOVE_DODGE };
	enum THAIMAN_ATKTYPE { ATK_NORMAL, ATK_COMBO };
	enum THAIMAN_HITTYPE { HIT_STRONG, HIT_NORMAL, HIT_WEAK };
	enum THAIMAN_CCTYPE { CC_STUN, CC_DOWN, CC_BLOW };
	enum THAIMAN_DEADTYPE { DEAD_DEAD, DEAD_DEAD_S, DEAD_DEAD_P };

	enum ATK_NORMAL_TYPE
	{
		NORMAL_RDIAGONAL, //n1
		NORMAL_L,
		NORMAL_BackDumpling,
		NORMAL_DropKick, //n4
		NORMAL_LkBk, //s1
		NORMAL_Capoeira,
		NORMAL_RDigonalLUpperRAccel, //sp1
		NORMAL_LkBkFk,
		NORMAL_RDigonalRRScrewRchop
	};

	enum ATK_COMBO_TYPE
	{
	};

	enum THAIMAN_ANI 
	{
		Idle,
		NF_Sit,
		NF_Sit_End,
		NF_Scratch,
		NF_Scratch_End,
		NF_Crouch,
		NF_Crouch_End,

		Run,
		Walk_R,
		Walk_L,
		Walk_F,
		Walk_B,
		Dodge_R,
		Dodge_L,

		Dodge,
		Stun_Start,
		Stun_Loop,
		Stun_End,

		Down_S_Loop,
		Down_S_End,

		Down_P_Loop,
		Down_P_End,
		DmgStrike_S,
		DmgStrike_P,
		DmgBlow_F,
		DmgBlow_B,

		Dmg_S_FR,
		Dmg_S_FL,
		Dmg_S_BR,
		Dmg_S_BL,

		Dmg_N_FR,
		Dmg_N_FL,
		Dmg_N_BR,
		Dmg_N_BL,

		Dmg_W_FR,
		Dmg_W_FL,
		Dmg_W_BR,
		Dmg_W_BL,

		Death_F,
		Death_B,
		Death,

		Atk_StimPack,
		Atk_Add2,
		Atk_Add1,
		Atk_SP03,
		Atk_SP02,
		Atk_SP01,
		Atk_S02,
		Atk_S01,
		Atk_N04,
		Atk_N03,
		Atk_N02,
		Atk_N01
	};

	enum BONE_TYPE
	{
		Bone_Range,
		Bone_Body,
		Bone_Head, 
		Bone_LeftHand, //0
		Bone_RightHand, //1
		Bone_LeftToeBase, //2
		Bone_RightToeBase, //3
		Bone_End 
	};

protected:
	explicit CThaiMan(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CThaiMan(const CThaiMan& rhs);
	virtual ~CThaiMan() = default;

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

	void Function_FBLR();
	void Function_RotateBody();
	void Function_MoveAround();
	void Function_CoolDown();
	void Function_Movement(_float _fspeed, _v3 _vDir = { V3_NULL });
	void Function_DecreMoveMent(_float _fMutiply = 1.f);
	void Function_ResetAfterAtk();

	void Check_PosY();
	void Check_Hit();
	void Check_Dist();
	void Check_AniEvent();
	
	void Play_RandomAtkNormal();
	void Play_RandomAtkCombo();
	void Play_RDiagonal(); //n1 ¿ì´ë°¢
	void Play_L(); //n2 ÁÂÈ¾
	void Play_BackDumpling(); //n3 ¹é´ýºí¸µ
	void Play_DropKick(); //n4
	void Play_LkBk(); //s1 ¿·,µÚµ¹¾ÆÂ÷±â
	void Play_Capoeira(); //s2
	void Play_RDigonalLUpperRAccel(); //sp1 ¿ì´ë°¢,ÁÂÈ¾,¾Ç¼¿Á¡ÇÁatk
	void Play_RkBkFk(); //sp2 R¿·,LµÚ,R¾ÕÂ÷±â
	void Play_RDigonalRRScrewRchop(); //sp3 ¿ì´ë°¢,¿ìÈ¾,¿ì½Â·æ±Ç,¿ì³»¸®Ä¡±â

	void Play_Idle();
	void Play_Move();
	void Play_Hit(); 
	void Play_CC();
	void Play_Dead();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT Ready_Status(void* pArg);
	HRESULT Ready_Collider();
	HRESULT Ready_BoneMatrix();

public:
	static CThaiMan* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	CMonsterUI*			m_pMonsterUI = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CNavMesh*			m_pNavMesh = nullptr;
	CCollider*			m_pCollider = nullptr;

	CTransform*			m_pTargetTransform = nullptr;

	_mat*				m_matBone[Bone_End];
	_double				m_dTimeDelta = 0;
	_double				m_dAniPlayMul = 1;

	_float				m_fSkillMoveSpeed_Cur = 0.f;
	_float				m_fSkillMoveSpeed_Max = 0.f;
	_float				m_fSkillMoveAccel_Cur = 0.5f;
	_float				m_fSkillMoveAccel_Max = 0.f;
	_float				m_fSkillMoveMultiply = 1.f;

	MONSTER_ANITYPE		m_eFirstCategory;
	THAIMAN_IDLETYPE	m_eSecondCategory_IDLE;
	THAIMAN_MOVETYPE	m_eSecondCategory_MOVE;
	THAIMAN_ATKTYPE		m_eSecondCategory_ATK;
	THAIMAN_HITTYPE		m_eSecondCategory_HIT;
	THAIMAN_CCTYPE		m_eSecondCategory_CC;
	THAIMAN_DEADTYPE	m_eSecondCategory_DEAD;

	MONSTER_WEAPONTYPE	m_eWeaponState = MONSTER_WEAPONTYPE::WEAPON_NONE;
	MONSTER_COLORTYPE	m_eColorState = MONSTER_COLORTYPE::COLOR_NONE;
	ATK_COMBO_TYPE		m_eAtkCombo;
	THAIMAN_ANI			m_eState;
	FBLR				m_eFBLR;

	_bool				m_bEventTrigger[20] = {};
	_bool				m_bCanPlayDead = false;
	_bool				m_bInRecognitionRange = false;
	_bool				m_bInAtkRange = false;
	_bool				m_bCanChase = false;
	_bool				m_bCanCoolDown = false;
	_bool				m_bIsCoolDown = false;
	_bool				m_bAtkCategory = true;
	_bool				m_bCanCombo = true;
	_bool				m_bIsCombo = false;
	_bool				m_bCanIdle = true;
	_bool				m_bIsIdle = false;

	_float				m_fRecognitionRange = 20.f;
	_float				m_fShotRange = 10.f;
	_float				m_fAtkRange = 5.f;
	_float				m_fPersonalRange = 2.f;
	_float				m_fCoolDownMax = 0.f;
	_float				m_fCoolDownCur = 0.f;
	_float				m_fSpeedForCollisionPush = 2.f;

	_int				m_iRandom = 0;
	_int				m_iDodgeCountMax = 3;
	_int				m_iDodgeCount = 0;
};

END

