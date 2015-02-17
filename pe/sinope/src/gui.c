/*
	$ Sinope PE Loader  (C) 2005-2012 mfboy
	$ gui.c, 0.1.1104
*/

#pragma comment (linker, "/subsystem:\"windows\"")

#define _SNP_GUI_

#include <stdio.h>
#include <Windows.h>
#include "base.h"
#include "image.h"
#include "gui.h"


/* main */
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	ZeroMemory(&RUNTIME,	sizeof RUNTIME);

	ZeroMemory(THREAD,		sizeof(thread_s));
	ZeroMemory(BP,			sizeof(bp_s));
	ZeroMemory(SECTION,		sizeof(section_s));
	ZeroMemory(IMPORT,		sizeof(import_s));
	ZeroMemory(MODULE,		sizeof(module_s));

	return gui(hInstance);
}


static int gui(HINSTANCE instance) {
	/* ���б���� */
	char *bpTitle[]			= {"", "�ڴ��ַ", "RVA", "����", "��ע"};
	char *sectionTitle[]	= {"����", "RVA", "�ڴ��ַ", "�ļ�ƫ��", "��С", "��Դ"};
	char *importTitle[]		= {"DLL", "����", "��ǰ���", "ʵ�����", "IAT"};
	char *moduleTitle[]		= {"ģ��", "��ʼ��ַ", "��С"};
	/* �����п�� */
	size_t bpWidth[]		= {30, 135, 135, 100, 120};
	size_t sectionWidth[]	= {100, 70, 70, 70, 65, 50};
	size_t importWidth[]	= {85, 145, 65, 65, 65};
	size_t moduleWidth[]	= {305, 60, 60};

	ZeroMemory(&wMain, sizeof wMain);

	wMain.classname	= WIN_CLASS;
	wMain.title		= WIN_TITLE;
	wMain.width		= WIN_WIDTH;
	wMain.height	= WIN_HEIGHT;
	wMain.instance	= instance;
	wMain.draw		= winDraw;
	wMain.keyboard	= winKeyboard;

	/* ������ */
	bExecute		= DRAW_BUTTON	(wMain, BUTTON_EXECUTE,		   5, 5, 100, 28, "ѡ���ļ�",			cbButtonExecute);
	bDump			= DRAW_BUTTON	(wMain, BUTTON_DUMP,		 110, 5, 100, 28, "�����ڴ澵��",		cbButtonDump);
	bSuspend		= DRAW_BUTTON	(wMain, BUTTON_SUSPEND,		 240, 5, 100, 28, "��ͣ (F5)",		cbButtonSuspend);
	bContinue		= DRAW_BUTTON	(wMain, BUTTON_CONTINUE,	 345, 5, 100, 28, "���� (F7)",		cbButtonContinue);
	bSingleStep		= DRAW_BUTTON	(wMain, BUTTON_SINGLE_STEP,	 450, 5, 100, 28, "����ִ�� (F8)",	cbButtonSingleStep);
	bRefresh		= DRAW_BUTTON	(wMain, BUTTON_REFRESH,		-230, 5,  70, 28, "ˢ��",			cbButtonRefresh);
	bRegister		= DRAW_BUTTON	(wMain, BUTTON_REGISTER,	-155, 5,  70, 28, "�Ĵ���",			cbButtonRegister);
	bConfig			= DRAW_BUTTON	(wMain, BUTTON_CONFIG,		 -80, 5,  70, 28, "����",			cbButtonConfig);
	bAbout			= DRAW_BUTTON	(wMain, BUTTON_ABOUT,		  -5, 5,  70, 28, "����",			cbButtonAbout);

	/* ָ���� */
	lbRva			= DRAW_LABEL	(wMain, LABEL_RVA,		 13, 42,  50, 14, "RVA��");
	lbEip			= DRAW_LABEL	(wMain, LABEL_EIP,		113, 42,  50, 14, "EIP��");
	lbSection		= DRAW_LABEL	(wMain, LABEL_SECTION,	223, 42,  50, 14, "���Σ�");
	lbCode			= DRAW_LABEL	(wMain, LABEL_CODE,		363, 42,  80, 14, "����ָ�");
	eRva			= DRAW_EDIT		(wMain, EDIT_RVA,		 10, 57,  90, 20, NULL);
	eEip			= DRAW_EDIT		(wMain, EDIT_EIP,		110, 57,  90, 20, NULL);
	eSection		= DRAW_EDIT		(wMain, EDIT_SECTION,	220, 57, 120, 20, NULL);
	eCode			= DRAW_EDIT		(wMain, EDIT_CODE,		360, 57, 250, 20, NULL);

	/* �߳�ѡ���� */
	cmbThread		= DRAW_COMBO	(wMain, COMBO_THREAD,	-10, 56, 380, 22, cbComboThread);

	/* �ϵ��� */
	lsBp			= DRAW_LIST		(wMain, LIST_BP,				 10,  90, 550, 280, bpTitle, bpWidth, sizeof bpTitle / sizeof(char *), cbListBp);
	bBpAdd			= DRAW_BUTTON	(wMain, BUTTON_BP_ADD,			460, 375, 100,  26, "�� ��",						cbButtonBpAdd);
	bBpEdit			= DRAW_BUTTON	(wMain, BUTTON_BP_EDIT,			460, 401, 100,  26, "�� ��",						cbButtonBpEdit);
	bBpDelete		= DRAW_BUTTON	(wMain, BUTTON_BP_DELETE,		460, 427, 100,  26, "ɾ ��",						cbButtonBpDelete);
	cBpDynmCode		= DRAW_CHECK	(wMain, CHECK_BP_DYNM_CODE,		 13, 381, 180,  22, "ִ�б���̬�޸ĵĴ���ʱ�ж�",	cbCheckBpDynmCode);
	cBpDynmMemory	= DRAW_CHECK	(wMain, CHECK_BP_DYNM_MEMORY,	 13, 403, 180,  22, "��ת����̬�����ڴ���ʱ�ж�",	cbCheckBpDynmMemory);
	cBpStride		= DRAW_CHECK	(wMain, CHECK_BP_STRIDE,		 13, 425, 180,  22, "��������תʱ�ж�",			cbCheckBpStride);
	rBpAlways		= DRAW_RADIO	(wMain, RADIO_BP_ALWAYS,		220, 381, 190,  22, "���Ǽ�����ĸĶ�",			1, cbRadioBpAlways);
	rBpStride		= DRAW_RADIO	(wMain, RADIO_BP_STRIDE,		220, 403, 190,  22, "��������תʱ������ĸĶ�",	1, cbRadioBpStride);
	rBpReturn		= DRAW_RADIO	(wMain, RADIO_BP_RETURN,		220, 425, 190,  22, "���س������ʱ������ĸĶ�",	1, cbRadioBpReturn);

	/* ������ */
	lsSection		= DRAW_LIST		(wMain, LIST_SECTION,			 -10,  90, 455, 150, sectionTitle, sectionWidth, sizeof sectionTitle / sizeof(char *), cbListSection);
	bSectionExport	= DRAW_BUTTON	(wMain, BUTTON_SECTION_EXPORT,	 -10, 245,  70,  26, "����",			cbButtonSectionExport);
	bSectionImport	= DRAW_BUTTON	(wMain, BUTTON_SECTION_IMPORT,	 -85, 245,  70,  26, "����",			cbButtonSectionImport);
	bSectionBp		= DRAW_BUTTON	(wMain, BUTTON_SECTION_BP,		-160, 245,  70,  26, "�ϵ�",			cbButtonSectionBp);
	bSectionAlloc	= DRAW_BUTTON	(wMain, BUTTON_SECTION_ALLOC,	-335, 245, 130,  26, "���붯̬�ڴ�",	cbButtonSectionAlloc);

	/* ������� */
	lsImport		= DRAW_LIST		(wMain, LIST_IMPORT,			 -10, 285, 455, 180, importTitle, importWidth, sizeof importTitle / sizeof(char *), cbListImport);
	bImportEdit		= DRAW_BUTTON	(wMain, BUTTON_IMPORT_EDIT,		 -10, 470, 100,  26, "�༭���",		cbButtonImportEdit);
	bImportFix		= DRAW_BUTTON	(wMain, BUTTON_IMPORT_FIX,		-115, 470, 100,  26, "�ָ����",		cbButtonImportFix);
	bImportBp		= DRAW_BUTTON	(wMain, BUTTON_IMPORT_BP,		-220, 470,  70,  26, "�ϵ�",			cbButtonImportBp);
	bImportRebuild	= DRAW_BUTTON	(wMain, BUTTON_IMPORT_REBUILD,	-365, 470, 100,  26, "�ؽ������",	cbButtonImportRebuild);

	/* ģ���� */
	lsModule		= DRAW_LIST		(wMain, LIST_MODULE,			 -10, 510, 455, 110, moduleTitle, moduleWidth, sizeof moduleTitle / sizeof(char *), cbListModule);
	bModuleExport	= DRAW_BUTTON	(wMain, BUTTON_MODULE_EXPORT,	 -10, 625,  70,  26, "����", cbButtonModuleExport);
	bModuleImport	= DRAW_BUTTON	(wMain, BUTTON_MODULE_IMPORT,	 -85, 625,  70,  26, "����", cbButtonModuleImport);
	bModuleBp		= DRAW_BUTTON	(wMain, BUTTON_MODULE_BP,		-160, 625,  70,  26, "�ϵ�", cbButtonModuleBp);

	/* ������Ϣ */
	eaWorking		= DRAW_EDITAREA	(wMain, EDITAREA_WORKING, 10, -10, 550, 185, "׼��������");

	erpWindow(&wMain);
	erpDestory(&wMain);

	return 0;
}

