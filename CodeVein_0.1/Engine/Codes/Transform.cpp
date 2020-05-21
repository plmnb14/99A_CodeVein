#include "Transform.h"

CTransform::CTransform(_Device _pGraphicDev)
	:CComponent(_pGraphicDev)
{
	ZeroMemory(&m_tInfo, sizeof(TRANS_INFO));

	D3DXMatrixIdentity(&m_tInfo.matWorld);

	m_tInfo.vAxisDir[AXIS_X] = WORLD_RIGHT;
	m_tInfo.vAxisDir[AXIS_Y] = WORLD_UP;
	m_tInfo.vAxisDir[AXIS_Z] = WORLD_LOOK;

	m_tInfo.vAt = { 0.f, 0.f, 1.f };

	ZeroMemory(&m_matRotAxis, sizeof(_mat));
	ZeroMemory(&m_matScale, sizeof(_mat));
	ZeroMemory(&m_matRot[0], sizeof(_mat) * AXIS_END);
	ZeroMemory(&m_matTrans, sizeof(_mat));
}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
{
	memcpy(&m_tInfo, &rhs.m_tInfo, sizeof(TRANS_INFO));

	ZeroMemory(&m_matRotAxis, sizeof(_mat));
	ZeroMemory(&m_matScale, sizeof(_mat));
	ZeroMemory(&m_matRot[0], sizeof(_mat) * AXIS_END);
	ZeroMemory(&m_matTrans, sizeof(_mat));
}

CTransform::~CTransform()
{
}

HRESULT CTransform::Ready_Component()
{
	D3DXMatrixIdentity(&m_tInfo.matWorld);

	// Matrix Setup
	memcpy(m_tInfo.vAxisDir[AXIS_X], &m_tInfo.matWorld.m[0][0], sizeof(_v3));
	memcpy(m_tInfo.vAxisDir[AXIS_Y], &m_tInfo.matWorld.m[1][0], sizeof(_v3));
	memcpy(m_tInfo.vAxisDir[AXIS_Z], &m_tInfo.matWorld.m[2][0], sizeof(_v3));
	memcpy(m_tInfo.vPos, &m_tInfo.matWorld.m[3][0], sizeof(_v3));

	return S_OK;
}

const TRANS_INFO CTransform::Get_Info() const
{
	return m_tInfo;
}

const _v3 & CTransform::Get_Axis(AXIS_TYPE _eAxis) const
{
	return m_tInfo.vAxisDir[_eAxis];
}

const _v3& CTransform::Get_Pos() const
{
	return m_tInfo.vPos;
}

const _v3& CTransform::Get_Size() const
{
	return m_tInfo.vScale;
}

const _v3 & CTransform::Get_At() const
{
	return m_tInfo.vAt;
}

_mat& CTransform::Get_WorldMat()
{
	return m_tInfo.matWorld;
}

const _v3 & CTransform::Get_Angle() const
{
	return m_tInfo.vAngle;
}

const float& CTransform::Get_Angle(AXIS_TYPE _eAxis) const
{
	return m_tInfo.vAngle[_eAxis];
}

void CTransform::Set_Pos(const _v3& vPos)
{
	m_tInfo.vPos = vPos;
}

void CTransform::Set_Pos_Axis(const _v3 & _vPos, const AXIS_TYPE _Axis)
{
	m_tInfo.vPos[_Axis] = _vPos[_Axis];
}

void CTransform::Set_Pos_Axis(const _float _fAxis, const AXIS_TYPE _Axis)
{
	m_tInfo.vPos[_Axis] = _fAxis;
}

void CTransform::Set_Axis(const _v3 & _vValue, const AXIS_TYPE _eAxis)
{
}

void CTransform::Set_Scale(const _v3& vScale)
{
	m_tInfo.vScale = vScale;
}

void CTransform::Set_Scale_Axis(const _v3 & _vScale, const AXIS_TYPE _Axis)
{
	m_tInfo.vScale[_Axis] = _vScale[_Axis];
}

