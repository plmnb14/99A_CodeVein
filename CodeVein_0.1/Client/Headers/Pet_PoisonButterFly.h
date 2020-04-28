#pragma once

#include "Pet.h"

BEGIN(Client)

class CPet_PoisonButterFly final : public CPet
{
public:
	enum ATK_NORMAL_TYPE
	{
		NORMAL_SHOT,
		NORMAL_MIST,
		NORMAL_RUSh,
		NORMAL_HEAL,
	};

	enum ATK_COMBO_TYPE
	{
		COMBO_R,
		COMBO_L
	};


	enum PET_POISIONBUTTERFLY_ANI
	{
		Walk_R,
		Walk_L,
		Walk_F,
		Walk_B,

		Turn_R90,
		Turn_L90,
		Idle,

		Down_P_Start,
		Down_P_Loop,
		Down_P_End,

		Dodge,
		Deformation,
		Death,
		Dmg_B,
		Dmg_F,
		Appearance_Loop,
		Appearance,
		AtkRush,
		AtkRotation,
		AtkkRightCombo04,
		AtkkRightCombo03,
		AtkkRightCombo02,
		AtkkRightCombo01,
		AtkPoisonShot,
		AtkPoisonMine,
		AtkPoisonBreath,
		AttkLeftCombo02,
		AtkLeftCombo01,
		AtkAllRangeShoot,
		Atk5wayShoot
	};

	enum BONE_TYPE
	{
		Bone_Range,
		Bone_Head,
		Bone_Body,
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
	virtual HRESULT Render_GameObject_SetPass(CShader * pShader, _int iPass) override;

	virtual void Update_Collider() override;
	virtual void Render_Collider() override;
	virtual void Check_CollisionEvent() override;
	virtual void Check_CollisionPush() override;
	virtual void Check_CollisionHit(list<CGameObject*> plistGameObject) override;

private:
	void Check_Hit();
	void Check_Dist();
	void Check_Target();
	void Check_AniEvent();
	void Check_DeadEffect(_double TimeDelta);

private:
	void Play_RandomAtkNormal(); //노말 공격
	void Play_RandomAtkCombo(); //콤보공격
	void Play_Get(); //아이탬 획득
	void Play_Aggro(); //아주 머나먼 미래에 사용할 수도 있으나 못할듯
	void Play_Shot();
	void Play_Mist();
	void Play_Rush();
	void Play_Heal();

protected:
	virtual void Play_Idle() override;
	virtual void Play_Move() override;
	virtual void Play_Hit() override;
	virtual void Play_CC() override;
	virtual void Play_Dead() override;

protected:
	virtual HRESULT Add_Component(void * pArg) override;
	virtual HRESULT SetUp_ConstantTable() override;
	virtual HRESULT Ready_Status(void * pArg) override;
	virtual HRESULT Ready_Weapon(void * pArg) override;
	virtual HRESULT Ready_Collider(void * pArg) override;
	virtual HRESULT Ready_BoneMatrix(void * pArg) override;

public:
	static CPet_PoisonButterFly* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	_mat*	m_matBone[Bone_End];
	ATK_COMBO_TYPE				m_eAtkCombo;
	PET_POISIONBUTTERFLY_ANI	m_eState;

};

END
