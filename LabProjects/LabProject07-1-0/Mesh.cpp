﻿#include "stdafx.h"
#include "Mesh.h"

CMesh::CMesh(ID3D11Device *pd3dDevice)
{
	m_nStride = sizeof(CVertex);
	m_nOffset = 0;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_nReferences = 0;

	//05-1
	m_pd3dRasterizerState = NULL;

	//07
	m_pd3dIndexBuffer = NULL;
	m_nIndices = 0;
	m_nStartIndex = 0;
	m_nBaseVertex = 0;
}

CMesh::~CMesh()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();

	//05-1
	if (m_pd3dRasterizerState) m_pd3dRasterizerState->Release();

	//07
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();

}

void CMesh::AddRef()
{
	m_nReferences++;
}

void CMesh::Release()
{
	m_nReferences--;
	if (m_nReferences <= 0) delete this;
}

void CMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	//07
	if (m_pd3dVertexBuffer) pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pd3dVertexBuffer, &m_nStride, &m_nOffset);
	//인덱스 버퍼가 있으면 인덱스 버퍼를 디바이스 컨텍스트에 연결한다.
	if (m_pd3dIndexBuffer) pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pd3dDeviceContext->IASetPrimitiveTopology(
		m_d3dPrimitiveTopology);
	if (m_pd3dRasterizerState) pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

	/*인덱스 버퍼가 있으면 인덱스 버퍼를 사용하여 메쉬를 렌더링하고 없으면 정점 버퍼만을 사용하여 메쉬를 렌더링한다.*/
	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexed(m_nIndices, m_nStartIndex, m_nBaseVertex);
	else
		pd3dDeviceContext->Draw(m_nVertices, m_nOffset);
}

//05-1
void CMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
}

//052
CTriangleMesh::CTriangleMesh(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{
	m_nVertices = 3;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	/*정점(삼각형의 꼭지점)의 색상은 시계방향 순서대로 빨간색, 녹색, 파란색으로 지정한다. D3DXCOLOR 매크로는 RGBA(Red, Green, Blue, Alpha) 4개의 파라메터를 사용하여 색상을 표현하기 위하여 사용한다. 각 파라메터는 0.0~1.0 사이의 실수값을 가진다.*/
	CDiffusedVertex pVertices[3];
	pVertices[0] = CDiffusedVertex(D3DXVECTOR3(0.0f, 0.5f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[1] = CDiffusedVertex(D3DXVECTOR3(0.5f, -0.5f, 0.0f), D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
	pVertices[2] = CDiffusedVertex(D3DXVECTOR3(-0.5f, -0.5f, 0.0f), D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = m_nStride * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dVertexBuffer);

	CreateRasterizerState(pd3dDevice);
}

CTriangleMesh::~CTriangleMesh()
{
}

void CTriangleMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CMesh::Render(pd3dDeviceContext);
}

//05-1
void CTriangleMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	//래스터라이저 단계에서 컬링(은면 제거)을 하지 않도록 래스터라이저 상태를 생성한다.
	d3dRasterizerDesc.CullMode = D3D11_CULL_NONE;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

//071
CCubeMesh::CCubeMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice)
{
	m_nVertices = 8;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	//프리미티브 유형을 삼각형 스트립(Triangle Strip)으로 설정한다.
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	CDiffusedVertex pVertices[8];
	pVertices[0] = CDiffusedVertex(D3DXVECTOR3(-fx, -fy, -fz), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[1] = CDiffusedVertex(D3DXVECTOR3(-fx, -fy, +fz), D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
	pVertices[2] = CDiffusedVertex(D3DXVECTOR3(+fx, -fy, +fz), D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));
	pVertices[3] = CDiffusedVertex(D3DXVECTOR3(+fx, -fy, -fz), D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	pVertices[4] = CDiffusedVertex(D3DXVECTOR3(-fx, +fy, -fz), D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f));
	pVertices[5] = CDiffusedVertex(D3DXVECTOR3(-fx, +fy, +fz), D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f));
	pVertices[6] = CDiffusedVertex(D3DXVECTOR3(+fx, +fy, +fz), D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));
	pVertices[7] = CDiffusedVertex(D3DXVECTOR3(+fx, +fy, -fz), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(CDiffusedVertex) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dVertexBuffer);

	/*삼각형 스트립(Triangle Strip)의 경우 각 삼각형은 마지막 2개의 정점과 새로운 정점 1개를 사용하여 구성된다. 그리고 삼각형의 와인딩 순서는 시계방향과 반시계방향이 번갈아 나타나야 한다. 반시계방향의 정점들은 Direct3D에서 내부적으로 시계방향으로 바뀌게 된다. 직육면체를 삼각형 스트립으로 표현할 때 가능한 적은 수의 인덱스를 사용하는 하나의 방법은 <그림 2>와 같이 16개의 인덱스를 사용하는 것이다. Direct3D에서 삼각형을 그릴 때 삼각형의 세 꼭지점이 기하적으로 삼각형을 나타낼 수 없으면 그려지지 않게 된다. 예를 들어, 두 개 이상의 점이 동일한 점이거나 세 개의 점이 하나의 직선 위에 있는 경우에는 삼각형이 그려지지 않는다(Degenerated Triangle). 또한 삼각형 스트립에서 시계방향의 삼각형을 그리는 순서에 반시계방향의 정점으로 구성된 삼각형이 주어지면 그려지지 않는다. 반시계방향의 경우에 시계방향의 정점으로 구성된 삼각형이 주어질 때도 그려지지 않을 것이다.*/
	m_nIndices = 16;
	UINT pIndices[16];
	pIndices[0] = 5; //5,6,4 - cw
	pIndices[1] = 6; //6,4,7 - ccw
	pIndices[2] = 4; //4,7,0 - cw
	pIndices[3] = 7; //7,0,3 - ccw
	pIndices[4] = 0; //0,3,1 - cw
	pIndices[5] = 3; //3,1,2 - ccw
	pIndices[6] = 1; //1,2,3 - cw 
	pIndices[7] = 2; //2,3,7 - ccw
	pIndices[8] = 3; //3,7,2 - cw – Degenerated Index (3)
	pIndices[9] = 7; //7,2,6 - ccw
	pIndices[10] = 2;//2,6,1 - cw
	pIndices[11] = 6;//6,1,5 - ccw
	pIndices[12] = 1;//1,5,0 - cw
	pIndices[13] = 5;//5,0,4 - ccw
	pIndices[14] = 0;
	pIndices[15] = 4;

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT) * m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);

	CreateRasterizerState(pd3dDevice);
}

//06
CCubeMesh::~CCubeMesh()
{
}

//06
void CCubeMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	//0710
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;

	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

//06
void CCubeMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CMesh::Render(pd3dDeviceContext);
}
