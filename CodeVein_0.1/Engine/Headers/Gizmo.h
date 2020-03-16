#pragma once

#include "Engine_Defines.h"
#include "Base.h"
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CGizmo : public CBase
{
	DECLARE_SINGLETON(CGizmo)

public:
	enum GIZMO_TYPE
	{
		G_VERTEX, G_LINE, G_TRIANGLE, G_PLANE, G_CAPSULE, G_SPHERE, G_CUBE
	};

private:
	explicit CGizmo(void);
	virtual ~CGizmo(void);

public:
	void Ready_Gizmo(_Device _pGraphicDev);

public:
	void Draw_XYZ(_Device _pGraphicDev, _v3 _vPos, _v3 _vLook, _v3 _vRight, _v3 _vUp = WORLD_UP);

	void Draw_Vertex(_Device _pGraphicDev, _v3 _vVertex, _v3 _vSize = { 0.1f, 0.1f,0.1f });
	void Draw_Sphere(_Device _pGraphicDev, _v3 _vVertex, _float _fRadius = 1.f);

	void Draw_Triangle_Line(_Device _pGraphicDev, _v3 * _vVertex, _bool _bSelect = false , CELL_PARAM _eParam = NORMAL);
	void Draw_Triangle(_Device _pGraphicDev, VTX_COL * _vVertex);

	void Draw_AABB(_Device _pGraphicDev, const _v3* _vVertex ,_v3 _vPos, _v3 _vSize = _v3{1.f, 1.f, 1.f});
	void Draw_OBB(_Device _pGraphicDev, const _v3* _vVertex, _v3 vRotate, _v3 _vPos, _v3 _vSize = _v3{ 1.f, 1.f, 1.f });

	void Draw_Capsule(_Device _pGraphicDev, const _v3 _vVertex, _float _fRadius, _float _fMaxHeight);


public:
	void Set_EnableGizmo();

public:
	virtual void Free(void);

private:
	//CVIBuffer*		m_pShpereBuffer;
	_Device			m_pGraphicDev;

private:
	_bool	m_bEnableGizmo;

};

END
