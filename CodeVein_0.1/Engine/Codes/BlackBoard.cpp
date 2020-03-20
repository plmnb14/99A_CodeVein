#include "../Headers/BlackBoard.h"

CBlackBoard::CBlackBoard()
{
}

CBlackBoard::CBlackBoard(const CBlackBoard & rhs)
{
	m_mapBool.insert(rhs.m_mapBool.begin(), rhs.m_mapBool.end());
	m_mapFloat.insert(rhs.m_mapFloat.begin(), rhs.m_mapFloat.end());
	m_mapInt.insert(rhs.m_mapInt.begin(), rhs.m_mapInt.end());
	m_mapVec3.insert(rhs.m_mapVec3.begin(), rhs.m_mapVec3.end());
}

CBlackBoard::BLACKBOARD_OUTPUT_STATE CBlackBoard::Set_Value(const _tchar * pName, const void * pData, BLACKBOARD_VALUE_STATE eValue_Type)
{
	//switch (eValue_Type)
	//{
	//case BLACKBOARD_VALUE_STATE::BOOL:
	//	auto iter = find_if(m_mapBool.begin(), m_mapBool.end(), CTag_Finder(pName));

	//	if (iter == m_mapBool.end())
	//		m_mapBool.emplace(MAP_BOOL::value_type(pName, *(_bool*)pData));
	//	else
	//		iter->second = *(_bool*)pData;
	//	break;
	//case BLACKBOARD_VALUE_STATE::FLOAT:
	//	auto iter = find_if(m_mapBool.begin(), m_mapBool.end(), CTag_Finder(pName));

	//	if (iter == m_mapBool.end())
	//		m_mapBool.emplace(MAP_BOOL::value_type(pName, *(_bool*)pData));
	//	else
	//		iter->second = *(_bool*)pData;
	//	break;
	//case BLACKBOARD_VALUE_STATE::INT:
	//	auto iter = find_if(m_mapBool.begin(), m_mapBool.end(), CTag_Finder(pName));

	//	if (iter == m_mapBool.end())
	//		m_mapBool.emplace(MAP_BOOL::value_type(pName, *(_bool*)pData));
	//	else
	//		iter->second = *(_bool*)pData;
	//	break;
	//case BLACKBOARD_VALUE_STATE::VEC3:
	//	auto iter = find_if(m_mapBool.begin(), m_mapBool.end(), CTag_Finder(pName));

	//	if (iter == m_mapBool.end())
	//		m_mapBool.emplace(MAP_BOOL::value_type(pName, *(_bool*)pData));
	//	else
	//		iter->second = *(_bool*)pData;
	//	break;
	//case BLACKBOARD_VALUE_STATE::GAMEOBJECT:
	//	auto iter = find_if(m_mapBool.begin(), m_mapBool.end(), CTag_Finder(pName));

	//	if (iter == m_mapBool.end())
	//		m_mapBool.emplace(MAP_BOOL::value_type(pName, *(_bool*)pData));
	//	else
	//		iter->second = *(_bool*)pData;
	//	break;
	//default:
	//	break;
	//}

	//return E_NOTIMPL;

	/*
	반환값
	NONE : 값 변경이 없을 떄
	UPDATE : 기존 값이 변하거나, 새로운 변수를 할당할 때
	*/


	BLACKBOARD_OUTPUT_STATE eState = BLACKBOARD_OUTPUT_STATE::NONE;

	if (eValue_Type == BLACKBOARD_VALUE_STATE::BOOL)
	{
		auto iter = find_if(m_mapBool.begin(), m_mapBool.end(), CTag_Finder(pName));

		if (iter == m_mapBool.end())
		{
			m_mapBool.emplace(MAP_BOOL::value_type(pName, *(_bool*)pData));
			eState = BLACKBOARD_OUTPUT_STATE::UPDATE;
		}
		else
		{
			if (iter->second != *(_bool*)pData)
			{
				iter->second = *(_bool*)pData;
				eState = BLACKBOARD_OUTPUT_STATE::UPDATE;
			}
		}
	}

	else if (eValue_Type == BLACKBOARD_VALUE_STATE::FLOAT)
	{
		auto iter = find_if(m_mapFloat.begin(), m_mapFloat.end(), CTag_Finder(pName));

		if (iter == m_mapFloat.end())
		{
			m_mapFloat.emplace(MAP_FLOAT::value_type(pName, *(_float*)pData));
			eState = BLACKBOARD_OUTPUT_STATE::UPDATE;
		}
		else
		{
			if (iter->second != *(_float*)pData)
			{	
				iter->second = *(_float*)pData;
				eState = BLACKBOARD_OUTPUT_STATE::UPDATE;
			}
		}
	}

	else if (eValue_Type == BLACKBOARD_VALUE_STATE::INT)
	{
		auto iter = find_if(m_mapInt.begin(), m_mapInt.end(), CTag_Finder(pName));

		if (iter == m_mapInt.end())
		{
			m_mapInt.emplace(MAP_INT::value_type(pName, *(_int*)pData));
			eState = BLACKBOARD_OUTPUT_STATE::UPDATE;
		}
		else
		{
			if (iter->second != *(_int*)pData)
			{
				iter->second = *(_int*)pData;
				eState = BLACKBOARD_OUTPUT_STATE::UPDATE;
			}
		}
	}

	else if (eValue_Type == BLACKBOARD_VALUE_STATE::VEC3)
	{
		auto iter = find_if(m_mapVec3.begin(), m_mapVec3.end(), CTag_Finder(pName));

		if (iter == m_mapVec3.end())
		{
			m_mapVec3.emplace(MAP_VEC3::value_type(pName, *(_v3*)pData));
			eState = BLACKBOARD_OUTPUT_STATE::UPDATE;
		}
		else
		{
			if (iter->second != *(_v3*)pData)
			{
				iter->second = *(_v3*)pData;
				eState = BLACKBOARD_OUTPUT_STATE::UPDATE;
			}
		}
	}

	return eState;
}

