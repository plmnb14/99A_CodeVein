#include "Camera.h"
#include "Management.h"
#include "Timer_Manager.h"
#include "CameraMgr.h"

CCamera::CCamera(_Device pGraphicDev)
	: CGameObject(pGraphicDev),
	m_pTransform(nullptr),
	m_eCamClass(STATIC_CAM), m_eCamView(THIRD_VIEW), m_eCamMode(DEFAULT_MODE),
	m_fNear(0), m_fFar(0), m_fFov(0),
	m_bGetMp(false), m_bReverseRight(false), m_bMouseControl(false)
{
}


CCamera::~CCamera()
{
}

void CCamera::Walk_V(_float _Speed)
{
	switch (m_eCamMode)
	{
	case DEFAULT_MODE:
	{
		_v3 vDir, vRight, vWorldUp;

		vDir = m_pTransform->Get_At() - m_pTransform->Get_Pos();
		D3DXVec3Normalize(&vDir, &vDir);

		D3DXVec3Cross(&vRight, &WORLD_UP, &vDir);
		D3DXVec3Normalize(&vRight, &vRight);

		m_pTransform->Add_Pos(vRight * _Speed);
		m_pTransform->Add_At(vRight * _Speed);
		break;
	}
	}
}

void CCamera::Walk_H(_float _Speed)
{
	_v3 vDir, vWorldUp;
	vWorldUp = { 0.f , 1.f ,0.f };

	vDir = m_pTransform->Get_At() - m_pTransform->Get_Pos();
	D3DXVec3Normalize(&vDir, &vDir);

	switch (m_eCamMode)
	{
	case DEFAULT_MODE:
	{
		m_pTransform->Add_Pos(vDir * _Speed);
		m_pTransform->Add_At(vDir * _Speed);

		break;
	}
	}
}

void CCamera::Rotate_X(_float _Angle)
{
	_mat T;
	_v3 tmpUp, tmpLook;

	D3DXMatrixRotationAxis(&T, &m_pTransform->Get_Axis(AXIS_X), D3DXToRadian(_Angle));

	D3DXVec3TransformCoord(&tmpUp, &m_pTransform->Get_Axis(AXIS_Y), &T);
	D3DXVec3TransformCoord(&tmpLook, &m_pTransform->Get_Axis(AXIS_Z), &T);

	m_pTransform->Set_Axis(tmpUp, AXIS_Y);
	m_pTransform->Set_Axis(tmpLook, AXIS_Z);
}

void CCamera::Rotate_Y(_float _Angle)
{
	_v3  vView, vWorldUp;
	_mat T;

	switch (m_eCamMode)
	{
	case DEFAULT_MODE:
	{
		D3DXMatrixRotationAxis(&T, &m_pTransform->Get_Axis(AXIS_Y), D3DXToRadian(_Angle));
		break;
	}
	}


	_v3 vTmpRight, vTempLook;

	D3DXVec3TransformCoord(&vTmpRight, &m_pTransform->Get_Axis(AXIS_X), &T);
	D3DXVec3TransformCoord(&vTempLook, &m_pTransform->Get_Axis(AXIS_Z), &T);

	m_pTransform->Set_Axis(vTmpRight, AXIS_X);
	m_pTransform->Set_Axis(vTempLook, AXIS_Z);
}

void CCamera::Rotate_Z(_float _Angle)
{
	_mat T;

	switch (m_eCamMode)
	{
	case DEFAULT_MODE:
	{
		D3DXMatrixRotationAxis(&T, &m_pTransform->Get_Axis(AXIS_Z), D3DXToRadian(_Angle));
		break;
	}
	}

	_v3 vTmpRight, vTmpUp;

	D3DXVec3TransformCoord(&vTmpRight, &m_pTransform->Get_Axis(AXIS_X), &T);
	D3DXVec3TransformCoord(&vTmpUp, &m_pTransform->Get_Axis(AXIS_Y), &T);

	m_pTransform->Set_Axis(vTmpRight, AXIS_X);
	m_pTransform->Set_Axis(vTmpUp, AXIS_Y);
}

void CCamera::Camera_Oscillatation_SetUp(_float _fDuration, _float _fFrequency, _float _fPower, _float _fMuliply, CAM_OSC_TYPE _eOSCType)
{
	m_fOSC_Duration = _fDuration;
	m_fOSC_Frequency = _fFrequency;
	m_fOSC_Power = _fPower;
	m_fOSC_Mutiply = _fMuliply;
	m_eOSCType = _eOSCType;
}

void CCamera::Camera_Oscillate()
{
	if (m_eOSCType == OSC_END)
		return;

	switch (m_eOSCType)
	{
	case ROT_OSC:
	{
		Camera_Oscillate_Rotate();
		break;
	}
	case POS_OSC:
	{
		Camera_Oscillate_Position();
		break;
	}
	case FOV_OSC:
	{
		Camera_Oscillate_FOV();
		break;
	}
	}

}

void CCamera::Camera_Oscillate_Rotate()
{
}

