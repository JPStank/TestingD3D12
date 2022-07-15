#include "pch.h"
#include <Shlwapi.h>

#include "Application.h"
#include "Tutorial2.h"

#include <dxgidebug.h>
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")

void ReportLiveObjects()
{
	IDXGIDebug1* debug;
	DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug));

	debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_IGNORE_INTERNAL);
	debug->Release();
}

int CALLBACK wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR lpCmdLine, _In_ int nCmdShow)
{
	int retCode = 0;

	WCHAR path[MAX_PATH];
	HMODULE hModule = GetModuleHandleW(NULL);
	if (GetModuleFileNameW(hModule, path, MAX_PATH) > 0)
	{
		PathRemoveFileSpecW(path);
		SetCurrentDirectoryW(path);
	}

	Application::Create(hInstance);
	{
		std::shared_ptr<Tutorial2> demo = std::make_shared<Tutorial2>(L"Learning DirectX 12 - Lesson 2", 1280, 720);
		retCode = Application::Get().Run(demo);
	}
	Application::Destroy();

	atexit(&ReportLiveObjects);


	return retCode;
}