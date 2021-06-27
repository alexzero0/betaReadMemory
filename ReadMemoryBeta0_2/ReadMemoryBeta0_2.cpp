// ReadMemoryBeta0_2.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#define READALL (PAGE_READONLY | PAGE_EXECUTE | PAGE_READWRITE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE)
//PAGE_EXECUTE_READWRITE// PAGE_NOACCESS | PAGE_WRITECOPY
using namespace std;

bool bDataCompare(unsigned char* pData,unsigned char* bMask,char* szMask)
{
	for (; *szMask; ++szMask, ++pData, ++bMask)
		if (*szMask == 'x' && *pData != *bMask)
			return false;
	return (*szMask) == 0;
}

bool DataCompare(BYTE* data, BYTE* sign, char* mask)
{
	for (; *mask; mask++, sign++, data++)
	{
		if (*mask == 'x' && *data != *sign)
			return false;
	}
	return true;
}

//bool bDataCompare(const unsigned char* pData, const unsigned char* bMask, const char* szMask)
//{
//	for (; *szMask; ++szMask, ++pData, ++bMask)
//		if (*szMask == 'x' && *pData != *bMask)
//			return false;
//	return (*szMask) == 0;
//}
//unsigned long dwFindPattern(unsigned char *bMask, char * szMask, unsigned long dw_Address, unsigned long dw_Len)
//{
//	for (unsigned long i = 0; i < dw_Len; i++)
//		if (bDataCompare((unsigned char*)(dw_Address + i), bMask, szMask))
//			return (unsigned long)(dw_Address + i);
//	return 0;
//}

bool Danger(void* info)
{
	if (info == 0)
	{
		cout << " Ошибка №" << GetLastError() << endl;
		return 0;
	}
	else
	{
		cout << "Едем дальше, получен ответ : "<< info << endl;
		return info;
	}
}

int main()
{
	setlocale(LC_ALL, "rus");
	HWND WindowGame = 0;
	DWORD adres = 0xA0000000;//мой нужный адрес хранится 0xFEA21540 , поэтому изменил , что бы было ближе 0xE0000000
	DWORD PID = 0;
	HANDLE OpenProc = 0;
	MEMORY_BASIC_INFORMATION MemBIn = { 0 };
	DWORD FindPattern=0;

	WindowGame = FindWindow(0, L"Cossacks 3");
	Danger(WindowGame);

	GetWindowThreadProcessId(WindowGame, &PID);
	Danger(&PID);

	OpenProc = OpenProcess(PROCESS_ALL_ACCESS, 0, PID);
	Danger(OpenProc);
/*
	cout << "| = " << (1 | 0) << endl;//1
	cout << "| = " << (0 | 0) << endl;//0
	cout << "| = " << (1 | 1) << endl;//1
	cout << "| = " << (0 | 1) << endl;//1

	cout << "& = " << (1 & 0) << endl;//0
	cout << "& = " << (0 & 1) << endl;//0
	cout << "& = " << (1 & 1) << endl;//1
	cout << "& = " << (0 & 0) << endl;//0
*/
	DWORD HELP = 0;
	for (;; Sleep(100))
	{
		VirtualQueryEx(OpenProc, (DWORD*)adres, &MemBIn, sizeof(MEMORY_BASIC_INFORMATION));
		cout << "Сейчас на адресе : " << MemBIn.BaseAddress << endl;

		//if (MemBIn.State & (MEM_COMMIT))
		//{
		//	if (!(MemBIn.Type & MEM_PRIVATE))//MEM_MAPPED//
		//	{
		//		if ((MemBIn.AllocationProtect & READALL)&&(!(MemBIn.AllocationProtect & PAGE_NOACCESS)))
		//		{
		//			if (MemBIn.Protect & PAGE_READWRITE)
		//			{
		//if ((MemBIn.State == MEM_COMMIT)            //если регион в состоянии передачи памяти
		//	&&                                    //и
		//	(MemBIn.Protect != PAGE_READONLY)        //регион не только для чнения
		//	&&                                    //и
		//	(MemBIn.Protect != PAGE_EXECUTE_READ)    //регион не тролько с исполнением программного кода и чтением
		//	&&                                    //и
		//	(MemBIn.Protect != PAGE_GUARD)            //регион без сигнала доступа к странице
		//	&&                                    //и
		//	(MemBIn.Protect != PAGE_NOACCESS)        //регион не защищён доступом к нему
		//	)//то
		if (MemBIn.State != MEM_FREE)
		{
			cout << "Незащищенный адрес : " << MemBIn.BaseAddress << endl;
			BYTE* Mass = new BYTE[MemBIn.RegionSize];
			ReadProcessMemory(OpenProc, MemBIn.BaseAddress, Mass, MemBIn.RegionSize, NULL);

			for (unsigned long i = 0; i < MemBIn.RegionSize; i++)
			{
				if (DataCompare(Mass + i, (PBYTE)"\x00\xD6\x03\x00\x00\xE8\x03\x00\x00\xE8\x03\x00\x00\xE8\x03\x00", "xxxxxxxxxxxxxxxx"))
				{
					//delete[] Mass;
					HELP= (DWORD)MemBIn.BaseAddress + i;
					cout << "adres : " << HELP;
					break;
					Sleep(3000);
					//return HELP;
				}
			}
			delete[] Mass;
			/*FindPattern = dwFindPattern(
				(unsigned char*)"\xE8\x03\x00\x00\xE8\x03\x00\x00\xE8\x03\x00\x00\xE8\x03\x00\x00\xE8\x03\x00\x00\xE8\x03\x00\x00\xE3\x03\x00\x00\xE8\x03\x00\x00",
				"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
				(DWORD)MemBIn.BaseAddress,
				MemBIn.RegionSize);*/

			/*if (FindPattern != 0)
			{
				cout << "adres : " << FindPattern;
				break;
			}*/
		}
					/*}
				}
			}
		}*/
		adres = reinterpret_cast<DWORD>(MemBIn.BaseAddress)+MemBIn.RegionSize;
	}

	system("pause");
    return 0;
}

