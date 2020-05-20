#pragma once

#include "Monster.h"

BEGIN(Client)

class CUrchin final : public CMonster
{
public:
	enum URCHIN_ANI 
	{
		Idle,
		Run,
		Dmg,
		Death,
		Atk_Step,
		Atk_Rush
	};

	enum BONE_TYPE 
	{ 
		Bone_Range,
		Bone_Body,
		Bone_Head,
		Bone_End
	};

protected:
	explicit CUrchin(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CUrchin(const CUrchin& rhs);
	virtual ~CUrchin() = default;

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

	void Play_Rolling();
	void Play_RollingRush();

	void Play_Idle();
	void Play_Move();
	void Play_CC();
	void Play_Hit();
	void Play_Dead();

private:
	HRESULT Add_Component(void * pArg);
	HRESULT SetUp_ConstantTable(CShader* pShader);
	HRESULT Ready_Status(void* pArg);
	HRESULT Ready_Collider();
	HRESULT Ready_BoneMatrix();
	HRESULT Ready_Rigid();

public:
	static CUrchin* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	_mat*					m_matBone[Bone_End];
	URCHIN_ANI				m_eState;

};

END