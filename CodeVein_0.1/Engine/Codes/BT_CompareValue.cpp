#include "..\Headers\BT_CompareValue.h"

CBT_CompareValue::CBT_CompareValue()
{
}

CBT_CompareValue::CBT_CompareValue(const CBT_CompareValue & rhs)
{
}

HRESULT CBT_CompareValue::Set_Child(CBT_Node * pNode)
{
	if (nullptr == pNode)
		return E_FAIL;

	m_pChild = pNode;

	return S_OK;
}

CBT_Node::BT_NODE_STATE CBT_CompareValue::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard * pBlackBoard, _bool bDebugging)
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
		case Mode::EQUAL:

			switch (m_eValue)
			{
			case Value::BOOL:
				m_bTargetBool = pBlackBoard->Get_BoolValue(m_pTargetKey_A);

				if (m_bTargetBool == m_bValue)
					return m_pChild->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging);

				break;
			case Value::FLOAT:
				m_fTargetValue = pBlackBoard->Get_FloatValue(m_pTargetKey_A);

				if (abs(m_fTargetValue - m_fValue) < 0.0001)
					return m_pChild->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging);

				break;
			case Value::INT:
				m_iTargetValue = pBlackBoard->Get_IntValue(m_pTargetKey_A);

				if (m_iTargetValue == m_iValue)
					return m_pChild->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging);

				break;
			}

			return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::FAILED, pBlackBoard, bDebugging);

		case Mode::NOT_EQUAL:

			switch (m_eValue)
			{
			case Value::BOOL:
				m_bTargetBool = pBlackBoard->Get_BoolValue(m_pTargetKey_A);

				if (m_bTargetBool != m_bValue)
					return m_pChild->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging);

				break;
			case Value::FLOAT:
				m_fTargetValue = pBlackBoard->Get_FloatValue(m_pTargetKey_A);

				if (m_fTargetValue != m_fValue)
					return m_pChild->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging);

				break;
			case Value::INT:
				m_iTargetValue = pBlackBoard->Get_IntValue(m_pTargetKey_A);

				if (m_iTargetValue != m_iValue)
					return m_pChild->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging);

				break;
			}

			return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::FAILED, pBlackBoard, bDebugging);

		case Mode::GREATER_THAN:

			switch (m_eValue)
			{
			case Value::BOOL:

				break;
			case Value::FLOAT:
				m_fTargetValue = pBlackBoard->Get_FloatValue(m_pTargetKey_A);

				if (m_fTargetValue > m_fValue)
					return m_pChild->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging);

				break;
			case Value::INT:
				m_iTargetValue = pBlackBoard->Get_IntValue(m_pTargetKey_A);

				if (m_iTargetValue > m_iValue)
					return m_pChild->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging);

				break;
			}

			return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::FAILED, pBlackBoard, bDebugging);

		case Mode::SMALLER_THAN:

			switch (m_eValue)
			{
			case Value::BOOL:

				break;
			case Value::FLOAT:
				m_fTargetValue = pBlackBoard->Get_FloatValue(m_pTargetKey_A);

				if (m_fTargetValue < m_fValue)
					return m_pChild->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging);

				break;
			case Value::INT:
				m_iTargetValue = pBlackBoard->Get_IntValue(m_pTargetKey_A);

				if (m_iTargetValue < m_iValue)
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

void CBT_CompareValue::Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
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

CBT_Node::BT_NODE_STATE CBT_CompareValue::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, CBlackBoard* pBlackBoard, _bool bDebugging)
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

HRESULT CBT_CompareValue::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	strcpy_s<STR_128>(m_pNodeName, temp.Target_NodeName);
	m_eValue = temp.eValue;
	lstrcpy(m_pTargetKey_A, temp.TargetKey_A);

	m_bValue = temp.bValue;
	m_fValue = temp.fValue;
	m_iValue = temp.iValue;

	m_eMode = temp.eMode;

	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return S_OK;
}

CBT_CompareValue * CBT_CompareValue::Create_Prototype()
{
	return new CBT_CompareValue();
}

CBT_Node * CBT_CompareValue::Clone(void * pInit_Struct)
{
	CBT_CompareValue* pInstance = new CBT_CompareValue(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_CompareValue");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBT_CompareValue::Free()
{
	Safe_Release(m_pChild);
}