static void winDraw(erp_window_s *win) {
	EDIT(eRva)->readonly(TRUE);
	EDIT(eEip)->readonly(TRUE);
	EDIT(eSection)->readonly(TRUE);
	EDIT(eCode)->readonly(TRUE);
	EDITAREA(eaWorking)->readonly(TRUE);

	CHECK(cBpDynmCode)->check(TRUE);
	CHECK(cBpDynmMemory)->check(TRUE);
	CHECK(cBpStride)->check(TRUE);

	RADIO(rBpStride)->select();

	enable(TRUE, FALSE, FALSE, FALSE);
}

static void winKeyboard(erp_window_s *win, DWORD key) {
	switch(key) {
		case VK_F5: BUTTON(bSuspend)->click();		break;
		case VK_F7: BUTTON(bContinue)->click();		break;
		case VK_F8: BUTTON(bSingleStep)->click();	break;
	}
}


void enable(BOOL execute, BOOL suspend, BOOL command, BOOL list) {
	/* ���� */
	BOOL bp			= TRUE;
	BOOL section	= TRUE;
	BOOL import		= TRUE;
	BOOL module		= TRUE;

	if(execute) {
		OBJECT(bExecute)->disable(FALSE);
	} else {
		OBJECT(bExecute)->disable(TRUE);
	}

	if(suspend) {
		OBJECT(bSuspend)->disable(FALSE);
	} else {
		OBJECT(bSuspend)->disable(TRUE);
	}

	if(command) {
		command = FALSE;
	} else {
		command = TRUE;
	}

	OBJECT(bDump)->disable(command);
	OBJECT(bContinue)->disable(command);
	OBJECT(bSingleStep)->disable(command);
	OBJECT(bRefresh)->disable(command);
	OBJECT(bRegister)->disable(command);
	OBJECT(bConfig)->disable(command);
	OBJECT(cmbThread)->disable(command);

	if(list == TRUE) {
		if(RUNTIME.alive == FALSE) {
			list = FALSE;
		}
	}

	if(list == TRUE) {
		if(LIST(lsBp)->selected >= 0) {
			bp = FALSE;
		}
		if(LIST(lsSection)->selected >= 0) {
			section = FALSE;
		}
		if(LIST(lsImport)->selected >= 0) {
			import = FALSE;
		}
		if(LIST(lsModule)->selected >= 0) {
			module = FALSE;
		}
	}

	OBJECT(bBpAdd)->disable(command);
	OBJECT(bBpEdit)->disable(bp);
	OBJECT(bBpDelete)->disable(bp);
	OBJECT(cBpDynmCode)->disable(command);
	OBJECT(cBpDynmMemory)->disable(command);
	OBJECT(cBpStride)->disable(command);
	OBJECT(rBpAlways)->disable(command);
	OBJECT(rBpStride)->disable(command);
	OBJECT(rBpReturn)->disable(command);

	OBJECT(bSectionExport)->disable(section);
	OBJECT(bSectionImport)->disable(section);
	OBJECT(bSectionBp)->disable(section);
	OBJECT(bSectionAlloc)->disable(command);

	OBJECT(bImportEdit)->disable(import);
	OBJECT(bImportFix)->disable(import);
	OBJECT(bImportBp)->disable(import);
	OBJECT(bImportRebuild)->disable(command);

	OBJECT(bModuleExport)->disable(module);
	OBJECT(bModuleImport)->disable(module);
	OBJECT(bModuleBp)->disable(module);
}


