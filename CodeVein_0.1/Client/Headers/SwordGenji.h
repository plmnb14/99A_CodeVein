#pragma once

#include "Monster.h"

BEGIN(Client)

//class CDamegeNumUI;
class CMonsterUI;
class CGet_ItemUI;
class CWeapon;
class CSwordGenji final : public CMonster
{
public:
	enum Color { White, Jungle, Normal };
	enum NF_Ani { Talk = 2, LookAround1 = 4, LookAround2 = 3, LookAround3 = 2, Sit1 = 6, Sit2 = 10 };

	typedef struct tagInitInfo
	{
		tagInitInfo(CSwordGenji::Color _eColor, NF_Ani _eNF_Ani, _float _fFov, _float _fMaxLength, _float _fMinLength,
			_bool _bSpawn = false, _v3 vPos = V3_NULL, _v3 vAngle = V3_NULL, _ushort eStageIdx = 0)//, _ushort sSubsetIdx = 0, _ushort sCellIdx = 0)
			: eColor(_eColor), eNF_Ani(_eNF_Ani), fFov(_fFov), fMaxLength(_fMaxLength), fMinLength(_fMinLength),
			bSpawnOnTrigger(_bSpawn), vPos(vPos), vAngle(vAngle), eStageIdx(eStageIdx)// , sSubSetIdx(sSubSetIdx), sCellIdx(sCellIdx)
		{}

		CSwordGenji::Color		eColor = Normal;
		CSwordGenji::NF_Ani		eNF_Ani = Talk;
		_float					fFov = 0.f;
		_float					fMaxLength = 0.f;
		_float					fMinLength = 0.f;

		//=======================================================
		// 트리거 소환용
		//=======================================================
		_bool					bSpawnOnTrigger = false;
		_v3						vPos = {};
		_v3						vAngle = {};
		_ushort					eStageIdx = 0;
		//_ushort					sSubSetIdx = 0;
		//_ushort					sCellIdx = 0;
		//=======================================================
	}INFO;

private:
	enum Ani {
		Ani_Idle = 1, Ani_Death = 70, Ani_Dmg01_FL = 67, Ani_Dmg01_FR = 66, Ani_Dmg01_BL = 65, Ani_Dmg01_BR = 64,
		Ani_StandUp1 = 7, Ani_StandUp2 = 11,
		DeadBodyLean_End = 72,
		Ani_DmgRepel = 105,
		Ani_Death_F = 62, Ani_Death_B = 63
	};

	typedef enum Execution_Ani {
		Ani_Execution_LongCoat_B_S = 53, Ani_Execution_LongCoat_B = 54
	}EXE_ANI;

private:
	enum BoneMatrix { Bone_Range, Bone_Body, Bone_Head, Bone_End };

protected:
	explicit CSwordGenji(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CSwordGenji(const CSwordGenji& rhs);
	virtual ~CSwordGenji() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);
	virtual HRESULT Render_GameObject_SetPass(CShader* pShader, _int iPass, _bool _bIsForMotionBlur = false);

public:
	_mat*	Get_Bonmatrix() { return m_matBones[Bone_Head]; }

private:	//패턴들
	// 근거리
	// 1. 일반베기1, 이동거리 : 1
	CBT_Composite_Node* Normal_Cut1();
	// 2. 일반베기2, 이동거리 : 1
	CBT_Composite_Node* Normal_Cut2();
	// 3. 일반베기3, 이동거리 : 1
	CBT_Composite_Node* Normal_Cut3();
	// 4. 강하게 오른쪽으로 베기, 이동거리 : 1
	CBT_Composite_Node* Strong_RightCut();
	// 5. 강하게 왼쪽으로 베기, 이동거리 : 1
	CBT_Composite_Node* Strong_LeftCut();
	// 6. 3연속 베기, 이동거리 : 1
	CBT_Composite_Node* ThreeCombo_Cut();
	// 7. 수평베기, 이동거리 : 1
	CBT_Composite_Node* Horizontal_Cut();
	// 8. 찌르기, 이동거리 : 1
	CBT_Composite_Node* Sting_Attack();
	// 9. 2연속 베기, 이동거리 : 0.5 + 0.5
	CBT_Composite_Node* TwoCombo_Cut();
	// 10. 왼쪽으로 베기
	CBT_Composite_Node* Left_Cut();

