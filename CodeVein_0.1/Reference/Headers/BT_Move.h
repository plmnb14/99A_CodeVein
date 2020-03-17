#pragma once

#include "BT_Task_Node.h"

BEGIN(Engine)
class CTransform;
class ENGINE_DLL CBT_Move final : public CBT_Task_Node
{
public:
	typedef struct tagInitInfo
	{
		CTransform*		m_pTargetTransform;
		_double			m_dMovingTime;
	}INFO;

protected:
	explicit CBT_Move();
	explicit CBT_Move(const CBT_Move& rhs);
	virtual ~CBT_Move() = default;

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack) override;

	HRESULT Initialize_Node(CTransform* pTargetTransform, _double dMovingTime);

protected:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack);
	virtual void End_Node(vector<CBT_Node*>* pNodeStack);

private:
	HRESULT Ready_Clone_Node(void* pInit_Struct);

private:
	CTransform*		m_pTargetTransform = nullptr;
	_double			m_dMovingTime = 0;
	_double			m_dCurTime = 0;

public:
	static CBT_Move* Create_Prototype();
	virtual CBT_Node* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END