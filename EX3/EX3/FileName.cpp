#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

//Direct3D 라이브러리 파일 포함
#pragma comment (lib,"d3d11.lib")
#pragma comment (lib,"d3dx11.lib")
#pragma comment (lib,"d3dx10.lib")

//전역 선언
IDXGISwapChain* swapchain;//스왑체인 인터페이스
ID3D11Device* dev;//Direct3D 장치 인터페이스
ID3D11DeviceContext* devcon;//Direct3D 컨텍스트

void Init3D(HWND hWnd);//Direct3D 초기화
void CleanD3D(void);//Direct3D 종료 및 메모리 해제

//WindowProc 함수 프로토타입
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//Direct3D 초기화 및 준비
void Init3D(HWND hWnd)
{
	//스왑체인 정보를 담은 구조체
	DXGI_SWAP_CHAIN_DESC scd;

	//구조체 사용하기 위해 메모리 초기화
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferCount = 1;//백버퍼 1개(+프론트 버퍼)
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//32비트 색상
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//스왑체인 사용 방법
	scd.OutputWindow = hWnd;//사용할 창 핸들
	scd.SampleDesc.Count = 4;//다중 샘플 수
	scd.Windowed = true;// 창모드-전체화면

	//스왑체인 구조체의 정보를 사용해 디바이스,컨텍스트,스왑체인 생성
	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL, NULL, NULL, NULL,
		D3D11_SDK_VERSION,
		&scd, &swapchain, &dev, NULL, &devcon);
}

//Direct3D와 COM 종료
void CleanD3D()
{
	//기존 COM 모두 종료
	swapchain->Release();
	devcon->Release();
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
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
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
		wr.right - wr.left, wr.bottom - wr.top, //너비,높이
		NULL, //부모 없음
		NULL, //메뉴 없음
		hInstance, //애플리케이션 핸들
		NULL); //다중 창 사용?

	//창을 화면에 띄움
	ShowWindow(hWnd, nCmdShow);

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
	}

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