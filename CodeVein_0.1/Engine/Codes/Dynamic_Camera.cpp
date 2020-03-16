#include "Dynamic_Camera.h"
#include "Management.h"

CDynamic_Camera::CDynamic_Camera(_Device pGraphicDev)
	: CCamera(pGraphicDev)
{
}


CDynamic_Camera::~CDynamic_Camera()
{
}

void CDynamic_Camera::KeyInput()
{
	float fMoveSpeed = 0.05f;
	if (GetAsyncKeyState('W') & 0x8000)
	{
		Walk_H(fMoveSpeed);
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		Walk_H(-fMoveSpeed);
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		Walk_V(-fMoveSpeed);
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		Walk_V(fMoveSpeed);
	}

	if (GetAsyncKeyState('R') & 0x8000)
	{
		Rotate_X(-fMoveSpeed * 1.5f);
	}

	if (GetAsyncKeyState('F') & 0x8000)
	{
		Rotate_X(fMoveSpeed * 1.5f);
	}

	if (GetAsyncKeyState('Q') & 0x8000)
	{
		Rotate_Y(-fMoveSpeed * 1.5f);
	}

	if (GetAsyncKeyState('E') & 0x8000)
	{
		Rotate_Y(fMoveSpeed * 1.5f);
	}

	if (GetAsyncKeyState('C') & 0x8000)
	{
		m_fFov -= fMoveSpeed;
	}

	if (GetAsyncKeyState('V') & 0x8000)
	{
		m_fFov += fMoveSpeed;
	}

	if (GetAsyncKeyState(VK_LSHIFT) & 0x8000)
	{
		m_pTransform->Add_Pos(fMoveSpeed, _v3(0, 1, 0));
		m_pTransform->Add_At(-fMoveSpeed, _v3(0, 1, 0));
	}

	if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)
	{
		m_pTransform->Add_Pos(-fMoveSpeed, _v3(0, 1, 0));
		m_pTransform->Add_At(-fMoveSpeed, _v3(0, 1, 0));
	}
}

_int CDynamic_Camera::Update_GameObject()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	Camera_Oscillate();
	SetUp_ViewType(m_eCamView);
	SetUp_MouseRotate();

	CCamera::SetUp_ViewMat();
	CCamera::SetUp_ProjMat();
	CCamera::Update_PipeLine();

	m_pGraphic_Dev->SetTransform(D3DTS_VIEW, &m_MatView);
	m_pGraphic_Dev->SetTransform(D3DTS_PROJECTION, &m_MatProj);

	return NO_EVENT;
}

void CDynamic_Camera::LateUpdate()
{
}

void CDynamic_Camera::Render()
{
}

HRESULT CDynamic_Camera::Initialize()
{
	AddComponent();

	CCamera::SetUp_Default();
	CCamera::SetUp_ViewMat();
	CCamera::SetUp_ProjMat();
	CCamera::SetUp_PipeLine();
	
	m_pGraphic_Dev->SetTransform(D3DTS_VIEW, &m_MatView);
	m_pGraphic_Dev->SetTransform(D3DTS_PROJECTION, &m_MatProj);

	return S_OK;
}

HRESULT CDynamic_Camera::LateInit()
{
	return S_OK;
}

void CDynamic_Camera::Free()
{
	Safe_Release(m_pTransform);

	CCamera::Free();
}

HRESULT CDynamic_Camera::AddComponent()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	return S_OK;
}

CDynamic_Camera * CDynamic_Camera::Create(_Device pGraphicDev, 
										  CameraView _CameraViewType, 
										  CameraMode _CameraMode)
{
	CDynamic_Camera* pInstance = new CDynamic_Camera(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	pInstance->m_eCamView = _CameraViewType;
	pInstance->m_eCamMode = _CameraMode;

	return pInstance;
}

CGameObject * CDynamic_Camera::Clone_GameObject(void * pArg)
{
	return nullptr;
}
