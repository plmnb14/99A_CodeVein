#include "Collider.h"

CCollider::CCollider(_Device _pGraphicDev)
	:CComponent(_pGraphicDev)
	,m_eColType(COL_AABB)
{
	ZeroMemory(&m_tColInfo, sizeof(COL_INFO));
}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)
{
	memcpy(&m_tColInfo, &rhs.m_tColInfo, sizeof(COL_INFO));
	memcpy(&m_eColType, &rhs.m_eColType, sizeof(COL_TYPE));
}

CCollider::~CCollider()
{
}

void CCollider::Update(_v3 _vPos)
{
	Set_CenterPos(_vPos);
	Set_UnderPos();
	SetUp_Box();
}

HRESULT CCollider::Ready_Component()
{
	return S_OK;
}

void CCollider::SetUp_Radius_Auto(const _v3 _vSize , const _v3 _vMin, const _v3 _vMax)
{
	LOOP(3)
	{
		m_tColInfo.vRadius[i] = fabs((_vMax[i] - _vMin[i]) * 0.5f * _vSize[i]);
	}
}

void CCollider::SetUp_Box()
{
	m_tColInfo.vMinPos = { m_tColInfo.vCenterPos - m_tColInfo.vRadius };
	m_tColInfo.vMaxPos = { m_tColInfo.vCenterPos + m_tColInfo.vRadius };
	Set_GizmoPos();
}

void CCollider::Set_OBBDir(_v3 _OBBDir, OBB_DIR _Direction)
{
	m_tColInfo.vOBB_Dir[_Direction] = _OBBDir;
}

void CCollider::Set_Radius(_v3 _Radius)
{
	m_tColInfo.vRadius = _Radius;
}

void CCollider::Set_Length(_v3 _Length)
{
	m_tColInfo.vLength = _Length;
}

void CCollider::Set_CenterPos(_v3 _CenterPos)
{
	m_tColInfo.vCenterPos = _CenterPos;
}

void CCollider::Set_UnderPos()
{
	m_tColInfo.vUnderPos = { m_tColInfo.vCenterPos.x ,
		m_tColInfo.vCenterPos.y - m_tColInfo.vRadius.y ,
		m_tColInfo.vCenterPos.z };
}

void CCollider::Set_IsCollision(_bool _bIsCollision)
{
	m_tColInfo.bIsCol = _bIsCollision;
}

