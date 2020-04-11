#pragma once

#include "Monster.h"

BEGIN(Client)

class CDamegeNumUI;
class CMonsterUI;
class CWeapon;
class CGunGenji final : public CMonster
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

public:
	_mat*	Get_Bonmatrix() { return m_matBones[Bone_End]; }

private:	//���ϵ�
	// ���Ÿ�
	// 1. �ѽ��
	CBT_Composite_Node* Shot();
	// 2. ���ڱ� �ѽ��
	CBT_Composite_Node* Sudden_Shot();

	// �ٰŸ�
	// 1. ���Ӹ��� ġ��, �̵��Ÿ� : 1.6
	CBT_Composite_Node* Upper_Slash();
	// 2. �Ȳ�ġ ġ��, �̵��Ÿ� : 1
	CBT_Composite_Node* Arm_Attack();
	// 3. ���, �̵��Ÿ� : 0.3
	CBT_Composite_Node* Sting_Attack();
	// 4. ���������� ����, �̵��Ÿ� : 0.5
	CBT_Composite_Node* Cut_To_Right();
	// 5. �Һ� �ѽ��, �̵��Ÿ� : -1.3
	CBT_Composite_Node* Tumbling_Shot();	// ����

	// ȸ��
	// 1. �ڷ� ȸ��
	CBT_Composite_Node* Dodge_B();	// ����


	CBT_Composite_Node*		Start_Game();

	CBT_Composite_Node*		Dist_Attack();	// �Ĵٺ��� ������ ��������, �ָ� ���Ÿ�����

	
	CBT_Composite_Node*		LookPlayer_NearAttack();	// �÷��̾� �ٶ� �� ���� ���� ����
	CBT_Composite_Node*		LookPlayer_FarAttack();		// �÷��̾� �ٶ� �� ���� ���Ÿ� ����
	CBT_Composite_Node*		Chase();					// ����

	CBT_Composite_Node*		NearAttack();	// ���� ���� ����
	CBT_Composite_Node*		FarAttack();	// ���� ���Ÿ� ����


	//////////////////// �ÿ�ȸ��

	CBT_Composite_Node*	Start_Show();

	CBT_Composite_Node* Show_ChaseAttack();
	CBT_Composite_Node* Show_Attack();	// ���� ������� ����

	//////////////////// 


private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CAIController*		m_pAIControllerCom = nullptr;
	CNavMesh*			m_pNavMesh = nullptr;
	CCollider*			m_pCollider = nullptr;

	CWeapon*			m_pGun = nullptr;

	//////////// ä����
	// ���� HP�� UI
	CMonsterUI*			m_pMonsterUI = nullptr;
	// ���� ������ UI
	CDamegeNumUI*		m_pMonDamegeUI = nullptr;
	////////////

	//�������� Ÿ�ӵ�Ÿ �������ؼ� �����س���
	_double				m_dTimeDelta = 0;

	_bool				m_bFindPlayer = false;	// �÷��̾� �߰� ���� ����
	_bool				m_bFight = false;

	//���ٱ�
	_mat*				m_matBones[Bone_End];
	_bool				m_bAIController = true;

	// �����忡�� ���� Pos �����
	_v3					m_vRightHandAttach = _v3(0.f, 0.f, 0.f);	//RightHandAttach
	_v3					m_vHead = _v3(0.f, 0.f, 0.f);	// Head
	_v3					m_vRightToeBase	= _v3(0.f, 0.f, 0.f);	// Toe

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
	static CGunGenji* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END