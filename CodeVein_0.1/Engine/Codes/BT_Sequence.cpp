#include "..\Headers\BT_Sequence.h"


CBT_Sequence::CBT_Sequence()
{
}

CBT_Sequence::CBT_Sequence(const CBT_Sequence & rhs)
{
}

HRESULT CBT_Sequence::Add_Child(CBT_Node * pNode)
{
	if (nullptr == pNode)
	{
		MSG_BOX("Sequence : Failed To Add_Child");
		return E_FAIL;
	}
	m_pChildren.emplace_back(pNode);
	return S_OK;
}

CBT_Node::BT_NODE_STATE CBT_Sequence::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	/*
	왼쪽부터 자식들을 검사해서 실패할 때가지 수행한다.
	*/
	Start_Node(pNodeStack, bDebugging);
	
	if(m_pCurIndex < m_pChildren.size())
	//for (CBT_Node* pChilde : m_pChildren)
	{
		switch (m_eChild_State)
		{
			case BT_NODE_STATE::FAILED:
				return End_Node(pNodeStack, BT_NODE_STATE::FAILED, bDebugging);

			case BT_NODE_STATE::INPROGRESS:
				return m_pChildren[m_pCurIndex++]->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, bDebugging);

			case BT_NODE_STATE::SUCCEEDED:
			case BT_NODE_STATE::SERVICE:
				//this->m_eChild_State = BT_NODE_STATE::INPROGRESS;
				return m_pChildren[m_pCurIndex++]->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, bDebugging);
		}

	}

	
	return End_Node(pNodeStack, BT_NODE_STATE::SUCCEEDED, bDebugging);
}

void CBT_Sequence::Start_Node(vector<CBT_Node*>* pNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { Sequence }" << endl;
		}

		pNodeStack->push_back(this);
		Safe_AddRef(this);

		m_eChild_State = BT_NODE_STATE::INPROGRESS;

		m_pCurIndex = 0;
		m_bInit = false;


	}
}

CBT_Node::BT_NODE_STATE CBT_Sequence::End_Node(vector<CBT_Node*>* pNodeStack, BT_NODE_STATE eState, _bool bDebugging)
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
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { Sequence }" << endl;
	}

	return eState;
}

HRESULT CBT_Sequence::Ready_Clone_Node(void* pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	strcpy_s<256>(m_pNodeName, temp.Target_NodeName);

	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return S_OK;
}

CBT_Sequence * CBT_Sequence::Create_Prototype()
{
	return new CBT_Sequence();
}

CBT_Node * CBT_Sequence::Clone(void* pInit_Struct)
{
	CBT_Sequence* pInstance = new CBT_Sequence(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
		Safe_Release(pInstance);

	return pInstance;
}

void CBT_Sequence::Free()
{
	for (CBT_Node* pChild : m_pChildren)
	{
		pChild->Free();
		Safe_Release(pChild);
	}
	m_pChildren.clear();
}
