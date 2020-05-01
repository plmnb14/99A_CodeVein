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
	// 차후 Attack에 스킬을 추가할 시 enum으로 나눠줘야 함
	enum Colleague_Type { Coll_Idle, Coll_Move, Coll_Guard, Coll_Attack, Coll_Hit, Coll_Dodge, Coll_Heal, Coll_Dead };
	enum Coll_Movement { Move_Walk, Move_BackWalk, Move_Run, Move_BackRun, Move_MonWalk, Move_MonRun };
	enum Coll_IdleMoment { Idle_Waiting, Idle_Guard };
	enum Coll_AttackMoment { Att_Skil, Att_Normal };
	enum Coll_Sub_AttMoment { Att_Base1, Att_Base2, Att_Base3, Att_Base4, Att_ThreeCombo, Att_CenterDown, Att_SlowGun };
	enum Coll_GuardMoment { Guard_Idle, Gurad_Walk, Gurad_Hit };
	enum Coll_DodgeMoment { Dodge_FrontRoll, Dodge_BackRoll };
	enum Coll_HealMoment { My_Heal, Player_Heal };

	enum Coll_FBLR { Coll_Front, Coll_Back };

private:
	enum Bonematrix_Type { Bone_Range, Bone_Body, Bone_Head, Bone_LHand, Bone_End };

private:
	enum Colleague_Ani
	{
		Ani_Start_Game,
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
		Ani_FrontWalk_Gruad,
		Ani_BackWalk_Gruad,
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
		Ani_PlayerHeal_or_Gun,
		Ani_Trun_Center_Att_Skil,
		Ani_Jump_CenterAtt_Skil
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
	void	Check_DeadEffect(_double TimeDelta);

	void	Check_Do_List();
	void	Check_MyHit();

	void	Set_AniEvent();

private:
	void	Colleague_Movement(_float fSpeed, _v3 vDir);
	void	Colleague_SkilMovement(_float Multiply);

private:
	void	Play_Dead();
	void	Play_Hit();
	void	Colleague_Guard();

private:
	void	CollMove_Walk();
	void	CollMove_BackWalk();
	void	CollMove_Run();
	void	CollMove_BackRun();
	void	CollMove_MonWalk();
	void	CollMove_MonRun();


	void	CollDodge_FrontRoll();	// 구르기 or 막기


	void	CollIdle_Waiting();

	void	CollAtt_Skil();
	void	CollAtt_ThreeCombo();
	void	CollAtt_CenterDown();
	void	CollAtt_SlowGun();

	void	CollAtt_Normal();
	void	CollAtt_Base1();
	void	CollAtt_Base2();
	void	CollAtt_Base3();
	void	CollAtt_Base4();


	void	CollGuard_Idle();
	void	CollGuard_Walk();
	void	CollGuard_Hit();

	void	CollHeal_ForMe();
	void	CollHeal_ForPlayer();

private:
	void	Funtion_RotateBody();
	void	Funtion_Reset_State();
	
	void	Enter_Collision();
	void	Check_Collision_PushOut();
	void	Check_Collision_Event(list<CGameObject*> plistGameObject);

	void	Function_CoolTIme();
	_bool	Function_Checking_AttCoolTime(_float fTImer);

	void	Function_FBRL();


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
	Colleague_Type			m_eMovetype;

	Coll_Movement			m_eColl_MoveMent;
	Coll_IdleMoment			m_eColl_IdleMoment;
	Coll_AttackMoment		m_eColl_AttackMoment;
	Coll_Sub_AttMoment		m_eColl_Sub_AttMoment;
	Coll_GuardMoment		m_eColl_GuardMoment;
	Coll_DodgeMoment		m_eColl_DodgeMoment;
	Coll_HealMoment			m_eColl_HealMoment;

	Colleague_Ani			m_eColleague_Ani;

	Coll_FBLR				m_eFBLR;

	//Move_Direction			m_eMoveDirection;

private:
	list<CGameObject*>*	m_List_pMonTarget[2];

	_mat*	m_matBone[Bone_End];


	_double	m_dPlayAni_Time = 1;
	_double m_dTimeDelta = 0.f;


	_uint	m_iNormalAtt_Count = 0;
	_uint	m_iDodgeCount = 0;
	_uint	m_iDodgeCountMax = 5;

	_uint	m_iMyHeal_Count = 4;


	_float	m_fSpeed = 0.f;
	_float	m_fAll_Length = 0.f;

	_float	m_fAtt_MoveSpeed_Cur = 0.f;
	_float	m_fAtt_MoveSpeed_Max = 0.f;
	_float	m_fAtt_MoveAccel_Cur = 0.5f;
	_float	m_fAtt_MoveAccel_Max = 0.f;
	_float	m_fAni_Multiply = 0.f;

	_float	m_fCoolTimerCenter = 5.f;
	_float	m_fCoolTimerThree = 5.f;

	_float	m_fCoolTImer_NomalAtt = 0.f;
	_float	m_fCoolTimer_limit = 0.f;

	_float	m_fMonDistance_Compare = 4.5f;

	_float	m_fCoolTime_MyHeal = 10.f;
	_float	m_fCoolTime_PlayerHeal = 10.f;
	_float	m_fCoolTime_Trun_Center_Att_Skil = 15.f;

	_float	m_fCoolTime_Cur = 0.f;
	_float	m_fCoolTime_Max = 0.f;


	_bool	m_bEventTrigger[20] = {};

	_bool	m_bNear_byMonster = false;
	_bool	m_bStart_Fighting = false;
	_bool	m_bMonExistence = false; // 몬스터 존재 유무
	_bool	m_bLook_Monster = false;
	_bool	m_bMonDead = false;

	_bool	m_bNot_AttcaingMon = false;
	_bool	m_bMyHiting = false;
	_bool	m_bNot_MoveAtt = false;

	_bool	m_bBase_Att[4] = {};		// 안 쓸 가능성 있음
	_bool	m_bCheck_Attcing = false;

	_bool	m_bStart_Attacting = false;

	_bool	m_bNest_Att_CoolTimer = false;
	_bool	m_bChecking_MyHit = false;

	_bool	m_bEnd_Attacting = false;

	_bool	m_bCheck_Distance = false;

	_bool	m_bCanCoolDown = false;
	_bool	m_bIsCoolDown = false;
	_bool	m_bAvailable_Skil = false;
	_bool	m_bTestRendom = false;

	_bool	m_bColleagueDead = false;
	_bool	m_bEnable = false;

	_bool	m_bCheck_HealMyHp = false;


private: // For Effect
	_float	m_fDeadEffect_Delay = 0.f;
	_float	m_fDeadEffect_Offset = 0.f;


public:
	static	CPlayer_Colleague* Create(_Device pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);

public:
	virtual void Free();


};

END