#include "Gizmo.h"

IMPLEMENT_SINGLETON(CGizmo)

CGizmo::CGizmo()
	: m_bEnableGizmo(false)
{
}


CGizmo::~CGizmo()
{
	//Free();
}

void CGizmo::Ready_Gizmo(_Device pGraphicDev)
{
	//_pGraphicDev = pGraphicDev;
	//Safe_AddRef(_pGraphicDev);

	//if (m_pShpereBuffer == nullptr)
	//	m_pShpereBuffer = static_cast<CVIBuffer*>(Engine::CResourcesMgr::GetInstance()->Clone(ENGINE::RESOURCE_STATIC, L"Buffer_Sphere"));
}

void CGizmo::Draw_Vertex(_Device _pGraphicDev, _v3 _vVertex, _v3 _vSize)
{
	if (!m_bEnableGizmo)
		return;

	_pGraphicDev->SetTexture(0, NULL);
	_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	_mat matWorld, matScale, matPos;
	D3DXMatrixIdentity(&matWorld);

	D3DXMatrixScaling(&matScale, _vSize.x , _vSize.y , _vSize.z);
	D3DXMatrixTranslation(&matPos, _vVertex.x, _vVertex.y, _vVertex.z);

	matWorld = matScale * matPos;

	_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	//m_pShpereBuffer->Render_Buffer(CVIBuffer::RENDER_SPHERE);

#ifdef TOOL_PROJECT
	//_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

#ifdef  CLIENT_PROJECT
	//_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
#endif // CLIENT_PROJECT
#endif // TOOL_PROJECT
}

