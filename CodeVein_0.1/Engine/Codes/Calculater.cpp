#include "Calculater.h"

CCalculater::CCalculater()
{
}


CCalculater::~CCalculater()
{
}

void CCalculater::V3_Axis(_mat * _pOut_1, _v3 * _pOut_2, const _v3 * _pIn_1, const _float _pAngle, _bool _bRadian)
{
	_mat pOut_1 = *_pOut_1;
	_v3 pOut_2 = *_pOut_2;
	_v3 pIn_1 = *_pIn_1;
	_float fRadian = 0;
	(_bRadian ? fRadian = D3DXToRadian(_pAngle) : fRadian = _pAngle);

	D3DXMatrixRotationAxis(&pOut_1, &pIn_1, fRadian);

	*_pOut_1 = pOut_1;
	*_pOut_2 = pOut_2;
}

void CCalculater::V3_Axis_Normal(_mat * _pOut_1, _v3 * _pOut_2, const _v3 * _pIn_1, const _float _pAngle, _bool _bRadian)
{
	_mat pOut_1 = *_pOut_1;
	_v3 pOut_2 = *_pOut_2;
	_v3 pIn_1 = *_pIn_1;
	_float fRadian = 0;
	(_bRadian ? fRadian = D3DXToRadian(_pAngle) : fRadian = _pAngle);

	D3DXMatrixRotationAxis(&pOut_1, &pIn_1, fRadian);
	D3DXVec3TransformNormal(&pOut_2, &pOut_2, &pOut_1);

	*_pOut_1 = pOut_1;
	*_pOut_2 = pOut_2;
}

void CCalculater::V3_Dir_Normal(_v3 * _pOut, const _v3 * _pIn_1, const _v3 * _pIn_2)
{
	_v3 pOut = *_pOut;
	_v3 pIn_1 = *_pIn_1;
	_v3 pIn_2 = *_pIn_2;

	pOut = pIn_1 - pIn_2;
	D3DXVec3Normalize(&pOut, &pOut);

	*_pOut = pOut;
}

void CCalculater::V3_Cross_Normal(_v3* _pOut, const _v3* _pIn_1, const _v3* _pIn_2)
{
	_v3 pOut = *_pOut;
	_v3 pIn_1 = *_pIn_1;
	_v3 pIn_2 = *_pIn_2;

	D3DXVec3Cross(&pOut, &pIn_1, &pIn_2);
	D3DXVec3Normalize(&pOut, &pOut);

	*_pOut = pOut;
}

void CCalculater::V3_RotX(_v3 * _pOut, const _v3 * _pIn, const _float _fRadian)
{
	_float fCos = cosf(_fRadian);
	_float fSin = sinf(_fRadian);

	_v3 pOut = *_pOut;
	_v3 pIn = *_pIn;

	pOut.x = pIn.x;
	pOut.y = pIn.y * fCos +pIn.z * -fSin;
	pOut.z = pIn.y * fSin +pIn.z * fCos;

	*_pOut = pOut;
}

void CCalculater::V3_RotY(_v3 * _pOut, const _v3 * _pIn, const _float _fRadian)
{
	_float fCos = cosf(_fRadian);
	_float fSin = sinf(_fRadian);

	_v3 pOut = *_pOut;
	_v3 pIn = *_pIn;

	pOut.x = pIn.x * fCos + pIn.z * fSin;
	pOut.y = pIn.y;
	pOut.z = pIn.x * -fSin + pIn.z * fCos;

	*_pOut = pOut;
}

void CCalculater::V3_RotZ(_v3 * _pOut, const _v3 * _pIn, const _float _fRadian)
{
	_float fCos = cosf(_fRadian);
	_float fSin = sinf(_fRadian);

	_v3 pOut = *_pOut;
	_v3 pIn = *_pIn;

	pOut.x = pIn.x * fCos + pIn.y * -fSin;
	pOut.y = pIn.x * fSin + pIn.y * fCos;
	pOut.z = pIn.z;

	*_pOut = pOut;
}

void CCalculater::Generate_ViewMat(_mat * _pOut, const _v3 * vPos, const _v3 * vRight, const _v3 * vUp, const _v3 * vLook)
{
	_mat pOutmat = *_pOut;

	pOutmat =
	{
		vRight->x , vRight->y, vRight->z, 0.f,
		vUp->x, vUp->y, vUp->z, 0.f,
		vLook->x, vLook->y, vLook->z, 0.f,
		-D3DXVec3Dot(vRight , vPos),
		-D3DXVec3Dot(vUp , vPos),
		-D3DXVec3Dot(vLook , vPos),
		1.f
	};

	*_pOut = pOutmat;
}

void CCalculater::Generate_ViewMat(_mat * _pOut, const _v3 * vPos, const _v3 * vLookAt, const _v3 * vUp)
{
	_mat pOutmat = *_pOut;

	_v3 vLook = *vPos - *vLookAt;
	D3DXVec3Normalize(&vLook , &vLook);

	_v3 vRight = *D3DXVec3Cross(&vRight, &vLook, vUp);
	D3DXVec3Normalize(&vRight, &vRight);

	pOutmat =
	{
		vRight.x , vRight.y, vRight.z, 0.f,
		vUp->x, vUp->y, vUp->z, 0.f,
		vLook.x, vLook.y, vLook.z, 0.f,
		-D3DXVec3Dot(&vRight , vPos),
		-D3DXVec3Dot(vUp , vPos),
		-D3DXVec3Dot(&vLook , vPos),
		1.f
	};

	*_pOut = pOutmat;
}

