/*
	$ Invoke   (C) 2005-2012 MF
	$ run.c, 0.1.1202
*/

#include <Windows.h>
#include <Psapi.h>
#include "base.h"
#include "gui.h"


/* ����״̬ */
static int			RUN = -1;

/* ������Ϣ */
static DEBUG_EVENT	debug;
/* �Ĵ�����Ϣ */
static CONTEXT		reg;


static void registerRead() {
	ZeroMemory(&reg, sizeof reg);

	reg.ContextFlags = CONTEXT_ALL;

	if(GetThreadContext(threadGet(debug.dwThreadId), &reg) == FALSE) {
		die("Invoke ���ִ����޷���ȡ��̵߳ļĴ�����Ϣ��Ӧ�ó����ѱ�����ֹ��");
	}
}

static void registerSave() {
	if(SetThreadContext(threadGet(debug.dwThreadId), &reg) == FALSE) {
		die("Invoke ���ִ����޷����û�̵߳ļĴ�����Ϣ��Ӧ�ó����ѱ�����ֹ��");
	}
}


static void runShow(function_s *func, BOOL justret) {
	int		i, mode, input;
	char	*buffer;

	DWORD		enter;
	argument_s	*arg;

	/* ����������Ϣ */
	RT.thread	= threadGet(debug.dwThreadId);
	RT.function	= func;

	/* ����� */
	activeWindow(wMain.hwnd);

	guiMainStatus(TRUE, FALSE, FALSE);

	if(justret) {
		OBJECT(eMainEsp)->disable(TRUE);
		OBJECT(eMainRet)->disable(TRUE);

		OBJECT(bMainCalc)->disable(TRUE);

		OBJECT(bMainContinue)->disable(TRUE);
		OBJECT(bMainRuntoret)->disable(TRUE);
		OBJECT(bMainRuntocon)->disable(TRUE);
	} else {
		guiMainTitle(func->module->modname, func->name, func->ordinal);

		EDIT(eMainModule)->setText("%s", func->module->modname);
		EDIT(eMainOrdinal)->setText("%ld", func->ordinal);

		if(func->name) {
			EDIT(eMainFuncname)->setText("%s", func->name);
		} else {
			EDIT(eMainFuncname)->setText("");
		}

		EDIT(eMainEsp)->setText("%08X", reg.Esp);

		OBJECT(eMainEsp)->disable(FALSE);
		OBJECT(eMainRet)->disable(FALSE);
		OBJECT(bMainCalc)->disable(FALSE);

		BUTTON(bMainCalc)->click();
	}

	EDIT(eMainEax)->setText("%08X", reg.Eax);

	/* �����ź� */
	while(1) {
		symbolReturn(-1);

		/* ִ��ģʽ */
		if((mode = symbolWait()) == 0) {
			break;
		}

		if(justret) {
			buffer	= EDIT(eMainEax)->getText();
			input	= sscanf(buffer, "%x", &enter);

			DEL(buffer);

			if(input <= 0) {
				warning(wMain.hwnd, "����ķ���������Ч��"); continue;
			}

			registerRead();
			reg.Eax = enter;
			registerSave();

			guiMainTitle(NULL, NULL, 0);
		} else {
			buffer	= EDIT(eMainRet)->getText();
			input	= sscanf(buffer, "%x", &enter);

			DEL(buffer);

			if(input <= 0) {
				warning(wMain.hwnd, "����ķ��ص�ַ��Ч��"); continue;
			}

			if(mode == 3) {
				if((func == NULL) || (func->hijack == NULL)) {
					warning(wMain.hwnd, "����ԭ�Ͳ����ڣ��޷�ֱ�ӷ��ء�"); continue;
				}

				registerRead();

				buffer	= EDIT(eMainEax)->getText();
				input	= sscanf(buffer, "%x", &reg.Eax);

				DEL(buffer);

				if(input <= 0) {
					warning(wMain.hwnd, "����ķ���������Ч��"); continue;
				}

				if(RT.mainStart && RT.mainEnd) {
					if((enter < RT.mainStart) || (enter >= RT.mainEnd)) {
						if(!confirmFormat(wMain.hwnd, "ֱ�ӷ�����ζ�Ų�����뺯���ڲ�ִ�У���ʱ EAX ������ֵ�൱�ں����ķ���ֵ����ջ ESP �� EBP ��ֵ������ݺ���ԭ�����ý���ƽ�⡣\n\n���ص�ַ��%08X \n��ģ���ַ��%08X - %08X\n\n����ķ��ص�ַ������ģ���ַ��Χ�ڣ��Ƿ������", enter, RT.mainStart, RT.mainEnd)) {
							continue;
						}
					}
				} else {
					if(!confirmFormat(wMain.hwnd, "ֱ�ӷ�����ζ�Ų�����뺯���ڲ�ִ�У���ʱ EAX ������ֵ�൱�ں����ķ���ֵ����ջ ESP �� EBP ��ֵ������ݺ���ԭ�����ý���ƽ�⡣\n\n�޷�ȷ������ķ��ص�ַ %08X �Ƿ�����ģ���ַ��Χ�ڣ��Ƿ������", enter)) {
						continue;
					}
				}

				reg.Eip = enter;

				/* ִ�� pop ebx */
				if(func->hijack->esp) {
					processRead(reg.Esp, &enter, sizeof enter);

					reg.Ebp  = enter;
					reg.Esp += 4;
				}

				/* ESP ��ȥ���ص�ַ */
				reg.Esp += 4;

				/* ����ģʽ */
				switch(func->hijack->invoke) {
					/* cdecl, ������ƽ�� */
					case CL_CDECL:
						break;

					/* stdcall, ��������ƽ�� */
					case CL_STDCALL:
						if(arg = func->hijack->argList) {
							while(arg) {
								reg.Esp += arg->type.size;
								arg = arg->next;
							}
						}
						break;

					/* fastcall, ǰ������������ ECX/EDX, ��������ƽ�� */
					case CL_FASTCALL:
						i = 0;

						if(arg = func->hijack->argList) {
							while(arg) {
								if(i > 1) {
									reg.Esp += arg->type.size;
								}

								i++;
								arg = arg->next;
							}
						}

						break;
				}
				
				registerSave();
			} else {
				/* ʹ��Ӳ���ϵ� */
				registerRead();
				reg.Dr0 = enter;
				reg.Dr7 = 0x101;
				registerSave();

				if(mode == 1) {
					threadRetSet(debug.dwThreadId, enter, TRUE);
				} else if(mode == 2) {
					threadRetSet(debug.dwThreadId, enter, FALSE);
				}
			}
		}

		break;
	}

	/* �ָ�����״̬ */
	guiMainStatus(FALSE, TRUE, TRUE);
}


