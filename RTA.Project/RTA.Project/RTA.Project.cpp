// RTA.Project.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "RTA.Project.h"
#include "DxInstance.h"
#include "DxObject.h"

#define WEAPON_JOINT_INDEX 33
#define MAX_LOADSTRING 100
#define W_WIDTH 800
#define W_HEIGHT 600
using namespace DXEnvironment;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
DxInstance myDX;
DxObject cube;
DxObject grid;
DxObject teddy;

std::vector<VertexPositionColor> teddyVerts;
std::string teddyName;

DxObject lucille(DxObjType::Object);
std::vector<VertexPositionColor> lucilleVerts;
std::string lucilleName;

//std::vector<DxObject> cubes;
//unsigned int totalCubes;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	// Enable Console
	#ifndef NDEBUG
		AllocConsole();
		FILE* new_std_in_out;
		freopen_s(&new_std_in_out, "CONOUT$", "w", stdout);
		freopen_s(&new_std_in_out, "CONIN$", "r", stdin);
		std::cout << "Debug Data:\n------------\n";
	#endif


    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
	
    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_RTAPROJECT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RTAPROJECT));

    MSG msg;

    // Main message loop:
	bool isDone = false;
	bool leftDown = false;
	bool keyDown = false;
	//myDX.loadDLL((LPCWSTR)"Poop");
	
	while (!isDone)
    {
		bool msgUp = PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
		
		teddy.TimerCheck();
		//There's room for improvement on mouse/key interaction.
		if (msg.message == WM_LBUTTONDOWN && !leftDown)
		{
			leftDown = true;
			myDX.MouseInteraction(true);
		}
		else if (msg.message == WM_LBUTTONUP)
		{
			leftDown = false;
		}
		else if(leftDown && msg.message == WM_MOUSEMOVE)
		{
			myDX.MouseInteraction();
		}
		else if (msg.message == WM_QUIT)
		{
			isDone = true;
		}
		else if (msg.message == WM_KEYDOWN)
		{
			keyDown = true;
		}
		else if (msg.message == WM_KEYUP)
		{
			keyDown = false;
		}
	
		if (keyDown)
		{
			//bool test = true;
			myDX.KeyInteraction(msg.wParam);
			teddy.ProcessInput(msg.wParam);
			keyDown = false;
		}

        if (msgUp && !TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

		//cube.SetBuffers();
		//cube.SetShadersLayout();
		//myDX.runExample();
		myDX.Start();
		myDX.UpdateMVPBuffer();
		//myDX.runDLL();
		grid.DrawObject();
		cube.DrawObject();
		teddy.DrawObject();
		lucille.DrawObject();
		//for (unsigned int i = 0; i < totalCubes; ++i)
		//{
		//	cubes[i].DrawObject();
		//}
		myDX.Refresh();
    }

	//We're done, Close the DLL
	myDX.unloadDLL();

	#ifndef NDEBUG
		FreeConsole();
	#endif

    return (int) msg.wParam;
}




//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RTAPROJECT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_RTAPROJECT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable
	
	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	   CW_USEDEFAULT, 0, W_WIDTH, W_HEIGHT, nullptr, nullptr, hInstance, nullptr);
	
	if (!hWnd)
	{
	   return FALSE;
	}
	
	// Initialize DirectX here (for hwnd)
	
	myDX.InitDX(hWnd, W_WIDTH, W_HEIGHT, true); // gotta init when i get dat handle
	myDX.Start();
	myDX.CreateMVPBuffer();
	myDX.UpdateMVPBuffer();
	
	std::vector<VertexPositionColor> gridVertices;
	std::vector<XMFLOAT4X4> teddyBones;
	std::vector<int> temp;
	std::vector<int> tempLine;
	//animClip tempClip;

	
	if (myDX.loadDLL("Teddy_Idle.fbx"), true)
	{
		unsigned int indexCount = 0;
		myDX.loadFBX(teddyVerts, temp, indexCount, teddyBones, tempLine);
		teddy.SetBindPose(teddyBones);
		teddy.CalculateSkinTransforms();

		std::vector<std::vector<XMFLOAT4X4> > theClips;
		std::vector<float> times;
		float duration = 1;
		myDX.loadAnimations(theClips, times, duration);

		animClip tempClip;


		for (unsigned int i = 0; i < theClips.size(); ++i)
		{
			oneKeyFrame tempFrame;

			tempFrame.joints = theClips[i];
			tempFrame.time = times[i];

			tempClip.keyFrames.push_back(tempFrame);
		}
		tempClip.duration = duration;

		teddy.SetClip(tempClip);
		teddy.SetIndices(temp, indexCount);
		teddy.SetBones(teddyBones);
		teddy.SetBoneIndices(tempLine);

		//teddy.CalculateSkinTransforms();

		myDX.runDLL(gridVertices);
		//animClip* aclip = teddy.GetAnimClip();
		//cube.TranformObject(teddyBones[1]);
	}

	teddy.LoadObject(myDX, false, true);
	teddy.CreateBuffers(true, &teddyVerts[0], teddyVerts.size(), (unsigned int*)&teddy.GetIndices()[0], teddy.GetIndexCount());


	myDX.GetTextureName(teddyName);
	teddy.SetTextureName(teddyName);
	teddy.LoadTexture();  //needs texture name.

	myDX.unloadDLL();

	std::vector<XMFLOAT4X4> lucilleBones;
	std::vector<int> temp2;
	std::vector<int> tempLine2;
	//animClip tempClip;
	
	if (myDX.loadDLL("bat_tri.fbx", false))
	{
		unsigned int indexCount = 0;
		myDX.loadFBX(lucilleVerts, temp2, indexCount, lucilleBones, tempLine2);

		int index = 0;
		for (unsigned int x = 0; x < lucilleVerts.size(); x++)
		{
			lucilleVerts[x].debugIndex = WEAPON_JOINT_INDEX;
			lucilleVerts[x].debugBool = Object;
		}
		
		lucille.AssignJoint(WEAPON_JOINT_INDEX);
	}
	
	lucille.LoadObject(myDX, false, false);
	lucille.CreateBuffers(true, &lucilleVerts[0], lucilleVerts.size(), nullptr, NULL);
	
	myDX.GetTextureName(lucilleName);
	lucille.SetTextureName(lucilleName);
	lucille.LoadTexture();


	cube.LoadObject(myDX, true);
	cube.CreateBuffers();

	

	//Test Data
	std::cout << "Teddy total vertices: " << teddyVerts.size() << std::endl << std::endl;
	std::cout << "Teddy first vertex: " << teddyVerts[0].pos.x << " " << teddyVerts[0].pos.y << " " << teddyVerts[0].pos.z << std::endl << std::endl;
	std::cout << "Teddy total indices: " << teddy.GetIndexCount() << std::endl << std::endl;
	std::cout << "Teddy first index: " << teddy.GetIndices()[0] << std::endl << std::endl;

	grid.LoadObject(myDX, true);
	grid.CreateBuffers(true, &gridVertices[0], gridVertices.size(), gridIndices, ARRAYSIZE(gridIndices));
	
	//I/O Test
	//std::string binName = "TeddyATTACK2.bin";
	//teddy.SaveBin(binName.c_str(), binName.c_str(), binName.size());
	//char* testHeader = new char[binName.size()];
	//teddy.LoadBin(binName.c_str(), testHeader, binName.size());
	//delete[] testHeader;
	// End Directx code

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
