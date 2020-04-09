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

	// ���ξ����� ���� ��Ʈ
	if (pNodeStack == plistSubNodeStack->front())
	{
		// ��ٿ� �غ� �Ϸ�
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
				//�θ𿡰� �˸��� ���ؼ� ù ���ÿ� �θ� �־���.
				m_pSubNodeStack.push_back(pNodeStack->back());
				//Safe_AddRef(pNodeStack->back());
				plistSubNodeStack->push_back(&m_pSubNodeStack);
				Start_Node(&m_pSubNodeStack, plistSubNodeStack, bDebugging);
			}

			return BT_NODE_STATE::FAILED;
		}
	}
	//���꾲���� ���� ��Ʈ
	else
	{
		m_dCurTime -= TimeDelta;

		// ��ٿ��� �Ϸ�ƴٸ�
		if (m_dCurTime < 0)
		{
			//m_bAddThread = true;
			m_eCurState = BT_NODE_STATE::SUCCEEDED;
			return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, pBlackBoard, bDebugging);
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

CBT_Node::BT_NODE_STATE CBT_Cooldown::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, CBlackBoard* pBlackBoard, _bool bDebugging)
{
	if (pNodeStack->empty())
		return eState;

	//���� ����
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
	�θ𿡰� �˸������� �����忡 ù ���ÿ� ���Ƿ� �־��� �� ���ش�.
	*/
	//Safe_Release(pNodeStack->back());
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
	for (auto child : m_pSubNodeStack)
	{
		Safe_Release(child);
	}
	m_pSubNodeStack.clear();

	if (false == m_bInit)
	{
		// this
		//m_pSubNodeStack.pop_back();
		m_pSubNodeStack[0] = nullptr;
		// �θ���
		//m_pSubNodeStack.pop_back();
		m_pSubNodeStack[1] = nullptr;
	}

	Safe_Release(m_pChild);
}
