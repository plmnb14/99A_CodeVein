#pragma once

#include "Light.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)
private:
	explicit CLight_Manager();
	virtual ~CLight_Manager() = default;
public:
	const D3DLIGHT9* Get_LightDesc(_uint iIndex);
public:
	HRESULT Add_Light(LPDIRECT3DDEVICE9 pGraphic_Device, D3DLIGHT9 LightDesc);
	HRESULT Render_Light(CShader* pShader);
public:
	list<CLight*>			m_LightList;
	typedef list<CLight*>	LIGHTLIST;
public:
	virtual void Free();
};

END