#pragma once

#include "BT_Task_Node.h"

BEGIN(Engine)

class CTransform;
class ENGINE_DLL CBT_MoveDirectly final : public CBT_Task_Node
{
public:
	/*
	Target_name이나 Target_Position 둘 중 하나만 전달해주면 된다.
	우선 순위는 1. Target_name, 2. Target_Position
	Acceptable Radius : 타겟으로의 접근을 허용할 최소 길이
	*/
	typedef struct tagInitInfo
	{
		tagInitInfo(char* _pNodeName, CTransform* pThis_Transform, _tchar* _pTarget_Key, _float _fMove_Speed,  _float _fAcceptable_Radius)
			: pTransform(pThis_Transform), fMove_Speed(_fMove_Speed), fAcceptable_Radius(_fAcceptable_Radius)
		{ strcpy_s<256>(Target_NodeName, _pNodeName); 
		lstrcpy(Target_Key, _pTarget_Key); }

		CTransform*	pTransform;
		char		Target_NodeName[256];
		_tchar		Target_Key[256];
		_float		fMove_Speed;
		_float		fAcceptable_Radius;
	} INFO;

protected:
	explicit CBT_MoveDirectly();
	explicit CBT_MoveDirectly(const CBT_MoveDirectly& rhs);
	virtual ~CBT_MoveDirectly() = default;

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, const CBlackBoard* pBlackBoard, _bool bDebugging) override;

public:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, _bool bDebugging);
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, BT_NODE_STATE eState, _bool bDebugging);

private:
	HRESULT Ready_Clone_Node(void* pInit_Struct);

private:
	CTransform*	m_pTransform = nullptr;
	_tchar		m_Target_Key[256] = { 0, };
	_float		m_fMove_Speed = 0.f;
	_float		m_dAcceptable_Radius = 0.f;

public:
	static CBT_MoveDirectly* Create_Prototype();
	virtual CBT_Node* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END