#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#define SCREEN_WIDTH 800
#define	SCREEN_HEIGHT 600

//Direct3D ���̺귯�� ���� ����
#pragma comment (lib,"d3d11.lib")
#pragma comment (lib,"d3dx11.lib")
#pragma comment (lib,"d3dx10.lib")

struct VERTEX
{
	float X, Y, Z;
	D3DXCOLOR COLOR;
};

//���� ����
IDXGISwapChain* swapchain;//����ü�� �������̽�
ID3D11Device* dev;//Direct3D ��ġ �������̽�
ID3D11DeviceContext* devcon;//Direct3D ���ؽ�Ʈ
ID3D11RenderTargetView* backbuffer;
ID3D11VertexShader* pVS;
ID3D11PixelShader* pPS;
ID3D11Buffer* pVBuffer;
ID3D11InputLayout* pLayout;

void InitD3D(HWND hWnd);//Direct3D �ʱ�ȭ
void CleanD3D(void);//Direct3D ���� �� �޸� ����
void InitPipeLine();
void InitGrapics();

//WindowProc �Լ� ������Ÿ��
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//������ ������ ���� �Լ�
void InitPipeLine();

//Direct3D �ʱ�ȭ �� �غ�
void InitD3D(HWND hWnd)
{
	//����ü�� ������ ���� ����ü
	DXGI_SWAP_CHAIN_DESC scd;

	//����ü ����ϱ� ���� �޸� �ʱ�ȭ
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferCount = 1;//����� 1��(+����Ʈ ����)
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//32��Ʈ ����
	scd.BufferDesc.Width = SCREEN_WIDTH;
	scd.BufferDesc.Height = SCREEN_HEIGHT;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//����ü�� ��� ���
	scd.OutputWindow = hWnd;//����� â �ڵ�
	scd.SampleDesc.Count = 4;//���� ���� ��
	scd.Windowed = true;// â���-��üȭ��
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;//��üȭ�� ��ȯ ���

	//����ü�� ����ü�� ������ ����� ����̽�,���ؽ�Ʈ,����ü�� ����
	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL, NULL, NULL, NULL,
		D3D11_SDK_VERSION,
		&scd, &swapchain, &dev, NULL, &devcon);

	//�� ������ �ּҸ� ������
	ID3D11Texture2D* pBackBuffer;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	//�� ������ �ּҸ� ����� ���� Ÿ���� ����
	dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
	pBackBuffer->Release();

	//���� Ÿ���� �� ���۷� ����
	devcon->OMSetRenderTargets(1, &backbuffer, NULL);

	//����Ʈ ����
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = SCREEN_WIDTH;
	viewport.Height = SCREEN_HEIGHT;

	devcon->RSSetViewports(1, &viewport);

	InitPipeLine();
	InitGrapics();
}

//Direct3D�� COM ����
void CleanD3D()
{
	//â���� ��ȯ
	swapchain->SetFullscreenState(FALSE, NULL);

	//���� COM ��� ����
	pLayout->Release();
	pVS->Release();
	//pPS->Release();
	pVBuffer->Release();
	swapchain->Release();
	backbuffer->Release();
	dev->Release();
	devcon->Release();
}

//���� �������� ������ �ϴ� �Լ�
void RenderFrame()
{
	//�� ���۸� ���� �Ķ�������
	devcon->ClearRenderTargetView(backbuffer, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f));

		//ǥ���� ���� ���� ����
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);

		//������� �⺻ ���� ����
		devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//���� ���۸� ����ۿ� �׸�
		devcon->Draw(3, 0);

	//����Ʈ�� ����� ��ȯ
	swapchain->Present(0, 0);
}

