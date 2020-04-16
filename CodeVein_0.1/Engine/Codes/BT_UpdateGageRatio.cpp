#include "BT_UpdateGageRatio.h"

CBT_UpdateGageRatio::CBT_UpdateGageRatio()
{
}

CBT_UpdateGageRatio::CBT_UpdateGageRatio(const CBT_UpdateGageRatio & rhs)
{
}

CBT_Node::BT_NODE_STATE CBT_UpdateGageRatio::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard * pBlackBoard, _bool bDebugging)
{
	Start_Node(pNodeStack, plistSubNodeStack, false);
	
	m_dCurTime += TimeDelta;

	if (m_dCurTime > m_dMaxTime)
	{
		switch (m_eMode)
		{
			// 积己 冉荐 力茄
		case CBT_Service_Node::Finite:
			if (m_iCur_Count_Of_Execution > m_iMax_Count_Of_Execution)
				break;
			else
			{
				_float CurGage = pBlackBoard->Get_FloatValue(m_pTargetKey_CurGage);
				_float MaxGage = pBlackBoard->Get_FloatValue(m_pTargetKey_MaxGage);

				if (0 <= CurGage || 0 <= MaxGage)
					pBlackBoard->Set_Value(m_pKey_Save_GageRatio, 0);
				else
					pBlackBoard->Set_Value(m_pKey_Save_GageRatio, _float(CurGage / MaxGage));

				++m_iCur_Count_Of_Execution;
			}
			break;

			// 积己 冉荐 公力茄
		case CBT_Service_Node::Infinite:
			_float CurGage = pBlackBoard->Get_FloatValue(m_pTargetKey_CurGage);
			_float MaxGage = pBlackBoard->Get_FloatValue(m_pTargetKey_MaxGage);

			if (0 <= CurGage || 0 <= MaxGage)
				pBlackBoard->Set_Value(m_pKey_Save_GageRatio, 0);
			else
				pBlackBoard->Set_Value(m_pKey_Save_GageRatio, _float(CurGage / MaxGage));

			End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, pBlackBoard, false);
			break;
		}
	}

	return BT_NODE_STATE::INPROGRESS;
}

void CBT_UpdateGageRatio::Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { Service : HPRatio }" << endl;
		}

		m_dCurTime = 0;
		m_dMaxTime = m_dUpdateTime + CALC::Random_Num_Double(-m_dOffset, m_dOffset);
		m_iCur_Count_Of_Execution = 0;

		m_bInit = false;
	}
}

CBT_Node::BT_NODE_STATE CBT_UpdateGageRatio::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, CBlackBoard* pBlackBoard, _bool bDebugging)
{
	if (bDebugging)
	{
		Cout_Indentation(pNodeStack);
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { Service : HPRatio }" << endl;
	}

	m_bInit = true;

	return eState;
}

HRESULT CBT_UpdateGageRatio::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	strcpy_s<256>(m_pNodeName, temp.Target_NodeName);
	lstrcpy(m_pKey_Save_GageRatio, temp.Target_Save_GageRatio);
	lstrcpy(m_pTargetKey_CurGage, temp.Target_KeyCurGage);
	lstrcpy(m_pTargetKey_MaxGage, temp.Target_KeyMaxGage);
	m_dUpdateTime = temp.Target_dUpdateTime;
	m_dOffset = temp.Target_dOffset;
	m_iMax_Count_Of_Execution = temp.Count_Of_Execution;
	m_eMode = temp.eMode;

	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return S_OK;
}

CBT_UpdateGageRatio * CBT_UpdateGageRatio::Create_Prototype()
{
	return new CBT_UpdateGageRatio();
}

CBT_UpdateGageRatio * CBT_UpdateGageRatio::Clone(void * pInit_Struct)
{
	CBT_UpdateGageRatio* pInstance = new CBT_UpdateGageRatio(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_UpdateGageRatio");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBT_UpdateGageRatio::Free()
{

}
