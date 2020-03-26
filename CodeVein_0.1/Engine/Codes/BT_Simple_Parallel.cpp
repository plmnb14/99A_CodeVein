#include "..\Headers\BT_Simple_Parallel.h"

CBT_Simple_Parallel::CBT_Simple_Parallel()
{
}

CBT_Simple_Parallel::CBT_Simple_Parallel(const CBT_Simple_Parallel & rhs)
{
}

HRESULT CBT_Simple_Parallel::Set_Main_Child(CBT_Node * pNode)
{
	if (nullptr == pNode)
		return E_FAIL;

	//for (CBT_Node* child : m_pChildren)
	//{
	//	if (child == pNode)
	//		Safe_AddRef(child);
	//}

	//0���� main�ൿ
	m_pChildren[0] = pNode;

	return NO_ERROR;
}

HRESULT CBT_Simple_Parallel::Set_Sub_Child(CBT_Node * pNode)
{
	if (nullptr == pNode)
		return E_FAIL;

	//for (CBT_Node* child : m_pChildren)
	//{
	//	if (child == pNode)
	//		Safe_AddRef(child);
	//}

	//1���� sub�ൿ
	m_pChildren[1] = pNode;

	return NO_ERROR;
}

CBT_Node::BT_NODE_STATE CBT_Simple_Parallel::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging)
{
	/*
	���� ������� 0��
	���� ������� 1�� ����

	2��° �Ű������� ���ÿ� �Ҵ��ϴ� ����
	*/



	if (2 != m_pChildren.size())
		return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::FAILED, bDebugging);

	Start_Node(pNodeStack, plistSubNodeStack, bDebugging);

	if (!m_bMain_InProgress)
	{
		if (!m_bMain_InProgress)
		{
			//Main StackȮ��			
			switch (m_pChildren[0]->Update_Node(TimeDelta, pNodeStack, plistSubNodeStack, pBlackBoard, bDebugging))
			{
			case BT_NODE_STATE::FAILED:
				m_bMainState = BT_NODE_STATE::FAILED;
				break;
			case BT_NODE_STATE::SERVICE:
			case BT_NODE_STATE::SUCCEEDED:
				m_bMainState = BT_NODE_STATE::SUCCEEDED;
				break;
			case BT_NODE_STATE::INPROGRESS:
				m_bMainState = BT_NODE_STATE::INPROGRESS;
				break;
			}
			m_bMain_InProgress = true;
		}

		if (!m_bSub_InProgress)
		{
			plistSubNodeStack->push_back(&m_pSubNodeStack);
			switch (m_pChildren[1]->Update_Node(TimeDelta, &m_pSubNodeStack, plistSubNodeStack, pBlackBoard, bDebugging))
			{
			case BT_NODE_STATE::FAILED:
				m_bSubState = BT_NODE_STATE::FAILED;
				break;
			case BT_NODE_STATE::SERVICE:
			case BT_NODE_STATE::SUCCEEDED:
				m_bSubState = BT_NODE_STATE::SUCCEEDED;
				break;
			case BT_NODE_STATE::INPROGRESS:
				m_bSubState = BT_NODE_STATE::INPROGRESS;
				break;
			}
			m_bSub_InProgress = true;
		}
	}

	//������
	else
	{
		// �� ��带 �θ� ���� ����������� �˻� 

		//���� ���ÿ��� �ҷ����� Ȯ��


		if (pNodeStack->back() == this)
		{
			if (BT_NODE_STATE::SUCCEEDED == m_bMainState)
			{
				// ������ ����, ���갡 �������� ���� ��üũ ���ϰ� ���
			}
			else
			{
				switch (m_eChild_State)
				{
				case BT_NODE_STATE::FAILED:
					m_bMainState = BT_NODE_STATE::FAILED;
					break;
				case BT_NODE_STATE::SERVICE:
				case BT_NODE_STATE::SUCCEEDED:
					m_bMainState = BT_NODE_STATE::SUCCEEDED;
					break;
				case BT_NODE_STATE::INPROGRESS:
					m_bMainState = BT_NODE_STATE::INPROGRESS;
					break;
				}

				m_eChild_State = BT_NODE_STATE::INPROGRESS;
			}
		}
		else
		{
			for (auto SubNodeStack : *plistSubNodeStack)
			{
				//���� ���ÿ��� �ҷ����� Ȯ��
				if (SubNodeStack->back() == this)
				{
					switch (m_eChild_State)
					{
					case BT_NODE_STATE::FAILED:
						m_bSubState = BT_NODE_STATE::FAILED;
						break;
					case BT_NODE_STATE::SERVICE:
					case BT_NODE_STATE::SUCCEEDED:
						m_bSubState = BT_NODE_STATE::SUCCEEDED;
						break;
					case BT_NODE_STATE::INPROGRESS:
						m_bSubState = BT_NODE_STATE::INPROGRESS;
						break;
					}
					//ã������ ���� ���ÿ����� �˻��� ������.
					break;

					m_eChild_State = BT_NODE_STATE::INPROGRESS;
				}
			}
		}
	}


	//// ��� ���µ� Ȯ��

	if (BT_NODE_STATE::FAILED == m_bMainState)
	{	
		//m_pChildren[1]->End_Node(pNodeStack, BT_NODE_STATE::FAILED, bDebugging);
		Delete_SubNodeStack(plistSubNodeStack, bDebugging);
		return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::FAILED, bDebugging);
	}

	switch (m_eMode)
	{
		//������ ������ ���굵 ����
	case Mode::Immediate:
		if ((BT_NODE_STATE::SUCCEEDED == m_bMainState))
		{
			//m_pChildren[1]->End_Node(&m_pSubNodeStatck, BT_NODE_STATE::SUCCEEDED, bDebugging);
			Delete_SubNodeStack(plistSubNodeStack, bDebugging);
			return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, bDebugging);
		}

		//���갡 ���������� ������ ��ٸ�
	case Mode::Delayed:
		if (BT_NODE_STATE::SUCCEEDED == m_bMainState &&
			BT_NODE_STATE::SUCCEEDED == m_bSubState)
		{		
			return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, bDebugging);
		}
	}

	return BT_NODE_STATE::INPROGRESS;
}

