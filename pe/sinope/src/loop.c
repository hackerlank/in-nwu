/*
	$ Sinope PE Loader  (C) 2005-2012 mfboy
	$ loop.c, 0.1.1104
*/

#include <Windows.h>
#include "base.h"
#include "image.h"
#include "gui.h"
#include "action.h"
#include "libdisasm/libdis.h"


/* ����ִ�е�ָ�� */
static struct {
	DWORD		eip;
	BOOL		main;
	BOOL		dynamic;

	section_s	*section;
	module_s	*module;

	/* �߳� */
	thread_s	*thread;
	/* �Ĵ��� */
	CONTEXT		reg;
} command = {0, FALSE, FALSE, NULL, NULL, {0}};


/* ģʽ�л� */
static void loopMode(BOOL e) {
	BOOL d = e ? FALSE : TRUE;

	enable(TRUE, d, e, d);
}

/* ��ǰִ��ָ�� */
static void loopCommand() {
	char	output[64] = {0};
	DWORD	rva;

	if((command.main == FALSE) && (CONFIG.dllEip == FALSE)) {
		return;
	}

	EDIT(eEip)->setText("%08X", command.eip);

	if(rva = addr2rva(command.eip)) {
		EDIT(eRva)->setText("%08X", rva);
	} else {
		EDIT(eRva)->setText("-");
	}

	if(command.section) {
		if(command.section->name) {
			EDIT(eSection)->setText(command.section->name);
		} else {
			EDIT(eSection)->setText("@ %08X", command.section->addr);
		}
	} else {
		EDIT(eSection)->setText("-");
	}

	loopDisasm(FALSE);
}


/* �ϵ���� */
static BOOL loopBreakpoint() {
	bp_s *bp = BP->next;

	while(bp) {
		/* �жϳɹ� */
		if((bp->addrFrom <= command.eip) && (bp->addrTo >= command.eip)) {
			bpActive(bp->index); return TRUE;
		}

		bp = bp->next;
	}

	return FALSE;
}

/* ��̬������ */
static BOOL loopDynmCode() {
	char		buffer[10]	= {0};
	size_t		size		= 0;

	switch(CONFIG.bp.dynamicCodeMode) {
		/* ���Ǽ�� */
		case 0:
			break;

		/* �����μ�� */
		case 1:
			if(RUNTIME.proc.tBreak == NULL) {
				return FALSE;
			}
			if(command.section == RUNTIME.proc.tBreak->run.section) {
				return FALSE;
			}
			break;

		/* ������� */
		case 2:
			if(RUNTIME.proc.tBreak == NULL) {
				return FALSE;
			}
			if((command.main == FALSE) || (RUNTIME.proc.tBreak->run.main == TRUE)) {
				return FALSE;
			}
			break;
	}

	if((command.section == NULL) || (command.section->copy == NULL)) {
		return FALSE;
	}

	/* ����Ƚϴ�С */
	size = (command.section->addr + command.section->sizeImage) - command.eip;
	size = (size > sizeof buffer) ? sizeof buffer : size;

	if(size > 0) {
		if(memoryRead(command.eip, buffer, size)) {
			/* �ڴ�Ƚ� */
			if(memcmp(buffer, (char *) command.section->copy + (command.eip - command.section->addr), size)) {
				return TRUE;
			}
		}
	}

	return FALSE;
}

/* ��⶯̬�ڴ� */
static BOOL loopDynmMemory() {
	if((RUNTIME.proc.tBreak == NULL) || (command.section == NULL)) {
		return FALSE;
	}
	if((command.section->dynamic == FALSE) || (command.section->file == FALSE)) {
		return FALSE;
	}
	if((RUNTIME.proc.tBreak->run.section->dynamic == TRUE) && (RUNTIME.proc.tBreak->run.section->file == TRUE)) {
		return FALSE;
	}

	return TRUE;
}

/* ��������ת */
static BOOL loopStride() {
	if(RUNTIME.proc.tBreak == NULL) {
		return FALSE;
	}
	if(command.section == RUNTIME.proc.tBreak->run.section) {
		return FALSE;
	}

	/* ����������������� */
	if(RUNTIME.proc.tBreak->run.main == FALSE) {
		return FALSE;
	}
	if(command.main == FALSE) {
		return FALSE;
	}

	return TRUE;
}


static BOOL loopPause() {
	/* ������ͣ״̬ */
	loopMode(TRUE);
	procTitle("��ͣ��");

	/* ����� */
	loopDisasm(TRUE);

	/* ���� */
	while(RUNTIME.running == FALSE) {
		/* �˳����� */
		if(RUNTIME.alive == FALSE) {
			loopTerminate(); return FALSE;
		}

		Sleep(1);
	}

	/* �ָ����� */
	loopMode(FALSE);
	procTitle("������");

	WORKING_SET("���ָ̻����С�");

	return TRUE;
}