void CGizmo::Draw_Sphere(_Device _pGraphicDev, _v3 _vVertex, _float _fRadius)
{
	if (!m_bEnableGizmo)
		return;

	_pGraphicDev->SetTexture(0, NULL);
	_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	_pGraphicDev->SetFVF(VTXFVF_COL);

	_mat matView, matViewInverse;
	_mat matWorld, matScale, matPos;

	_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixIdentity(&matViewInverse);
	D3DXMatrixIdentity(&matWorld);

	matViewInverse = matView; 

	//matViewInverse._11 = matView._11;
	//matViewInverse._13 = matView._13;
	//matViewInverse._31 = matView._31;
	//matViewInverse._33 = matView._33;

	D3DXMatrixInverse(&matViewInverse , NULL , &matViewInverse);

	memcpy(&matViewInverse._41, _vVertex, sizeof(D3DXVECTOR3));

	D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
	D3DXMatrixTranslation(&matPos, _vVertex.x, _vVertex.y, _vVertex.z);

	matWorld = matScale * matPos;

	VTX_COL pVtxCol_Z[21] = {};
	VTX_COL pVtxCol_X[21] = {};
	VTX_COL pVtxCol_Y[21] = {};

	_v3	vVertex = {};

	_float fRadian = D3DXToRadian(360.f / 20);


	vVertex = { _fRadius, 0.f, 0.f };
	LOOP(21)
	{
		// 반지름 10개 생성
		pVtxCol_Z[i].vPos = vVertex;
		pVtxCol_Z[i].dwColor = COLOR_GREEN(1.f);
		CALC::V3_RotZ(&vVertex, &vVertex, fRadian);
	}

	vVertex = { 0.f, 0.f, _fRadius };
	LOOP(21)
	{
		// 반지름 10개 생성
		pVtxCol_X[i].vPos = vVertex;
		pVtxCol_X[i].dwColor = COLOR_GREEN(1.f);
		CALC::V3_RotX(&vVertex, &vVertex, fRadian);
	}

	vVertex = { 0.f, 0.f, _fRadius };
	LOOP(21)
	{
		// 반지름 10개 생성
		pVtxCol_Y[i].vPos = vVertex;
		pVtxCol_Y[i].dwColor = COLOR_GREEN(1.f);
		CALC::V3_RotY(&vVertex, &vVertex, fRadian);
	}

	_pGraphicDev->SetTransform(D3DTS_WORLD, &(matViewInverse));

	_pGraphicDev->DrawPrimitiveUP(D3DPT_LINESTRIP, 20, pVtxCol_Z, sizeof(VTX_COL));

	_pGraphicDev->SetTransform(D3DTS_WORLD, &(matWorld));

	_pGraphicDev->DrawPrimitiveUP(D3DPT_LINESTRIP, 20, pVtxCol_Z, sizeof(VTX_COL));
	_pGraphicDev->DrawPrimitiveUP(D3DPT_LINESTRIP, 20, pVtxCol_X, sizeof(VTX_COL));
	_pGraphicDev->DrawPrimitiveUP(D3DPT_LINESTRIP, 20, pVtxCol_Y, sizeof(VTX_COL));

	_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

void CGizmo::Draw_Triangle_Line(_Device _pGraphicDev, _v3 * _vVertex, _bool _bSelect , CELL_PARAM _eParam)
{
	if (!m_bEnableGizmo)
		return;

	_pGraphicDev->SetTexture(0, NULL);
	_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	_mat matView, matProj, matWorld, Trans , matTmp;

	_v3 vAB[2];
	_v3 vBC[2];
	_v3 vCA[2];
	_v3 vTriangle[4];
	_v3 vTransVtx[3];

	_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	//_pGraphicDev->GetTransform(D3DTS_WORLD, &matWorld);

	D3DXMatrixIdentity(&Trans);
	D3DXMatrixIdentity(&matTmp);

	LOOP(3)
	{
		D3DXVec3TransformCoord(&vTransVtx[i], &_vVertex[i], &matView);

		if (vTransVtx[i].z <= 0.01f)
			vTransVtx[i].z = 0.01f;

		D3DXVec3TransformCoord(&vTransVtx[i], &vTransVtx[i], &matProj);
	}


	if (_bSelect == false)
	{
		LOOP(3)
			vTriangle[i] = vTransVtx[i];
		vTriangle[3] = vTransVtx[0];
	}

	else
	{
		vAB[0] = vTransVtx[0];
		vAB[1] = vTransVtx[1];

		vBC[0] = vTransVtx[1];
		vBC[1] = vTransVtx[2];

		vCA[0] = vTransVtx[2];
		vCA[1] = vTransVtx[0];
	}


	ID3DXLine *Line;
	D3DXCreateLine(_pGraphicDev, &Line);

	(_bSelect ? Line->SetWidth(5) : Line->SetWidth(3));
	Line->SetAntialias(true);

	Line->Begin();

	if (_bSelect == false)
	{
		//if(_eParam == NORMAL)
		//	Line->DrawTransform(vTriangle, 4, &matTmp, COLOR_DARKTEAL(1.f));
		//
		//if (_eParam == WATER)
		//	Line->DrawTransform(vTriangle, 4, &matTmp, COLOR_DARKTEAL(1.f));
		//
		//if (_eParam == EVENT_TRIGGER)
		//	Line->DrawTransform(vTriangle, 4, &matTmp, COLOR_DARKTEAL(1.f));
		//
		//if (_eParam == CINEMA_TRIGGER)
			Line->DrawTransform(vTriangle, 4, &matTmp, COLOR_DARKTEAL(1.f));
	}

	else
	{
		Line->DrawTransform(vAB, 2, &matTmp, COLOR_RED(1.f));
		Line->DrawTransform(vBC, 2, &matTmp, COLOR_GREEN(1.f));
		Line->DrawTransform(vCA, 2, &matTmp, COLOR_BLUE(1.f));
	}

	Line->End();

	Line->Release();

	_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CGizmo::Draw_Triangle(_Device _pGraphicDev, VTX_COL * _vVertex)
{
	if (!m_bEnableGizmo)
		return;

	_pGraphicDev->SetTexture(0, NULL);
	_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	_pGraphicDev->SetFVF(VTXFVF_COL);

	_pGraphicDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, _vVertex, sizeof(VTX_COL));

	_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
}

void  CGizmo::Draw_AABB(_Device _pGraphicDev, const _v3* _vVertex, _v3 _vPos, _v3 _vSize)
{
	if (!m_bEnableGizmo)
		return;

	_pGraphicDev->SetTexture(0, NULL);
	_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	_pGraphicDev->SetFVF(VTXFVF_COL);

	_mat matWorld, matScale, matPos;
	D3DXMatrixIdentity(&matWorld);

	D3DXMatrixScaling(&matScale, _vSize.x, _vSize.y, _vSize.z);
	D3DXMatrixTranslation(&matPos, _vPos.x, _vPos.y, _vPos.z);

	matWorld = matScale * matPos;

	_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	VTX_COL pVtxCol[8] = {};

	LOOP(8)
	{
		pVtxCol[i].vPos = _vVertex[i];
		pVtxCol[i].dwColor = COLOR_GREEN(1.f);
	}

	POLYGON32 wIdx[12] = {};

	wIdx[0]._0 = 0;
	wIdx[0]._1 = 1;
	wIdx[0]._2 = 2;

	wIdx[1]._0 = 0;
	wIdx[1]._1 = 2;
	wIdx[1]._2 = 3;

	wIdx[2]._0 = 6;
	wIdx[2]._1 = 5;
	wIdx[2]._2 = 4;

	wIdx[3]._0 = 6;
	wIdx[3]._1 = 4;
	wIdx[3]._2 = 7;

	wIdx[4]._0 = 5;
	wIdx[4]._1 = 0;
	wIdx[4]._2 = 4;

	wIdx[5]._0 = 0;
	wIdx[5]._1 = 3;
	wIdx[5]._2 = 4;

	wIdx[6]._0 = 1;
	wIdx[6]._1 = 6;
	wIdx[6]._2 = 7;

	wIdx[7]._0 = 1;
	wIdx[7]._1 = 7;
	wIdx[7]._2 = 2;

	wIdx[8]._0 = 7;
	wIdx[8]._1 = 4;
	wIdx[8]._2 = 2;

	wIdx[9]._0 = 4;
	wIdx[9]._1 = 3;
	wIdx[9]._2 = 2;

	wIdx[10]._0 = 5;
	wIdx[10]._1 = 6;
	wIdx[10]._2 = 1;

	wIdx[11]._0 = 5;
	wIdx[11]._1 = 1;
	wIdx[11]._2 = 0;

	_pGraphicDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 8, 12, wIdx, D3DFMT_INDEX32, pVtxCol, sizeof(VTX_COL));

	_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

#ifdef TOOL_PROJECT
	_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

#ifdef  CLIENT_PROJECT
	_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
#endif // CLIENT_PROJECT
#endif // TOOL_PROJECT
}

