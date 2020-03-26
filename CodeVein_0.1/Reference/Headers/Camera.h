#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CPipeLine;
class CTransform;
class ENGINE_DLL CCamera : public CGameObject
{
public:
	typedef enum Camera_Oscillation_type
	{
		ROT_OSC, POS_OSC, FOV_OSC, OSC_END

	}CAM_OSC_TYPE;

protected:
	explicit CCamera(_Device pGraphicDev);
	virtual ~CCamera();

	// 이동
public:
	void Walk_H(_float _Speed);
	void Walk_V(_float _Speed);

	// 축 회전
public:
	void Rotate_X(_float _Angle);
	void Rotate_Y(_float _Angle);
	void Rotate_Z(_float _Angle);

public:
	void Camera_Oscillatation_SetUp(_float _fDuration, _float _fFrequency, _float _fPower, _float _fMuliply, CAM_OSC_TYPE _eOSCType);
	void Camera_Oscillate();
	void Camera_Oscillate_Rotate();
	void Camera_Oscillate_Position();
	void Camera_Oscillate_FOV();

public:
	HRESULT SetUp_ViewMat();
	HRESULT SetUp_ProjMat();
	HRESULT SetUp_ViewType(CameraView _CameraViewType);
	HRESULT SetUp_Zoom();
	HRESULT SetUp_MouseRotate();
	HRESULT SetUp_Default();
	HRESULT SetUp_PipeLine();

public:
	void	Update_PipeLine();

public:
	_v3* Calc_Pos_and_At();

public:
	void Set_Pos(_v3 _vPos);
	virtual void Set_MaxDist(_float _fMaxDist) { m_fMaxDist = _fMaxDist; }
	virtual void Set_MidDist(_float _fMidDist) { m_fMidDist = _fMidDist; }
	virtual void Set_MinDist(_float _fMinDist) { m_fMinDist = _fMinDist; }

public:
	void Set_ReverseRight(_bool _bReverse);

public:
	void Set_At(_v3 _At);

public:
	void Tps_Aiming();

public:
	void Set_CameraMode(CameraMode _CameraType);
	void Set_CameraViewType(CameraView _CameraViewType);

public:
	void Set_MouseControl(_bool _bMouseControl);
	void Set_OnAimingTarget();
	void Set_AimingTarget(CGameObject* pAimingTarget) { m_pAimingTarget = pAimingTarget; }

public:
	void Add_At(_float _fSpeed, _v3 _vDir);
	void Add_Pos(_float _fSpeed, _v3 _vDir);

private:
	void Change_Type_Option(CameraView _CameraViewType);

public:
	_float		Get_XAngle();
	_float		Get_YAngle();

public:
	_v3			Get_Pos();
	_v3			Get_LookAt();
	_v3			Get_Look();
	_v3			Get_Right();
	_v3			Get_Up();

public:
	CameraView	Get_ViewType() { return m_eCamView; }
	_mat		Get_ViewMat();
	_mat		Get_ProjMat();

public:
	_bool		Get_MouseControl() { return m_bMouseControl; }

public:
	void		KeyInput();

public:
	virtual _int Update_GameObject();
	virtual void LateUpdate();
	virtual void Render();

private:
	virtual HRESULT Initialize();
	virtual HRESULT LateInit();

public:
	virtual void Free() override;

protected:
	CTransform*		m_pTransform = nullptr;

protected:
	CPipeLine*		m_pPipeLine = nullptr;

protected:	// State
	CameraClass		m_eCamClass;
	CameraMode		m_eCamMode;
	CameraView		m_eCamView;

protected:	// Device Setting
	_mat			m_MatView;
	_mat			m_MatProj;

protected:
	_v3				m_vOriginAt;
	_v3				m_vReturnValue[2];

protected:
	_float  m_fLerpTimer = 0.f;
	_v3		m_vOriginPos = {};

protected:
	_float	m_fOSC_Duration = 0.f;	// 지속 시간
	_float	m_fOSC_Frequency = 0.f;	// 주기
	_float	m_fOSC_Power = 0.f; // 강도
	_float	m_fOSC_Mutiply = 0.f;

	_float	m_fOSC_Timer[3] = { 0.f };

	_float  m_fOSCAxis_Gap[3] = { 0.f };
	_bool  m_bOSCReverse[3] = { 0 };
	_bool  m_bOSCOrigin = false;

	CAM_OSC_TYPE m_eOSCType = OSC_END;

protected:	// Camera Position Setting
	_float m_fDistance;			// Camera to target distance
	_float m_fEyeHeight;		// Camera height from target
	_float m_fLength;			// Cur camera to old camera vec3 lenth
	_bool  m_bChange;


protected:	// Zoom Setting
	_float m_fZoom_Max;
	_float m_fZoom_Min;
	_float m_fZoom_Cur;
	_float m_fZoom_Speed;


protected:	// Projectile Setting
	_float m_fNear;
	_float m_fFar;
	_float m_fFov;


protected:	// Rotate Value
	_float m_fX_Angle;
	_float m_fY_Angle;
	_float m_fX_LockAngle;
	_float m_fY_LockAngle;
	_float m_fX_MaxLockAngle;
	_float m_fY_MaxLockAngle;

protected:
	_float m_fMaxDist;
	_float m_fMidDist;
	_float m_fMinDist;
	_float m_fMaxRangeDist;
	_float m_fMinRangeDist;

protected:
	_bool m_bReverseRight;

protected:
	_bool  m_bMouseControl;

protected:
	_bool m_bMainCamera;
	_bool m_bGetMp;
	_bool m_bOnAiming = false;

	// lerp things
protected:
	_v3			m_vOldAt = {};
	_v3			m_vOldPos = {};
	_float		m_fAtLerpValue = 0.f;
	_float		m_fPosLerpValue = 0.f;
	_bool		m_bOnLerpAt = false;
	_bool		m_bOnLerpPos = false;

protected:
	CGameObject*	m_pAimingTarget = nullptr;
};

END