BOOL guiDump() {
	ZeroMemory(&wDump, sizeof wDump);

	wDump.classname		= DUMP_CLASS;
	wDump.title			= DUMP_TITLE;
	wDump.width			= DUMP_WIDTH;
	wDump.height		= DUMP_HEIGHT;
	wDump.instance		= wMain.instance;
	wDump.parent		= wMain.hwnd;
	wDump.draw			= guiDumpDraw;
	wDump.destory		= guiDumpDestory;
	wDump.center		= TRUE;
	wDump.tool			= TRUE;

	lbDumpExport		= DRAW_LABEL	(wDump, DUMP_LABEL_EXPORT,			 20,  20, 300, 20, "������");
	cDumpRealSize		= DRAW_CHECK	(wDump, DUMP_CHECK_REAL_SIZE,		 30,  40, 400, 20, "ʹ��ʵ��ռ���ڴ��С������ռ���̿ռ��С�����ν��е���",	NULL);
	cDumpSectionAlign	= DRAW_CHECK	(wDump, DUMP_CHECK_SECTION_ALIGN,	 30,  60, 400, 20, "���Ը��� PE �еĶ������öԵ��������ݽ��ж���",			NULL);
	cDumpCopy			= DRAW_CHECK	(wDump, DUMP_CHECK_COPY,			 30,  80, 400, 20, "ֱ�ӶԾ�����и��ƶ��Ƕ�������η����ķ�ʽ���е���",		NULL);

	lbDumpImport		= DRAW_LABEL	(wDump, DUMP_LABEL_IMPORT,			 20, 120, 300, 20, "�����");
	cDumpFillIat		= DRAW_CHECK	(wDump, DUMP_CHECK_FILL_IAT,		 30, 140, 400, 20, "�ÿհ���䵼����� IAT ����ָ��",					NULL);

	bDumpOk				= DRAW_BUTTON	(wDump, DUMP_BUTTON_OK,				-20, -20,  90, 26, "��������",										cbDumpButonOk);

	erpWindow(&wDump);
	erpDestory(&wDump);

	if(wDump.s.quit == 1) {
		return TRUE;
	} else {
		return FALSE;
	}
}

static void guiDumpDraw(erp_window_s *win) {
	if(CONFIG.dump.realSize) {
		CHECK(cDumpRealSize)->check(TRUE);
	}
	if(CONFIG.dump.sectionAlign) {
		CHECK(cDumpSectionAlign)->check(TRUE);
	}
	if(CONFIG.dump.copy) {
		CHECK(cDumpCopy)->check(TRUE);
	}

	if(CONFIG.dump.fillIat) {
		CHECK(cDumpFillIat)->check(TRUE);
	}
}

static void * guiDumpDestory(erp_window_s *win, DWORD arg) {
	if(arg == 1) {
		CONFIG.dump.realSize		= CHECK(cDumpRealSize)->checked;
		CONFIG.dump.sectionAlign	= CHECK(cDumpSectionAlign)->checked;
		CONFIG.dump.copy			= CHECK(cDumpCopy)->checked;

		CONFIG.dump.fillIat			= CHECK(cDumpFillIat)->checked;
	}

	return NULL;
}