void CBT_Simple_Parallel::Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{

	if (m_bInit)
	{
		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { Simple_Parallel } " << endl;
		}

		pNodeStack->push_back(this);
		Safe_AddRef(this);

		m_bMainState = BT_NODE_STATE::INPROGRESS;
		m_bSubState = BT_NODE_STATE::INPROGRESS;

		m_bMain_InProgress = false;
		m_bSub_InProgress = false;

		m_bInit = false;

		// ���񽺳�� �����忡 ���� �߰�
		if (!m_listServiceNodeStack.empty())
		{
			for (auto Child : m_listServiceNodeStack)
			{
				plistSubNodeStack->emplace_back(Child);
			}
		}
	}
}

CBT_Node::BT_NODE_STATE CBT_Simple_Parallel::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, _bool bDebugging)
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
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { Simple_Parallel } " << endl;
	}

	return eState;
}

HRESULT CBT_Simple_Parallel::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	strcpy_s<256>(m_pNodeName, temp.Target_NodeName);

	m_pChildren.resize(2);

	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return NO_ERROR;
}

HRESULT CBT_Simple_Parallel::Delete_SubNodeStack(list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	for (auto iter = plistSubNodeStack->begin(); iter != plistSubNodeStack->end(); ++iter)
	{
		if (*iter == &m_pSubNodeStack)
		{
			if (!(*iter)->empty())
			{
				(*iter)->front()->End_Node(*iter, plistSubNodeStack, BT_NODE_STATE::FAILED, bDebugging);

				//Safe_Release((*iter)->front());

			}

			plistSubNodeStack->erase(iter);
			break;
		}
	}
	m_pSubNodeStack.clear();

	return NO_ERROR;
}

CBT_Simple_Parallel * CBT_Simple_Parallel::Create_Prototype()
{
	return new CBT_Simple_Parallel();
}

CBT_Node * CBT_Simple_Parallel::Clone(void * pInit_Struct)
{
	CBT_Simple_Parallel* pInstance = new CBT_Simple_Parallel(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_Simple_Parallel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBT_Simple_Parallel::Free()
{
	CBT_Composite_Node::Free();

	for (auto child : m_pSubNodeStack)
	{
		//child->Free();

		Safe_Release(child);
	}
	m_pSubNodeStack.clear();

	for (auto iter = m_pChildren.begin(); iter != m_pChildren.end(); )
	{
		(*iter)->Free();
		Safe_Release(*iter);

		if (nullptr == *iter)
			iter = m_pChildren.erase(iter);
		else
			++iter;
	}

	//for (CBT_Node* child : m_pChildren)
	//{
	//	//child->Free();

	//	Safe_Release(child);
	//}

	//m_pChildren.clear();
}
