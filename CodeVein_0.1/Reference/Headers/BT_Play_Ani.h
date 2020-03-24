#pragma once

#include "..\headers\BT_Task_Node.h"

BEGIN(Engine)
class CMesh_Dynamic;
class ENGINE_DLL CBT_Play_Ani final : public CBT_Task_Node
{
public:
	/*
	fWeight : 애니메이션 끝나는 시간 보정, defalt = 1.f;
	*/
	typedef struct tagInitInfo
	{
		tagInitInfo(char* pNodeName, CMesh_Dynamic* pMesh_Dynamic, _uint _iAni_Index, _float _fAniWeight = 0.9f)
			: Target_pMesh_Dynamic(pMesh_Dynamic), Target_iAni_iIndex(_iAni_Index), fAniWeight(_fAniWeight)
		{ strcpy_s<256>(Target_NodeName, pNodeName); }

		char			Target_NodeName[256];
		CMesh_Dynamic*	Target_pMesh_Dynamic;
		_uint			Target_iAni_iIndex;
		_float			fAniWeight;
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
	_float			m_fAniWeight = 0.f;

public:
	static CBT_Play_Ani* Create_Prototype();
	virtual CBT_Node* Clone(void* pInit_Struct) override;

	virtual void Free();
};

END