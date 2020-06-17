#pragma once

#include "Monster.h"

BEGIN(Client)

class CYetiBullet;

class CYeti final : public CMonster
{
public:
	enum ATK_NORMAL_TYPE
	{
		NORMAL_RUSH,
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
		COMBO_RLRL_SHOULDER,
		COMBO_RLRL_SMASH,
		COMBO_RLRL_SWING
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

	enum BONE_TYPE 
	{ 
		Bone_Range, 
		Bone_Head, 
		Bone_Body, 
		Bone_LeftHand, 
		Bone_RightHand, 
		Bone_Shoulder, 
		Bone_End
	};

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
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);
	virtual HRESULT Render_GameObject_SetPass(CShader* pShader, _int iPass, _bool _bIsForMotionBlur = false);

private:
	void Update_Collider();
	void Render_Collider();

	void Check_PosY();
	void Check_Hit();
	void Check_Dist();
	void Check_AniEvent();
	void Check_DeadEffect(_double TimeDelta);
	void Check_FootSound();

	void Play_SequenceAtk();
	void Play_RandomAtkNormal();
	void Play_RandomAtkCombo();

	void Play_BodyPress();
	void Play_Howling();
	void Play_SlowLR();
	void Play_RUpperChop();
	void Play_LRSweap();
	void Play_FastLR();
	void Play_WoodChop();
	void Play_RLRL();
	void Play_IceThrowing();
	void Play_RollingSlash();
	void Play_Rush();
	void Play_Combo_RLRL_Shoulder();
	void Play_Combo_RLRL_Smash();
	void Play_Combo_RLRL_Swing();

	void Play_Idle();
	void Play_Move();
	void Play_Hit(); 
	void Play_CC(); 
	void Play_Dead();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(CShader* pShader);
	HRESULT Ready_Status(void* pArg);
	HRESULT Ready_Collider();
	HRESULT Ready_BoneMatrix();
	HRESULT Ready_Rigid();

public:
	static CYeti* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	_mat*					m_matBone[Bone_End];
	ATK_COMBO_TYPE			m_eAtkCombo;
	YETI_ANI				m_eState;
	_float					m_fFootSound = 0.f;
	_float					m_fFootSoundMax = 0.f;

};

END

