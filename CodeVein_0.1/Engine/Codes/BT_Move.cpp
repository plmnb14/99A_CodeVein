#include "../Headers/BT_Move.h"
#include "../Headers/Transform.h"

CBT_Move::CBT_Move()
{
}

CBT_Move::CBT_Move(const CBT_Move & rhs)
{
}

CBT_Node::BT_NODE_STATE CBT_Move::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	Start_Node(pNodeStack, bDebugging);

	m_dCurTime += TimeDelta;

	if (m_dCurTime > m_dMovingTime)
	{
		return End_Node(pNodeStack, BT_NODE_STATE::SUCCEEDED, bDebugging);
	}
	m_pTransform->Add_Pos(_float(m_dMoveSpeed * TimeDelta));

	return BT_NODE_STATE::INPROGRESS;
}

void CBT_Move::Start_Node(vector<CBT_Node*>* pNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		pNodeStack->push_back(this);
		Safe_AddRef(this);

		m_dCurTime = 0;

		m_bInit = false;

		if (bDebugging)
		{
			cout << "[" << m_iNodeNumber << "]" << "Move Start" << endl;
		}
	}
}

CBT_Node::BT_NODE_STATE CBT_Move::End_Node(vector<CBT_Node*>* pNodeStack, BT_NODE_STATE eState, _bool bDebugging)
{
	if (pNodeStack->empty())
		return eState;

	Safe_Release(pNodeStack->back());
	pNodeStack->pop_back();

	if (!pNodeStack->empty())
		Notify_Parent_Of_State(pNodeStack->back(), eState);
	m_bInit = true;

	if (bDebugging)
	{
		cout << "[" << m_iNodeNumber << "]" << "Move End" << endl;
	}

	return eState;
}

HRESULT CBT_Move::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	m_pTransform = temp.Target_pTargetTransform;
	Safe_AddRef(m_pTransform);

	m_dMoveSpeed = temp.Target_dMoveSpeed;
	m_dMovingTime = temp.Target_dMovingTime;

	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
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
	Safe_Release(m_pTransform);
}
