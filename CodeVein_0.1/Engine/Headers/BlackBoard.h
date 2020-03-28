#pragma once

#include "Base.h"

/*
AI�� �γ��� �ش��Ѵ�.
AI���� �ʿ��� ������ ���⿡ �����Ѵ�.
*/
BEGIN(Engine)

class CGameObject;
class ENGINE_DLL CBlackBoard final : public CBase
{
public:
	enum VALUE { BOOL, FLOAT, INT, VEC3};
	enum OUTPUT { NONE, UPDATE};

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
	OUTPUT Set_Value(const _tchar* pName, _bool bValue);
	OUTPUT Set_Value(const _tchar* pName, _float fValue);
	OUTPUT Set_Value(const _tchar* pName, _int iValue);
	OUTPUT Set_Value(const _tchar* pName, _v3 _vValue);

public:
	const _bool Get_BoolValue(const _tchar* pName) const;
	const _float Get_FloatValue(const _tchar* pName) const;
	const _int Get_IntValue(const _tchar* pName) const;
	const _v3 Get_V3Value(const _tchar* pName) const;

private:
	const _bool* Find_Value_In_mapBool(const _tchar* pName) const;
	const _float* Find_Value_In_mapFloat(const _tchar* pName) const;
	const _int* Find_Value_In_mapInt(const _tchar* pName) const;
	const _v3* Find_Value_In_mapVec3(const _tchar* pName) const;

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