static void runDebugProcessExit() {
	notice(wMain.hwnd, "���ڵ��ԵĽ����Ѿ���ֹ���У����ȷ�� Invoke �����Զ��˳���");

	ExitProcess(EXIT_SUCCESS);
}

static void runDebugThreadCreate() {
	threadCreate(debug.dwThreadId, debug.u.CreateThread.hThread);
}

static void runDebugThreadExit() {
	threadExit(debug.dwThreadId);
}

static void runDebugDllLoad() {
	int check = 0;
	module_s *module;
	
	/* ����ģ�� */
	module = moduleLoad(debug.u.LoadDll.hFile, (DWORD) debug.u.LoadDll.lpBaseOfDll);
	/* �رվ�� */
	CloseHandle(debug.u.LoadDll.hFile);

	if(module == NULL) {
		return;
	}

	/* �Ƽ�����ģ�� */
	if(strEqual(module->modname, "user32.dll")) {
		check = 1;
	}

	/* �ϵ㴰�� */
	if(guiBp(wMain.hwnd, module->modname, &module, 1, check)) {
		/* �ٳִ��� */
		guiHij(wMain.hwnd);
	}
}

static void runDebugDllUnload() {
	moduleDelete((DWORD) debug.u.UnloadDll.lpBaseOfDll);
}

static void runDebugExSglStep() {
	BOOL retBk;
	function_s	*func;

	/* �ϵ��ַ */
	DWORD		address = (DWORD) debug.u.Exception.ExceptionRecord.ExceptionAddress;

	EDIT(eMainBp)->setText("%08X", address);

	if(func = threadFunctionGet(debug.dwThreadId)) {
		/* �ָ��ϴν���ĺ����ϵ� */
		hookSetBp(func);
		threadFunctionSet(debug.dwThreadId, NULL);

		/* �ָ������߳� */
		threadResume(debug.dwThreadId);
	}

	/* ���ص� */
	if(address == threadRetGet(debug.dwThreadId, &retBk)) {
		registerRead();
		reg.Dr0 = 0;
		reg.Dr7 = 0;
		reg.Eip = address;
		registerSave();

		threadRetSet(debug.dwThreadId, 0, FALSE);

		if(retBk) {
			runShow(NULL, TRUE);
		} else {
			EDIT(eMainEax)->setText("%08X", reg.Eax);
		}
	}
}

