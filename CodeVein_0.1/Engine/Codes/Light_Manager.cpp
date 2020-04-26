#include "..\Headers\Light_Manager.h"

IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
{\
}

const NEW_LIGHT * CLight_Manager::Get_LightDesc(_uint iIndex)
{
	if (m_LightList[Static_Light].size() <= 0)
		return nullptr;

	auto	iter = m_LightList[Static_Light].begin();
	
	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_LightDesc();
	
}

const _mat CLight_Manager::Get_LightViewProj(_uint iIndex)
{
	if (m_LightList[Static_Light].size() <= 0)
		return _mat();

	Update_Light();

	auto	iter = m_LightList[Static_Light].begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_LightViewProj();
}

void CLight_Manager::Set_Pos(_uint iIndex, _v3 vPos)
{
	if (m_LightList[Static_Light].size() <= 0)
		return;

	auto	iter = m_LightList[Static_Light].begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	(*iter)->Set_Pos(vPos);
}

HRESULT CLight_Manager::Add_Light(LPDIRECT3DDEVICE9 pGraphic_Device, NEW_LIGHT LightDesc, L_TYPE _eLightType)
{
	CLight*		pLight = CLight::Create(pGraphic_Device, LightDesc);

	if (nullptr == pLight)
		return E_FAIL;

	m_LightList[_eLightType].push_back(pLight);

	return NOERROR;
}

HRESULT CLight_Manager::Render_Light(CShader* pShader)
{
	LOOP(2)
	{
		for (auto& pLight : m_LightList[i])
		{
			if (nullptr != pLight)
			{
				if (false == pLight->Get_Dead())
					pLight->Render_Light(pShader);

				else
				{
					Safe_Release(pLight);
				}
			}
		}
	}

	return NOERROR;
}

void CLight_Manager::Update_Light()
{
	LOOP(2)
	{
		for (auto& pLight : m_LightList[i])
		{
			if (nullptr != pLight)
			{
				if (false == pLight->Get_Dead())
					pLight->Update_Light();

				else
				{
					Safe_Release(pLight);
				}
			}
		}
	}
}

HRESULT CLight_Manager::Clear_LightList()
{
	LOOP(2)
	{
		for (auto& pLight : m_LightList[i])
		{
			Safe_Release(pLight);
		}

		m_LightList[i].clear();
	}

	return S_OK;
}

HRESULT CLight_Manager::Clear_LightList(L_TYPE _eLightType)
{
	for (auto& pLight : m_LightList[_eLightType])
	{
		Safe_Release(pLight);
	}

	m_LightList[_eLightType].clear();

	return S_OK;
}

void CLight_Manager::Free()
{
	Clear_LightList();
}