void CCamera::Camera_Oscillate_Position()
{
	if (m_fOSC_Frequency <= 0.001f || m_fOSC_Duration <= m_fOSC_Timer[0] || 0.001f >= m_fOSC_Power)
	{
		m_eOSCType = OSC_END;

		LOOP(3)
		{
			m_fOSCAxis_Gap[i] = 0.f;
			m_fOSC_Timer[i] = 0.f;
			m_bOSCReverse[i] = 0.f;
		}

		m_fOSC_Power = 0.f;
		m_fOSC_Frequency = 0.f;
		m_fOSC_Duration = 0.f;
		m_fOSC_Mutiply = 0.f;

		m_bOSCOrigin = false;

		m_fMidDist = 3.f;

		return;
	}

	m_fOSC_Timer[0] += Engine::CTimer_Manager::Get_Instance()->Get_DeltaTime(L"Timer_Fps_60_2");

	// AXIS_X
	if (true == m_bOSCReverse[0])
	{
		if (false == m_bOSCOrigin)
		{
			m_fOSCAxis_Gap[0] += Engine::CTimer_Manager::Get_Instance()->Get_DeltaTime(L"Timer_Fps_60_2") * m_fOSC_Frequency;

			if (m_fOSCAxis_Gap[0] >= m_fOSC_Power)
			{
				m_bOSCOrigin = true;
				m_fOSC_Frequency *= m_fOSC_Mutiply;

				m_fOSCAxis_Gap[0] = m_fOSC_Power;
			}
		}

		else
		{
			m_fOSCAxis_Gap[0] -= Engine::CTimer_Manager::Get_Instance()->Get_DeltaTime(L"Timer_Fps_60_2") * m_fOSC_Frequency;

			if (m_fOSCAxis_Gap[0] <= 0)
			{
				m_bOSCOrigin = false;
				m_fOSC_Power *= m_fOSC_Mutiply;

				m_bOSCReverse[0] = false;
			}
		}
	}

	else if (false == m_bOSCReverse[0])
	{
		if (false == m_bOSCOrigin)
		{
			m_fOSCAxis_Gap[0] -= Engine::CTimer_Manager::Get_Instance()->Get_DeltaTime(L"Timer_Fps_60_2") * m_fOSC_Frequency;

			if (m_fOSCAxis_Gap[0] <= -m_fOSC_Power)
			{
				m_bOSCOrigin = true;
				m_fOSC_Frequency *= m_fOSC_Mutiply;

				m_fOSCAxis_Gap[0] = -m_fOSC_Power;
			}
		}

		else
		{
			m_fOSCAxis_Gap[0] += Engine::CTimer_Manager::Get_Instance()->Get_DeltaTime(L"Timer_Fps_60_2") * m_fOSC_Frequency;

			if (m_fOSCAxis_Gap[0] >= 0)
			{
				m_bOSCOrigin = false;
				m_fOSC_Power *= m_fOSC_Mutiply;

				m_bOSCReverse[0] = true;
			}
		}
	}

	// AXIS_Y

	if (true == m_bOSCReverse[1])
	{
		if (false == m_bOSCOrigin)
		{
			m_fOSCAxis_Gap[1] += Engine::CTimer_Manager::Get_Instance()->Get_DeltaTime(L"Timer_Fps_60_2") * (m_fOSC_Frequency);

			if (m_fOSCAxis_Gap[1] >= m_fOSC_Power)
			{
				////cout << "??gfdgdfgd?" << endl;

				m_bOSCOrigin = true;
				m_fOSC_Frequency *= m_fOSC_Mutiply;

				m_fOSCAxis_Gap[1] = m_fOSC_Power;
			}
		}

		else
		{
			m_fOSCAxis_Gap[1] -= Engine::CTimer_Manager::Get_Instance()->Get_DeltaTime(L"Timer_Fps_60_2") * (m_fOSC_Frequency);

			if (m_fOSCAxis_Gap[1] <= 0)
			{
				////cout << "??GFGFG?" << endl;

				m_bOSCOrigin = false;
				m_fOSC_Power *= m_fOSC_Mutiply;

				m_bOSCReverse[1] = false;
			}
		}
	}

	else if (false == m_bOSCReverse[1])
	{
		if (false == m_bOSCOrigin)
		{
			m_fOSCAxis_Gap[1] -= Engine::CTimer_Manager::Get_Instance()->Get_DeltaTime(L"Timer_Fps_60_2") * (m_fOSC_Frequency);

			if (m_fOSCAxis_Gap[1] <= -m_fOSC_Power)
			{
				////cout << "???" << endl;

				m_bOSCOrigin = true;
				m_fOSC_Frequency *= m_fOSC_Mutiply;

				m_fOSCAxis_Gap[1] = -m_fOSC_Power;
			}
		}

		else
		{
			m_fOSCAxis_Gap[1] += Engine::CTimer_Manager::Get_Instance()->Get_DeltaTime(L"Timer_Fps_60_2") * (m_fOSC_Frequency);

			if (m_fOSCAxis_Gap[1] >= 0)
			{
				////cout << "!!!" << endl;

				m_bOSCOrigin = false;
				m_fOSC_Power *= m_fOSC_Mutiply;

				m_bOSCReverse[1] = true;
			}
		}
	}
}

void CCamera::Camera_Oscillate_FOV()
{
}