void  CGizmo::Draw_OBB(_Device _pGraphicDev, const _v3* _vVertex, _v3 vRotate, _v3 _vPos, _v3 _vSize)
{
	if (!m_bEnableGizmo)
		return;

	_pGraphicDev->SetTexture(0, NULL);
	_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	_pGraphicDev->SetFVF(VTXFVF_COL);

	_mat matWorld, matScale, matPos;
	_mat matRot[3];

	D3DXMatrixIdentity(&matWorld);

	D3DXMatrixScaling(&matScale, _vSize.x, _vSize.y, _vSize.z);

	D3DXMatrixRotationX(&matRot[AXIS_X], vRotate[AXIS_X]);
	D3DXMatrixRotationY(&matRot[AXIS_Y], vRotate[AXIS_Y]);
	D3DXMatrixRotationZ(&matRot[AXIS_Z], vRotate[AXIS_Z]);

	D3DXMatrixTranslation(&matPos, _vPos.x, _vPos.y, _vPos.z);

	matWorld = matScale * matRot[AXIS_X] * matRot[AXIS_Y] * matRot[AXIS_Z] * matPos;

	_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	VTX_COL pVtxCol[8] = {};

	LOOP(8)
	{
		pVtxCol[i].vPos = _vVertex[i];
		pVtxCol[i].dwColor = COLOR_PINK(1.f);
	}

	POLYGON32 wIdx[12] = {};

	wIdx[0]._0 = 0;
	wIdx[0]._1 = 1;
	wIdx[0]._2 = 2;

	wIdx[1]._0 = 0;
	wIdx[1]._1 = 2;
	wIdx[1]._2 = 3;

	wIdx[2]._0 = 6;
	wIdx[2]._1 = 5;
	wIdx[2]._2 = 4;

	wIdx[3]._0 = 6;
	wIdx[3]._1 = 4;
	wIdx[3]._2 = 7;

	wIdx[4]._0 = 5;
	wIdx[4]._1 = 0;
	wIdx[4]._2 = 4;

	wIdx[5]._0 = 0;
	wIdx[5]._1 = 3;
	wIdx[5]._2 = 4;

	wIdx[6]._0 = 1;
	wIdx[6]._1 = 6;
	wIdx[6]._2 = 7;

	wIdx[7]._0 = 1;
	wIdx[7]._1 = 7;
	wIdx[7]._2 = 2;

	wIdx[8]._0 = 7;
	wIdx[8]._1 = 4;
	wIdx[8]._2 = 2;

	wIdx[9]._0 = 4;
	wIdx[9]._1 = 3;
	wIdx[9]._2 = 2;

	wIdx[10]._0 = 5;
	wIdx[10]._1 = 6;
	wIdx[10]._2 = 1;

	wIdx[11]._0 = 5;
	wIdx[11]._1 = 1;
	wIdx[11]._2 = 0;

	_pGraphicDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 8, 12, wIdx, D3DFMT_INDEX32, pVtxCol, sizeof(VTX_COL));

	_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

