#pragma once

#include "BT_Task_Node.h"

BEGIN(Engine)

class CTransform;
class ENGINE_DLL CBT_MoveDirectly final : public CBT_Task_Node
{
public:
	/*
	CHASE : 추적
	RUSH : 타겟 방향으로 돌진
	*/
	enum MODE { CHASE, RUSH };

	/*
	CHASE
	Acceptable Radius : 타겟으로의 접근을 허용할 최소 길이

	RUSH


	*/
	typedef struct tagInitInfo
	{
		tagInitInfo(char* _pNodeName, CTransform* pThis_Transform, _tchar* _pTarget_Key, _float _fMove_Speed,  _float _fAcceptable_Radius, _double _MoveTime, _double _TimeOffset, MODE _eMode)
			: pTransform(pThis_Transform), fMove_Speed(_fMove_Speed), fAcceptable_Radius(_fAcceptable_Radius), dMoveTime(_MoveTime), dTimeOffset(_TimeOffset), eMode(_eMode)
		{ strcpy_s<256>(Target_NodeName, _pNodeName); 
		if(nullptr != _pTarget_Key)lstrcpy(Target_Key, _pTarget_Key); 
		else ZeroMemory(Target_Key, sizeof(_tchar) * 256);}

		CTransform*	pTransform;
		char		Target_NodeName[256];
		_tchar		Target_Key[256];
		_float		fMove_Speed;
		_float		fAcceptable_Radius;
		_double		dMoveTime;
		_double		dTimeOffset;
		MODE		eMode;
	} INFO;

protected:
	explicit CBT_MoveDirectly();
	explicit CBT_MoveDirectly(const CBT_MoveDirectly& rhs);
	virtual ~CBT_MoveDirectly() = default;

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging) override;

public:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging);
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, _bool bDebugging);

private:
	HRESULT Ready_Clone_Node(void* pInit_Struct);

private:
	CTransform*	m_pTransform = nullptr;
	_tchar		m_Target_Key[256] = { 0, };
	_float		m_fMove_Speed = 0.f;
	_float		m_dAcceptable_Radius = 0.f;
	_double		m_dCurTime = 0;
	_double		m_dMoveTime = 0;
	_double		m_dTimeOffset = 0;
	MODE		m_eMode = CHASE;

	_double		m_dMaxTime = 0;

public:
	static CBT_MoveDirectly* Create_Prototype();
	virtual CBT_Node* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END