HRESULT CCamera::SetUp_ViewMat()
{
	D3DXMatrixLookAtLH(&m_MatView, &m_pTransform->Get_Pos(), &m_pTransform->Get_At(), &WORLD_UP);

	_v3 vRight, vUp, vLook;
	vLook = m_pTransform->Get_At() - m_pTransform->Get_Pos();
	D3DXVec3Normalize(&vLook, &vLook);

	D3DXVec3Cross(&vRight, &vUp, &vLook);
	D3DXVec3Normalize(&vRight, &vRight);

	D3DXVec3Cross(&vUp, &vLook, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);

	m_pTransform->Set_Axis(vLook, AXIS_Z);
	m_pTransform->Set_Axis(vRight, AXIS_X);
	m_pTransform->Set_Axis(vUp, AXIS_Y);

	return S_OK;
}

HRESULT CCamera::SetUp_ProjMat()
{
	D3DXMatrixPerspectiveFovLH(&m_MatProj, D3DXToRadian(m_fFov), WINCX / (float)WINCY, m_fNear, m_fFar);

	return S_OK;
}

void CCamera::Set_CameraMode(CameraMode _CameraMode)
{
	m_eCamMode = _CameraMode;
}

void CCamera::Set_CameraViewType(CameraView _CameraViewType)
{
	m_eCamView = _CameraViewType;
	m_bGetMp = false;
	Change_Type_Option(_CameraViewType);
}

void CCamera::Set_MouseControl(_bool _bMouseControl)
{
	m_bMouseControl = _bMouseControl;
}

void CCamera::Set_OnAimingTarget(_bool _bOnAim)
{
	m_bOnAiming = _bOnAim;

	//m_bOnAiming = (m_bOnAiming == true ? false : true);

	m_vOldPos = m_pTransform->Get_Pos();
	m_vOldAt = m_pTransform->Get_At();

	m_bOnLerpPos = true;
	m_bOnLerpAt = true;

	m_fAtLerpValue = 0.f;
	m_fPosLerpValue = 0.f;

	//if (false == m_bOnAiming)
	//{
	//	m_fY_LockAngle = 22.5f;
	//}
}

void CCamera::Set_AimUI(_bool _bAimUI)
{
	m_bAimUI = _bAimUI;
}

void CCamera::Add_At(_float _fSpeed, _v3 _vDir)
{
	m_pTransform->Add_At(_fSpeed, _vDir);
}

void CCamera::Add_Pos(_float _fSpeed, _v3 _vDir)
{
	m_pTransform->Add_Pos(_fSpeed, _vDir);
}

void CCamera::Change_Type_Option(CameraView _CameraViewType)
{
	switch (_CameraViewType)
	{
	case FIRST_VIEW:
	{
		//m_pTransform->Set_At(TARGET_TO_TRANS(m_pTarget)->Get_Pos() + TARGET_TO_TRANS(m_pTarget)->Get_Axis(AXIS_Z) + _v3{0,5,0});
		break;
	}
	case BACK_VIEW:
	{
		m_bMouseControl = true;
		m_fDistance = 2.f;
		m_pTarget = CManagement::Get_Instance()->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);
		break;
	}
	}
}

_float CCamera::Get_YAngle()
{
	return m_fY_LockAngle;
}

_float CCamera::Get_XAngle()
{
	return m_fX_LockAngle;
}

