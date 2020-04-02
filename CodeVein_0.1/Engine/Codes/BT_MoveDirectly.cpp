#include "..\Headers\BT_MoveDirectly.h"
#include "..\Headers\Transform.h"

CBT_MoveDirectly::CBT_MoveDirectly()
{
}

CBT_MoveDirectly::CBT_MoveDirectly(const CBT_MoveDirectly & rhs)
{
}

CBT_Node::BT_NODE_STATE CBT_MoveDirectly::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging)
{
	if ( CHASE == m_eMode && 0 == lstrcmp(m_Target_Key, L""))
		return BT_NODE_STATE::FAILED;

	Start_Node(pNodeStack, plistSubNodeStack, bDebugging);

	_v3 vTarget_Pos = _v3(0.f, 0.f, 0.f);
	_v3 vLength = _v3(0.f, 0.f, 0.f);

	switch(m_eMode)
	{
		case CHASE:
			vTarget_Pos = pBlackBoard->Get_V3Value(m_Target_Key);
			vTarget_Pos.y = 0.f;

			vLength = vTarget_Pos - m_pTransform->Get_Pos();
			vLength.y = 0.f;

			/*
			높이와 상관없이 거리를 판단함.
			타겟이 허용거리 안에 있다. ->  이동 끝냄
			*/
			//	허용거리				타겟
			if (m_dAcceptable_Radius >= D3DXVec3Length(&vLength))
			{
				return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, bDebugging);
			}
			else
			{
				// 방향 타겟쪽으로 변경
				Look_At_Target(TimeDelta, vTarget_Pos);
				
				// 이동
				m_pTransform->Add_Pos(_float(m_fMove_Speed * TimeDelta));
			}

			break;

		case RUSH:
			// 보고 있는 방향으로 이동
			m_dCurTime += TimeDelta;

			if (m_dCurTime > m_dMaxTime)
			{
				End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, bDebugging);
			}

			m_pTransform->Add_Pos(_float(m_fMove_Speed * TimeDelta));
			break;

		default:
			return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::FAILED, bDebugging);
	}




	return BT_NODE_STATE::INPROGRESS;
}

void CBT_MoveDirectly::Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{
		switch (m_eMode)
		{
		case CHASE:
			if (bDebugging)
			{
				Cout_Indentation(pNodeStack);
				cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { MoveDirectly : Chase" << m_dMoveTime << " +- " << m_dTimeOffset << " }" << endl;
			}
			break;

		case RUSH:
			if (bDebugging)
			{
				Cout_Indentation(pNodeStack);
				cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { MoveDirectly : Rush" << m_dMoveTime << " +- " << m_dTimeOffset << " }" << endl;
			}
			break;
		}

		pNodeStack->push_back(this);
		Safe_AddRef(this);
		
		m_dCurTime = 0;
		m_dMaxTime = m_dMoveTime + CALC::Random_Num_Double(-m_dTimeOffset, m_dTimeOffset);

		m_bInit = false;
	}
}

CBT_Node::BT_NODE_STATE CBT_MoveDirectly::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, _bool bDebugging)
{
	if (pNodeStack->empty())
		return eState;

	Safe_Release(pNodeStack->back());
	pNodeStack->pop_back();

	if (!pNodeStack->empty())
		Notify_Parent_Of_State(pNodeStack->back(), eState);
	m_bInit = true;

	switch (m_eMode)
	{
	case CHASE:
		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { MoveDirectly : Rush" << m_dMoveTime << " +- " << m_dTimeOffset << "  EndTime  " << m_dCurTime << " }" << endl;
		}
		break;

	case RUSH:
		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { MoveDirectly : Rush" << m_dMoveTime << " +- " << m_dTimeOffset << "  EndTime  " << m_dCurTime << " }" << endl;
		}
		break;
	}




	return eState;
}

HRESULT CBT_MoveDirectly::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	m_pTransform = temp.pTransform;

	if (m_pTransform)
		Safe_AddRef(m_pTransform);

	m_eMode = temp.eMode;
	strcpy_s<256>(m_pNodeName, temp.Target_NodeName);

	if( MODE::CHASE == m_eMode)
		lstrcpy(m_Target_Key, temp.Target_Key);

	m_fMove_Speed = temp.fMove_Speed;
	m_dAcceptable_Radius = temp.fAcceptable_Radius;
	m_dMoveTime = temp.dMoveTime;
	m_dTimeOffset = temp.dTimeOffset;

	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return S_OK;
}

void CBT_MoveDirectly::Look_At_Target(_double TimeDelta, const _v3& Target_Pos)
{
	_v3 vOriginDir = *(_v3*)(&m_pTransform->Get_WorldMat().m[2]);
	vOriginDir.y = 0.f;
	D3DXVec3Normalize(&vOriginDir, &vOriginDir);

	_v3 vToPlayerDir = Target_Pos - m_pTransform->Get_Pos();
	vToPlayerDir.y = 0.f;
	D3DXVec3Normalize(&vToPlayerDir, &vToPlayerDir);

	float temp = D3DXVec3Dot(&vOriginDir, &vToPlayerDir);
	//1.0보다 커짐 방지
	if (temp > 1.0f)
		temp = 0.999f;
	_float Radian = acosf(temp);

	_v3 vRight;
	D3DXVec3Normalize(&vRight, (_v3*)(&m_pTransform->Get_WorldMat().m[0]));

	if (0 < D3DXVec3Dot(&vRight, &vToPlayerDir))
		m_pTransform->Add_Angle(AXIS_TYPE::AXIS_Y, D3DXToDegree(_float(6 * Radian * TimeDelta)));
	else
		m_pTransform->Add_Angle(AXIS_TYPE::AXIS_Y, -D3DXToDegree(_float( 6 * Radian * TimeDelta)));
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
