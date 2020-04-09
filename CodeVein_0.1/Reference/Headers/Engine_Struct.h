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

	typedef struct tagWeaponParam
	{
		_float		fDamage;

		_float		fTrail_Min;
		_float		fTrail_Max;

		_float		fCol_Height;
		_float		fRadius;
	}WPN_PARAM;

	typedef struct tagObjectParameter
	{
		_float		fHp_Cur;		// ���� HP
		_float		fHp_Max;		// �ִ� HP_Max
		_float		fStamina_Cur;	// ���� ���׹̳�
		_float		fStamina_Max;	// �ִ� ���׹̳�
		_float		fArmor_Cur;		// ���� ��
		_float		fArmor_Max;		// �ִ� ��
		_float		fHoldGage_Cur;	// ���� ������
		_float		fHoldGage_Max;	// �ִ� ������
		_float		fDamage;		// ���ݷ�

		_float		fLength;		// �浹 ������ �Ÿ�

		_bool		bDown;			// �Ѿ��� ������ ��
		_bool		bAir;			// ������ ������ ��
		_bool		bKnockBack;		// �˹� ������ ��

		_bool		bIsHit;			// �´� ��������
		_bool		bCanHit;		// ���� �� �ִ���
		_bool		bHitAgain;		// �� �´� ����

		_bool		bIsAttack;		// ���� ������.
		_bool		bCanAttack;		// ���ݰ�������

		_bool		bDodge;			// ȸ�� ��������
		_float		fDodgeTimer;	// ȸ�� �� ���� �ð�

		_v3			vHitDir;		// ���� ����

	}OBJECT_PARAM;

	typedef struct tagSkillInfo
	{	
		Skill_Index		eCurSkillIdx;			// ���� ��ų �ε���
		Skill_Index		ePreSkillIdx[4];		// �ʼ� ���� ��ų �ε���
		_ulong			dwAnimationIdx;			// ��ų �ִϸ��̼� �ε���
		_bool			bOneHand;				// ��� �Ѽ� ����
		_ulong			dwSkillCost;			// ��ų �ڽ�Ʈ

	}SKILL_INFO;

	typedef struct tagActorInfo
	{
		_float		fHP;				// ü��
		_float		fArmor;				// ��
		_float		fStamina;			// ���¹̳�
		_float		fDamage;			// ���ݷ�

		_float		fMoveSpeed_Cur;		// ���� �̵��ӵ�
		_float		fMoveSpeed_Max;		// �ִ� �̵��ӵ�
		_float		fMoveAccel_Cur;		// ���� �̵����ӵ�
		_float		fMoveAccel_Max;		// �ִ� �̵����ӵ�

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
		_v3		vPos;				// ��ġ
		_v3		vScale;				// ũ��
		_v3		vAxisDir[3];	// xyz �� ���⺤��
		_v3		vAngle;				// xyz �� ȸ�� ��

		_v3		vAt;				// ī�޶�� vAt

		_mat	matWorld;			// World

	}TRANS_INFO;

	typedef struct tagCollider
	{
		// ĸ����
		///////////////////////////////////////////////////////

		_v3		vBegin;		// ���� ��
		_v3		vEnd;			// �ٴ� ��
		_v3		vAxis;			// ��
		_float	fBeginEndLength;

		///////////////////////////////////////////////////////

		_v3  vCrossPoint;	// ���� ����

		_v3	 pGizmoPos[8];	// ������ Gizmo ��ġ
		_v3	 vCenterPos;	// �ߴ���
		_v3	 vUnderPos;		// �ϴ���

		_v3 vMaxPos;		// �ִ���
		_v3 vMinPos;		// �ּ���

		_v3 vOBB_Dir[3];	// OBB �Ͻ� xyz ���⺤��
		_v3 vLength;		// ��ĥ�� ��ģ�Ÿ�
		_v3 vRadius;		// xyz �� ������

		_bool bIsDynamic;	// �����̴� �繰����
		_bool bIsCol;		// �浹üũ �Ǿ�����
		_bool bIsTrigger;	// Ʈ��������
		_bool bIsEnabled;	// Ȱ��ȭ ����

	}COL_INFO;

	typedef struct tagRigidBody
	{
		D3DXVECTOR3 vAccel;			// ���ӵ�
		D3DXVECTOR3 vMaxAccel;		// �ִ� ���ӵ�
		D3DXVECTOR3 vSpeed;			// �ӵ�
		D3DXVECTOR3 vMaxSpeed;		// �ִ� �ӵ�

		bool bIsGravity; // �߷� ������� üũ�մϴ�.

		bool bIsGround;	 // ������ üũ �մϴ�.
		bool bIsAir;	 // �������� üũ�մϴ�.

		bool bIsHit;		// �ǰ� üũ
		bool bIsAttck;		// ���� ����üũ
		bool bIsFall;		// ���� üũ
		bool bIsJump;		// ���� üũ

		float fPower;		// ��

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
			TYPE_END };

		LPDIRECT3DTEXTURE9		pTextures[TYPE_END];
		_ulong					m_dwMaterialPass;
	}MESHTEXTURE;


	typedef struct tagD3DXMeshContainer_Derived : public D3DXMESHCONTAINER
	{
		LPD3DXMESH			pOriginalMesh;
		MESHTEXTURE*		pMeshTexture; // ����°�����ŭ ����ü�� �����Ѵ�.(�� ����¸��� �ִ� �װ��� �ؽ��ĸ� ������ �� �ֵ���)
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
}
