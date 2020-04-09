#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CDamegeNumUI;
class CMonsterUI;
class CWeapon;
class CSwordGenji final : public CGameObject
{
public:
	enum Color { White, Jungle, Normal };
	enum NF_Ani {Talk = 143, LookAround1 = 151, LookAround2 = 150, LookAround3 = 149, Sit1 = 144, Sit2 = 152};

	typedef struct tagInitInfo
	{
		tagInitInfo(CSwordGenji::Color _eColor, NF_Ani _eNF_Ani, _float _fFov, _float _fMaxLength, _float _fMinLength)
			: eColor(_eColor), eNF_Ani(_eNF_Ani), fFov(_fFov), fMaxLength(_fMaxLength), fMinLength(_fMinLength)
		{}

		CSwordGenji::Color		eColor = Normal;
		CSwordGenji::NF_Ani		eNF_Ani = Talk;
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
	explicit CSwordGenji(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CSwordGenji(const CSwordGenji& rhs);
	virtual ~CSwordGenji() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:	//���ϵ�
	// �ٰŸ�
	// 1. �Ϲݺ���1, �̵��Ÿ� : 1
	CBT_Composite_Node* Normal_Cut1();
	// 2. �Ϲݺ���2, �̵��Ÿ� : 1
	CBT_Composite_Node* Normal_Cut2();
	// 3. �Ϲݺ���3, �̵��Ÿ� : 1
	CBT_Composite_Node* Normal_Cut3();
	// 4. ���ϰ� ���������� ����, �̵��Ÿ� : 1
	CBT_Composite_Node* Strong_RightCut();
	// 5. ���ϰ� �������� ����, �̵��Ÿ� : 1
	CBT_Composite_Node* Strong_LeftCut();
	// 6. 3���� ����, �̵��Ÿ� : 1
	CBT_Composite_Node* ThreeCombo_Cut();
	// 7. ���򺣱�, �̵��Ÿ� : 1
	CBT_Composite_Node* Horizon_Cut();
	// 8. ���, �̵��Ÿ� : 1
	CBT_Composite_Node* Sting_Attack();
	// 9. 2���� ����, �̵��Ÿ� : 0.5 + 0.5
	CBT_Composite_Node* TwoCombo_Cut();
	// 10. �������� ����
	CBT_Composite_Node* Left_Cut();

	// ���Ÿ�
	// 1. ������ ������
	CBT_Composite_Node* Throwing1();
	// 2. ������ ������
	CBT_Composite_Node* Throwing2();
	// 3. �齺�� ������, �̵��Ÿ� : -1
	CBT_Composite_Node* Throwing_BackStep();
	// 4. �޸��鼭 ��������, �̵��Ÿ� : 4.5
	CBT_Composite_Node* Run_Straight_Cut();


	CBT_Composite_Node*		Start_Game();

	CBT_Composite_Node*		RotationAndNearAttack();
	CBT_Composite_Node*		ChaseAndNearAttack();

	CBT_Composite_Node*		Chase_NearOrFar();
	CBT_Composite_Node*		Chase(_float fAcceptable_Radius);
	CBT_Composite_Node*		NearAttack();	// ���� �ٰŸ� ����

	CBT_Composite_Node*		LookPlayer_FarAttack();	//�÷��̾� �ٶ� �� ���� ���Ÿ� ����
	CBT_Composite_Node*		FarAttack();	// ���� ���Ÿ� ����

	////////////// �ÿ�ȸ��

	CBT_Composite_Node* Show_ChaseAndNeqrAttack();

	CBT_Composite_Node* Show_NearAttack();


	//////////////

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CAIController*		m_pAIControllerCom = nullptr;
	CNavMesh*			m_pNavMesh = nullptr;
	CCollider*			m_pCollider = nullptr;

	CWeapon*			m_pSword = nullptr;

	////////////// MonsterHP UI
	CDamegeNumUI*		m_pDamegeNumUI = nullptr;
	CMonsterUI*			pMonsterHpUI = nullptr;

	//�������� Ÿ�ӵ�Ÿ �������ؼ� �����س���
	_double				m_dTimeDelta = 0;

	_bool				m_bFindPlayer = false;	// �÷��̾� �߰� ���� ����
	_bool				m_bFight = false;	

	//���ٱ�
	_mat*				m_matBones[Bone_End];
	_bool				m_bAIController = true;

	_v3					m_vLeftHand = _v3(0.f, 0.f, 0.f);

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
	void Push_Collider();

	HRESULT Draw_Collider();

private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();

	HRESULT Ready_Weapon();
	HRESULT Ready_BoneMatrix();
	HRESULT Ready_Collider();
	HRESULT Ready_NF(void* pArg);

public:
	static CSwordGenji* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END