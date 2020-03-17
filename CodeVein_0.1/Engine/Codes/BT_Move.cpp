#include "../Headers/BT_Move.h"
#include "../Headers/Transform.h"

CBT_Move::CBT_Move()
{
}

CBT_Move::CBT_Move(const CBT_Move & rhs)
{
}

CBT_Node::BT_NODE_STATE CBT_Move::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack)
{
	Start_Node(pNodeStack);

	m_dCurTime += TimeDelta;

	if (m_dCurTime > m_dMovingTime)
	{
		End_Node(pNodeStack);
		return BT_NODE_STATE::SUCCEEDED;
	}
	m_pTargetTransform->Add_Pos(_float(1.f * TimeDelta));

	return BT_NODE_STATE::INPROGRESS;
}

void CBT_Move::Start_Node(vector<CBT_Node*>* pNodeStack)
{
	if (m_bInit)
	{
		pNodeStack->push_back(this);


		m_bInit = false;
	}
}

void CBT_Move::End_Node(vector<CBT_Node*>* pNodeStack)
{
	pNodeStack->pop_back();
	m_bInit = true;

	m_dCurTime = 0;
}

HRESULT CBT_Move::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	m_dMovingTime = temp.m_dMovingTime;
	m_pTargetTransform = temp.m_pTargetTransform;
	Safe_AddRef(m_pTargetTransform);

	return S_OK;
}

CBT_Move * CBT_Move::Create_Prototype()
{
	return new CBT_Move();
}

CBT_Node * CBT_Move::Clone(void* pInit_Struct)
{
	CBT_Move* pInstance = new CBT_Move(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_Move");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBT_Move::Free()
{
	Safe_Release(m_pTargetTransform);
}
