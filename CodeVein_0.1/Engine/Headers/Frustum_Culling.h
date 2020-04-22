#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CFrustum_Culling : public CComponent
{
private:
	explicit	CFrustum_Culling(_Device _pGraphicDev);
	explicit	CFrustum_Culling(const CFrustum_Culling& rhs);
	virtual		~CFrustum_Culling();

public:
	_bool		Check_InFrustumObj(const _v3* _pPos, const _float& fRadius);
	_bool		Check_InFrustum(const _v3* _Pos, const _float fRadius = 0.f);

private:
	_v3			m_vPoint[8];
	D3DXPLANE	m_Plane[6];

private:
	HRESULT		Ready_Component();
	HRESULT		Ready_Frustum(void);

public:
	static	CFrustum_Culling*		Create(_Device _pGraphicDev);
	virtual CComponent*		Clone_Component(void* pArg);

public:
	virtual void Free();

};

END