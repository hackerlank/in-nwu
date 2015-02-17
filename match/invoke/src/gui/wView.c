/*
	$ Invoke   (C) 2005-2012 MF
	$ wView.c, 0.1.1203
*/

#include "../base.h"
#include "../gui.h"

#define CLASSNAME		"INVOKE_VIEW"
#define TITLE			"����"
#define WIDTH			380
#define HEIGHT			400


static void guiViewDraw(erp_window_s *win) {
	EDIT(eViewAddress)->setText("%08X", packVar(win->param, 0, DWORD));

	EDIT(eaViewData)->readonly(TRUE);
	EDIT(eaViewPointer)->readonly(TRUE);

	typeComboInit(cmbViewType, packPtr(win->param, 1, type_s), TRUE);

	BUTTON(bViewAddress)->click();
}

void guiView(HWND win, DWORD address, type_s *type) {
	packNew(arg, &address, type);

	ZeroMemory(&wView, sizeof wView);

	wView.classname	= CLASSNAME;
	wView.title		= TITLE;
	wView.width		= WIDTH;
	wView.height	= HEIGHT;
	wView.instance	= RT.instance;
	wView.draw		= guiViewDraw;
	wView.center	= TRUE;
	wView.param		= (DWORD) arg;
	wView.parent	= win;

	eViewAddress	= DRAW_EDIT		(wView, VIEW_EDIT_ADDRESS,		 15,  15, 230,  22, NULL);
	bViewAddress	= DRAW_BUTTON	(wView, VIEW_BUTTON_ADDRESS,	-15,  13,  80,  26, "�鿴",	cbViewButtonAddress);
	
	cmbViewType		= DRAW_COMBO	(wView, VIEW_COMBO_TYPE,		 15,  45, 343,  22, cbViewComboType);

	eaViewData		= DRAW_EDITAREA	(wView, VIEW_EDITAREA_DATA,		 15,  85, 343, 150, NULL);
	eaViewPointer	= DRAW_EDITAREA	(wView, VIEW_EDITAREA_POINTER,	 15, 240, 343,  70, NULL);

	bViewOk			= DRAW_BUTTON	(wView, VIEW_BUTTON_OK,			-15, -15, 100,  28, "ȷ��",	cbViewButtonOk);

	erpWindow(&wView);
}


ERP_CALLBACK(cbViewComboType) {
	if(typeComboChange((int) arg, P_COMBO(obj), packPtr(win->param, 1, type_s), TRUE) == 0) {
		warning(win->hwnd, "ѡ������Ч�����͡�");
		COMBO(obj)->select(0);
		
		return;
	}

	BUTTON(bViewAddress)->click();
}

ERP_CALLBACK(cbViewButtonAddress) {
	/* �������� */
	char *data		= NULL;
	/* ָ����� */
	char *pointer	= NULL;

	char	*buffer;
	int		enter;
	DWORD	address;

	buffer	= EDIT(eViewAddress)->getText();
	enter	= sscanf(buffer, "%x", &address);

	DEL(buffer);

	if(enter <= 0) {
		warning(win->hwnd, "����ĵ�ַ��Ч��"); return;
	}

	data = typeReadDetail(address, packPtr(win->param, 1, type_s), &pointer);

	if(data) {
		EDITAREA(eaViewData)->setText("%s", data);

		DEL(data);
	} else {
		EDITAREA(eaViewData)->setText("û����ص����ݿ�����ʾ��");
	}

	if(pointer) {
		EDITAREA(eaViewPointer)->setText("%s", pointer);

		DEL(pointer);
	} else {
		EDITAREA(eaViewPointer)->setText("û��ָ������¼���ݡ�");
	}
}

ERP_CALLBACK(cbViewButtonOk) {
	win->quit(0);
}