void CCollider::Set_GizmoPos()
{
	if (m_eColType == COL_AABB)
	{
		m_tColInfo.pGizmoPos[0].x = -m_tColInfo.vRadius.x;
		m_tColInfo.pGizmoPos[0].y = m_tColInfo.vRadius.y;
		m_tColInfo.pGizmoPos[0].z = -m_tColInfo.vRadius.z;

		m_tColInfo.pGizmoPos[1].x = m_tColInfo.vRadius.x;
		m_tColInfo.pGizmoPos[1].y = m_tColInfo.vRadius.y;
		m_tColInfo.pGizmoPos[1].z = -m_tColInfo.vRadius.z;

		m_tColInfo.pGizmoPos[2].x = m_tColInfo.vRadius.x;
		m_tColInfo.pGizmoPos[2].y = -m_tColInfo.vRadius.y;
		m_tColInfo.pGizmoPos[2].z = -m_tColInfo.vRadius.z;

		m_tColInfo.pGizmoPos[3].x = -m_tColInfo.vRadius.x;
		m_tColInfo.pGizmoPos[3].y = -m_tColInfo.vRadius.y;
		m_tColInfo.pGizmoPos[3].z = -m_tColInfo.vRadius.z;

		m_tColInfo.pGizmoPos[4].x = -m_tColInfo.vRadius.x;
		m_tColInfo.pGizmoPos[4].y = -m_tColInfo.vRadius.y;
		m_tColInfo.pGizmoPos[4].z = m_tColInfo.vRadius.z;

		m_tColInfo.pGizmoPos[5].x = -m_tColInfo.vRadius.x;
		m_tColInfo.pGizmoPos[5].y = m_tColInfo.vRadius.y;
		m_tColInfo.pGizmoPos[5].z = m_tColInfo.vRadius.z;

		m_tColInfo.pGizmoPos[6].x = m_tColInfo.vRadius.x;
		m_tColInfo.pGizmoPos[6].y = m_tColInfo.vRadius.y;
		m_tColInfo.pGizmoPos[6].z = m_tColInfo.vRadius.z;

		m_tColInfo.pGizmoPos[7].x = m_tColInfo.vRadius.x;
		m_tColInfo.pGizmoPos[7].y = -m_tColInfo.vRadius.y;
		m_tColInfo.pGizmoPos[7].z = m_tColInfo.vRadius.z;

		LOOP(8)
		{
			m_tColInfo.pGizmoPos[i] * 0.2f;
		}
	}

	if (m_eColType == COL_OBB)
	{
		m_tColInfo.pGizmoPos[0].x = -m_tColInfo.vRadius.x;
		m_tColInfo.pGizmoPos[0].y = m_tColInfo.vRadius.y;
		m_tColInfo.pGizmoPos[0].z = -m_tColInfo.vRadius.z;

		m_tColInfo.pGizmoPos[1].x = m_tColInfo.vRadius.x;
		m_tColInfo.pGizmoPos[1].y = m_tColInfo.vRadius.y;
		m_tColInfo.pGizmoPos[1].z = -m_tColInfo.vRadius.z;

		m_tColInfo.pGizmoPos[2].x = m_tColInfo.vRadius.x;
		m_tColInfo.pGizmoPos[2].y = -m_tColInfo.vRadius.y;
		m_tColInfo.pGizmoPos[2].z = -m_tColInfo.vRadius.z;

		m_tColInfo.pGizmoPos[3].x = -m_tColInfo.vRadius.x;
		m_tColInfo.pGizmoPos[3].y = -m_tColInfo.vRadius.y;
		m_tColInfo.pGizmoPos[3].z = -m_tColInfo.vRadius.z;

		m_tColInfo.pGizmoPos[4].x = -m_tColInfo.vRadius.x;
		m_tColInfo.pGizmoPos[4].y = -m_tColInfo.vRadius.y;
		m_tColInfo.pGizmoPos[4].z = m_tColInfo.vRadius.z;

		m_tColInfo.pGizmoPos[5].x = -m_tColInfo.vRadius.x;
		m_tColInfo.pGizmoPos[5].y = m_tColInfo.vRadius.y;
		m_tColInfo.pGizmoPos[5].z = m_tColInfo.vRadius.z;

		m_tColInfo.pGizmoPos[6].x = m_tColInfo.vRadius.x;
		m_tColInfo.pGizmoPos[6].y = m_tColInfo.vRadius.y;
		m_tColInfo.pGizmoPos[6].z = m_tColInfo.vRadius.z;

		m_tColInfo.pGizmoPos[7].x = m_tColInfo.vRadius.x;
		m_tColInfo.pGizmoPos[7].y = -m_tColInfo.vRadius.y;
		m_tColInfo.pGizmoPos[7].z = m_tColInfo.vRadius.z;

		LOOP(8)
		{
			m_tColInfo.pGizmoPos[i] * 0.2f;
		}
	}
}

void CCollider::Set_Dynamic(_bool _Dynamic)
{
	m_tColInfo.bIsDynamic = _Dynamic;
}

void CCollider::Set_Trigger(_bool _Trigger)
{
	m_tColInfo.bIsTrigger = _Trigger;
}

void CCollider::Set_IsCol(_bool _Collision)
{
	m_tColInfo.bIsCol = _Collision;
}

