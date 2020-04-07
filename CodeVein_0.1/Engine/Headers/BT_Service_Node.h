#pragma once

#include "BT_Node.h"

/*
ComPosite�� �ٿ��� ����Ѵ�.
�ش�бⰡ ���� ���϶�, ������ �󵵿� ���� �� ��带 �����Ѵ�.
���� BlackBoard ������Ʈ�� Ư�� ���� �˻��� �� ����Ѵ�.
*/

BEGIN(Engine)

class ENGINE_DLL CBT_Service_Node abstract : public CBT_Node
{
public:
	enum Mode { Infinite, Finite };

protected:
	explicit CBT_Service_Node();
	virtual ~CBT_Service_Node() = default;

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool Debugging) = 0;

protected:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging) = 0;
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, CBlackBoard* pBlackBoard, _bool bDebugging) = 0;

protected:
	_uint		m_iMax_Count_Of_Execution = 0;
	_uint		m_iCur_Count_Of_Execution = 0;
	_bool		m_bFinish_Service = false;

	Mode		m_eMode = Infinite;

public:
	virtual CBT_Node* Clone(void* pInit_Struct) = 0;

	virtual void Free();
};

END