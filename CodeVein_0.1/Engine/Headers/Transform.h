#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform : public CComponent
{
private:
	explicit	CTransform(_Device _pGraphicDev);
	explicit	CTransform(const CTransform& rhs);
	virtual		~CTransform();

public:
	virtual HRESULT Ready_Component();

public:
	const TRANS_INFO			Get_Info()	const;
	const _v3&					Get_Axis(const AXIS_TYPE _eAxis)  const;
	const _v3&					Get_Pos()	const;
	const _v3&					Get_Size()	const;
	const _v3&					Get_At()	const;
	_mat&						Get_WorldMat();

public:
	const _v3&		Get_Angle()					const;
	const float&	Get_Angle(AXIS_TYPE _eAxis) const;

public:
	void Set_Pos(const _v3& _vPos);
	void Set_Pos_Axis(const _v3& _vPos, const AXIS_TYPE _Axis);
	void Set_Pos_Axis(const _float _fAxis, const AXIS_TYPE _Axis);
	void Set_Axis(const _v3& _vValue, const AXIS_TYPE _eAxis);
	void Set_Scale(const _v3& _vScale);
	void Set_Scale_Axis(const _v3& _vScale, const AXIS_TYPE _Axis);
	void Set_Scale_Axis(const _float _fScale, const AXIS_TYPE _Axis);
	void Set_WorldMat(const _mat& _matWorld);
	void Set_At(const _v3& _At);
	void Set_Info(const TRANS_INFO _tTransInfo) { memcpy(&m_tInfo, &_tTransInfo, sizeof(TRANS_INFO)); }

public:
	void Set_Angle(const _v3& vAngle);
	void Set_Angle(const AXIS_TYPE _eAxis, const _float& fAngle);

public:
	void Add_Pos(const _float _fSpeed);
	void Add_Pos(const _float _fSpeed, const _v3& _vDir);
	void Add_Pos(const _v3 & _vPos);
	void Add_Pos_Divide(const _float _fSpeed, const AXIS_TYPE _eAxis);

public:
	void Add_At(const _float _fSpeed);
	void Add_At(const _float _fSpeed, const _v3& _vDir);
	void Add_At(const _v3& _At);

public:
	void Add_Angle(const AXIS_TYPE _eAxis, const _float fSpeed);

public:
	// 해당 Vector 방향으로 이동
	const _v3    Chase_Target(const _v3 * pTargetPos);

	// 해당 Vector 방향으로의 각도를 계산
	const _float Chase_Target_Angle(const _v3 * pTargetPos);

public:
	const _mat * CTransform::Compute_LookAtTarget(const _v3 * _pTargetDir);

public:
	void Calc_ParentMat(const _mat* _matParent);

public:
	virtual _int Update_Component();

public:
	static	CTransform*		Create(_Device _pGraphicDev);
	virtual CComponent*		Clone_Component(void* pArg);

public:
	virtual void Free();

private:
	TRANS_INFO	m_tInfo;

	_mat    m_matRotAxis;
	_mat	m_matScale;				// 크기 행렬
	_mat	m_matRot[AXIS_END];		// 회전 행렬
	_mat	m_matTrans;				// 크기 행렬

};

END