void CTransform::Set_Scale_Axis(const _float _fScale, const AXIS_TYPE _Axis)
{
	m_tInfo.vScale[_Axis] = _fScale;
}

void CTransform::Set_WorldMat(const _mat& matWorld)
{
	m_tInfo.matWorld = matWorld;
}

void CTransform::Set_At(const _v3 & _At)
{
	m_tInfo.vAt = _At;
}

// xyz 축 Angle 설정
void CTransform::Set_Angle(const _v3 & vAngle)
{
	m_tInfo.vAngle = vAngle;
}

// 특정 축으로 Angle 설정
void CTransform::Set_Angle(AXIS_TYPE eAxis, const  _float& fAngle)
{
	m_tInfo.vAngle[eAxis] = fAngle;
}

// speed 값을 넘겨주면 Look 방향으로 이동
void CTransform::Add_Pos(const _float _fSpeed)
{
	D3DXVec3Normalize(&m_tInfo.vAxisDir[AXIS_Z], &m_tInfo.vAxisDir[AXIS_Z]);
	m_tInfo.vPos += m_tInfo.vAxisDir[AXIS_Z] * _fSpeed;
}

// speed 값과 방향을 넘겨주면 해당 방향으로 이동
void CTransform::Add_Pos(const _float _fSpeed, const _v3& _vDir)
{
	m_tInfo.vPos += _vDir * _fSpeed;
}

// Vector 크기만큼 이동
void CTransform::Add_Pos(const _v3& _vPos)
{
	m_tInfo.vPos += _vPos;
}

// 특정 축으로만 
void CTransform::Add_Pos_Divide(const _float _fSpeed, AXIS_TYPE _eAxis)
{
	m_tInfo.vPos[_eAxis] += _fSpeed;
}

void CTransform::Add_At(const _float _fSpeed)
{
	D3DXVec3Normalize(&m_tInfo.vAxisDir[AXIS_Z], &m_tInfo.vAxisDir[AXIS_Z]);
	m_tInfo.vAt += m_tInfo.vAxisDir[AXIS_Z] * _fSpeed;
}

void CTransform::Add_At(const _float _fSpeed, const _v3 & _vDir)
{
	m_tInfo.vAt += _vDir * _fSpeed;
}

void CTransform::Add_At(const _v3 & _At)
{
	D3DXVec3Normalize(&m_tInfo.vAxisDir[AXIS_Z], &m_tInfo.vAxisDir[AXIS_Z]);
	m_tInfo.vAt += _At;
}

void CTransform::Add_Angle(const AXIS_TYPE eAxis, const _float _fRadian)
{
	m_tInfo.vAngle[eAxis] += D3DXToRadian(_fRadian);
}

void CTransform::Calc_ParentMat(const _mat* _matParent)
{
	m_tInfo.matWorld *= *_matParent;
}

const _v3 CTransform::Chase_Target(const _v3 * pTargetPos)
{
	_v3	vDirection = *pTargetPos - m_tInfo.vPos;

	//Look 구해주고
	D3DXVec3Normalize(&vDirection, &vDirection);

	// Up은 있으니까 Right 를 구하자
	_v3	vRight = *D3DXVec3Cross(&vRight, &m_tInfo.vAxisDir[AXIS_Y], &vDirection);
	V3_NORMAL_SELF(&vRight);

	_mat matRotY;

	_v3	vLook;
	D3DXVec3Normalize(&vLook, &m_tInfo.vAxisDir[AXIS_Z]);

	_float	fDot = acosf(D3DXVec3Dot(&_v3{ 0,0,1 }, &vDirection));

	if (vRight.z > 0)
		fDot *= -1.f;

	//D3DXMatrixRotationY(&matRotY, fDot);
	//
	////_mat		matRot = *Compute_LookAtTarget(&vDirection);
	//_mat		matScale, matTrans;
	//
	//D3DXMatrixScaling(&matScale, m_tInfo.vScale.x, m_tInfo.vScale.y, m_tInfo.vScale.z);
	//D3DXMatrixTranslation(&matTrans, m_tInfo.vPos.x, m_tInfo.vPos.y, m_tInfo.vPos.z);
	//
	//m_tInfo.matWorld = matScale * matRotY * matTrans;

	Set_Angle({ 0, fDot, 0 });

	return vDirection;
}