HRESULT CCamera::SetUp_ViewType(CameraView _CameraViewType)
{
	switch (_CameraViewType)
	{
	case FIRST_VIEW:
	{
		//switch (m_eCamMode)
		//{
		//case DEFAULT_MODE:
		//{
		//	CTransform* TargetTans = TARGET_TO_TRANS(m_pTarget);
		//
		//	_v3 TargetPos = TargetTans->Get_Pos();
		//	_v3 TargetPosGap = TargetPos - TargetTans->Get_Origin();
		//
		//	m_pTransform->Set_Pos({ TargetPos.x, TargetPos.y + 5 ,TargetPos.z });
		//	m_pTransform->Add_At(TargetPosGap);
		//}
		//}
		//
		//break;
	}
	case THIRD_VIEW:
	{
		break;
	}

	case CINEMA_VIEW:
	{
		//Tps_Aiming();

		if (m_eOSCType != OSC_END)
		{
			//Tps_Aiming();

			_v3 vDir, vRight_2;

			_v3 vEyePos = m_pTransform->Get_Pos();
			_v3 vAt = m_pTransform->Get_At();

			vDir = WORLD_LOOK;
			CALC::V3_Cross_Normal(&vRight_2, &_v3{ 0,1,0 }, &vDir);

			_v3 vRight_NoY = _v3{ m_pTransform->Get_Axis(AXIS_X).x , 0.f , m_pTransform->Get_Axis(AXIS_X).z } *m_fOSCAxis_Gap[AXIS_X];
			_v3 vUP = _v3{ vRight_2.y , m_pTransform->Get_Axis(AXIS_Y).y , m_pTransform->Get_Axis(AXIS_Z).y } *m_fOSCAxis_Gap[AXIS_Y];

			vAt = vEyePos + WORLD_LOOK + vRight_NoY + vUP;
			m_pTransform->Set_At(vAt);
		}

		break;
	}

	case BACK_VIEW:
	{
		if (nullptr == m_pTarget)
			return S_OK;

		CTransform* vTrans = TARGET_TO_TRANS(m_pTarget);

		D3DXMATRIX matRotAxis;
		_v3 vDir, vRight_2, vUp_2;
		_v3 vEyePos = m_pTransform->Get_Pos();
		_v3 vRight = m_pTransform->Get_Axis(AXIS_X);
		_v3 vUp = m_pTransform->Get_Axis(AXIS_Y);
		_v3 vAt = vTrans->Get_Pos();
		_v3 vTransPos = vTrans->Get_Pos();
		_v3 vRightextra = {};
		_v3 vLerpTargetPos, vLerpTargetAt;
		_float fLerpAngle = 0.f;

		// 에이밍 중이 아닐 때
		if (false == m_bOnAiming)
		{
			Tps_Aiming();

			(m_bReverseRight ?
				vRightextra = _v3{ vRight_2.x * 2.f  , 0.f , vRight_2.z * 2.f } :
				vRightextra = _v3{ vRight_2.x * -2.f  , 0.f , vRight_2.z * -2.f });

			vEyePos = -WORLD_LOOK;
			V3_NORMAL_SELF(&vEyePos);

			vEyePos *= m_fDistance;
			
			CALC::V3_Axis_Normal(&matRotAxis, &vEyePos, &WORLD_RIGHT, m_fY_LockAngle, true);
			CALC::V3_Axis_Normal(&matRotAxis, &vEyePos, &WORLD_UP, m_fX_LockAngle, true);

			vEyePos += vTransPos + _v3{ 0,1,0 } +(vRight * m_fOSCAxis_Gap[AXIS_X]) + (WORLD_UP * m_fOSCAxis_Gap[AXIS_Y]);
			vAt = vTransPos + (WORLD_UP * 1.5f) + (vRight * m_fOSCAxis_Gap[AXIS_X]) + (WORLD_UP * m_fOSCAxis_Gap[AXIS_Y]);


			if (m_bOnLerpAt)
			{
				if (m_fAtLerpValue >= 1.f)
				{
					m_fAtLerpValue = 0.f;
					m_bOnLerpAt = false;

					vLerpTargetAt = vAt;
				}

				else
				{
					m_fAtLerpValue += Engine::CTimer_Manager::Get_Instance()->Get_DeltaTime(L"Timer_Fps_60_2") * 2.f;
					D3DXVec3Lerp(&vLerpTargetAt, &m_vOldAt, &vAt, m_fAtLerpValue);
					m_vOldAt = vLerpTargetAt;
				}
			}

			else
			{
				vLerpTargetAt = vAt;
			}

			if (m_bOnLerpPos)
			{
				if (m_fPosLerpValue >= 1.f)
				{
					m_fPosLerpValue = 0.f;
					m_bOnLerpPos = false;

					vLerpTargetPos = vEyePos;
				}

				else
				{
					m_fPosLerpValue += Engine::CTimer_Manager::Get_Instance()->Get_DeltaTime(L"Timer_Fps_60_2") * 2.f;
					D3DXVec3Lerp(&vLerpTargetPos, &m_vOldPos, &vEyePos, m_fPosLerpValue);

					//V3_NORMAL_SELF(&vLerpTargetPos);
					//vLerpTargetPos *= m_fDistance;
				}
			}

			else
			{
				vLerpTargetPos = vEyePos;
			}

			m_pTransform->Set_Pos(vLerpTargetPos);
			m_pTransform->Set_At(vLerpTargetAt);

			//cout << "카메라 x 축 : " << m_fX_LockAngle << endl;
		}

		// 에이밍 중일 때
		else if (true == m_bOnAiming)
		{
			Tps_Aiming();

			_v3 vAimAt;
			_v3 vOwnerDir = vTrans->Get_Axis(AXIS_Z);
			_v3 vOwnerRight = vTrans->Get_Axis(AXIS_X);
			_v3 vLerpTargetPos, vLerpTargetAt;

			vAimAt = TARGET_TO_TRANS(m_pAimingTarget)->Get_Pos(); // +(WORLD_UP * 1.5f);

			vEyePos = vOwnerDir *= -1.f;
			vEyePos *= m_fDistance;

			vEyePos.y = m_bAimUI ? vEyePos.y + m_fAim_YPos : vEyePos.y;

			CALC::V3_Axis_Normal(&matRotAxis, &vEyePos, &vOwnerRight, 22.5f, true);

			vOwnerRight = m_bAimUI ? vOwnerRight * m_fAim_XPosMulti : vOwnerRight * 1.f;

			vEyePos += vTransPos + _v3{ 0,1,0 } + vOwnerRight;

			if (m_bOnLerpAt)
			{
				if (m_fAtLerpValue >= 1.f)
				{
					m_fAtLerpValue = 0.f;
					m_bOnLerpAt = false;

					vLerpTargetAt = vAimAt;
				}

				else
				{
					m_fAtLerpValue += Engine::CTimer_Manager::Get_Instance()->Get_DeltaTime(L"Timer_Fps_60_2");
					D3DXVec3Lerp(&vLerpTargetAt, &m_vOldAt, &vAimAt, m_fAtLerpValue);
					m_vOldAt = vLerpTargetAt;
				}
			}

			else 
			{
				vLerpTargetAt = vAimAt;
			}

			if (m_bOnLerpPos)
			{
				if (m_fPosLerpValue >= 1.f)
				{
					m_fPosLerpValue = 0.f;
					m_bOnLerpPos = false;

					vLerpTargetPos = vEyePos;
				}
			
				else
				{
					m_fPosLerpValue += Engine::CTimer_Manager::Get_Instance()->Get_DeltaTime(L"Timer_Fps_60_2") * 2.f;
					D3DXVec3Lerp(&vLerpTargetPos, &m_vOldPos, &vEyePos, m_fPosLerpValue);
					m_vOldPos = vLerpTargetPos;
				}
			}

			else
			{
				vLerpTargetPos = vEyePos;
			}

			m_pTransform->Set_Pos(vLerpTargetPos + (vRight * m_fOSCAxis_Gap[AXIS_X]) + (WORLD_UP * m_fOSCAxis_Gap[AXIS_Y]));
			m_pTransform->Set_At(vLerpTargetAt + (vRight * m_fOSCAxis_Gap[AXIS_X]) + (WORLD_UP * m_fOSCAxis_Gap[AXIS_Y]));
		}

		break;

	}

	case TOOL_VIEW:
	{
		KeyInput();
		break;
	}
	case TOP_VIEW:
	{
		KeyInput();
		break;
	}

	case QUATER_VIEW:
	{
		CTransform* vTrans = TARGET_TO_TRANS(m_pTarget);

		D3DXMATRIX matRotAxis;
		_v3 vDir, vRight_2;
		_v3 vEyePos = m_pTransform->Get_Pos();
		_v3 vRight = m_pTransform->Get_Axis(AXIS_X);
		_v3 vUp = m_pTransform->Get_Axis(AXIS_Y);
		_v3 vAt = vTrans->Get_Pos();
		_v3 vWorldRight = { 1,0,0 };
		_v3 vWorldLook = { 0,0,1 };
		_v3 vRightextra = {};

		vEyePos = -vWorldLook;
		V3_NORMAL_SELF(&vEyePos);

		vEyePos *= m_fDistance * 2;

		CALC::V3_Axis_Normal(&matRotAxis, &vEyePos, &vWorldRight, 60.f, true);
		CALC::V3_Axis_Normal(&matRotAxis, &vEyePos, &WORLD_UP, 45.f, true);

		vEyePos += vTrans->Get_Pos();
		vAt = vTrans->Get_Pos();

		m_pTransform->Set_Pos(vEyePos);
		m_pTransform->Set_At(vAt);

		break;
	}

	}

	return S_OK;
}

