#include "../Headers/BehaviorTree.h"
#include "../Headers/BT_Root.h"

CBehaviorTree::CBehaviorTree()
{
}

HRESULT CBehaviorTree::Set_Child(CBT_Node* pNode)
{
	m_pRoot->Set_Child(pNode);

	return S_OK;
}


void CBehaviorTree::Update_BeHaviorTree(_double TimeDelta, CBlackBoard* pBlackBoard)
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
			//INPROGRESS 반환하면 while문 계속 진행
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

	// 리스트 0번째에 본인의 노드스택을 넣어준다.
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
	// 본인이 가지고 있던 스택은 본인이 직접 지운다.
	for (auto child : m_pNodeStack)
	{
		Safe_Release(child);
	}
	m_pNodeStack.clear();



	// 루트부터 모든 노드를 순회해서 지운다.
	Safe_Release(m_pRoot);

	
	// 쿨다운 노드 검색 후 그 노드만 지워줌
	for (auto pvecSubNode : m_plistNodeStack)
	{
		if(pvecSubNode->empty())
			continue;

		Safe_Release((*pvecSubNode)[1]);
		//for (auto aaaa : *pvecSubNode)
		//{
		//	Safe_Release(aaaa);
		//	if (pvecSubNode->empty())
		//		break;
		//}
	}
	m_plistNodeStack.clear();
}
