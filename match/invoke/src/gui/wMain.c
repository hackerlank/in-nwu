/*
	$ Invoke   (C) 2005-2012 MF
	$ wMain.c, 0.1.1201
*/

#include "../base.h"
#include "../gui.h"

#define CLASSNAME		"INVOKE_MAIN"
#define TITLE			"Invoke"
#define WIDTH			900
#define HEIGHT			380


/* �Ĵ��� */
static CONTEXT reg;

/* ������Ӧ�ĵ�ַ */
static size_t	argCount	= 0;
static DWORD	*argAddr	= NULL;


static void registerRead() {
	ZeroMemory(&reg, sizeof reg);
	reg.ContextFlags = CONTEXT_FULL;

	GetThreadContext(RT.thread, &reg);
}

static void memoryGet(char *buffer, DWORD addr, size_t size) {
	size_t i;
	unsigned char data[16];

	if(processRead(addr, data, size) == 0) {
		*buffer = 0; return;
	}

	for(i = 0; i < size; i++) {
		sprintf(buffer + i * 3, "%02X ", data[i]);
	}
}


static void guiMainDraw(erp_window_s *win) {
	guiMainTitle(NULL, NULL, 0);
	guiMainStatus(FALSE, TRUE, TRUE);

	OBJECT(bMainHijack)->disable(FALSE);

	EDIT(eMainModule)->readonly(TRUE);
	EDIT(eMainOrdinal)->readonly(TRUE);
	EDIT(eMainFuncname)->readonly(TRUE);
	EDIT(eMainBp)->readonly(TRUE);

	if(RT.entry) {
		OBJECT(bMainSuspend)->caption("��������");
	}
}

static void * guiMainDestroy(erp_window_s *win, DWORD arg) {
	/* �ָ����̺͵����߳� */
	processResume(RT.handle);
	ResumeThread(RT.debug);

	return NULL;
}

void guiMain() {
	char	*title[]	= {"��ַ", "�ڴ�����", "����", "����", "����ֵ"};
	size_t	width[]		= {70, 130, 120, 160, 370};

	ZeroMemory(&wMain, sizeof wMain);

	wMain.classname	= CLASSNAME;
	wMain.title		= TITLE;
	wMain.width		= WIDTH;
	wMain.height	= HEIGHT;
	wMain.instance	= RT.instance;
	wMain.draw		= guiMainDraw;
	wMain.destroy	= guiMainDestroy;
	wMain.center	= TRUE;

	bMainSuspend	= DRAW_BUTTON	(wMain, MAIN_BUTTON_SUSPEND,	   5,   5,  90,  28, "�ָ�����",		cbMainButtonSuspend);
	bMainDump		= DRAW_BUTTON	(wMain, MAIN_BUTTON_DUMP,		 100,   5,  90,  28, "��������",		cbMainButtonDump);
	bMainHijack		= DRAW_BUTTON	(wMain, MAIN_BUTTON_HIJACK,		  -5,   5, 120,  28, "����ٳֺ���",	cbMainButtonHijack);

	bMainRegister	= DRAW_BUTTON	(wMain, MAIN_BUTTON_REGISTER,	 280,   5,  80,  28, "�Ĵ���",		cbMainButtonRegister);
	bMainDisasm		= DRAW_BUTTON	(wMain, MAIN_BUTTON_DISASM,		 365,   5,  80,  28, "�����",		cbMainButtonDisasm);
	bMainMemory		= DRAW_BUTTON	(wMain, MAIN_BUTTON_MEMORY,		 450,   5,  80,  28, "�ڴ�",			cbMainButtonMemory);
	bMainViewer		= DRAW_BUTTON	(wMain, MAIN_BUTTON_VIEWER,		 535,   5,  90,  28, "�鿴����",		cbMainButtonViewer);

	lsMainStack		= DRAW_LIST		(wMain, MAIN_LIST_STACK,		   7,  40, 880, 200, title, width, sizeof title / sizeof(char *), cbMainListStack);

	lbMainModule	= DRAW_LABEL	(wMain, MAIN_LABEL_MODULE,		  10, 260,  80,  20, "ģ�飺");
	lbMainModule	= DRAW_LABEL	(wMain, MAIN_LABEL_FUNCTION,	  10, 285,  80,  20, "������");
	lbMainBp		= DRAW_LABEL	(wMain, MAIN_LABEL_BP,			 470, 258, 100,  20, "�ϵ��ַ��");
	lbMainEsp		= DRAW_LABEL	(wMain, MAIN_LABEL_ESP,			 570, 258, 100,  20, "ESP ջ����");
	lbMainRet		= DRAW_LABEL	(wMain, MAIN_LABEL_RET,			 670, 258, 100,  20, "���ص�ַ��");
	lbMainEax		= DRAW_LABEL	(wMain, MAIN_LABEL_EAX,			 780, 258, 120,  20, "EAX �������ݣ�");

	eMainModule		= DRAW_EDIT		(wMain, MAIN_EDIT_MODULE,		  60, 255, 350,  22, NULL);
	eMainOrdinal	= DRAW_EDIT		(wMain, MAIN_EDIT_ORDINAL,		  60, 280,  80,  22, NULL);
	eMainFuncname	= DRAW_EDIT		(wMain, MAIN_EDIT_FUNCNAME,		 145, 280, 265,  22, NULL);
	eMainBp			= DRAW_EDIT		(wMain, MAIN_EDIT_BP,			 470, 275,  90,  22, NULL);
	eMainEsp		= DRAW_EDIT		(wMain, MAIN_EDIT_ESP,			 570, 275,  90,  22, NULL);
	eMainRet		= DRAW_EDIT		(wMain, MAIN_EDIT_RET,			 670, 275,  90,  22, NULL);
	eMainEax		= DRAW_EDIT		(wMain, MAIN_EDIT_EAX,			 780, 275, 105,  22, NULL);

	bMainContinue	= DRAW_BUTTON	(wMain, MAIN_BUTTON_CONTINUE,	   5,  -5, 100,  28, "��������",		cbMainButtonContinue);
	bMainRuntoret	= DRAW_BUTTON	(wMain, MAIN_BUTTON_RUNTORET,	 110,  -5, 110,  28, "ִ�е�����",	cbMainButtonRuntoret);
	bMainRuntocon	= DRAW_BUTTON	(wMain, MAIN_BUTTON_RUNTOCON,	 225,  -5, 110,  28, "���غ����",	cbMainButtonRuntocon);
	bMainReturn		= DRAW_BUTTON	(wMain, MAIN_BUTTON_RETURN,		 340,  -5, 100,  28, "ֱ�ӷ���",		cbMainButtonReturn);

	bMainCalc		= DRAW_BUTTON	(wMain, MAIN_BUTTON_CALC,		  -5,  -5,  90,  28, "ˢ�¶�ջ",		cbMainButtonCalc);
	bMainFunction	= DRAW_BUTTON	(wMain, MAIN_BUTTON_FUNCTION,	-100,  -5,  90,  28, "����ԭ��",		cbMainButtonFunction);

	bMainAbout		= DRAW_BUTTON	(wMain, MAIN_BUTTON_ABOUT,		-240,  -5,  80,  28, "����",			cbMainButtonAbout);

	erpWindow(&wMain);
}

