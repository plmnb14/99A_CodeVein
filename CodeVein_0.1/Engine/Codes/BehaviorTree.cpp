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

void CBehaviorTree::Update_BeHaviorTree(_double TimeDelta, const CBlackBoard* pBlackBoard)
{
	if (m_pNodeStack.empty())
	{
		if (m_pRoot)
			m_pRoot->Update_Node(TimeDelta, &m_pNodeStack, &m_plistNodeStack, pBlackBoard, m_bDebuging);
	}
	else
	{
		for (auto iter = m_plistNodeStack.begin(); iter != m_plistNodeStack.end();)//   auto SubNodeStack : m_plistNodeStack)
		{
			//INPROGRESS ��ȯ�ϸ� while�� ��� ����
			while (!(*iter)->empty() && CBT_Node::BT_NODE_STATE::INPROGRESS != (*iter)->back()->Update_Node(TimeDelta, (*iter), &m_plistNodeStack, pBlackBoard, m_bDebuging));
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

		
	}
}

HRESULT CBehaviorTree::Ready_BehaviorTree(_bool bDebuging)
{
	m_pRoot = CBT_Root::Create();

	m_bDebuging = bDebuging;
	m_pNodeStack.reserve(20);

	// ����Ʈ 0��°�� ������ ��彺���� �־��ش�.
	m_plistNodeStack.push_back(&m_pNodeStack);


	CBT_Node::_Init_NodeNumber();
	return S_OK;
}

CBehaviorTree * CBehaviorTree::Create(_bool bDebuging)
{
	CBehaviorTree* pInstance = new CBehaviorTree();

	if (FAILED(pInstance->Ready_BehaviorTree(bDebuging)))
		Safe_Release(pInstance);

	return pInstance;
}

void CBehaviorTree::Free()
{
	// ������ ������ �ִ� ������ ������ ���� �����.
	for (auto child : m_pNodeStack)
	{
		//child->Free();
		Safe_Release(child);
	}
	m_pNodeStack.clear();

	//// ��� ��������� ���� �ּҸ� clear
	//for (auto pvecSubNode : m_plistNodeStack)
	//{
	//	if (!pvecSubNode->empty())
	//	{
	//		pvecSubNode->clear();
	//	}
	//}
	//m_plistNodeStack.clear();

	//�������� ��Ʈ���� ��� ��带 ��ȸ�ؼ� �����.
	//if (m_pRoot)
	//	m_pRoot->Free();

	Safe_Release(m_pRoot);
}
