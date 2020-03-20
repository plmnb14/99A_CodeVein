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
			m_pRoot->Update_Node(TimeDelta, &m_pNodeStack, &m_plistNodeStack, m_bDebuging);
	}
	else
	{
		for (auto SubNodeStack : m_plistNodeStack)
		{
			while (!SubNodeStack->empty() && CBT_Node::BT_NODE_STATE::INPROGRESS != SubNodeStack->back()->Update_Node(TimeDelta, SubNodeStack, &m_plistNodeStack, m_bDebuging));
		}


		//// 메인 스레드
		//while (!m_pNodeStack.empty() && CBT_Node::BT_NODE_STATE::INPROGRESS != m_pNodeStack.back()->Update_Node(TimeDelta, &m_pNodeStack, &m_plistNodeStack, m_bDebuging));
		//
		//// 서브 스레드
		//for (auto SubNodeStack : m_plistNodeStack)
		//{
		//	while (CBT_Node::BT_NODE_STATE::INPROGRESS != SubNodeStack->back()->Update_Node(TimeDelta, SubNodeStack, &m_plistNodeStack, m_bDebuging));
		//}
	}
}

HRESULT CBehaviorTree::Ready_BehavioTree(_bool bDebuging)
{
	m_pRoot = CBT_Root::Create();

	m_bDebuging = bDebuging;
	m_pNodeStack.reserve(20);

	m_plistNodeStack.push_back(&m_pNodeStack);


	CBT_Node::_Init_NodeNumber();
	return S_OK;
}

CBehaviorTree * CBehaviorTree::Create(_bool bDebuging)
{
	CBehaviorTree* pInstance = new CBehaviorTree();

	if (FAILED(pInstance->Ready_BehavioTree(bDebuging)))
		Safe_Release(pInstance);

	return pInstance;
}

void CBehaviorTree::Free()
{
	for (auto child : m_pNodeStack)
	{
		child->Free();
		Safe_Release(child);
	}
	m_pNodeStack.clear();

	// 모든 쓰레드안의 노드들 주소 제거
	for (auto pvecSubNode : m_plistNodeStack)
	{
		if (!pvecSubNode->empty())
		{
			//for (auto pNode : *pvecSubNode)
			//	Safe_Release(pNode);

			pvecSubNode->clear();
		}
	}
	m_plistNodeStack.clear();

	if (m_pRoot)
		m_pRoot->Free();

	Safe_Release(m_pRoot);

	//// 모든 쓰레드안의 노드들 주소 제거
	//for (auto pvecSubNode : m_plistNodeStack)
	//{
	//	//if (pvecSubNode == &m_pNodeStack)
	//	//	continue;

	//	if (!pvecSubNode->empty())
	//	{
	//		for (auto pNode : *pvecSubNode)
	//			Safe_Release(pNode);

	//		pvecSubNode->clear();
	//	}
	//}
	//m_plistNodeStack.clear();

	//Safe_Release(m_pRoot);
}
