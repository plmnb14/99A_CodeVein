#pragma once

#include "Base.h"

/*
AI�� �γ��� �ش��Ѵ�.
AI���� �ʿ��� ������ ���⿡ �����Ѵ�.
*/
BEGIN(Engine)

class CGameObject;
class CBlackBoard final : public CBase
{
public:
	/*
	enum�� ������ �񱳰� ����������,
	enum class�� �ش��ϴ� enum�̶��� �񱳰� �����ϴ�.
	ex) if( 0 == MyEnum )  �Ұ�
	    if( BOOL == MyEnum) ����
	*/
	enum class BLACKBOARD_VALUE_STATE { BOOL, FLOAT, INT, VEC3};
	enum class BLACKBOARD_OUTPUT_STATE { NONE, UPDATE};

private:
	explicit CBlackBoard();
	explicit CBlackBoard(const CBlackBoard& rhs);
	virtual ~CBlackBoard() = default;

public:
	/*
	pName : �����Ϳ� �ٿ��� �̸�
	pData : ���� ������
	BLACKBOARD_VALUE : ���� �����Ϳ� �ش��ϴ� Enum

	���� �ش� ������ �̸��� �̹� �����Ѵٸ�, �� ���� �����.
	*/
	BLACKBOARD_OUTPUT_STATE Set_Value(const _tchar* pName, const void* pData, BLACKBOARD_VALUE_STATE eValue_Type);

public:
	/*
	template T : OutPut �ڷ���
	pName : ã�� ���� �̸�
	BLACKBOARD_VALUE : ã�� ���� �ش��ϴ� Enum
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
	//����ҵ�
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