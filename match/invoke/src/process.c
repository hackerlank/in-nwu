/*
	$ Invoke   (C) 2005-2012 MF
	$ process.c, 0.1.1124
*/

#pragma comment (lib, "Psapi.lib")

#include <Windows.h>
#include <Tlhelp32.h>
#include <Psapi.h>
#include "base.h"
#include "gui.h"


/* ���������� */
#define COUNT		1024
/* ������·�� */
#define BUFFER		(MAX_PATH * 2)


/* ������Ϣ */
static DWORD	*processPid		= NULL;
static char		**processName	= NULL;

/* �ڴ�ָ�� */
static DWORD	memoryPtr		= 0;


static char * processPe(FILE *fp) {
	IMAGE_DOS_HEADER dos;
	IMAGE_NT_HEADERS pe;

	/* ���� PE ͷ */
	if((fread(&dos, sizeof dos, 1, fp) == 0) || (dos.e_magic != IMAGE_DOS_SIGNATURE)) {
		return "�����ļ� DOS ͷʧ�ܣ���ѡ���ļ����ܲ���һ����ȷ�� EXE �ļ���";
	} else {
		fseek(fp, (long) dos.e_lfanew, SEEK_SET);
	}
	if((fread(&pe, sizeof pe, 1, fp) == 0) || (pe.Signature != IMAGE_NT_SIGNATURE)) {
		return "�����ļ� PE ͷʧ�ܣ���ѡ���ļ����ܲ���һ����ȷ�� EXE �ļ���";
	}

	/* �ж��ǲ��� DLL */
	if((pe.FileHeader.Characteristics & IMAGE_FILE_DLL) == IMAGE_FILE_DLL) {
		return "��ѡ���ļ��� DLL ���͵� PE �ļ������������д����ļ���";
	} else if((pe.FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) != IMAGE_FILE_EXECUTABLE_IMAGE) {
		return "�޷�ʶ����ѡ���ļ������б�ǡ�";
	}

	/* �ж��ǲ��� PE+ */
	switch(pe.OptionalHeader.Magic) {
		case IMAGE_NT_OPTIONAL_HDR32_MAGIC:	break;
		case IMAGE_NT_OPTIONAL_HDR64_MAGIC:	return "��ѡ���ļ��� 64 λ������ļ������������д����ļ���";
		default:							return "�޷�ʶ����ѡ���ļ������ͱ�ǡ�";
	}

	/* ���������� */
	switch(pe.FileHeader.Machine) {
		case IMAGE_FILE_MACHINE_I386:	break;
		default:						return "�޷�ʶ��������ѡ���ļ�����Ҫ�Ĵ��������͡�";
	}

	/* ��ϵͳ */
	switch(pe.OptionalHeader.Subsystem) {
		case IMAGE_SUBSYSTEM_WINDOWS_GUI:
			RT.console = 0; break;
		case IMAGE_SUBSYSTEM_WINDOWS_CUI:
			RT.console = 1; break;

		default: return "��֧��������ѡ���ļ�����Ҫ����ϵͳ��";
	}

	RT.attach		= FALSE;
	RT.uninstall	= FALSE;
	RT.entry		= pe.OptionalHeader.AddressOfEntryPoint;

	return NULL;
}

static void processInit(DWORD pid, HANDLE handle) {
	char	buffer[BUFFER];
	char	*temp, *next;

	RT.file		= "Unknown";
	RT.pid		= pid;
	RT.handle	= handle;

	/* ��ȡ���� */
	if((RT.snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid)) == NULL) {
		die("�޷���ȡ���Խ��̵ľ�����Ϣ��");
	}
	if(RT.snapshot == INVALID_HANDLE_VALUE) {
		die("�޷���ȡ���Խ��̵ľ�����Ϣ��");
	}

	/* ��ȡ��ģ���� */
	if(GetModuleFileNameEx(handle, NULL, buffer, BUFFER) == FALSE) {
		die("�޷���ȡ���Խ��̵���ģ��·����");
	} else {
		NEW(RT.module, char, strlen(buffer) + 1);

		strcpy(RT.module, buffer);

		next = RT.module;
	}

	while(temp = strstr(next, "\\")) {
		next = temp + 1;
	}

	if(*next) {
		RT.file = next;
	}
}

static DWORD processBase() {
	MODULEENTRY32 module;
	module.dwSize = sizeof module;

	do {
		if(Module32First(RT.snapshot, &module) == FALSE) {
			break;
		}

		do {
			if(strcmp(RT.module, module.szExePath) == 0) {
				RT.mainStart	= (DWORD) module.modBaseAddr;
				RT.mainEnd		= RT.mainStart + module.modBaseSize;

				return (DWORD) RT.mainStart;
			}
		} while(Module32Next(RT.snapshot, &module));
	} while(0);

	die("�޷���ȡ���Խ�����ģ�����Ϣ��");

	return 0;
}

