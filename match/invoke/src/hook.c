/*
	$ Invoke   (C) 2005-2012 MF
	$ hook.c, v2 @ 0.1.1202
*/

#include "base.h"


void hookSetCode(function_s *func) {
	if(func == NULL) {
		return;
	}

	if(processWrite(func->bp, &func->code, 1) == 0) {
		die("Invoke ���ִ����޷�д�뱻���Խ��̵��ڴ棬Ӧ�ó����ѱ�����ֹ��");
	}
}

void hookSetBp(function_s *func) {
	char bkpoint = 0xCC;

	if(func == NULL) {
		return;
	}

	if(processWrite(func->bp, &bkpoint, sizeof bkpoint) == 0) {
		die("Invoke ���ִ����޷�д�뱻���Խ��̵��ڴ棬Ӧ�ó����ѱ�����ֹ��");
	}
}


int hookInstall(function_s *func) {
	char bkpoint = 0xCC;

	if(func->hook) {
		return 1;
	}

	/* �����ָ�� */
	if(processRead(func->bp, &func->code, 1) == 0) {
		return 0;
	}

	/* д�� INT3 */
	if(processWrite(func->bp, &bkpoint, sizeof bkpoint) == 0) {
		return 0;
	}

	return 1;
}

void hookInstallAll(HWND win) {
	module_s	*module		= MOD->next;
	function_s	*function	= NULL;

	while(module) {
		function = module->function;
		
		while(function) {
			if((function->hijack) && (function->hook == FALSE)) {
				if(hookInstall(function)) {
					function->hook = TRUE;
				} else {
					if(function->name) {
						warningFormat(win, "��ģ�� %s �еĺ��� %s ��װ�ϵ�ʧ�ܡ�", function->module, function->name);
					} else {
						warningFormat(win, "��ģ�� %s �еĵ��뺯�� %ld ��װ�ϵ�ʧ�ܡ�", function->module, function->ordinal);
					}
				}
			}

			function = function->next;
		}

		module = module->next;
	}
}


int hookUninstall(function_s *func) {
	if(func->hook == FALSE) {
		return 1;
	}

	if(processWrite(func->bp, &func->code, 1) == 0) {
		return 0;
	}

	func->hook = FALSE;

	return 1;
}

void hookUninstallAll() {
	int failed = 0;

	module_s	*module = MOD->next;
	function_s	*function;

	while(module) {
		function = module->function;

		while(function) {
			if(function->hook) {
				if(processWrite(function->bp, &function->code, 1) == 0) {
					failed = 1;
				}
			}

			function = function->next;
		}

		module = module->next;
	}

	if(failed) {
		warning(NULL, "���ֶϵ�ж��ʧ�ܣ������ԵĽ��̿��ܹ�����������");
	}
}


function_s * hookFind(DWORD address, module_s **module) {
	module_s	*mod		= MOD->next;
	function_s	*function	= NULL;

	while(mod) {
		if((address >= mod->base) && ((mod->size == 0) || (address < (mod->base + mod->size)))) {
			function = mod->function;

			while(function) {
				if(address == function->bp) {
					*module = mod;

					return function;
				}

				function = function->next;
			}
		}

		mod = mod->next;
	}

	return NULL;
}
