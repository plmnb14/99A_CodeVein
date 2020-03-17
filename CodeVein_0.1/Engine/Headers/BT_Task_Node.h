#pragma once

#include "BT_Node.h"

/*
AI가 행동할 액션을 정의한다.
Tree에서 leaf(자식이 없는 최종 Node)에 해당한다.
*/

BEGIN(Engine)

class ENGINE_DLL CBT_Task_Node abstract : public CBT_Node
{
protected:
	explicit CBT_Task_Node();
	virtual ~CBT_Task_Node() = default;

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack) = 0;

protected:
	/*
	Task 노드가 시작될때 Start_Node를 호출
	            끝날때 End_Node를 호출
	*/
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack) = 0;
	virtual void End_Node(vector<CBT_Node*>* pNodeStack) = 0;

public:
	virtual CBT_Node* Clone(void* pInit_Struct) = 0;

	virtual void Free();
};

END