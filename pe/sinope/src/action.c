/*
	$ Sinope PE Loader  (C) 2005-2012 mfboy
	$ action.c, 0.1.1104
*/

#include <Windows.h>
#include "base.h"
#include "image.h"
#include "gui.h"
#include "action.h"


void threadFlush() {
	int		index		= 1;
	DWORD	va			= 0;
	char	buffer[128]	= {0};

	thread_s *thread = THREAD->next;

	while(thread) {
		if(thread->index == 0) {
			va = addr2va(thread->start);

			if(index == 1) {
				wsprintf(buffer, "���߳� (�ڴ��ַ=%08X, VA=%08X)", thread->start, va);
			} else {
				if(va == 0) {
					wsprintf(buffer, "�߳� #%ld (�ڴ��ַ=%08X)", index, thread->start);
				} else {
					wsprintf(buffer, "�߳� #%ld (�ڴ��ַ=%08X, VA=%08X)", index, thread->start, va);
				}
			}

			/* �״γ�ʼ�� */
			if(index == 1) {
				COMBO(cmbThread)->clear();
				COMBO(cmbThread)->append("�����߳�");
			}

			thread->index = COMBO(cmbThread)->append(buffer);
		} else if(thread->index != index) {
			thread->index = index;
		}

		index++;
		thread = thread->next;
	}
}


void bpDraw(bp_s *bp) {
	char buffer[2][32]	= {0};
	char *data[5]		= {NULL, buffer[0], buffer[1], NULL, NULL};

	section_s	*section;
	import_s	*import;
	module_s	*module;

	/* �������� */
	if(section = getSectionByAddr(bp->addrFrom)) {
		data[3] = section->name;
	} else {
		data[3] = "";
	}

	/* ��ע��Ϣ */
	if(import = getImportByAddr(bp->addrFrom)) {
		if(import->function) {
			data[4] = import->function;
		} else {
			data[4] = import->dll;
		}
	} else if(module = getModuleByAddr(bp->addrFrom)) {
		if(RUNTIME.proc.base == module->address) {
			data[4] = "";
		} else {
			data[4] = module->path;
		}
	} else {
		data[4] = "";
	}

	/* �ж� */
	if(RUNTIME.proc.tBreak && (RUNTIME.proc.tBreak->run.eip >= bp->addrFrom) && (RUNTIME.proc.tBreak->run.eip <= bp->addrTo)) {
		data[0] = "=>";
	} else {
		data[0] = "";
	}

	/* �ڴ��ַ */
	if(bp->addrFrom == bp->addrTo) {
		sprintf(buffer[0], "%08X", bp->addrFrom);
	} else {
		sprintf(buffer[0], "%08X - %08X", bp->addrFrom, bp->addrTo);
	}

	/* RVA */
	if(bp->vaFrom) {
		if(bp->vaFrom == bp->vaTo) {
			sprintf(buffer[1], "%08X", va2rva(bp->vaFrom));
		} else {
			sprintf(buffer[1], "%08X - %08X", va2rva(bp->vaFrom), va2rva(bp->vaTo));
		}
	}

	if(bp->index < 0) {
		bp->index = LIST(lsBp)->append(data);
	} else {
		LIST(lsBp)->modify(bp->index, data);
	}
}

void bpFlush() {
	bp_s *bp = BP->next;

	LIST(lsBp)->clear();

	while(bp) {
		bp->index = -1;
		bpDraw(bp);

		bp = bp->next;
	}
}

void bpActive(int index) {
	bp_s	*bp			= BP->next;
	char	*data[5]	= {NULL};

	while(bp) {
		if(bp->index == index) {
			data[0] = "=>";
		} else {
			data[0] = "";
		}

		LIST(lsBp)->modify(bp->index, data);

		bp = bp->next;
	}
}

void bpAdd(DWORD from, DWORD to, BOOL rva) {
	bp_s *bp = BP;

	/* ����β */
	while(bp->next) {
		bp = bp->next;
	}

	mem_alloc(bp->next, bp_s, 1);

	bp->next->index = -1;

	bpEdit(bp->next, from, to, rva);
}

void bpEdit(bp_s *bp, DWORD from, DWORD to, BOOL rva) {
	if(rva) {
		bp->addrFrom	= rva2addr(from);
		bp->addrTo		= rva2addr(to);
		bp->vaFrom		= rva2va(from);
		bp->vaTo		= rva2va(to);
	} else {
		bp->addrFrom	= from;
		bp->addrTo		= to;
		bp->vaFrom		= addr2va(from);
		bp->vaTo		= addr2va(to);
	}

	bp->rva = rva;

	bpDraw(bp);
}

