#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#define SCREEN_WIDTH 800
#define	SCREEN_HEIGHT 600

//Direct3D 라이브러리 파일 포함
#pragma comment (lib,"d3d11.lib")
#pragma comment (lib,"d3dx11.lib")
#pragma comment (lib,"d3dx10.lib")

struct VERTEX
{
	float X, Y, Z;
	D3DXCOLOR COLOR;
};

//전역 선언
IDXGISwapChain* swapchain;//스왑체인 인터페이스
ID3D11Device* dev;//Direct3D 장치 인터페이스
ID3D11DeviceContext* devcon;//Direct3D 컨텍스트
ID3D11RenderTargetView* backbuffer;
ID3D11VertexShader* pVS;
ID3D11PixelShader* pPS;
ID3D11Buffer* pVBuffer;
ID3D11InputLayout* pLayout;

void InitD3D(HWND hWnd);//Direct3D 초기화
void CleanD3D(void);//Direct3D 종료 및 메모리 해제
void InitPipeLine();
void InitGrapics();

//WindowProc 함수 프로토타입
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//렌더링 파이프 라인 함수
void InitPipeLine();

//Direct3D 초기화 및 준비
void InitD3D(HWND hWnd)
{
	//스왑체인 정보를 담은 구조체
	DXGI_SWAP_CHAIN_DESC scd;

	//구조체 사용하기 위해 메모리 초기화
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferCount = 1;//백버퍼 1개(+프론트 버퍼)
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//32비트 색상
	scd.BufferDesc.Width = SCREEN_WIDTH;
	scd.BufferDesc.Height = SCREEN_HEIGHT;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//스왑체인 사용 방법
	scd.OutputWindow = hWnd;//사용할 창 핸들
	scd.SampleDesc.Count = 4;//다중 샘플 수
	scd.Windowed = true;// 창모드-전체화면
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;//전체화면 전환 허용

	//스왑체인 구조체의 정보를 사용해 디바이스,컨텍스트,스왑체인 생성
	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL, NULL, NULL, NULL,
		D3D11_SDK_VERSION,
		&scd, &swapchain, &dev, NULL, &devcon);

	//백 버퍼의 주소를 가져옴
	ID3D11Texture2D* pBackBuffer;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	//백 버퍼의 주소를 사용해 렌더 타겟을 만듬
	dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
	pBackBuffer->Release();

	//렌더 타겟을 백 버퍼로 설정
	devcon->OMSetRenderTargets(1, &backbuffer, NULL);

	//뷰포트 설정
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

//Direct3D와 COM 종료
void CleanD3D()
{
	//창모드로 전환
	swapchain->SetFullscreenState(FALSE, NULL);

	//기존 COM 모두 종료
	pLayout->Release();
	pVS->Release();
	//pPS->Release();
	pVBuffer->Release();
	swapchain->Release();
	backbuffer->Release();
	dev->Release();
	devcon->Release();
}

//단일 프레임을 렌더링 하는 함수
void RenderFrame()
{
	//백 버퍼를 진한 파란색으로
	devcon->ClearRenderTargetView(backbuffer, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f));

		//표시할 정점 버퍼 선택
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);

		//사용중인 기본 유형 선택
		devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//정점 버퍼를 백버퍼에 그림
		devcon->Draw(3, 0);

	//프론트와 백버퍼 전환
	swapchain->Present(0, 0);
}

