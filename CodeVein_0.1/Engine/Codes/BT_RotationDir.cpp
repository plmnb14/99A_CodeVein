#include "..\Headers\BT_RotationDir.h"
#include "..\Headers\Transform.h"

CBT_RotationDir::CBT_RotationDir()
{
}

CBT_RotationDir::CBT_RotationDir(const CBT_RotationDir & rhs)
{
}

CBT_Node::BT_NODE_STATE CBT_RotationDir::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging)
{
	if (0 == lstrcmp(m_Target_Key, L""))
		return BT_NODE_STATE::FAILED;

	Start_Node(pNodeStack, plistSubNodeStack, bDebugging);

	Look_At_Target(TimeDelta, pBlackBoard->Get_V3Value(m_Target_Key));

	// 사이각 0.003 정도는 넘어간다.
	if ( m_dDestRadian < 0.4 )
	{
		return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, bDebugging);
	}

	return BT_NODE_STATE::INPROGRESS;
}

void CBT_RotationDir::Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{

		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { RotationDir : " << m_dDestRadian << " }" << endl;
		}


		pNodeStack->push_back(this);
		Safe_AddRef(this);



		m_bInit = false;
	}
}

CBT_Node::BT_NODE_STATE CBT_RotationDir::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, _bool bDebugging)
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
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { RotationDir : " << m_dDestRadian << " }" << endl;
	}

	m_dDestRadian = 0;

	return eState;
}

HRESULT CBT_RotationDir::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	m_pTransform = temp.pTransform;

	if (m_pTransform)
		Safe_AddRef(m_pTransform);

	strcpy_s<256>(m_pNodeName, temp.Target_NodeName);
	lstrcpy(m_Target_Key, temp.Target_Key);
	m_dTurnSpeed = temp.dTurnSpeed;
	
	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return S_OK;
}

HRESULT CBT_RotationDir::Look_At_Target(_double TimeDelta, const _v3& vTarget_Pos)
{
	_v3 vOriginDir = *(_v3*)(&m_pTransform->Get_WorldMat().m[2]);
	vOriginDir.y = 0.f;
	D3DXVec3Normalize(&vOriginDir, &vOriginDir);

	_v3 vToTarget = vTarget_Pos - m_pTransform->Get_Pos();
	D3DXVec3Normalize(&vToTarget, &vToTarget);

	float fTemp = D3DXVec3Dot(&vOriginDir, &vToTarget);
	//1.0보다 커짐 방지
	if (fTemp > 1.0f)
		fTemp = 0.999f;

	m_dDestRadian = acosf(fTemp);

	_v3 vRight;
	D3DXVec3Normalize(&vRight, (_v3*)(&m_pTransform->Get_WorldMat().m[0]));

	if (0 < D3DXVec3Dot(&vRight, &vToTarget))
		m_pTransform->Add_Angle(AXIS_TYPE::AXIS_Y, D3DXToDegree(_float(m_dTurnSpeed * m_dDestRadian * TimeDelta)));
	else
		m_pTransform->Add_Angle(AXIS_TYPE::AXIS_Y, -D3DXToDegree(_float(m_dTurnSpeed * m_dDestRadian * TimeDelta)));

	return S_OK;
}

CBT_RotationDir * CBT_RotationDir::Create_Prototype()
{
	return new CBT_RotationDir();
}

CBT_Node * CBT_RotationDir::Clone(void * pInit_Struct)
{
	CBT_RotationDir* pInstance = new CBT_RotationDir(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_RotationDir");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBT_RotationDir::Free()
{
	Safe_Release(m_pTransform);
}
