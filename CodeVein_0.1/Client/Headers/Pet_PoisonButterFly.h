#pragma once

#include "Pet.h"

BEGIN(Client)

class CPet_PoisonButterFly :public CPet
{
public:
	enum PET_POISIONBUTTERFLY_ANI
	{

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

protected:
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

protected:
	void Function_Find_Target();

protected:
	void Check_PosY();
	void Check_Hit();
	void Check_Dist();
	void Check_AniEvent();
	void Check_DeadEffect(_double TimeDelta);

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
	PET_POISIONBUTTERFLY_ANI m_eState;

};

END
