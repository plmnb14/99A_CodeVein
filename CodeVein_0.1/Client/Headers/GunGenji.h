#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CDamegeNumUI;
class CMonsterUI;
class CWeapon;
class CGunGenji final : public CGameObject
{
public:
	enum Color { White, Jungle, Normal };
	enum NF_Ani {
		Talk = 143, LookAround1 = 151, LookAround2 = 150, LookAround3 = 149, Sit1 = 144, Sit2 = 152,
		CheckGun = 155
	};

	typedef struct tagInitInfo
	{
		tagInitInfo(CGunGenji::Color _eColor, NF_Ani _eNF_Ani, _float _fFov, _float _fMaxLength, _float _fMinLength)
			: eColor(_eColor), eNF_Ani(_eNF_Ani), fFov(_fFov), fMaxLength(_fMaxLength), fMinLength(_fMinLength)
		{}

		CGunGenji::Color		eColor = Normal;
		CGunGenji::NF_Ani		eNF_Ani = Talk;
		_float					fFov = 0.f;
		_float					fMaxLength = 0.f;
		_float					fMinLength = 0.f;
	}INFO;

private:
	enum Ani {
		Ani_Idle = 42, Ani_Death = 64, Ani_Dmg01_FL = 122,
		Ani_StandUp1 = 140, Ani_StandUp2 = 153
	};

private:
	enum BoneMatrix { Bone_Range, Bone_Body, Bone_Head, Bone_End };

protected:
	explicit CGunGenji(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CGunGenji(const CGunGenji& rhs);
	virtual ~CGunGenji() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_SetPass(CShader* pShader, _int iPass);

private:	//패턴들
	// 원거리
	// 1. 총쏘기
	CBT_Composite_Node* Shot();
	// 2. 갑자기 총쏘기
	CBT_Composite_Node* Sudden_Shot();

	// 근거리
	// 1. 개머리판 치기, 이동거리 : 1.6
	CBT_Composite_Node* Upper_Slash();
	// 2. 팔꿈치 치기, 이동거리 : 1
	CBT_Composite_Node* Arm_Attack();
	// 3. 찌르기, 이동거리 : 0.3
	CBT_Composite_Node* Sting_Attack();
	// 4. 오른쪽으로 베기, 이동거리 : 0.5
	CBT_Composite_Node* Cut_To_Right();
	// 5. 텀블링 총쏘기, 이동거리 : -1.3
	CBT_Composite_Node* Tumbling_Shot();

	// 회피
	// 1. 뒤로 회피
	CBT_Composite_Node* Dodge_B();


	CBT_Composite_Node*		Start_Game();

	CBT_Composite_Node*		Dist_Attack();	// 쳐다보고 가까우면 근접공격, 멀면 원거리공격

	
	CBT_Composite_Node*		LookPlayer_NearAttack();	// 플레이어 바라본 후 랜덤 근접 공격
	CBT_Composite_Node*		LookPlayer_FarAttack();		// 플레이어 바라본 후 랜덤 원거리 공격
	CBT_Composite_Node*		Chase();					// 추적

	CBT_Composite_Node*		NearAttack();	// 랜덤 근접 공격
	CBT_Composite_Node*		FarAttack();	// 랜덤 원거리 공격


	//////////////////// 시연회용

	CBT_Composite_Node*	Start_Show();

	CBT_Composite_Node* Show_ChaseAttack();
	CBT_Composite_Node* Show_Attack();	// 패턴 순서대로 공격

	//////////////////// 


private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CAIController*		m_pAIControllerCom = nullptr;
	CNavMesh*			m_pNavMesh = nullptr;

	CWeapon*			m_pGun = nullptr;

	//////////// 채유미
	// 몬스터 HP바 UI
	CMonsterUI*			m_pMonsterUI = nullptr;
	// 몬스터 데미지 UI
	CDamegeNumUI*		m_pMonDamegeUI = nullptr;
	////////////

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

	_bool Is_InFov(_float fDegreeOfFov, _v3 vTargetPos);

	void Check_PhyCollider();

	HRESULT Draw_Collider();

private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();

	HRESULT Ready_Weapon();
	HRESULT Ready_BoneMatrix();
	HRESULT Ready_Collider();
	HRESULT Ready_NF(void* pArg);

public:
	static CGunGenji* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END