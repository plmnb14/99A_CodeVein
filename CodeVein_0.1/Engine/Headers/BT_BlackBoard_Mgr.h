#pragma once

#include "Base.h"

BEGIN(Engine)

class CBlackBoard;
class CBT_BlackBoard_Manager final : public CBase
{
	DECLARE_SINGLETON(CBT_BlackBoard_Manager)
private:
	explicit CBT_BlackBoard_Manager();
	virtual ~CBT_BlackBoard_Manager() = default;

public:
	CBlackBoard* Clone_BlackBoard(const _tchar* pPrototypeTag);

public:
	HRESULT Add_Prototype_BlackBoard(const _tchar* pPrototypeTag, CBlackBoard* pPrototype);

private:
	CBlackBoard* Find_Prototype(const _tchar* pPrototypeTag);

private:
	map<const _tchar*, CBlackBoard*>			m_Prototypes;
	typedef map<const _tchar*, CBlackBoard*>	PROTOTYPES;

public:
	virtual void Free();
};

END