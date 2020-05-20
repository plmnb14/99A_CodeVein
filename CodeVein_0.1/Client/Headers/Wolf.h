#pragma once

#include "Monster.h"

BEGIN(Client)

class CWolf final : public CMonster
{
public:
	enum WOLF_ANI
	{
		Idle,
		Threat,
		NF_Sit,
		NF_Sit_End,
		NF_Eat,
		NF_Eat_End,

		Run,
		Walk,
		Dodge,

		Down_S_Start,
		Down_S_Loop,
		Down_S_End,

		Down_P_Start,
		Down_P_Loop,
		Down_P_End,

		Dmg_F,
		Dmg_B,

		Death_B,
		Death,

		Atk_Third,
		Atk_Jump_R90,
		Atk_Jump_L90,
		Atk_Jump,
	};
	
	enum BONE_TYPE 
	{ 
		Bone_Range,
		Bone_Body,
		Bone_Head,
		Bone_End
	};

protected:
	explicit CWolf(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CWolf(const CWolf& rhs);
	virtual ~CWolf() = default;

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

	void Play_Bite_LRL();
	void Play_RDodgeAtk();
	void Play_LDodgeAtk();
	void Play_Frisbee();

	void Play_Idle();
	void Play_Move();
	void Play_Hit();
	void Play_CC();
	void Play_Dead();

private:
	HRESULT Add_Component(void * pArg);
	HRESULT SetUp_ConstantTable(CShader* pShader);
	HRESULT Ready_Status(void* pArg);
	HRESULT Ready_Collider();
	HRESULT Ready_BoneMatrix();
	HRESULT Ready_Rigid();

public:
	static CWolf* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	_mat*					m_matBone[Bone_End];
	WOLF_ANI				m_eState;

};

END