#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

//Direct3D ���̺귯�� ���� ����
#pragma comment (lib,"d3d11.lib")
#pragma comment (lib,"d3dx11.lib")
#pragma comment (lib,"d3dx10.lib")

//���� ����
IDXGISwapChain* swapchain;//����ü�� �������̽�
ID3D11Device* dev;//Direct3D ��ġ �������̽�
ID3D11DeviceContext* devcon;//Direct3D ���ؽ�Ʈ

void Init3D(HWND hWnd);//Direct3D �ʱ�ȭ
void CleanD3D(void);//Direct3D ���� �� �޸� ����

//WindowProc �Լ� ������Ÿ��
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//Direct3D �ʱ�ȭ �� �غ�
void Init3D(HWND hWnd)
{
	//����ü�� ������ ���� ����ü
	DXGI_SWAP_CHAIN_DESC scd;

	//����ü ����ϱ� ���� �޸� �ʱ�ȭ
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferCount = 1;//����� 1��(+����Ʈ ����)
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//32��Ʈ ����
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//����ü�� ��� ���
	scd.OutputWindow = hWnd;//����� â �ڵ�
	scd.SampleDesc.Count = 4;//���� ���� ��
	scd.Windowed = true;// â���-��üȭ��

	//����ü�� ����ü�� ������ ����� ����̽�,���ؽ�Ʈ,����ü�� ����
	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL, NULL, NULL, NULL,
		D3D11_SDK_VERSION,
		&scd, &swapchain, &dev, NULL, &devcon);
}

//Direct3D�� COM ����
void CleanD3D()
{
	//���� COM ��� ����
	swapchain->Release();
	devcon->Release();
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
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
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
		wr.right - wr.left, wr.bottom - wr.top, //�ʺ�,����
		NULL, //�θ� ����
		NULL, //�޴� ����
		hInstance, //���ø����̼� �ڵ�
		NULL); //���� â ���?

	//â�� ȭ�鿡 ���
	ShowWindow(hWnd, nCmdShow);

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
	}

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