#pragma once

// 화면 사이즈
////////////////////////////////////////////////////////////////////////////////////////////////////
#define WINCX 1280
#define WINCY 720

////////////////////////////////////////////////////////////////////////////////////////////////////

// 오브젝트 이벤트
////////////////////////////////////////////////////////////////////////////////////////////////////
#define NO_EVENT 0
#define DEAD_OBJ 1

////////////////////////////////////////////////////////////////////////////////////////////////////

// 숫자
////////////////////////////////////////////////////////////////////////////////////////////////////
#define F_NULL 0.f
#define F_HLF  0.5f
#define F_ONE  1.f

#define MAX_STR 256

#define GRAVITY 9.80665f

////////////////////////////////////////////////////////////////////////////////////////////////////

// 기본 벡터
////////////////////////////////////////////////////////////////////////////////////////////////////
#define WORLD_UP		_v3{0.f ,1.f ,0.f}
#define WORLD_RIGHT		_v3{1.f, 0.f, 0.f}
#define WORLD_LOOK		_v3{0.f, 0.f, 1.f}

#define WORLD_UP_N		_v3{0.f, -1.f, 0.f}
#define WORLD_RIGHT_N	_v3{-1.f, 0.f, 0.f}
#define WORLD_LOOK_N	_v3{0.f, 0.f, -1.f}

#define V2_NULL			_v2{0.f, 0.f}
#define V3_NULL			_v3{0.f, 0.f, 0.f}
#define V4_NULL			_v4{0.f, 0.f, 0.f, 0.f}

#define V2_ONE			_v2{1.f, 1.f}
#define V3_ONE			_v3{1.f, 1.f, 1.f}
#define V4_ONE			_v4{1.f, 1.f, 1.f, 1.f}
////////////////////////////////////////////////////////////////////////////////////////////////////

// 벡터 계산
////////////////////////////////////////////////////////////////////////////////////////////////////
#define V3_LENGTH(_pIn)				D3DXVec3Length(_pIn)
#define V3_NORMAL(_pOut , _pIn)		D3DXVec3Normalize(_pOut, _pIn)
#define V3_NORMAL_SELF(_pOut)		D3DXVec3Normalize(_pOut, _pOut)
////////////////////////////////////////////////////////////////////////////////////////////////////

// 반복문
////////////////////////////////////////////////////////////////////////////////////////////////////
#define LOOP(_iCount)						for(_int i = 0; i < (_iCount); ++i)
#define LOOP_RANGE(_iCount , _iBeginCnt)	for(_int i = (_iBeginCnt); i < (_iCount); ++i)
#define LOOP_SUB(_iCount , _NameOfValue)	for(_int (_NameOfValue) = 0; (_NameOfValue) < (_iCount); ++i)

#define LOOP_AUTO(_STL)	for(auto& iter : (_STL))
////////////////////////////////////////////////////////////////////////////////////////////////////

// Null 체크
////////////////////////////////////////////////////////////////////////////////////////////////////
#define IF_NULL(_pointer)				if((_pointer) == nullptr)
#define IF_NOT_NULL(_pointer)			if((_pointer) != nullptr)

#define IF_NULL_RETURN(_pointer)		if((_pointer) == nullptr) { return; }
#define IF_NULL_VALUE_RETURN(_pointer, _value)		if((_pointer) == nullptr) { return _value; }
#define IF_NULL_BRAEK(_pointer)			if((_pointer) == nullptr) { break; }
#define IF_NULL_KEEP(_pointer)			if((_pointer) == nullptr) { continue; }

#define IF_NOT_NULL_RETURN(_pointer)	if((_pointer) != nullptr) { return; }
#define IF_NOT_NULL_VALUE_RETURN(_pointer, _value)		if((_pointer) == nullptr) { return _value; }
#define IF_NOT_NULL_BRAEK(_pointer)		if((_pointer) != nullptr) { break; }
#define IF_NOT_NULL_KEEP(_pointer)		if((_pointer) != nullptr) { continue; }
////////////////////////////////////////////////////////////////////////////////////////////////////

// 색상
////////////////////////////////////////////////////////////////////////////////////////////////////
#define COLOR_RED(_fAlpha)			D3DXCOLOR(1.f, 0.f, 0.f, (_fAlpha))
#define COLOR_GREEN(_fAlpha)		D3DXCOLOR(0.f, 1.f, 0.f, (_fAlpha))
#define COLOR_BLUE(_fAlpha)			D3DXCOLOR(0.f, 0.f, 1.f, (_fAlpha))

#define COLOR_LIGHT_GRAY(_fAlpha)	D3DXCOLOR(0.25f, 0.25f, 0.25f, (_fAlpha))
#define COLOR_MID_GRAY(_fAlpha)		D3DXCOLOR(0.5f, 0.5f, 0.5f, (_fAlpha))
#define COLOR_DARK_GRAY(_fAlpha)	D3DXCOLOR(0.75f, 0.75f, 0.75f, (_fAlpha))

#define COLOR_YELLOW(_fAlpha)		D3DXCOLOR(0.98f, 0.82f, 0.01f, (_fAlpha))
#define COLOR_PINK(_fAlpha)			D3DXCOLOR(1.f, 0.75f, 0.79f, (_fAlpha))
#define COLOR_PURPLE(_fAlpha)		D3DXCOLOR(0.31f, 0.14f, 0.78f, (_fAlpha))
#define COLOR_W_PURPLE(_fAlpha)		D3DXCOLOR(0.21f, 0.14f, 0.6f, (_fAlpha))
#define COLOR_ORANGE(_fAlpha)		D3DXCOLOR(0.97f, 0.6f, 0.42f, (_fAlpha))
#define COLOR_SKYBLUE(_fAlpha)		D3DXCOLOR(0.f, 0.37f, 0.92f, (_fAlpha))
#define COLOR_TEAL(_fAlpha)			D3DXCOLOR(0.48f, 0.96f, 0.96f, (_fAlpha))
#define COLOR_DARKTEAL(_fAlpha)		D3DXCOLOR(0.f, 0.59f, 0.59f, (_fAlpha))
#define COLOR_DARKBLUE(_fAlpha)		D3DXCOLOR(0.05f, 0.3f, 0.57f, (_fAlpha))
////////////////////////////////////////////////////////////////////////////////////////////////////	

#define DELTA_60 Engine::CTimer_Manager::Get_Instance()->Get_DeltaTime(L"Timer_Fps_60")

////////////////////////////////////////////////////////////////////////////////////////////////////	

#define TARGET_TO_S_MESH(_Target)	static_cast<Engine::CMesh_Static*>((_Target)->Get_Component(L"Com_MeshStatic"))
#define TARGET_TO_D_MESH(_Target)	static_cast<Engine::CMesh_Dynamic*>((_Target)->Get_Component(L"Com_MeshDynamic"))
#define TARGET_TO_TRANS(_Target)	static_cast<Engine::CTransform*>((_Target)->Get_Component(L"Com_Transform"))
#define TARGET_TO_COL(_Target)		static_cast<Engine::CCollider*>((_Target)->Get_Component(L"Com_Collider"))
#define TARGET_TO_NAV(_Target)		static_cast<Engine::CNavMesh*>((_Target)->Get_Component(L"Com_NavMesh"))