_bool CCollider::Check_RayCollision(CCollider * _rDstCol, RAY _rSrcRay, _float*_vCrossLength)
{
	COL_INFO* rDstCol = _rDstCol->Get_ColInfo();

	_v3		  vRayOri = _rSrcRay.vRayPos;

	_v3		vRayConvertDir = {};
	_float	t[6] = {};

	vRayConvertDir.x = 1.0f / _rSrcRay.vRayDir.x;
	vRayConvertDir.y = 1.0f / _rSrcRay.vRayDir.y;
	vRayConvertDir.z = 1.0f / _rSrcRay.vRayDir.z;

	t[0] = (rDstCol->vMinPos.x - vRayOri.x) * vRayConvertDir.x;
	t[1] = (rDstCol->vMaxPos.x - vRayOri.x) * vRayConvertDir.x;
	t[2] = (rDstCol->vMinPos.y - vRayOri.y) * vRayConvertDir.y;
	t[3] = (rDstCol->vMaxPos.y - vRayOri.y) * vRayConvertDir.y;
	t[4] = (rDstCol->vMinPos.z - vRayOri.z) * vRayConvertDir.z;
	t[5] = (rDstCol->vMaxPos.z - vRayOri.z) * vRayConvertDir.z;

	float tMin = max(max(min(t[0], t[1]), min(t[2], t[3])), min(t[4], t[5]));
	float tMax = min(min(max(t[0], t[1]), max(t[2], t[3])), max(t[4], t[5]));

	if (tMax < 0)
	{
		*_vCrossLength = 0.f;
		return false;
	}

	if (tMin > tMax)
	{
		*_vCrossLength = 0.f;
		return false;
	}

	*_vCrossLength = tMin;
	//*_vCrossLength = D3DXVec3Length(&(rDstCol->vCenterPos - vRayOri));
	return true;
}

_bool CCollider::Check_Sphere(COL_INFO _rDstColInfo, RAY _rSrcRay, _v3 _vCapsuleVertexPos)
{
	_v3				 vRayPos = _rSrcRay.vRayPos;

	_v3 vDistance = _vCapsuleVertexPos - vRayPos;

	if (vDistance.x < _rDstColInfo.vRadius.x && vDistance.z < _rDstColInfo.vRadius.y && vDistance.z < _rDstColInfo.vRadius.z)
	{
		return true;
	}

	return false;
}

_bool CCollider::Check_AABB(CCollider * _rDstCol, RAY _rSrcRay)
{
	COL_INFO* rDstCol = _rDstCol->Get_ColInfo();
	_v3				 vRayPos = _rSrcRay.vRayPos;

	if (rDstCol->vMinPos.x <= vRayPos.x && rDstCol->vMaxPos.x >= vRayPos.x &&
		rDstCol->vMinPos.y <= vRayPos.y && rDstCol->vMaxPos.y >= vRayPos.y &&
		rDstCol->vMinPos.z <= vRayPos.z && rDstCol->vMaxPos.z >= vRayPos.z)
	{
		return true;
	}

	return false;
}

_bool CCollider::Check_Sphere(CCollider * _rDstCol, RAY _rSrcRay)
{
	COL_INFO* rDstCol = _rDstCol->Get_ColInfo();
	_v3				 vRayPos = _rSrcRay.vRayPos;

	_v3 vDistance = rDstCol->vCenterPos - vRayPos;

	if (vDistance.x < rDstCol->vRadius.x && vDistance.z < rDstCol->vRadius.y && vDistance.z < rDstCol->vRadius.x)
	{
		return true;
	}

	return false;
}

