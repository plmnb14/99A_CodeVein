#pragma once

#include "Monster.h"

BEGIN(Client)

class CMonkey final : public CMonster
{
public:
	enum ATK_NORMAL_TYPE 
	{
		NORMAL_ATK_ROTBODY,
		NORMAL_RDIGONAL,
		NORMAL_JUMPDOWN,
		NORMAL_JUMPLHAND,
		NORMAL_JUMP_ROTBODY,
		NORMAL_FANGSHOOT 
	};

	enum ATK_COMBO_TYPE 
	{ 
		COMBO_NORMAL,
		COMBO_JUMP_CLOCK,
		COMBO_RUNATK
	};

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

	enum BONE_TYPE 
	{ 
		Bone_Range,
		Bone_Head,
		Bone_Body,
		Bone_LeftHand,
		Bone_End
	};

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
	virtual HRESULT Render_GameObject_SetPass(CShader* pShader, _int iPass, _bool _bIsForMotionBlur = false);

protected:
	void Update_Collider();
	void Render_Collider();

	void Check_PosY();
	void Check_Hit();
	void Check_Dist();
	void Check_AniEvent();
	void Check_DeadEffect(_double TimeDelta);

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
	_mat*					m_matBone[Bone_End];
	ATK_COMBO_TYPE			m_eAtkCombo;
	MONKEY_ANI				m_eState;

};

END

