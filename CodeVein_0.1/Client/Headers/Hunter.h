#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CWeapon;
class CMonsterUI;

class CHunter final : public CGameObject
{
public:
	enum MONSTER_ANITYPE { IDLE, MOVE, ATTACK, HIT, CC, DEAD };

	enum WEAPON_ANITYPE	 {GUN, HALVERD, HAMMER, LSWORD, SSWORD, WEAPON_ANITYPE_END};

	enum HUNTER_IDLETYPE { IDLE_IDLE, IDLE_STAND, IDLE_CROUCH, IDLE_SIT };
	enum HUNTER_MOVETYPE { MOVE_RUN, MOVE_WALK, MOVE_DODGE }; //�پ��� �̵�, ȸ��
	enum HUNTER_ATKTYPE  { ATK_NORMAL, ATK_COMBO }; //�Ϲݰ���, �޺�����
	enum HUNTER_HITTYPE  { HIT_STRONG, HIT_NORMAL, HIT_WEAK }; //�ǰ� ����, �������� ���� �ǰ� ���
	enum HUNTER_CCTYPE { CC_DOWN, CC_STUN, CC_BLOW }; //�ٿ�,����,���ư�
	enum HUNTER_DEADTYPE { DEAD_DEAD, DEAD_DEAD_S }; //�Ϲ�, Ư�� ��Ȳ���� �׾��� ���

	enum ATK_NORMAL_TYPE
	{
		NORMAL_GUN_KICK, 
		NORMAL_GUN_R, 
		NORMAL_GUN_SHOOT, 
		NORMAL_GUN_SNIPE,

		NORMAL_HALVERD_STEPPIERCE,
		NORMAL_HALVERD_RISEUP,
		NORMAL_HALVERD_PIERCE,
		NORMAL_HALVERD_DEEPPIERCE,
		NORMAL_HALVERD_CLOCKTWICE,
		NORMAL_HALVERD_SWING_JUMP,
		NORMAL_HALVERD_SWEAP,
		NORMAL_HALVERD_SLASHFORTH,
		NORMAL_HALVERD_TWOUPPER,

		NORMAL_HAMMER_UPPER,
		NORMAL_HAMMER_SLASH,
		NORMAL_HAMMER_SMASH,
		NORMAL_HAMMER_TWOUPPER,

		NORMAL_LSWORD_KNEEKICK,
		NORMAL_LSWORD_RIGHT,
		NORMAL_LSWORD_RDIAGONAL,
		NORMAL_LSWORD_SMASH,
		//NORMAL_LSWORD_SWING, //���

		NORMAL_SSWORD_JUMP,
		NORMAL_SSWORD_RAISEUP,
		NORMAL_SSWORD_UPPER,
		NORMAL_SSWORD_Upper_L,
		NORMAL_SSWORD_WOODCHOP,
		NORMAL_SSWORD_ELBOW,
		NORMAL_SSWORD_HELMETBREAK,
		NORMAL_SSWORD_CRITICALDRAW

	};
	enum ATK_COMBO_TYPE 
	{
		COMBO_GUN_SHOT, COMBO_GUN_CQC,
		COMBO_HALVERD_THIRDATK, COMBO_HALVERD_PIERCETWICE, COMBO_HALVERD_PIERCEWIND,
		COMBO_LSWORD_NORMAL, COMBO_LSWORD_STRONG,
		COMBO_SSWORD_STEPPIERCE, COMBO_SSWORD_STRONG, COMBO_SSWORD_Diagonal_L

	};

	enum HUNTER_ANI
	{
		Idle,
		Deformation,
		Stand,
		Stand_End,
		Crouch,
		Crouch_End,
		Sit,
		Sit_End,
		LookAround,

		Run,
		NF_Walk,

		Walk_R,
		Walk_L,
		Walk_FR,
		Walk_FL_R,
		Walk_FL_L,
		Walk_F,
		Walk_BR_RFoot,
		Walk_BR_LFoot,
		Walk_BL,
		Walk_B,

		Dodge,