//Windows 프로그램 진입점
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//함수로 채워진 윈도우 핸들
	HWND hWnd;

	//이 구조체는 윈도우 클래스에 대한 정보를 가짐
	WNDCLASSEX wc;

	//윈도우 클래스를 사용하기 위해 초기화
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	//구조체를 필요한 정보로 채움
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = L"WindowClass1";

	//윈도우 클래스 등록
	RegisterClassEx(&wc);

	//위치xy, 크기xy
	RECT wr = { 0,0,500,400 };

	//클라이언트 크기를 기준으로 창 크기 재조정
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	//창을 생성하고 핸들처럼 사용
	hWnd = CreateWindowEx(NULL,
		L"WindowClass1",//윈도우 클래스 명
		L"Our First Windowed Program",//창 이름
		WS_OVERLAPPEDWINDOW,//윈도우 스타일
		300, 300, //xy위치
		SCREEN_WIDTH, SCREEN_HEIGHT, //너비,높이
		NULL, //부모 없음
		NULL, //메뉴 없음
		hInstance, //애플리케이션 핸들
		NULL); //다중 창 사용?

	//창을 화면에 띄움
	ShowWindow(hWnd, nCmdShow);

	//화면 초기화
	InitD3D(hWnd);

	//메인루프 진입

	//이 구조체는 윈도우 이벤트 메세지를 가짐
	MSG msg = { 0 };

	//무한 루프 진입
	while (true)
	{
		//큐에 메세지가 있는지 확인
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			//키 입력 메세지를 올바른 형식으로 변환
			TranslateMessage(&msg);

			//WindowProc 함수에 메세지 보냄
			DispatchMessage(&msg);

			//종료 메세지인지 확인
			if (msg.message == WM_QUIT) break;//루프 탈출
		}
		else
		{
			//게임코드
		}
		RenderFrame();
	}
	//메모리 해제
	CleanD3D();

	//WM_QUIT 메세지의 이 부분을 windows에 반환
	return msg.wParam;
}

//프로그램을 위한 메인 메세지 핸들러
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//주어진 메세지에 대해 실행할 코드 정렬, 찾기
	switch (message)
	{
		//이 메세지는 윈도우가 닫힐때 읽음
	case WM_DESTROY:
	{
		//애플리케이션을 완전히 닫음
		PostQuitMessage(0);
		return 0;
	}
	break;
	}

	//
	return DefWindowProc(hWnd, message, wParam, lParam);
}

//렌더링 파이프라인 함수
void InitPipeLine()
{
	/*HRESULT D3DX11CompileFromFile(LPCTSTR pSrcFile, D3D10_SHADER_MACRO * pDefines, LPD3D10INCLUDE pInclude, LPCSTR pFunctionName, LPCSTR pProfile, UINT Flag1, UINT Flag2, ID3DX11ThreadPump * Pump,ID3D10Blob * *ppShader, ID3D10Blob * *ppErrorMags, HRESULT * pHResult)*/

	//Vertext Shder, Pixel Shader
	ID3D10Blob* VS, * PS;
	D3DX11CompileFromFile(L"Shader.shader", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, 0, 0);
	D3DX11CompileFromFile(L"Shader.shader", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, 0, 0);

	//쉐이더를 객체로 캡슐화
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
	//구조체 이용해 삼각형 생성
	VERTEX OurVertices[] =
	{
		{0.0f,0.5f,0.0f,D3DXCOLOR(1.0f,0.0f,0.0f,1.0f)},
		{0.45f,-0.5f,0.0f,D3DXCOLOR(0.0f,1.0f,0.0f,1.0f)},
		{-0.45f,-0.5f,0.0f,D3DXCOLOR(0.0f,0.0f,1.0f,1.0f)}
	};

	//정점 버퍼 생성
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;//CPU쓰기, GPU 읽기 액세스
	bd.ByteWidth = sizeof(VERTEX) * 3;//정점의 3배 크기로 설정
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;//버퍼의 유형이 정점 버퍼임
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//CPU가 버퍼에 쓰기 가능

	dev->CreateBuffer(&bd, NULL, &pVBuffer);//버퍼 생성

	//GPU쪽에서 정점을 버퍼에 복사하기
	D3D11_MAPPED_SUBRESOURCE ms;
	
	devcon->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);//버퍼 매핑
	memcpy(ms.pData, OurVertices, sizeof(OurVertices));//정점 데이터 복사
	devcon->Unmap(pVBuffer, NULL);

}