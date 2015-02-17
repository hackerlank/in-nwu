/*
	$ Invoke   (C) 2005-2012 MF
	$ thread.c, 0.1.1202
*/

#include "base.h"


/* �߳̽ṹ�� */
typedef struct thread_s {
	DWORD	threadid;
	HANDLE	handle;

	/* ����λ�� */
	DWORD	ret;
	/* ���ضϵ� */
	BOOL	retBk;

	/* �ָ�����ĺ��� */
	function_s *function;

	struct thread_s *next;
} thread_s;


/* �߳����� */
static thread_s *THDS = NULL;
/* ���ʹ�� */
static thread_s *LAST = NULL;


static thread_s * threadFind(DWORD threadid) {
	thread_s *list = THDS;

	/* �������ʹ�� */
	if(LAST && (LAST->threadid == threadid)) {
		return LAST;
	}

	/* ���� */
	while(list->next) {
		if(list->next->threadid == threadid) {
			LAST = list->next;

			return LAST;
		}

		list = list->next;
	}

	return NULL;
}


void threadCreate(DWORD threadid, HANDLE handle) {
	thread_s *list;

	if(THDS == NULL) {
		NEW(THDS, thread_s, 1);
	}

	list = THDS;

	while(list->next) {
		if(list->next->threadid == threadid) {
			list = list->next; break;
		}

		list = list->next;
	}

	/* ����ͬ�� */
	if(list->threadid == threadid) {
		list->handle = handle;
	} else {
		NEW(list->next, thread_s, 1);

		list->next->threadid	= threadid;
		list->next->handle		= handle;
	}
}

void threadExit(DWORD threadid) {
	thread_s *next;
	thread_s *list = THDS;

	while(list->next) {
		if(list->next->threadid == threadid) {
			break;
		}

		list = list->next;
	}

	/* �ҵ��߳� */
	if(list->next->threadid == threadid) {
		if(LAST == list->next) {
			LAST = NULL;
		}

		/* �ָ������ϵ� */
		hookSetCode(list->next->function);

		next = list->next->next;

		DEL(list->next);

		list->next = next;
	}
}

HANDLE threadGet(DWORD threadid) {
	thread_s *list = THDS;

	/* �������ʹ�� */
	if(LAST && (LAST->threadid == threadid)) {
		return LAST->handle;
	}

	/* ���� */
	while(list->next) {
		if(list->next->threadid == threadid) {
			LAST = list->next;


			return LAST->handle;
		}

		list = list->next;
	}

	/* �����еĽ��� */
	if(RT.entry) {
		die("Invoke ���ִ��󣬷Ƿ����߳̾����ѯ��Ӧ�ó����ѱ�����ֹ��");
	}

	NEW(list->next, thread_s, 1);

	list			= list->next;
	list->threadid	= threadid;

	/* �򿪾�� */
	if((list->handle = OpenThread (
		THREAD_QUERY_INFORMATION | THREAD_SET_INFORMATION | THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME | THREAD_TERMINATE,
		FALSE, list->threadid)) == NULL) {
			die("Invoke ���ִ����޷���ȡ����߳̾����Ӧ�ó����ѱ�����ֹ��");
	}

	LAST = list;

	return list->handle;
}

function_s * threadFunctionGet(DWORD threadid) {
	thread_s *thread;

	if(thread = threadFind(threadid)) {
		return thread->function;
	} else {
		return NULL;
	}
}

void threadFunctionSet(DWORD threadid, function_s *func) {
	thread_s *thread;

	if(thread = threadFind(threadid)) {
		thread->function = func;
	}
}

DWORD threadRetGet(DWORD threadid, BOOL *retBk) {
	thread_s *thread;

	if(thread = threadFind(threadid)) {
		if(retBk) {
			*retBk = thread->retBk;
		}

		return thread->ret;
	} else {
		return 0;
	}
}

void threadRetSet(DWORD threadid, DWORD ret, BOOL retBk) {
	thread_s *thread;

	if(thread = threadFind(threadid)) {
		thread->ret		= ret;
		thread->retBk	= retBk;
	}
}

void threadSuspend(DWORD threadid) {
	thread_s *thread = THDS->next;

	while(thread) {
		if(thread->threadid != threadid) {
			SuspendThread(thread->handle);
		}

		thread = thread->next;
	}
}

void threadResume(DWORD threadid) {
	thread_s *thread = THDS->next;

	while(thread) {
		if(thread->threadid != threadid) {
			ResumeThread(thread->handle);
		}

		thread = thread->next;
	}
}
