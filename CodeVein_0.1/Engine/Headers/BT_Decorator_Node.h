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
	virtual BT_NODE_STATE Update_Node(_double TimeDelta) = 0;

public:
	virtual CBT_Node* Clone(void* pInit_Struct) = 0;

	virtual void Free();

};

END