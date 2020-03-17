#pragma once

#include "Base.h"
#include "BehaviorTree.h"
/*
Composite
Decorator
Service
Task

위 노드들의 최상위 클래스입니다.
*/

BEGIN(Engine)

class ENGINE_DLL CBT_Node abstract : public CBase
{
public:
	enum class BT_NODE_STATE { ABORTED, FAILED, SUCCEEDED, INPROGRESS, SERVICE };

protected:
	explicit CBT_Node();
	virtual ~CBT_Node() = default;

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack) = 0;

protected:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack) = 0;
	virtual void End_Node(vector<CBT_Node*>* pNodeStack) = 0;

protected:
	BT_NODE_STATE	m_eNode_State = BT_NODE_STATE::FAILED;
	_bool	m_bInit = true;

public:
	virtual CBT_Node* Clone(void* pInit_Struct) = 0;

	virtual void Free();
};

END