//Windows ���α׷� ������
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//�Լ��� ä���� ������ �ڵ�
	HWND hWnd;

	//�� ����ü�� ������ Ŭ������ ���� ������ ����
	WNDCLASSEX wc;

	//������ Ŭ������ ����ϱ� ���� �ʱ�ȭ
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	//����ü�� �ʿ��� ������ ä��
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = L"WindowClass1";

	//������ Ŭ���� ���
	RegisterClassEx(&wc);

	//��ġxy, ũ��xy
	RECT wr = { 0,0,500,400 };

	//Ŭ���̾�Ʈ ũ�⸦ �������� â ũ�� ������
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	//â�� �����ϰ� �ڵ�ó�� ���
	hWnd = CreateWindowEx(NULL,
		L"WindowClass1",//������ Ŭ���� ��
		L"Our First Windowed Program",//â �̸�
		WS_OVERLAPPEDWINDOW,//������ ��Ÿ��
		300, 300, //xy��ġ
		SCREEN_WIDTH, SCREEN_HEIGHT, //�ʺ�,����
		NULL, //�θ� ����
		NULL, //�޴� ����
		hInstance, //���ø����̼� �ڵ�
		NULL); //���� â ���?

	//â�� ȭ�鿡 ���
	ShowWindow(hWnd, nCmdShow);

	//ȭ�� �ʱ�ȭ
	InitD3D(hWnd);

	//���η��� ����

	//�� ����ü�� ������ �̺�Ʈ �޼����� ����
	MSG msg = { 0 };

	//���� ���� ����
	while (true)
	{
		//ť�� �޼����� �ִ��� Ȯ��
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			//Ű �Է� �޼����� �ùٸ� �������� ��ȯ
			TranslateMessage(&msg);

			//WindowProc �Լ��� �޼��� ����
			DispatchMessage(&msg);

			//���� �޼������� Ȯ��
			if (msg.message == WM_QUIT) break;//���� Ż��
		}
		else
		{
			//�����ڵ�
		}
		RenderFrame();
	}
	//�޸� ����
	CleanD3D();

	//WM_QUIT �޼����� �� �κ��� windows�� ��ȯ
	return msg.wParam;
}

//���α׷��� ���� ���� �޼��� �ڵ鷯
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//�־��� �޼����� ���� ������ �ڵ� ����, ã��
	switch (message)
	{
		//�� �޼����� �����찡 ������ ����
	case WM_DESTROY:
	{
		//���ø����̼��� ������ ����
		PostQuitMessage(0);
		return 0;
	}
	break;
	}

	//
	return DefWindowProc(hWnd, message, wParam, lParam);
}

//������ ���������� �Լ�
void InitPipeLine()
{
	/*HRESULT D3DX11CompileFromFile(LPCTSTR pSrcFile, D3D10_SHADER_MACRO * pDefines, LPD3D10INCLUDE pInclude, LPCSTR pFunctionName, LPCSTR pProfile, UINT Flag1, UINT Flag2, ID3DX11ThreadPump * Pump,ID3D10Blob * *ppShader, ID3D10Blob * *ppErrorMags, HRESULT * pHResult)*/

	//Vertext Shder, Pixel Shader
	ID3D10Blob* VS, * PS;
	D3DX11CompileFromFile(L"Shader.shader", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, 0, 0);
	D3DX11CompileFromFile(L"Shader.shader", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, 0, 0);

	//���̴��� ��ü�� ĸ��ȭ
	dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
	dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

	devcon->VSSetShader(pVS, 0, 0);
	devcon->PSSetShader(pPS, 0, 0);

	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0}
	};

	/*HRESULT CreatInputLayout(D3D11_INPUT_ELEMENT_DESC * pInputElementDescs,
		UINT NumElements, void* pShaderBytecodeWithInputSignature,
		SIZE_T BytecodeLength, ID3D11InputLayout * *pInputLayOut);*/
	dev->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
	devcon->IASetInputLayout(pLayout);
}

void InitGrapics()
{
	//����ü �̿��� �ﰢ�� ����
	VERTEX OurVertices[] =
	{
		{0.0f,0.5f,0.0f,D3DXCOLOR(1.0f,0.0f,0.0f,1.0f)},
		{0.45f,-0.5f,0.0f,D3DXCOLOR(0.0f,1.0f,0.0f,1.0f)},
		{-0.45f,-0.5f,0.0f,D3DXCOLOR(0.0f,0.0f,1.0f,1.0f)}
	};

	//���� ���� ����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;//CPU����, GPU �б� �׼���
	bd.ByteWidth = sizeof(VERTEX) * 3;//������ 3�� ũ��� ����
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;//������ ������ ���� ������
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//CPU�� ���ۿ� ���� ����

	dev->CreateBuffer(&bd, NULL, &pVBuffer);//���� ����

	//GPU�ʿ��� ������ ���ۿ� �����ϱ�
	D3D11_MAPPED_SUBRESOURCE ms;
	
	devcon->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);//���� ����
	memcpy(ms.pData, OurVertices, sizeof(OurVertices));//���� ������ ����
	devcon->Unmap(pVBuffer, NULL);

}