void guiRegister(CONTEXT *reg) {
	ZeroMemory(&wRegister, sizeof wRegister);

	wRegister.classname	= REGISTER_CLASS;
	wRegister.title		= REGISTER_TITLE;
	wRegister.width		= REGISTER_WIDTH;
	wRegister.height	= REGISTER_HEIGHT;
	wRegister.instance	= wMain.instance;
	wRegister.parent	= wMain.hwnd;
	wRegister.draw		= guiRegisterDraw;
	wRegister.center	= TRUE;
	wRegister.tool		= TRUE;
	wRegister.param		= (DWORD) reg;

	lbRegisterHex		= DRAW_LABEL	(wRegister, REGISTER_LABEL_HEX,		100,  20,  60, 20, "ʮ������");
	lbRegisterDec		= DRAW_LABEL	(wRegister, REGISTER_LABEL_DEC,		230,  20,  60, 20, "ʮ����");

	lbRegisterEax		= DRAW_LABEL	(wRegister, REGISTER_LABEL_EAX,		 35,  40,  60, 20, "EAX");
	lbRegisterEcx		= DRAW_LABEL	(wRegister, REGISTER_LABEL_ECX,		 35,  65,  60, 20, "ECX");
	lbRegisterEdx		= DRAW_LABEL	(wRegister, REGISTER_LABEL_EDX,		 35,  90,  60, 20, "EDX");
	lbRegisterEbx		= DRAW_LABEL	(wRegister, REGISTER_LABEL_EBX,		 35, 115,  60, 20, "EBX");
	lbRegisterEsi		= DRAW_LABEL	(wRegister, REGISTER_LABEL_ESI,		 35, 140,  60, 20, "ESI");
	lbRegisterEdi		= DRAW_LABEL	(wRegister, REGISTER_LABEL_EDI,		 35, 165,  60, 20, "EDI");
	lbRegisterEsp		= DRAW_LABEL	(wRegister, REGISTER_LABEL_ESP,		 35, 190,  60, 20, "ESP");
	lbRegisterEbp		= DRAW_LABEL	(wRegister, REGISTER_LABEL_EBP,		 35, 215,  60, 20, "EBP");
	lbRegisterEip		= DRAW_LABEL	(wRegister, REGISTER_LABEL_EIP,		 35, 240,  60, 20, "EIP");

	lbRegisterCs		= DRAW_LABEL	(wRegister, REGISTER_LABEL_CS,		 37, 280,  60, 20, "CS");
	lbRegisterDs		= DRAW_LABEL	(wRegister, REGISTER_LABEL_DS,		 37, 305,  60, 20, "DS");
	lbRegisterSs		= DRAW_LABEL	(wRegister, REGISTER_LABEL_SS,		 37, 330,  60, 20, "SS");
	lbRegisterEs		= DRAW_LABEL	(wRegister, REGISTER_LABEL_ES,		 37, 355,  60, 20, "ES");
	lbRegisterFs		= DRAW_LABEL	(wRegister, REGISTER_LABEL_FS,		 37, 380,  60, 20, "FS");
	lbRegisterGs		= DRAW_LABEL	(wRegister, REGISTER_LABEL_GS,		 37, 405,  60, 20, "GS");

	lbRegisterEflags	= DRAW_LABEL	(wRegister, REGISTER_LABEL_EFLAGS,	 25, 445,  60, 20, "EFLAGS");

	eRegisterEaxHex		= DRAW_EDIT		(wRegister, REGISTER_EDIT_EAX_HEX,	 90,  38, 110, 20, NULL);
	eRegisterEaxDec		= DRAW_EDIT		(wRegister, REGISTER_EDIT_EAX_DEC,	220,  38, 110, 20, NULL);
	eRegisterEcxHex		= DRAW_EDIT		(wRegister, REGISTER_EDIT_ECX_HEX,	 90,  63, 110, 20, NULL);
	eRegisterEcxDec		= DRAW_EDIT		(wRegister, REGISTER_EDIT_ECX_DEC,	220,  63, 110, 20, NULL);
	eRegisterEdxHex		= DRAW_EDIT		(wRegister, REGISTER_EDIT_EDX_HEX,	 90,  88, 110, 20, NULL);
	eRegisterEdxDec		= DRAW_EDIT		(wRegister, REGISTER_EDIT_EDX_DEC,	220,  88, 110, 20, NULL);
	eRegisterEbxHex		= DRAW_EDIT		(wRegister, REGISTER_EDIT_EBX_HEX,	 90, 113, 110, 20, NULL);
	eRegisterEbxDec		= DRAW_EDIT		(wRegister, REGISTER_EDIT_EBX_DEC,	220, 113, 110, 20, NULL);
	eRegisterEsiHex		= DRAW_EDIT		(wRegister, REGISTER_EDIT_ESI_HEX,	 90, 138, 110, 20, NULL);
	eRegisterEsiDec		= DRAW_EDIT		(wRegister, REGISTER_EDIT_ESI_DEC,	220, 138, 110, 20, NULL);
	eRegisterEdiHex		= DRAW_EDIT		(wRegister, REGISTER_EDIT_EDI_HEX,	 90, 163, 110, 20, NULL);
	eRegisterEdiDec		= DRAW_EDIT		(wRegister, REGISTER_EDIT_EDI_DEC,	220, 163, 110, 20, NULL);
	eRegisterEspHex		= DRAW_EDIT		(wRegister, REGISTER_EDIT_ESP_HEX,	 90, 188, 110, 20, NULL);
	eRegisterEspDec		= DRAW_EDIT		(wRegister, REGISTER_EDIT_ESP_DEC,	220, 188, 110, 20, NULL);
	eRegisterEbpHex		= DRAW_EDIT		(wRegister, REGISTER_EDIT_EBP_HEX,	 90, 213, 110, 20, NULL);
	eRegisterEbpDec		= DRAW_EDIT		(wRegister, REGISTER_EDIT_EBP_DEC,	220, 213, 110, 20, NULL);
	eRegisterEipHex		= DRAW_EDIT		(wRegister, REGISTER_EDIT_EIP_HEX,	 90, 238, 110, 20, NULL);
	eRegisterEipDec		= DRAW_EDIT		(wRegister, REGISTER_EDIT_EIP_DEC,	220, 238, 110, 20, NULL);

	eRegisterCsHex		= DRAW_EDIT		(wRegister, REGISTER_EDIT_CS_HEX,	 90, 278, 110, 20, NULL);
	eRegisterCsDec		= DRAW_EDIT		(wRegister, REGISTER_EDIT_CS_DEC,	220, 278, 110, 20, NULL);
	eRegisterDsHex		= DRAW_EDIT		(wRegister, REGISTER_EDIT_DS_HEX,	 90, 303, 110, 20, NULL);
	eRegisterDsDec		= DRAW_EDIT		(wRegister, REGISTER_EDIT_DS_DEC,	220, 303, 110, 20, NULL);
	eRegisterSsHex		= DRAW_EDIT		(wRegister, REGISTER_EDIT_SS_HEX,	 90, 328, 110, 20, NULL);
	eRegisterSsDec		= DRAW_EDIT		(wRegister, REGISTER_EDIT_SS_DEC,	220, 328, 110, 20, NULL);
	eRegisterEsHex		= DRAW_EDIT		(wRegister, REGISTER_EDIT_ES_HEX,	 90, 353, 110, 20, NULL);
	eRegisterEsDec		= DRAW_EDIT		(wRegister, REGISTER_EDIT_ES_DEC,	220, 353, 110, 20, NULL);
	eRegisterFsHex		= DRAW_EDIT		(wRegister, REGISTER_EDIT_FS_HEX,	 90, 378, 110, 20, NULL);
	eRegisterFsDec		= DRAW_EDIT		(wRegister, REGISTER_EDIT_FS_DEC,	220, 378, 110, 20, NULL);
	eRegisterGsHex		= DRAW_EDIT		(wRegister, REGISTER_EDIT_GS_HEX,	 90, 403, 110, 20, NULL);
	eRegisterGsDec		= DRAW_EDIT		(wRegister, REGISTER_EDIT_GS_DEC,	220, 403, 110, 20, NULL);

	eRegisterEflags		= DRAW_EDIT		(wRegister, REGISTER_EDIT_EFLAGS,	 90, 443, 240, 20, NULL);

	bRegisterOk			= DRAW_BUTTON	(wRegister, REGISTER_BUTTON_OK,		-30, -20,  80, 26, "�� ��", cbRegisterButtonOk);

	erpWindow(&wRegister);
	erpDestory(&wRegister);
}

