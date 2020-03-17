#pragma once

#include "BT_Task_Node.h"

BEGIN(Engine)

class ENGINE_DLL CBT_Wait final : public CBT_Task_Node
{
public:
	typedef struct tagInitInfo
	{
		_double m_dMaxTime;
	}INFO;

protected:
	explicit CBT_Wait();
	explicit CBT_Wait(const CBT_Wait& rhs);
	virtual ~CBT_Wait() = default;

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack) override;

	HRESULT Initialize_Node(_double dMaxTime);

protected:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack);
	virtual void End_Node(vector<CBT_Node*>* pNodeStack);

private:
	HRESULT Ready_Clone_Node(void* pInit_Struct);

private:
	_double		m_dCurTime = 0;
	_double		m_dMaxTime = 0;

public:
	static CBT_Wait* Create_Prototype();
	virtual CBT_Node* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END