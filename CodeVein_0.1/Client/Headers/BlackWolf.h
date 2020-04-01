#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CBlackWolf final : public CGameObject
{
protected:
	explicit CBlackWolf(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBlackWolf(const CBlackWolf& rhs);
	virtual ~CBlackWolf() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	static CBlackWolf* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT Update_Bone_Of_BlackBoard();
	HRESULT Update_Value_Of_BB();

	// 1. �������� == �����ϸ鼭 ��
	CBT_Composite_Node* Frisbee();
	// 2. ��ȸ�� �� ����
	CBT_Composite_Node* JumpDodgeL_And_Bite();
	// 3. ��ȸ�� �� ����
	CBT_Composite_Node* JumpDodgeR_And_Bite();
	// 4. �� �� ��
	CBT_Composite_Node* Bite_LeftRightLeft();

	//Near
	//�þ߰� �̳��� ��� == 4���� 1�� ����
	//�þ߰� ���� ��� == ȸ��

	//Far
	//�þ߰� �̳��� ��� == ����
	//�þ߰� ���� ��� == �ϻ��ൿ

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CAIController*		m_pAIControllerCom = nullptr;

	//�������� Ÿ�ӵ�Ÿ �������ؼ� �����س���
	_double				m_dTimeDelta = 0;
	_v3					m_vHeadTeeth = _v3(0.f, 0.f, 0.f);	//�Ӹ�
	_v3					m_vBody = _v3(0.f, 0.f, 0.f);	//ô��


};

END