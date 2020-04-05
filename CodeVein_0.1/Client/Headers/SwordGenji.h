#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CSwordGenji final : public CGameObject
{
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
	// 9. 2���� ����, �̵��Ÿ� : 1 + 1
	CBT_Composite_Node* TwoCombo_Cut();


	// ���Ÿ�
	// ������1
	CBT_Composite_Node* Throwing1();
	// ������2
	CBT_Composite_Node* Throwing2();
	// �齺�� ������, �̵��Ÿ� : 1
	CBT_Composite_Node* Throwing_BackStep();
	// �޸��鼭 ��������, �̵��Ÿ� : 7
	CBT_Composite_Node* Run_Straight_Cut();


	CBT_Composite_Node*		Start_Game();

	CBT_Composite_Node*		ChaseAndNearAttack();
	CBT_Composite_Node*		Chase();
	CBT_Composite_Node*		NearAttack();

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

	//�������� Ÿ�ӵ�Ÿ �������ؼ� �����س���
	_double				m_dTimeDelta = 0;

private:
	HRESULT Update_Bone_Of_BlackBoard();
	HRESULT Update_Value_Of_BB();

private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();

public:
	static CSwordGenji* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END