_bool CCollider::Check_Capsule(CCollider * _rDstCol, RAY _rSrcRay)
{
	// Sphere 체크 우선

	// 가까운 점 찾아내기
	_v3 vThisClose, vOtherClose;

	_v3 u = _rSrcRay.vRayDir;
	_v3 v = m_tColInfo.vEnd - m_tColInfo.vBegin;
	_v3 w = _rSrcRay.vRayPos - m_tColInfo.vBegin;

	_v3 vRayEnd = _rSrcRay.vRayPos + u;

	// a = dot  u u
	float a = D3DXVec3Dot(&u, &u);
	// b = dot u v
	float b = D3DXVec3Dot(&u, &v);
	// c = dot v v
	float c = D3DXVec3Dot(&v, &v);
	// d = dot u w
	float d = D3DXVec3Dot(&u, &w);
	// e = dot v w
	float e = D3DXVec3Dot(&v, &w);

	// ac - bb = 0  평행
	float denominator = (a * c - (b * b));

	if (fabsf(denominator) < 0.0001f)		// 평행함
	{
		float x = D3DXVec3Dot(&_rSrcRay.vRayDir, &m_tColInfo.vAxis);

		if (x < 0)	// 둔각
			vThisClose = m_tColInfo.vBegin;
		else
			vThisClose = m_tColInfo.vEnd;
	}
	else
	{
		// t = (ae - bd) / d
		float t = (a * e - b * d) / denominator;

		if (t <= 0)
			vThisClose = m_tColInfo.vBegin;
		else if (t >= 1)
			vThisClose = m_tColInfo.vEnd;
		else
			vThisClose = m_tColInfo.vBegin + v * t;
	}
	\
	return Check_Sphere(m_tColInfo, _rSrcRay, vThisClose);
}

