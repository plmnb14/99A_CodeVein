#pragma once

#define STR_128 128
#define STR_256 256

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

	typedef struct tagNewLight : public D3DLIGHT9
	{
		_bool		bDecreAlpha;
		_bool		bLifeTime;

		_float		fLifeTime_Cur;
		_float		fLifeTime_Max;
		_float		fAlpha;

	}NEW_LIGHT;

	typedef struct tagWeaponParam
	{
		_int		iWeaponType;	// 무기 타입 ex) Weapon_Sword
		_int		iWeaponName;	// 무기 이름 ex) Sword_Military

		_uint		iReinforce;		//강화 단계

		_float		fDamage;		//기본 공격력

		_float		fTrail_Min;		// 트레일 최소
		_float		fTrail_Max;		// 트레일 최대

		_float		fCol_Height;	// 충돌체 최초 위치
		_float		fRadius;		// 충돌 구 범위

	}WPN_PARAM;

	typedef struct tagRimParam
	{
		_float		fRimValue;			// 림라이트 수치
		_float		fRimAlpha;			// 림라이트 수치
		_float		fRimChangeValue;	// 림라이트 컬러 보간 수치
		_float		fRimTimer_Max;		// 림라이트 지속시간
		_float		fRimTimer_Cur;		// 림라이트 지속시간

		_float		fOrigin_RimValue;		// 림라이트 원본 강도
		_float		fOrigin_RimAlpha;		// 림라이트 원본 알파

		_v4			vRimColor_Cur;		// 현재 림라이트 색상
		_v4			vRimColor_Change;	// 바뀌길 원하는 림라이트 색상
		_v4			vRimColor_Old;		// 최초 림라이트 색상

		_bool		bDecreRim;			// 림라이트가 가능한지

	}RIM_PARAM;

	typedef struct tagObjectParameter
	{
		_float		fHp_Cur;		// 현재 HP
		_float		fHp_Max;		// 최대 HP_Max
		_float		fStamina_Cur;	// 현재 스테미나
		_float		fStamina_Max;	// 최대 스테미나

		_float		fArmor_Cur;		// 현재 방어도
		_float		fArmor_Max;		// 최대 방어도
		_float		fHoldGage_Cur;	// 현재 경직도
		_float		fHoldGage_Max;	// 최대 경직도

		_float		fDamage;		// 공격력

		_float		fLength;		// 충돌 대상과의 거리

		_float		fDodgeTimer_Cur;	// 회피 중 무적 시간
		_float		fDodgeTimer_Max;	// 회피 중 최대 무적 시간
		_float		fGuardTimer_Cur;	// 가드 중 무적 시간
		_float		fGuardTimer_Max;	// 가드 중 최대 무적 시간

		_float		fKnockBackValue;	// 넉백 파워

		_bool		bDown;			// 넘어진 상태일 때
		_bool		bAir;			// 공중인 상태일 때
		_bool		bIsKnockBack;	// 넉백 상태일 때

		_bool		bIsHit;			// 맞는 도중인지
		_bool		bCanHit;		// 맞을 수 있는지
		_bool		bHitAgain;		// 또 맞는 건지

		_bool		bIsAttack;		// 공격 중인지.
		_bool		bCanAttack;		// 공격가능한지

		_bool		bCanGuard;		// 가드 가능한 상태인지
		_bool		bIsGuard;		// 가드 가능한 상태인지

		_bool		bCanDodge;		// 회피 가능 상태인지
		_bool		bIsDodge;		// 회피 중인 상태인지

		_bool		bInvisible;		// 투명 상태인지 ( 애니메이션은 재생되지만, 메쉬 랜더는 안되고, 충돌처리도 안되는 상태 )
		_bool		bSuperArmor;	// true 일 시 넉백, 피격모션 출력, 다운 상태를 받을 수 없음

		_bool		bCanExecution;	// 처형을 할 수 있거나, 당할 수 있는 상태
		_bool		bIsExecution;	// 처형 중인 경우, 혹은 처형 당하는 중
		_bool		bIgnoreExecution;	// 처형불가 대상
			
		_bool		bCanCounter;	// 카운터 가능한지
		_bool		bIsCounter;		// 카운터 중인지

		_bool		bCanRepel;		// 반격당했는지
		_bool		bIsRepel;		// 반격당하는 중인지

		_v3			vHitDir;		// 맞은 방향
		_v3			vHitPos;		// 때린놈 위치

		EXE_WPN		eExecutionWpn;	// 처형 시 무기
		EXE_TYPE	eExecutionType;	// 처형 시 타입

		_short		sMana_Cur;		// 스킬 마나
		_short		sMana_Max;		// 스킬 마나 맥스

	}OBJECT_PARAM;

	typedef struct tagSkillInfo
	{	
		Skill_Index		eCurSkillIdx;			// 현재 스킬 인덱스
		Skill_Index		ePreSkillIdx[4];		// 필수 하위 스킬 인덱스
		_ulong			dwAnimationIdx;			// 스킬 애니메이션 인덱스
		_bool			bOneHand;				// 양손 한손 유무
		_ulong			dwSkillCost;			// 스킬 코스트

	}SKILL_INFO;

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

	typedef	struct tagVertexCubeColor
	{
		_v3			vPosition;
		_ulong		dwColor;

	}VTXCUBE_COL;

	typedef struct tagViewPortVertex
	{
		_v4			vPosition;
		_v2			vTexUV;

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
		_bool	bIsDynamic = false;

		//_tchar sztrStateKey[STR_128] = L"";
		//_tchar sztrImgPath[STR_128] = L"";
		//_tchar sztrFileName[STR_128] = L"";
		//_tchar szIsDynamic[STR_128] = L"";

	}MESH_INFO;

	typedef struct tagTexInfo
	{
		//_tchar sztrStateKey[STR_128] = L"";
		//_tchar sztrImgPath[STR_128] = L"";
		//_tchar sztrFileName[STR_128] = L"";
		//_tchar szIsDynamic[STR_128] = L"";
		//_tchar szImgCnt[STR_128] = L"";

		std::wstring wstrStateKey = L"";
		std::wstring wstrImgPath = L"";
		std::wstring wstrFileName = L"";
		_ushort sImgCnt = 0;

	}TEX_INFO;

	typedef struct tagMeshInfo_EffTool
	{
		std::wstring wstrStateKey = L"";
		std::wstring wstrImgPath = L"";
		std::wstring wstrFileName = L"";
		_tchar szIsDynamic[STR_128] = L"";

	}MESH_INFO_EFFTOOL;

	typedef struct tagTexInfo_EffTool
	{
		std::wstring wstrStateKey = L"";
		std::wstring wstrImgPath = L"";
		std::wstring wstrFileName = L"";
		_tchar szIsDynamic[STR_128] = L"";
		_tchar szImgCnt[STR_128] = L"";

	}TEX_INFO_EFFTOOL;

	typedef struct tagPathInfo
	{
		_tchar sztrStateKey[STR_128] = L"";
		_tchar sztrImgPath[STR_128] = L"";
		_tchar sztrFileName[STR_128] = L"";
		_bool	bIsDynamic = false;
		_ushort sImgCnt = 0;
		_tchar szProtoTypeName[STR_128] = L"";
		_tchar szIsDynamic[STR_128] = L"";
		_tchar szImgCnt[STR_128] = L"";

	}PATH_INFO;

	typedef struct tagObjInfo
	{
		_tchar szName[STR_128] = L"";
		_tchar szTag[STR_128] = L"";
		_tchar szLayerIdx[STR_128] = L"";

		_tchar szIndex[STR_128] = L"";

		_tchar szPos_X[STR_128] = L"";
		_tchar szPos_Y[STR_128] = L"";
		_tchar szPos_Z[STR_128] = L"";

		_tchar szRot_X[STR_128] = L"";
		_tchar szRot_Y[STR_128] = L"";
		_tchar szRot_Z[STR_128] = L"";

		_tchar szScale_X[STR_128] = L"";
		_tchar szScale_Y[STR_128] = L"";
		_tchar szScale_Z[STR_128] = L"";

	}OBJ_INFO;

	typedef struct tagCellInfo
	{
		_tchar szIndex[STR_128] = L"";

		_tchar szAB_Index[STR_128] = L"";
		_tchar szBC_Index[STR_128] = L"";
		_tchar szCA_Index[STR_128] = L"";

		_tchar szVtx_A_x[STR_128] = L"";
		_tchar szVtx_A_y[STR_128] = L"";
		_tchar szVtx_A_z[STR_128] = L"";

		_tchar szVtx_B_x[STR_128] = L"";
		_tchar szVtx_B_y[STR_128] = L"";
		_tchar szVtx_B_z[STR_128] = L"";

		_tchar szVtx_C_x[STR_128] = L"";
		_tchar szVtx_C_y[STR_128] = L"";
		_tchar szVtx_C_z[STR_128] = L"";

		_tchar szOption[STR_128] = L"";
		_tchar szSubset[STR_128] = L"";

		_int   intOption;

	}CELL_INFO;

	typedef struct tagMeshTexture
	{
		enum TYPE { 
			TYPE_DIFFUSE_MAP, 
			TYPE_NORMAL_MAP,
			TYPE_SPECULAR_MAP, 
			TYPE_EMISSIVE_MAP,
			TYPE_ROUGHNESS_MAP,
			TYPE_UNION_MAP,
			TYPE_TRANSPARENCY_MAP,
			TYPE_ID_MAP,
			TYPE_AO_MAP,
			TYPE_NR_MAP,
			TYPE_HEIGHT_MAP,
			TYPE_END 
		};

		LPDIRECT3DTEXTURE9		pTextures[TYPE_END];
		_ulong					m_dwMaterialPass;
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
		TCHAR	szName[STR_256];
		TCHAR	szGradientName[STR_256];
		TCHAR	szColorName[STR_256];

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
		_float	fDistortionPower;
		_bool	bGravity;
		_bool	bSlowly;
		_bool	bDissolve;

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

	typedef struct tagBulletInfo
	{
		tagBulletInfo(_v3 _vCreatePos, _v3 _vDir, _float _fSpeed, _double _dLifeTime)
			: vCreatePos(_vCreatePos), vDir(_vDir), fSpeed(_fSpeed), dLifeTime(_dLifeTime)
		{}

		_v3			vCreatePos = _v3(0.f, 0.f, 0.f);
		_v3			vDir = _v3(0.f, 0.f, 0.f);
		_float		fSpeed = 0.f;
		_double		dLifeTime = 0;
	}BULLET_INFO;

	// 인스턴스 데이터
	typedef struct tagInstanceData
	{
		float fRight[4];
		float fUp[4];
		float fLook[4];
		float fPos[4];
		float fColor[4];
		float fDistortion;
		float fAlpha;
		float fDissolve;
		bool bDissolve;
		bool bUseColorTex;
		bool bReverseColor;
		bool bUseRGBA;
		bool bUseMaskTex;
	}INSTANCEDATA;

	typedef struct tagArmorParam
	{
		_uint iArmorType; // 아장 타입
		_float fDef;		// 방어력
		_uint iPrice;		// 가격(상점용)
	}ARMOR_PARAM;
}
