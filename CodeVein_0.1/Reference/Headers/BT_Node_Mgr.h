#pragma once

#include "Base.h"
#include "../Headers/BT_All_Node.h"

BEGIN(Engine)

class CBT_Node;
class CBehaviorTree;
class CBT_Node_Manager final : public CBase
{
public:
	enum NODE_TYPE { COMPOSITE, DECORATOR, SERVICE, TASK, NODE_END };

	DECLARE_SINGLETON(CBT_Node_Manager)
private:
	explicit CBT_Node_Manager();
	virtual ~CBT_Node_Manager() = default;

public:
	CBT_Node* Clone_Node(const _tchar* pPrototypeTag, NODE_TYPE eType, void* pInit_Struct);

public:
	HRESULT Add_Prototype_Node(const _tchar* pPrototypeTag, CBT_Node* pPrototype, NODE_TYPE eType);

	HRESULT Ready_BT_Node();

private:
	CBT_Node* Find_Prototype(const _tchar* pPrototypeTag, NODE_TYPE eType);

private:
	map<const _tchar*, CBT_Node*>			m_Prototypes[NODE_END]; 
	typedef map<const _tchar*, CBT_Node*>	PROTOTYPES;

public:
	virtual void Free();
};

END