HRESULT CCamera::SetUp_Zoom()
{
	//if (m_pInputMgr->KeyPressing(ENGINE::KEY_SPACE))
	//{
	//	if (m_tCamInfo.fDistance > m_fZoom_Min)
	//		m_tCamInfo.fDistance += -0.1f;
	//}
	//
	//if (GetAsyncKeyState('X'))
	//{
	//	if (m_tCamInfo.fDistance < m_fZoom_Max)
	//		m_tCamInfo.fDistance += 0.1f;
	//}

	return S_OK;
}

HRESULT CCamera::SetUp_MouseRotate()
{
	POINT mp = { 0,0 };

	if (m_bGetMp == true)
		mp = CInput_Device::Get_Instance()->Get_MouseGap();

	else if (m_bGetMp == false)
		m_bGetMp = true;

	_mat	matRot;
	_v3		vDir, vRight, tmpAt, vUp, vLook;

	D3DXMatrixIdentity(&matRot);

	if (m_eCamView != QUATER_VIEW && m_eCamView != TOOL_VIEW)
	{
		if (m_bMouseControl == false)
			return E_FAIL;

		CInput_Device::Get_Instance()->Set_MouseLock();
	}

	if (m_eCamView == FIRST_VIEW || m_eCamView == TOP_VIEW)
	{
		if (mp.x != 0)
		{
			m_fX_Angle = mp.x * 0.5f;
			m_fX_LockAngle += m_fX_Angle;

			if (m_fX_LockAngle > 360.f)
				m_fX_LockAngle -= 360.f;

			else if (m_fX_LockAngle < -360.f)
				m_fX_LockAngle += 360.f;

			////cout << m_fX_LockAngle << endl;

			vDir = m_pTransform->Get_At() - m_pTransform->Get_Pos();
			D3DXVec3Normalize(&vDir, &vDir);

			TARGET_TO_TRANS(m_pTarget)->Add_Angle(AXIS_Y, m_fX_Angle);

			m_pTransform->Add_Angle(AXIS_Y, m_fX_Angle);

			////cout << "카메라 Y : " << m_pTransform->Get_Angle(ENGINE::ANGLE_Y) << endl;

			D3DXMatrixRotationAxis(&matRot, &WORLD_UP, D3DXToRadian(m_fX_Angle));
			D3DXVec3TransformCoord(&tmpAt, &vDir, &matRot);
			tmpAt += m_pTransform->Get_Pos();

			m_pTransform->Set_At(tmpAt);
		}

		if (mp.y != 0)
		{
			if (m_pTransform->Get_Axis(AXIS_Z).z < 0.2f && m_pTransform->Get_Axis(AXIS_Z).z > 0.f)
			{
				if (m_pTransform->Get_Axis(AXIS_Z).y < -0.8f)
				{
					if (mp.y > 0.f)
						return E_FAIL;
				}

				else if (m_pTransform->Get_Axis(AXIS_Z).y > 0.8f)
				{
					if (mp.y < 0.f)
						return E_FAIL;
				}
			}

			else if (m_pTransform->Get_Axis(AXIS_Z).z > -0.2f && m_pTransform->Get_Axis(AXIS_Z).z < 0.f)
			{
				if (m_pTransform->Get_Axis(AXIS_Z).y < -0.8f)
				{
					if (mp.y > 0)
						return E_FAIL;
				}

				if (m_pTransform->Get_Axis(AXIS_Z).y > 0.8f)
				{
					if (mp.y < 0)
						return E_FAIL;
				}
			}

			m_fY_Angle = mp.y * 0.5f;
			m_fY_LockAngle += m_fY_Angle;

			m_pTransform->Add_Angle(AXIS_X, m_fY_Angle);

			vDir = m_pTransform->Get_At() - m_pTransform->Get_Pos();
			D3DXVec3Normalize(&vDir, &vDir);

			CALC::V3_Cross_Normal(&vRight, &WORLD_UP, &vDir);

			D3DXMatrixRotationAxis(&matRot, &vRight, D3DXToRadian(m_fY_Angle));
			D3DXVec3TransformCoord(&tmpAt, &vDir, &matRot);
			tmpAt += m_pTransform->Get_Pos();

			m_pTransform->Set_At(tmpAt);
		}
	}

	else if (m_eCamView == TOOL_VIEW)
	{
		if (m_bMouseControl == false)
			return E_FAIL;

		CInput_Device::Get_Instance()->Set_MouseLock(false);

		if (mp.x != 0)
		{
			m_fX_Angle = mp.x * 0.2f;
			m_fX_LockAngle += m_fX_Angle;

			vDir = m_pTransform->Get_At() - m_pTransform->Get_Pos();
			D3DXVec3Normalize(&vDir, &vDir);

			m_pTransform->Add_Angle(AXIS_Y, m_fX_Angle);

			D3DXMatrixRotationAxis(&matRot, &WORLD_UP, D3DXToRadian(m_fX_Angle));
			D3DXVec3TransformCoord(&tmpAt, &vDir, &matRot);
			tmpAt += m_pTransform->Get_Pos();

			m_pTransform->Set_At(tmpAt);
		}

		if (mp.y != 0)
		{
			if (m_pTransform->Get_Axis(AXIS_Z).z < 0.2f && m_pTransform->Get_Axis(AXIS_Z).z > 0.f)
			{
				if (m_pTransform->Get_Axis(AXIS_Z).y < -0.9f)
				{
					if (mp.y > 0.f)
						return E_FAIL;
				}

				else if (m_pTransform->Get_Axis(AXIS_Z).y > 0.9f)
				{
					if (mp.y < 0.f)
						return E_FAIL;
				}
			}

			else if (m_pTransform->Get_Axis(AXIS_Z).z > -0.2f && m_pTransform->Get_Axis(AXIS_Z).z < 0.f)
			{
				if (m_pTransform->Get_Axis(AXIS_Z).y < -0.9f)
				{
					if (mp.y > 0)
						return E_FAIL;
				}

				if (m_pTransform->Get_Axis(AXIS_Z).y > 0.9f)
				{
					if (mp.y < 0)
						return E_FAIL;
				}
			}

			m_fY_Angle = mp.y * 0.2f;
			m_fY_LockAngle += m_fY_Angle;

			m_pTransform->Add_Angle(AXIS_X, m_fY_Angle);

			vDir = m_pTransform->Get_At() - m_pTransform->Get_Pos();
			D3DXVec3Normalize(&vDir, &vDir);

			CALC::V3_Cross_Normal(&vRight, &WORLD_UP, &vDir);

			D3DXMatrixRotationAxis(&matRot, &vRight, D3DXToRadian(m_fY_Angle));
			D3DXVec3TransformCoord(&tmpAt, &vDir, &matRot);
			tmpAt += m_pTransform->Get_Pos();

			m_pTransform->Set_At(tmpAt);
		}
	}

	else if (m_eCamView != CINEMA_VIEW)
	{
		if (m_bOnAiming)
			return S_OK;

		if (mp.x != 0)
		{
			if(m_fX_LockAngle != m_fY_LateLockAngle)
				m_fY_LateLockAngle = m_fX_LockAngle;

			m_fX_Angle = mp.x * 1.f;
			m_fX_LockAngle += m_fX_Angle;

			if (m_fX_LockAngle > 360.f)
				m_fX_LockAngle -= 360.f;

			else if (m_fX_LockAngle < -360.f)
				m_fX_LockAngle += 360.f;
		}

		if (mp.y != 0)
		{
			m_fY_Angle = mp.y * 0.5f;

			if (fabs(m_fY_LockAngle) < fabs(m_fY_MaxLockAngle))
			{
				m_fY_LockAngle += m_fY_Angle;

				if (fabs(m_fY_LockAngle) >= fabs(m_fY_MaxLockAngle))
				{
					if (m_fY_LockAngle < 0)
						m_fY_LockAngle = -m_fY_MaxLockAngle;

					else if (m_fY_LockAngle > 0)
						m_fY_LockAngle = m_fY_MaxLockAngle;
				}
			}

			else if (fabs(m_fY_LockAngle) >= fabs(m_fY_MaxLockAngle))
			{
				if (m_fY_LockAngle >= m_fY_MaxLockAngle && m_fY_Angle < 0)
				{
					m_fY_LockAngle += m_fY_Angle;
				}

				if (m_fY_LockAngle <= m_fY_MaxLockAngle && m_fY_Angle > 0)
				{
					m_fY_LockAngle += m_fY_Angle;
				}
			}
		}
	}


	return S_OK;
}

