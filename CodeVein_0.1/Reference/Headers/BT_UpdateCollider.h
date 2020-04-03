#pragma once

#include "BT_Service_Node.h"

BEGIN(Engine)
class CCollider;
class ENGINE_DLL CBT_UpdateCollider final : public CBT_Service_Node
{
public:
	typedef struct tagInitInfo
	{
		tagInitInfo(char* pNodeName, OBJECT_PARAM* _pTarget_ObjParam, _double _Service_Start_Time, _uint _Count_Of_Execution, _double dUpdateTime, _double dOffset, CBT_Service_Node::Mode _eMode)
			: Target_ObjParam(_pTarget_ObjParam), Service_Start_Time(_Service_Start_Time), Target_dUpdateTime(dUpdateTime), Target_dOffset(dOffset), Count_Of_Execution(_Count_Of_Execution), eMode(_eMode)
		{ strcpy_s<256>(Target_NodeName, pNodeName); }

		char					Target_NodeName[256] = {0,};
		_double					Target_dUpdateTime = 0;
		_double					Target_dOffset = 0;
		OBJECT_PARAM*			Target_ObjParam = nullptr;
		_double					Service_Start_Time = 0;
		_uint					Count_Of_Execution = 0;
		CBT_Service_Node::Mode	eMode = CBT_Service_Node::Infinite;
	}INFO;

protected:
	explicit CBT_UpdateCollider();
	explicit CBT_UpdateCollider(const CBT_UpdateCollider& rhs);
	virtual ~CBT_UpdateCollider() = default;

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging) override;

public:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging);
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, _bool bDebugging);

private:
	HRESULT Ready_Clone_Node(void* pInit_Struct);

	// dMaxTime = m_dWaitingTime +- m_dOffset
	_double		m_dMaxTime = 0;

private:
	_double				m_dCurTime = 0;
	_double				m_dUpdateTime = 0;
	_double				m_dOffset = 0;

	_double					m_dService_StartTime = 0;
	_bool					m_bService_Start = false;

	OBJECT_PARAM*			m_pTarget_ObjParam = nullptr;

public:
	static CBT_UpdateCollider* Create_Prototype();
	virtual CBT_UpdateCollider* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END