static void guiRegisterDraw(erp_window_s *win) {
	CONTEXT	*reg = (CONTEXT *) win->param;

	EDIT(eRegisterEaxHex)->setText("%08X", reg->Eax);
	EDIT(eRegisterEaxDec)->setText("%lu", reg->Eax);
	EDIT(eRegisterEcxHex)->setText("%08X", reg->Ecx);
	EDIT(eRegisterEcxDec)->setText("%lu", reg->Ecx);
	EDIT(eRegisterEdxHex)->setText("%08X", reg->Edx);
	EDIT(eRegisterEdxDec)->setText("%lu", reg->Edx);
	EDIT(eRegisterEbxHex)->setText("%08X", reg->Ebx);
	EDIT(eRegisterEbxDec)->setText("%lu", reg->Ebx);
	EDIT(eRegisterEsiHex)->setText("%08X", reg->Esi);
	EDIT(eRegisterEsiDec)->setText("%lu", reg->Esi);
	EDIT(eRegisterEdiHex)->setText("%08X", reg->Edi);
	EDIT(eRegisterEdiDec)->setText("%lu", reg->Edi);
	EDIT(eRegisterEspHex)->setText("%08X", reg->Esp);
	EDIT(eRegisterEspDec)->setText("%lu", reg->Esp);
	EDIT(eRegisterEbpHex)->setText("%08X", reg->Ebp);
	EDIT(eRegisterEbpDec)->setText("%lu", reg->Ebp);
	EDIT(eRegisterEipHex)->setText("%08X", reg->Eip);
	EDIT(eRegisterEipDec)->setText("%lu", reg->Eip);

	EDIT(eRegisterCsHex)->setText("%08X", reg->SegCs);
	EDIT(eRegisterCsDec)->setText("%lu", reg->SegCs);
	EDIT(eRegisterDsHex)->setText("%08X", reg->SegDs);
	EDIT(eRegisterDsDec)->setText("%lu", reg->SegDs);
	EDIT(eRegisterSsHex)->setText("%08X", reg->SegSs);
	EDIT(eRegisterSsDec)->setText("%lu", reg->SegSs);
	EDIT(eRegisterEsHex)->setText("%08X", reg->SegEs);
	EDIT(eRegisterEsDec)->setText("%lu", reg->SegEs);
	EDIT(eRegisterFsHex)->setText("%08X", reg->SegFs);
	EDIT(eRegisterFsDec)->setText("%lu", reg->SegFs);
	EDIT(eRegisterGsHex)->setText("%08X", reg->SegGs);
	EDIT(eRegisterGsDec)->setText("%lu", reg->SegGs);

	EDIT(eRegisterEflags)->setText("0x%08X", reg->EFlags);

	EDIT(eRegisterEaxHex)->readonly(TRUE);
	EDIT(eRegisterEaxDec)->readonly(TRUE);
	EDIT(eRegisterEcxHex)->readonly(TRUE);
	EDIT(eRegisterEcxDec)->readonly(TRUE);
	EDIT(eRegisterEdxHex)->readonly(TRUE);
	EDIT(eRegisterEdxDec)->readonly(TRUE);
	EDIT(eRegisterEbxHex)->readonly(TRUE);
	EDIT(eRegisterEbxDec)->readonly(TRUE);
	EDIT(eRegisterEsiHex)->readonly(TRUE);
	EDIT(eRegisterEsiDec)->readonly(TRUE);
	EDIT(eRegisterEdiHex)->readonly(TRUE);
	EDIT(eRegisterEdiDec)->readonly(TRUE);
	EDIT(eRegisterEspHex)->readonly(TRUE);
	EDIT(eRegisterEspDec)->readonly(TRUE);
	EDIT(eRegisterEbpHex)->readonly(TRUE);
	EDIT(eRegisterEbpDec)->readonly(TRUE);
	EDIT(eRegisterEipHex)->readonly(TRUE);
	EDIT(eRegisterEipDec)->readonly(TRUE);

	EDIT(eRegisterCsHex)->readonly(TRUE);
	EDIT(eRegisterCsDec)->readonly(TRUE);
	EDIT(eRegisterDsHex)->readonly(TRUE);
	EDIT(eRegisterDsDec)->readonly(TRUE);
	EDIT(eRegisterSsHex)->readonly(TRUE);
	EDIT(eRegisterSsDec)->readonly(TRUE);
	EDIT(eRegisterEsHex)->readonly(TRUE);
	EDIT(eRegisterEsDec)->readonly(TRUE);
	EDIT(eRegisterFsHex)->readonly(TRUE);
	EDIT(eRegisterFsDec)->readonly(TRUE);
	EDIT(eRegisterGsHex)->readonly(TRUE);
	EDIT(eRegisterGsDec)->readonly(TRUE);

	EDIT(eRegisterEflags)->readonly(TRUE);
}


