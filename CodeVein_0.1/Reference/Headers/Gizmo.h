#pragma once

#include "Engine_Defines.h"
#include "Base.h"
#include "VIBuffer.h"

BEGIN(Engine)

class CShader;
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
	void Draw_XYZ(_v3 _vPos, _v3 _vLook, _v3 _vRight, _v3 _vUp = WORLD_UP);

	void Draw_Vertex(_v3 _vVertex, _v3 _vSize = { 0.1f, 0.1f,0.1f });
	void Draw_Sphere(_v3 _vVertex, _float _fRadius = 1.f);

	void Draw_Triangle_Line(_v3 * _vVertex, _bool _bSelect = false , CELL_PARAM _eParam = NORMAL);
	void Draw_Triangle(VTX_COL * _vVertex);

	void Draw_AABB(const _v3* _vVertex ,_v3 _vPos, _v3 _vSize = _v3{1.f, 1.f, 1.f});
	void Draw_OBB(const _v3* _vVertex, _v3 vRotate, _v3 _vPos, _v3 _vSize = _v3{ 1.f, 1.f, 1.f });

	void Draw_Capsule(const _v3 _vVertex, _float _fRadius, _float _fMaxHeight);

public:
	HRESULT Init_Shader(_mat _DefaultMat);

public:
	void Gizmo_Toggle();
	void Enable_GizmoCell();
	void Enable_GizmoCollider();

public:
	virtual void Free(void);

private:
	//CVIBuffer*		m_pShpereBuffer;
	_Device				m_pGraphicDev;
	CShader*			m_pGizmoShader;
	D3DXCOLOR			m_Color;

private:
	_bool	m_bEnableCell = false;
	_bool	m_bEnableCollider = false;
	_bool	m_bToogle = false;

};

END
