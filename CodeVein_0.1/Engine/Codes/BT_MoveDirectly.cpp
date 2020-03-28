#include "..\Headers\BT_MoveDirectly.h"
#include "..\Headers\Transform.h"

CBT_MoveDirectly::CBT_MoveDirectly()
{
}

CBT_MoveDirectly::CBT_MoveDirectly(const CBT_MoveDirectly & rhs)
{
}

CBT_Node::BT_NODE_STATE CBT_MoveDirectly::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, const CBlackBoard* pBlackBoard, _bool bDebugging)
{
	if (0 == lstrcmp(m_Target_Key, L""))
		return BT_NODE_STATE::FAILED;

	Start_Node(pNodeStack, bDebugging);

	_v3 vTarget_Pos = pBlackBoard->Get_V3Value(m_Target_Key);

	_v3 vLength = vTarget_Pos - m_pTransform->Get_Pos();
	vLength.y = 0.f;

	/*
	높이와 상관없이 거리를 판단함.
	타겟이 허용거리 안에 있다. ->  이동 끝냄
	*/
	//	허용거리				타겟
	if (m_dAcceptable_Radius >= D3DXVec3Length(&vLength))
	{
		return End_Node(pNodeStack, BT_NODE_STATE::SUCCEEDED, bDebugging);
	}
	else
	{
		// 방향 타겟쪽으로 변경
		m_pTransform->Chase_Target(&vTarget_Pos);


		// 이동
		m_pTransform->Add_Pos(_float(m_fMove_Speed * TimeDelta));
	}


	return BT_NODE_STATE::INPROGRESS;
}

void CBT_MoveDirectly::Start_Node(vector<CBT_Node*>* pNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { MoveDirectly : " << " }" << endl;
		}

		pNodeStack->push_back(this);
		Safe_AddRef(this);
		
		m_bInit = false;
	}
}

CBT_Node::BT_NODE_STATE CBT_MoveDirectly::End_Node(vector<CBT_Node*>* pNodeStack, BT_NODE_STATE eState, _bool bDebugging)
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
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { MoveDirectly : "  << " }" << endl;
	}

	return eState;
}

HRESULT CBT_MoveDirectly::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	m_pTransform = temp.pTransform;

	if (m_pTransform)
		Safe_AddRef(m_pTransform);

	strcpy_s<256>(m_pNodeName, temp.Target_NodeName);
	lstrcpy(m_Target_Key, temp.Target_Key);
	m_fMove_Speed = temp.fMove_Speed;
	m_dAcceptable_Radius = temp.fAcceptable_Radius;

	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return S_OK;
}

CBT_MoveDirectly * CBT_MoveDirectly::Create_Prototype()
{
	return new CBT_MoveDirectly();
}

CBT_Node * CBT_MoveDirectly::Clone(void * pInit_Struct)
{
	CBT_MoveDirectly* pInstance = new CBT_MoveDirectly(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_MoveDirectly");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBT_MoveDirectly::Free()
{
	Safe_Release(m_pTransform);
}
