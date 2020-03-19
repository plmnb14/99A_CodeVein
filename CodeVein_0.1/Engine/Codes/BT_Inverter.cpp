#include "../Headers/BT_Inverter.h"

CBT_Inverter::CBT_Inverter()
{
}

CBT_Inverter::CBT_Inverter(const CBT_Inverter & rhs)
{
}

HRESULT CBT_Inverter::Set_Child(CBT_Node * pNode)
{
	if (nullptr == pNode)
		return E_FAIL;

	m_pChild = pNode;

	return NO_ERROR;
}

CBT_Node::BT_NODE_STATE CBT_Inverter::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	/*
	�ڽĳ�� ����� �ݴ밪�� ��ȯ�մϴ�.
	*/
	if (nullptr == m_pChild)
		return BT_NODE_STATE::FAILED;

	Start_Node(pNodeStack, bDebugging);

	// ���� ��
	if (!m_bInProgress)
	{
		switch (m_pChild->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, bDebugging))
		{
		case BT_NODE_STATE::SERVICE:
		case BT_NODE_STATE::FAILED:
			return End_Node(pNodeStack, BT_NODE_STATE::SUCCEEDED, bDebugging);

		case BT_NODE_STATE::INPROGRESS:
			return BT_NODE_STATE::INPROGRESS;

		case BT_NODE_STATE::SUCCEEDED:
			return End_Node(pNodeStack, BT_NODE_STATE::FAILED, bDebugging);
		}

		m_bInProgress = true;
	}
	// ������ ��
	else
	{
		switch (m_eChild_State)
		{
		case BT_NODE_STATE::SERVICE:
		case BT_NODE_STATE::FAILED:
			return End_Node(pNodeStack, BT_NODE_STATE::SUCCEEDED, bDebugging);

		case BT_NODE_STATE::INPROGRESS:
			return BT_NODE_STATE::INPROGRESS;

		case BT_NODE_STATE::SUCCEEDED:
			return End_Node(pNodeStack, BT_NODE_STATE::FAILED, bDebugging);
		}
	}

	return BT_NODE_STATE::INPROGRESS;
}

void CBT_Inverter::Start_Node(vector<CBT_Node*>* pNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		pNodeStack->push_back(this);
		Safe_AddRef(this);

		m_bInProgress = false;
		m_bInit = false;

		if (bDebugging)
		{
			cout << "Inverter Start" << endl;
		}
	}
}

CBT_Node::BT_NODE_STATE CBT_Inverter::End_Node(vector<CBT_Node*>* pNodeStack, BT_NODE_STATE eState, _bool bDebugging)
{
	Safe_Release(pNodeStack->back());
	pNodeStack->pop_back();

	if (!pNodeStack->empty())
		Notify_Parent_Of_State(pNodeStack->back(), eState);

	m_bInit = true;

	if (bDebugging)
	{
		cout << "Inverter End" << endl;
	}

	return eState;
}

HRESULT CBT_Inverter::Ready_Clone_Node(void * pInit_Struct)
{

	CBT_Node::Set_Auto_Number(&m_iNodeNumber);
	return NO_ERROR;
}

CBT_Inverter * CBT_Inverter::Create_Prototype()
{
	return new CBT_Inverter;
}

CBT_Node * CBT_Inverter::Clone(void * pInit_Struct)
{
	CBT_Inverter* pInstance = new CBT_Inverter(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
		Safe_Release(pInstance);

	return pInstance;
}

void CBT_Inverter::Free()
{
	if(m_pChild)
		m_pChild->Free();

	Safe_Release(m_pChild);
}