HRESULT CCamera::SetUp_Default()
{
	// Proj
	m_fFov = 60.f;
	m_fNear = 0.1f;
	m_fFar = 500.f;


	// Zoom
	m_fZoom_Max = 3.3f;
	m_fZoom_Min = 0.33f;
	m_fZoom_Speed = 1.0f;
	m_fZoom_Cur = 0.f;


	// Distance
	m_fDistance = 1.5f;
	m_fMaxDist = 3.75f;
	m_fMidDist = 3.f;
	m_fMinDist = 2.f;
	m_fMinRangeDist = 1.3f;
	m_fMaxRangeDist = 2.0f;


	// Pos
	m_fEyeHeight = 0.f;
	m_fLength = 0.f;
	m_bChange = false;

	// Rotate
	m_fX_Angle = 0.f;
	m_fY_Angle = 0.f;
	m_fX_LockAngle = 0.f;
	m_fY_LockAngle = 45.f;
	m_fX_MaxLockAngle = 360.f;
	m_fY_MaxLockAngle = 70.f;

	// Position
	m_pTransform->Set_Pos({ 0,0,0 });
	m_pTransform->Set_Axis(WORLD_RIGHT, AXIS_X);
	m_pTransform->Set_Axis(WORLD_UP, AXIS_X);
	m_pTransform->Set_Axis(WORLD_LOOK, AXIS_X);
	m_pTransform->Set_At({ 0.f,0.f,1.f });

	return S_OK;
}

