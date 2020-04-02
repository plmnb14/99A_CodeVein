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
		Guard_Break = 0,
		Guard_Hit_01,
		Guard_Hit_02,
		Guard_Start = 3,
		Guard_Loop = 4,
		Guard_End = 5,
		Idle_State = 42
	}TMP_ANI;

	typedef enum tmp_ActState
	{
		ACT_IDLE, ACT_HIT, ACT_BREAK
	}TMP_ACT;

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
	void Skill_Movement(_float _fspeed, _v3 _vDir = { 0.f , 0.f , 0.f });
	void Decre_Skill_Movement(_float _fMutiply = 1.f);

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
	TMP_ACT		m_eActState = ACT_IDLE;
	_ulong				m_dwHitCnt = 0;
	_float				m_fTimer = 0.f;
	_bool				m_bOnGuard = false;

private:
	_float					m_fSkillMoveSpeed_Cur = 0.f;
	_float					m_fSkillMoveSpeed_Max = 0.f;
	_float					m_fSkillMoveAccel_Cur = 0.5f;
	_float					m_fSkillMoveAccel_Max = 0.f;
	_float					m_fSkillMoveMultiply = 1.f;
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