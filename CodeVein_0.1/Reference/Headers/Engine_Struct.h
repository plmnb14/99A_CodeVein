#pragma once

namespace Engine
{
	// Buffer
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const _ulong VTXFVF_COL = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0;
	const _ulong VTXFVF_TEX = D3DFVF_XYZ | D3DFVF_TEX1;
	const _ulong VTXFVF_CUBE = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);
	const _ulong VTXFVF_MESH = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

	const _ulong VTXFVF_TEX_N = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
	const _ulong VTXFVF_CUBE_N = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);
	const _ulong FVF_SCREEN = D3DFVF_XYZRHW | D3DFVF_TEX1;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	typedef struct tagActorInfo
	{
		_float		fHP;				// 체력
		_float		fArmor;				// 방어도
		_float		fStamina;			// 스태미나
		_float		fDamage;			// 공격력

		_float		fMoveSpeed_Cur;		// 현재 이동속도
		_float		fMoveSpeed_Max;		// 최대 이동속도
		_float		fMoveAccel_Cur;		// 현재 이동가속도
		_float		fMoveAccel_Max;		// 최대 이동가속도

	}ACTOR_INFO;

	typedef struct tagPlayerInfo : public tagActorInfo
	{

	}PLAYER_INFO;

	typedef	struct tagVertexColor
	{
		_v3			vPos;
		_ulong		dwColor;

	}VTX_COL;

	typedef struct tagVertexStatic
	{
		_v3 vPos;
		_v3 vNormal;
		_v3 vTangent;
		_v3 vBiNormal;
		_v2 vTexUV;

	}VTX_STATIC;

	typedef struct tagVertexTexture
	{
		_v3			vPosition;
		_v2		 	vTexUV;
	}VTXTEX;

	typedef	struct tagVertexNormalTexture
	{
		_v3			vPosition;
		_v3			vNormal;
		_v2		 	vTexUV;

	}VTXNORTEX;

	typedef	struct tagVertexCubeTexture
	{
		_v3			vPosition;
		_v3		 	vTexUV;

	}VTXCUBE;

	typedef struct tagViewPortVertex
	{
		_v4				vPosition;
		_v2				vTexUV;

	}VTXVIEWPORT;

	typedef	struct tagPolygon16
	{
		_ushort	_0, _1, _2;

	}POLYGON16;

	typedef	struct tagPolygon32
	{
		_ulong	_0, _1, _2;

	}POLYGON32;

	typedef struct tagRaycast
	{
		_v3		vRayPos;
		_v3		vRayDir;
	}RAY;

	typedef struct tagTrail
	{
		_v3		m_vBeginPos;
		_v3		m_vEndPos;

	}TRAIL_INFO;

	typedef struct tagNavLine
	{
		_v2		m_vPoint[2];
		_v2		m_vDir;
		_v2		m_vNor;

	}NAV_LINE;

	typedef struct tagTransform
	{
		_v3		vPos;				// 위치
		_v3		vScale;				// 크기
		_v3		vAxisDir[3];	// xyz 축 방향벡터
		_v3		vAngle;				// xyz 축 회전 값

		_v3		vAt;				// 카메라용 vAt

		_mat	matWorld;			// World

	}TRANS_INFO;

	typedef struct tagCollider
	{
		// 캡슐용
		///////////////////////////////////////////////////////

		_v3		vBegin;		// 시작 점
		_v3		vEnd;			// 바닥 점
		_v3		vAxis;			// 축
		_float	fBeginEndLength;

		///////////////////////////////////////////////////////

		_v3  vCrossPoint;	// 교차 지점

		_v3	 pGizmoPos[8];	// 디버깅용 Gizmo 위치
		_v3	 vCenterPos;	// 중단점
		_v3	 vUnderPos;		// 하단점

		_v3 vMaxPos;		// 최대점
		_v3 vMinPos;		// 최소점

		_v3 vOBB_Dir[3];	// OBB 일시 xyz 방향벡터
		_v3 vLength;		// 겹칠시 겹친거리
		_v3 vRadius;		// xyz 축 반지름

		_bool bIsDynamic;	// 움직이는 사물인지
		_bool bIsCol;		// 충돌체크 되었는지
		_bool bIsTrigger;	// 트리거인지
		_bool bIsEnabled;	// 활성화 유무

	}COL_INFO;

	typedef struct tagRigidBody
	{
		D3DXVECTOR3 vAccel;			// 가속도
		D3DXVECTOR3 vMaxAccel;		// 최대 가속도
		D3DXVECTOR3 vSpeed;			// 속도
		D3DXVECTOR3 vMaxSpeed;		// 최대 속도

		bool bIsGravity; // 중력 영향력을 체크합니다.

		bool bIsGround;	 // 땅인지 체크 합니다.
		bool bIsAir;	 // 공중인지 체크합니다.

		bool bIsHit;		// 피격 체크
		bool bIsAttck;		// 공격 가능체크
		bool bIsFall;		// 낙하 체크
		bool bIsJump;		// 점프 체크

		float fPower;		// 힘

	}RIGID;

	typedef struct tagMeshInfo
	{
		std::wstring wstrStateKey = L"";
		std::wstring wstrImgPath = L"";
		std::wstring wstrFileName = L"";
		std::wstring	szIsDynamic = L"";

		//_tchar sztrStateKey[256] = L"";
		//_tchar sztrImgPath[256] = L"";
		//_tchar sztrFileName[256] = L"";
		//_tchar szIsDynamic[256] = L"";

	}MESH_INFO;

	typedef struct tagTexInfo
	{
		//_tchar sztrStateKey[256] = L"";
		//_tchar sztrImgPath[256] = L"";
		//_tchar sztrFileName[256] = L"";
		//_tchar szIsDynamic[256] = L"";
		//_tchar szImgCnt[256] = L"";

		std::wstring wstrStateKey = L"";
		std::wstring wstrImgPath = L"";
		std::wstring wstrFileName = L"";
		std::wstring	szIsDynamic = L"";
		std::wstring	szImgCnt = L"";

	}TEX_INFO;

	typedef struct tagPathInfo
	{
		_tchar sztrStateKey[256] = L"";
		_tchar sztrImgPath[256] = L"";
		_tchar sztrFileName[256] = L"";
		_tchar szIsDynamic[256] = L"";
		_tchar szImgCnt[256] = L"";

	}PATH_INFO;

	typedef struct tagObjInfo
	{
		_tchar szName[256] = L"";
		_tchar szTag[256] = L"";
		_tchar szLayerIdx[256] = L"";

		_tchar szIndex[256] = L"";

		_tchar szPos_X[256] = L"";
		_tchar szPos_Y[256] = L"";
		_tchar szPos_Z[256] = L"";

		_tchar szRot_X[256] = L"";
		_tchar szRot_Y[256] = L"";
		_tchar szRot_Z[256] = L"";

		_tchar szScale_X[256] = L"";
		_tchar szScale_Y[256] = L"";
		_tchar szScale_Z[256] = L"";

	}OBJ_INFO;

	typedef struct tagCellInfo
	{
		_tchar szIndex[256] = L"";

		_tchar szAB_Index[256] = L"";
		_tchar szBC_Index[256] = L"";
		_tchar szCA_Index[256] = L"";

		_tchar szVtx_A_x[256] = L"";
		_tchar szVtx_A_y[256] = L"";
		_tchar szVtx_A_z[256] = L"";

		_tchar szVtx_B_x[256] = L"";
		_tchar szVtx_B_y[256] = L"";
		_tchar szVtx_B_z[256] = L"";

		_tchar szVtx_C_x[256] = L"";
		_tchar szVtx_C_y[256] = L"";
		_tchar szVtx_C_z[256] = L"";

		_tchar szOption[256] = L"";
		_tchar szSubset[256] = L"";

		_int   intOption;

	}CELL_INFO;

	typedef struct tagMeshTexture
	{
		enum TYPE { TYPE_DIFFUSE, TYPE_NORMAL, TYPE_SPECULAR, TYPE_EMISSIVE, TYPE_END };
		LPDIRECT3DTEXTURE9		pTextures[TYPE_END];
	}MESHTEXTURE;


	typedef struct tagD3DXMeshContainer_Derived : public D3DXMESHCONTAINER
	{
		LPD3DXMESH			pOriginalMesh;
		MESHTEXTURE*		pMeshTexture; // 서브셋갯수만큼 구조체르 생성한다.(각 서브셋마다 최대 네개의 텍스쳐를 보관할 수 있도록)
		D3DXMATRIX*			pOffsetMatrices;
		D3DXMATRIX**		ppCombinedTransformationMatrices;
		D3DXMATRIX*			pRenderMatrices;
		unsigned long		dwNumBones;

	}D3DXMESHCONTAINER_DERIVED, *LPD3DXMESHCONTAINER_DERIVED;

	typedef struct D3DXFrame_Derived : public D3DXFRAME
	{
		D3DXMATRIX		CombinedTransformationMatrix;
	}D3DXFRAME_DERIVED, *LPD3DXFRAME_DERIVED;

	typedef struct tagEffectInfo
	{
		TCHAR	szName[256];
		TCHAR	szGradientName[256];
		TCHAR	szColorName[256];

		_bool	bUseRGBA;
		_bool	bStaticFrame;
		_float	fMaxFrame;
		_float	fColorIndex;
		_float	fMaskIndex;

		_float	fAnimSpeed;
		_float  fLifeTime;

		_int	iMaxCount; // For Particle

		_float  fMaxAlpha;
		_float	fAlphaSpeed;
		_float	fAlphaSpeed_Min;
		_float	fAlphaSpeed_Max;

		_bool	bUseColorTex;
		_bool	bRevColor;

		_bool	bBillBoard;
		_bool	bOnlyYRot;

		_bool	bLinearMove;
		_bool	bEaseInMove;
		_bool	bEaseOutMove;

		_bool	bFadeIn;
		_bool	bFadeOut;

		_float	fCreateDelay;
		_float	fCreateDelay_Min;
		_float	fCreateDelay_Max;

		_float	fMoveSpeed;
		_float	fMoveSpeed_Min;
		_float	fMoveSpeed_Max;

		_v3		vStartPos;
		_float	fRandStartPosRange_Min[AXIS_END];
		_float	fRandStartPosRange_Max[AXIS_END];
		_bool	bRandStartPos;

		_bool	bDistortion;
		_bool	bGravity;

		_bool	bDirMove;
		_v3		vMoveDirection;
		_bool	bRandomMove;
		_v3		vRandDirectionRange;
		_bool	bMoveWithRot;

		_bool	bRandScale;
		_bool	bScaleMove;
		_v3		vStartScale;
		_v3		vMoveScale;
		_float	fMoveScaleSpeed;

		_bool	bRotMove;
		_bool	bRandomRot;
		_v3		vRotDirection;
		_float	fRotSpeed;
		_float	fRotSpeed_Min;
		_float	fRotSpeed_Max;

		_bool	bColorMove;
		_v4		vStartColor;
		_v4		vEndColor;
		_float	fColorSpeed;
	}EFFECT_INFO;

}