char * bpCallback(int *from, int *to, BOOL *rva) {
	int		temp;
	DWORD	addrFrom, addrTo;

	if(*to < 0) {
		if(*from < 0) {
			return "������һ����ȷ�ĵ�ַ��";
		} else if(*from == 0) {
			return "�������жϵ�ַ 0��";
		}

		*to = *from;
	} else {
		if(*from < 0) {
			return "��������ȷ����ʼ��ַ��";
		} else if(*from == 0) {
			return "��ʼ��ַ������Ϊ 0��";
		} else if(*to == 0) {
			return "������ַ������Ϊ 0��";
		}

		if(*to < *from) {
			temp	= *to;

			*to		= *from;
			*from	= temp;
		}
	}

	if(*rva) {
		addrFrom	= rva2addr((DWORD) *from);
		addrTo		= rva2addr((DWORD) *to);
	} else {
		addrFrom	= (DWORD) *from;
		addrTo		= (DWORD) *to;
	}

	/* ���Ƿ��������� */
	if(getSectionByAddr(addrFrom) || getSectionByAddr(addrTo)) {
		return NULL;
	}

	/* ���Ƿ���ģ���� */
	if(getModuleByAddr(addrFrom) || getModuleByAddr(addrTo)) {
		return NULL;
	}

	if(confirm("����ĵ�ַ�������κ���֪�����κ�ģ���ڣ��������Զ���ᱻ�������Ƿ������")) {
		return NULL;
	} else {
		return "";
	}
}


void sectionDraw(section_s *section) {
	DWORD	rva;

	char	buffer[4][16]	= {0};
	char	*data[6]		= {NULL, buffer[0], buffer[1], buffer[2], buffer[3], NULL};

	if(section->name) {
		data[0] = section->name;
	} else {
		data[0] = "-";
	}

	if(rva = va2rva(section->va)) {
		sprintf(buffer[0], "%08X", rva);
	} else {
		buffer[0][0] = '-';
	}

	sprintf(buffer[1], "%08X", section->addr);

	if(section->dynamic) {
		strcpy(buffer[2], "��̬");
	} else {
		sprintf(buffer[2], "%08X", section->offset);
	}

	if((section->dynamic == TRUE) || (CONFIG.secSize == FALSE)) {
		sprintf(buffer[3], "%lu", section->sizeImage);
	} else {
		sprintf(buffer[3], "%lu", section->sizeRaw);
	}

	if(section->file == TRUE) {
		if(section->dynamic) {
			data[5] = "PE ��̬";
		} else {
			data[5] = "PE";
		}
	} else {
		if(section->dynamic) {
			data[5] = "�û�";
		} else {
			data[5] = "������";
		}
	}

	if(section->index < 0) {
		section->index = LIST(lsSection)->append(data);
	} else {
		LIST(lsSection)->modify(section->index, data);
	}
}

void sectionFlush() {
	section_s *section = SECTION->next;

	LIST(lsSection)->clear();

	while(section) {
		section->index = -1;
		sectionDraw(section);

		section = section->next;
	}
}

void sectionAdd(char *name, DWORD address, DWORD size) {
	section_s *next = SECTION;

	while(next->next) {
		next = next->next;
	}

	mem_alloc(next->next, section_s, 1);

	next->next->name		= name;
	next->next->addr		= address;
	next->next->sizeImage	= size;
	next->next->dynamic		= TRUE;
	next->next->index		= -1;

	sectionDraw(next->next);
}

char * sectionCallback(char *name, int size) {
	void *p;

	if(size <= 0) {
		return "��������Ҫ������ڴ�ռ��С��";
	}
	if(name == NULL) {
		if(confirm("�Ƿ�ȷ�����������֣�") == FALSE) {
			return "";
		}
	}

	if((p = VirtualAllocEx(RUNTIME.proc.handle, NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE)) == NULL) {
		return "�ڴ�����ʧ�ܣ����Ժ����ԡ�";
	}

	return NULL;
}


void importDraw(import_s *import) {
	char	buffer[4][16]	= {0};
	char	*data[5]		= {NULL, NULL, buffer[1], buffer[2], buffer[3]};

	data[0] = import->dll;

	if(import->function) {
		data[1] = import->function;
	} else {
		data[1] = buffer[0];
		sprintf(buffer[0], "#%lu", import->ordinal);
	}

	sprintf(buffer[1], "%08X", import->point);
	sprintf(buffer[2], "%08X", import->entry);

	switch(CONFIG.iatType) {
		case 0: sprintf(buffer[3], "%08X", va2rva(import->iatVa)); break;
		case 1: sprintf(buffer[3], "%08X", va2addr(import->iatVa)); break;
		case 2: sprintf(buffer[3], "%08X", import->iatOffset); break;
	}

	if(import->index < 0) {
		import->index = LIST(lsImport)->append(data);
	} else {
		LIST(lsImport)->modify(import->index, data);
	}
}

void importFlush() {
	import_s *import = IMPORT->next;

	LIST(lsImport)->clear();

	while(import) {
		import->index = -1;
		importDraw(import);

		import = import->next;
	}
}

char * importCallback(int size) {
	return "ʧ��";
}


void moduleDraw(module_s *module) {
	char	buffer[2][16]	= {0};
	char	*data[]			= {NULL, buffer[0], buffer[1]};

	data[0] = module->path;

	sprintf(buffer[0], "%08X", module->address);
	sprintf(buffer[1], "%lu", module->size);

	if(module->index < 0) {
		module->index = LIST(lsModule)->append(data);
	} else {
		LIST(lsModule)->modify(module->index, data);
	}
}

void moduleFlush() {
	module_s *module = MODULE->next;

	LIST(lsModule)->clear();

	while(module) {
		module->index = -1;
		moduleDraw(module);

		module = module->next;
	}
}
