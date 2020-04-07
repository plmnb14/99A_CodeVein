#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CPoisonButterfly final : public CGameObject
{
public:
	typedef struct tagInitInfo
	{
		tagInitInfo(_float _fFov, _float _fMaxLength, _float _fMinLength)
			: fFov(_fFov), fMaxLength(_fMaxLength), fMinLength(_fMinLength)
		{}

		_float					fFov = 0.f;
		_float					fMaxLength = 0.f;
		_float					fMinLength = 0.f;
	}INFO;

private:
	enum Ani {
		Ani_Idle = 6, Ani_Death = 12, Ani_Dmg01_B = 14, Ani_Appearance = 16
	};

private:
	enum BoneMatrix { Bone_Range, Bone_Body, Bone_Head, Bone_End };

protected:
	explicit CPoisonButterfly(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CPoisonButterfly(const CPoisonButterfly& rhs);
	virtual ~CPoisonButterfly() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:	//패턴들
	// 근거리
	// 1. 왼쪽 냠
	CBT_Composite_Node* Left_Eat();		//충돌완	공격 거리 5
	// 2. 오른쪽 냠
	CBT_Composite_Node* Right_Eat();	//충돌완	공격 거리 5
	// 3. 냠-> 빙그르르냠
	CBT_Composite_Node* Eat_Turn_Eat();	//충돌완 이펙트 수정 바람, 모션 수정함
	// 4. 기모아서 독 소용돌이
	CBT_Composite_Node* Poison_Tornado_After_Charging();	// 공격범위 3 이상
	// 5. 좁은 범위 한바퀴 독 발사
	CBT_Composite_Node* OneTurn_Poison();	//충돌완	공격 거리 3
	// 6. 냠 회전
	CBT_Composite_Node* Eat_Turn();	// 충돌완	이펙트 추가 바람
	// 7. 트린다미어
	CBT_Composite_Node* WhirlWind();	// 충돌완
	// 8. 오른쪽 냠, 왼쪽 냠
	CBT_Composite_Node* Right_Eat_Left_Eat();


	// 원거리
	// 1. 돌진, 이동거리 : 13
	CBT_Composite_Node* Rush();		//충돌완
	// 2. 오발탄
	CBT_Composite_Node* Fire_5Bullet();		//완
	// 3. 저격 샷
	CBT_Composite_Node* Fire_ChaseBullet();	// 발사체 추가 완

	//
	/*
	방향 조절 -> 패턴 선택  -> 근거리 패턴, 추적 후 공격 or 그냥 그 자리에서 공격
							-> 원거리 패턴, 제자리에서 공격
	*/
	CBT_Composite_Node*		Start_Game();

	CBT_Composite_Node*		Dist_Attack();

	CBT_Composite_Node*		ChaseAndNearAttack();
	CBT_Composite_Node*		TurnAndFarAttack();
	CBT_Composite_Node*		Chase();

	CBT_Composite_Node*		NearAttack_Dist3();
	CBT_Composite_Node*		NearAttack_Dist6();

	CBT_Composite_Node*		NearAttack();	//랜덤 공격
	CBT_Composite_Node*		FarAttack();	//랜덤 공격



	//////////////////// 시연회용

	CBT_Composite_Node*		Start_Show();

	// 시야각내에 있으면 공격(첫 패턴부터 차례대로), 없으면 추적
	CBT_Composite_Node*		Show_ChaseAndNearAttack();
	CBT_Composite_Node*		Show_TurnAndFarAttack();
	CBT_Composite_Node*		Show_NearAttack();	// 패턴 순서대로 근거리 공격	
	CBT_Composite_Node*		Show_FarAttack();	// 패턴 순서대로 원거리 공격

	//////////////////// 

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CAIController*		m_pAIControllerCom = nullptr;
	CNavMesh*			m_pNavMesh = nullptr;


	//렌더에서 타임델타 쓰기위해서 저장해놓음
	_double				m_dTimeDelta = 0;

	_bool				m_bFindPlayer = false;	// 플레이어 발견 못한 상태
	_bool				m_bFight = false;

private:
	_int				m_iHitCount = 0;

private:
	// 충돌체를 위한 뼈다귀
	_mat*				m_matBones[Bone_End];
	_bool				m_bAIController = false;

	// 블랙보드에서 뼈의 Pos 저장소
	_v3					m_vTail2 = _v3(0.f, 0.f, 0.f);	//Tail2
	_v3					m_vTail4 = _v3(0.f, 0.f, 0.f);	//Tail4
	_v3					m_vTail6 = _v3(0.f, 0.f, 0.f);	//Tail6
	_v3					m_vTail6_Tongue2 = _v3(0.f, 0.f, 0.f);	//Tail6_Tongue2
	_v3					m_vBody = _v3(0.f, 0.f, 0.f);	//Spine2
	_v3					m_vHead = _v3(0.f, 0.f, 0.f);	//Head

private:	// 최초상태 세팅
	_float				m_fFov = 0.f;
	_float				m_fMaxLength = 0.f;
	_float				m_fMinLength = 0.f;

private:
	_bool				m_bPlayerFriendly = false;		// 플레이어 껀지

private:
	HRESULT Update_Bone_Of_BlackBoard();
	HRESULT Update_Value_Of_BB();

	HRESULT Update_NF();
	HRESULT Update_Collider();

private:
	_bool Is_InFov(_float fDegreeOfFov, _v3 vTargetPos);

	void Check_PhyCollider();

	HRESULT Draw_Collider();

private:
	void OnCollisionEnter();
	void OnCollisionEvent(list<CGameObject*> plistGameObject);

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

	HRESULT Ready_BoneMatrix();
	HRESULT Ready_Collider();
	HRESULT Ready_NF(void* pArg);

public:
	static CPoisonButterfly* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END