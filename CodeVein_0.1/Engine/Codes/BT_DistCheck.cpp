#include "..\Headers\BT_DistCheck.h"
#include "..\Headers\Transform.h"

CBT_DistCheck::CBT_DistCheck()
{
}

CBT_DistCheck::CBT_DistCheck(const CBT_DistCheck & rhs)
{
}

HRESULT CBT_DistCheck::Set_Child(CBT_Node * pNode)
{
	if (nullptr == pNode)
		return E_FAIL;

	m_pChild = pNode;

	return S_OK;
}

CBT_Node::BT_NODE_STATE CBT_DistCheck::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard * pBlackBoard, _bool bDebugging)
{
	if (nullptr == m_pTransform)
		return BT_NODE_STATE::FAILED;

	Start_Node(pNodeStack, plistSubNodeStack, bDebugging);

	//최초 평가
	if (!m_bInProgress)
	{
		_v3 vTarget_Pos = pBlackBoard->Get_V3Value(m_Target_Key);

		_v3 vLength = vTarget_Pos - m_pTransform->Get_Pos();
		vLength.y = 0.f;

		/*
		높이와 상관없이 거리를 판단함.
		타겟이 허용거리 안에 있다.  ->  시야각 판단 시작
		*/
		//	허용거리		타겟
		if (m_fMaxLength >= D3DXVec3Length(&vLength))
		{
			m_bInProgress = true;
			return m_pChild->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging);
		}
		else
		{
			return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::FAILED, pBlackBoard, bDebugging);
		}
	}
	// 진행중 평가
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

void CBT_DistCheck::Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { ConeCheck : Length " << m_fMaxLength << " }" << endl;
		}

		pNodeStack->push_back(this);
		Safe_AddRef(this);

		m_eChild_State = BT_NODE_STATE::INPROGRESS;
		m_bInProgress = false;
		m_bInit = false;
	}
}

CBT_Node::BT_NODE_STATE CBT_DistCheck::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, CBlackBoard* pBlackBoard, _bool bDebugging)
{
	if (pNodeStack->empty())
		return eState;

	Safe_Release(pNodeStack->back());
	pNodeStack->pop_back();

	if(!pNodeStack->empty())
		Notify_Parent_Of_State(pNodeStack->back(), eState);
	m_bInit = true;

	if (bDebugging)
	{
		Cout_Indentation(pNodeStack);
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { DistCheck : Length " << m_fMaxLength << " }" << endl;
	}

	return eState;
}

HRESULT CBT_DistCheck::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	strcpy_s<STR_128>(m_pNodeName, temp.Target_NodeName);
	
	lstrcpy(m_Target_Key, temp.Target_Key);
	m_pTransform = temp.pTransform;
	Safe_AddRef(m_pTransform);
	m_fMaxLength = temp.fMaxLength;

	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return S_OK;
}

CBT_DistCheck * CBT_DistCheck::Create_Prototype()
{
	return new CBT_DistCheck();
}

CBT_Node * CBT_DistCheck::Clone(void * pInit_Struct)
{
	CBT_DistCheck* pInstance = new CBT_DistCheck(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_DistCheck");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBT_DistCheck::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pChild);
}