void CCalculater::Generate_ProjMat_Per(_mat * matOut, const _float * fFovY, const _float * fAspect, const _float * fZN, const _float * fZF)
{
	_mat pOutmat = *matOut;
	
	ZeroMemory(pOutmat, sizeof(_mat));

	_float h = 1.f / tanf(*fFovY * 0.5f);
	_float w = h / *fAspect;
	_float r = *fZF / (*fZF - *fZN);

	pOutmat._11 = w;
	pOutmat._22 = h;
	pOutmat._33 = r;
	pOutmat._43 = -*fZN * r;
	pOutmat._34 = 1.f;

	*matOut = pOutmat;
}

void CCalculater::Generate_ProjMat_Ortho(_mat * matOut, const _float * fW, const _float * fH, const _float * fZN, const _float * fZF)
{
	_mat pOutmat = *matOut;

	ZeroMemory(pOutmat, sizeof(_mat));

	pOutmat._11 = 2.f / *fW;
	pOutmat._22 = 2.f / *fH;
	pOutmat._33 = 1.f / (*fZF - *fZN);
	pOutmat._43 = -*fZN / (*fZN - *fZF);
	pOutmat._34 = 1.f;

	*matOut = pOutmat;
}

// _fValue1 - 시작점, _fValue2 - 마지막점, _fAmount - 시간
float CCalculater::Lerp(float _fValue1, float _fValue2, float _fAmount)
{
	return float(_fValue1 + ((float)(_fValue2 - _fValue1) * _fAmount));
}

const _double CCalculater::Random_Num_Double(_double dBegin, _double _dEnd)
{
	random_device rd;
	mt19937_64 rng(rd());

	if (dBegin > _dEnd)
	{
		_double dTemp = dBegin;
		dBegin = _dEnd;
		_dEnd = dTemp;
	}

	uniform_real<> Value(dBegin, _dEnd);

	return Value(rng);
}

const _int CCalculater::Random_Num(_int _iBegin, _int _iEnd)
{
	random_device rd;
	mt19937_64	rng(rd());

	uniform_int_distribution<> Value(_iBegin, _iEnd);

	return Value(rng);
}

const _v3 CCalculater::Random_Dir(_float _fDirX, _float _fDirY, _float _fDirZ)
{
	_v3 vRandomDir;

	random_device rd;
	mt19937_64	rng(rd());

	uniform_int_distribution<> dis_a(-100, 100);
	uniform_int_distribution<> dis_b(-100, 100);
	uniform_int_distribution<> dis_c(-100, 100);

	vRandomDir.x = _float(dis_a(rng)) * 0.01f * _fDirX;
	vRandomDir.y = _float(dis_b(rng)) * 0.01f * _fDirY;
	vRandomDir.z = _float(dis_c(rng)) * 0.01f * _fDirZ;

	return (*V3_NORMAL_SELF(&vRandomDir));
}

const _v3 CCalculater::Random_Dir(_float _fLength)
{
	_v3 vRandomDir;

	random_device rd;
	mt19937_64	rng(rd());

	uniform_int_distribution<> dis_a(-100, 100);
	uniform_int_distribution<> dis_b(-100, 100);
	uniform_int_distribution<> dis_c(-100, 100);

	vRandomDir.x = _float(dis_a(rng)) * 0.01f;
	vRandomDir.y = _float(dis_b(rng)) * 0.01f;
	vRandomDir.z = _float(dis_c(rng)) * 0.01f;

	return (*V3_NORMAL_SELF(&vRandomDir)) * _fLength;
}

const _v3 CCalculater::Random_Dir(_int _iX_Begin, _int _iX_End, _int _iY_Begin, _int _iY_End, _int _iZ_Begin, _int _iZ_End)
{
	_v3 vRandomDir;

	random_device rd;
	mt19937_64	rng(rd());

	uniform_int_distribution<> dis_a(_iX_Begin, _iX_End);
	uniform_int_distribution<> dis_b(_iY_Begin, _iY_End);
	uniform_int_distribution<> dis_c(_iZ_Begin, _iZ_End);

	vRandomDir.x = _float(dis_a(rng)) * 0.01f;
	vRandomDir.y = _float(dis_b(rng)) * 0.01f;
	vRandomDir.z = _float(dis_c(rng)) * 0.01f;

	return (*V3_NORMAL_SELF(&vRandomDir));
}

const _v3 CCalculater::Random_Dir_Positive()
{
	_v3 vRandomDir;

	random_device rd;
	mt19937_64	rng(rd());

	uniform_int_distribution<> dis_a(-100, 100);
	uniform_int_distribution<> dis_b(-100, 100);
	uniform_int_distribution<> dis_c(-100, 100);

	vRandomDir.x = _float(dis_a(rng)) * 0.01f;
	vRandomDir.y = _float(dis_b(rng)) * 0.01f;
	vRandomDir.z = _float(dis_c(rng)) * 0.01f;

	return (*V3_NORMAL_SELF(&vRandomDir));
}

const _v3 CCalculater::Random_Dir_Nagative()
{
	_v3 vRandomDir;

	random_device rd;
	mt19937_64	rng(rd());

	uniform_int_distribution<> dis_a(-100, 100);
	uniform_int_distribution<> dis_b(-100, 100);
	uniform_int_distribution<> dis_c(-100, 100);

	vRandomDir.x = _float(dis_a(rng)) * 0.01f;
	vRandomDir.y = _float(dis_b(rng)) * 0.01f;
	vRandomDir.z = _float(dis_c(rng)) * 0.01f;

	return (*V3_NORMAL_SELF(&vRandomDir));
}