template<class T>
T CBlackBoard::Get_Value(const _tchar * pName, BLACKBOARD_VALUE_STATE eValue_Type) const
{
	switch (eValue_Type)
	{
	case BLACKBOARD_VALUE_STATE::BOOL:
		return T(*Find_Value_In_mapBool(pName));
	case BLACKBOARD_VALUE_STATE::FLOAT:
		return T(*Find_Value_In_mapFloat(pName));
	case BLACKBOARD_VALUE_STATE::INT:
		return T(*Find_Value_In_mapInt(pName));
	case BLACKBOARD_VALUE_STATE::VEC3:
		return T(*Find_Value_In_mapVec3(pName));
	default:
		break;
	}

	return T(NULL);
}

const _bool* CBlackBoard::Find_Value_In_mapBool(const _tchar * pName)
{
	auto iter = find_if(m_mapBool.begin(), m_mapBool.end(), CTag_Finder(pName));

	if (iter == m_mapBool.end())
		return nullptr;

	return &iter->second;
}

const _int * CBlackBoard::Find_Value_In_mapInt(const _tchar * pName)
{
	auto iter = find_if(m_mapInt.begin(), m_mapInt.end(), CTag_Finder(pName));

	if (iter == m_mapInt.end())
		return nullptr;

	return &iter->second;
}

const _float* CBlackBoard::Find_Value_In_mapFloat(const _tchar * pName)
{
	auto iter = find_if(m_mapFloat.begin(), m_mapFloat.end(), CTag_Finder(pName));

	if (iter == m_mapFloat.end())
		return nullptr;

	return &iter->second;
}

const _v3* CBlackBoard::Find_Value_In_mapVec3(const _tchar * pName)
{
	auto iter = find_if(m_mapVec3.begin(), m_mapVec3.end(), CTag_Finder(pName));

	if (iter == m_mapVec3.end())
		return nullptr;

	return &iter->second;
}

HRESULT CBlackBoard::Ready_BlackBoard()
{
	return S_OK;
}

CBlackBoard * CBlackBoard::Create()
{
	CBlackBoard* pInstance = new CBlackBoard;

	if (FAILED(pInstance->Ready_BlackBoard()))
		Safe_Release(pInstance);

	return pInstance;
}

CBlackBoard * CBlackBoard::Clone()
{
	return new CBlackBoard(*this);
}

void CBlackBoard::Free()
{
}

