/*
	$ Sinope PE Loader  (C) 2005-2012 mfboy
	$ process.c, 0.1.1104
*/

#define _SNP_PROCESS_

#include <Windows.h>
#include <Psapi.h>
#include "base.h"
#include "image.h"
#include "gui.h"
#include "action.h"


/* ָ�� */
size_t pfPosition = 0;


/* ģ����� */
BOOL procMapModule() {
	int			i, j;
	char		proc[MAX_PATH], name[MAX_PATH];

	module_s	*next, *curr;

	DWORD		count;
	HMODULE		module[4096];
	MODULEINFO	info;

	/* ��ȡ������ */
	if(GetModuleFileNameEx(RUNTIME.proc.handle, NULL, proc, MAX_PATH) == 0) {
		return FALSE;
	}
	/* ��ȡ����ģ�� */
	if(EnumProcessModules(RUNTIME.proc.handle, module, sizeof module, &count) == FALSE) {
		return FALSE;
	}

	j		= count / sizeof(HMODULE);
	next	= MODULE;

	for(i = 0; i < j; i++) {
		if(GetModuleFileNameEx(RUNTIME.proc.handle, module[i], name, MAX_PATH) == 0) {
			continue;
		}
		/* ��ѯģ�鳤�� */
		if(GetModuleInformation(RUNTIME.proc.handle, module[i], &info, sizeof info) == FALSE) {
			continue;
		}

		/* ����ģ��� */
		mem_alloc(curr, module_s, 1);
		mem_alloc(curr->path, char, strlen(name) + 1);

		curr->address	= (DWORD) module[i];
		curr->index		= -1;
		curr->size		= info.SizeOfImage;

		strcpy(curr->path, name);

		/* ����ģ���ȡ��ʵ�Ļ���ַ */
		if(strcmp(proc, name) == 0) {
			RUNTIME.proc.base	= (DWORD) module[i];
			RUNTIME.proc.size	= curr->size;
			RUNTIME.proc.module	= curr;
		}

		next->next = curr;
		next = curr;
	}

	/* û��ģ�� */
	if(MODULE->next == NULL) {
		return FALSE;
	}

	/* δ��ȡ����ַ */
	if(RUNTIME.proc.base == 0) {
		return FALSE;
	}

	RUNTIME.proc.entry	= RUNTIME.pe.entry + RUNTIME.proc.base;
	THREAD->next->start	= RUNTIME.proc.entry;

	return TRUE;
}

/* ����ӳ�� */
void procMapSection() {
	BOOL		image = FALSE;
	section_s	*section = SECTION->next;

	while(section) {
		/* ʵ�ʵ�ַ */
		section->addr = section->va + RUNTIME.proc.base;

		/* �������� */
		if((section->file == TRUE) && (section->dynamic == FALSE)) {
			mem_alloc(section->copy, char, section->sizeImage);

			/* �����ڴ� */
			if(memoryRead(section->addr, section->copy, section->sizeImage)) {
				image = TRUE;
			} else {
				mem_delete(section->copy);
			}
		}

		section = section->next;
	}

	if(image == FALSE) {
		warning("�޷�Ϊ PE �ļ��κ�һ�����δ���ԭʼ���ݣ�����ܻ���ɶ�̬������Ĺ����޷�ʹ�á�");
	}
}


/* ��ʼ�� */
static BOOL procInit() {
	/* ����״̬ */
	BOOL		run		= 1;
	/* ���߳� */
	thread_s	*thread	= THREAD->next;

	/* ������Ϣ */
	DEBUG_EVENT	debug;
	/* �Ĵ��� */
	CONTEXT		reg;
	/* ״̬ */
	DWORD		status;

	/* ���üĴ��� */
	reg.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;

	WORKING_SET("�������н��� ...");

	while(run) {
		status = DBG_EXCEPTION_NOT_HANDLED;

		WaitForDebugEvent(&debug, INFINITE);
		GetThreadContext(thread->handle, &reg);

		if(debug.dwDebugEventCode == EXCEPTION_DEBUG_EVENT) {
			if(debug.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_SINGLE_STEP) {
				switch(run) {
					/* �״��ж� */
					case 1:
						reg.Dr0 = reg.Eax;
						reg.Dr7 = 0x101;

						run = 2; break;

					/* �������б��ݺͻ��� */
					case 2:
						/* ��ȡģ�� */
						if(procMapModule() == FALSE) {
							loopTerminate(); return FALSE;
						}

						/* ӳ�� */
						procMapSection();
						procImport();

						/* �����ڶϵ� */
						bpFlush();
						bpAdd(RUNTIME.proc.entry, RUNTIME.proc.entry, FALSE);

						/* ���ó������ */
						reg.Dr0 = RUNTIME.proc.entry;
						reg.Dr7 = 0x101;

						/* ���ñ��� */
						procTitle("������");

						run = 0; break;
				}

				SetThreadContext(thread->handle, &reg);

				status = DBG_CONTINUE;
			}
		} else if(debug.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT) {
			return FALSE;
		}

		ContinueDebugEvent(RUNTIME.proc.pid, thread->id, status);
	}

	return TRUE;
}

