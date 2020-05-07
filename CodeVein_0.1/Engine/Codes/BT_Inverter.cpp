#include "../Headers/BT_Inverter.h"

CBT_Inverter::CBT_Inverter()
{
}

CBT_Inverter::CBT_Inverter(const CBT_Inverter & rhs)
{
}

HRESULT CBT_Inverter::Set_Child(CBT_Node * pNode)
{
	if (nullptr == pNode)
		return E_FAIL;

	m_pChild = pNode;

	return NO_ERROR;
}

CBT_Node::BT_NODE_STATE CBT_Inverter::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging)
{
	/*
	자식노드 결과의 반대값을 반환합니다.
	*/
	if (nullptr == m_pChild)
		return BT_NODE_STATE::FAILED;

	Start_Node(pNodeStack, plistSubNodeStack, bDebugging);

	// 최초 평가
	if (!m_bInProgress)
	{
		switch (m_pChild->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging))
		{
		case BT_NODE_STATE::SERVICE:
		case BT_NODE_STATE::FAILED:
			return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, pBlackBoard, bDebugging);

		case BT_NODE_STATE::INPROGRESS:
			break;

		case BT_NODE_STATE::SUCCEEDED:
			return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::FAILED, pBlackBoard, bDebugging);
		}

		m_bInProgress = true;
	}
	// 진행중 평가
	else
	{
		switch (m_eChild_State)
		{
		case BT_NODE_STATE::SERVICE:
		case BT_NODE_STATE::FAILED:
			return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, pBlackBoard, bDebugging);

		case BT_NODE_STATE::INPROGRESS:
			return BT_NODE_STATE::INPROGRESS;

		case BT_NODE_STATE::SUCCEEDED:
			return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::FAILED, pBlackBoard, bDebugging);
		}
	}

	return BT_NODE_STATE::INPROGRESS;
}

void CBT_Inverter::Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { Inverter } " << endl;
		}

		pNodeStack->push_back(this);
		Safe_AddRef(this);

		m_eChild_State = BT_NODE_STATE::INPROGRESS;

		m_bInProgress = false;
		m_bInit = false;
	}
}

CBT_Node::BT_NODE_STATE CBT_Inverter::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, CBlackBoard* pBlackBoard, _bool bDebugging)
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
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { Inverter } " << endl;
	}

	return eState;
}

HRESULT CBT_Inverter::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	strcpy_s<STR_128>(m_pNodeName, temp.Target_NodeName);

	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return NO_ERROR;
}

CBT_Inverter * CBT_Inverter::Create_Prototype()
{
	return new CBT_Inverter;
}

CBT_Node * CBT_Inverter::Clone(void * pInit_Struct)
{
	CBT_Inverter* pInstance = new CBT_Inverter(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_Inverter");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBT_Inverter::Free()
{
	//if(m_pChild)
	//	m_pChild->Free();

	Safe_Release(m_pChild);
}