static int runDebugExBkPoint() {
	module_s	*module;
	function_s	*function, *oldfunc;

	/* �ϵ��ַ */
	DWORD		address = (DWORD) debug.u.Exception.ExceptionRecord.ExceptionAddress;

	EDIT(eMainBp)->setText("%08X", address);

	/* �ָ��ϴν��뺯���Ķϵ� */
	if(oldfunc = threadFunctionGet(debug.dwThreadId)) {
		hookSetBp(oldfunc);
		threadFunctionSet(debug.dwThreadId, NULL);
	}

	if(((function = hookFind(address, &module)) == NULL) || (function->hook == FALSE)) {
		if(debug.u.Exception.dwFirstChance) {
			//warningFormat(wMain.hwnd, "������һ���ϵ㣬λ���� %08X��", address);
			
			return 0;
		} else {
			return 1;
		}
	}

	/* ���ñ��ν��뺯�� */
	threadFunctionSet(debug.dwThreadId, function);
	/* �ָ����� */
	hookSetCode(function);

	/* �޸� EIP ִ��ԭʼָ�� */
	registerRead();
	reg.Eip		= address;
	reg.EFlags	|= 0x100;
	registerSave();

	/* ���û�����÷��� */
	if(threadRetGet(debug.dwThreadId, NULL) == 0) {
		runShow(function, FALSE);
	}

	/* ��ͣ���������߳� */
	threadSuspend(debug.dwThreadId);

	return 1;
}

static int runDebugExMisc() {
	char *message;

	switch(debug.u.Exception.ExceptionRecord.ExceptionCode) {
		case EXCEPTION_ACCESS_VIOLATION:			message = "EXCEPTION_ACCESS_VIOLATION";			break;
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:		message = "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";	break;
		case EXCEPTION_DATATYPE_MISALIGNMENT:		message = "EXCEPTION_DATATYPE_MISALIGNMENT";	break;
		case EXCEPTION_FLT_DENORMAL_OPERAND:		message = "EXCEPTION_FLT_DENORMAL_OPERAND";		break;
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:			message = "EXCEPTION_FLT_DIVIDE_BY_ZERO";		break;
		case EXCEPTION_FLT_INEXACT_RESULT:			message = "EXCEPTION_FLT_INEXACT_RESULT";		break;
		case EXCEPTION_FLT_INVALID_OPERATION:		message = "EXCEPTION_FLT_INVALID_OPERATION";	break;
		case EXCEPTION_FLT_OVERFLOW:				message = "EXCEPTION_FLT_OVERFLOW";				break;
		case EXCEPTION_FLT_STACK_CHECK:				message = "EXCEPTION_FLT_STACK_CHECK";			break;
		case EXCEPTION_FLT_UNDERFLOW:				message = "EXCEPTION_FLT_UNDERFLOW";			break;
		case EXCEPTION_ILLEGAL_INSTRUCTION:			message = "EXCEPTION_ILLEGAL_INSTRUCTION";		break;
		case EXCEPTION_IN_PAGE_ERROR:				message = "EXCEPTION_IN_PAGE_ERROR";			break;
		case EXCEPTION_INT_DIVIDE_BY_ZERO:			message = "EXCEPTION_INT_DIVIDE_BY_ZERO";		break;
		case EXCEPTION_INT_OVERFLOW:				message = "EXCEPTION_INT_OVERFLOW";				break;
		case EXCEPTION_INVALID_DISPOSITION:			message = "EXCEPTION_INVALID_DISPOSITION";		break;
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:	message = "EXCEPTION_INT_OVERFLOW";				break;
		case EXCEPTION_PRIV_INSTRUCTION:			message = "EXCEPTION_PRIV_INSTRUCTION";			break;
		case EXCEPTION_STACK_OVERFLOW:				message = "EXCEPTION_STACK_OVERFLOW";			break;

		default:
			return 0;
	}

	/* ����ϵͳ���� */
	if(debug.u.Exception.dwFirstChance) {
		return 0;
	} else {
		return 1;
	}

	/*
		errorFormat (
			wMain.hwnd, "��������ʱ���������жϣ�\n[0x%08X] %s @ 0x%08X",
			debug.u.Exception.ExceptionRecord.ExceptionCode,
			message,
			debug.u.Exception.ExceptionRecord.ExceptionAddress
		);
	*/
}


