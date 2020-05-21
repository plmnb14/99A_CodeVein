#pragma once

#include "Monster.h"

BEGIN(Client)

//class CDamegeNumUI;
class CMonsterUI;
class CWeapon;
class CGunGenji final : public CMonster
{
public:
	enum Color { White, Jungle, Normal };
	enum NF_Ani {
		Talk = 2, LookAround1 = 4, LookAround2 = 3, LookAround3 = 2, Sit1 = 6, Sit2 = 10,
		CheckGun = 14
	};

	typedef struct tagInitInfo
	{
		tagInitInfo(CGunGenji::Color _eColor, NF_Ani _eNF_Ani, _float _fFov, _float _fMaxLength, _float _fMinLength,
			_bool _bSpawn = false, _v3 vPos = V3_NULL, _v3 vAngle = V3_NULL, _ushort eStageIdx = 0)//, _ushort sSubsetIdx = 0, _ushort sCellIdx = 0)
			: eColor(_eColor), eNF_Ani(_eNF_Ani), fFov(_fFov), fMaxLength(_fMaxLength), fMinLength(_fMinLength),
			bSpawnOnTrigger(_bSpawn), vPos(vPos), vAngle(vAngle), eStageIdx(eStageIdx)// , sSubSetIdx(sSubSetIdx), sCellIdx(sCellIdx)
		{}

		CGunGenji::Color		eColor = Normal;
		CGunGenji::NF_Ani		eNF_Ani = Talk;
		_float					fFov = 0.f;
		_float					fMaxLength = 0.f;
		_float					fMinLength = 0.f;

		//=======================================================
		// Ʈ���� ��ȯ��
		//=======================================================
		_bool					bSpawnOnTrigger = false;
		_v3						vPos = {};
		_v3						vAngle = {};
		_ushort					eStageIdx = 0;
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
		Ani_Execution_LongCoat_B_S = 53 , Ani_Execution_LongCoat_B = 54
	}EXE_ANI;

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
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);
	virtual HRESULT Render_GameObject_SetPass(CShader* pShader, _int iPass, _bool _bIsForMotionBlur = false);

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
	EXE_ANI				m_eExecutionAnim = Ani_Execution_LongCoat_B_S;

	CWeapon*			m_pGun = nullptr;

	//////////// ä����

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

	// �� �ּ�
	D3DXFRAME_DERIVED*	m_pRightHandAttachFrame = nullptr;
	D3DXFRAME_DERIVED*	m_pHeadFrame = nullptr;
	D3DXFRAME_DERIVED*	m_pRightToeBaseFrame = nullptr;

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

	void Check_TargetDist();	// �ʱ���·� ���ư��� ���� �Լ�.

private:
	void Check_DeadEffect(_double TimeDelta);

private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable(CShader * pShader);

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
	static CGunGenji* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END