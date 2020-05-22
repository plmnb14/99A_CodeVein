#pragma once

#include "Pet.h"

BEGIN(Client)

class CPet_PoisonButterFly final : public CPet
{
public:
	enum ATK_NORMAL_TYPE
	{
		NORMAL_5SHOT,
		NORMAL_POISONWHEELWIND,
	};

	enum PET_POISIONBUTTERFLY_ANI
	{
		Idle,
		Deformation,
		Appearance,
		Appearance_End,

		Walk_R,
		Walk_L,
		Walk_F,
		Walk_B,

		Dodge,
		Down_Start,
		Down_Loop,
		Down_End,

		Dmg_F,
		Dmg_B,

		Death,

		Atk_Rush,
		Atk_Rotation,
		Atk_R04,
		Atk_R03,
		Atk_R02,
		Atk_R01,
		Atk_PoisonMist,
		Atk_PoisonMine,
		Atk_PoisonBreath,
		Atk_L02,
		Atk_L01,
		Atk_AllRangeShoot,
		Atk_5wayShoot,
	};

	enum BONE_TYPE
	{
		Bone_Range,
		Bone_Head,
		Bone_Body,
		Bone_Tail,
		Bone_Tail6,
		Bone_End
	};

protected:
	explicit CPet_PoisonButterFly(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CPet_PoisonButterFly(const CPet_PoisonButterFly& rhs);
	virtual ~CPet_PoisonButterFly() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg) override;
	virtual _int Update_GameObject(_double TimeDelta) override;
	virtual _int Late_Update_GameObject(_double TimeDelta) override;
	virtual HRESULT Render_GameObject() override;
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader) override;
	virtual HRESULT Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur =false) override;
	virtual void Play_Deformation() override;

private:
	void Update_Collider();

	void Check_Dist();
	void Check_Action();
	void Check_AniEvent();
	void Check_DeadEffect(_double TimeDelta);

	void Play_5Shot(); // Atk_5wayShoot
	void Play_PoisonWheelWind(); //atk_poisonmine

protected:
	virtual void Play_Idle() override;
	virtual void Play_Move() override;
	virtual void Play_CC() override;
	virtual void Play_Hit() override;
	virtual void Play_Dead() override;

protected:
	virtual HRESULT Add_Component(void * pArg) override;
	virtual HRESULT SetUp_ConstantTable(CShader* pShader) override;
	virtual HRESULT Ready_Status(void * pArg) override;
	virtual HRESULT Ready_Weapon(void * pArg) override;
	virtual HRESULT Ready_Collider(void * pArg) override;
	virtual HRESULT Ready_BoneMatrix(void * pArg) override;

public:
	static CPet_PoisonButterFly* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	_mat*						m_matBone[Bone_End];
	PET_POISIONBUTTERFLY_ANI	m_eState;

};

END
