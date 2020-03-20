#include "..\Headers\BT_Loop.h"

CBT_Loop::CBT_Loop()
{
}

CBT_Loop::CBT_Loop(const CBT_Loop & rhs)
{
}

HRESULT CBT_Loop::Set_Child(CBT_Node * pNode)
{
	if (nullptr == pNode)
		return E_FAIL;

	m_pChild = pNode;

	return NO_ERROR;
}

CBT_Node::BT_NODE_STATE CBT_Loop::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	if (nullptr == m_pChild)
		return BT_NODE_STATE::FAILED;

	Start_Node(pNodeStack, bDebugging);

	if (m_iCurLoopCount > m_iMaxLoopCount)
	{
		cout << "Loop End" << endl;
			
		return End_Node(pNodeStack, BT_NODE_STATE::SUCCEEDED, bDebugging);
	}

	if (!m_bInProgress)
	{
		switch (m_pChild->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, bDebugging))
		{
		case BT_NODE_STATE::FAILED:
			++m_iCurLoopCount;
			break;
		case BT_NODE_STATE::INPROGRESS:
			return BT_NODE_STATE::INPROGRESS;
		case BT_NODE_STATE::SUCCEEDED:
		case BT_NODE_STATE::SERVICE:
			++m_iCurLoopCount;
			break;
		}
		m_eChild_State = BT_NODE_STATE::INPROGRESS;
		m_bInProgress = true;
	}
	else
	{
		switch (m_eChild_State)
		{
		case BT_NODE_STATE::FAILED:
			++m_iCurLoopCount;
			m_bInProgress = false;
			break;
		case BT_NODE_STATE::INPROGRESS:
			return BT_NODE_STATE::INPROGRESS;
		case BT_NODE_STATE::SUCCEEDED:
		case BT_NODE_STATE::SERVICE:
			++m_iCurLoopCount;
			m_bInProgress = false;
			break;
		}
	}

	return BT_NODE_STATE::INPROGRESS;
}

void CBT_Loop::Start_Node(vector<CBT_Node*>* pNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		pNodeStack->push_back(this);
		Safe_AddRef(this);

		m_iCurLoopCount = 0;
		m_bInProgress = false;
		m_bInit = false;

		if (bDebugging)
		{
			cout << "[" << m_iNodeNumber << "]" << "Loop Start" << endl;

		}
	}
}

CBT_Node::BT_NODE_STATE CBT_Loop::End_Node(vector<CBT_Node*>* pNodeStack, BT_NODE_STATE eState, _bool bDebugging)
{
	Safe_Release(pNodeStack->back());
	pNodeStack->pop_back();

	if (!pNodeStack->empty())
		Notify_Parent_Of_State(pNodeStack->back(), eState);
	
	m_bInit = true;

	if (bDebugging)
	{
		cout << "[" << m_iNodeNumber << "]" << "Loop " << m_iCurLoopCount << " : End" << endl;

	}

	return eState;
}

HRESULT CBT_Loop::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	m_iMaxLoopCount = temp.iMaxLoopCount;

	CBT_Node::Set_Auto_Number(&m_iNodeNumber);
	return NO_ERROR;
}

CBT_Loop * CBT_Loop::Create_Prototype()
{
	return new CBT_Loop;
}

CBT_Node * CBT_Loop::Clone(void * pInit_Struct)
{
	CBT_Loop* pInstance = new CBT_Loop(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
		Safe_Release(pInstance);

	return pInstance;
}

void CBT_Loop::Free()
{
	if(m_pChild)
		m_pChild->Free();

	Safe_Release(m_pChild);
}