static int loopWait() {
	DEBUG_EVENT	debug;
	char		*msg = NULL;

	while(1) {
		/* ���ֹͣ */
		if(RUNTIME.alive == FALSE) {
			loopTerminate(); return -1;
		}

		/* ��ͣ */
		if(RUNTIME.running == FALSE) {
			if(loopPause() == FALSE) {
				return -1;
			}
		}

		/* �ȴ� */
		if(WaitForDebugEvent(&debug, 1)) {
			break;
		}
	}

	/* ��ǰ�߳� */
	if(command.thread = threadGet(debug.dwThreadId)) {
		/* ��ȡ�Ĵ��� */
		GetThreadContext(command.thread->handle, &command.reg);

		/* ��ȡ��Ϣ */
		command.eip		= command.reg.Eip;
		command.main	= isPeModule(command.eip);
		command.section	= getSectionByAddr(command.eip);
		command.module	= getModuleByAddr(command.eip);
	} else {
		if(debug.dwDebugEventCode != CREATE_THREAD_DEBUG_EVENT) {
			loopTerminate(); return -1;
		}
	}

	/* ����ָ���� */
	loopCommand();

	switch(debug.dwDebugEventCode) {
		/* �̴߳��� */
		case CREATE_THREAD_DEBUG_EVENT:
			threadCreate(
				/* handle */	debug.u.CreateThread.hThread,
				/* threadid */	GetThreadId(debug.u.CreateThread.hThread),
				/* address */	(DWORD) debug.u.CreateThread.lpStartAddress
			);

			threadFlush();

			/* ���߳� */
			command.thread = threadGet(debug.dwThreadId);

			/* ��ʼ���Ĵ��� */
			command.reg.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;
			GetThreadContext(command.thread->handle, &command.reg);

			return 0;

		/* �쳣 */
		case EXCEPTION_DEBUG_EVENT:
			break;

		/* DLL ���� */
		case LOAD_DLL_DEBUG_EVENT:
			CloseHandle(debug.u.LoadDll.hFile); return 1;

		/* �߳��˳� */
		case EXIT_THREAD_DEBUG_EVENT:
			threadExit(threadGet(debug.dwThreadId)->handle); return 0;

		/* �����˳� */
		case EXIT_PROCESS_DEBUG_EVENT:
			return -1;

		default:
			return 0;
	}
	
	switch(debug.u.Exception.ExceptionRecord.ExceptionCode) {
		case EXCEPTION_SINGLE_STEP:
			break;

		default:
			return 0;
	}

	/* �ж��ǲ�����Ҫ���Ե��߳� */
	if(RUNTIME.proc.tDebug != NULL) {
		if(RUNTIME.proc.tBreak->id != debug.dwThreadId) {
			return 1;
		}
	}

	/* �ϵ��ж� */
	do {
		if(RUNTIME.singleStep) {
			RUNTIME.singleStep = FALSE;

			msg = "����ִ���жϡ�"; break;
		}

		if(loopBreakpoint()) {
			msg = "�����˶ϵ㡣"; break;
		}

		if(CONFIG.bp.dynamicCode) {
			if(loopDynmCode()) {
				msg = "ִ�б���̬�޸ĵĴ��롣"; break;
			}
		}
		if(CONFIG.bp.dynamicMemory) {
			if(loopDynmMemory()) {
				msg = "��ת����̬�����ڴ��С�"; break;
			}
		}
		if(CONFIG.bp.stride) {
			if(loopStride()) {
				msg = "��������ת��"; break;
			}
		}
	} while(0);

	/* ����״̬ */
	RUNTIME.proc.tBreak					= command.thread;
	RUNTIME.proc.tBreak->run.eip		= command.eip;
	RUNTIME.proc.tBreak->run.main		= command.main;
	RUNTIME.proc.tBreak->run.dynamic	= command.dynamic;
	RUNTIME.proc.tBreak->run.section	= command.section;
	RUNTIME.proc.tBreak->run.module		= command.module;

	if(msg) {
		/* ����״̬ */
		RUNTIME.running = FALSE;

		/* ������ʾ */
		WORKING_SET(msg);

		/* ��ʾ�� */
		if(CONFIG.beep) {
			MessageBeep(MB_ICONASTERISK);
		}

		/* ������ͣ״̬ */
		if(loopPause() == FALSE) {
			return -1;
		}

		bpActive(-1);
	}

	return 1;
}

static void loopContinue(BOOL handled) {
	DWORD	status;

	/* ���üĴ��� */
	command.reg.Dr0 = 0;
	command.reg.Dr7 = 0;
	command.reg.EFlags |= 0x100;

	if(handled) {
		status = DBG_CONTINUE;
	} else {
		status = DBG_EXCEPTION_NOT_HANDLED;
	}

	/* ���üĴ��� */
	SetThreadContext(command.thread->handle, &command.reg);

	/* �������� */
	ContinueDebugEvent(RUNTIME.proc.pid, command.thread->id, status);
}


void loop() {
	/* ��ʼ�� command */
	command.reg.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;

	while(1) {
		switch(loopWait()) {
			/* �����˳� */
			case -1: return;

			/* δ���� */
			case 0: loopContinue(FALSE); break;

			/* �Ѵ��� */
			case 1: loopContinue(TRUE); break;
		}
	}
}

void loopTerminate() {
	DebugActiveProcessStop(RUNTIME.proc.pid);
	TerminateProcess(RUNTIME.proc.handle, 0);
}

/* ����� */
void loopDisasm(BOOL disasm) {
	x86_insn_t		insn;

	int				i;
	char			output[128] = {0};
	unsigned char	buffer[10];

	if(memoryRead(command.eip, buffer, sizeof buffer) == FALSE) {
		output[0] = '-';
	} else {
		if((disasm == TRUE) && (CONFIG.disasm == TRUE)) {
			if(x86_disasm(buffer, sizeof buffer, addr2rva(command.eip), 0, &insn)) {
				x86_format_insn(&insn, output, sizeof output, intel_syntax);

				x86_oplist_free(&insn);
			}
		}

		if(output[0] == 0) {
			for(i = 0; i < sizeof buffer; i++) {
				sprintf(output + i * 3, "%02X ", buffer[i]);
			}
		}
	}

	EDIT(eCode)->setText(output);
}