_bool CCollider::Check_OBB(CCollider * _rSrc)
{
	COL_INFO* rDstCol = &m_tColInfo;
	COL_INFO* rSrtCol = _rSrc->Get_ColInfo();

	// 두 사각형 벡터의 길이
	_v3 fDist = rDstCol->vCenterPos - rSrtCol->vCenterPos;

	_float fC[3][3];
	_float fAbsC[3][3];
	_float fD[3];
	_float fR[3];
	_float fCutOff = 0.999999f;
	_bool  bPair = FALSE;

	// Dst 의 x 축 기준

	// 계산법 참조 /////////////////////////////////////////////////////////////////////////////
	LOOP(3)
	{
		fC[0][i] = D3DXVec3Dot(&rDstCol->vOBB_Dir[0], &rSrtCol->vOBB_Dir[i]);
		fAbsC[0][i] = abs(fC[0][i]);

		if (fAbsC[0][i] > fCutOff)
			bPair = TRUE;
	}

	fD[0] = D3DXVec3Dot(&fDist, &rDstCol->vOBB_Dir[0]);
	fR[0] = abs(fD[0]);
	fR[1] = rDstCol->vRadius[0];
	fR[2] = rSrtCol->vRadius[0] * fAbsC[0][0] +
		rSrtCol->vRadius[1] * fAbsC[0][1] +
		rSrtCol->vRadius[2] * fAbsC[0][2];

	if (fR[0] > fR[1] + fR[2])
		return FALSE;

	LOOP(3)
	{
		fC[1][i] = D3DXVec3Dot(&rDstCol->vOBB_Dir[1], &rSrtCol->vOBB_Dir[i]);
		fAbsC[1][i] = abs(fC[1][i]);

		if (fAbsC[1][i] > fCutOff)
			bPair = TRUE;
	}

	fD[1] = D3DXVec3Dot(&fDist, &rDstCol->vOBB_Dir[1]);
	fR[0] = abs(fD[1]);
	fR[1] = rDstCol->vRadius[1];
	fR[2] = rSrtCol->vRadius[0] * fAbsC[1][0] +
		rSrtCol->vRadius[1] * fAbsC[1][1] +
		rSrtCol->vRadius[2] * fAbsC[1][2];

	if (fR[0] > fR[1] + fR[2])
		return FALSE;

	LOOP(3)
	{
		fC[2][i] = D3DXVec3Dot(&rDstCol->vOBB_Dir[2], &rSrtCol->vOBB_Dir[i]);
		fAbsC[2][i] = abs(fC[2][i]);

		if (fAbsC[2][i] > fCutOff)
			bPair = TRUE;
	}

	fD[2] = D3DXVec3Dot(&fDist, &rDstCol->vOBB_Dir[2]);
	fR[0] = abs(fD[2]);
	fR[1] = rDstCol->vRadius[2];
	fR[2] = rSrtCol->vRadius[0] * fAbsC[2][0] +
		rSrtCol->vRadius[1] * fAbsC[2][1] +
		rSrtCol->vRadius[2] * fAbsC[2][2];

	if (fR[0] > fR[1] + fR[2])
		return FALSE;

	//LOOP(3)
	//{
	//	for (int j = 0; j < 3; ++j)
	//	{
	//		fC[i][j] = D3DXVec3Dot(&rDstCol->vOBB_Dir[i], &rSrtCol->vOBB_Dir[j]);
	//		fAbsC[i][j] = abs(fC[i][j]);
	//
	//		if (fAbsC[i][j] > fCutOff)
	//			bPair = TRUE;
	//	}
	//
	//	fD[i] = D3DXVec3Dot(&fDist, &rDstCol->vOBB_Dir[i]);
	//	fR[0] = abs(fD[i]);
	//	fR[1] = rDstCol->vRadius[i];
	//	fR[2] = rSrtCol->vRadius[0] * fAbsC[i][0] +
	//		rSrtCol->vRadius[1] * fAbsC[i][1] +
	//		rSrtCol->vRadius[2] * fAbsC[i][2];
	//
	//	if (fR[0] > fR[1] + fR[2])
	//		return FALSE;
	//}

	fR[0] = D3DXVec3Dot(&fDist, &rSrtCol->vOBB_Dir[0]);
	fR[1] = rDstCol->vRadius[0] * fAbsC[0][0] +
		rDstCol->vRadius[1] * fAbsC[1][0] +
		rDstCol->vRadius[2] * fAbsC[2][0];
	fR[2] = rSrtCol->vRadius[0];

	if (fR[0] > fR[1] + fR[2])
		return FALSE;


	fR[0] = D3DXVec3Dot(&fDist, &rSrtCol->vOBB_Dir[1]);
	fR[1] = rDstCol->vRadius[0] * fAbsC[0][1] +
		rDstCol->vRadius[1] * fAbsC[1][1] +
		rDstCol->vRadius[2] * fAbsC[2][1];
	fR[2] = rSrtCol->vRadius[1];

	if (fR[0] > fR[1] + fR[2])
		return FALSE;


	fR[0] = D3DXVec3Dot(&fDist, &rSrtCol->vOBB_Dir[2]);
	fR[1] = rDstCol->vRadius[0] * fAbsC[0][2] +
		rDstCol->vRadius[1] * fAbsC[1][2] +
		rDstCol->vRadius[2] * fAbsC[2][2];
	fR[2] = rSrtCol->vRadius[2];

	if (fR[0] > fR[1] + fR[2])
		return FALSE;

	if (bPair == TRUE)
		return TRUE;

	/////////////////////////////////////////////////////////////////
	//
	fR[0] = abs(fD[2] * fC[1][0] - fD[1] * fC[2][0]);
	fR[1] = rDstCol->vRadius[1] * fAbsC[2][0] + rDstCol->vRadius[2] * fAbsC[1][0];
	fR[2] = rSrtCol->vRadius[1] * fAbsC[0][2] + rSrtCol->vRadius[2] * fAbsC[0][1];

	if (fR[0] > fR[1] + fR[2])
		return FALSE;

	//
	fR[0] = abs(fD[2] * fC[1][1] - fD[1] * fC[2][1]);
	fR[1] = rDstCol->vRadius[1] * fAbsC[2][1] + rDstCol->vRadius[2] * fAbsC[1][1];
	fR[2] = rSrtCol->vRadius[0] * fAbsC[0][2] + rSrtCol->vRadius[2] * fAbsC[0][0];

	if (fR[0] > fR[1] + fR[2])
		return FALSE;

	//
	fR[0] = abs(fD[2] * fC[1][2] - fD[1] * fC[2][2]);
	fR[1] = rDstCol->vRadius[1] * fAbsC[2][2] + rDstCol->vRadius[2] * fAbsC[1][2];
	fR[2] = rSrtCol->vRadius[0] * fAbsC[0][1] + rSrtCol->vRadius[1] * fAbsC[0][0];

	if (fR[0] > fR[1] + fR[2])
		return FALSE;
	/////////////////////////////////////////////////////////////////

	//
	fR[0] = abs(fD[0] * fC[2][0] - fD[2] * fC[0][0]);
	fR[1] = rDstCol->vRadius[0] * fAbsC[2][0] + rDstCol->vRadius[2] * fAbsC[0][0];
	fR[2] = rSrtCol->vRadius[1] * fAbsC[1][2] + rSrtCol->vRadius[2] * fAbsC[1][1];

	if (fR[0] > fR[1] + fR[2])
		return FALSE;


	//
	fR[0] = abs(fD[0] * fC[2][1] - fD[2] * fC[0][1]);
	fR[1] = rDstCol->vRadius[0] * fAbsC[2][1] + rDstCol->vRadius[2] * fAbsC[0][1];
	fR[2] = rSrtCol->vRadius[0] * fAbsC[1][2] + rSrtCol->vRadius[2] * fAbsC[1][0];

	if (fR[0] > fR[1] + fR[2])
		return FALSE;


	//
	fR[0] = abs(fD[0] * fC[2][2] - fD[2] * fC[0][2]);
	fR[1] = rDstCol->vRadius[0] * fAbsC[2][2] + rDstCol->vRadius[2] * fAbsC[0][2];
	fR[2] = rSrtCol->vRadius[0] * fAbsC[1][1] + rSrtCol->vRadius[1] * fAbsC[1][0];

	if (fR[0] > fR[1] + fR[2])
		return FALSE;

	/////////////////////////////////////////////////////////////////
	//
	fR[0] = abs(fD[1] * fC[0][0] - fD[0] * fC[1][0]);
	fR[1] = rDstCol->vRadius[0] * fAbsC[1][0] + rDstCol->vRadius[1] * fAbsC[0][0];
	fR[2] = rSrtCol->vRadius[1] * fAbsC[2][2] + rSrtCol->vRadius[2] * fAbsC[2][1];

	if (fR[0] > fR[1] + fR[2])
		return FALSE;


	//
	fR[0] = abs(fD[1] * fC[0][1] - fD[0] * fC[1][1]);
	fR[1] = rDstCol->vRadius[0] * fAbsC[1][1] + rDstCol->vRadius[1] * fAbsC[0][1];
	fR[2] = rSrtCol->vRadius[0] * fAbsC[2][2] + rSrtCol->vRadius[2] * fAbsC[2][0];

	if (fR[0] > fR[1] + fR[2])
		return FALSE;


	//
	fR[0] = abs(fD[1] * fC[0][2] - fD[0] * fC[1][2]);
	fR[1] = rDstCol->vRadius[0] * fAbsC[1][2] + rDstCol->vRadius[1] * fAbsC[0][2];
	fR[2] = rSrtCol->vRadius[0] * fAbsC[2][1] + rSrtCol->vRadius[1] * fAbsC[2][0];

	if (fR[0] > fR[1] + fR[2])
		return FALSE;

	return TRUE;
}

