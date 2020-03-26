#pragma once

#include "BT_Node.h"

/*
조건문이라고 생각하면 된다.
다른 노드에 붙어서 이 노드를 실행할지 말지 결정한다.
*/

BEGIN(Engine)

class ENGINE_DLL CBT_Decorator_Node abstract : public CBT_Node
{
protected:
	explicit CBT_Decorator_Node();
	virtual ~CBT_Decorator_Node() = default;

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging) = 0;

public:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging) = 0;
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, _bool bDebugging) = 0;

public:
	virtual CBT_Node* Clone(void* pInit_Struct) = 0;

	virtual void Free();

};

END