// TestDrMemory.cpp
//

#include "stdafx.h"
#include <cor.h>
#include <corhdr.h>
#include <corhlpr.h>
#include <corerror.h>
#include <corprof.h>
#include <ICeeFileGen.h>
//#include <ImageHlp.h>
#include <mscoree.h>
#include <metahost.h>
#include <string>
#include <vector>
#include <atlbase.h>
#include <metahost.h>
#include <iostream>

using namespace std;
IMetaDataDispenserEx* pDisp;
CComPtr<ICLRMetaHost> pMetaHost;
CComPtr<ICLRRuntimeInfo> pRuntime;

void loadDispenser() {
	// Get a DotNet Runtime to load the assembly to
	static HMODULE hmod = GetModuleHandle("mscoree.dll");
	if (hmod == NULL) 
		hmod = ::LoadLibrary("mscoree.dll");
	static CLRCreateInstanceFnPtr pfnClrCreateInstance = (CLRCreateInstanceFnPtr)GetProcAddress(hmod, "CLRCreateInstance");

	pfnClrCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, (void **)&pMetaHost);
	pMetaHost->GetRuntime(L"v2.0.50727", IID_ICLRRuntimeInfo, (void **)&pRuntime);

	pRuntime->GetInterface(CLSID_CorMetaDataDispenser, IID_IMetaDataDispenserEx, (void **)&pDisp);
}

int main()
{
	string assembly = _T("TestClass.dll");
	CoInitialize(NULL);
	loadDispenser();
	HANDLE hFile = NULL;
	cout << "opening File Handle" << endl;
	hFile = ::CreateFile(assembly.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	DWORD fileSize = GetFileSize(hFile, NULL);

	cout << "Create File Mapping" << endl;
	HANDLE hmap = ::CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	cout << "MapViewOfFile" << endl;
	LPVOID mapView = ::MapViewOfFile(hmap, FILE_MAP_READ, 0, 0, 0);
	IMetaDataTables* pTables = NULL;

	// ========================================================================
	//  If the line "pDisp->OpenScopeOnMemory" is not called, drmemory 
	//  finishes its task without any problems
	// ========================================================================
	cout << "OpenScopeOnMemory - IID_IMetaDataTables" << endl;
	pDisp->OpenScopeOnMemory(mapView, fileSize, ofRead, IID_IMetaDataTables, (LPUNKNOWN*)&pTables);
	cout << "Finished OpenScopeOnMemory" << endl;

	cout << "Releasing Tables" << endl;
	if (pTables) {
		pTables->Release();
		pTables = NULL;
	}

	cout << "Closing Mapping View" << endl;
	::UnmapViewOfFile(mapView);
	cout << "Closing File Mapping Handle" << endl;
	::CloseHandle(hmap);
	cout << "Closing FileHandle" << endl;
	::CloseHandle(hFile);

	cout << "Releasing Dispenser" << endl;
	if (pDisp) {
		pDisp->Release();
		pDisp = NULL;
	}

	cout << "CoUninitialize" << endl;
	CoUninitialize();

	cout << "Finished !!! " << endl;
}



