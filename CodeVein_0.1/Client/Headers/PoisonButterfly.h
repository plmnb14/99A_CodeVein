#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CPoisonButterfly final : public CGameObject
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
		Ani_Idle = 6, Ani_Death = 12, Ani_Dmg01_B = 14, Ani_Appearance = 16
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

private:	//���ϵ�
	// �ٰŸ�
	// 1. ���� ��
	CBT_Composite_Node* Left_Eat();		//�浹��	���� �Ÿ� 5
	// 2. ������ ��
	CBT_Composite_Node* Right_Eat();	//�浹��	���� �Ÿ� 5
	// 3. ��-> ���׸�����
	CBT_Composite_Node* Eat_Turn_Eat();	//�浹�� ����Ʈ ���� �ٶ�, ��� ������
	// 4. ���Ƽ� �� �ҿ뵹��
	CBT_Composite_Node* Poison_Tornado_After_Charging();	// ���ݹ��� 3 �̻�
	// 5. ���� ���� �ѹ��� �� �߻�
	CBT_Composite_Node* OneTurn_Poison();	//�浹��	���� �Ÿ� 3
	// 6. �� ȸ��
	CBT_Composite_Node* Eat_Turn();	// �浹��	����Ʈ �߰� �ٶ�
	// 7. Ʈ���ٹ̾�
	CBT_Composite_Node* WhirlWind();	// �浹��
	// 8. ������ ��, ���� ��
	CBT_Composite_Node* Right_Eat_Left_Eat();


	// ���Ÿ�
	// 1. ����, �̵��Ÿ� : 13
	CBT_Composite_Node* Rush();		//�浹��
	// 2. ����ź
	CBT_Composite_Node* Fire_5Bullet();		//��
	// 3. ���� ��
	CBT_Composite_Node* Fire_ChaseBullet();	// �߻�ü �߰� ��

	//
	/*
	���� ���� -> ���� ����  -> �ٰŸ� ����, ���� �� ���� or �׳� �� �ڸ����� ����
							-> ���Ÿ� ����, ���ڸ����� ����
	*/
	CBT_Composite_Node*		Start_Game();

	CBT_Composite_Node*		Dist_Attack();

	CBT_Composite_Node*		ChaseAndNearAttack();
	CBT_Composite_Node*		TurnAndFarAttack();
	CBT_Composite_Node*		Chase();

	CBT_Composite_Node*		NearAttack_Dist3();
	CBT_Composite_Node*		NearAttack_Dist6();

	CBT_Composite_Node*		NearAttack();	//���� ����
	CBT_Composite_Node*		FarAttack();	//���� ����



	//////////////////// �ÿ�ȸ��

	CBT_Composite_Node*		Start_Show();

	// �þ߰����� ������ ����(ù ���Ϻ��� ���ʴ��), ������ ����
	CBT_Composite_Node*		Show_ChaseAndNearAttack();
	CBT_Composite_Node*		Show_TurnAndFarAttack();
	CBT_Composite_Node*		Show_NearAttack();	// ���� ������� �ٰŸ� ����	
	CBT_Composite_Node*		Show_FarAttack();	// ���� ������� ���Ÿ� ����

	//////////////////// 

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CAIController*		m_pAIControllerCom = nullptr;
	CNavMesh*			m_pNavMesh = nullptr;


	//�������� Ÿ�ӵ�Ÿ �������ؼ� �����س���
	_double				m_dTimeDelta = 0;

	_bool				m_bFindPlayer = false;	// �÷��̾� �߰� ���� ����
	_bool				m_bFight = false;

private:
	_int				m_iHitCount = 0;

private:
	// �浹ü�� ���� ���ٱ�
	_mat*				m_matBones[Bone_End];
	_bool				m_bAIController = false;

	// �����忡�� ���� Pos �����
	_v3					m_vTail2 = _v3(0.f, 0.f, 0.f);	//Tail2
	_v3					m_vTail4 = _v3(0.f, 0.f, 0.f);	//Tail4
	_v3					m_vTail6 = _v3(0.f, 0.f, 0.f);	//Tail6
	_v3					m_vTail6_Tongue2 = _v3(0.f, 0.f, 0.f);	//Tail6_Tongue2
	_v3					m_vBody = _v3(0.f, 0.f, 0.f);	//Spine2
	_v3					m_vHead = _v3(0.f, 0.f, 0.f);	//Head

private:	// ���ʻ��� ����
	_float				m_fFov = 0.f;
	_float				m_fMaxLength = 0.f;
	_float				m_fMinLength = 0.f;

private:
	_bool				m_bPlayerFriendly = false;		// �÷��̾� ����

private:
	HRESULT Update_Bone_Of_BlackBoard();
	HRESULT Update_Value_Of_BB();

	HRESULT Update_NF();
	HRESULT Update_Collider();

private:
	_bool Is_InFov(_float fDegreeOfFov, _v3 vTargetPos);

	void Check_PhyCollider();

	HRESULT Draw_Collider();

private:
	void OnCollisionEnter();
	void OnCollisionEvent(list<CGameObject*> plistGameObject);

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

	HRESULT Ready_BoneMatrix();
	HRESULT Ready_Collider();
	HRESULT Ready_NF(void* pArg);

public:
	static CPoisonButterfly* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END