void	CGizmo::Draw_Capsule(_Device _pGraphicDev, const _v3 _vVertex, _float _fRadius, _float _fMaxHeight)
{
	if (!m_bEnableGizmo)
		return;

	_pGraphicDev->SetTexture(0, NULL);
	_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	_pGraphicDev->SetFVF(VTXFVF_COL);

	_mat matWorld, matScale, matPos;
	D3DXMatrixIdentity(&matWorld);

	D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
	D3DXMatrixTranslation(&matPos, _vVertex.x, _vVertex.y, _vVertex.z);

	matWorld = matScale * matPos;

	_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	VTX_COL pVtxCol_Z[23] = {};
	VTX_COL pVtxCol_X[23] = {};
	VTX_COL pVtxCol_Y_Top[21] = {};
	VTX_COL pVtxCol_Y_Mid[21] = {};
	VTX_COL pVtxCol_Y_Bot[21] = {};
	_v3	vVertex = { 0.f, _fRadius, 0.f };
	_float fRadian = D3DXToRadian(360.f / 20);
	_float fInnerHeight = _fMaxHeight - (_fRadius * 2);

	vVertex = { 0.f, 0.f, _fRadius };
	LOOP(11)
	{
		// 반지름 10개 생성
		pVtxCol_Z[i].vPos = vVertex - _v3{ 0.f, fInnerHeight * 0.5f ,0.f };
		pVtxCol_Z[i].dwColor = COLOR_GREEN(1.f);
		CALC::V3_RotX(&vVertex, &vVertex, fRadian);
	}

	// 수직 선
	pVtxCol_Z[11].vPos = pVtxCol_Z[10].vPos + _v3{ 0.f, fInnerHeight, 0.f };
	pVtxCol_Z[11].dwColor = COLOR_GREEN(1.f);

	LOOP(11)
	{
		//반지름 10개 생성
		pVtxCol_Z[12 + i].vPos = vVertex + _v3{ 0.f, fInnerHeight * 0.5f ,0.f };
		pVtxCol_Z[12 + i].dwColor = COLOR_GREEN(1.f);
		CALC::V3_RotX(&vVertex, &vVertex, fRadian);
	}

	pVtxCol_Z[22].vPos = pVtxCol_Z[21].vPos - _v3{ 0.f, fInnerHeight, 0.f };
	pVtxCol_Z[22].dwColor = COLOR_GREEN(1.f);



	vVertex = { _fRadius, 0.f, 0.f };
	LOOP(11)
	{
		// 반지름 10개 생성
		pVtxCol_X[i].vPos = vVertex + _v3{ 0.f, fInnerHeight * 0.5f ,0.f };
		pVtxCol_X[i].dwColor = COLOR_GREEN(1.f);
		CALC::V3_RotZ(&vVertex, &vVertex, fRadian);
	}

	// 수직 선
	pVtxCol_X[11].vPos = pVtxCol_X[10].vPos - _v3{ 0.f, fInnerHeight, 0.f };
	pVtxCol_X[11].dwColor = COLOR_GREEN(1.f);

	LOOP(11)
	{
		//반지름 10개 생성
		pVtxCol_X[12 + i].vPos = vVertex - _v3{ 0.f, fInnerHeight * 0.5f ,0.f };
		pVtxCol_X[12 + i].dwColor = COLOR_GREEN(1.f);
		CALC::V3_RotZ(&vVertex, &vVertex, fRadian);
	}

	pVtxCol_X[22].vPos = pVtxCol_X[21].vPos + _v3{ 0.f, fInnerHeight, 0.f };
	pVtxCol_X[22].dwColor = COLOR_GREEN(1.f);


	vVertex = { 0.f, 0.f, _fRadius };
	LOOP(21)
	{
		pVtxCol_Y_Top[i].vPos = vVertex + _v3{ 0.f, fInnerHeight * 0.5f ,0.f };;
		pVtxCol_Y_Top[i].dwColor = COLOR_GREEN(1.f);
		CALC::V3_RotY(&vVertex, &vVertex, fRadian);
	}

	vVertex = { 0.f, 0.f, _fRadius };
	LOOP(21)
	{
		pVtxCol_Y_Mid[i].vPos = vVertex;
		pVtxCol_Y_Mid[i].dwColor = COLOR_GREEN(1.f);
		CALC::V3_RotY(&vVertex, &vVertex, fRadian);
	}

	vVertex = { 0.f, 0.f, _fRadius };
	LOOP(21)
	{
		pVtxCol_Y_Bot[i].vPos = vVertex - _v3{ 0.f, fInnerHeight * 0.5f ,0.f };;
		pVtxCol_Y_Bot[i].dwColor = COLOR_GREEN(1.f);
		CALC::V3_RotY(&vVertex, &vVertex, fRadian);
	}

	_pGraphicDev->DrawPrimitiveUP(D3DPT_LINESTRIP, 20, pVtxCol_Y_Top, sizeof(VTX_COL));
	_pGraphicDev->DrawPrimitiveUP(D3DPT_LINESTRIP, 20, pVtxCol_Y_Mid, sizeof(VTX_COL));
	_pGraphicDev->DrawPrimitiveUP(D3DPT_LINESTRIP, 20, pVtxCol_Y_Bot, sizeof(VTX_COL));

	_pGraphicDev->DrawPrimitiveUP(D3DPT_LINESTRIP, 22, pVtxCol_Z, sizeof(VTX_COL));
	_pGraphicDev->DrawPrimitiveUP(D3DPT_LINESTRIP, 22, pVtxCol_X, sizeof(VTX_COL));

	_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
}

