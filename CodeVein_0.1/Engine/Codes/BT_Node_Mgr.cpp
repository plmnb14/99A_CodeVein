#include "../Headers/BT_Node_Mgr.h"

IMPLEMENT_SINGLETON(CBT_Node_Manager)

CBT_Node_Manager::CBT_Node_Manager()
{

}

CBT_Node* CBT_Node_Manager::Clone_Node(const _tchar* pPrototypeTag, NODE_TYPE eType, void* pInit_Struct)
{
	CBT_Node* Temp_Node = Find_Prototype(pPrototypeTag, eType);

	if (nullptr != Temp_Node)
	{
		Temp_Node = Temp_Node->Clone(pInit_Struct);

		return Temp_Node;
	}

	return nullptr;
}

HRESULT CBT_Node_Manager::Add_Prototype_Node(const _tchar* pPrototypeTag, CBT_Node * pPrototype, NODE_TYPE eType)
{
	if (nullptr == pPrototype)
		return E_FAIL;

	if (nullptr != Find_Prototype(pPrototypeTag, eType))
		return E_FAIL;

	m_Prototypes[eType].emplace(PROTOTYPES::value_type(pPrototypeTag, pPrototype));

	return S_OK;
}

HRESULT CBT_Node_Manager::Ready_BT_Node()
{
	// Composite
	if (FAILED(Add_Prototype_Node(L"Selector", CBT_Selector::Create_Prototype(), CBT_Node_Manager::COMPOSITE)))
		return E_FAIL;
	if (FAILED(Add_Prototype_Node(L"Sequence", CBT_Sequence::Create_Prototype(), CBT_Node_Manager::COMPOSITE)))
		return E_FAIL;
	if (FAILED(Add_Prototype_Node(L"Simple_Parallel", CBT_Simple_Parallel::Create_Prototype(), CBT_Node_Manager::COMPOSITE)))
		return E_FAIL;

	// Decorator
	if (FAILED(Add_Prototype_Node(L"Inverter", CBT_Inverter::Create_Prototype(), CBT_Node_Manager::DECORATOR)))
		return E_FAIL;
	if (FAILED(Add_Prototype_Node(L"Loop", CBT_Loop::Create_Prototype(), CBT_Node_Manager::DECORATOR)))
		return E_FAIL;
	if (FAILED(Add_Prototype_Node(L"Cooldown", CBT_Cooldown::Create_Prototype(), CBT_Node_Manager::DECORATOR)))
		return E_FAIL;
	if (FAILED(Add_Prototype_Node(L"ConeCheck", CBT_ConeCheck::Create_Prototype(), CBT_Node_Manager::DECORATOR)))
		return E_FAIL;
	if (FAILED(Add_Prototype_Node(L"Compare", CBT_Compare::Create_Prototype(), CBT_Node_Manager::DECORATOR)))
		return E_FAIL;

	// Service
	if (FAILED(Add_Prototype_Node(L"UpdatePos", CBT_UpdatePos::Create_Prototype(), CBT_Node_Manager::SERVICE)))
		return E_FAIL;
	if (FAILED(Add_Prototype_Node(L"UpdateHPRatio", CBT_UpdateGageRatio::Create_Prototype(), CBT_Node_Manager::SERVICE)))
		return E_FAIL;

	// Task
	if (FAILED(Add_Prototype_Node(L"Wait", CBT_Wait::Create_Prototype(), CBT_Node_Manager::TASK)))
		return E_FAIL;
	if (FAILED(Add_Prototype_Node(L"MoveTo", CBT_MoveTo::Create_Prototype(), CBT_Node_Manager::TASK)))
		return E_FAIL;
	if (FAILED(Add_Prototype_Node(L"MoveDirectly", CBT_MoveDirectly::Create_Prototype(), CBT_Node_Manager::TASK)))
		return E_FAIL;
	if (FAILED(Add_Prototype_Node(L"RotationDir", CBT_RotationDir::Create_Prototype(), CBT_Node_Manager::TASK)))
		return E_FAIL;
	if (FAILED(Add_Prototype_Node(L"Play_Ani", CBT_Play_Ani::Create_Prototype(), CBT_Node_Manager::TASK)))
		return E_FAIL;

	return S_OK;
}

CBT_Node * CBT_Node_Manager::Find_Prototype(const _tchar * pPrototypeTag, NODE_TYPE eType)
{
	auto iter = find_if(m_Prototypes[eType].begin(), m_Prototypes[eType].end(), CTag_Finder(pPrototypeTag));

	if (iter == m_Prototypes[eType].end())
		return nullptr;

	return iter->second;
}

void CBT_Node_Manager::Free()
{
	for (size_t i = 0; i < NODE_END; ++i)
	{
		for (auto& Pair : m_Prototypes[i])
			Safe_Release(Pair.second);

		m_Prototypes[i].clear();
	}
}
