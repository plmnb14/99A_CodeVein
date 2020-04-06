#pragma once

#include "BT_Task_Node.h"

BEGIN(Engine)

class CNavMesh;
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
		tagInitInfo(char* _pNodeName, CTransform* pThis_Transform, CNavMesh* _pNavMesh, _tchar* _pTarget_Key, _tchar* _pBB_Speed_Key, _tchar* _pBB_MoveDir_Key, _float _fMove_Speed,  _float _fAcceptable_Radius, _double _MoveTime, _double _TimeOffset, MODE _eMode)
			: pTransform(pThis_Transform), pNavMesh(_pNavMesh), fMove_Speed(_fMove_Speed), fAcceptable_Radius(_fAcceptable_Radius), dMoveTime(_MoveTime), dTimeOffset(_TimeOffset), eMode(_eMode)
		{ strcpy_s<256>(Target_NodeName, _pNodeName);
		if (nullptr != _pTarget_Key) lstrcpy(Target_Key, _pTarget_Key);
		lstrcpy(BB_Speed_Key, _pBB_Speed_Key); lstrcpy(BB_MoveDir_Key, _pBB_MoveDir_Key);}
		
		CTransform*	pTransform = nullptr;
		CNavMesh*	pNavMesh = nullptr;
		char		Target_NodeName[256] = { 0, };
		_tchar		Target_Key[256] = { 0, };
		_tchar		BB_Speed_Key[256] = { 0, };
		_tchar		BB_MoveDir_Key[256] = { 0, };
		_float		fMove_Speed = 0;
		_float		fAcceptable_Radius = 0;
		_double		dMoveTime = 0;
		_double		dTimeOffset = 0;
		MODE		eMode = CHASE;
	} INFO;

protected:
	explicit CBT_MoveDirectly();
	explicit CBT_MoveDirectly(const CBT_MoveDirectly& rhs);
	virtual ~CBT_MoveDirectly() = default;

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging) override;

public:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging);
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, CBlackBoard* pBlackBoard, _bool bDebugging);

private:
	HRESULT Ready_Clone_Node(void* pInit_Struct);

	void Look_At_Target(_double TimeDelta, const _v3& Target_Pos);

private:
	CTransform*	m_pTransform = nullptr;
	CNavMesh*	m_pNavMesh = nullptr;
	_tchar		m_Target_Key[256] = { 0, };
	_tchar		m_BB_SpeedKey[256] = { 0, };
	_tchar		m_BB_MoveDir_Key[256] = { 0, };
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