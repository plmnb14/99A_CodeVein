#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CYeti final : public CGameObject
{
public:
	enum MONSTER_ANITYPE { IDLE, MOVE, ATTACK, HIT, CC, DEAD };

	enum YETI_IDLETYPE { IDLE_IDLE, IDLE_THREAT };
	enum YETI_MOVETYPE { MOVE_RUN, MOVE_WALK };
	enum YETI_ATKTYPE { ATK_NORMAL, ATK_COMBO }; //일반공격, 콤보공격
	enum YETI_HITTYPE { HIT_HIT_F, HIT_HIT_B };
	enum YETI_CCTYPE { CC_DOWN, CC_STUN, CC_BLOW }; //다운,스턴,날아감
	enum YETI_DEADTYPE { DEAD_DEAD, DEAD_DEAD_S }; //일반, 특정 상황에서 죽었을 경우

	enum ATK_NORMAL_TYPE
	{
		NORMAL_UPPER,
		NORMAL_SMASH,
		NORMAL_SHOULDER,
		NORMAL_Rush_Start,
		NORMAL_Rush_Loop,
		NORMAL_Rush_End,
		NORMAL_ROLL,
		NORMAL_ROLLINGSLASH,
		NORMAL_ICETHROW,
		NORMAL_R_L_R_L,
		NORMAL_WOODCHOP,
		NORMAL_FASTL_R,
		NORMAL_L_R_SWEAP,
		NORMAL_R_UPPER_ACCELDOWN,
		NORMAL_SLOWL_R,
		NORMAL_HOULING,
		NORMAL_BODYPRESS
	};
	enum ATK_COMBO_TYPE
	{
		//sp6 shoulder 0.5 0.85
		//sp6 smash 0.5 0.85
		//sp6 swing 0.5 0.85
	};

	enum YETI_ANI
	{
		Idle,
		NF_Threat,
		NF_Threat_Loop,

		Turn_R180,
		Turn_R90,
		Turn_L180,
		Turn_L90,

		Run,
		Walk_R,
		Walk_L,
		Walk_FR,
		Walk_FL_RFoot,
		Walk_FL_LFoot,
		Walk_F,
		Walk_BR_RFoot,
		Walk_BR_LFoot,
		Walk_BL,
		Walk_B,

		Dmg_F,
		Dmg_B,

		Death,

		Atk_Swing,
		Atk_DonkeyKick, //폐기
		Atk_Smash,
		Atk_Shoulder,
		Atk_Rush_Start,
		Atk_Rush_Loop,
		Atk_Rush_End,
		Atk_Rush,
		Atk_RollingSlash,
		Atk_IceThrowing,
		Atk_Sp06,
		Atk_Sp05,
		Atk_Sp04,
		Atk_Sp03,
		Atk_Sp02,
		Atk_Sp01,
		Atk_Field,
		Atk_BodyPress
	};

	enum BONE_TYPE { Bone_Range, Bone_Body, Bone_Head, Bone_LeftHand, Bone_RightHand, Bone_Shoulder, Bone_End };

	enum FBLR { FRONT, BACK, LEFT, RIGHT };

protected:
	explicit CYeti(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CYeti(const CYeti& rhs);
	virtual ~CYeti() = default;

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
	void Function_TargetAround();

	void Check_Hit();
	void Check_FBLR();
	void Check_Dist();
	void Set_AniEvent();

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

	void Play_Idle(); //일상 동작 전부 이곳에
	void Play_Move(); //걷기,달리기 회피 한곳에
	void Play_Hit();  //피격 한곳에
	void Play_CC(); //cc 한곳에
	void Play_Dead(); //죽음 한곳에

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT Ready_Collider();
	HRESULT Ready_BoneMatrix();

public:
	static CYeti* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CNavMesh*			m_pNavMesh = nullptr;
	CCollider*			m_pCollider = nullptr;

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

	MONSTER_ANITYPE		m_eFirstCategory; //대분류
	YETI_IDLETYPE		m_eSecondCategory_IDLE; //중분류
	YETI_MOVETYPE		m_eSecondCategory_MOVE;
	YETI_ATKTYPE		m_eSecondCategory_ATK;
	YETI_HITTYPE		m_eSecondCategory_HIT;
	YETI_CCTYPE			m_eSecondCategory_CC;
	YETI_DEADTYPE		m_eSecondCategory_DEAD;

	ATK_COMBO_TYPE		m_eAtkCombo;
	YETI_ANI			m_eState;

	_bool				m_bEventTrigger[20] = {};
	/////////Test
	_bool				m_bCanDead = false;
	_bool				m_bCanDissolve = false;
	//////////////
	_bool				m_bCanPlayDeadAni = false;
	_bool				m_bIsPlayDeadAni = false;

	_bool				m_bInRecognitionRange = false;
	_bool				m_bInAtkRange = false;

	_bool				m_bCanChase = false;

	_bool				m_bCanCoolDown = false;
	_bool				m_bIsCoolDown = false;

	_bool				m_bCanAtkCategoryRandom = true;
	_bool				m_bIsAtkCombo = false;
	_bool				m_bCanIdleRandom = true;

	_float				m_fRecognitionRange = 20.f; //유동
	_float				m_fShotRange = 10.f; //유동
	_float				m_fAtkRange = 5.f; //유동
	_float				m_fCoolDownMax = 0.f; //0초기화
	_float				m_fCoolDownCur = 0.f; //0초기화
	_float				m_fSpeedForCollisionPush = 2.f; //선 충돌 측정용 고정값

	_int				m_iRandom = 0; //사용할 필요 없어져가니 지울 예정
	_int				m_iDodgeCountMax = 3; //3회 피격시 회피 유동값
	_int				m_iDodgeCount = 0; //n회 피격시 회피 0초기화

};

END