/* ���� */
static void procFree() {
	void *next;

	RUNTIME.alive		= FALSE;
	RUNTIME.running		= FALSE;
	RUNTIME.singleStep	= FALSE;

	ZeroMemory(&RUNTIME.proc, sizeof RUNTIME.proc);
	ZeroMemory(&RUNTIME.pe, sizeof RUNTIME.pe);

	/* �����߳� */
	while(THREAD->next) {
		next = THREAD->next->next;

		mem_delete(THREAD->next);

		THREAD->next = (thread_s *) next;
	}

	/* ����ϵ� */
	while(BP->next) {
		next = BP->next->next;

		mem_delete(BP->next);

		BP->next = (bp_s *) next;
	}

	/* �������� */
	while(SECTION->next) {
		next = SECTION->next->next;

		//mem_delete(SECTION->next->name);
		mem_delete(SECTION->next->copy);
		mem_delete(SECTION->next);

		SECTION->next = (section_s *) next;
	}

	/* ������� */
	while(IMPORT->next) {
		next = IMPORT->next->next;

		//mem_delete(IMPORT->next->dll);
		//mem_delete(IMPORT->next->function);
		mem_delete(IMPORT->next);

		IMPORT->next = (import_s *) next;
	}

	/* ����ģ�� */
	while(MODULE->next) {
		next = MODULE->next->next;

		mem_delete(MODULE->next->path);
		mem_delete(MODULE->next);

		MODULE->next = (module_s *) next;
	}
}

/* �������� */
static char * process() {
	int		status = 0;

	STARTUPINFO			si;
	PROCESS_INFORMATION	pi;
	CONTEXT				reg;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	si.cb		= sizeof(STARTUPINFO);
	si.dwFlags	= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	/* CUI ��������̨ */
	if(RUNTIME.pe.console) {
		if(CONFIG.console == FALSE) {
			if(AllocConsole() == FALSE) {
				return "��������̨����ʧ�ܡ�";
			}

			CONFIG.console = TRUE;
		}

		si.hStdInput	= GetStdHandle(STD_INPUT_HANDLE);
		si.hStdOutput	= GetStdHandle(STD_OUTPUT_HANDLE);
		si.hStdError	= GetStdHandle(STD_ERROR_HANDLE);
	}

	/* ���н��� */
	if(CreateProcess(RUNTIME.file, NULL, NULL, NULL, FALSE, PROCESS_QUERY_INFORMATION | DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS, NULL, NULL, &si, &pi) == FALSE) {
		return "��������ʧ�ܡ�";
	}

	RUNTIME.alive		= TRUE;
	RUNTIME.running		= TRUE;
	RUNTIME.singleStep	= FALSE;

	/* ������Ϣ */
	RUNTIME.proc.pid	= pi.dwProcessId;
	RUNTIME.proc.handle	= pi.hProcess;

	/* �������߳� */
	threadCreate(pi.hThread, pi.dwThreadId, 0);

	/* ���ñ���Ի�üĴ�����Ϣ */
	reg.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;

	/* ���� EF ���, ʵ�ֵ���ִ�� */
	GetThreadContext(pi.hThread, &reg);
	reg.EFlags |= 0x100;
	SetThreadContext(pi.hThread, &reg);

	/* �������� */
	ResumeThread(pi.hThread);

	/* ��ʼ��������Ϣ */
	if(procInit() == FALSE) {
		return "�����쳣�˳���";
	}

	/* ���ø���״̬ */
	enable(TRUE, FALSE, FALSE, FALSE);
	procTitle("������");

	/* ��ʾ������Ϣ */
	threadFlush();
	sectionFlush();
	importFlush();
	moduleFlush();

	/* ѡ�����߳� */
	COMBO(cmbThread)->select(0);

	/* ��ѭ�� */
	loop();

	return NULL;
}


thNew(Process) {
	char *error;

	/* �޸İ�ť */
	OBJECT(bExecute)->caption("��ֹ����");
	OBJECT(bExecute)->disable(TRUE);

	/* ����ִ�� */
	if((error = process()) == NULL) {
		WORKING_SET("�������˳���");
	} else {
		WORKING_SET_ALERT(error);
	}

	procFree();
	procTitle(NULL);

	enable(TRUE, FALSE, FALSE, FALSE);

	OBJECT(bExecute)->caption("��������");
}


