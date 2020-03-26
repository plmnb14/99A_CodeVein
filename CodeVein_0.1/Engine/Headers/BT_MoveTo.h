#pragma once

#include "..\headers\BT_Task_Node.h"

BEGIN(Engine)
class CTransform;
class ENGINE_DLL CBT_MoveTo final : public CBT_Task_Node
{
public:
	typedef struct tagInitInfo
	{
		tagInitInfo(char* pNodeName, CTransform* pTransform, _double dMoveSpeed, _double dMovingTime)
			: Target_pTargetTransform(pTransform), Target_dMoveSpeed(dMovingTime)
			, Target_dMovingTime(dMovingTime)			
		{ strcpy_s<256>(Target_NodeName, pNodeName); }

		char			Target_NodeName[256];
		CTransform*		Target_pTargetTransform;
		_double			Target_dMovingTime;
		_double			Target_dMoveSpeed;
	}INFO;

protected:
	explicit CBT_MoveTo();
	explicit CBT_MoveTo(const CBT_MoveTo& rhs);
	virtual ~CBT_MoveTo() = default;

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging) override;

public:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging);
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, _bool bDebugging);

private:
	HRESULT Ready_Clone_Node(void* pInit_Struct);

private:
	CTransform*		m_pTransform = nullptr;
	_double			m_dMoveSpeed = 0;
	_double			m_dMovingTime = 0;

	_double			m_dCurTime = 0;

public:
	static CBT_MoveTo* Create_Prototype();
	virtual CBT_Node* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END