void guiConfig() {
	ZeroMemory(&wConfig, sizeof wConfig);

	wConfig.classname	= CONFIG_CLASS;
	wConfig.title		= CONFIG_TITLE;
	wConfig.width		= CONFIG_WIDTH;
	wConfig.height		= CONFIG_HEIGHT;
	wConfig.instance	= wMain.instance;
	wConfig.parent		= wMain.hwnd;
	wConfig.draw		= guiConfigDraw;
	wConfig.destory		= guiConfigDestory;
	wConfig.center		= TRUE;
	wConfig.tool		= TRUE;

	lbConfigSec			= DRAW_LABEL	(wConfig, CONFIG_LABEL_SEC,			  20, 120, 500, 20, "���α�����ʾ�Ĵ�С��");
	lbConfigIat			= DRAW_LABEL	(wConfig, CONFIG_LABEL_IAT,			  20, 175, 500, 20, "��������� IAT ��ʾ����Ϣ��");

	cConfigDisasm		= DRAW_CHECK	(wConfig, CONFIG_CHECK_DISASM,		  20,  20, 500, 20, "����� EIP ��ָ������ݲ���ʾ��ָ�����ġ�����ָ���",	NULL);
	cConfigModBase		= DRAW_CHECK	(wConfig, CONFIG_CHECK_MODBASE,		  20,  40, 500, 20, "��ʾ�� RVA ��ַʹ����ģ����ʼ��ַ��Ϊ����ַ",			NULL);
	cConfigBeep			= DRAW_CHECK	(wConfig, CONFIG_CHECK_BEEP,		  20,  60, 500, 20, "�����ϵ�ʱ������ʾ��",								NULL);
	cConfigDllEip		= DRAW_CHECK	(wConfig, CONFIG_CHECK_DLLEIP,		  20,  80, 500, 20, "ִ�г��������Ĵ���ʱ���� EIP",					NULL);

	rConfigSecImage		= DRAW_RADIO	(wConfig, CONFIG_RADIO_SEC_IMAGE,	  40, 135, 120, 20, "�ڴ��е�ʵ�ʴ�С",	1, NULL);
	rConfigSecRaw		= DRAW_RADIO	(wConfig, CONFIG_RADIO_SEC_RAW,		 180, 135, 120, 20, "�ļ��е����ݴ�С",	1, NULL);

	rConfigIatRva		= DRAW_RADIO	(wConfig, CONFIG_RADIO_IAT_RVA,		  40, 200,  70, 20, "RVA ��ַ",			2, NULL);
	rConfigIatAddr		= DRAW_RADIO	(wConfig, CONFIG_RADIO_IAT_ADDR,	 130, 200, 100, 20, "ʵ���ڴ��ַ",		2, NULL);
	rConfigIatOffset	= DRAW_RADIO	(wConfig, CONFIG_RADIO_IAT_OFFSET,	 240, 200, 120, 20, "PE �ļ���ƫ����",		2, NULL);

	bConfigOk			= DRAW_BUTTON	(wConfig, CONFIG_BUTTON_OK,			-110, -20,  80, 26, "�� ��", cbConfigButtonOk);
	bConfigCancel		= DRAW_BUTTON	(wConfig, CONFIG_BUTTON_CANCEL,		 -20, -20,  80, 26, "ȡ ��", cbConfigButtonCancel);

	erpWindow(&wConfig);
	erpDestory(&wConfig);
}

static void guiConfigDraw(erp_window_s *win) {
	if(CONFIG.disasm) {
		CHECK(cConfigDisasm)->check(TRUE);
	}
	if(CONFIG.modBase) {
		CHECK(cConfigModBase)->check(TRUE);
	}
	if(CONFIG.beep) {
		CHECK(cConfigBeep)->check(TRUE);
	}
	if(CONFIG.dllEip) {
		CHECK(cConfigDllEip)->check(TRUE);
	}

	if(CONFIG.secSize) {
		RADIO(rConfigSecRaw)->select();
	} else {
		RADIO(rConfigSecImage)->select();
	}

	switch(CONFIG.iatType) {
		case 0: RADIO(rConfigIatRva)->select(); break;
		case 1: RADIO(rConfigIatAddr)->select(); break;
		case 2: RADIO(rConfigIatOffset)->select(); break;
	}
}

static void * guiConfigDestory(erp_window_s *win, DWORD arg) {
	if(arg) {
		CONFIG.disasm		= CHECK(cConfigDisasm)->checked;
		CONFIG.modBase		= CHECK(cConfigModBase)->checked;
		CONFIG.beep			= CHECK(cConfigBeep)->checked;
		CONFIG.dllEip		= CHECK(cConfigDllEip)->checked;
		CONFIG.secSize		= RADIO(rConfigSecRaw)->selected;

		if(RADIO(rConfigIatRva)->selected) {
			CONFIG.iatType = 0;
		} else if(RADIO(rConfigIatAddr)->selected) {
			CONFIG.iatType = 1;
		} else if(RADIO(rConfigIatOffset)->selected) {
			CONFIG.iatType = 2;
		}
	}

	return NULL;
}


