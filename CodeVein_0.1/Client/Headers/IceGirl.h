#pragma once

#include "Monster.h"

BEGIN(Client)

class CWeapon;
class CIceGirl final : public CMonster
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
		Ani_Idle = 0, Ani_Death = 26, Ani_Appearance = 3, Ani_Down_Start = 18, Ani_Down_Loop = 19, Ani_Down_End = 20
	};

private:
	enum BoneMatrix { Bone_Range, Bone_Body, Bone_Head, Bone_End };

protected:
	explicit CIceGirl(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CIceGirl(const CIceGirl& rhs);
	virtual ~CIceGirl() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_SetPass(CShader * pShader, _int iPass);

private:	// ���ϵ�
	// 1. ȸ������
	CBT_Composite_Node* Turn_Cut();
	// 2. ��ܺ���,  �� + �� + ��
	CBT_Composite_Node* ThreeCombo_Cut1();
	// 3. ��ܺ���,  �� + �� + ȸ������
	CBT_Composite_Node* ThreeCombo_Cut2();
	// 4. �������
	CBT_Composite_Node* Jump_Attack();
	// 5. �� �ֵθ��� ���� ��ȯ
	CBT_Composite_Node* Ice_Cut();
	// 6. ���� ��ȣ��
	CBT_Composite_Node* Ice_Barrier();

	// 7. ��ܺ���, �ι�° ����° �� �����ϰ� ����


	// ���Ÿ�
	// 1. ���� �� ���� ���
	CBT_Composite_Node* Charge_Rush();

	// 2. �÷��̾� ������ �ݵ��, 31�� �ִ�
	

	// ȸ��
	// ��
	CBT_Composite_Node* Dodge_Left();
	// ��
	CBT_Composite_Node* Dodge_Right();
	// ��
	CBT_Composite_Node* Dodge_Front();
	// ��
	CBT_Composite_Node* Dodge_Back();


	///// ���� ����
	CBT_Composite_Node* Start_Game();

	CBT_Composite_Node*	NearAttack_Dist5();

	CBT_Composite_Node* NearAttack();	//���� ����
	CBT_Composite_Node*	FarAttack();	//���� ����

	CBT_Composite_Node* Create_IceBarrier_Or_Not();

	// ���� ��ġ ȸ��
	CBT_Composite_Node* Random_Dodge();


	//// �ÿ�ȸ��
	CBT_Composite_Node* Start_Show();
	CBT_Composite_Node* Show_RotationAndNearAttack();
	CBT_Composite_Node* Show_NearAttack();
	CBT_Composite_Node* Show_FarAttack();


private:
	void Down();

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CAIController*		m_pAIControllerCom = nullptr;
	CNavMesh*			m_pNavMesh = nullptr;
	CCollider*			m_pCollider = nullptr;

	CWeapon*			m_pSword = nullptr;

	// �ǰݰ��� ���� �ð�
	_double				m_dHitTime = 0;

	//�������� Ÿ�ӵ�Ÿ �������ؼ� �����س���
	_double				m_dTimeDelta = 0;

	_bool				m_bFindPlayer = false;	// �÷��̾� �߰� ���� ����
	_bool				m_bFight = false;

private:	// �ٿ� ���¸� ���� ����
	_bool				m_bDown_Start = false;
	_bool				m_bDown_Finish = false;

	_bool				m_bDown_StartAni = false;
	_bool				m_bDown_LoopAni = false;
	_bool				m_bDown_EndAni = false;
	_double				m_dDownTime = 0;

private:
	// �浹ü�� ���� ���ٱ�
	_mat*				m_matBones[Bone_End];
	_bool				m_bAIController = true;

	// �����忡�� ���� Pos �����

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
	static CIceGirl* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};
END