static char * processOpen(char *file) {
	static int console = 0;

	int		run		= 1;
	DWORD	mode	= 0;

	STARTUPINFO			si;
	PROCESS_INFORMATION	pi;
	CONTEXT				reg;
	DEBUG_EVENT			debug;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	si.cb		= sizeof(STARTUPINFO);
	si.dwFlags	= STARTF_USESTDHANDLES;

	reg.ContextFlags = CONTEXT_ALL;

	/* CUI */
	if(RT.console) {
		if(console == 0) {
			if(AllocConsole() == FALSE) {
				return "��������̨����ʧ�ܡ�";
			}

			console = 1;
		}

		si.hStdInput	= GetStdHandle(STD_INPUT_HANDLE);
		si.hStdOutput	= GetStdHandle(STD_OUTPUT_HANDLE);
		si.hStdError	= GetStdHandle(STD_ERROR_HANDLE);
	}

	if(CreateProcess(NULL, file, NULL, NULL, FALSE, PROCESS_QUERY_INFORMATION | DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS, NULL, NULL, &si, &pi) == FALSE) {
		return "��������ʧ�ܡ�";
	}

	/* �����߳� */
	threadCreate(pi.dwThreadId, pi.hThread);

	/* ���� EF ��� */
	GetThreadContext(pi.hThread, &reg);
	reg.EFlags |= 0x100;
	/* ʵ�ֵ���ִ�� */
	SetThreadContext(pi.hThread, &reg);

	ResumeThread(pi.hThread);

	while(run) {
		mode = DBG_EXCEPTION_NOT_HANDLED;

		WaitForDebugEvent(&debug, INFINITE);
		GetThreadContext(pi.hThread, &reg);

		if(debug.dwDebugEventCode == EXCEPTION_DEBUG_EVENT) {
			if(debug.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_BREAKPOINT) {
				/* �ϵ��� NtContinue@ntdll.dll, for XP */
				if(run == 1) {
					reg.Dr0 = (DWORD) GetProcAddress(GetModuleHandle("ntdll.dll"), "NtContinue");
					reg.Dr7 = 0x101;

					run = 2;
				}

				mode = DBG_CONTINUE;
			} else if(debug.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_SINGLE_STEP) {
				switch(run) {
					/* ��ʼ���ж� */
					case 2:
						reg.Dr0 = reg.Eax;
						reg.Dr7 = 0x101;

						run = 3; break;

					/* ��ʼ����� */
					case 3:
						processInit(pi.dwProcessId, pi.hProcess);

						/* ���ó������ */
						reg.Dr0 = processBase() + RT.entry;
						reg.Dr7 = 0x101;

						run = 0; break;
				}

				mode = DBG_CONTINUE;
			}
		} else if(debug.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT) {
			return FALSE;
		}

		SetThreadContext(pi.hThread, &reg);
		ContinueDebugEvent(debug.dwProcessId, debug.dwThreadId, mode);
	}

	return NULL;
}

static BOOL processPrivilege(HANDLE token, char *privilege, BOOL enable) {
	DWORD				size;

	LUID				luid;
	TOKEN_PRIVILEGES	tp;
	TOKEN_PRIVILEGES	tpPrevious;

	if(LookupPrivilegeValue(NULL, privilege, &luid) == FALSE) {
		return FALSE;
	}

	size = sizeof(TOKEN_PRIVILEGES);

	tp.PrivilegeCount			= 1;
	tp.Privileges[0].Luid		= luid;
	tp.Privileges[0].Attributes	= 0;

	if(AdjustTokenPrivileges(token, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), &tpPrevious, &size) == FALSE) {
		return FALSE;
	}

	tpPrevious.PrivilegeCount		= 1;
	tpPrevious.Privileges[0].Luid	= luid;

	if(enable) {
		tpPrevious.Privileges[0].Attributes |= SE_PRIVILEGE_ENABLED;
	} else {
		tpPrevious.Privileges[0].Attributes ^= (SE_PRIVILEGE_ENABLED & tpPrevious.Privileges[0].Attributes);
	}

	return AdjustTokenPrivileges(token, FALSE, &tpPrevious, size, NULL, NULL);
}


int processList(DWORD **pid, char ***name) {
	HANDLE	handle;
	DWORD	size;
	int		i, j, count;

	if(processPid == NULL) {
		NEW(processPid,		DWORD,	COUNT);
		NEW(processName,	char *,	COUNT);
	} else {
		ZeroMemory(processPid,	sizeof(DWORD) * COUNT);
		
		for(i = 0; i < COUNT; i++) {
			DEL(processName[i]);
		}
	}

	if(EnumProcesses(processPid, COUNT, &size) == FALSE) {
		return 0;
	}

	if(size == 0) {
		return 0;
	} else {
		count = (int) size / sizeof(DWORD);
	}

	for(i = j = 0; i < count; j++) {
		size = BUFFER;

		if(i != j) {
			processPid[i] = processPid[j];
		}

		/* �򿪽��̲�ѯ */
		if((handle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processPid[i])) == NULL) {
			count--; continue;
		}

		NEW(processName[i], char, BUFFER);

		if(GetProcessImageFileName(handle, processName[i], size) == FALSE) {
			DEL(processName[i]);
		}

		/* ������·��ת��Ϊ�̷� */
		device2NtLetter(processName[i]);

		CloseHandle(handle);

		i++;
	}

	*pid	= processPid;
	*name	= processName;

	return count;
}

