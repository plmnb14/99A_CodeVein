#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider : public CComponent
{
public:
	enum OBB_DIR { DIR_X, DIR_Y, DIR_Z };

private:
	explicit	CCollider(_Device _pGraphicDev);
	explicit	CCollider(const CCollider& rhs);
	virtual		~CCollider();

public:
	virtual HRESULT Ready_Component();

public:
	virtual void Update(_v3 _vPos);

public:
	void SetUp_Radius_Auto(const _v3 _vSize, const _v3 _vMin, const _v3 _vMax);
	void SetUp_Box();

public:
	void Set_OBBDir(_v3 _OBBDir, OBB_DIR _Direction);
	void Set_Radius(_v3 _Radius);
	void Set_Length(_v3 _Length);
	void Set_CenterPos(_v3 _CenterPos);
	void Set_UnderPos();
	void Set_IsCollision(_bool _bIsCollision);
	void Set_GizmoPos();

public:
	void Set_Dynamic(_bool _Dynamic);
	void Set_Trigger(_bool _Trigger);
	void Set_IsCol(_bool _Collision);
	inline void Set_Enabled(_bool _Enabled) { m_tColInfo.bIsEnabled = _Enabled; }

public:
	void Set_ColInfo(COL_INFO _Info) { m_tColInfo = _Info; }
	void Set_Type(COL_TYPE _Type) { m_eColType = _Type; }

public:
	const _v3	Get_CenterPos()	const { return m_tColInfo.vCenterPos; }
	const _v3	Get_UnderPos()	const { return m_tColInfo.vUnderPos; }
	const _v3	Get_Length()	const { return m_tColInfo.vLength; }
	const _v3	Get_Radius()	const { return m_tColInfo.vRadius; }
	const _v3*	Get_GizmoPos()	const { return m_tColInfo.pGizmoPos; }
	const _v3*	Get_OBB_Dir()	const { return m_tColInfo.vOBB_Dir; }

	const _bool Get_IsCol()		const { return m_tColInfo.bIsCol; }
	const _bool Get_Enabled()	const { return m_tColInfo.bIsEnabled; }

	COL_INFO*	Get_ColInfo() { return &m_tColInfo; }
	COL_TYPE	Get_ColType() { return m_eColType; }

public:
	_bool Check_RayCollision(CCollider* _rDstCol, RAY _rSrcRay, _float* _vCrossLength);



	// 캡슐 충돌 체크용
private:
	_bool Check_Sphere(COL_INFO _rDstColInfo, RAY _rSrcRay, _v3 _vCapsuleVertexPos);

	// 도형과 Ray
public:
	_bool Check_AABB(CCollider* _rDstCol, RAY _rSrcRay);
	_bool Check_Sphere(CCollider* _rDstCol, RAY _rSrcRay);
	_bool Check_Capsule(CCollider* _rDstCol, RAY _rSrcRay);

	// 도형과 도형
public:
	_bool Check_OBB(CCollider* _rSrc);
	_bool Check_AABB(CCollider* _rSrc);
	_bool Check_Sphere(CCollider * _rSrc, _float * _fLegnthOut, _v3 * _vDirOut);
	_bool Check_Sphere(CCollider* _rSrc);
	_bool Check_Capsule(CCollider* _rSrc);

public:
	_v3	Calc_Length(COL_INFO * _rDst, COL_INFO * _rSrc, _bool _dynamic = false);

public:
	static CCollider* Create(_Device _pGraphicDev);

public:
	virtual void			Free();
	virtual CComponent*		Clone_Component(void* _pArg);

private:
	COL_INFO		m_tColInfo;
	COL_TYPE		m_eColType;
};

END