HRESULT CCamera::SetUp_PipeLine()
{
	m_pPipeLine = CPipeLine::Get_Instance();

	if (nullptr == m_pPipeLine)
		return E_FAIL;

	Safe_AddRef(m_pPipeLine);

	if (FAILED(m_pPipeLine->Set_Transform(D3DTS_VIEW, m_MatView)))
		return E_FAIL;

	if (FAILED(m_pPipeLine->Set_Transform(D3DTS_PROJECTION, m_MatProj)))
		return E_FAIL;

	return S_OK;
}

void CCamera::Update_PipeLine()
{
	if (FAILED(m_pPipeLine->Set_Transform(D3DTS_VIEW, m_MatView)))
		return;

	if (FAILED(m_pPipeLine->Set_Transform(D3DTS_PROJECTION, m_MatProj)))
		return;
}

_v3 * CCamera::Calc_Pos_and_At()
{
	CTransform* vTrans = TARGET_TO_TRANS(m_pTarget);

	D3DXMATRIX matRotAxis;
	_v3 vDir, vRight_2, vUp_2;
	_v3 vEyePos = m_pTransform->Get_Pos();
	_v3 vRight = m_pTransform->Get_Axis(AXIS_X);
	_v3 vUp = m_pTransform->Get_Axis(AXIS_Y);
	_v3 vAt = vTrans->Get_Pos();
	_v3 vTransPos = vTrans->Get_Pos();
	_v3 vWorldRight = { 1,0,0 };
	_v3 vWorldLook = { 0,0,1 };
	_v3 vRightextra = {};

	(m_bReverseRight ?
		vRightextra = _v3{ vRight_2.x * 2.f  , 0.f , vRight_2.z * 2.f } :
		vRightextra = _v3{ vRight_2.x * -2.f  , 0.f , vRight_2.z * -2.f });

	vEyePos = -vWorldLook;
	V3_NORMAL_SELF(&vEyePos);

	vEyePos *= 3.f;


	CALC::V3_Axis_Normal(&matRotAxis, &vEyePos, &vWorldRight, m_fY_LockAngle, true);
	CALC::V3_Axis_Normal(&matRotAxis, &vEyePos, &WORLD_UP, m_fX_LockAngle - 22.5f, true);

	vRight_2 = { 1,0,0 };
	CALC::V3_Dir_Normal(&vDir, &m_pTransform->Get_Pos(), &vTransPos);
	CALC::V3_Cross_Normal(&vRight_2, &_v3{ 0,1,0 }, &vDir);
	CALC::V3_Cross_Normal(&vUp_2, &vRight_2, &vDir);

	_v3 vRight_NoY = _v3{ m_pTransform->Get_Axis(AXIS_X).x , 0.f , m_pTransform->Get_Axis(AXIS_X).z };
	_v3 vUP = _v3{ vRight_2.y ,m_pTransform->Get_Axis(AXIS_Y).y , m_pTransform->Get_Axis(AXIS_Z).y };

	vEyePos += vTransPos + _v3{ 0,1,0 };

	CALC::V3_Dir_Normal(&vDir, &m_pTransform->Get_Pos(), &vTransPos);
	CALC::V3_Cross_Normal(&vRight_2, &_v3{ 0,1,0 }, &vDir);
	CALC::V3_Cross_Normal(&vUp_2, &vRight_2, &vDir);


	vAt = vTransPos + _v3{ vRight_2.x * +0.66f, 1.6f , vRight_2.z * -0.66f };

	m_vReturnValue[0] = vEyePos;
	m_vReturnValue[1] = vAt;

	return m_vReturnValue;
}