static int runDebug() {
	void (*handle)() = NULL;

	switch(debug.dwDebugEventCode) {
		/* �����˳� */
		case EXIT_PROCESS_DEBUG_EVENT:	handle = runDebugProcessExit;	break;

		/* �̴߳��������� */
		case CREATE_THREAD_DEBUG_EVENT:	handle = runDebugThreadCreate;	break;
		case EXIT_THREAD_DEBUG_EVENT:	handle = runDebugThreadExit;	break;

		/* ��������� DLL ģ�� */
		case LOAD_DLL_DEBUG_EVENT:		handle = runDebugDllLoad;		break;
		case UNLOAD_DLL_DEBUG_EVENT:	handle = runDebugDllUnload;		break;

		/* �쳣������ */
		case EXCEPTION_DEBUG_EVENT:
			switch(debug.u.Exception.ExceptionRecord.ExceptionCode) {
				/* ���� */
				case EXCEPTION_SINGLE_STEP:
					handle = runDebugExSglStep; break;

				/* �ϵ� */
				case EXCEPTION_BREAKPOINT:
					return runDebugExBkPoint();
				/* ����Ӧ�ó����쳣 */
				default:
					return runDebugExMisc();
			}

			break;
	}

	if(handle) {
		handle();
	}

	return 1;
}


void runSuspendResume() {
	/* ��ʼ���� */
	if(RUN == -1) {
		OBJECT(bMainSuspend)->caption("��ͣ����");
		OBJECT(bMainSuspend)->disable(FALSE);

		RUN = 1;

		/* �����ź� */
		symbolRunAsyn(0);

		return;
	}

	/* �����е���ͣ */
	if(RUN) {
		/* ��ͣ�߳� */
		SuspendThread(RT.debug);
		/* ��ͣ���Խ��� */
		processSuspend(RT.handle);

		guiMainStatus(FALSE, TRUE, TRUE);

		OBJECT(bMainSuspend)->caption("�ָ�����");

		OBJECT(bMainDump)->disable(FALSE);
		OBJECT(bMainDisasm)->disable(FALSE);
		OBJECT(bMainMemory)->disable(FALSE);
		OBJECT(bMainHijack)->disable(FALSE);

		if(LIST(lsMainStack)->selected >= 0) {
			OBJECT(bMainViewer)->disable(FALSE);
		}

		RUN = 0;
	} else {
		OBJECT(bMainSuspend)->caption("��ͣ����");

		guiMainStatus(FALSE, TRUE, TRUE);

		/* �ָ����� */
		processResume(RT.handle);
		ResumeThread(RT.debug);

		RUN = 1;
	}
}

threadNewVoid(runDebug) {
	DWORD	status;

	int		mode;
	int		wait	= 1;
	int		inited	= 0;

	char	*error;

	ZeroMemory(&debug, sizeof debug);

	/* ���ؽ��� */
	do {
		mode	= symbolWait();
		error	= NULL;

		/* �����½��� */
		if(mode == -1) {
			error = processFile();
		} else {
			if((error = processSelect(mode)) == NULL) {
				inited = 1;
			}
		}

		/* ���ش�����Ϣ */
		symbolReturn((int) error);

		if(error == NULL) {
			wait = 0;
		}
	} while(wait);

	/* �ȴ�����ѭ�� */
	symbolWait();

	while(1) {
		status = DBG_EXCEPTION_NOT_HANDLED;

		/* �ȴ�������Ϣ */
		if(WaitForDebugEvent(&debug, INFINITE) == FALSE) {
			errorFormat(wMain.hwnd, "Invoke ���ִ����޷���ȡ������Ϣ��Ӧ�ó����ѱ�����ֹ��������룺%d", GetLastError());
		}

		/* �״ν������, ��� Dr0 �ϵ� */
		if(inited == 0) {
			registerRead();

			reg.Dr0 = 0;
			reg.Dr7 = 0;

			registerSave();

			inited = 1;
		}

		/* �ַ�������Ϣ */
		if(debug.dwDebugEventCode) {
			if(runDebug()) {
				status = DBG_CONTINUE;
			}
		}

		/* ����ִ�� */
		ContinueDebugEvent(debug.dwProcessId, debug.dwThreadId, status);
	}
}