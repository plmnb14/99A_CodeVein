#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

typedef class ENGINE_DLL CCalculater
{
private:
	explicit CCalculater();
	virtual ~CCalculater();

public:
	static void V3_Axis(_mat* _pOut_1, _v3* _pOut_2, const _v3* _pIn_1, const _float _pAngle, _bool _bRadian = false);
	static void V3_Axis_Normal(_mat* _pOut_1 , _v3* _pOut_2, const _v3* _pIn_1, const _float _pAngle, _bool _bRadian = false);

public:
	static void V3_Dir_Normal(_v3* _pOut, const _v3* _pIn_1, const _v3* _pIn_2);
	static void V3_Cross_Normal(_v3* _pOut, const _v3* _pIn_1, const _v3* _pIn_2);

public:
	static void V3_RotX(_v3* _pOut, const _v3* _pIn , const _float _fRadian);
	static void V3_RotY(_v3* _pOut, const _v3* _pIn , const _float _fRadian);
	static void V3_RotZ(_v3* _pOut, const _v3* _pIn , const _float _fRadian);

public:
	static const _int Random_Num(_int _iBegin, _int _iEnd);
	static const _v3  Random_Dir(_float _fLength);
	static const _v3  Random_Dir(_float _fDirX = 1.f, _float _fDirY = 1.f, _float _fDirZ = 1.f);
	static const _v3  Random_Dir(_int _iX_Begin, _int _iX_End, _int _iY_Begin, _int _iY_End, _int _iZ_Begin, _int _iZ_End);
	static const _v3  Random_Dir_Positive();
	static const _v3  Random_Dir_Nagative();

}CALC;

END
