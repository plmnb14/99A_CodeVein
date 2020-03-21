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
		for (auto iter = m_plistNodeStack.begin(); iter != m_plistNodeStack.end();)//   auto SubNodeStack : m_plistNodeStack)
		{
			//INPROGRESS 반환하면 while문 계속 진행
			while (!(*iter)->empty() && CBT_Node::BT_NODE_STATE::INPROGRESS != (*iter)->back()->Update_Node(TimeDelta, (*iter), &m_plistNodeStack, m_bDebuging));
			//while (!SubNodeStack->empty() && CBT_Node::BT_NODE_STATE::INPROGRESS != SubNodeStack->back()->Update_Node(TimeDelta, SubNodeStack, &m_plistNodeStack, m_bDebuging));

			if ((*iter) == &m_pNodeStack)
				++iter;
			else
			{
				if ((*iter)->empty())
					iter = m_plistNodeStack.erase(iter);
				else
					++iter;
			}
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
	// 본인이 가지고 있던 스택은 본인이 직접 지운다.
	for (auto child : m_pNodeStack)
	{
		child->Free();
		Safe_Release(child);
	}
	m_pNodeStack.clear();

	// 모든 쓰레드안의 노드들 주소만 clear
	for (auto pvecSubNode : m_plistNodeStack)
	{
		if (!pvecSubNode->empty())
		{
			pvecSubNode->clear();
		}
	}
	m_plistNodeStack.clear();

	//최종으로 루트부터 모든 노드를 순회해서 지운다.
	if (m_pRoot)
		m_pRoot->Free();

	Safe_Release(m_pRoot);
}
