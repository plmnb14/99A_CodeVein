#pragma once

#include "Monster.h"

BEGIN(Client)
class CBossHP;
class CPoisonButterfly final : public CMonster
{
public:
	typedef struct tagInitInfo
	{
		tagInitInfo(_float _fFov, _float _fMaxLength, _float _fMinLength,
			_bool _bSpawn = false, _v3 vPos = V3_NULL, _v3 vAngle = V3_NULL, _ushort sStageIdx = 0)//, _ushort sSubsetIdx = 0, _ushort sCellIdx = 0)
			: fFov(_fFov), fMaxLength(_fMaxLength), fMinLength(_fMinLength)
			, bSpawnOnTrigger(_bSpawn), vPos(vPos), vAngle(vAngle), sStageIdx(sStageIdx)// , sSubSetIdx(sSubSetIdx), sCellIdx(sCellIdx)
		{}

		_float					fFov = 0.f;
		_float					fMaxLength = 0.f;
		_float					fMinLength = 0.f;

		//=======================================================
		// 트리거 소환용
		//=======================================================
		_bool					bSpawnOnTrigger = false;
		_v3						vPos = {};
		_v3						vAngle = {};
		_ushort					sStageIdx = 0;
		//=======================================================
	}INFO;

private:
	enum Ani {
		Ani_Idle = 0, Ani_Death = 14, /*Ani_Dmg01_B = 14, */Ani_Appearance = 2, Ani_Appearance_End = 3, Ani_Down_Start = 9,
		Ani_Down_Loop = 10, Ani_Down_End = 11
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
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);
	virtual HRESULT Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur = false);

private:	//패턴들
	// 근거리
	// 1. 왼쪽 냠
	CBT_Composite_Node* Left_Eat(_float fWeight = 0.95f);		//충돌완	공격 거리 5
	// 2. 오른쪽 냠
	CBT_Composite_Node* Right_Eat(_float fWeight = 0.95f);	//충돌완	공격 거리 5
	// 3. 냠-> 뒤돌아서 덥썩
	CBT_Composite_Node* Eat_TurnEat();	//충돌완 이펙트 수정 바람, 모션 수정함,
	// 4. 기모아서 독 소용돌이
	CBT_Composite_Node* Poison_Tornado_After_Charging();	//충돌완 공격범위 3 이상
	// 5. 좁은 범위 한바퀴 독 발사
	CBT_Composite_Node* OneTurn_Poison();	//충돌완	공격 거리 3
	// 6. 냠 회전
	CBT_Composite_Node* Eat_Turn();	// 충돌완
	// 7. 트린다미어
	CBT_Composite_Node* WhirlWind();	// 충돌완

	// 원거리
	// 1. 돌진, 이동거리 : 13
	CBT_Composite_Node* Rush();		//충돌완
	// 2. 오발탄
	CBT_Composite_Node* Fire_5Bullet();		//완
	// 3. 저격 샷
	CBT_Composite_Node* Fire_ChaseBullet();	// 발사체 추가 완
	// 4. 돌면서 사방으로 독 날리기
	CBT_Composite_Node* Turn_4PoisonShot(); // 충돌완

	/////// 응용
	CBT_Composite_Node* Smart_Eat();
	CBT_Composite_Node* Rush_And_ChaseBullet();

	//
	CBT_Composite_Node*		Start_Game();

	// 체력 70퍼 이상	
	CBT_Composite_Node* More_Than_HP_70();
	CBT_Composite_Node* NearAttack_Dist5_More_Than_HP70();
	CBT_Composite_Node* FarAttack_More_Than_HP70();

	// 체력 70퍼 미만
	CBT_Composite_Node* More_Than_HP_40();
	CBT_Composite_Node* NearAttack_Dist5_More_Than_HP40();
	CBT_Composite_Node* FarAttack_More_Than_HP40();

	// 체력 40퍼 미만
	CBT_Composite_Node* HP_Final();
	CBT_Composite_Node* NearAttack_Dist5_Final();
	CBT_Composite_Node* FarAttack_Fianl();

	//////////////////// 시연회용

	CBT_Composite_Node*		Start_Show();

	// 시야각내에 있으면 공격(첫 패턴부터 차례대로), 없으면 추적
	CBT_Composite_Node*		Show_ChaseAndNearAttack();
	CBT_Composite_Node*		Show_TurnAndFarAttack();
	CBT_Composite_Node*		Show_NearAttack();	// 7개, 패턴 순서대로 근거리 공격	
	CBT_Composite_Node*		Show_FarAttack();	// 4개, 패턴 순서대로 원거리 공격

	//////////////////// 

private:
	void	Down();		// 체력 70퍼 이하면 다운

private:
	// 피격가능 현재 시간
	_double				m_dHitTime = 0;

	//렌더에서 타임델타 쓰기위해서 저장해놓음
	_double				m_dTimeDelta = 0;

	_bool				m_bFindPlayer = false;	// 플레이어 발견 못한 상태
	_bool				m_bFight = false;
	_bool				m_bAppearanceEffect = false;

private:	// UI(지원)
	CBossHP*			m_pBossUI = nullptr;
	_bool				m_bUITrigger = false;

private:	// 다운 상태를 위한 변수
	_bool				m_bDown_Start = false;
	
	_bool				m_bDown_StartAni = false;
	_bool				m_bDown_LoopAni = false;
	_bool				m_bDown_EndAni = false;
	_double				m_dDownTime = 0;

	_byte				m_iDownCount = 0;

private:
	// 충돌체를 위한 뼈다귀
	_mat*				m_matBones[Bone_End];
	_bool				m_bAIController = true;

	// 블랙보드에서 뼈의 Pos 저장소
	_v3					m_vTail2 = _v3(0.f, 0.f, 0.f);	//Tail2
	_v3					m_vTail4 = _v3(0.f, 0.f, 0.f);	//Tail4
	_v3					m_vTail6 = _v3(0.f, 0.f, 0.f);	//Tail6
	_v3					m_vTail6_Tongue2 = _v3(0.f, 0.f, 0.f);	//Tail6_Tongue2
	_v3					m_vBody = _v3(0.f, 0.f, 0.f);	//Spine2
	_v3					m_vHead = _v3(0.f, 0.f, 0.f);	//Head

	// 뼈 주소
	D3DXFRAME_DERIVED*	m_pTail6Frame = nullptr;
	D3DXFRAME_DERIVED*	m_pTail4Frame = nullptr;
	D3DXFRAME_DERIVED*	m_pTail2Frame = nullptr;
	D3DXFRAME_DERIVED*	m_pHeadFrame = nullptr;
	D3DXFRAME_DERIVED*	m_pTail6_Tongue2Frame = nullptr;

	_bool				m_bFinishCamShake = false;

private:	// 최초상태 세팅
	_float				m_fFov = 0.f;
	_float				m_fMaxLength = 0.f;
	_float				m_fMinLength = 0.f;

private:
	_bool				m_bPlayerFriendly = false;		// 플레이어 껀지

private:
	map<_int, const TCHAR*>	m_mapSound;

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
	HRESULT SetUp_ConstantTable(CShader* pShader);

	HRESULT Ready_BoneMatrix();
	HRESULT Ready_Collider();
	HRESULT Ready_NF(void* pArg);
	HRESULT Ready_Sound();

public:
	static CPoisonButterfly* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END