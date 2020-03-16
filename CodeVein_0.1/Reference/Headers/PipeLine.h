#pragma once

#include "Base.h"

BEGIN(Engine)

class CPipeLine final : public CBase
{
	DECLARE_SINGLETON(CPipeLine)
private:
	explicit CPipeLine();
	virtual ~CPipeLine() = default;
public:
	_mat Get_Transform(D3DTRANSFORMSTATETYPE eState) {
		if (eState == D3DTS_WORLD) 
			return _mat();
		return eState == D3DTS_VIEW ? m_ViewMatrix : m_ProjMatrix;
	}
	_mat Get_Transform_Inverse(D3DTRANSFORMSTATETYPE eState) {
		if (eState == D3DTS_WORLD)
			return _mat();
		return eState == D3DTS_VIEW ? m_WorldMatrixofCamera : m_ProjMatrixInverse;
	}
	_v3	Get_CamPosition() const {
		return *(_v3*)&m_WorldMatrixofCamera.m[3][0]; }
public:
	_int Update_PipeLine();
	HRESULT Set_Transform(D3DTRANSFORMSTATETYPE eState, _mat StateMatrix);
private:
	_mat			m_WorldMatrixofCamera;
	_mat			m_ViewMatrix;
	_mat			m_ProjMatrixInverse;
	_mat			m_ProjMatrix;
public:
	virtual void Free();
};

END