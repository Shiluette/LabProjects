#pragma once

#include "Camera.h"
#include "Mesh.h"
#include "Shader.h"

class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();

private:
	int m_nReferences;

protected:
	D3DXMATRIX m_d3dxmtxWorld;
	CMesh *m_pMesh;
	CShader *m_pShader;

public:
	void AddRef();
	void Release();

	virtual void SetMesh(CMesh *pMesh);
	virtual void SetShader(CShader *pShader);
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject();
	virtual ~CRotatingObject();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};
