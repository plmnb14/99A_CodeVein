#pragma once

#include "..\headers\BT_Task_Node.h"

BEGIN(Engine)
class CTransform;
class CNavMesh;
class CRigidBody;
class ENGINE_DLL CBT_MoveTo final : public CBT_Task_Node
{
public:
	typedef struct tagInitInfo
	{
		tagInitInfo(char* pNodeName, CTransform* pTransform, CNavMesh* _pNavMesh, CRigidBody* _pRigid, _tchar* _pPos_Key, _double _dMovingTime)
			: pTargetTransform(pTransform), pNavMesh(_pNavMesh), pRigid(_pRigid), Target_dMovingTime(_dMovingTime)
		{ strcpy_s<STR_128>(Target_NodeName, pNodeName);
		lstrcpy(Pos_Key, _pPos_Key); }

		char			Target_NodeName[STR_128] = {0,};
		CTransform*		pTargetTransform = nullptr;
		CNavMesh*		pNavMesh = nullptr;
		CRigidBody*		pRigid = nullptr;
		_tchar			Pos_Key[STR_128] = { 0, };
		_double			Target_dMovingTime = 0;
	}INFO;

protected:
	explicit CBT_MoveTo();
	explicit CBT_MoveTo(const CBT_MoveTo& rhs);
	virtual ~CBT_MoveTo() = default;

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging) override;

public:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging);
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, CBlackBoard* pBlackBoard, _bool bDebugging);

private:
	HRESULT Ready_Clone_Node(void* pInit_Struct);

	HRESULT Cal_MoveSpeed(CBlackBoard* pBB);

private:
	CTransform*		m_pTransform = nullptr;
	CNavMesh*		m_pNavMesh = nullptr;
	CRigidBody*		m_pRigid = nullptr;
	_tchar			m_pPosKey[STR_128] = { 0, };
	_double			m_dMovingTime = 0;

	_double			m_dCurTime = 0;

	_float			m_fMoveSpeed = 0;
	_v3				m_vMoveDir = _v3(0.f, 0.f, 0.f);
public:
	static CBT_MoveTo* Create_Prototype();
	virtual CBT_Node* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END