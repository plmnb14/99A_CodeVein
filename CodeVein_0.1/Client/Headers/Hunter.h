#pragma once

#include "Monster.h"

BEGIN(Client)

class CHunterBullet;

class CHunter final : public CMonster
{
public:
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
		DmgRepel, //���

		Down_S_End,
		Down_S_Loop,
		Down_S_Start, //�� �Ѿ���

		Down_P_End,
		Down_P_Loop,
		Down_P_Start, //�� �Ѿ���

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
		///////////////////////////////////
		KetsugiGroundRange,
		KetsugiEnchant,
		KetsugiBuffSelf,	//�̻��
		KetsugiAllRangeShoot,
		KetsugiAimShoot,
		///////////////////////////////////
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

	enum BONE_TYPE 
	{ 
		Bone_Range,
		Bone_Head,
		Bone_Body,
		Bone_RightForeArm,
		Bone_LeftKnee,
		Bone_LeftToe,
		Bone_RightHandAttach,
		Bone_End
	};

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

	void Check_PosY();
	void Check_Hit();
	void Check_Dist();
	void Check_AniEvent();
	void Check_DeadEffect(_double TimeDelta);

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
	void Play_SSword_Combo_StepPierce(); //sp3 st,loop, end 0.935 0.9 0.915
	void Play_SSword_Combo_Strong(); //s1 0.308 s2 0.65 s3 0.9 �Ȳ�ġ, ��������Ⱦ����, ��� ���
	void Play_SSword_Combo_Diagonal_L(); //n2 n3 0.5 0.9

	void Play_Idle();
	void Play_Move(); //�޸���, �ȱ� ���⺰��, ȸ��
	void Play_Hit(); //�ǰ� ����,�������� ���� �پ��� ���
	void Play_CC(); //����,�Ѿ��� ���� �پ��� �ִϵ� ����->�̋� ��Ʈ �������� �Ѿ�� �ʰ� �������� ����
	void Play_Dead(); //cc���¿��� ���� ��� �پ��� ��� ����

private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();
	HRESULT Ready_Status(void* pArg);
	HRESULT Ready_Weapon();
	HRESULT Ready_Collider();
	HRESULT Ready_BoneMatrix();

public:
	static CHunter* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	_mat*					m_matBone[Bone_End];
	ATK_COMBO_TYPE			m_eAtkCombo;
	HUNTER_ANI				m_eState;

};

END