#pragma once

#include "Base.h"

/*
Ai를 작동시키는 행동트리의 인터페이스
*/

BEGIN(Engine)
class CBT_Task_Node;
class CBT_Composite_Node;
class CBT_Root;
class CBT_Node;
class ENGINE_DLL CBehaviorTree final : public CBase
{
private:
	explicit CBehaviorTree();
	virtual ~CBehaviorTree() = default;

public:
	HRESULT Set_Child(CBT_Composite_Node* pComposite_Node);
	HRESULT Set_Child(CBT_Task_Node* pTask_Node);

public:
	virtual void Update_BeHaviorTree(_double TimeDelta);

private:
	HRESULT Ready_BehavioTree(_bool bDebuging);

private:
	CBT_Root*					m_pRoot = nullptr;
	//메인 쓰레드
	vector<CBT_Node*>			m_pNodeStack;

	/* 
	0번   : mainThread
	1번 ~ : SubThread
	*/
	list<vector<CBT_Node*>*>	m_plistNodeStack;

	_bool						m_bDebuging = false;

public:
	static CBehaviorTree* Create(_bool bDebuging = false);

	virtual void Free();
};

END