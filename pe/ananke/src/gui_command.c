/*
	$ Ananke PE Loader  (C) 2005-2012 mfboy
	$ gui_command.c, 0.1.1027
*/

#include <Windows.h>
#include <process.h>
#include "base.h"
#include "process.h"


extern HWND winHwnd;

extern HWND buttonExecuteHwnd, buttonDumpHwnd;
extern HWND buttonSuspendHwnd, buttonRunBpHwnd, buttonRunModuleHwnd, buttonRunSingleHwnd;
extern HWND buttonRegisterHwnd, buttonConfigHwnd, buttonAboutHwnd;

extern HWND textRvaHwnd, textEipHwnd, textSectionHwnd, textCodeHwnd;
extern HWND checkDisasmHwnd;

extern HWND textWorkingHwnd;

extern process_h PROCESS;


void cmdEnable(BOOL enable) {
	EnableWindow(buttonDumpHwnd,		enable);
	EnableWindow(buttonSuspendHwnd,		enable);
	EnableWindow(buttonRunBpHwnd,		enable);
	EnableWindow(buttonRunModuleHwnd,	enable);
	EnableWindow(buttonRunSingleHwnd,	enable);
	EnableWindow(buttonRegisterHwnd,	enable);
	EnableWindow(buttonConfigHwnd,		enable);

	EnableWindow(checkDisasmHwnd,		enable);
}

void cmdExecuteEnable(BOOL enable) {
	EnableWindow(buttonExecuteHwnd, enable);
}


void cmdInit() {
	bpInit();
	sectionInit();
	importInit();
	moduleInit();

	cmdEnable(FALSE);
}


void cmdExecute() {
	char *file;

	if(PROCESS.file == NULL) {
		if((file = SelectFileOpen("exe", "��ѡ����Ҫ����� PE �ļ�")) == NULL) {
			return;
		}

		/* �����߳� */
		th_run(Process, file);
	} else {

	}
}

void cmdDump() {

}

void cmdSuspend() {

}


void cmdRunBp() {

}

void cmdRunModule() {

}

void cmdRunSingle() {

}

void cmdRunRegister() {

}


void cmdConfig() {

}

void cmdAbout() {
	MessageBox(winHwnd, "Ananke PE Loader \n\n����һ�����õġ�ͼ�λ��� PE �ļ����ص��Թ��ߣ�ӵ������ʽ�Ĵ���\n�������棬�����ڶ� EXE �����ļ����е��Ժ��ѿǵȡ�\n\n���������Ϊ mfboy����ǰ����ʹ�õİ汾�� v0.1.1027��", "����", MB_OK | MB_ICONINFORMATION);
}


void cmdAddress(DWORD rva, DWORD eip, char *section, char *code) {
	char buffer[10];

	sprintf(buffer, "%08X", eip);
	SetWindowText(textEipHwnd, buffer);

	if(rva == 0) {
		SetWindowText(textRvaHwnd, "-");
	} else {
		sprintf(buffer, "%08X", rva);
		SetWindowText(textRvaHwnd, buffer);
	}

	if(section == NULL) {
		SetWindowText(textSectionHwnd, "-");
	} else {
		SetWindowText(textSectionHwnd, section);
	}

	if(code == NULL) {
		SetWindowText(textCodeHwnd, "");
	} else {
		SetWindowText(textCodeHwnd, section);
	}
}


void cmdWorking(int append, char *format, ...) {
	int i, length;

	char *text;
	char buffer[1024];

	va_list arg;
	va_start(arg, format) ;

	wvsprintf(buffer, format, arg);

	if(append > 0) {
		if(length = GetWindowTextLength(textWorkingHwnd)) {
			mem_alloc(text, char, length + strlen(buffer) + append * 2 + 1);

			GetWindowText(textWorkingHwnd, text, length);

			for(i = 0; i < append; i++) {
				strcat(text, "\r\n");
			}

			strcat(text, buffer);

			SetWindowText(textWorkingHwnd, text);
			SendMessage(textWorkingHwnd, EM_SCROLLCARET, 0, 0);

			mem_delete(text);

			return;
		}
	}

	SetWindowText(textWorkingHwnd, buffer);
}