void CGizmo::Set_EnableGizmo()
{
	(m_bEnableGizmo == true ? m_bEnableGizmo = false : m_bEnableGizmo = true);
}

void CGizmo::Draw_XYZ(_Device _pGraphicDev, _v3 _vPos, _v3 _vLook, _v3 _vRight, _v3 _vUp)
{
	if (!m_bEnableGizmo)
		return;

	_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	_mat matView, matProj, matWorld, Trans;

	_v3 vPoint_X[2];
	_v3 vPoint_Z[2];
	_v3 vPoint_Y[2];
	_v3 vTarget[2];

	_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	_pGraphicDev->GetTransform(D3DTS_WORLD, &matWorld);

	D3DXMatrixIdentity(&Trans);

	Trans = matView * matProj;

	vPoint_X[0] = _vPos;
	vPoint_X[1] = _vPos + _vRight * 5;

	vPoint_Y[0] = _vPos;
	vPoint_Y[1] = _vPos + _vUp * 5;

	vPoint_Z[0] = _vPos;
	vPoint_Z[1] = _vPos + _vLook * 5;


	ID3DXLine *Line;
	D3DXCreateLine(_pGraphicDev, &Line);
	Line->SetWidth(2);
	Line->SetAntialias(true);

	Line->Begin();
	Line->DrawTransform(vPoint_X, 2, &Trans, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
	Line->DrawTransform(vPoint_Y, 2, &Trans, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	Line->DrawTransform(vPoint_Z, 2, &Trans, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));
	Line->DrawTransform(vTarget, 2, &Trans, D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f));
	Line->End();

	Line->Release();

	_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
}

void CGizmo::Free(void)
{
	//Safe_Release(m_pShpereBuffer);
}
