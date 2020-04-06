#include "..\Headers\\BT_Composite_Node.h"
#include "..\Headers\BT_Service_Node.h"

CBT_Composite_Node::CBT_Composite_Node()
{
}

HRESULT CBT_Composite_Node::Add_Service(CBT_Service_Node * pNode)
{
	if (nullptr == pNode)
	{
		MSG_BOX("Sequence : Failed To Add_Service");
		return E_FAIL;
	}

	vector<CBT_Node*>*	pvecNode = new vector<CBT_Node*>;
	pvecNode->push_back(pNode);

	m_listServiceNodeStack.emplace_back(pvecNode);

	return S_OK;
}

HRESULT CBT_Composite_Node::Release_ServiceNode(list<vector<CBT_Node*>*>* m_plistNodeStack, list<vector<CBT_Node*>*>* plistServiceNode, CBlackBoard* pBlackBoard, _bool bDebugging)
{
	for (auto iter_service = plistServiceNode->begin(); iter_service != plistServiceNode->end(); ++iter_service)
	{
		for (auto iter = m_plistNodeStack->begin(); iter != m_plistNodeStack->end(); ++iter)
		{
			// 서비스에서 벡터공간 == 리스트에서 벡터공간
			if ((*iter_service) == *iter)
			{
				(*iter)->front()->End_Node(*iter_service, m_plistNodeStack, BT_NODE_STATE::SUCCEEDED, pBlackBoard, bDebugging);

				//메인진행리스트에서 서비스벡터 공간 삭제
				m_plistNodeStack->erase(iter);

				break;
			}

		}

		//Safe_Delete(*iter_service);

		////if (*iter == pServiceNode)
		////{
		////	(*iter)->front()->End_Node(pServiceNode, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, bDebugging);
		////	plistSubNodeStack->erase(iter);
		////	return S_OK;
		////}
	}

	return S_OK;
}

void CBT_Composite_Node::Free()
{
	for (auto vecNode : m_listServiceNodeStack)
	{
		for (auto Child : *vecNode)
		{
			Safe_Release(Child);
		}
		vecNode->clear();
		Safe_Delete(vecNode);
	}
	m_listServiceNodeStack.clear();

	CBT_Node::Free();
}