_bool CCollider::Check_AABB(CCollider* _rSrc)
{
	COL_INFO* rDstCol = &m_tColInfo;
	COL_INFO* rSrtCol = _rSrc->Get_ColInfo();

	if (rDstCol->vMinPos.x <= rSrtCol->vMaxPos.x && rDstCol->vMaxPos.x >= rSrtCol->vMinPos.x &&
		rDstCol->vMinPos.y <= rSrtCol->vMaxPos.y && rDstCol->vMaxPos.y >= rSrtCol->vMinPos.y &&
		rDstCol->vMinPos.z <= rSrtCol->vMaxPos.z && rDstCol->vMaxPos.z >= rSrtCol->vMinPos.z)
	{
		if (!rDstCol->bIsTrigger && !rSrtCol->bIsTrigger)  // 두 박스가 모두 Trigger 가 아닌지 판단
		{
			_v3 vLength = { 0,0,0 };
			_bool bCheckDynamic = false;

			// 대상들이 동적인 Object 인지 여부
			if (rDstCol->bIsDynamic)
			{
				if (rSrtCol->bIsDynamic)
					bCheckDynamic = true;

				vLength = Calc_Length(rDstCol, rSrtCol, bCheckDynamic);

				(rDstCol->vCenterPos.x < rSrtCol->vCenterPos.x ? vLength.x *= -1.f : vLength.x *= 1.f);
				(rDstCol->vCenterPos.y < rSrtCol->vCenterPos.y ? vLength.y *= -1.f : vLength.y *= 1.f);
				(rDstCol->vCenterPos.z < rSrtCol->vCenterPos.z ? vLength.z *= -1.f : vLength.z *= 1.f);

				this->Set_Length(vLength);
			}

			if (rSrtCol->bIsDynamic)
			{
				if (rDstCol->bIsDynamic)
					bCheckDynamic = true;

				vLength = Calc_Length(rDstCol, rSrtCol, bCheckDynamic);

				(rDstCol->vCenterPos.x < rSrtCol->vCenterPos.x ? vLength.x *= 1.f : vLength.x *= -1.f);
				(rDstCol->vCenterPos.y < rSrtCol->vCenterPos.y ? vLength.y *= 1.f : vLength.y *= -1.f);
				(rDstCol->vCenterPos.z < rSrtCol->vCenterPos.z ? vLength.z *= 1.f : vLength.z *= -1.f);

				_rSrc->Set_Length(vLength);
			}
		}

		return true;
	}

	return false;
}

