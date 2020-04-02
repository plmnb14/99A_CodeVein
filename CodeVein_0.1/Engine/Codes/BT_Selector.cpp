#include "..\Headers\BT_Selector.h"


CBT_Selector::CBT_Selector()
{
}

CBT_Selector::CBT_Selector(const CBT_Selector & rhs)
{
}

HRESULT CBT_Selector::Add_Child(CBT_Node * pNode)
{
	if (nullptr == pNode)
		return E_FAIL;

	m_pChildren.emplace_back(pNode);
	return S_OK;
}

CBT_Node::BT_NODE_STATE CBT_Selector::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging)
{
	/*
	왼쪽부터 자식들을 검사해서 성공한 자식을 수행한다.
	*/
	Start_Node(pNodeStack, plistSubNodeStack, bDebugging);

	switch (m_eMode)
	{
	case Engine::CBT_Selector::Normal:
		if (m_pCurIndex < m_pChildren.size())
		{
			switch (m_eChild_State)
			{
			case BT_NODE_STATE::FAILED:
			case BT_NODE_STATE::SERVICE:
				return m_pChildren[m_pCurIndex++]->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging);

				//++m_pCurIndex;
				//this->m_eChild_State = BT_NODE_STATE::INPROGRESS;
				//return BT_NODE_STATE::INPROGRESS;

			case BT_NODE_STATE::INPROGRESS:
				return m_pChildren[m_pCurIndex++]->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging);

			case BT_NODE_STATE::SUCCEEDED:
				return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, bDebugging);
			}
		}
		break;

	case Engine::CBT_Selector::Random:
		switch (m_eChild_State)
		{
		case BT_NODE_STATE::FAILED:
		case BT_NODE_STATE::SERVICE:
			return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::FAILED, bDebugging);
		
		case BT_NODE_STATE::INPROGRESS:
			return m_pChildren[m_iRandomNum]->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging);
		
		case BT_NODE_STATE::SUCCEEDED:
			return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, bDebugging);
		}

		break;
	}


	return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::FAILED, bDebugging);
}

void CBT_Selector::Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { Selector } " << endl;
		}

		pNodeStack->push_back(this);
		Safe_AddRef(this);

		m_eChild_State = BT_NODE_STATE::INPROGRESS;

		m_iRandomNum = CALC::Random_Num(0, (_int)m_pChildren.size() - 1);
		m_pCurIndex = 0;
		m_bInit = false;

		// 서비스노드 쓰레드에 각각 추가
		if (!m_listServiceNodeStack.empty())
		{
			for (auto Child : m_listServiceNodeStack)
			{
				plistSubNodeStack->emplace_back(Child);
			}
		}
	}
}

CBT_Node::BT_NODE_STATE CBT_Selector::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, _bool bDebugging)
{
	m_bInit = true;

	if (pNodeStack->empty())
		return eState;

	Safe_Release(pNodeStack->back());
	pNodeStack->pop_back();

	if (!pNodeStack->empty())
		Notify_Parent_Of_State(pNodeStack->back(), eState);

	if (!m_listServiceNodeStack.empty())
		Release_ServiceNode(plistSubNodeStack, &m_listServiceNodeStack, bDebugging);

	if (bDebugging)
	{
		Cout_Indentation(pNodeStack);
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { Selector } " << endl;

	}

	return eState;
}

HRESULT CBT_Selector::Ready_Clone_Node(void* pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	strcpy_s<256>(m_pNodeName, temp.Target_NodeName);
	m_eMode = temp.eMode;

	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return S_OK;
}

CBT_Selector * CBT_Selector::Create_Prototype()
{
	return new CBT_Selector();
}

CBT_Node * CBT_Selector::Clone(void* pInit_Struct)
{
	CBT_Selector* pInstance = new CBT_Selector(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_Selector");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBT_Selector::Free()
{
	CBT_Composite_Node::Free();

	for (auto iter = m_pChildren.begin(); iter != m_pChildren.end(); )
	{
		Safe_Release(*iter);

		if (nullptr == *iter)
			iter = m_pChildren.erase(iter);
		else
			++iter;
	}

	//for (CBT_Node* pChild : m_pChildren)
	//{
	//	//pChild->Free();

	//	Safe_Release(pChild);
	//}
	//m_pChildren.clear();
}
