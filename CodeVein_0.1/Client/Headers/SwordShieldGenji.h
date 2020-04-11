#pragma once

#include "Monster.h"

BEGIN(Client)

class CMonsterUI;
class CDamegeNumUI;
class CWeapon;
class CSwordShieldGenji final : public CMonster
{
public:
	enum Color { White, Jungle, Normal };
	enum NF_Ani { Talk = 143, LookAround1 = 151, LookAround2 = 150, LookAround3 = 149, Sit1 = 144, Sit2 = 152 };
	typedef struct tagInitInfo
	{
		tagInitInfo(CSwordShieldGenji::Color _eColor, NF_Ani _eNF_Ani, _float _fFov, _float _fMaxLength, _float _fMinLength)
			: eColor(_eColor), eNF_Ani(_eNF_Ani), fFov(_fFov), fMaxLength(_fMaxLength), fMinLength(_fMinLength)
		{}

		CSwordShieldGenji::Color		eColor = Normal;
		CSwordShieldGenji::NF_Ani		eNF_Ani = Talk;
		_float					fFov = 0.f;
		_float					fMaxLength = 0.f;
		_float					fMinLength = 0.f;
	}INFO;

private:
	enum Ani {
		Ani_Idle = 42, Ani_Death = 64, Ani_Dmg01_FL = 122,
		Ani_StandUp1 = 140, Ani_StandUp2 = 153,
		Ani_GuardHit_Weak = 2, Ani_GuardHit_Strong = 1, Ani_GuardBreak = 0
	};

private:
	enum BoneMatrix { Bone_Range, Bone_Body, Bone_Head, Bone_End };

protected:
	explicit CSwordShieldGenji(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CSwordShieldGenji(const CSwordShieldGenji& rhs);
	virtual ~CSwordShieldGenji() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	_mat*	Get_Bonmatrix() { return m_matBones[Bone_Head]; }

private:	//패턴들
	// 근거리
	// 1. 어퍼슬래쉬, 이동거리 : 1
	CBT_Composite_Node* Upper_Slash();
	// 2. 긴 딜레이 찌르기, 이동거리 : 1
	CBT_Composite_Node* LongDelay_Sting();
	// 3. 방패치기, 이동거리 : 0.5
	CBT_Composite_Node* Shield_Attack();
	// 4. 한바퀴 돌면서 베기, 이동거리 : 2
	CBT_Composite_Node* Turning_Cut();
	// 5. 짧은 딜레이 찌르기, 이동거리 : 0.6
	CBT_Composite_Node* ShortDelay_Sting();

	// 변칙패턴
	// 1. 찌르는 모션 진행하다가  플레이어가 멀면 멀리 찌르고, 가까우면 가까이 찌르고
	CBT_Composite_Node* Anomaly_LongSting();


	// 가드
	CBT_Composite_Node* Guard(_double dGuardTime);

	CBT_Composite_Node* LGuard_MoveAround();
	CBT_Composite_Node* RGuard_MoveAround();

	CBT_Composite_Node*		Start_Game();

	CBT_Composite_Node*		Dist_Attack();

	CBT_Composite_Node*		RotationAndNearAttack();
	CBT_Composite_Node*		RotationAndNearAttack_NotShield();
	CBT_Composite_Node*		Chase_Guard_NearAttack();
	CBT_Composite_Node*		MoveAround();

	CBT_Composite_Node*		LeftMoveAround();
	CBT_Composite_Node*		RightMoveAround();

	CBT_Composite_Node*		Chase();
	CBT_Composite_Node*		NearAttack_Except_Shield_Attack();	// 랜덤 근접 공격, 방패치기는 뻄
	CBT_Composite_Node*		NearAttack();

	// 방패들고 이동 -> 방향별로 행동 나눠야함.

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CAIController*		m_pAIControllerCom = nullptr;
	CNavMesh*			m_pNavMesh = nullptr;
	CCollider*			m_pCollider = nullptr;

	CWeapon*			m_pSword = nullptr;
	CWeapon*			m_pShield = nullptr;

	//////////채유미
	// MonsterHP UI
	CMonsterUI*			m_pMonsterUI = nullptr;
	CDamegeNumUI*		m_pDamegeNumUI = nullptr;

	//렌더에서 타임델타 쓰기위해서 저장해놓음
	_double				m_dTimeDelta = 0;

	_bool				m_bFindPlayer = false;	// 플레이어 발견 못한 상태
	_bool				m_bFight = false;

	//뼈다귀
	_mat*				m_matBones[Bone_End];
	_bool				m_bAIController = true;

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
	HRESULT Update_Bone_Of_BlackBoard();
	HRESULT Update_Value_Of_BB();

	HRESULT Update_NF();
	HRESULT Update_Collider();

private:
	void Skill_Movement(_float _fspeed, _v3 _vDir = { 0.f , 0.f , 0.f });
	void Decre_Skill_Movement(_float _fMutiply = 1.f);

	void Check_PhyCollider();
	void Push_Collider();

private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();

	HRESULT Ready_Weapon();
	HRESULT Ready_BoneMatrix();
	HRESULT Ready_Collider();
	HRESULT Ready_NF(void* pArg);

public:
	static CSwordShieldGenji* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END