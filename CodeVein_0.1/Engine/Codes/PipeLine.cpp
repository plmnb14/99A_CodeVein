#include "..\Headers\PipeLine.h"

IMPLEMENT_SINGLETON(CPipeLine)

CPipeLine::CPipeLine()
{
}

_int CPipeLine::Update_PipeLine()
{
	D3DXMatrixInverse(&m_WorldMatrixofCamera, nullptr, &m_ViewMatrix);
	D3DXMatrixInverse(&m_ProjMatrixInverse, nullptr, &m_ProjMatrix);

	return _int();
}

HRESULT CPipeLine::Set_Transform(D3DTRANSFORMSTATETYPE eState, _mat StateMatrix)
{
	switch (eState)
	{
	case D3DTS_VIEW:
		m_ViewMatrix = StateMatrix;
		break;
	case D3DTS_PROJECTION:
		m_ProjMatrix = StateMatrix;
		break;	
	}
	return NOERROR;
}

void CPipeLine::Free()
{

}
