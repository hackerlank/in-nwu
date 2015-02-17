/*
	$ Sinope PE Loader  (C) 2005-2012 mfboy
	$ pe.c, 0.1.1104
*/

#define _SNP_PE_

#include <Windows.h>
#include "base.h"
#include "image.h"
#include "gui.h"


/* �ļ���� */
static FILE *fp;


char * peHeader(BOOL refresh) {
	IMAGE_DOS_HEADER dos;
	IMAGE_NT_HEADERS pe;

	ZeroMemory(&dos, sizeof dos);
	ZeroMemory(&pe, sizeof pe);

	pfSeek(0, SEEK_SET);

	/* �� DOS ͷ */
	if((pfRead(&dos, sizeof dos) == FALSE) || (dos.e_magic != IMAGE_DOS_SIGNATURE)) {
		return "����ѡ���ļ���δ�ҵ� DOS ͷ�������ܲ���һ����ȷ�� PE �ļ���";
	}

	/* �ƶ��� PE ͷ */
	pfSeek((long) dos.e_lfanew, SEEK_SET);

	/* �� PE ͷ */
	if((pfRead(&pe, sizeof pe) == FALSE) || (pe.Signature != IMAGE_NT_SIGNATURE)) {
		return "����ѡ���ļ���δ�ҵ� PE ͷ�������ܲ���һ����ȷ�� PE �ļ���";
	}

	/* �ж��ǲ��� PE+ */
	switch(pe.OptionalHeader.Magic) {
		case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
			break;

		case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
			return "����֧�ּ��� 64 λ����� PE �ļ���";

		default:
			return "�޷�ʶ���ļ������͡�";
	}

	/* ���������� */
	switch(pe.FileHeader.Machine) {
		case IMAGE_FILE_MACHINE_I386:
			break;

		default:
			return "�޷�ʶ�����д� PE �ļ�����Ҫ�Ĵ��������͡�";
	}

	/* �ж��ǲ��� DLL */
	if((pe.FileHeader.Characteristics & IMAGE_FILE_DLL) == IMAGE_FILE_DLL) {
		return "�������� DLL �ļ���";
	} else if((pe.FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) != IMAGE_FILE_EXECUTABLE_IMAGE) {
		return "�޷�ʶ���ļ��ı�ǡ�";
	}

	/* ��ϵͳ */
	switch(pe.OptionalHeader.Subsystem) {
		case IMAGE_SUBSYSTEM_WINDOWS_GUI: RUNTIME.pe.console = 0; break;
		case IMAGE_SUBSYSTEM_WINDOWS_CUI: RUNTIME.pe.console = 1; break;

		default:
			return "�޷�ʶ�����д� PE �ļ�����Ҫ����ϵͳ��";
	}

	RUNTIME.pe.entry	= pe.OptionalHeader.AddressOfEntryPoint;
	RUNTIME.pe.import	= pe.OptionalHeader.DataDirectory[1].VirtualAddress;
	RUNTIME.pe.section	= pfTell();

	if(refresh == FALSE) {
		RUNTIME.pe.base = pe.OptionalHeader.ImageBase;
	}

	return NULL;
}

char * peSection() {
	section_s				*section, *next = SECTION;
	IMAGE_SECTION_HEADER	header;

	pfSeek(RUNTIME.pe.section, SEEK_SET);

	while(next->next != NULL) {
		next = next->next;
	}

	while(!pfEnd()) {
		if(pfRead(&header, sizeof header) == 0) {
			break;
		}
		if(header.VirtualAddress == 0) {
			break;
		}

		mem_alloc(section, section_s, 1);

		section->name		= readFromBuffer((char *) header.Name, sizeof header.Name);
		section->addr		= 0;
		section->va			= header.VirtualAddress;
		section->offset		= header.PointerToRawData;
		section->sizeImage	= header.Misc.VirtualSize;
		section->sizeRaw	= header.SizeOfRawData;
		section->dynamic	= FALSE;
		section->file		= TRUE;
		section->copy		= NULL;
		section->index		= -1;

		next->next = section;
		next = section;
	}

	if(SECTION->next == NULL) {
		return "û���� PE �ļ����ҵ��κ����Ρ�";
	} else {
		return NULL;
	}
}

