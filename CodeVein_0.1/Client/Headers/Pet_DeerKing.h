#pragma once

#include "Pet.h"

BEGIN(Client)

class CPet_DeerKing final : public CPet
{
public:
	enum ATK_NORMAL_TYPE
	{
		NORMAL_CHOP,
		NORMAL_SNOWQUAKE,
		NORMAL_COMBO
	};

	enum PET_DEERKING_ANI
	{
		Idle,
		Shield_Deformation,
		Shield_Appearance,
		Shield_Appearance_End,

		Run,
		Walk_R,
		Walk_L,
		Walk_F,
		Walk_B,

		Dodge_R,
		Dodge_L,
		Dodge,

		Down,
		Down_Start,
		Down_Loop,
		Down_End,

		Dmg_FR,
		Dmg_FL,
		Dmg_BR,
		Dmg_BL,

		Death,

		Ketsugi_AimShoot,
		Shield_Atk_Slide_Start,
		Shield_Atk_Slide_Loop,
		Shield_Atk_Slide_End,

		Shield_Atk_N04,
		Shield_Atk_N03,
		Shield_Atk_N02,
		Shield_Atk_N01,

		Shield_Atk_Jump,
		Shield_Atk_Horn02,
		Shield_Atk_Horn01,

		Ketsugi_IceBlade,

		BoostUp_AllShoot,
		BoostUp_AllShoot_Start,
		BoostUp_AllShoot_Loop,
		BoostUp_AllShoot_End,

		BoostUp_AtkFall,
		BoostUp_AtkFall_Start,
		BoostUp_AtkFall_Loop,
		BoostUp_AtkFall_End,

		Atk_N,

		Atk_Jump_Start,
		Atk_Jump_Loop,
		Atk_Jump_End,

		Atk_Boost_Start,
		Atk_Boost_Loop,
		Atk_Boost_End02,
		Atk_Boost_End01
	};

	enum BONE_TYPE
	{
		Bone_RightHand,
		Bone_LeftHand,
		Bone_End
	};

protected:
	explicit CPet_DeerKing(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CPet_DeerKing(const CPet_DeerKing& rhs);
	virtual ~CPet_DeerKing() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg) override;
	virtual _int Update_GameObject(_double TimeDelta) override;
	virtual _int Late_Update_GameObject(_double TimeDelta) override;
	virtual HRESULT Render_GameObject() override;
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader) override;
	virtual HRESULT Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur = false) override;

private:
	void Update_Collider();
	void Render_Collider();

	void Check_Dist();
	void Check_Action();
	void Check_AniEvent();
	void Check_DeadEffect(_double TimeDelta);

	void Play_Chop();
	void Play_SnowQuake();
	void Play_Combo();

protected:
	virtual void Play_Deformation() override;
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
	static CPet_DeerKing* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	_mat*				m_matBone[Bone_End];
	PET_DEERKING_ANI	m_eState;

};

END


