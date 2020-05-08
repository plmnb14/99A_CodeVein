#pragma once

#include "BT_Service_Node.h"

BEGIN(Engine)

class ENGINE_DLL CBT_CreateBullet final : public CBT_Service_Node
{
public:
	typedef struct tagInitInfo
	{
		tagInitInfo(char* pNodeName, _tchar* _Object_Tag, _tchar* _Create_Pos_Key, _tchar* _Dir_Key, _float _fSpeed, _double _dLifeTime, _double _dService_Start_Time, _uint _MaxCount_Of_Execution, _double dCreateTime, _double dOffset, CBT_Service_Node::Mode _eMode)
			: fSpeed(_fSpeed), dLifeTime(_dLifeTime), Service_Start_Time(_dService_Start_Time), Target_dCreateTime(dCreateTime), Target_dOffset(dOffset), MaxCount_Of_Execution(_MaxCount_Of_Execution), eMode(_eMode)
		{strcpy_s<STR_128>(Target_NodeName, pNodeName);
		lstrcpy(Object_Tag, _Object_Tag);
		lstrcpy(Create_Pos_Key, _Create_Pos_Key);
		lstrcpy(Dir_Key, _Dir_Key);}

		char					Target_NodeName[STR_128] = {0,};
		_tchar					Object_Tag[STR_128] = { 0, };
		_tchar					Create_Pos_Key[STR_128] = { 0, };
		_tchar					Dir_Key[STR_128] = { 0, };
		_float					fSpeed = 0.f;
		_double					dLifeTime = 0;
		_double					Service_Start_Time = 0;
		_double					Target_dCreateTime = 0;
		_double					Target_dOffset = 0;
		_uint					MaxCount_Of_Execution = 0;
		CBT_Service_Node::Mode	eMode = CBT_Service_Node::Infinite;
		

	}INFO;

protected:
	explicit CBT_CreateBullet();
	explicit CBT_CreateBullet(const CBT_CreateBullet& rhs);
	virtual ~CBT_CreateBullet() = default;

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, CBlackBoard* pBlackBoard, _bool bDebugging) override;

public:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, _bool bDebugging);
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, BT_NODE_STATE eState, CBlackBoard* pBlackBoard, _bool bDebugging);

private:
	HRESULT Ready_Clone_Node(void* pInit_Struct);

	// dMaxTime = m_dWaitingTime +- m_dOffset
	_double		m_dMaxTime = 0;

private:
	_tchar					m_pObject_Tag[STR_128] = { 0, };
	_tchar					m_pCreate_Pos_Key[STR_128] = { 0, };
	_tchar					m_pDir_Key[STR_128] = { 0, };

	_v3						m_vCreate_Pos = _v3(0.f, 0.f, 0.f);
	_v3						m_vDir = _v3(0.f, 0.f, 0.f);
	_float					m_fSpeed = 0.f;
	_double					m_dLifeTime = 0;

	_double					m_dService_StartTime = 0;
	_bool					m_bService_Start = false;

	_double					m_dCurTime = 0;
	_double					m_dCreateTime = 0;
	_double					m_dOffset = 0;
	
public:
	static CBT_CreateBullet* Create_Prototype();
	virtual CBT_CreateBullet* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END