#pragma  once

#include "BT_Node.h"

/*
BehaviorTree�� �������Դϴ�.
service�� decorator�� Root�� �Ҵ��� �� �����ϴ�.

Composite �̳� Task�� child�� �Ҵ� �����մϴ�.
*/

BEGIN(Engine)

class CBT_Composite_Node;
class CBT_Task_Node;
class ENGINE_DLL CBT_Root final : public CBT_Node
{
protected:
	explicit CBT_Root();
	virtual ~CBT_Root() = default;

public:
	HRESULT Set_Child(CBT_Composite_Node* pComposite_Node);
	HRESULT Set_Child(CBT_Task_Node* pTask_Node);

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging) override;

protected:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, _bool bDebugging) {};
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, BT_NODE_STATE eState, _bool bDebugging) { return BT_NODE_STATE::SUCCEEDED; };

private:
	CBT_Node*		m_pChildNode = nullptr;

public:
	static CBT_Root* Create();

	virtual CBT_Node* Clone(void* pInit_Struct) override { return nullptr; }

	virtual void Free();
};

END