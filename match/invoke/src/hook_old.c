/*
	$ Invoke   (C) 2005-2012 MF
	$ hook_old.c, v1 @ 0.1.1125

	!!! !!! DO NOT LINK IN PROJECT
*/

#include "base.h"


/* ָ��ת�� */
#define CODE(code, count, ...) \
	if(outOffset = hookJumpCreate(&ncodeSize, ncode, output, code, count, __VA_ARGS__)) { \
		break; \
	}


/* ת���� */
static DWORD routeAddr = 0;
static DWORD routeSize = 0;


/* ��ת���봴�� */
static int hookJumpCreate(int *ncodeSize, char *ncode, char *output, char *code, int count, ...) {
	va_list arg;

	int i;
	int address;
	size_t length = strlen(code);

	/* ǰ׺����ȷ */
	if(memcmp(output, code, length)) {
		return 0;
	}
	/* ָ���Ϊ \t */
	if(output[length] != '\t') {
		return 0;
	}
	/* ��ȡָ����ַ */
	if(sscanf(output + length + 1, "%x", &address) == 0) {
		return 0;
	}

	/* �ɱ�� */
	va_start(arg, count);

	/* Ϊ buffer д�� */
	for(i = 0; i < count; i++) {
		*((unsigned char *) (ncode + i)) = va_arg(arg, unsigned char);
	}

	va_end(arg);

	*ncodeSize = count;

	return length + 1;
}


/* ��ת��ַ���� */
static DWORD hookAddress(function_s *func, int codeSize, char *buffer, char *output) {
	/* ָ���е�ַƫ�� */
	int		outOffset	= 0;
	/* ת��ָ���С */
	int		ncodeSize	= 0;
	/* ת��ָ������ */
	char	ncode[16];

	/* ��תת�Ʒ�Χ */
	int		jump = 0;
	/* ת������С */
	DWORD	size = 5;
	/* ��ַ */
	DWORD	address;

	/* �ж��Ƿ�����ת���� */
	do {
		CODE("call",	1,	0xE8);
		CODE("jmp",		1,	0xE9);

		CODE("jcxz",	6,	0x66, 0x83, 0xF9, 0x00,			/* CMP CX, 0 */
							0x0F, 0x84);					/* JE ... */
		CODE("jecxz",	5,	0x83, 0xF9, 0x00,				/* CMP ECX, 0 */
							0x0F, 0x84);					/* JE ... */

		CODE("je",		2,	0x0F, 0x84);
		CODE("jne",		2,	0x0F, 0x85);
		CODE("jz",		2,	0x0F, 0x84);
		CODE("jnz",		2,	0x0F, 0x85);

		CODE("js",		2,	0x0F, 0x88);
		CODE("jns",		2,	0x0F, 0x89);

		CODE("jc",		2,	0x0F, 0x82);
		CODE("jnc",		2,	0x0F, 0x83);
		CODE("jb",		2,	0x0F, 0x82);
		CODE("jnb",		2,	0x0F, 0x83);

		CODE("jc",		2,	0x0F, 0x82);
		CODE("jnc",		2,	0x0F, 0x83);

		CODE("jo",		2,	0x0F, 0x80);
		CODE("jno",		2,	0x0F, 0x81);

		CODE("ja",		2,	0x0F, 0x87);
		CODE("jna",		2,	0x0F, 0x86);
		CODE("jae",		2,	0x0F, 0x83);
		CODE("jnae",	2,	0x0F, 0x82);

		CODE("jg",		2,	0x0F, 0x8F);
		CODE("jng",		2,	0x0F, 0x8E);
		CODE("jge",		2,	0x0F, 0x8D);
		CODE("jnge",	2,	0x0F, 0x8C);

		CODE("jb",		2,	0x0F, 0x82);
		CODE("jnb",		2,	0x0F, 0x83);
		CODE("jbe",		2,	0x0F, 0x86);
		CODE("jnbe",	2,	0x0F, 0x87);

		CODE("jl",		2,	0x0F, 0x8C);
		CODE("jnl",		2,	0x0F, 0x8D);
		CODE("jle",		2,	0x0F, 0x8E);
		CODE("jnle",	2,	0x0F, 0x8F);

		CODE("jp",		2,	0x0F, 0x8A);
		CODE("jnp",		2,	0x0F, 0x8B);
		CODE("jpe",		2,	0x0F, 0x8A);
		CODE("jpo",		2,	0x0F, 0x8B);
	} while(0);

	/* ����ת��������ռ��С */
	if(ncodeSize) {
		size += ncodeSize + 4;
	} else {
		size += codeSize;
	}
	/* ���� */
	if(size % 16) {
		size = ((size / 16) + 1) * 16;
	}

	/* ����ռ� */
	if(routeSize < size) {
		if((routeAddr = (DWORD) VirtualAllocEx(RT.handle, NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE)) == 0) {
			die("����Խ��̵Ŀռ������ڴ�ʧ�ܡ�");
		}

		routeSize = 4096;
	}

	/* ����ԭʼָ�� */
	func->codeSize = codeSize;
	memcpy(func->codeData, buffer, codeSize);

	/* д��ת�����ָ����ƶ�ԭʼָ�� */
	if(ncodeSize) {
		/* �����ַ */
		if(sscanf(output + outOffset, "%x", &address) == 0) {
			die("�����ϵ�������޷���ȡ���ص�ַ��");
		}

		/* ����ת�Ʒ�Χ */
		jump = (int) address - ((int) routeAddr + ncodeSize + 4);

		/* д��ת�����ָ�� */
		processWrite(routeAddr, ncode, ncodeSize);
		processWrite(routeAddr + ncodeSize, &jump, 4);

		/* ������ָ���С */
		ncodeSize += 4;
	} else {
		processWrite(routeAddr, buffer, codeSize);

		ncodeSize = codeSize;
	}

	/* ���㷵�ص�ַ */
	address	= routeAddr;
	jump	= ((int) func->bp + codeSize) - ((int) routeAddr + ncodeSize + 5);

	/* ���� JMP */
	ncode[0] = 0xE9;

	/* д�� JMP */
	processWrite(routeAddr + ncodeSize, ncode, 1);
	processWrite(routeAddr + ncodeSize + 1, &jump, 4);

	/* �ռ���� */
	routeSize -= size;
	routeAddr += size;

	return address;
}

