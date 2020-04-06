#include "..\Headers\BT_MoveAround.h"
#include "..\Headers\Transform.h"
#include "..\Headers\NavMesh.h"

CBT_MoveAround::CBT_MoveAround()
{
}

CBT_MoveAround::CBT_MoveAround(const CBT_MoveAround & rhs)
{
}

CBT_Node::BT_NODE_STATE CBT_MoveAround::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging)
{
	if (0 == lstrcmp(m_Target_Key, L""))
		return BT_NODE_STATE::FAILED;

	Start_Node(pNodeStack, plistSubNodeStack, bDebugging);

	_v3 vTarget_Pos = _v3(0.f, 0.f, 0.f);
	vTarget_Pos = pBlackBoard->Get_V3Value(m_Target_Key);
	vTarget_Pos.y = 0.f;

	m_dCurTime += TimeDelta;

	if (m_dCurTime > m_dMaxTime)
	{
		return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, bDebugging);
	}
	else
	{
		_v3 vLook = *D3DXVec3Normalize(&_v3(), (_v3*)m_pTransform->Get_WorldMat().m[2]);
		_v3 vToTarget = vTarget_Pos - m_pTransform->Get_Info().vPos;
		D3DXVec3Normalize(&vToTarget, &vToTarget);
		_v3 vRight = _v3(0.f, 0.f, 0.f);

		if (m_fMove_Speed >= 0.f)
		{
			// 타겟을 향한 벡터를 회전
			D3DXVec3TransformNormal(&vToTarget, &vToTarget, D3DXMatrixRotationY(&_mat(), D3DXToRadian(3)));
			// 회전된 벡터를 기준으로 right벡터를 구함.
			D3DXVec3Cross(&vRight, &vToTarget, &_v3(0.f, 1.f, 0.f));

			m_pTransform->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransform->Get_Pos(), &vRight, m_fMove_Speed * (_float)TimeDelta)));

		}
		else
		{
			// 반대 회전

			// 타겟을 향한 벡터를 회전
			D3DXVec3TransformNormal(&vToTarget, &vToTarget, D3DXMatrixRotationY(&_mat(), D3DXToRadian(-3)));
			// 회전된 벡터를 기준으로 right벡터를 구함.
			D3DXVec3Cross(&vRight, &vToTarget, &_v3(0.f, 1.f, 0.f));
			vRight *= -1.f;

			m_pTransform->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransform->Get_Pos(), &vRight, -m_fMove_Speed * (_float)TimeDelta)));
		}
	}



	return BT_NODE_STATE::INPROGRESS;
}

void CBT_MoveAround::Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{

		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { MoveAround : " << m_dMoveTime << " +- " << m_dTimeOffset << " }" << endl;
		}



		pNodeStack->push_back(this);
		Safe_AddRef(this);

		m_dCurTime = 0;
		m_dMaxTime = m_dMoveTime + CALC::Random_Num_Double(-m_dTimeOffset, m_dTimeOffset);

		m_bInit = false;
	}
}

CBT_Node::BT_NODE_STATE CBT_MoveAround::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, _bool bDebugging)
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
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { MoveAround : " << m_dMoveTime << " +- " << m_dTimeOffset << "  EndTime  " << m_dCurTime << " }" << endl;
	}




	return eState;
}

HRESULT CBT_MoveAround::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	m_pTransform = temp.pTransform;

	if (m_pTransform)
		Safe_AddRef(m_pTransform);

	m_pNavMesh = temp.pNavMesh;

	if (m_pNavMesh)
		Safe_AddRef(m_pNavMesh);

	strcpy_s<256>(m_pNodeName, temp.Target_NodeName);
	lstrcpy(m_Target_Key, temp.Target_Key);
	m_fMove_Speed = temp.fMove_Speed;
	m_dMoveTime = temp.dMoveTime;
	m_dTimeOffset = temp.dTimeOffset;

	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return S_OK;
}

CBT_MoveAround * CBT_MoveAround::Create_Prototype()
{
	return new CBT_MoveAround();
}

CBT_Node * CBT_MoveAround::Clone(void * pInit_Struct)
{
	CBT_MoveAround* pInstance = new CBT_MoveAround(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_MoveAround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBT_MoveAround::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pNavMesh);
}
