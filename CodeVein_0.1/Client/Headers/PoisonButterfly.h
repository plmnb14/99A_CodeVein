#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CPoisonButterfly final : public CGameObject
{
protected:
	explicit CPoisonButterfly(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CPoisonButterfly(const CPoisonButterfly& rhs);
	virtual ~CPoisonButterfly() = default;

public:
	

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:	//���ϵ�
	// �ٰŸ�
	// 1. ���� ��
	CBT_Composite_Node* Left_Eat();
	// 2. ������ ��
	CBT_Composite_Node* Right_Eat();
	// 3. ��-> ���׸�����
	CBT_Composite_Node* Eat_Turn_Eat();
	// 4. ���Ƽ� �� �ҿ뵹��
	CBT_Composite_Node* Poison_Tornado_After_Charging();
	// 5. ���� ���� �ѹ��� �� �߻�
	CBT_Composite_Node* OneTurn_Poison();

	// ���Ÿ�
	// 1. ����
	CBT_Composite_Node* Rush();
	// 2. ����ź
	CBT_Composite_Node* Fire_5Bullet();
	// 3. ���� ��
	CBT_Composite_Node* Fire_ChaseBullet();
	// 4. Ʈ���ٹ̾�
	CBT_Composite_Node* WhirlWind();

	//
	/*
	���� ���� -> ���� ����  -> �ٰŸ� ����, ���� �� ���� or �׳� �� �ڸ����� ����
							-> ���Ÿ� ����, ���ڸ����� ����
	*/
	
	CBT_Composite_Node*		NearAttack();	//���� ����
	CBT_Composite_Node*		FarAttack();	//���� ����


	CBT_Composite_Node*		Start_Game();


	//////////////////// �ÿ�ȸ��

	CBT_Composite_Node*		Start_Show();

	// �þ߰����� ������ ����(ù ���Ϻ��� ���ʴ��), ������ ����
	CBT_Composite_Node*		Show_ChaseAndNearAttack();
	CBT_Composite_Node*		Show_TurnAndFarAttack();
	// ���� ������� �ٰŸ� ����
	CBT_Composite_Node*		Show_NearAttack();
	// ���� ������� ���Ÿ� ����
	CBT_Composite_Node*		Show_FarAttack();

	//////////////////// 

private:
	HRESULT Update_Bone_Of_BlackBoard();
	HRESULT Update_Value_Of_BB();

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CAIController*		m_pAIControllerCom = nullptr;
	CCollider*			m_pCollider = nullptr;


	//�������� Ÿ�ӵ�Ÿ �������ؼ� �����س���
	_double				m_dTimeDelta = 0;

private:	// ���� Pos �����
	_v3					m_vTail = _v3(0.f, 0.f, 0.f);	//Tail6
	_v3					m_vBody = _v3(0.f, 0.f, 0.f);	//Spine2

private:
	

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CPoisonButterfly* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END