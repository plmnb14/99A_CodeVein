#pragma once

#include "Monster.h"

BEGIN(Client)

class CCocoon final : public CMonster
{
public:
	enum COCOON_ANI
	{ 
		Idle,
		Threat,
		Dead,
		Dmg_B,
		Dmg_F,
		Shot,
		Mist
	};

	enum BONE_TYPE
	{ 
		Bone_Range, 
		Bone_Body, 
		Bone_Head, 
		Bone_Neck, 
		Bone_Jaw_Tongue, 
		Bone_End
	};

protected:
	explicit CCocoon(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCocoon(const CCocoon& rhs);
	virtual ~CCocoon() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_SetPass(CShader* pShader, _int iPass, _bool _bIsForMotionBlur = false);

private:
	void Update_Collider();
	void Render_Collider();

	void Check_PosY();
	void Check_Hit();
	void Check_Dist();
	void Check_AniEvent();
	void Check_DeadEffect(_double TimeDelta);

	void Play_Shot();
	void Play_Mist();

	void Play_Idle();
	void Play_Hit();
	void Play_Dead();

private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();
	HRESULT Ready_Status(void* pArg);
	HRESULT Ready_Collider();
	HRESULT Ready_BoneMatrix();

public:
	static CCocoon* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	_mat*					m_matBone[Bone_End];
	COCOON_ANI				m_eState;

};

END

