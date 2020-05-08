#pragma once

#include "BT_Task_Node.h"

BEGIN(Engine)

class CNavMesh;
class CTransform;
class ENGINE_DLL CBT_MoveAround final : public CBT_Task_Node
{
public:

	typedef struct tagInitInfo
	{
		tagInitInfo(char* _pNodeName, CTransform* pThis_Transform, CNavMesh* _pNavMesh, _tchar* _pTarget_Key, _tchar* _pBB_Speed_Key, _tchar* _pBB_MoveDir_Key, _float _fMove_Speed, _double _MoveTime, _double _TimeOffset)
			: pTransform(pThis_Transform), pNavMesh(_pNavMesh), fMove_Speed(_fMove_Speed), dMoveTime(_MoveTime), dTimeOffset(_TimeOffset)
		{ strcpy_s<STR_128>(Target_NodeName, _pNodeName); lstrcpy(Target_Key, _pTarget_Key);
		lstrcpy(BB_Speed_Key, _pBB_Speed_Key); lstrcpy(BB_MoveDir_Key, _pBB_MoveDir_Key);}
		
		CTransform*	pTransform = nullptr;
		CNavMesh*	pNavMesh = nullptr;
		char		Target_NodeName[STR_128] = { 0, };
		_tchar		Target_Key[STR_128] = { 0, };
		_tchar		BB_Speed_Key[STR_128] = { 0, };
		_tchar		BB_MoveDir_Key[STR_128] = { 0, };
		_float		fMove_Speed = 0;
		_double		dMoveTime = 0;
		_double		dTimeOffset = 0;
	} INFO;

protected:
	explicit CBT_MoveAround();
	explicit CBT_MoveAround(const CBT_MoveAround& rhs);
	virtual ~CBT_MoveAround() = default;

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging) override;

public:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging);
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, CBlackBoard* pBlackBoard, _bool bDebugging);

private:
	HRESULT Ready_Clone_Node(void* pInit_Struct);

private:
	CTransform*	m_pTransform = nullptr;
	CNavMesh*	m_pNavMesh = nullptr;
	_tchar		m_Target_Key[STR_128] = { 0, };
	_tchar		m_BB_SpeedKey[STR_128] = { 0, };
	_tchar		m_BB_MoveDir_Key[STR_128] = { 0, };
	_float		m_fMove_Speed = 0.f;
	_double		m_dCurTime = 0;
	_double		m_dMoveTime = 0;
	_double		m_dTimeOffset = 0;

	_double		m_dMaxTime = 0;

public:
	static CBT_MoveAround* Create_Prototype();
	virtual CBT_Node* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END