void guiMainTitle(char *module, char *function, size_t ordinal) {
	/* �̶����� */
	static char *fixed = NULL;

	if(fixed == NULL) {
		if(uacAdminCheck()) {
			fixed = TITLE " [����Ա]";
		} else {
			fixed = TITLE;
		}
	}

	if(module) {
		if(function) {
			wMain.caption("%s@%s - %s", function, module, fixed);
		} else {
			wMain.caption("#%ld@%s - %s", ordinal, module, fixed);
		}
	} else {
		wMain.caption(fixed);
	}
}

void guiMainStatus(BOOL suspend, BOOL proc, BOOL run) {
	OBJECT(bMainSuspend)->disable(suspend);

	OBJECT(bMainDump)->disable(proc);
	OBJECT(bMainHijack)->disable(proc);
	OBJECT(bMainRegister)->disable(proc);
	OBJECT(bMainDisasm)->disable(proc);
	OBJECT(bMainMemory)->disable(proc);

	OBJECT(bMainContinue)->disable(run);
	OBJECT(bMainRuntoret)->disable(run);
	OBJECT(bMainRuntocon)->disable(run);
	OBJECT(bMainReturn)->disable(run);
	OBJECT(bMainCalc)->disable(run);
	OBJECT(bMainFunction)->disable(run);

	OBJECT(eMainEsp)->disable(run);
	OBJECT(eMainRet)->disable(run);
	OBJECT(eMainEax)->disable(run);

	if((run == FALSE) && (LIST(lsMainStack)->selected >= 0)) {
		OBJECT(bMainViewer)->disable(FALSE);
	} else {
		OBJECT(bMainViewer)->disable(TRUE);
	}
}


ERP_CALLBACK(cbMainListStack) {
	switch(evt) {
		case SELECT:	break;
		default:		return;
	}

	if((arg == -1) || OBJECT(bMainHijack)->disabled) {
		OBJECT(bMainViewer)->disable(TRUE);
	} else {
		OBJECT(bMainViewer)->disable(FALSE);
	}
}

ERP_CALLBACK(cbMainButtonSuspend) {
	guiMainStatus(TRUE, TRUE, TRUE);
	runSuspendResume();
}

ERP_CALLBACK(cbMainButtonDump) {
	moduleDump();
}

ERP_CALLBACK(cbMainButtonRegister) {
	registerRead();
	guiReg(win->hwnd, &reg);
}

ERP_CALLBACK(cbMainButtonDisasm) {
	registerRead();

	/* ��ʾ����ര�� */
	guiAsm(win->hwnd, "����൱ǰ EIP", reg.Eip, NULL, FALSE, 0, 0);
}