/* ����� */
void procImport() {
	DWORD		address;
	import_s	*import = IMPORT->next;

	while(import) {
		if(memoryRead(va2addr(import->iatVa), (char *) &address, sizeof address)) {
			if(import->entry == 0) {
				import->entry = address;
			}

			import->point = address;
		}

		import = import->next;
	}
}

/* ���� */
void procTitle(char *status) {
	int admin = -1;

	if(admin < 0) {
		admin = (int) uacIsAdmin();
	}

	if(status == NULL) {
		wMain.caption(WIN_TITLE);
	} else if(admin == 0) {
		wMain.caption("%s (%s) - %s", RUNTIME.filename, status, WIN_TITLE);
	} else {
		wMain.caption("%s (%s) - %s [����Ա]", RUNTIME.filename, status, WIN_TITLE);
	}
}

/* ���� PE �ļ���Ϣ */
void procClear() {
	section_s	*section = SECTION, *sectionNext;
	import_s	*import;
	module_s	*module;

	/* �������� */
	while(section && section->next) {
		/* ���� PE �ļ�������� */
		if((section->next->dynamic == FALSE) && (section->next->file == TRUE)) {
			sectionNext = section->next->next;

			//mem_delete(section->next->name);
			mem_delete(section->next->copy);
			mem_delete(section->next);

			section = sectionNext;
		} else {
			section = section->next;
		}
	}

	/* ������� */
	while(IMPORT->next) {
		import = IMPORT->next->next;

		//mem_delete(IMPORT->next->dll);
		//mem_delete(IMPORT->next->function);
		mem_delete(IMPORT->next);

		IMPORT->next = import;
	}

	/* ����ģ�� */
	while(MODULE->next) {
		module = MODULE->next->next;

		mem_delete(MODULE->next->path);
		mem_delete(MODULE->next);

		MODULE->next = (module_s *) module;
	}
}


thread_s * threadGet(DWORD id) {
	thread_s *thread = THREAD->next;

	while(thread) {
		if(thread->id == id) {
			return thread;
		}

		thread = thread->next;
	}

	return NULL;
}

thread_s * threadCreate(HANDLE handle, DWORD id, DWORD addr) {
	thread_s *next = THREAD;

	/* ����β */
	while(next->next != NULL) {
		next = next->next;
	}

	/* ��ȡ threadid */
	if(id == 0) {
		id = GetThreadId(handle);
	}

	mem_alloc(next->next, thread_s, 1);

	next->next->id		= id;
	next->next->handle	= handle;
	next->next->start	= addr;

	return next->next;
}

void threadExit(HANDLE handle) {
	thread_s *curr = NULL;
	thread_s *next = THREAD;

	while(next->next) {
		if(next->next->handle == handle) {
			curr = next->next;
			next->next = curr->next;

			break;
		}

		next = next->next;
	}
	
	if(curr) {
		COMBO(cmbThread)->remove(curr->index);
		mem_delete(curr);
	}

	threadFlush();
}


BOOL memoryRead(DWORD address, void *buffer, DWORD size) {
	size_t count;

	if(ReadProcessMemory(RUNTIME.proc.handle, (LPCVOID) address, buffer, size, (SIZE_T *) &count)) {
		return TRUE;
	}

	return FALSE;
}

BOOL memoryWrite(DWORD address, void *buffer, DWORD size) {
	size_t count;

	if(WriteProcessMemory(RUNTIME.proc.handle, (LPVOID) address, buffer, size, (SIZE_T *) &count)) {
		return TRUE;
	}

	return FALSE;
}

BOOL isPeModule(DWORD addr) {
	if(addr < RUNTIME.proc.base) {
		return FALSE;
	}
	if(addr >= (RUNTIME.proc.base + RUNTIME.proc.module->size)) {
		return FALSE;
	}

	return TRUE;
}


BOOL dataReadMemory(void *buffer, size_t size) {
	if(memoryRead(RUNTIME.proc.base + pfPosition, buffer, size)) {
		pfPosition += size;

		return TRUE;
	}

	return FALSE;
}

void dataSeekMemory(long offset, int mode) {
	switch(mode) {
		case SEEK_SET: pfPosition = (size_t) offset; break;
		case SEEK_CUR: pfPosition = (size_t) ((long) pfPosition + offset); break;
	}
}

long dataTellMemory() {
	return (long) pfPosition;
}

DWORD dataOffsetMemory(DWORD va) {
	return va;
}

BOOL dataEndMemory() {
	if(pfPosition < (RUNTIME.proc.base + RUNTIME.proc.size)) {
		return FALSE;
	} else {
		return TRUE;
	}
}
