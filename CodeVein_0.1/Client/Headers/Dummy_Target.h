#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CDummy_Target final : public CGameObject
{
private:
	typedef enum tmp_Animation
	{
		Guard_Start = 3,
		Guard_Loop = 4,
		Guard_End = 5,
		Idle_State = 42
	}TMP_ANI;

private:
	enum BoneMatrix
	{
		Bone_Range, Bone_Body, Bone_Head, Bone_End
	};

protected:
	explicit CDummy_Target(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CDummy_Target(const CDummy_Target& rhs);
	virtual ~CDummy_Target() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	virtual void Ready_BoneMatrix();
	virtual void Ready_Collider();

private:
	virtual void Draw_Collider();
	virtual void Update_Collder();

public:
	virtual void OnCollisionEnter();
	virtual void OnCollisionEvent_Physic(list<CGameObject*> plistGameObject);

private:
	_mat*					m_matBones[Bone_End];

private:
	CTransform*			m_pTransform = nullptr;
	CRenderer*			m_pRenderer = nullptr;
	CShader*			m_pShader = nullptr;
	CMesh_Dynamic*		m_pDynamic_Mesh = nullptr;

	_double				m_dTimeDelta = 0;

	//======================================================================
	TMP_ANI				m_eTmpAnimNum = Idle_State;
	_float				m_fTimer = 0.f;
	_bool				m_bOnGuard = false;
	//======================================================================

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CDummy_Target* Create(_Device pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END