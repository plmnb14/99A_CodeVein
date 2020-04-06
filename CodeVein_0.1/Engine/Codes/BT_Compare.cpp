#include "..\Headers\BT_Compare.h"

CBT_Compare::CBT_Compare()
{
}

CBT_Compare::CBT_Compare(const CBT_Compare & rhs)
{
}

HRESULT CBT_Compare::Set_Child(CBT_Node * pNode)
{
	if (nullptr == pNode)
		return E_FAIL;

	m_pChild = pNode;

	return S_OK;
}

CBT_Node::BT_NODE_STATE CBT_Compare::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard * pBlackBoard, _bool bDebugging)
{
	Start_Node(pNodeStack, plistSubNodeStack, bDebugging);

	/*
	Value 상태 확인 -> 조건 확인
	*/

	//최초 평가
	if (!m_bInProgress)
	{
		m_bInProgress = true;

		switch (m_eMode)
		{
		case Value::BOOL:
			m_bTargetBool[0] = pBlackBoard->Get_BoolValue(m_pTargetKey_A);
			m_bTargetBool[1] = pBlackBoard->Get_BoolValue(m_pTargetKey_B);

			switch (m_eValue)
			{
			case Mode::EQUAL:
				if (m_bTargetBool[0] == m_bTargetBool[1])
					return m_pChild->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging);

				break;
			case Mode::NOT_EQUAL:
				if (m_bTargetBool[0] != m_bTargetBool[1])
					return m_pChild->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging);

				break;
			case Mode::GREATER_THAN:
			case Mode::SMALLER_THAN:
				break;
			}

			return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::FAILED, pBlackBoard, bDebugging);

		case Value::FLOAT:
			m_fTargetValue[0] = pBlackBoard->Get_FloatValue(m_pTargetKey_A);
			m_fTargetValue[1] = pBlackBoard->Get_FloatValue(m_pTargetKey_B);

			switch (m_eValue)
			{
			case Mode::EQUAL:
				if ( abs( m_fTargetValue[0] - m_fTargetValue[1]) < 0.0001)
					return m_pChild->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging);

				break;
			case Mode::NOT_EQUAL:
				if (m_fTargetValue[0] != m_fTargetValue[1])
					return m_pChild->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging);

				break;
			case Mode::GREATER_THAN:
				if (m_fTargetValue[0] > m_fTargetValue[1])
					return m_pChild->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging);

				break;
			case Mode::SMALLER_THAN:
				if (m_fTargetValue[0] < m_fTargetValue[1])
					return m_pChild->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging);

				break;
			}

			return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::FAILED, pBlackBoard, bDebugging);

		case Value::INT:
			m_iTargetValue[0] = pBlackBoard->Get_IntValue(m_pTargetKey_A);
			m_iTargetValue[1] = pBlackBoard->Get_IntValue(m_pTargetKey_B);
			
			switch (m_eValue)
			{
			case Mode::EQUAL:
				if (m_iTargetValue[0] == m_iTargetValue[1])
					return m_pChild->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging);

				break;
			case Mode::NOT_EQUAL:
				if (m_iTargetValue[0] != m_iTargetValue[1])
					return m_pChild->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging);

				break;
			case Mode::GREATER_THAN:
				if (m_iTargetValue[0] > m_iTargetValue[1])
					return m_pChild->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging);

				break;
			case Mode::SMALLER_THAN:
				if (m_iTargetValue[0] < m_iTargetValue[1])
					return m_pChild->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging);

				break;
			}

			return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::FAILED, pBlackBoard, bDebugging);

		}
	}
	else
	{
		switch (m_eChild_State)
		{
		case BT_NODE_STATE::SERVICE:
		case BT_NODE_STATE::FAILED:
			return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::FAILED, pBlackBoard, bDebugging);

		case BT_NODE_STATE::INPROGRESS:
			return BT_NODE_STATE::INPROGRESS;

		case BT_NODE_STATE::SUCCEEDED:
			return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, pBlackBoard, bDebugging);
		}
	}

	return BT_NODE_STATE::INPROGRESS;
}

void CBT_Compare::Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { Compare }" << endl;
		}

		pNodeStack->push_back(this);
		Safe_AddRef(this);

		m_eChild_State = BT_NODE_STATE::INPROGRESS;
		m_bInProgress = false;
		m_bInit = false;
	}
}

CBT_Node::BT_NODE_STATE CBT_Compare::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, CBlackBoard* pBlackBoard, _bool bDebugging)
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
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { Compare }" << endl;
	}

	return eState;
}

HRESULT CBT_Compare::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	strcpy_s<256>(m_pNodeName, temp.Target_NodeName);
	m_eValue = temp.eValue;
	lstrcpy(m_pTargetKey_A, temp.TargetKey_A);
	lstrcpy(m_pTargetKey_B, temp.TargetKey_B);
	m_eMode = temp.eMode;

	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return S_OK;
}

CBT_Compare * CBT_Compare::Create_Prototype()
{
	return new CBT_Compare();
}

CBT_Node * CBT_Compare::Clone(void * pInit_Struct)
{
	CBT_Compare* pInstance = new CBT_Compare(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_Compare");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBT_Compare::Free()
{
	Safe_Release(m_pChild);
}
