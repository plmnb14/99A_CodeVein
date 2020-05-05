#pragma once

#include "Monster.h"

BEGIN(Client)

class CWeapon;
class CDeerKing final : public CMonster
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
		Ani_Idle = 0, Ani_Death = 20, Ani_Appearance = 2, Ani_Appearance_End = 3, Ani_Down_Start = 13, Ani_Down_Loop = 14, Ani_Down_End = 15,
		Ani_Throw_Shield = 1
	};

private:
	enum BoneMatrix { Bone_Range, Bone_Body, Bone_Head, Bone_End };

protected:
	explicit CDeerKing(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CDeerKing(const CDeerKing& rhs);
	virtual ~CDeerKing() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur = false);

private:	// 패턴들
	// 1페이지, 방패들고 있음
	// 1. 왼팔 휘두르기
	CBT_Composite_Node* LeftHand_Attack(_float fWeight = 0.95f);
	// 2. 오른발로 찍기
	CBT_Composite_Node* RightFoot_Attack(_float fWeight = 0.95f);
	// 3. 돌진, 오른발 찍기와 연계
	CBT_Composite_Node* Rush_RightFoot();
	// 4. 휠윈드, 오른발 찍기와 연계
	CBT_Composite_Node* WhirlWind_RightFoot();	// 돌면서 추격
	// 5. 머리박고 고드름 생성
	CBT_Composite_Node* Head_ColdBeam();
	// 6. 슬라이드 공격
	CBT_Composite_Node* Slide_Attack();
	// 7. 점프해서 방패 찍기
	CBT_Composite_Node* Jump_Attack(_float fWeight = 0.95f);

	// 8. 투사체 원형으로 8개 던지기
	CBT_Composite_Node* Throwing();

	// 2페이지, 방패 던졌음
	// 1. 제자리 점프 후 내려찍기, 큰 폭발
	CBT_Composite_Node* Jump_In_Place();
	// 2. 돌진 몸통박치기
	CBT_Composite_Node* Rush_Body();
	// 3. 윈스턴처럼 점프 찍기
	CBT_Composite_Node* Jump_Fist();
	// 4. 왼손에 얼음칼 베기
	CBT_Composite_Node* Blade_Attack();	// 얼음검 이펙트 넣어야함
	// 5. 오른팔 휘두르기
	CBT_Composite_Node* RightHand_Attack(_float fWeight = 0.95f);	// 이펙트 타이밍 맞춰야함.


	//////////// 응용
	CBT_Composite_Node* Smart_Three_Attack();
	CBT_Composite_Node* Smart_JumpAttack();
	CBT_Composite_Node* Four_Combo_Punch();

	CBT_Composite_Node* Chase_Timer(_double dRunTime, _float fSpeed);
	CBT_Composite_Node* RightFoot_Attack__Rush_Or_WhirlWind();



	//// 게임 시작
	CBT_Composite_Node* Start_Game();

	// 체력 60퍼 이상
	CBT_Composite_Node* More_Than_HP_60();
	CBT_Composite_Node* NearAttack_Dist5_More_Than_HP60();
	CBT_Composite_Node* FarAttack_More_Than_HP60();

	// 체력 60퍼 미만
	CBT_Composite_Node* HP_Final();
	CBT_Composite_Node* NearAttack_Dist5_Final();
	CBT_Composite_Node* FarAttack_Fianl();

	//// 시연회용
	CBT_Composite_Node* Start_Show();
	CBT_Composite_Node* Show_RotationAndNearAttack();
	CBT_Composite_Node* Show_NearAttack();
	CBT_Composite_Node* Show_FarAttack();


private:
	void Down();	// 방패 집어던짐

	void Update_Shield();	// 방패 던짐
	void Update_Dir_Shield_Throwing();

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CAIController*		m_pAIControllerCom = nullptr;
	CNavMesh*			m_pNavMesh = nullptr;
	CCollider*			m_pCollider = nullptr;

	CWeapon*			m_pShield = nullptr;

	// 피격가능 현재 시간
	_double				m_dHitTime = 0;

	//렌더에서 타임델타 쓰기위해서 저장해놓음
	_double				m_dTimeDelta = 0;

	_bool				m_bFindPlayer = false;	// 플레이어 발견 못한 상태
	_bool				m_bFight = false;

	// 방패 던지기 변수들
	_bool				m_bFinish_Throw_Shield = false;
	_bool				m_bThrow_Shield = false;
	_v3					m_bOld_RightHandAttach_Pos = _v3(0.f, 0.f, 0.f);
	_v3					m_bCur_RightHandAttach_Pos = _v3(0.f, 0.f, 0.f);
	_v3					m_vThrowing_Dir = _v3(0.f, 0.f, 0.f);
	_float				m_fThrowing_Speed = 15.f;

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
	_v3					m_vRightHand = _v3(0.f, 0.f, 0.f);	//RightHand
	_v3					m_vLeftHand = _v3(0.f, 0.f, 0.f);	//LeftHand
	_v3					m_vLeftHandAttach = _v3(0.f, 0.f, 0.f);	//LeftHandAttach
	_v3					m_vLeftJet = _v3(0.f, 0.f, 0.f);	//Spine3_LeftJet
	_v3					m_vRightJet = _v3(0.f, 0.f, 0.f);	//Spine3_RightJet
	_v3					m_vRightHandAttach = _v3(0.f, 0.f, 0.f); //RightHandAttach
	_v3					m_vHeadColdBeamPos = _v3(0.f, 0.f, 0.f);	//Head_LeftCorner (사슴뿔, 머리 고드름 생성위치)

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

	HRESULT Update_Effect(); // 보스 몸에서 계속 나오는 이펙트

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
	static CDeerKing* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END