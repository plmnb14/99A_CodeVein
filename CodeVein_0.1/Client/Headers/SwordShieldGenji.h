#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CMonsterUI;
class CWeapon;
class CSwordShieldGenji final : public CGameObject
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
		Ani_StandUp1 = 140, Ani_StandUp2 = 153
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

private:	//���ϵ�
	// �ٰŸ�
	// 1. ���۽�����, �̵��Ÿ� : 1
	CBT_Composite_Node* Upper_Slash();
	// 2. �� ������ ���, �̵��Ÿ� : 1
	CBT_Composite_Node* LongDelay_Sting();
	// 3. ����ġ��, �̵��Ÿ� : 0.5
	CBT_Composite_Node* Shield_Attack();
	// 4. �ѹ��� ���鼭 ����, �̵��Ÿ� : 2
	CBT_Composite_Node* Turning_Cut();
	// 5. ª�� ������ ���, �̵��Ÿ� : 0.6
	CBT_Composite_Node* ShortDelay_Sting();

	// ����
	// 1. ���е�� ���ϰ� ����
	CBT_Composite_Node* Hit_Weakly();
	// 2. ���е�� ���ϰ� ����
	CBT_Composite_Node* Hit_Strongly();

	// ����
	CBT_Composite_Node* Guard(_double dGuardTime);
	CBT_Composite_Node*	Guard_LeftMoveAround();
	CBT_Composite_Node*	Guard_RightMoveAround();


	// ���е�� �ȱ�	�÷��̾�� �Ÿ� ��� �� �ȴ´�.

	CBT_Composite_Node*		Start_Game();

	CBT_Composite_Node*		Dist_Attack();

	CBT_Composite_Node*		RotationAndNearAttack();
	CBT_Composite_Node*		Chase_Guard_NearAttack();
	CBT_Composite_Node*		MoveAround_NearAttack();

	CBT_Composite_Node*		Guard_MoveAround();

	CBT_Composite_Node*		Chase();
	CBT_Composite_Node*		NearAttack();	// ���� ���� ����

	// ���е�� �̵� -> ���⺰�� �ൿ ��������.

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CAIController*		m_pAIControllerCom = nullptr;
	CNavMesh*			m_pNavMesh = nullptr;

	CWeapon*			m_pSword = nullptr;
	CWeapon*			m_pShield = nullptr;

	//////////ä����
	// MonsterHP UI
	CMonsterUI*			m_pMonsterUI = nullptr;

	//�������� Ÿ�ӵ�Ÿ �������ؼ� �����س���
	_double				m_dTimeDelta = 0;

	_bool				m_bFindPlayer = false;	// �÷��̾� �߰� ���� ����
	_bool				m_bFight = false;

	//���ٱ�
	_mat*				m_matBones[Bone_End];
	_bool				m_bAIController = true;

private:
	_float				m_fSkillMoveSpeed_Cur = 0.f;
	_float				m_fSkillMoveSpeed_Max = 0.f;
	_float				m_fSkillMoveAccel_Cur = 0.5f;
	_float				m_fSkillMoveAccel_Max = 0.f;
	_float				m_fSkillMoveMultiply = 1.f;

	_v3					m_vPushDir_forHitting = _v3(0.f, 0.f, 0.f);

private:	// ���ʻ��� ����
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
	static CSwordShieldGenji* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END