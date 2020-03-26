#include "..\Headers\BT_Root.h"

CBT_Root::CBT_Root()
{
}

HRESULT CBT_Root::Set_Child(CBT_Composite_Node * pComposite_Node)
{
	if (nullptr == pComposite_Node)
		return E_FAIL;

	m_pChildNode = (CBT_Node*)pComposite_Node;

	return S_OK;
}

HRESULT CBT_Root::Set_Child(CBT_Task_Node * pTask_Node)
{
	if (nullptr == pTask_Node)
		return E_FAIL;

	m_pChildNode = (CBT_Node*)pTask_Node;

	return S_OK;
}

HRESULT CBT_Root::Set_Child(CBT_Decorator_Node * pDecorator_Node)
{
	if (nullptr == pDecorator_Node)
		return E_FAIL;

	m_pChildNode = (CBT_Node*)pDecorator_Node;

	return S_OK;
}

CBT_Node::BT_NODE_STATE CBT_Root::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging)
{
	//Start_Node(pNodeStack, bDebugging);
	
	if (m_pChildNode)
	{	
		m_pChildNode->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging);
		//return End_Node(pNodeStack, BT_NODE_STATE::SUCCEEDED, bDebugging);
	}

	return BT_NODE_STATE::SUCCEEDED;
}

void CBT_Root::Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { root } " << endl;
		}

		pNodeStack->push_back(this);
		Safe_AddRef(this);

		m_bInit = false;
	}
}

CBT_Node::BT_NODE_STATE CBT_Root::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, _bool bDebugging)
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
		Cout_Indentation(pNodeStack);
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { Selector } " << endl;

	}

	return eState;
}

CBT_Root * CBT_Root::Create()
{
	return new CBT_Root;
}

void CBT_Root::Free()
{
	if(m_pChildNode)
		m_pChildNode->Free();
	
	Safe_Release(m_pChildNode);
	
	CBT_Node::Free();
}
