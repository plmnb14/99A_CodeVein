#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CFrustum_Culling : public CBase
{
private:
	explicit	CFrustum_Culling();
	virtual		~CFrustum_Culling();

public:
	//_bool		Check_InFrustumObj(const _v3* _pPos, const _float& fRadius , CQuadTree* pQuadTree = nullptr);
	_bool		Check_InFrustum(const _v3* _Pos, const _float fRadius = 0.f);

public:
	_bool		Check_InFrustum_Advenced(const _v3* _Pos, const _float fRadius = 0.f);

private:
	_v3			m_vPoint[8] = {};
	D3DXPLANE	m_Plane[6] = {};

private:
	HRESULT		Ready_Component();
	HRESULT		Ready_Frustum(void);

public:
	static	CFrustum_Culling*		Create();

public:
	virtual void Free();

};

END