/* ��װ�ϵ� */
static int hookFunction(function_s *func, char *breakpoint) {
	int		size;
	char	buffer[64];
	char	output[128];

	if(processRead(func->bp, buffer, 64) == 0) {
		return 0;
	}

	/* ԭ������ INT3 */
	if((unsigned char) *((unsigned char *) buffer) == 0xCC) {
		func->code = func->bp + 1;
		
		return 1;
	}
	
	/* ���Ƚ��з���� */
	if((size = disasm(func->bp, buffer, 64, output, 128)) == 0) {
		return 0;
	}
	/* ����ת������д��� */
	if((func->code = hookAddress(func, size, buffer, output)) == 0) {
		return 0;
	}

	/* д�� INT 3 */
	if(processWrite(func->bp, breakpoint, (unsigned int) size) == 0) {
		return 0;
	}

	return 1;
}


/* hooking */
int hookInstall(erp_window_s *win) {
	int failed = 0;

	unsigned int i;
	char breakpoint[32];

	module_s	*module		= NULL;
	function_s	*function	= NULL;

	DWORD		count		= packVar(win->param, 1, DWORD);
	module_s	**modules	= packPtr(win->param, 0, module_s *);

	/* INT 3 */
	memset(breakpoint, 0xCC, sizeof breakpoint);
	
	for(i = 0; i < count; i++) {
		module = modules[i];

		function = module->function;
		
		while(function) {
			if(function->hook == FALSE) {
				if(hookFunction(function, breakpoint)) {
					function->hook = TRUE;
				} else {
					failed = 1;
				}
			}

			function = function->next;
		}
	}

	return failed;
}

void hookUninstall() {
	int failed = 0;

	module_s	*module = MOD->next;
	function_s	*function;

	while(module) {
		function = module->function;

		while(function) {
			if(function->hook && function->codeSize) {
				if(processWrite(function->bp, function->codeData, function->codeSize) == 0) {
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
		if((address >= mod->base) && (address < (mod->base + mod->size))) {
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