_bool CCollider::Check_Sphere(CCollider * _rSrc , _float* _fLegnthOut, _v3* _vDirOut)
{
	_float fLength = D3DXVec3Length(&(_rSrc->Get_CenterPos() - m_tColInfo.vCenterPos));
	_float fRadiusSum = _rSrc->Get_Radius().x + m_tColInfo.vRadius.x;

	*_fLegnthOut = fLength - fRadiusSum;
	*_vDirOut = *V3_NORMAL_SELF(&(_rSrc->Get_CenterPos() - m_tColInfo.vCenterPos));

	return (fLength > fRadiusSum ? false : true);
}

_bool CCollider::Check_Sphere(CCollider * _rSrc)
{
	_float fLength = D3DXVec3Length(&(_rSrc->Get_CenterPos() - m_tColInfo.vCenterPos));
	_float fRadiusSum = _rSrc->Get_Radius().x + m_tColInfo.vRadius.x;

	return (fLength > fRadiusSum ? false : true);
}

_bool CCollider::Check_Capsule(CCollider * _rSrc)
{
	COL_INFO* rSrtCol = _rSrc->Get_ColInfo();

	// Sphere 체크 우선

	// 가까운 점 찾아내기
	_v3 vThisClose, vOtherClose;

	_v3 u = m_tColInfo.vEnd - m_tColInfo.vBegin;
	_v3 v = rSrtCol->vEnd - rSrtCol->vBegin;
	_v3 w = rSrtCol->vEnd - m_tColInfo.vBegin;

	// a = dot  u u
	float a = D3DXVec3Dot(&u, &u);
	// b = dot u v
	float b = D3DXVec3Dot(&u, &v);
	// c = dot v v
	float c = D3DXVec3Dot(&v, &v);
	// d = dot u w
	float d = D3DXVec3Dot(&u, &w);
	// e = dot v w
	float e = D3DXVec3Dot(&v, &w);

	// ac - bb = 0  평행
	float denominator = (a * c - (b * b));

	if (denominator < 0.0001f)		// 평행함
	{
		float x = D3DXVec3Dot(&(rSrtCol->vEnd - m_tColInfo.vBegin), &m_tColInfo.vAxis);

		if (x < 0)	// 둔각
		{
			vOtherClose = rSrtCol->vEnd;
			vThisClose = m_tColInfo.vBegin;
		}
		else
		{
			float y = D3DXVec3Dot(&(rSrtCol->vEnd - rSrtCol->vBegin), &rSrtCol->vAxis);

			if (y < 0)
			{
				vOtherClose = rSrtCol->vBegin;
				vThisClose = m_tColInfo.vEnd;
			}
			else
			{
				if (x < y)		// 짧은 쪽의 중앙이 가까운 점
				{
					vThisClose = m_tColInfo.vBegin + x * 0.5f * m_tColInfo.vAxis;
					vOtherClose = rSrtCol->vBegin + D3DXVec3Dot(&(vThisClose - rSrtCol->vBegin), &rSrtCol->vAxis) * rSrtCol->vAxis;
				}
				else
				{
					vOtherClose = rSrtCol->vBegin + y * 0.5f * rSrtCol->vAxis;
					vThisClose = m_tColInfo.vBegin + D3DXVec3Dot(&(vOtherClose - m_tColInfo.vBegin), &m_tColInfo.vAxis) * m_tColInfo.vAxis;
				}
			}
		}
	}

	else
	{
		denominator = 1.f / denominator;
		// s = (be - cd) / d
		float s = (b * e - c * d) * denominator;

		if (s <= 0)
			vOtherClose = rSrtCol->vBegin;
		else if (s >= 1)
			vOtherClose = rSrtCol->vEnd;
		else
			vOtherClose = rSrtCol->vBegin + u * s;

		// t = (ae - bd) / d
		float t = (a * e - b * d) * denominator;

		if (t <= 0)
			vThisClose = m_tColInfo.vBegin;
		else if (t >= 1)
			vThisClose = m_tColInfo.vEnd;
		else
			vThisClose = m_tColInfo.vBegin + v * t;
	}

	_v3 vDiff(vOtherClose - vThisClose);
	float fSumRadius = rSrtCol->vRadius.x + m_tColInfo.vRadius.x;

	float fOverlap = fSumRadius - D3DXVec3Length(&vDiff);

	if (fOverlap < 0)
		return false;

	V3_NORMAL_SELF(&vDiff);

	if (m_tColInfo.bIsTrigger || rSrtCol->bIsTrigger)	// 둘 중 하나라도 트리거 이면
	{
		m_tColInfo.vCrossPoint = vThisClose + vDiff * (m_tColInfo.vRadius.x - fOverlap * 0.5f);	// 겹친부위의 중간
		return true;		// 바로 리턴
	}

	// 충돌 거리 계산

	return false;
}