static void peImportFunction(IMAGE_IMPORT_DESCRIPTOR *info) {
	int		i;
	long	offset;
	char	*dll, *function;

	import_s			*import, *next = IMPORT;
	IMAGE_THUNK_DATA	thunk;

	/* ת�����͵����� */
	if(info->ForwarderChain != 0) {
		return;
	}
	
	/* �� DLL ���� */
	pfSeek(pfOffset(info->Name), SEEK_SET);

	/* ���������� */
	if((dll = readFromFile()) == NULL) {
		return;
	} else {
		/* �� dll ת��ΪСд */
		strtolower(dll);
	}

	/* ����β */
	while(next->next != NULL) {
		next = next->next;
	}

	/* �ƶ��� INT ��� */
	if(info->OriginalFirstThunk) {
		pfSeek(pfOffset(info->OriginalFirstThunk), SEEK_SET);
	} else if(info->FirstThunk) {
		pfSeek(pfOffset(info->FirstThunk), SEEK_SET);
	} else {
		return;
	}

	/* ����������, ���ڶ�λ INT */
	i = 0;

	while(!pfEnd()) {
		if(pfRead(&thunk, sizeof thunk) == FALSE) {
			break;
		}
		if(isBlank(&thunk, sizeof thunk)) {
			break;
		}

		mem_alloc(import, import_s, 1);

		/* ������ǰ INT λ�� */
		offset = pfTell();

		/* ������ŵ��� */
		if(thunk.u1.Function & 0x80000000) {
			import->ordinal = thunk.u1.Ordinal & 0x7FFFFFFF;
		} else {
			/* ������λ�� */
			pfSeek(pfOffset(thunk.u1.AddressOfData) + sizeof(WORD), SEEK_SET);

			if((function = readFromFile()) == NULL) {
				import->function = "?";
			} else {
				import->function = function;
			}

			pfSeek(offset, SEEK_SET);
		}

		import->point		= 0;
		import->entry		= 0;

		import->dll			= dll;
		import->iatVa		= info->FirstThunk + i * sizeof thunk;
		import->iatOffset	= offset - sizeof thunk;
		import->iatData		= thunk.u1.AddressOfData;
		import->index		= -1;

		next->next = import;
		next = import;

		i++;
	}
}

void peImport() {
	long offset;
	IMAGE_IMPORT_DESCRIPTOR	import;

	if(RUNTIME.pe.import) {
		offset = (long) pfOffset(RUNTIME.pe.import);
		pfSeek(offset, SEEK_SET);
	} else {
		return;
	}

	while(!pfEnd()) {
		if(pfRead(&import, sizeof import) == FALSE) {
			break;
		}
		if(isBlank(&import, sizeof import)) {
			break;
		}

		/* ���浱ǰλ�� */
		offset = pfTell();

		peImportFunction(&import);

		/* ���� */
		pfSeek(offset, SEEK_SET);
	}
}


BOOL dataReadFile(void *buffer, size_t size) {
	if(fread(buffer, size, 1, fp) == 1) {
		return TRUE;
	} else {
		return FALSE;
	}
}

void dataSeekFile(long offset, int mode) {
	fseek(fp, offset, mode);
}

long dataTellFile() {
	return ftell(fp);
}

DWORD dataOffsetFile(DWORD va) {
	return va2offset(va);
}

BOOL dataEndFile() {
	if(feof(fp)) {
		return TRUE;
	} else {
		return FALSE;
	}
}


