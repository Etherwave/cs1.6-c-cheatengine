#include <bits/stdc++.h>
#include <iostream>
#include <Windows.h>
#include <Tlhelp32.h>
#include <stdio.h>
#include <time.h>
using namespace std;

void changeMoney();
bool init();
HMODULE fnGetProcessBase(DWORD PID);
DWORD GetLastErrorBox(HWND hWnd, LPSTR lpTitle) ;
uintptr_t FindDMAAddy(uintptr_t ptr, vector<unsigned int> offsets);

int money = 16000;
HWND hwnd;
//����ĵ�ַ��������һ��int
DWORD procID;
HANDLE handle;
//����ַcstrike.exe 
unsigned int BaseAddress;

void changeMoney()
{
	vector<unsigned int> offsets;
	offsets.push_back(0x11069BC);
	offsets.push_back(0x7C);
	offsets.push_back(0x1CC);
	uintptr_t addr = FindDMAAddy(BaseAddress,offsets);
	//д��ʵ�ʵ�money 
	WriteProcessMemory(handle, (LPVOID)addr, &money, sizeof(money), 0);
	//д����ʾ��money 
	WriteProcessMemory(handle, (LPVOID)(BaseAddress+0x61B9FC), &money, sizeof(money), 0);
}

void changePistolBullets()
{
	vector<unsigned int> offsets;
	offsets.push_back(0x0);
	offsets.push_back(0x7C);
	offsets.push_back(0x5DC);
	offsets.push_back(0xCC);
	uintptr_t addr = 0x25069bc;
	addr = FindDMAAddy(addr,offsets);
	int num=30;
	WriteProcessMemory(handle, (LPVOID)addr, &num, sizeof(num), 0);
}

void changeRifleBullets()
{
	vector<unsigned int> offsets;
	offsets.push_back(0x0);
	offsets.push_back(0x7C);
	offsets.push_back(0x5D8);
	offsets.push_back(0xCC);
	uintptr_t addr = 0x25069bc;
	addr = FindDMAAddy(addr,offsets);
	int num=30;
	bool state = WriteProcessMemory(handle, (LPVOID)addr, &num, sizeof(num), 0);
	if(!state)cout<<"rifle change failed!"<<endl;
}

int main()
{
	if(!init())
	{
		cout<<"�޸�����ʼ��ʧ�ܣ�"<<endl;
		Sleep(3000);
		return 0;
	}
	
	while(1)
	{
		changeMoney();//�޸Ľ�Ǯ 
		changePistolBullets();//�޸���ǹ�ӵ��� 
		changeRifleBullets();//�޸��������ӵ��� 
		Sleep(5000);//��ͣ5�룬ʵս�����ˢ��Ƶ�� 
	}
	CloseHandle(handle);

	return 0;
}


bool init()
{
	//ͨ��cs�Ľ������ֻ�ȡ���ھ�� 
	hwnd = FindWindowA(NULL, "Counter-Strike");
	if (hwnd == NULL)
	{
		cout << "There is no such a window!" << endl;
		Sleep(3000);
		return 0;
	}
	
	//��ȡ����PID 
	GetWindowThreadProcessId(hwnd, &procID);

	//��ȡ���̾�� 
	handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

	if (handle == NULL)
	{
		cout << "There is no such a process!" << endl;
		Sleep(3000);
		return 0;
	}
	
	HMODULE hModule = fnGetProcessBase(procID);
	if(hModule==NULL)
	{
		return 0;	
	}
	BaseAddress = (UINT_PTR)hModule;
	
	return 1;
}

//ͨ��PID��ȡ����ַ 
HMODULE fnGetProcessBase(DWORD PID)
{
	//��ȡ���̻�ַ
	HANDLE hSnapShot;
	//ͨ��CreateToolhelp32Snapshot���߳�ID����ȡ���̿���
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);
	if (hSnapShot == INVALID_HANDLE_VALUE)
	{
		GetLastErrorBox(NULL,"can't create Snapshot!");
		return NULL;
	}
	MODULEENTRY32 ModuleEntry32;
	ModuleEntry32.dwSize = sizeof(ModuleEntry32);
	if (Module32First(hSnapShot, &ModuleEntry32))
	{
		do 
		{
			TCHAR szExt[5];
			strcpy(szExt, ModuleEntry32.szExePath + strlen(ModuleEntry32.szExePath) - 4);
			for (int i = 0;i < 4;i++)
			{
				if ((szExt[i] >= 'a')&&(szExt[i] <= 'z'))
				{
					szExt[i] = szExt[i] - 0x20;
				}
			}
			if (!strcmp(szExt, ".EXE"))
			{
				CloseHandle(hSnapShot);
				return ModuleEntry32.hModule;
			}
		} while (Module32Next(hSnapShot, &ModuleEntry32));
	}
	CloseHandle(hSnapShot);
	return NULL;
 
}

// ��ʾ������Ϣ  
DWORD GetLastErrorBox(HWND hWnd, LPSTR lpTitle)  
{  
	LPVOID lpv;  
	DWORD dwRv;  
 
	if (GetLastError() == 0) return 0;  
 
	dwRv = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |  
		FORMAT_MESSAGE_FROM_SYSTEM,  
		NULL,  
		GetLastError(),  
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),  
		(LPSTR)&lpv,  
		0,  
		NULL);  
 
	MessageBox(hWnd, (LPCSTR)lpv, lpTitle, MB_OK);  
 
	if(dwRv)  
		LocalFree(lpv);  
 
	SetLastError(0);  
	return dwRv;  
}

uintptr_t FindDMAAddy(uintptr_t ptr, vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;
	uintptr_t t;
	for (unsigned int i = 0; i != offsets.size(); i++)
	{
		//printf("%x %x\n",addr,offsets[i]);
		addr += offsets[i];
		//printf("%x\n",addr);
		if(i<offsets.size()-1)//���һ��ֻ��ƫ���������ö�ȡ�� 
		{
			bool state = ReadProcessMemory(handle, (LPVOID)addr, &t, sizeof(t), 0);
			if(!state)cout<<"error in reading memory!"<<endl;
			addr = t;
		}
	}
	return addr;
}

