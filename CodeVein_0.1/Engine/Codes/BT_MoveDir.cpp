#include "..\Headers\BT_MoveDir.h"
#include "..\Headers\Transform.h"

CBT_MoveDir::CBT_MoveDir()
{
}

CBT_MoveDir::CBT_MoveDir(const CBT_MoveDir & rhs)
{
}

CBT_Node::BT_NODE_STATE CBT_MoveDir::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging)
{
	if (0 == lstrcmp(m_Dir_Key, L""))
		return BT_NODE_STATE::FAILED;

	Start_Node(pNodeStack, plistSubNodeStack, bDebugging);

	_v3 fDir = _v3(0.f, 0.f, 0.f);

	m_dCurTime += TimeDelta;

	fDir = pBlackBoard->Get_V3Value(m_Dir_Key);

	if (m_dCurTime > m_dMaxTime)
	{
		End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, bDebugging);
	}
	else
	{
		m_pTransform->Add_Pos(_float(m_fMove_Speed * TimeDelta), fDir);
	}

	return BT_NODE_STATE::INPROGRESS;
}

void CBT_MoveDir::Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { MoveDir : " << m_dMoveTime << " +- " << m_dTimeOffset << " }" << endl;
		}

		pNodeStack->push_back(this);
		Safe_AddRef(this);

		m_dCurTime = 0;
		m_dMaxTime = m_dMoveTime + CALC::Random_Num_Double(-m_dTimeOffset, m_dTimeOffset);

		m_bInit = false;
	}
}

CBT_Node::BT_NODE_STATE CBT_MoveDir::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, _bool bDebugging)
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
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { MoveDir : " << m_dMoveTime << " +- " << m_dTimeOffset << "  EndTime  " << m_dCurTime << " }" << endl;
	}

	return eState;
}

HRESULT CBT_MoveDir::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	m_pTransform = temp.pTransform;

	if (m_pTransform)
		Safe_AddRef(m_pTransform);

	strcpy_s<256>(m_pNodeName, temp.Target_NodeName);
	lstrcpy(m_Dir_Key, temp.Dir_Key);

	m_fMove_Speed = temp.fMove_Speed;
	m_dMoveTime = temp.dMoveTime;
	m_dTimeOffset = temp.dTimeOffset;

	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return S_OK;
}

CBT_MoveDir * CBT_MoveDir::Create_Prototype()
{
	return new CBT_MoveDir();
}

CBT_Node * CBT_MoveDir::Clone(void * pInit_Struct)
{
	CBT_MoveDir* pInstance = new CBT_MoveDir(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_MoveDir");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBT_MoveDir::Free()
{
	Safe_Release(m_pTransform);
}
