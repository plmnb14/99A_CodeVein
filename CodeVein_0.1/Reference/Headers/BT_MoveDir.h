#pragma once

#include "BT_Task_Node.h"

BEGIN(Engine)

class CTransform;
class ENGINE_DLL CBT_MoveDir final : public CBT_Task_Node
{
public:

	typedef struct tagInitInfo
	{
		tagInitInfo(char* _pNodeName, CTransform* pThis_Transform, _tchar* _pDir_Key, _tchar* _pBB_Speed_Key, _tchar* _pBB_MoveDir_Key, _float _fMove_Speed, _double _MoveTime, _double _TimeOffset)
			: pTransform(pThis_Transform), fMove_Speed(_fMove_Speed), dMoveTime(_MoveTime), dTimeOffset(_TimeOffset)
		{ strcpy_s<256>(Target_NodeName, _pNodeName); lstrcpy(Dir_Key, _pDir_Key);}
		
		CTransform*	pTransform = nullptr;
		char		Target_NodeName[256] = { 0, };
		_tchar		Dir_Key[256] = { 0, };
		_tchar		BB_Speed_Key[256] = { 0, };
		_tchar		BB_MoveDir_Key[256] = { 0, };
		_float		fMove_Speed = 0;
		_double		dMoveTime = 0;
		_double		dTimeOffset = 0;
	} INFO;

protected:
	explicit CBT_MoveDir();
	explicit CBT_MoveDir(const CBT_MoveDir& rhs);
	virtual ~CBT_MoveDir() = default;

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging) override;

public:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging);
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, CBlackBoard* pBlackBoard, _bool bDebugging);

private:
	HRESULT Ready_Clone_Node(void* pInit_Struct);

private:
	CTransform*	m_pTransform = nullptr;
	_tchar		m_Dir_Key[256] = { 0, };
	_tchar		m_BB_SpeedKey[256] = { 0, };
	_tchar		m_BB_MoveDir_Key[256] = { 0, };
	_float		m_fMove_Speed = 0.f;

	_double		m_dCurTime = 0;
	_double		m_dMoveTime = 0;
	_double		m_dTimeOffset = 0;

	_double		m_dMaxTime = 0;

public:
	static CBT_MoveDir* Create_Prototype();
	virtual CBT_Node* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END