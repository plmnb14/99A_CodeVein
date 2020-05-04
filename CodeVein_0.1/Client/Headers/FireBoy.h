#pragma once

#include "Monster.h"

BEGIN(Client)
class CBossHP;
class CFireBoy final : public CMonster
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
		Ani_Idle = 0, Ani_Death = 17, Ani_Appearance = 2, Ani_Appearance_End = 3, Ani_Down_Start = 10, Ani_Down_Loop = 11, Ani_Down_End = 12
	};

private:
	enum BoneMatrix { Bone_Range, Bone_Body, Bone_Head, Bone_End };

protected:
	explicit CFireBoy(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CFireBoy(const CFireBoy& rhs);
	virtual ~CFireBoy() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_SetPass(CShader * pShader, _int iPass);

private:	// 패턴
	// 1. 팔꿈치 치기
	CBT_Composite_Node* Arm_Attack();
	// 2. 포신으로 치기
	CBT_Composite_Node* Gun_Attack();
	// 3. 자신 주위에 불 소용돌이 발생
	CBT_Composite_Node* Fire_Tornado();
	// 4. 백 대쉬
	CBT_Composite_Node* Back_Dash();

	////// 원거리
	// 1. 추적하면서 지속적으로 불 발사,	불 이펙트 크기 키우기
	CBT_Composite_Node* Fire_Tracking();
	// 2. 불 부채꼴로 발사
	CBT_Composite_Node* Fire_Cone();
	// 3. 추적 화염구 발사
	CBT_Composite_Node* Fire_BigSphere();
	// 4. 플레이어 바닥에서 화염
	CBT_Composite_Node* Fire_Ground();
	// 5. 위로 불쏘고 폭죽마냥 십자가 모양으로 떨어짐
	CBT_Composite_Node* Fire_Flame();



	///// 게임 시작
	CBT_Composite_Node* Start_Game();

	CBT_Composite_Node* NearAttack_Dist4();

	CBT_Composite_Node* NearAttack();	//랜덤 공격
	CBT_Composite_Node*	FarAttack();	//랜덤 공격

	//// 시연회용
	CBT_Composite_Node* Start_Show();
	CBT_Composite_Node* Show_NearAttack();
	CBT_Composite_Node* Show_FarAttack();

private:
	void Down();

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CAIController*		m_pAIControllerCom = nullptr;
	CNavMesh*			m_pNavMesh = nullptr;
	CCollider*			m_pCollider = nullptr;

	// 피격가능 현재 시간
	_double				m_dHitTime = 0;

	//렌더에서 타임델타 쓰기위해서 저장해놓음
	_double				m_dTimeDelta = 0;

	_bool				m_bFindPlayer = false;	// 플레이어 발견 못한 상태
	_bool				m_bFight = false;

private:	// UI(지원)
	CBossHP*			m_pBossUI = nullptr;

private:	// 다운 상태를 위한 변수
	_bool				m_bDown_Start = false;
	_bool				m_bDown_Finish = false;

	_bool				m_bDown_StartAni = false;
	_bool				m_bDown_LoopAni = false;
	_bool				m_bDown_EndAni = false;
	_double				m_dDownTime = 0;

private:
	// 충돌체를 위한 뼈다귀
	_mat*				m_matBones[Bone_End];
	_bool				m_bAIController = true;

	// 블랙보드에서 뼈의 Pos 저장소
	_v3					m_vMuzzle		= _v3(0.f, 0.f, 0.f);	// 총구 뼈
	_v3					m_vLeftForeArm	= _v3(0.f, 0.f, 0.f);	// 팔꿈치 뼈
	_v3					m_vLeftHand		= _v3(0.f, 0.f, 0.f);	// 왼손 뼈
	_v3					m_vRightHand	= _v3(0.f, 0.f, 0.f);	// 오른손 뼈

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
	void Check_PhyCollider();
	void Push_Collider();

private:
	void OnCollisionEnter();
	void OnCollisionEvent(list<CGameObject*> plistGameObject);

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

	HRESULT Ready_Weapon();
	HRESULT Ready_BoneMatrix();
	HRESULT Ready_Collider();
	HRESULT Ready_NF(void* pArg);

public:
	static CFireBoy* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

};


END