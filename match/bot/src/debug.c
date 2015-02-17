/*
	$ Robot for LinLink   (C) 2005-2013 MF
	$ debug.c
*/

#include "base.h"


/* �߳����� */
typedef struct thread_s {
	HANDLE	handle;
	DWORD	threadid;

	struct thread_s *next;
} thread_s;


/* �߳����� */
static thread_s *thread = NULL;


/* �̴߳��� */
static DWORD threadCreate(HANDLE handle, DWORD threadid) {
	thread_s *list = thread;

	while(list->next) {
		list = list->next;
	}

	NEW(list->next, thread_s, 1);

	list->next->handle		= handle;
	list->next->threadid	= threadid;

	return DBG_CONTINUE;
}

/* �߳̽��� */
static DWORD threadExit(DWORD threadid) {
	thread_s *next;
	thread_s *list = thread;

	while(list->next) {
		if(list->next->threadid == threadid) {
			break;
		}

		list = list->next;
	}

	if(list->next == NULL) {
		return DBG_CONTINUE;
	}
	if(list->next->threadid != threadid) {
		return DBG_CONTINUE;
	}

	next = list->next;
	list->next = next->next;

	DEL(next);

	return DBG_CONTINUE;
}

/* �̻߳�ȡ */
static HANDLE threadGet(DWORD threadid) {
	thread_s *list = thread;

	while(list->next) {
		if(list->next->threadid == threadid) {
			break;
		}

		list = list->next;
	}

	if(list->next && (list->next->threadid == threadid)) {
		return list->next->handle;
	}

	NEW(list->next, thread_s, 1);

	list = list->next;
	list->threadid = threadid;

	/* �򿪾�� */
	if((list->handle = OpenThread (
			THREAD_QUERY_INFORMATION | THREAD_SET_INFORMATION | THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME | THREAD_TERMINATE,
			FALSE, threadid
		)) == NULL) {
			DIE("�����޷���ȡ��̵߳ľ����������ֹ��");
	}

	return list->handle;
}


/* ��ȡ�Ĵ��� */
static void registerRead(HANDLE thread, CONTEXT *reg) {
	ZeroMemory(reg, sizeof(CONTEXT));

	reg->ContextFlags = CONTEXT_ALL;

	if(GetThreadContext(thread, reg) == FALSE) {
		DIE("�����޷���ȡ��̵߳ļĴ������ݣ�������ֹ��");
	}
}

/* ����Ĵ��� */
static void registerSave(HANDLE thread, CONTEXT *reg) {
	if(SetThreadContext(thread, reg) == FALSE) {
		DIE("�����޷������̵߳ļĴ������ݣ�������ֹ��");
	}
}


THREAD(debug) {
	/* ͬ����Ϣ */
	DWORD		symbol;
	/* ����״̬ */
	DWORD		status;
	/* �߳̾�� */
	HANDLE		handle;

	/* �Ĵ��� */
	CONTEXT		reg;
	/* ������Ϣ */
	DEBUG_EVENT	debug;

	/* �����߳����� */
	NEW(thread, thread_s, 1);

	/* �ȴ����� */
	while(1) {
		WAIT(Run);

		if(procMount()) {
			break;
		}
	}

	/* ������ѭ�� */
	while(1) {
		status = DBG_EXCEPTION_NOT_HANDLED;

		/* ������ֹ */
		TIMEOUT(symbol, Destroy, 0) {
			break;
		}

		/* �ȴ���Ϣ */
		if(WaitForDebugEvent(&debug, 1) == FALSE) {
			if(GetLastError() == ERROR_SEM_TIMEOUT) {
				continue;
			}

			DIE("�����ڶ�ȡ����״̬ʱ���ִ��󣬱�����ֹ��");
		}

		/* ���������Ϣ */
		switch(debug.dwDebugEventCode) {
			/* �����˳� */
			case EXIT_PROCESS_DEBUG_EVENT:
				NOTICE("��Ϸ�����Ѿ��˳������ȷ��������Զ��رա�");
				ExitProcess(EXIT_SUCCESS);

			/* �̴߳��� */
			case CREATE_THREAD_DEBUG_EVENT:
				status = threadCreate(debug.u.CreateThread.hThread, debug.dwThreadId); break;
			/* �߳��˳� */
			case EXIT_THREAD_DEBUG_EVENT:
				status = threadExit(debug.dwThreadId); break;

			/* ���� DLL */
			case LOAD_DLL_DEBUG_EVENT:
				CloseHandle(debug.u.LoadDll.hFile); status = DBG_CONTINUE; break;

			/* �쳣 */
			case EXCEPTION_DEBUG_EVENT:
				/* ������ϵ� */
				if((debug.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_SINGLE_STEP) || (debug.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_BREAKPOINT)) {
					/* ��ȡ�߳̾�� */
					handle = threadGet(debug.dwThreadId);

					registerRead(handle, &reg);
					procAddress((size_t) debug.u.Exception.ExceptionRecord.ExceptionAddress, &reg);
					registerSave(handle, &reg);

					status = DBG_CONTINUE;
				} else {
					/* ����ϵͳ���� */
					if(debug.u.Exception.dwFirstChance == 0) {
						status = DBG_CONTINUE;
					}
				}
				break;
		}
		
		/* ����ִ�� */
		ContinueDebugEvent(debug.dwProcessId, debug.dwThreadId, status);
	}

	/* �ָ������ڴ� */
	procRestore();
}
