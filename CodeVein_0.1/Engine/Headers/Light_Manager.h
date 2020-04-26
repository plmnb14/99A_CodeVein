#pragma once

#include "Light.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)

public:
	typedef enum Light_Type
	{
		Static_Light, Dynamic_Light, Light_End
	}L_TYPE;

private:
	explicit CLight_Manager();
	virtual ~CLight_Manager() = default;
public:
	const NEW_LIGHT* Get_LightDesc(_uint iIndex);
	const _mat		 Get_LightViewProj(_uint iIndex);

public:
	void Set_Pos(_uint iIndex, _v3 vPos);

public:
	HRESULT Add_Light(LPDIRECT3DDEVICE9 pGraphic_Device, NEW_LIGHT LightDesc , L_TYPE _eLightType);
	HRESULT Render_Light(CShader* pShader);
	void	Update_Light();

public:
	HRESULT Clear_LightList();
	HRESULT Clear_LightList(L_TYPE _eLightType);

public:
	typedef list<CLight*>	LIGHTLIST;
	LIGHTLIST		m_LightList[Light_End];

public:
	virtual void Free();
};

END