	// 원거리
	// 1. 옆에서 던지기
	CBT_Composite_Node* Throwing1();
	// 2. 앞으로 던지기
	CBT_Composite_Node* Throwing2();
	// 3. 백스탭 던지기, 이동거리 : -1
	CBT_Composite_Node* Throwing_BackStep();
	// 4. 달리면서 직선베기, 이동거리 : 4.5
	CBT_Composite_Node* Run_Straight_Cut();


	CBT_Composite_Node*		Start_Game();

	CBT_Composite_Node*		RotationAndNearAttack();
	CBT_Composite_Node*		ChaseAndNearAttack();

	CBT_Composite_Node*		Chase_NearOrFar();
	CBT_Composite_Node*		Chase(_float fAcceptable_Radius);
	CBT_Composite_Node*		NearAttack();	// 랜덤 근거리 공격

	CBT_Composite_Node*		LookPlayer_FarAttack();	//플레이어 바라본 후 랜덤 원거리 공격
	CBT_Composite_Node*		FarAttack();	// 랜덤 원거리 공격

	////////////// 시연회용

	CBT_Composite_Node* Show_ChaseAndNeqrAttack();

	CBT_Composite_Node* Show_NearAttack();


	//////////////

private:
	EXE_ANI				m_eExecutionAnim = Ani_Execution_LongCoat_B_S;

	CWeapon*			m_pSword = nullptr;

	////////////// MonsterHP UI
	CGet_ItemUI*		m_pGet_Item = nullptr;

	//렌더에서 타임델타 쓰기위해서 저장해놓음
	_double				m_dTimeDelta = 0;

	_bool				m_bFindPlayer = false;	// 플레이어 발견 못한 상태
	_bool				m_bFight = false;

	//뼈다귀
	_mat*				m_matBones[Bone_End];
	_bool				m_bAIController = true;

	_v3					m_vLeftHand = _v3(0.f, 0.f, 0.f);
	_v3					m_vHead = _v3(0.f, 0.f, 0.f);	// Head
	_v3					m_vRightToeBase = _v3(0.f, 0.f, 0.f);	// Toe

	// 뼈 주소
	D3DXFRAME_DERIVED*	m_pLeftHandAttachFrame = nullptr;
	D3DXFRAME_DERIVED*	m_pHeadFrame = nullptr;
	D3DXFRAME_DERIVED*	m_pRightToeBaseFrame = nullptr;

private:
	_float				m_fSkillMoveSpeed_Cur = 0.f;
	_float				m_fSkillMoveSpeed_Max = 0.f;
	_float				m_fSkillMoveAccel_Cur = 0.5f;
	_float				m_fSkillMoveAccel_Max = 0.f;
	_float				m_fSkillMoveMultiply = 1.f;

	_v3					m_vPushDir_forHitting = _v3(0.f, 0.f, 0.f);

private:	// 최초상태 세팅
	NF_Ani				m_eNF_Ani = Talk;
	_float				m_fFov = 0.f;
	_float				m_fMaxLength = 0.f;
	_float				m_fMinLength = 0.f;

private:
	map<_int, const TCHAR*>	m_mapSound;


private: // For Effect
	_float			m_fDeadEffect_Delay = 0.f;
	_float			m_fDeadEffect_Offset = 0.f;

private:
	HRESULT Update_Bone_Of_BlackBoard();
	HRESULT Update_Value_Of_BB();

	HRESULT Update_NF();
	HRESULT Update_Collider();

private:
	void Skill_Movement(_float _fspeed, _v3 _vDir = { 0.f , 0.f , 0.f });
	void Decre_Skill_Movement(_float _fMutiply = 1.f);

	void Check_PhyCollider();
	void Push_Collider();

	void Check_TargetDist();	// 초기상태로 돌아가기 위한 함수.

private:
	void Check_DeadEffect(_double TimeDelta);

private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable(CShader* pShader);

	HRESULT Ready_Weapon();
	HRESULT Ready_BoneMatrix();
	HRESULT Ready_Collider();
	HRESULT Ready_NF(void* pArg);
	HRESULT Ready_Sound();
	HRESULT Ready_Rigid();

private:
	void Check_Execution();
	void Check_ExecutionAnim();
	void Check_OrgeExecution();
	void Check_IvyExecution();
	void Check_StingerExecution();
	void Check_HoundsExecution();

private:
	void Check_Repel();

public:
	static CSwordGenji* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END