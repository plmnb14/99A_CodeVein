#include "..\Headers\Light_Manager.h"

IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
{

}

const D3DLIGHT9 * CLight_Manager::Get_LightDesc(_uint iIndex)
{
	if (m_LightList.size() <= 0)
		return nullptr;

	auto	iter = m_LightList.begin();	
	
	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_LightDesc();
	
}

const _mat CLight_Manager::Get_LightViewProj(_uint iIndex)
{
	if (m_LightList.size() <= 0)
		return _mat();

	Update_Light();

	auto	iter = m_LightList.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_LightViewProj();
}

void CLight_Manager::Set_Pos(_uint iIndex, _v3 vPos)
{
	if (m_LightList.size() <= 0)
		return;

	auto	iter = m_LightList.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	(*iter)->Set_Pos(vPos);
}

HRESULT CLight_Manager::Add_Light(LPDIRECT3DDEVICE9 pGraphic_Device, D3DLIGHT9 LightDesc)
{
	CLight*		pLight = CLight::Create(pGraphic_Device, LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_LightList.push_back(pLight);

	return NOERROR;
}

HRESULT CLight_Manager::Render_Light(CShader* pShader)
{
	for (auto& pLight : m_LightList)
	{
		if (nullptr != pLight)
			pLight->Render_Light(pShader);
	}

	return NOERROR;
}

void CLight_Manager::Update_Light()
{
	for (auto& pLight : m_LightList)
		if (nullptr != pLight)
			pLight->Update_Light();
}

HRESULT CLight_Manager::Clear_LightList()
{
	for (auto& pLight : m_LightList)
		Safe_Release(pLight);

	m_LightList.clear();

	return S_OK;
}

void CLight_Manager::Free()
{
	for (auto& pLight : m_LightList)
		Safe_Release(pLight);

	m_LightList.clear();
}
