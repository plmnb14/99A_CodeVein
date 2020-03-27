#include "../Headers/BT_Cooldown.h"

CBT_Cooldown::CBT_Cooldown()
{
}

CBT_Cooldown::CBT_Cooldown(const CBT_Cooldown & rhs)
{
}

HRESULT CBT_Cooldown::Set_Child(CBT_Node * pNode)
{
	if (nullptr == pNode)
		return E_FAIL;

	m_pChild = pNode;

	return S_OK;
}

CBT_Node::BT_NODE_STATE CBT_Cooldown::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging)
{
	if (nullptr == m_pChild)
		return BT_NODE_STATE::FAILED;

	// 메인쓰레드 실행 루트
	if (pNodeStack == plistSubNodeStack->front())
	{
		if (BT_NODE_STATE::SUCCEEDED == m_eCurState)
		{
			m_bInit = true;
			m_eCurState = BT_NODE_STATE::INPROGRESS;

			return m_pChild->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging);
		}
		else
		{
			if (m_bInit)
			{
				//부모에게 알리기 위해서 첫 스택에 부모를 넣어줌.
				m_pSubNodeStatck.push_back(pNodeStack->back());
				Safe_AddRef(pNodeStack->back());
				plistSubNodeStack->push_back(&m_pSubNodeStatck);
				Start_Node(&m_pSubNodeStatck, plistSubNodeStack, bDebugging);
			}

			return BT_NODE_STATE::FAILED;
		}
	}
	//서브쓰레드 실행 루트
	else
	{
		m_dCurTime -= TimeDelta;

		// 쿨다운이 완료됐다면
		if (m_dCurTime < 0)
		{
			//m_bAddThread = true;
			m_eCurState = BT_NODE_STATE::SUCCEEDED;
			return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, bDebugging);
		}
	}

	return BT_NODE_STATE::INPROGRESS;
}

void CBT_Cooldown::Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { Cooldown : " << m_dCoolTime << " }" << endl;

		}

		pNodeStack->push_back(this);
		Safe_AddRef(this);

		m_dCurTime = m_dCoolTime;

		m_bInProgress = false;
		m_bAddThread = true;
		m_bInit = false;
	}
}

CBT_Node::BT_NODE_STATE CBT_Cooldown::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, _bool bDebugging)
{
	if (pNodeStack->empty())
		return eState;

	//본인 제거
	Safe_Release(pNodeStack->back());
	pNodeStack->pop_back();
	
	if (!pNodeStack->empty())
		Notify_Parent_Of_State(pNodeStack->back(), eState);

	m_bInit = true;

	if (bDebugging)
	{
		Cout_Indentation(pNodeStack);
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { Cooldown : " << m_dCoolTime << " }" << endl;
	}

	/*
	부모에게 알리기위해 쓰레드에 첫 스택에 임의로 넣었던 것 빼준다.
	*/
	Safe_Release(pNodeStack->back());
	pNodeStack->pop_back();


	return eState;
}

HRESULT CBT_Cooldown::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	strcpy_s<256>(m_pNodeName, temp.Target_NodeName);
	m_dCoolTime = temp.Target_dCoolTime;

	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return S_OK;
}

CBT_Cooldown * CBT_Cooldown::Create_Prototype()
{
	return new CBT_Cooldown();
}

CBT_Node * CBT_Cooldown::Clone(void * pInit_Struct)
{
	CBT_Cooldown* pInstance = new CBT_Cooldown(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_Cooldown");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBT_Cooldown::Free()
{
	//부모와 본인것 주소 제거
	if (!m_pSubNodeStatck.empty())
	{
		Safe_Release(m_pSubNodeStatck[0]);
		Safe_Release(m_pSubNodeStatck[1]);

		m_pSubNodeStatck.clear();
	}

	//if (m_pChild)
	//	m_pChild->Free();

	Safe_Release(m_pChild);
}
