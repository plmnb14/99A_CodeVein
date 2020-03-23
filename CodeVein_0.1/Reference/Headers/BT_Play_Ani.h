#pragma once

#include "..\headers\BT_Task_Node.h"

BEGIN(Engine)
class CMesh_Dynamic;
class ENGINE_DLL CBT_Play_Ani final : public CBT_Task_Node
{
public:
	typedef struct tagInitInfo
	{
		tagInitInfo(char* pNodeName, CMesh_Dynamic* pMesh_Dynamic, _uint iAni_Index)
			: Target_pMesh_Dynamic(pMesh_Dynamic), Target_iAni_iIndex(iAni_Index)
		{ strcpy_s<256>(Target_NodeName, pNodeName); }

		char			Target_NodeName[256];
		CMesh_Dynamic*	Target_pMesh_Dynamic;
		_uint			Target_iAni_iIndex;
	}INFO;

protected:
	explicit CBT_Play_Ani();
	explicit CBT_Play_Ani(const CBT_Play_Ani& rhs);
	virtual ~CBT_Play_Ani() = default;

public:
	virtual BT_NODE_STATE Update_Node(_double TimeDelta, vector<CBT_Node*>* pNodeStack, list<vector<CBT_Node*>*>* plistSubNodeStack, const CBlackBoard* pBlackBoard, _bool bDebugging) override;

public:
	virtual void Start_Node(vector<CBT_Node*>* pNodeStack, _bool bDebugging);
	virtual BT_NODE_STATE End_Node(vector<CBT_Node*>* pNodeStack, BT_NODE_STATE eState, _bool bDebugging);

private:
	HRESULT Ready_Clone_Node(void* pInit_Struct);

private:
	CMesh_Dynamic*	m_pMesh_Dynamic = nullptr;
	_uint			m_iAni_Index = 0;

public:
	static CBT_Play_Ani* Create_Prototype();
	virtual CBT_Node* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END