ERP_CALLBACK(cbMainButtonMemory) {
	DWORD	addr;
	int		index = LIST(lsMainStack)->selected;

	if((index < 0) || (index >= (int) argCount)) {
		registerRead();

		/* ��ջ���� */
		addr = reg.Esp;
	} else {
		addr = argAddr[index];
	}

	guiMem(win->hwnd, addr);
}

ERP_CALLBACK(cbMainButtonHijack) {
	guiHij(win->hwnd);
}

ERP_CALLBACK(cbMainButtonCalc) {
	int		i = 0, enter = -1;
	DWORD	addr, esp, ret;

	char	buffer[2][128];
	char	*data[] = {buffer[0], buffer[1], NULL, NULL, NULL};
	char	*input;

	function_s	*func = RT.function;
	argument_s	*argv;
	type_s		type;

	/* �ɱ������ */
	typeSet(&type, US_INT_32, 0, 0);

	/* ��ȡ���õ� ESP */
	input = EDIT(eMainEsp)->getText();

	sscanf(input, "%x", &enter);
	DEL(input);

	if(enter < 0) {
		warning(win->hwnd, "��������ȷ�� ESP ջ����ַ��"); return;
	} else {
		esp = (DWORD) enter;
	}

	/* ���㷵�ص�ַ */
	if(func->hijack == NULL) {
		warning(win->hwnd, "����ԭ���ѱ�ɾ����"); return;
	}

	if(func->hijack->esp) {
		addr = esp + 4;
	} else {
		addr = esp;
	}

	/* ��ȡ����ʾ���ص�ַ */
	processRead(addr, &ret, sizeof ret);
	EDIT(eMainRet)->setText("%08X", ret);

	/* ESP ���ص�ַ���� */
	esp = addr + 4;

	LIST(lsMainStack)->clear();

	argCount = 0;
	DEL(argAddr);

	if(argv = func->hijack->argList) {
		/* ����ǿɱ���� */
		if(func->hijack->vararg) {
			argCount = func->hijack->argCount + 4;
		} else {
			argCount = func->hijack->argCount;
		}

		NEW(argAddr, DWORD, argCount);

		while(argv) {
			sprintf(data[0], "%08X", esp);
			memoryGet(data[1], esp, argv->type.size);

			data[2] = argv->name;
			data[3] = type2string(&argv->type);
			data[4] = typeReadSimple(esp, &argv->type);

			LIST(lsMainStack)->append(data);

			DEL(data[3]);
			DEL(data[4]);

			/* ���� ESP */
			argAddr[i] = esp;

			/* ESP ���� */
			esp += argv->type.size;

			argv = argv->next;

			i++;
		}
	} else {
		argCount = 4;

		NEW(argAddr, DWORD, argCount);
	}

	/* �ɱ�α� */
	for(; i < (int) argCount; i++) {
		sprintf(data[0], "%08X", esp);
		memoryGet(data[1], esp, type.size);

		data[2] = "���/��ջ";
		data[3] = type2string(&type);
		data[4] = typeReadSimple(esp, &type);

		LIST(lsMainStack)->append(data);

		DEL(data[3]);
		DEL(data[4]);

		/* ���� ESP */
		argAddr[i] = esp;

		/* ESP ���� */
		esp += type.size;
	}
}

ERP_CALLBACK(cbMainButtonFunction) {
	if(RT.function && RT.function->hijack) {
		guiPrt(RT.function->hijack, win->hwnd);

		BUTTON(bMainCalc)->click();
	}
}

ERP_CALLBACK(cbMainButtonViewer) {
	int		i = 0;
	int		index = LIST(lsMainStack)->selected;

	type_s		type, *stype = &type;
	argument_s	*argv;

	/* �ɱ������ */
	typeSet(&type, US_INT_32, 0, 0);

	if((index < 0) || (index >= (int) argCount)) {
		return;
	}

	/* �ж��ǲ��ǲ��� */
	if(RT.function && RT.function->hijack) {
		argv = RT.function->hijack->argList;

		while(argv) {
			if(i == index) {
				stype = &argv->type; break;
			}

			i++;
			argv = argv->next;
		}
	}

	guiView(win->hwnd, argAddr[index], stype);
}

ERP_CALLBACK(cbMainButtonContinue) {
	symbolRunAsyn(0);
}

ERP_CALLBACK(cbMainButtonRuntoret) {
	symbolRunAsyn(1);
}

ERP_CALLBACK(cbMainButtonRuntocon) {
	symbolRunAsyn(2);
}

ERP_CALLBACK(cbMainButtonReturn) {
	symbolRunAsyn(3);
}

ERP_CALLBACK(cbMainButtonAbout) {
	notice(win->hwnd, "Invoke   (C) 2005-2012 MF\n\n��ǰӦ�ó���汾Ϊ 1.0.1204��\n��Ϊ������ѧ��ʮ��������Ļ������ڲ����汾��");
}