void guiAbout() {
	ZeroMemory(&wAbout, sizeof wAbout);

	wAbout.classname	= ABOUT_CLASS;
	wAbout.title		= ABOUT_TITLE;
	wAbout.width		= ABOUT_WIDTH;
	wAbout.height		= ABOUT_HEIGHT;
	wAbout.instance		= wMain.instance;
	wAbout.parent		= wMain.hwnd;
	wAbout.center		= TRUE;
	wAbout.tool			= TRUE;

	lbAboutProgram		= DRAW_LABEL	(wAbout, ABOUT_LABEL_PROGRAM,	 20,  20, 500, 20, "��ӭʹ�� Sinope PE Loader��");
	lbAboutComment		= DRAW_LABEL	(wAbout, ABOUT_LABEL_COMMENT,	 20,  50, 600, 40, "Sinope ��һ�����׵� PE �ļ����������ṩ�����ĳ������и��ٹ��ܡ�\r\nͨ��ʹ��������������������˽��������й��̣��Լ�ʹ����������Լӿ��ļ������ѿǡ�");
	lbAboutAuthor		= DRAW_LABEL	(wAbout, ABOUT_LABEL_AUTHOR,	 20,  90, 500, 40, "��Ȩ��(C) 2005-2012 mfboy");
	lbAboutVersion		= DRAW_LABEL	(wAbout, ABOUT_LABEL_VERSION,	 20, 105, 500, 40, "�汾��0.1.1104");
	bAboutOk			= DRAW_BUTTON	(wAbout, ABOUT_BUTTON_OK,		-20, -20,  80, 26, "�� ��", cbAboutButtonOk);

	erpWindow(&wAbout);
	erpDestory(&wAbout);
}


BOOL guiBp(DWORD *from, DWORD *to, BOOL *rva, void *func) {
	packNew(storage, from, to, rva, func);

	ZeroMemory(&wBp, sizeof wBp);

	/* ��� */
	if((*from == 0) && (*to == 0)) {
		wBp.title = BP_TITLE_ADD;
	} else {
		wBp.title = BP_TITLE_EDIT;
	}

	wBp.classname	= BP_CLASS;
	wBp.width		= BP_WIDTH;
	wBp.height		= BP_HEIGHT;
	wBp.instance	= wMain.instance;
	wBp.parent		= wMain.hwnd;
	wBp.draw		= guiBpDraw;
	wBp.destory		= guiBpDestory;
	wBp.center		= TRUE;
	wBp.tool		= TRUE;
	wBp.param		= (DWORD) storage;

	lbBpFrom		= DRAW_LABEL	(wBp, BP_LABEL_FROM,		  20,  20,  80, 20, "��ʼ��ַ��");
	lbBpTo			= DRAW_LABEL	(wBp, BP_LABEL_TO,			  20,  45,  80, 20, "������ַ��");
	lbBpType		= DRAW_LABEL	(wBp, BP_LABEL_TYPE,		  20,  80,  80, 20, "��ַ���ͣ�");
	lbBpComment		= DRAW_LABEL	(wBp, BP_LABEL_COMMENT,		  20, 120, 500, 40, "��������Ҫ�ϵ�ĵ�ַ��Χ��������ʼ��ַ�ͽ�����ַ��\r\n��Ҫ���һ�������ַ���ǵ�ַ��Χ����ֱ�����ս�����ַ��");

	eBpFrom			= DRAW_EDIT		(wBp, BP_EDIT_FROM,			  90,  18, 200, 20, NULL);
	eBpTo			= DRAW_EDIT		(wBp, BP_EDIT_TO,			  90,  43, 200, 20, NULL);

	rBpTypeRva		= DRAW_RADIO	(wBp, BP_RADIO_TYPE_RVA,	  90,  77,  70, 20, "RVA ��ַ",		1, NULL);
	rBpTypeAddr		= DRAW_RADIO	(wBp, BP_RADIO_TYPE_ADDR,	 180,  77,  90, 20, "ʵ���ڴ��ַ",	1, NULL);

	bBpOk			= DRAW_BUTTON	(wBp, BP_BUTTON_OK,			-110,  -20, 80, 26, "ȷ ��",	cbBpButtonOk);
	bBpCancel		= DRAW_BUTTON	(wBp, BP_BUTTON_CANCEL,		 -20,  -20, 80, 26, "ȡ ��",	cbBpButtonCancel);

	erpWindow(&wBp);
	erpDestory(&wBp);

	if(wBp.s.quit == 1) {
		return TRUE;
	} else {
		return FALSE;
	}
}

static void guiBpDraw(erp_window_s *win) {
	DWORD	from	= packVar(win->param, 0, DWORD);
	DWORD	to		= packVar(win->param, 1, DWORD);
	BOOL	rva		= packVar(win->param, 2, BOOL);

	if(from != 0) {
		EDIT(eBpFrom)->setText("%X", from);
	}
	if(to != 0) {
		EDIT(eBpTo)->setText("%X", to);
	}

	if(rva) {
		RADIO(rBpTypeRva)->select();
	} else {
		RADIO(rBpTypeAddr)->select();
	}
}