BOOL pe() {
	char *error;

	/* ����ָ�� */
	pfRead		= dataReadFile;
	pfSeek		= dataSeekFile;
	pfTell		= dataTellFile;
	pfOffset	= dataOffsetFile;
	pfEnd		= dataEndFile;

	WORKING_SET("���ڴ��ļ���%s�� ...\r\n\r\n", RUNTIME.file);

	if((fp = fopen(RUNTIME.file, "rb")) == NULL) {
		WORKING_APPEND_ALERT("�޷����ļ���%s���������ļ��Ƿ���ڻ�ɶ���", RUNTIME.file);
		return FALSE;
	}

	/* ���� PE ͷ */
	if((error = peHeader(FALSE)) != NULL) {
		WORKING_APPEND_ALERT(error);
		return FALSE;
	}

	/* �������� */
	if((error = peSection()) != NULL) {
		WORKING_APPEND_ALERT(error);
		return FALSE;
	}

	/* ����� */
	peImport();

	/* ���� filename */
	if(RUNTIME.filename = strrchr(RUNTIME.file, '\\')) {
		RUNTIME.filename++;
	} else {
		RUNTIME.filename = RUNTIME.file;
	}

	return TRUE;
}


void dump() {

}


DWORD va2rva(DWORD va) {
	if(CONFIG.modBase) {
		return RUNTIME.proc.base + va;
	} else {
		return RUNTIME.pe.base + va;
	}
}

DWORD va2addr(DWORD va) {
	section_s *section = getSectionByVa(va);

	if(section == NULL) {
		return va;
	}
	if(va >= (section->va + section->sizeImage)) {
		return 0;
	}

	return (va - section->va) + section->addr;
}

DWORD va2offset(DWORD va) {
	section_s *section = getSectionByVa(va);

	if(section == NULL) {
		return va;
	}
	if(va >= (section->va + section->sizeRaw)) {
		return 0;
	}

	return (va - section->va) + section->offset;
}

DWORD rva2va(DWORD rva) {
	DWORD base;

	if(CONFIG.modBase) {
		base = RUNTIME.proc.base;
	} else {
		base = RUNTIME.pe.base;
	}

	if(base > rva) {
		return 0;
	} else {
		return rva - base;
	}
}

DWORD rva2addr(DWORD rva) {
	DWORD va = rva2va(rva);

	if(va == 0) {
		return 0;
	} else {
		return va2addr(va);
	}
}

DWORD addr2va(DWORD addr) {
	section_s *section = getSectionByAddr(addr);

	if(section == NULL) {
		return 0;
	}

	return (addr - section->addr) + section->va;
}

DWORD addr2rva(DWORD addr) {
	DWORD va = addr2va(addr);

	if(va == 0) {
		return 0;
	} else {
		return va2rva(va);
	}
}


section_s * getSectionByVa(DWORD va) {
	section_s *curr = NULL;
	section_s *next = SECTION->next;

	while(next) {
		if(va >= next->va) {
			curr = next;
			next = next->next;
		} else {
			break;
		}
	}

	if(curr == NULL) {
		return NULL;
	}
	if(va >= (curr->va + curr->sizeImage)) {
		return NULL;
	}

	return curr;
}

section_s * getSectionByAddr(DWORD addr) {
	section_s *curr = NULL;
	section_s *next = SECTION->next;

	while(next) {
		if(addr >= next->addr) {
			curr = next;
			next = next->next;
		} else {
			break;
		}
	}

	if(curr == NULL) {
		return NULL;
	}
	if(addr >= (curr->addr + curr->sizeImage)) {
		return NULL;
	}

	return curr;
}

import_s * getImportByAddr(DWORD addr) {
	import_s *import = IMPORT->next;

	while(import) {
		if((import->entry == addr) || (import->point == addr) || (import->hook)) {
			return import;
		}

		import = import->next;
	}

	return NULL;
}

module_s * getModuleByAddr(DWORD addr) {
	module_s *module = MODULE->next;

	while(module) {
		if((addr >= module->address) && (addr < (module->address + module->size))) {
			return module;
		}

		module = module->next;
	}

	return NULL;
}