char * processSelect(int index) {
	DWORD	pid;
	HANDLE	token;

	/* ָ�� pid */
	pid = processPid[index];

	/* ��Ȩ */
	if(OpenProcessToken(RT.process, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token) == FALSE) {
		return "����������Ȩʧ�ܡ�";
	}
	if(processPrivilege(token, SE_DEBUG_NAME, TRUE) == FALSE) {
		CloseHandle(token);
		
		return "����������Ȩʧ�ܡ�";
	}

	/* ��Ŀ����� */
	if((RT.handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid)) == NULL) {
		CloseHandle(token);

		return "�޷�������ѡ�Ľ��̣������������Ȩ�޲��㵼�µġ�";
	}

	processPrivilege(token, SE_DEBUG_NAME, FALSE);
	CloseHandle(token);

	if(DebugActiveProcess(pid) == FALSE) {
		return "����ѡ�Ľ���ʧ�ܡ���ɵ���������ԭ���������Ϊϵͳ���̣������� 64 λ���̣���������ֹ��";
	}

	if(!confirm(NULL, "�� Invoke �˳�ʱ�Ƿ���ֹ�����ԵĽ��̣�")) {
		RT.uninstall = TRUE;

		DebugSetProcessKillOnExit(FALSE);
	}

	processInit(pid, RT.handle);

	RT.attach = TRUE;

	return NULL;
}

char * processFile() {
	FILE	*fp;
	char	*file, *error;

	if((file = selectFileOpen(NULL, "exe", "��ѡ����Ҫ���е��ļ�")) == NULL) {
		return "";
	}
	if((fp = fopen(file, "rb")) == NULL) {
		return "�޷����ļ�����ȷ���ļ��Ƿ���ڻ����ڱ�����Ӧ�ó���ʹ�á�";
	}

	error = processPe(fp);
	fclose(fp);

	if(error == NULL) {
		error = processOpen(file);
	}

	return error;
}

void processFree() {
	int i;

	for(i = 0; i < COUNT; i++) {
		DEL(processName[i]);
	}

	DEL(processPid);
	DEL(processName);
}


int processRead(DWORD addr, void *buffer, unsigned int size) {
	int read;

	if(ReadProcessMemory(RT.handle, (LPCVOID) addr, buffer, size, (SIZE_T *) &read)) {
		if(read < (int) size) {
			return 0;
		} else {
			return read;
		}
	} else {
		return 0;
	}
}

int processWrite(DWORD addr, void *buffer, unsigned int size) {
	int		written;
	DWORD	permOld, permTemp;

	if(WriteProcessMemory(RT.handle, (LPVOID) addr, buffer, size, (SIZE_T *) &written)) {
		return written;
	}

	/* ��Ȩʧ�� */
	if(VirtualProtectEx(RT.handle, (LPVOID) addr, size, PAGE_EXECUTE_READWRITE, &permOld) == FALSE) {
		return 0;
	}

	/* ��Ȩ��д�� */
	WriteProcessMemory(RT.handle, (LPVOID) addr, buffer, size, (SIZE_T *) &written);
	/* �ָ�Ȩ�� */
	VirtualProtectEx(RT.handle, (LPVOID) addr, size, permOld, &permTemp);

	return written;
}


void memoryRewind() {
	memoryPtr = 0;
}

int memorySeek(int offset, int mode){
	switch(mode) {
		case SEEK_SET: memoryPtr = (DWORD) offset; break;
		case SEEK_CUR: memoryPtr = (DWORD) ((int) memoryPtr + offset); break;
	}

	return memoryPtr;
}

int memoryTell() {
	return memoryPtr;
}

int memoryAvail(int offset) {
	if(memoryPtr < (DWORD) offset) {
		return 1;
	} else {
		return 0;
	}
}

int memoryRead(void *buffer, unsigned int size) {
	int result = processRead(memoryPtr, buffer, size);

	memorySeek((int) size, SEEK_CUR);

	return result;
}

char * memoryReadString(unsigned int max, int display) {
	char *data;
	unsigned int i;

	NEW(data, char, max);

	for(i = 0; i < max; i++) {
		if(memoryRead(data + i, sizeof(char))) {
			if(data[i] == 0) {
				return data;
			}

			if(display) {
				if((data[i] < 0x20) || (data[i] > 0x7E)) {
					data[i] = '?';
				}
			}
		}
	}

	DEL(data);

	return NULL;
}

int memoryWrite(void *buffer, unsigned int size) {
	int result = processWrite(memoryPtr, buffer, size);

	memorySeek((int) size, SEEK_CUR);

	return result;
}