_v3 CCollider::Calc_Length(COL_INFO * _rDst, COL_INFO * _rSrc, _bool _dynamic)
{
	_v3 vMin, vMax, vLength = {};

	vMax.x = min(_rDst->vMaxPos.x, _rSrc->vMaxPos.x);
	vMax.y = min(_rDst->vMaxPos.y, _rSrc->vMaxPos.y);
	vMax.z = min(_rDst->vMaxPos.z, _rSrc->vMaxPos.z);

	vMin.x = max(_rDst->vMinPos.x, _rSrc->vMinPos.x);
	vMin.y = max(_rDst->vMinPos.y, _rSrc->vMinPos.y);
	vMin.z = max(_rDst->vMinPos.z, _rSrc->vMinPos.z);

	vLength.x = vMax.x - vMin.x;
	vLength.y = vMax.y - vMin.y;
	vLength.z = vMax.z - vMin.z;

	if (vLength.x == vLength.y && vLength.y == vLength.z)
		return V3_NULL;

	(fabs(vLength.x) < fabs(vLength.y) ?
		(vLength.y = 0, fabs(vLength.x) < fabs(vLength.z) ? vLength.z = 0 : vLength.x = 0) :
		(vLength.x = 0, fabs(vLength.y) < fabs(vLength.z) ? vLength.z = 0 : vLength.y = 0));

	if (_dynamic)
		return vLength * 0.5f;

	else
		return vLength;
}


CCollider* CCollider::Create(_Device _pGraphicDev)
{
	CCollider* pComponent = new CCollider(_pGraphicDev);

	if (FAILED(pComponent->Ready_Component()))
	{
		Safe_Release(pComponent);
		return nullptr;
	}

	return pComponent;
}

void CCollider::Free()
{
	CComponent::Free();
}

CComponent * CCollider::Clone_Component(void* _pArg)
{
	return new CCollider(*this);
}
