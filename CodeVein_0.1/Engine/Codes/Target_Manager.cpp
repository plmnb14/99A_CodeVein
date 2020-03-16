#include "..\Headers\Target_Manager.h"

IMPLEMENT_SINGLETON(CTarget_Manager)

CTarget_Manager::CTarget_Manager()
{

}

LPDIRECT3DTEXTURE9 CTarget_Manager::Get_Texture(const _tchar * pTargetTag)
{
	CTarget*	pRenderTarget = Find_RenderTarget(pTargetTag);

	if (nullptr == pRenderTarget)
		return nullptr;

	return pRenderTarget->Get_Texture();
}

HRESULT CTarget_Manager::Add_Render_Target(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar * pTargetTag, _uint iWidth, _uint iHeight, D3DFORMAT eFormat, D3DXCOLOR ClearColor)
{
	if (nullptr != Find_RenderTarget(pTargetTag))
		return E_FAIL;

	CTarget*		pTarget = CTarget::Create(pGraphic_Device, iWidth, iHeight, eFormat, ClearColor);
	if (nullptr == pTarget)
		return E_FAIL;

	m_Targets.insert(TARGETS::value_type(pTargetTag, pTarget));
	
	return NOERROR;
}

HRESULT CTarget_Manager::Add_MRT(const _tchar * pMRTTag, const _tchar * pTargetTag)
{
	CTarget* pRenderTarget = Find_RenderTarget(pTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	list<CTarget*>*	pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList) // 처음 생성되는 MRT이다. 
	{
		list<CTarget*>		MRTList;

		MRTList.push_back(pRenderTarget);

		m_MRTs.insert(MRTS::value_type(pMRTTag, MRTList));
	}

	else
		pMRTList->push_back(pRenderTarget);

	return NOERROR;
}

HRESULT CTarget_Manager::Begin_MRT(const _tchar * pMRTTag)
{
	list<CTarget*>*	pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	_uint		iIndex = 0;

	for (auto& pTarget : *pMRTList)
	{
		if (nullptr != pTarget)
		{
			if (FAILED(pTarget->Clear_Target()))
				return E_FAIL;
		}
	}

	for (auto& pTarget : *pMRTList)
	{
		if (nullptr != pTarget)
		{
			if (FAILED(pTarget->SetUp_OnGraphicDev(iIndex++)))
				return E_FAIL;
		}
	}

	return NOERROR;
}

HRESULT CTarget_Manager::End_MRT(const _tchar * pMRTTag)
{
	list<CTarget*>*	pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	_uint		iIndex = 0;

	for (auto& pTarget : *pMRTList)
	{
		if (nullptr != pTarget)
		{
			if (FAILED(pTarget->Release_OnGraphicDev(iIndex++)))
				return E_FAIL;
		}
	}

	return NOERROR;
}

#ifdef _DEBUG

HRESULT CTarget_Manager::Ready_Debug_Buffer(const _tchar * pTargetTag, _float fStartX, _float fStartY, _float fSizeX, _float fSizeY)
{
	CTarget* pRenderTarget = Find_RenderTarget(pTargetTag);

	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Ready_Debug_Buffer(fStartX, fStartY, fSizeX, fSizeY);	
}

HRESULT CTarget_Manager::Render_Debug_Buffer(const _tchar * pMRTTag)
{
	list<CTarget*>*	pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	for (auto& pTarget : *pMRTList)
	{
		if (nullptr != pTarget)
			pTarget->Render_Debug_Buffer();
	}

	return NOERROR;
}

#endif

CTarget * CTarget_Manager::Find_RenderTarget(const _tchar * pTargetTag)
{
	auto	iter = find_if(m_Targets.begin(), m_Targets.end(), CTag_Finder(pTargetTag));

	if (iter == m_Targets.end())
		return nullptr;

	return iter->second;
}

list<CTarget*>* CTarget_Manager::Find_MRT(const _tchar * pMRTTag)
{
	auto	iter = find_if(m_MRTs.begin(), m_MRTs.end(), CTag_Finder(pMRTTag));

	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

void CTarget_Manager::Free()
{
	for (auto& Pair : m_Targets)
		Safe_Release(Pair.second);

	m_Targets.clear();
}