		Turn_R180,
		Turn_R90,
		Turn_L180,
		Turn_L90,

		Groggy_Start,
		Groggy_Loop,
		Groggy_End,
		DmgRepel, //��� ���ϴ� �ɷ�
		DmgBlow_B,
		DmgBlow_F,
		Dmg02_FR,
		Dmg02_FL,
		Dmg02_BR,
		Dmg02_BL,
		Dmg01_FR,
		Dmg01_FL,
		Dmg01_BR,
		Dmg01_BL,

		Death_F,
		Death_B,
		Death,

		KetsugiGroundRange,
		KetsugiEnchant,
		KetsugiBuffSelf,
		KetsugiAllRangeShoot,
		KetsugiAimShoot,

		SSword_Atk_Sp03_Start,
		SSword_Atk_Sp03_Loop,
		SSword_Atk_Sp03_End,
		SSword_Atk_Sp02,
		SSword_Atk_Sp01,
		SSword_Atk_S03,
		SSword_Atk_S02,
		SSword_Atk_S01,
		SSword_Atk_N05,
		SSword_Atk_N04,
		SSword_Atk_N03,
		SSword_Atk_N02,
		SSword_Atk_N01,
		SSword_Atk_Range, //���
		SSword_Atk_RaiseUp,
		SSword_Atk_Jump,

		LSword_Atk_Swing,
		LSword_Atk_Smash,
		LSword_Atk_S03,
		LSword_Atk_S02,
		LSword_Atk_S01,
		LSword_Atk_N03,
		LSword_Atk_N02,
		LSword_Atk_N01,
		LSword_Atk_Kick,

		Hammer_Atk_TwoUpper,
		Hammer_Atk_Smash,
		Hammer_Atk_Slash,
		Hammer_Atk_N,

		Halberd_Atk_TwoUpper,
		Halberd_Atk_TwoSlash,
		Halberd_Atk_Sweep,
		Halberd_Atk_Sp04, //���
		Halberd_Atk_Sp03,
		Halberd_Atk_Sp02,
		Halberd_Atk_Sp01, //���
		Halberd_Atk_S03,
		Halberd_Atk_S02,
		Halberd_Atk_S01,
		Halberd_Atk_N03,
		Halberd_Atk_N02,
		Halberd_Atk_N01,
		Halberd_Atk_RaiseUp,
		Halberd_Atk_Force,

		Bayonet_Atk_Snipe,
		Bayonet_Atk_Shoot02,
		Bayonet_Atk_Shoot01,
		Bayonet_Atk_N03,
		Bayonet_Atk_N02,
		Bayonet_Atk_N01,
		Bayonet_Atk_Kick

	};

	enum BONE_TYPE { Bone_Range, Bone_Body, Bone_Head, Bone_RightForeArm, Bone_LeftKnee, Bone_LeftToe, Bone_End };

