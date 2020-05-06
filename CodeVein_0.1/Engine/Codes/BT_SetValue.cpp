#include "../Headers/BT_SetValue.h"

CBT_SetValue::CBT_SetValue()
{
}

CBT_SetValue::CBT_SetValue(const CBT_SetValue & rhs)
{
}

CBT_Node::BT_NODE_STATE CBT_SetValue::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging)
{

	Start_Node(pNodeStack, plistSubNodeStack, bDebugging);

	switch (m_eMode)
	{
	case BOOL:
		pBlackBoard->Set_Value(m_pTarget_Key, m_bValue);
		break;
	case FLOAT:
		pBlackBoard->Set_Value(m_pTarget_Key, m_fValue);
		break;
	case INT:
		pBlackBoard->Set_Value(m_pTarget_Key, m_iValue);
		break;
	case VEC3:
		pBlackBoard->Set_Value(m_pTarget_Key, m_vValue);
		break;
	}

	return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, pBlackBoard, bDebugging);
}

void CBT_SetValue::Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { Set_Value : }" << endl;
		}

		pNodeStack->push_back(this);
		Safe_AddRef(this);

		m_bInit = false;
	}

}

CBT_Node::BT_NODE_STATE CBT_SetValue::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, CBlackBoard* pBlackBoard, _bool bDebugging)
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
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { Set_Value : }" << endl;
	}

	return eState;
}

HRESULT CBT_SetValue::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	strcpy_s<STR_128>(m_pNodeName, temp.Target_NodeName);
	lstrcpy(m_pTarget_Key, temp.Target_Key);

	m_bValue = temp.bValue;
	m_fValue = temp.fValue;
	m_iValue = temp.iValue;
	m_vValue = temp.vValue;
	m_eMode = temp.eMode;

	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return S_OK;
}


CBT_SetValue * CBT_SetValue::Create_Prototype()
{
	return new CBT_SetValue();
}

CBT_Node * CBT_SetValue::Clone(void* pInit_Struct)
{
	CBT_SetValue* pInstance = new CBT_SetValue(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_SetValue");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBT_SetValue::Free()
{
}
