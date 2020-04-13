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
	enum Colleague_MoveType { Coll_Idle, Coll_Move, Coll_Attack, Coll_Hit, Coll_Dead };
	enum Colleague_Movement { Move_Walk, Move_Run, Move_Dodge };
	enum Move_Direction { Move_Front, Move_Back, Move_Left, Move_Right, Move_End };

private:
	enum Bonematrix_Type { Bone_Range, Bone_Body, Bone_Head, Bone_End };

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
	HRESULT	Ready_BoneMatrix();
	HRESULT	Ready_Collider();
	HRESULT	Ready_Weapon();

private:
	_bool	Checking_Player();

	void	Colleague_Movement(_float fSpeed, _v3 vDir);

	void	ColleagueMove_Walk();
	void	ColleagueMove_Run();
	void	ColleagueMove_Dodge();	// 구르기 or 막기

	void	Funtion_RotateBody();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CMesh_Static*			m_pStaticMesh = nullptr;
	CMesh_Dynamic*			m_pDynamicMesh = nullptr;
	CNavMesh*				m_pNavMesh = nullptr;
	CCollider*				m_pCollider = nullptr;

	CWeapon*				m_pSword = nullptr;
	CTransform*				m_pTargetTransformCom = nullptr;

private:
	Colleague_MoveType		m_eMovetype;
	Move_Direction			m_eMoveDirection;

private:
	_mat*	m_matBone[Bone_End];

	_float	m_fSpeed = 0.f;

	_bool	m_bChecking_With_Player = false;

public:
	static	CPlayer_Colleague* Create(_Device pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);

public:
	virtual void Free();


};

END