void CCamera::Set_Pos(_v3 _vPos)
{
	m_pTransform->Set_Pos(_vPos);
}

void CCamera::Set_ReverseRight(_bool _bReverse)
{
	m_bReverseRight = _bReverse;
}

void CCamera::Set_At(_v3 _At)
{
	m_pTransform->Set_At(_At);
}

void CCamera::Tps_Aiming()
{
	_float tmpSpeed = 0.f;

	if (m_eCamMode == AIM_MODE)
	{
		tmpSpeed = fabs(m_fDistance - m_fMinDist) * 20.f + 0.66f;

		if (m_fDistance >= m_fMinDist)
			m_fDistance -= tmpSpeed * 0.01f;
	}

	else if (m_eCamMode == RANGE_MODE)
	{
		if (m_fDistance >= m_fMaxRangeDist)
		{
			tmpSpeed = fabs(m_fDistance - m_fMaxRangeDist) * 20.f + 0.66f;
			m_fDistance -= tmpSpeed * 0.01f;

			if (m_fDistance <= m_fMaxRangeDist)
				m_fDistance = m_fMaxRangeDist;
		}

		else if (m_fDistance < m_fMaxRangeDist)
		{
			tmpSpeed = fabs(m_fDistance - m_fMaxRangeDist) * 5.f + 0.66f;
			m_fDistance += tmpSpeed * 0.01f;
		}
	}

	else if (m_eCamMode == RANGE_AIM_MODE)
	{
		tmpSpeed = fabs(m_fDistance - m_fMinRangeDist) * 20.f + 0.66f;

		if (m_fDistance >= m_fMinRangeDist)
			m_fDistance -= tmpSpeed * 0.01f;
	}

	else
	{
		tmpSpeed = fabs(m_fDistance - m_fMidDist) * 5.f + 0.66f;

		if (m_fDistance < m_fMidDist)
		{
			////cout << m_fMidDist << endl;

			m_fDistance += tmpSpeed * 0.01f;

			if (m_fDistance >= m_fMidDist)
			{
				m_fDistance = m_fMidDist;
			}
		}

		else if (m_fDistance > m_fMidDist)
		{
			m_fDistance -= tmpSpeed * 0.01f;

			if (m_fDistance <= m_fMidDist)
			{
				m_fDistance = m_fMidDist;
			}
		}
	}
}

_v3 CCamera::Get_Pos()
{
	return m_pTransform->Get_Pos();
}

_v3 CCamera::Get_LookAt()
{
	return m_pTransform->Get_At();
}

_v3 CCamera::Get_Look()
{
	return m_pTransform->Get_Axis(AXIS_Z);
}

_v3 CCamera::Get_Right()
{
	return m_pTransform->Get_Axis(AXIS_X);
}

_v3 CCamera::Get_Up()
{
	return m_pTransform->Get_Pos();
}

_mat CCamera::Get_ViewMat()
{
	return m_MatView;
}

_mat CCamera::Get_ProjMat()
{
	return m_MatProj;
}

void CCamera::KeyInput()
{
	//if (m_eCamMode != FLY_MODE)
	//	return;

	float fMoveSpeed = 0.1f;

	if (CInput_Device::Get_Instance()->Key_Up(DIK_LCONTROL))
	{
		CCameraMgr::Get_Instance()->Set_MouseCtrl(false);
	}

	if (CInput_Device::Get_Instance()->Key_Down(DIK_LCONTROL))
	{
		CCameraMgr::Get_Instance()->Set_MouseCtrl(true);
		CInput_Device::Get_Instance()->Calc_MouseLockPos();
	}

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
		Rotate_X(-fMoveSpeed * 1.f);
	}

	if (GetAsyncKeyState('F') & 0x8000)
	{
		Rotate_X(fMoveSpeed * 1.f);
	}

	if (GetAsyncKeyState('Q') & 0x8000)
	{
		Rotate_Y(-fMoveSpeed * 1.f);
	}

	if (GetAsyncKeyState('E') & 0x8000)
	{
		//cout << m_pTransform->Get_Pos() << endl;
		Rotate_Y(fMoveSpeed * 1.f);
	}

	//if (GetAsyncKeyState('C') & 0x8000)
	//{
	//	m_fFov -= fMoveSpeed;
	//}
	//
	//if (GetAsyncKeyState('V') & 0x8000)
	//{
	//	m_fFov += fMoveSpeed;
	//}

	if (CInput_Device::Get_Instance()->Key_Pressing(DIK_Z))
	{
		m_pTransform->Add_Pos(fMoveSpeed, _v3(0, 1, 0));
		m_pTransform->Add_At(fMoveSpeed, _v3(0, 1, 0));
	}

	if (CInput_Device::Get_Instance()->Key_Pressing(DIK_X))
	{
		m_pTransform->Add_Pos(-fMoveSpeed, _v3(0, 1, 0));
		m_pTransform->Add_At(-fMoveSpeed, _v3(0, 1, 0));
	}
}

_int CCamera::Update_GameObject()
{
	return 0;
}

void CCamera::LateUpdate()
{
}

void CCamera::Render()
{
}

HRESULT CCamera::Initialize()
{
	return S_OK;
}

HRESULT CCamera::LateInit()
{
	return S_OK;
}

void CCamera::Free()
{
	Safe_Release(m_pPipeLine);

	CGameObject::Free();
}
