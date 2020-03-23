#pragma once

#include "BT_Node.h"

/*
ComPosite에 붙여서 사용한다.
해당분기가 실행 중일때, 정해진 빈도에 따라 이 노드를 실행한다.
보통 BlackBoard 업에이트나 특정 값을 검사할 떄 사용한다.
*/

BEGIN(Engine)

class ENGINE_DLL CBT_Service_Node abstract : public CBT_Node
{
protected:
	explicit CBT_Service_Node();
	virtual ~CBT_Service_Node() = default;

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, const CBlackBoard* pBlackBoard, _bool Debugging) = 0;

protected:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack) = 0;
	virtual void End_Node(vector<CBT_Node*>* pNodeStack) = 0;

public:
	virtual CBT_Node* Clone(void* pInit_Struct) = 0;

	virtual void Free();
};

END