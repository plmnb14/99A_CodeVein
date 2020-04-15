#pragma once

#include "BT_Service_Node.h"

BEGIN(Engine)

class CTransform;
class CAIController;
class ENGINE_DLL CBT_CreateBuff final : public CBT_Service_Node
{
public:
	typedef struct tagBuffInfo
	{
		tagBuffInfo(CTransform* _pTransform, CAIController* _pAIController, _double	_dLifeTime)
			: pTransform(_pTransform), pAIController(_pAIController), dLifeTime(_dLifeTime)
		{}

		CTransform*		pTransform = nullptr;
		CAIController*	pAIController = nullptr;
		_double			dLifeTime = 0;
	}BUFF_INFO;


	typedef struct tagInitInfo
	{
		tagInitInfo(char* pNodeName, _tchar* _Object_Tag, CTransform* _pTransform, CAIController* _pAIController, _double _dLifeTime, _double _dService_Start_Time, _uint _MaxCount_Of_Execution, _double dCreateTime, _double dOffset, CBT_Service_Node::Mode _eMode)
			: pTransform(_pTransform), pAIController(_pAIController), dLifeTime(_dLifeTime), Service_Start_Time(_dService_Start_Time), Target_dCreateTime(dCreateTime), Target_dOffset(dOffset), MaxCount_Of_Execution(_MaxCount_Of_Execution), eMode(_eMode)
		{strcpy_s<256>(Target_NodeName, pNodeName);
		lstrcpy(Object_Tag, _Object_Tag);}

		char					Target_NodeName[256] = {0,};
		_tchar					Object_Tag[256] = { 0, };
		CTransform*				pTransform = nullptr;
		CAIController*			pAIController = nullptr;
		_double					dLifeTime = 0;
		_double					Service_Start_Time = 0;
		_double					Target_dCreateTime = 0;
		_double					Target_dOffset = 0;
		_uint					MaxCount_Of_Execution = 0;
		CBT_Service_Node::Mode	eMode = CBT_Service_Node::Infinite;
		

	}INFO;

protected:
	explicit CBT_CreateBuff();
	explicit CBT_CreateBuff(const CBT_CreateBuff& rhs);
	virtual ~CBT_CreateBuff() = default;

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
	_tchar					m_pObject_Tag[256] = { 0, };

	CTransform*				m_pTransform = nullptr;
	CAIController*			m_pAIcontroller = nullptr;

	_double					m_dLifeTime = 0;

	_double					m_dService_StartTime = 0;
	_bool					m_bService_Start = false;

	_double					m_dCurTime = 0;
	_double					m_dCreateTime = 0;
	_double					m_dOffset = 0;
	
public:
	static CBT_CreateBuff* Create_Prototype();
	virtual CBT_CreateBuff* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END