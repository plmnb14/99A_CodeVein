#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CDrain_Weapon;
class CWeapon;
class CPlayer_Colleague final : public CGameObject
{
public:
	// ���� Attack�� ��ų�� �߰��� �� enum���� ������� ��
	enum Colleague_MoveType { Coll_Idle, Coll_Move, Coll_Guard, Coll_Attack, Coll_Hit, Coll_Dead };
	enum Coll_Movement { Move_Walk, Move_Run, Move_Dodge };
	enum Coll_IdleMoment { Idle_Waiting, Idle_Guard };
	enum Coll_Attackmoment { Att_waiting, Att_Normal, Att_Skill };
	enum Coll_Guardmoment { Guard_Idle, Gurad_Walk, Gurad_Hit };
	
	//enum Move_Direction { Move_Front, Move_Back, Move_Left, Move_Right, Move_End };

private:
	enum Bonematrix_Type { Bone_Range, Bone_Body, Bone_Head, Bone_End };

private:
	enum Colleague_Ani
	{
		Ani_Idle,
		Ani_PlayerDead,
		Ani_Trun_Left90,
		Ani_Front_Walk,
		Ani_Back_Walk,
		Ani_Front_Run,
		Ani_Back_Run,
		Ani_Start_Gurad,
		Ani_Loop_Gruad,
		Ani_End_Gruad,
		Ani_Hit_Gruad,
		Ani_Walk_Gruad,
		Ani_Front_Hit,
		Ani_Back_Hit,
		Ani_Front_Roll,
		Ani_Back_Roll,
		One_Att,
		Two_Att,
		Three_Att,
		Four_Att,
		Ani_Dead,
		Ani_Heal,
		Ani_PlayerHeal,
		Ani_ThreeAtt_Skil,
		Ani_CenterAtt_Skil
	};

protected:
	explicit CPlayer_Colleague(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CPlayer_Colleague(const CPlayer_Colleague& rhs);
	virtual ~CPlayer_Colleague() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT LateInit_GameObject();
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT	SetUp_Default();
	HRESULT SetUp_ConstantTable();

private:
	HRESULT	Ready_BoneMatrix();
	HRESULT	Ready_Collider();
	HRESULT	Ready_Weapon();

private:
	void	Set_Length(_float Length) { m_fAll_Length = Length; }

private:
	void	Update_Collider();
	void	Render_Collider();

private:
	void	Check_Do_List();
	void	Set_AniEvent();

private:
	void	Colleague_Movement(_float fSpeed, _v3 vDir/*, _v3 YDir*/);

private:
	void	Colleague_Dead();

	void	Colleague_Guard();

	void	Colleague_Hit();

private:
	void	CollMove_Walk();
	void	CollMove_Run();
	void	CollMove_Dodge();	// ������ or ����

	void	CollIdle_Waiting();

	void	CollAtt_Waiting();
	void	CollAtt_Normal();

private:
	void	Funtion_RotateBody();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CMesh_Dynamic*			m_pDynamicMesh = nullptr;
	CNavMesh*				m_pNavMesh = nullptr;
	CCollider*				m_pCollider = nullptr;

	CWeapon*				m_pSword = nullptr;
	CTransform*				m_pTargetTransformCom = nullptr;

	CGameObject*			m_pObject_Mon = nullptr;

private:
	Colleague_MoveType		m_eMovetype;

	Coll_Movement			m_eColl_Movement;
	Coll_IdleMoment			m_eColl_IdleMoment;
	Coll_Attackmoment		m_eColl_AttackMoment;
	Coll_Guardmoment		m_eColl_GuardMoment;

	Colleague_Ani			m_eColleague_Ani;

	//Move_Direction			m_eMoveDirection;

private:
	list<CGameObject*>*	m_List_pMonTarget[2];

	_double	m_dPlayAni_Time = 1;

	_mat*	m_matBone[Bone_End];

	_float	m_fSpeed = 0.f;
	_float	m_fAll_Length = 0.f;

	_float	m_fCoolTimer = 3.f;

	_bool	m_bNear_byMonster = false;
	_bool	m_bStart_Fighting = false;
	_bool	m_bMonExistence = false; // ���� ���� ����

public:
	static	CPlayer_Colleague* Create(_Device pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);

public:
	virtual void Free();


};

END