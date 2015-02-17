/*
	$ Ananke PE Loader  (C) 2005-2012 mfboy
	$ process.c, 0.1.1027
*/

#include <Windows.h>
#include <Psapi.h>
#include "base.h"
#include "pe.h"


/* ������Ϣ */
process_h PROCESS;

/* �ϵ㡢�ڵ㡢������ģ������ */
bp_s		*BP			= NULL;
section_s	*SECTION	= NULL;
import_s	*IMPORT		= NULL;
module_s	*MODULE		= NULL;


static char * procRead(char *file) {
	FILE *fp;

	section_s	*section, *next;
	import_s	*import;

	IMAGE_DOS_HEADER		fileDos;
	IMAGE_NT_HEADERS		filePe;
	IMAGE_SECTION_HEADER	fileSection;
	IMAGE_IMPORT_DESCRIPTOR	fileImport;

	if((fp = fopen(file, "rb")) == NULL) {
		return "�޷�����ѡ���ļ��������ļ��Ƿ���ڻ����ӵ�з���Ȩ�ޡ�";
	}

	/* �� DOS ͷ */
	if((fread(&fileDos, sizeof fileDos, 1, fp) == 1) && (fileDos.e_magic == IMAGE_DOS_SIGNATURE)) {
		/* �ƶ��� PE ͷ */
		fseek(fp, (long) fileDos.e_lfanew, SEEK_SET);
	} else {
		return "û������ѡ���ļ����ҵ� DOS ͷ�����ļ����ܲ���һ���Ϸ��� PE �ļ���";
	}
	/* �� PE ͷ */
	if((fread(&filePe, sizeof filePe, 1, fp) != 1) || (filePe.Signature != IMAGE_NT_SIGNATURE)) {
		return "û������ѡ���ļ����ҵ� PE ͷ�����ļ����ܲ���һ���Ϸ��� PE �ļ���";
	}

	/* �ж��ǲ��� PE+ */
	switch(filePe.OptionalHeader.Magic) {
		case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
			break;

		case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
			return "������ʱ��֧�ּ��� 64 λ����� PE �ļ���";

		default:
			return "�޷�ʶ���ļ������͡�";
	}

	/* ���������� */
	switch(filePe.FileHeader.Machine) {
		case IMAGE_FILE_MACHINE_I386:
			break;

		default:
			return "�޷�ʶ�����д� PE �ļ�����Ҫ�Ĵ��������͡�";
	}

	/* ��ϵͳ */
	switch(filePe.OptionalHeader.Subsystem) {
		case IMAGE_SUBSYSTEM_WINDOWS_GUI:
		case IMAGE_SUBSYSTEM_WINDOWS_CUI:
			break;

		default:
			return "�޷�ʶ�����д� PE �ļ�����Ҫ����ϵͳ��";
	}

	/* �ж��ǲ��� DLL */
	if((filePe.FileHeader.Characteristics & IMAGE_FILE_DLL) == IMAGE_FILE_DLL) {
		return "�������� DLL �ļ���";
	} else if((filePe.FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) != IMAGE_FILE_EXECUTABLE_IMAGE) {
		return "�޷�ʶ���ļ��ı�ǡ�";
	}

	/* ��ʼ������ */
	if(BP == NULL) {
		mem_alloc(BP,		bp_s,		1);
		mem_alloc(SECTION,	section_s,	1);
		mem_alloc(IMPORT,	import_s,	1);
		mem_alloc(MODULE,	module_s,	1);
	}

	cmdWorking(2, "%s", "���ڶ�ȡ���α� ...");

	/* ���α� */
	next = SECTION;

	/* ��ȡ���� */
	while(!feof(fp)) {
		if(fread(&fileSection, sizeof fileSection, 1, fp) == 0) {
			break;
		}
		if(fileSection.VirtualAddress == 0) {
			break;
		}

		mem_alloc(section, section_s, 1);

		section->virtualaddr	= fileSection.VirtualAddress;
		section->offset			= fileSection.PointerToRawData;
		section->size			= fileSection.Misc.VirtualSize;
		section->name			= readFromBuffer((char *) fileSection.Name, 8);
		section->owner			= PE;

		next->next = section;
		next = section;

		sectionPush(section);
	}

	if(SECTION->next == NULL) {
		return "û���� PE �ļ����ҵ��κ����Ρ�";
	}

	/* ����� */
	if(filePe.OptionalHeader.DataDirectory[1].VirtualAddress != 0) {
		cmdWorking(1, "%s", "���ڶ�ȡ����� ...");

		/* �ƶ�������� */
		fseek(fp, (long) rva2offset(filePe.OptionalHeader.DataDirectory[1].VirtualAddress), SEEK_SET);

		while(!feof(fp)) {
			if(fread(&fileImport, sizeof fileImport, 1, fp) == 0) {
				break;
			}
			if(*((DWORD *) &fileImport) == 0) {
				break;
			}

			peFunction(fp, &fileImport);
		}

		import = IMPORT->next;

		while(import != NULL) {
			importPush(import);

			import = import->next;
		}
	}

	PROCESS.file		= file;
	PROCESS.status		= LOAD;
	PROCESS.entry		= filePe.OptionalHeader.AddressOfEntryPoint;
	PROCESS.baseRVAddr	= filePe.OptionalHeader.ImageBase;

	return NULL;
}