	enum FBLR { FRONT, BACK, LEFT, RIGHT };

protected:
	explicit CHunter(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CHunter(const CHunter& rhs);
	virtual ~CHunter() = default;

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

	void Play_Gun_Kick();
	void Play_Gun_R();
	void Play_Gun_Shoot();
	void Play_Gun_Snipe();
	void Play_Gun_Combo_Shot();
	void Play_Gun_Combo_CQC();

	void Play_Halverd_StepPierce();
	void Play_Halverd_RiseUp();
	void Play_Halverd_Pierce(); //���� ���n01
	void Play_Halverd_DeepPierce(); //������� �s01
	void Play_Halverd_ClockTwice(); //2ȸ�� sp2
	void Play_Halverd_Swing_Jump(); //�ֵθ��� �����ؼ� ����ġ�� sp3
	void Play_Halverd_Sweap(); //5Ÿ��
	void Play_Halverd_SlashForth(); //4Ÿ��
	void Play_Halverd_TwoUpper(); //��,�� ���� 1ȸ��
	void Play_Halverd_Combo_ThirdAtk(); //�Ǿ,Ⱦ����,�ֵѰ��� n1 0.6 n2 0.6 n3 0.95
	void Play_Halverd_Combo_PierceTwice(); //���,�Ѽ� ��� s1 0.5 s3 0.95
	void Play_Halverd_Combo_PierceWind(); //��� �ֵθ��� s1 0.4 s2 0.95

	void Play_Hammer_Upper();
	void Play_Hammer_Slash();
	void Play_Hammer_Smash();
	void Play_Hammer_TwoUpper();

	void Play_LSword_KneeKick(); 
	void Play_LSword_Right(); //n1
	void Play_LSword_RDiagonal(); //s1
	void Play_LSword_Smash();
	void Play_LSword_Combo_Normal(); //��,��,�ֵθ��� 0.6 0.55 0.9 n1,2,3
	void Play_LSword_Combo_Strong(); //��밢, ���, �������� 0.45 0.45 0.9 s1,2,3
	
	void Play_SSword_Jump();
	void Play_SSword_RaiseUp();
	void Play_SSword_Upper(); //N1
	void Play_SSword_Upper_L(); //n4
	void Play_SSword_WoodChop();//n5 ����ġ��
	void Play_SSword_Elbow(); //s1
	void Play_SSword_HelmetBreak(); //sp1
	void Play_SSword_CriticalDraw(); //sp2
	void Play_SSword_Combo_StepPierce(); //sp3 st,loop, end
	void Play_SSword_Combo_Strong(); //s1 0.35 s2 0.65 s3 0.9 �Ȳ�ġ, ��������Ⱦ����, ��� ���
	void Play_SSword_Combo_Diagonal_L(); //n2 n3

	void Play_Idle();
	void Play_Stand();
	void Play_Crouch();
	void Play_Sit();

	void Play_Run();
	void Play_Walk();
	void Play_Dodge();

	void Play_Hit(); //�ǰ� ����,�������� ���� �پ��� ���
	void Play_CC(); //����,�Ѿ��� ���� �پ��� �ִϵ� ����
	void Play_Dead();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT Ready_Weapon();
	HRESULT Ready_Collider();
	HRESULT Ready_BoneMatrix();

public:
	static CHunter* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	//CMonsterUI*			m_pMonsterUI = nullptr;

	//_mat*				m_matHeadBone_for_YM;

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CNavMesh*			m_pNavMesh = nullptr;
	CCollider*			m_pCollider = nullptr;
	CWeapon*			m_pWeapon = nullptr;
	CMonsterUI*			m_pMonsterUI = nullptr;

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

	MONSTER_ANITYPE		m_eFirstCategory; //��з�
	HUNTER_IDLETYPE		m_eSecondCategory_IDLE; //�ߺз�
	HUNTER_MOVETYPE		m_eSecondCategory_MOVE;
	HUNTER_ATKTYPE		m_eSecondCategory_ATK;
	HUNTER_HITTYPE		m_eSecondCategory_HIT;
	HUNTER_CCTYPE		m_eSecondCategory_CC;
	HUNTER_DEADTYPE		m_eSecondCategory_DEAD;

	ATK_COMBO_TYPE		m_eAtkCombo;

	WEAPON_ANITYPE		m_eWeaponState = WEAPON_ANITYPE::SSWORD; //�ش� ���� ���� ��� �ִϵ��� �ٲ�� �������� �սô�
	HUNTER_ANI			m_eState;

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

	//��Ÿ���� ��� shot������ �̳������� ���Ÿ� ��������
	//fatkrange ������ ���� ��� kick,r,cqc �� �ϳ��� �����ϰ� �����Ѵ�
	_float				m_fRecognitionRange = 20.f;
	_float				m_fShotRange = 10.f;
	_float				m_fAtkRange = 5.f;
	_float				m_fCoolDownMax = 0.f;
	_float				m_fCoolDownCur = 0.f;
	_float				m_fSpeedForCollisionPush = 2.f;

	_int				m_iRandom = 0;
	_int				m_iDodgeCountMax = 3; //3ȸ �ǰݽ� ȸ��
	_int				m_iDodgeCount = 0; //nȸ �ǰݽ� ȸ��

};

END