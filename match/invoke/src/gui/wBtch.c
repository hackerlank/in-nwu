/*
	$ Invoke   (C) 2005-2012 MF
	$ wBtch.c, 0.1.1125
*/

#include "../base.h"
#include "../gui.h"

#define CLASSNAME	"INVOKE_BATCH"
#define TITLE		"��������"
#define WIDTH		350
#define HEIGHT		250


/* Ĭ������ */
static DWORD	defOffset		= 1;
static BOOL		defMoveStack	= TRUE;
static BOOL		defMoveCall		= TRUE;
static BOOL		defMoveJump		= TRUE;
static BOOL		defMoveRet		= TRUE;


static void guiBtchDraw(erp_window_s *win) {
	EDIT(eBtchOffset)->setText("%ld", defOffset);

	CHECK(cBtchMoveStack)->check(defMoveStack);
	CHECK(cBtchMoveCall)->check(defMoveCall);
	CHECK(cBtchMoveJump)->check(defMoveJump);
	CHECK(cBtchMoveRet)->check(defMoveRet);
}

static void * guiBtchDestroy(erp_window_s *win, DWORD arg) {
	return (void *) arg;
}

int guiBtch(HWND parent, DWORD *offset, BOOL *moveStack, BOOL *moveCall, BOOL *moveJump, BOOL *moveRet) {
	packNew(arg, offset, moveStack, moveCall, moveJump, moveRet);

	ZeroMemory(&wBtch, sizeof wBtch);

	wBtch.classname	= CLASSNAME;
	wBtch.title		= TITLE;
	wBtch.width		= WIDTH;
	wBtch.height	= HEIGHT;
	wBtch.instance	= RT.instance;
	wBtch.draw		= guiBtchDraw;
	wBtch.destroy	= guiBtchDestroy;
	wBtch.center	= TRUE;
	wBtch.tool		= TRUE;
	wBtch.parent	= parent;
	wBtch.param		= (DWORD) arg;

	lbBtchOffset	= DRAW_LABEL	(wBtch, BTCH_LABEL_OFFSET,		 15,  18, 200, 20, "�ڵڼ���ָ������ INT3 �ϵ㣺");
	lbBtchMove		= DRAW_LABEL	(wBtch, BTCH_LABEL_MOVE,		 15,  60, 200, 20, "�����������ǰ���öϵ㣺");

	eBtchOffset		= DRAW_EDIT		(wBtch, BTCH_EDIT_OFFSET,		190,  15, 80,  20, NULL);

	cBtchMoveStack	= DRAW_CHECK	(wBtch, BTCH_CHECK_MOVE_STACK,	 30,  78, 200, 20, "��ջ����ѹ��򵯳�ʱ",			NULL);
	cBtchMoveCall	= DRAW_CHECK	(wBtch, BTCH_CHECK_MOVE_CALL,	 30,  95, 200, 20, "CALL ָ����������ӳ���ʱ",		NULL);
	cBtchMoveJump	= DRAW_CHECK	(wBtch, BTCH_CHECK_MOVE_JUMP,	 30, 112, 200, 20, "ִ����������������תָ��ʱ",		NULL);
	cBtchMoveRet	= DRAW_CHECK	(wBtch, BTCH_CHECK_MOVE_RET,	 30, 129, 200, 20, "ʹ�� RET ָ���ʱ",			NULL);

	bBtchOk			= DRAW_BUTTON	(wBtch, BTCH_BUTTON_OK,			-15, -15,  80, 28, "ȷ��", cbBtchButtonOk);

	return (int) erpWindow(&wBtch);
}


ERP_CALLBACK(cbBtchButtonOk) {
	int		result, offset;
	char	*input;

	input	= EDIT(eBtchOffset)->getText();
	result	= sscanf(input, "%ld", &offset);

	DEL(input);

	if((result == 0) || (offset <= 0)) {
		warning(win->hwnd, "����Ķϵ�λ����Ч��");
		
		return;
	}

	defOffset		= (DWORD) offset;
	defMoveStack	= CHECK(cBtchMoveStack)->checked;
	defMoveCall		= CHECK(cBtchMoveCall)->checked;
	defMoveJump		= CHECK(cBtchMoveJump)->checked;
	defMoveRet		= CHECK(cBtchMoveRet)->checked;

	packVar(win->param, 0, DWORD)	= defOffset;
	packVar(win->param, 1, BOOL)	= defMoveStack;
	packVar(win->param, 2, BOOL)	= defMoveCall;
	packVar(win->param, 3, BOOL)	= defMoveJump;
	packVar(win->param, 4, BOOL)	= defMoveRet;

	win->quit(1);
}