const _float CTransform::Chase_Target_Angle(const _v3 * pTargetPos)
{
	_v3	vDirection = *pTargetPos - m_tInfo.vPos;

	//Look 구해주고
	D3DXVec3Normalize(&vDirection, &vDirection);

	// Up은 있으니까 Right 를 구하자
	_v3	vRight = *D3DXVec3Cross(&vRight, &m_tInfo.vAxisDir[AXIS_Y], &vDirection);
	V3_NORMAL_SELF(&vRight);

	_v3	vLook;
	D3DXVec3Normalize(&vLook, &m_tInfo.vAxisDir[AXIS_Z]);

	_float	fDot = acosf(D3DXVec3Dot(&WORLD_LOOK, &vDirection));

	if (vRight.z > 0)
		fDot *= -1.f;

	return fDot;
}

const _float CTransform::Calc_HitTarget_Angle(const _v3 pHitTargetPos)
{
	_v3 vMyPos = m_tInfo.vPos;
	_v3 vTargetPos = pHitTargetPos;
	_v3 vMyLook = m_tInfo.vAxisDir[AXIS_Z];

	V3_NORMAL_SELF(&vMyLook);

	// 내 위치로 부터 대상 까지의 방향 벡터
	_v3 vDirection = vTargetPos - vMyPos;
	V3_NORMAL_SELF(&vDirection);

	_float fDotRadian = acosf(D3DXVec3Dot(&vMyLook, &vDirection));

	_v3 vVertical;
	D3DXVec3Cross(&vVertical, &vMyLook, &vDirection);
	V3_NORMAL_SELF(&vVertical);

	if (vVertical.y < 0)
		fDotRadian *= -1.f;
	//_v3	vRight = *D3DXVec3Cross(&vRight, &WORLD_UP, &vDirection);
	//V3_NORMAL_SELF(&vRight);

	//if (vRight.z > 0)
	//	fDotRadian *= -1.f;

	return fDotRadian;
}

const _mat * CTransform::Compute_LookAtTarget(const _v3 * _pTargetDir)
{
	_v3	vDirection = *_pTargetDir;

	//Look 구해주고
	D3DXVec3Normalize(&vDirection, &vDirection);

	// Up은 있으니까 Right 를 구하자
	_v3	vRight = *D3DXVec3Cross(&vRight, &m_tInfo.vAxisDir[AXIS_Y], &vDirection);
	V3_NORMAL_SELF(&vRight);

	_v3	vLook;
	D3DXVec3Normalize(&vLook, &m_tInfo.vAxisDir[AXIS_Z]);

	_float	fDot = acosf(D3DXVec3Dot(&vLook, &vDirection));

	if (vRight.z > 0)
		fDot *= -1.f;

	D3DXMatrixRotationY(&m_matRotAxis, fDot);

	return &m_matRotAxis;
}

