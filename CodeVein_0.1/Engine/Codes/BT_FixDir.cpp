#include "..\Headers\BT_FixDir.h"
#include "..\Headers\Transform.h"

CBT_FixDir::CBT_FixDir()
{
}

CBT_FixDir::CBT_FixDir(const CBT_FixDir & rhs)
{
}

CBT_Node::BT_NODE_STATE CBT_FixDir::Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging)
{
	if (0 == lstrcmp(m_Target_Key, L""))
		return BT_NODE_STATE::FAILED;
	
	Start_Node(pNodeStack, plistSubNodeStack, bDebugging);

	m_dCurTime += TimeDelta;

	if ( m_dCurTime > m_dMaxTime)
		return End_Node(pNodeStack, plistSubNodeStack, BT_NODE_STATE::SUCCEEDED, bDebugging);
	else
	{
		Look_At_Target(TimeDelta, pBlackBoard->Get_V3Value(m_Target_Key));
	}

	return BT_NODE_STATE::INPROGRESS;
}

void CBT_FixDir::Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging)
{
	if (m_bInit)
	{

		if (bDebugging)
		{
			Cout_Indentation(pNodeStack);
			cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " Start   { FixDir : }" << endl;
		}


		pNodeStack->push_back(this);
		Safe_AddRef(this);

		m_dCurTime = 0;
		m_dMaxTime = m_dFixTime + CALC::Random_Num_Double(-m_dTimeOffset, m_dTimeOffset);

		m_bInit = false;
	}
}

CBT_Node::BT_NODE_STATE CBT_FixDir::End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, _bool bDebugging)
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
		cout << "[" << m_iNodeNumber << "] " << m_pNodeName << " End   { FixnDir : }" << endl;
	}

	m_dCurTime = 0;

	return eState;
}

HRESULT CBT_FixDir::Ready_Clone_Node(void * pInit_Struct)
{
	INFO temp = *(INFO*)pInit_Struct;

	m_pTransform = temp.pTransform;

	if (m_pTransform)
		Safe_AddRef(m_pTransform);

	strcpy_s<256>(m_pNodeName, temp.Target_NodeName);
	lstrcpy(m_Target_Key, temp.Target_Key);

	m_dFixTime = temp.dFixTime;
	m_dTimeOffset = temp.dTimeOffset;
	
	CBT_Node::_Set_Auto_Number(&m_iNodeNumber);
	return S_OK;
}

HRESULT CBT_FixDir::Look_At_Target(_double TimeDelta, const _v3& vTarget_Pos)
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

	_float fRadian = acosf(fTemp);

	_v3 vRight;
	D3DXVec3Normalize(&vRight, (_v3*)(&m_pTransform->Get_WorldMat().m[0]));

	if (0 < D3DXVec3Dot(&vRight, &vToTarget))
		m_pTransform->Add_Angle(AXIS_TYPE::AXIS_Y, D3DXToDegree(_float(2 * fRadian * TimeDelta)));
	else
		m_pTransform->Add_Angle(AXIS_TYPE::AXIS_Y, -D3DXToDegree(_float(2 * fRadian * TimeDelta)));

	return S_OK;
}

CBT_FixDir * CBT_FixDir::Create_Prototype()
{
	return new CBT_FixDir();
}

CBT_Node * CBT_FixDir::Clone(void * pInit_Struct)
{
	CBT_FixDir* pInstance = new CBT_FixDir(*this);

	if (FAILED(pInstance->Ready_Clone_Node(pInit_Struct)))
	{
		MSG_BOX("Failed To Clone CBT_FixDir");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBT_FixDir::Free()
{
	Safe_Release(m_pTransform);
}
