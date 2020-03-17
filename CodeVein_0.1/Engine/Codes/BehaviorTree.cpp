#include "../Headers/BehaviorTree.h"
#include "../Headers/BT_Root.h"

CBehaviorTree::CBehaviorTree()
{
}

HRESULT CBehaviorTree::Set_Child(CBT_Composite_Node * pComposite_Node)
{
	m_pRoot->Set_Child(pComposite_Node);

	return S_OK;
}

HRESULT CBehaviorTree::Set_Child(CBT_Task_Node * pTask_Node)
{
	m_pRoot->Set_Child(pTask_Node);

	return S_OK;
}

void CBehaviorTree::Update_BeHaviorTree(_double TimeDelta)
{
	if (m_pNodeStack.empty())
	{
		if (m_pRoot)
			m_pRoot->Update_Node(TimeDelta, &m_pNodeStack);
	}
	else
		m_pNodeStack.back()->Update_Node(TimeDelta, &m_pNodeStack);
}

HRESULT CBehaviorTree::Ready_BehavioTree()
{
	m_pRoot = CBT_Root::Create();

	m_pNodeStack.reserve(20);

	return S_OK;
}

CBehaviorTree * CBehaviorTree::Create()
{
	CBehaviorTree* pInstance = new CBehaviorTree();

	if (FAILED(pInstance->Ready_BehavioTree()))
		Safe_Release(pInstance);

	return pInstance;
}

void CBehaviorTree::Free()
{
	m_pNodeStack.clear();

	if(m_pRoot)
		m_pRoot->Free();

	Safe_Release(m_pRoot);
}
