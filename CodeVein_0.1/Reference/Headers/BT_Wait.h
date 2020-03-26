#pragma once

#include "..\headers\BT_Task_Node.h"

BEGIN(Engine)

class ENGINE_DLL CBT_Wait final : public CBT_Task_Node
{
public:
	/*
	대기시간 = dMaxTime +- dOffsetTime
	*/
	typedef struct tagInitInfo
	{
		tagInitInfo(char* pNodeName, _double dWaitingTime, _double dOffset)
			: Target_dWaitingTime(dWaitingTime), Target_dOffset(dOffset)
		{ strcpy_s<256>(Target_NodeName, pNodeName); }

		char	Target_NodeName[256];
		_double Target_dWaitingTime;
		_double Target_dOffset;
	}INFO;

protected:
	explicit CBT_Wait();
	explicit CBT_Wait(const CBT_Wait& rhs);
	virtual ~CBT_Wait() = default;

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack,CBlackBoard* pBlackBoard, _bool bDebugging) override;

public:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging);
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, _bool bDebugging);

private:
	HRESULT Ready_Clone_Node(void* pInit_Struct);
	
private:
	_double		m_dCurTime = 0;
	_double		m_dWaitingTime = 0;
	_double		m_dOffset = 0;

	// dMaxTime = m_dWaitingTime +- m_dOffset
	_double		m_dMaxTime = 0;

public:
	static CBT_Wait* Create_Prototype();
	virtual CBT_Node* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END