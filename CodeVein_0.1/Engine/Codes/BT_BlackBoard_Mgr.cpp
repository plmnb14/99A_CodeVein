#include "../Headers/BT_BlackBoard_Mgr.h"
#include "../Headers/BlackBoard.h" 

IMPLEMENT_SINGLETON(CBT_BlackBoard_Manager)

CBT_BlackBoard_Manager::CBT_BlackBoard_Manager()
{

}

CBlackBoard * CBT_BlackBoard_Manager::Clone_BlackBoard(const _tchar* pPrototypeTag)
{
	CBlackBoard* Temp_BlackBoard = Find_Prototype(pPrototypeTag);

	if (nullptr != Temp_BlackBoard)
		Temp_BlackBoard = Temp_BlackBoard->Clone();

	return Temp_BlackBoard;
}

HRESULT CBT_BlackBoard_Manager::Add_Prototype_BlackBoard(const _tchar* pPrototypeTag, CBlackBoard * pPrototype)
{
	if (nullptr == pPrototype)
		return E_FAIL;

	if (nullptr != Find_Prototype(pPrototypeTag))
		return E_FAIL;

	m_Prototypes.emplace(PROTOTYPES::value_type(pPrototypeTag, pPrototype));

	return S_OK;
}

CBlackBoard * CBT_BlackBoard_Manager::Find_Prototype(const _tchar * pPrototypeTag)
{
	auto iter = find_if(m_Prototypes.begin(), m_Prototypes.end(), CTag_Finder(pPrototypeTag));

	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}

void CBT_BlackBoard_Manager::Free()
{
	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);

	m_Prototypes.clear();

}
