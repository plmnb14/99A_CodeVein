#pragma once

#include "..\headers\BT_Node.h"

/*
AI�� �ൿ�� �׼��� �����Ѵ�.
Tree���� leaf(�ڽ��� ���� ���� Node)�� �ش��Ѵ�.
*/

BEGIN(Engine)

class ENGINE_DLL CBT_Task_Node abstract : public CBT_Node
{
protected:
	explicit CBT_Task_Node();
	virtual ~CBT_Task_Node() = default;

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging) = 0;

public:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging) = 0;
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, CBlackBoard* pBlackBoard, _bool bDebugging) = 0;

public:
	virtual CBT_Node* Clone(void* pInit_Struct) = 0;

	virtual void Free();
};

END