static char * procLoad(char *file) {
	int loaded = 0;

	STARTUPINFO			si;
	PROCESS_INFORMATION	pi;
	CONTEXT				regs;
	DEBUG_EVENT			debug;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	si.cb			= sizeof(STARTUPINFO);
	si.dwFlags		= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	if(CreateProcess(file, NULL, NULL, NULL, FALSE, PROCESS_QUERY_INFORMATION | DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS, NULL, NULL, &si, &pi) == FALSE) {
		return "��������ʧ�ܡ�";
	}

	PROCESS.status		= RUN;
	PROCESS.procHandle	= pi.hProcess;
	PROCESS.procId		= pi.dwProcessId;

	regs.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;

	/* ���� EF �����ʵ�ֵ���ִ�� */
	GetThreadContext(pi.hThread, &regs);
	regs.EFlags |= 0x100;
	SetThreadContext(pi.hThread, &regs);

	/* �ָ����� */
	ResumeThread(pi.hThread);

	cmdWorking(2, "����׼��������Ϣ ...");

	while(1) {
		WaitForDebugEvent(&debug, INFINITE);

		if(debug.dwDebugEventCode == EXCEPTION_DEBUG_EVENT) {
			if(debug.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_SINGLE_STEP) {
				GetThreadContext(pi.hThread, &regs);

				switch(loaded) {
					/* �״��ж� */
					case 0:
						regs.Dr0 = regs.Eax;
						regs.Dr7 = 0x101;

						loaded = 1; break;

					/* �ж���ϵͳ������Ϣ����ǰ */
					case 1:
						peEntry();

						/* �Ҳ�������ַ */
						if(PROCESS.baseAddr == 0) {
							TerminateProcess(pi.hProcess, 0);

							return "�޷��ҵ�������ڵ�ַ��";
						} else {
							PROCESS.baseOffset = ((long) PROCESS.baseAddr) - ((long) PROCESS.baseRVAddr);
						}
						/* û��ģ�� */
						if(MODULE->next == NULL) {
							TerminateProcess(pi.hProcess, 0);

							return "δ�ҵ��κμ��ص�ģ�顣";
						}

						regs.Dr0 = ((long) PROCESS.entry) + PROCESS.baseAddr;
						regs.Dr7 = 0x101;

						loaded = 2;

						peSection();
						peImport(FALSE);
						peBreakAddress(regs.Dr0);

						cmdWorking(0, "���������� ...");

						break;

					/* �ж� */
					default:
						cmdAddress (
							addr2rva(regs.Eip),
							regs.Eip,
							addr2section(regs.Eip),
							NULL
						);

						loop(regs.Eip);

						regs.Dr0 = 0;
						regs.Dr7 = 0;
						regs.EFlags |= 0x100;
				}

				SetThreadContext(pi.hThread, &regs);
			}
		} else if(debug.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT) {
			break;
		}

		ContinueDebugEvent(pi.dwProcessId, pi.dwThreadId, DBG_CONTINUE);
	}

	return NULL;
}


th_new_char(Process) {
	char *message = NULL;

	cmdExecuteEnable(FALSE);
	cmdWorking(0, "���ڴ��ļ� \"%s\" ...", arg);

	if((message = procRead(arg)) == NULL) {
		if((message = procLoad(arg)) == NULL) {
			cmdWorking(0, "�������˳���");
		} else {
			cmdWorking(0, "%s", message);
		}
	} else {
		cmdWorking(2, "%s", message);

		mem_delete(arg);
	}

	if(message != NULL) {
		warning(message);
	}

	cmdExecuteEnable(TRUE);
}
