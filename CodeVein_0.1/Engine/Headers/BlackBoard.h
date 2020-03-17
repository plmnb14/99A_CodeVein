#pragma once

#include "Base.h"

/*
AI의 두뇌에 해당한다.
AI에게 필요한 값들은 여기에 보관한다.
*/
BEGIN(Engine)

class CGameObject;
class CBlackBoard final : public CBase
{
public:
	/*
	enum은 정수와 비교가 가능하지만,
	enum class는 해당하는 enum이랑만 비교가 가능하다.
	ex) if( 0 == MyEnum )  불가
	    if( BOOL == MyEnum) 가능
	*/
	enum class BLACKBOARD_VALUE_STATE { BOOL, FLOAT, INT, VEC3};
	enum class BLACKBOARD_OUTPUT_STATE { NONE, UPDATE};

private:
	explicit CBlackBoard();
	explicit CBlackBoard(const CBlackBoard& rhs);
	virtual ~CBlackBoard() = default;

public:
	/*
	pName : 데어터에 붙여줄 이름
	pData : 넣을 데이터
	BLACKBOARD_VALUE : 넣을 데이터에 해당하는 Enum

	만약 해당 변수의 이름이 이미 존재한다면, 그 값에 덮어쓴다.
	*/
	BLACKBOARD_OUTPUT_STATE Set_Value(const _tchar* pName, const void* pData, BLACKBOARD_VALUE_STATE eValue_Type);

public:
	/*
	template T : OutPut 자료형
	pName : 찾을 값의 이름
	BLACKBOARD_VALUE : 찾을 값에 해당하는 Enum
	*/
	template<class T>
	T Get_Value(const _tchar* pName, BLACKBOARD_VALUE_STATE eValue_Type) const;

private:
	const _bool* Find_Value_In_mapBool(const _tchar* pName);
	const _float* Find_Value_In_mapFloat(const _tchar* pName);
	const _int* Find_Value_In_mapInt(const _tchar* pName);
	const _v3* Find_Value_In_mapVec3(const _tchar* pName);

private:
	HRESULT Ready_BlackBoard();

private:
	//저장소들
	map<const _tchar*, _bool>					m_mapBool;
	typedef map<const _tchar*, _bool>			MAP_BOOL;

	map<const _tchar*, _float>					m_mapFloat;
	typedef map<const _tchar*, _float>			MAP_FLOAT;

	map<const _tchar*, _int>					m_mapInt;
	typedef map<const _tchar*, _int>			MAP_INT;

	map<const _tchar*, _v3>						m_mapVec3;
	typedef map<const _tchar*, _v3>				MAP_VEC3;


public:
	static CBlackBoard* Create();
	CBlackBoard* Clone();

	virtual void Free() override;
};

END