_int CTransform::Update_Component()
{
	LOOP(3)
	{
		if (m_tInfo.vAngle[i] > 360.f)
			m_tInfo.vAngle[i] -= 360.f;

		else if (m_tInfo.vAngle[i] < -360.f)
			m_tInfo.vAngle[i] += 360.f;
	}

	D3DXMatrixIdentity(&m_tInfo.matWorld);

	memcpy(m_tInfo.vAxisDir[AXIS_X], &m_tInfo.matWorld.m[0][0], sizeof(_v3));
	memcpy(m_tInfo.vAxisDir[AXIS_Y], &m_tInfo.matWorld.m[1][0], sizeof(_v3));
	memcpy(m_tInfo.vAxisDir[AXIS_Z], &m_tInfo.matWorld.m[2][0], sizeof(_v3));

	// Matrix Setup
	D3DXVec3Normalize(&m_tInfo.vAxisDir[AXIS_X], &m_tInfo.vAxisDir[AXIS_X]);
	m_tInfo.vAxisDir[AXIS_X] *= *(((_float*)&m_tInfo.vScale) + 0);

	D3DXVec3Normalize(&m_tInfo.vAxisDir[AXIS_Y], &m_tInfo.vAxisDir[AXIS_Y]);
	m_tInfo.vAxisDir[AXIS_Y] *= *(((_float*)&m_tInfo.vScale) + 1);

	D3DXVec3Normalize(&m_tInfo.vAxisDir[AXIS_Z], &m_tInfo.vAxisDir[AXIS_Z]);
	m_tInfo.vAxisDir[AXIS_Z] *= *(((_float*)&m_tInfo.vScale) + 2);


	// Rotation
	_mat		matRot[3];
	D3DXMatrixRotationX(&matRot[AXIS_X], m_tInfo.vAngle.x);
	D3DXMatrixRotationY(&matRot[AXIS_Y], m_tInfo.vAngle.y);
	D3DXMatrixRotationZ(&matRot[AXIS_Z], m_tInfo.vAngle.z);

	// Rotation Done
	D3DXVec3TransformNormal(&m_tInfo.vAxisDir[AXIS_X], &m_tInfo.vAxisDir[AXIS_X], &matRot[AXIS_X]);
	D3DXVec3TransformNormal(&m_tInfo.vAxisDir[AXIS_X], &m_tInfo.vAxisDir[AXIS_X], &matRot[AXIS_Y]);
	D3DXVec3TransformNormal(&m_tInfo.vAxisDir[AXIS_X], &m_tInfo.vAxisDir[AXIS_X], &matRot[AXIS_Z]);

	D3DXVec3TransformNormal(&m_tInfo.vAxisDir[AXIS_Y], &m_tInfo.vAxisDir[AXIS_Y], &matRot[AXIS_X]);
	D3DXVec3TransformNormal(&m_tInfo.vAxisDir[AXIS_Y], &m_tInfo.vAxisDir[AXIS_Y], &matRot[AXIS_Y]);
	D3DXVec3TransformNormal(&m_tInfo.vAxisDir[AXIS_Y], &m_tInfo.vAxisDir[AXIS_Y], &matRot[AXIS_Z]);

	D3DXVec3TransformNormal(&m_tInfo.vAxisDir[AXIS_Z], &m_tInfo.vAxisDir[AXIS_Z], &matRot[AXIS_X]);
	D3DXVec3TransformNormal(&m_tInfo.vAxisDir[AXIS_Z], &m_tInfo.vAxisDir[AXIS_Z], &matRot[AXIS_Y]);
	D3DXVec3TransformNormal(&m_tInfo.vAxisDir[AXIS_Z], &m_tInfo.vAxisDir[AXIS_Z], &matRot[AXIS_Z]);

	// Translation
	memcpy(&m_tInfo.matWorld.m[0][0], &m_tInfo.vAxisDir[AXIS_X], sizeof(_v3));
	memcpy(&m_tInfo.matWorld.m[1][0], &m_tInfo.vAxisDir[AXIS_Y], sizeof(_v3));
	memcpy(&m_tInfo.matWorld.m[2][0], &m_tInfo.vAxisDir[AXIS_Z], sizeof(_v3));
	memcpy(&m_tInfo.matWorld.m[3][0], &m_tInfo.vPos, sizeof(_v3));

	return NO_EVENT;
}

CTransform* CTransform::Create(_Device _pGraphicDev)
{
	CTransform* pComponent = new CTransform(_pGraphicDev);

	if (FAILED(pComponent->Ready_Component()))
	{
		Safe_Release(pComponent);
		return nullptr;
	}

	return pComponent;
}

CComponent * CTransform::Clone_Component(void* pArg)
{
	return new CTransform(*this);
}

void CTransform::Free()
{
	CComponent::Free();
}