static void * guiBpDestory(erp_window_s *win, DWORD arg) {
	void	*func;
	char	*error, *fromStr, *toStr;

	int		from = -1, to = -1;
	BOOL	rva = FALSE;

	if(arg) {
		func	= packPtr(win->param, 3, void);

		fromStr	= EDIT(eBpFrom)->getText();
		toStr	= EDIT(eBpTo)->getText();

		sscanf(fromStr, "%x", &from);
		sscanf(toStr, "%x", &to);

		mem_delete(fromStr);
		mem_delete(toStr);

		if((error = ((char * (*)(int *, int *, BOOL *)) func)(&from, &to, &rva)) == NULL) {
			packVar(win->param, 0, DWORD)	= (DWORD) from;
			packVar(win->param, 1, DWORD)	= (DWORD) to;

			packVar(win->param, 2, BOOL)	= RADIO(rBpTypeRva)->selected;
		} else {
			if(error[0]) {
				MessageBox(win->hwnd, error, "", MB_OK | MB_ICONWARNING);
			}

			win->cancel = TRUE;
		}
	}

	return NULL;
}


void guiSection(void *func) {
	ZeroMemory(&wBp, sizeof wBp);

	wSection.classname	= SECTION_CLASS;
	wSection.title		= SECTION_TITLE;
	wSection.width		= SECTION_WIDTH;
	wSection.height		= SECTION_HEIGHT;
	wSection.instance	= wMain.instance;
	wSection.parent		= wMain.hwnd;
	wSection.destory	= guiSectionDestory;
	wSection.center		= TRUE;
	wSection.tool		= TRUE;
	wSection.param		= (DWORD) func;

	lbSectionName		= DRAW_LABEL	(wSection, SECTION_LABEL_NAME,		  20, 20,  50, 20, "���ƣ�");
	lbSectionSize		= DRAW_LABEL	(wSection, SECTION_LABEL_SIZE,		  20, 45,  50, 20, "��С��");
	lbSectionComment	= DRAW_LABEL	(wSection, SECTION_LABEL_COMMENT,	  20, 85, 400, 80, "���ƿ�ѡ�����������������ֲ�ͬ�Ķ�̬�ڴ漰�������Ρ�\r\n����Ĵ�С��λ���ֽڣ����Բ��ý���ҳ���롣\r\n\r\n����Ķ�̬�ڴ���ж�д��ִ��Ȩ�ޡ�");

	eSectionName		= DRAW_EDIT		(wSection, SECTION_EDIT_NAME,		  60, 18, 200, 20, NULL);
	eSectionSize		= DRAW_EDIT		(wSection, SECTION_EDIT_SIZE,		  60, 43, 200, 20, NULL);

	bSectionOk			= DRAW_BUTTON	(wSection, SECTION_BUTTON_OK,		-110,  -20, 80, 26, "ȷ ��",	cbSectionButtonOk);
	bSectionCancel		= DRAW_BUTTON	(wSection, SECTION_BUTTON_CANCEL,	 -20,  -20, 80, 26, "ȡ ��",	cbSectionButtonCancel);

	erpWindow(&wSection);
	erpDestory(&wSection);
}

static void * guiSectionDestory(erp_window_s *win, DWORD arg) {
	int		size = -1;
	char	*name, *buffer, *error;

	if(arg) {
		name	= EDIT(eSectionName)->getText();
		buffer	= EDIT(eSectionSize)->getText();

		/* δ�������� */
		if(name[0] == 0) {
			mem_delete(name);
		}

		sscanf(buffer, "%d", &size);
		mem_delete(buffer);

		if((error = ((char * (*)(char *, int)) win->param)(name, size)) != NULL) {
			if(error[0] != 0) {
				MessageBox(win->hwnd, error, "", MB_OK | MB_ICONWARNING);
			}

			win->cancel = TRUE;

			mem_delete(name);
		}
	}

	return NULL;
}


void guiImport(DWORD addr, void *func) {
	packNew(storage, (void *) addr, func);

	ZeroMemory(&wImport, sizeof wImport);

	wImport.classname	= IMPORT_CLASS;
	wImport.title		= IMPORT_TITLE;
	wImport.width		= IMPORT_WIDTH;
	wImport.height		= IMPORT_HEIGHT;
	wImport.instance	= wMain.instance;
	wImport.parent		= wMain.hwnd;
	wImport.draw		= guiImportDraw;
	wImport.destory		= guiImportDestory;
	wImport.center		= TRUE;
	wImport.tool		= TRUE;
	wImport.param		= (DWORD) storage;

	lbImportTip			= DRAW_LABEL	(wImport, IMPORT_LABEL_TIP,		  20,  20, 200, 20, "��ڵ�ַ��");
	eImportAddr			= DRAW_EDIT		(wImport, IMPORT_EDIT_ADDR,		  20,  38, 340, 20, NULL);
	cImportRva			= DRAW_CHECK	(wImport, IMPORT_CHECK_RVA,		  20,  65, 100, 20, "ʹ�� RVA ��ַ", NULL);

	bImportOk			= DRAW_BUTTON	(wImport, IMPORT_BUTTON_OK,		-110, -20,  80, 26, "ȷ ��",	cbImportButtonOk);
	bImportCancel		= DRAW_BUTTON	(wImport, IMPORT_BUTTON_CANCEL,	 -20, -20,  80, 26, "ȡ ��",	cbImportButtonCancel);

	erpWindow(&wImport);
	erpDestory(&wImport);
}

static void guiImportDraw(erp_window_s *win) {
	EDIT(eImportAddr)->setText("%X", packVar(win->param, 0, DWORD));
}

static void * guiImportDestory(erp_window_s *win, DWORD arg) {
	int		size = -1;

	char	*buffer, *error;
	void	*func;

	if(arg) {
		func	= packPtr(win->param, 1, void);
		buffer	= EDIT(eImportAddr)->getText();

		sscanf(buffer, "%x", &size);
		mem_delete(buffer);

		if((error = ((char * (*)(int)) func)(size)) != NULL) {
			if(error[0] != 0) {
				MessageBox(win->hwnd, error, "", MB_OK | MB_ICONWARNING);
			}

			win->cancel = TRUE;
		}
	}

	return NULL;
}
