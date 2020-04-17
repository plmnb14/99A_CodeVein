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

	enum WEAPON_ANITYPE	 {GUN, HALBERD, HAMMER, LSWORD, SWORD, WEAPON_ANITYPE_END};

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

		NORMAL_HALBERD_STEPPIERCE,
		NORMAL_HALBERD_RISEUP,
		NORMAL_HALBERD_PIERCE,
		NORMAL_HALBERD_DEEPPIERCE,
		NORMAL_HALBERD_CLOCKTWICE,
		NORMAL_HALBERD_SWING_JUMP,
		NORMAL_HALBERD_SWEAP,
		NORMAL_HALBERD_SLASHFORTH,
		NORMAL_HALBERD_TWOUPPER,

		NORMAL_HAMMER_UPPER,
		NORMAL_HAMMER_SLASH,
		NORMAL_HAMMER_SMASH,
		NORMAL_HAMMER_TWOUPPER,

		NORMAL_LSWORD_KNEEKICK,
		NORMAL_LSWORD_RIGHT,
		NORMAL_LSWORD_RDIAGONAL,
		NORMAL_LSWORD_SMASH,
		//NORMAL_LSWORD_SWING, //���

		NORMAL_SWORD_JUMP,
		NORMAL_SWORD_RAISEUP,
		NORMAL_SWORD_UPPER,
		NORMAL_SWORD_Upper_L,
		NORMAL_SWORD_WOODCHOP,
		NORMAL_SWORD_ELBOW,
		NORMAL_SWORD_HELMETBREAK,
		NORMAL_SWORD_CRITICALDRAW

	};
	enum ATK_COMBO_TYPE 
	{
		COMBO_GUN_SHOT, COMBO_GUN_CQC,
		COMBO_HALBERD_THIRDATK, COMBO_HALBERD_PIERCETWICE, COMBO_HALBERD_PIERCEWIND,
		COMBO_LSWORD_NORMAL, COMBO_LSWORD_STRONG,
		COMBO_SWORD_STEPPIERCE, COMBO_SWORD_STRONG, COMBO_SWORD_Diagonal_L

	};

	enum HUNTER_ANI
	{
		Idle,
		Bayonet_Idle,
		Halberd_Idle,
		Hammer_Idle,
		LSword_Idle,
		Sword_Idle,
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

		Down_S_End,
		Down_S_Loop,
		Down_S_Start, //�ں���

		Down_P_End,
		Down_P_Loop,
		Down_P_Start, //������

		Dmg02_FR, //�� ������ ���ƿ�
		Dmg02_FL, //���� ����
		Dmg02_BR,
		Dmg02_BL,
		Dmg01_FR, //������ ���ƿ�
		Dmg01_FL, //������ ����
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

	enum FBLR { FRONT, FRONTLEFT, FRONTRIGHT, BACK, BACKLEFT, BACKRIGHT, LEFT, RIGHT };

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
	void Check_AniEvent();

	void Play_RandomAtkNormal();
	void Play_RandomAtkCombo();

	void Play_Gun_Kick();
	void Play_Gun_R(); //n1
	void Play_Gun_Shoot(); 
	void Play_Gun_Snipe();
	void Play_Gun_Combo_Shot(); //���Ÿ� 3���� 0.3 0.62 0.9
	void Play_Gun_Combo_CQC(); //n1,2,3 0.3 0.65 0.9

	void Play_Halberd_StepPierce();
	void Play_Halberd_RiseUp();
	void Play_Halberd_Pierce(); //n01 0.9
	void Play_Halberd_DeepPierce(); //������� �s01 0.9
	void Play_Halberd_ClockTwice(); //2ȸ�� sp2 0.95
	void Play_Halberd_Swing_Jump(); //�ֵθ��� �����ؼ� ����ġ�� sp3 0.95
	void Play_Halberd_Sweap(); //5Ÿ�� 0.95
	void Play_Halberd_SlashForth(); //4Ÿ�� 0.95
	void Play_Halberd_TwoUpper(); //��,�� ���� 1ȸ�� 0.95
	void Play_Halberd_Combo_ThirdAtk(); //�Ǿ,Ⱦ����,�ֵѰ��� n1 0.6 n2 0.6 n3 0.95
	void Play_Halberd_Combo_PierceTwice(); //���,�Ѽ� ��� s1 0.5 s3 0.9
	void Play_Halberd_Combo_PierceWind(); //��� �ֵθ��� s1 0.4 s2 0.9

	void Play_Hammer_Upper(); //n
	void Play_Hammer_Slash(); //slash
	void Play_Hammer_Smash(); //smash
	void Play_Hammer_TwoUpper(); //twoupper

	void Play_LSword_KneeKick(); //0.95
	void Play_LSword_Right(); //n1 0.9
	void Play_LSword_RDiagonal(); //s1 0.9
	void Play_LSword_Smash(); //smash 0.95
	void Play_LSword_Combo_Normal(); //��,��,�ֵθ��� 0.6 0.55 0.95 n1,2,3
	void Play_LSword_Combo_Strong(); //��밢, ���, �������� 0.45 0.45 0.95 s1,2,3
	
	void Play_SSword_Jump(); //0.95
	void Play_SSword_RaiseUp(); //0.95
	void Play_SSword_Upper(); //N1 0.9
	void Play_SSword_Upper_L(); //n4 0.9
	void Play_SSword_WoodChop();//n5 ����ġ�� 0.95
	void Play_SSword_Elbow(); //s1 0.95
	void Play_SSword_HelmetBreak(); //sp1 0.95
	void Play_SSword_CriticalDraw(); //sp2 0.95
	void Play_SSword_Combo_StepPierce(); //sp3 0.9 0.9 0.9
	void Play_SSword_Combo_Strong(); //s1 0.35 s2 0.65 s3 0.95 �Ȳ�ġ, ��������Ⱦ����, ��� ���
	void Play_SSword_Combo_Diagonal_L(); //n2 n3 0.5 0.9

	void Play_Idle();
	void Play_Move(); //�޸���, �ȱ� ���⺰��, ȸ��
	void Play_Hit(); //�ǰ� ����,�������� ���� �پ��� ���
	void Play_CC(); //����,�Ѿ��� ���� �پ��� �ִϵ� ����->�̋� ��Ʈ �������� �Ѿ�� �ʰ� �������� ����
	void Play_Dead(); //cc���¿��� ���� ��� �پ��� ��� ����

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

	MONSTER_ANITYPE		m_eFirstCategory; //��з�
	HUNTER_IDLETYPE		m_eSecondCategory_IDLE; //�ߺз�
	HUNTER_MOVETYPE		m_eSecondCategory_MOVE;
	HUNTER_ATKTYPE		m_eSecondCategory_ATK;
	HUNTER_HITTYPE		m_eSecondCategory_HIT;
	HUNTER_CCTYPE		m_eSecondCategory_CC;
	HUNTER_DEADTYPE		m_eSecondCategory_DEAD;

	WEAPON_ANITYPE		m_eWeaponState = WEAPON_ANITYPE::HALBERD;
	ATK_COMBO_TYPE		m_eAtkCombo;
	HUNTER_ANI			m_eState;
	FBLR				m_eFBLR;

	_bool				m_bEventTrigger[20] = {};

	_bool				m_bCanPlayDead = false;
	_bool				m_bCanDissolve = false;

	_bool				m_bInRecognitionRange = false;
	_bool				m_bInAtkRange = false;

	_bool				m_bCanChase = false;

	_bool				m_bCanCoolDown = false;
	_bool				m_bIsCoolDown = false;

	_bool				m_bCanAtkCategoryRandom = true;
	_bool				m_bIsCombo = false;
	_bool				m_bCanComboInterrupt = true; 	//�׻�True �޺� ������ Ư�� �ִϱ������� true�� �༭ 
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