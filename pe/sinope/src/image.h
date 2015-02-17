/*
	$ Sinope PE Loader  (C) 2005-2012 mfboy
	$ image.h, 0.1.1104
*/

#ifndef _SNP_IMAGE_H_
#define _SNP_IMAGE_H_

#if defined(__cplusplus)
	extern "C" {
#endif


/* �ṹ��Ϣ */
struct section_s;
struct import_s;
struct module_s;


/* �߳� */
typedef struct thread_s {
	DWORD	id;
	HANDLE	handle;

	/* ��ʼ��ַ */
	DWORD	start;

	/* ������Ϣ */
	struct {
		DWORD	eip;
		BOOL	main;
		BOOL	dynamic;

		struct section_s	*section;
		struct module_s		*module;
	} run;

	int index;
	struct thread_s *next;
} thread_s;


/* ���� */
typedef struct runtime_s {
	/* �ļ� */
	char	*file;
	char	*filename;

	/* ���̴�� */
	BOOL	alive;
	/* ������ */
	BOOL	running;
	/* ����ִ�� */
	BOOL	singleStep;

	/* ���� */
	struct {
		DWORD	pid;
		HANDLE	handle;

		/* ��ģ���С */
		DWORD	size;

		/* �ض�λ��Ļ���ַ */
		DWORD	base;
		/* ʵ����� */
		DWORD	entry;

		/* �����߳� */
		thread_s	*tDebug;
		thread_s	*tBreak;

		/* ��ģ�� */
		struct module_s *module;
	} proc;

	/* PE */
	struct {
		/* CUI */
		BOOL	console;

		/* Ĭ�ϻ���ַ */
		DWORD	base;

		/* ��ڵ�ַ */
		DWORD	entry;

		/* ���ε�ַ */
		long	section;
		/* ������ַ */
		DWORD	import;
	} pe;
} runtime_s;


/* ���� */
typedef struct config_s {
	/* ���������̨ */
	BOOL	console;

	/* ����� */
	BOOL	disasm;
	/* ����ģ����ʼ��ַ��Ϊ����ַ */
	BOOL	modBase;
	/* �ϵ� beep */
	BOOL	beep;
	/* EIP */
	BOOL	dllEip;

	/* ���α���ʾ�Ĵ�СΪ PE �д�С */
	BOOL	secSize;

	/* IAT  0=RVA, 1=addr, 2=offset */
	int		iatType;

	/* ���� */
	struct {
		/* ʹ���ڴ��С */
		BOOL	realSize;
		/* ���Զ��� */
		BOOL	sectionAlign;
		/* ֱ�Ӹ��� */
		BOOL	copy;

		/* ��� IAT */
		BOOL	fillIat;
	} dump;

	/* �ϵ� */
	struct {
		/* ��̬���� */
		BOOL	dynamicCode;
		int		dynamicCodeMode;

		/* ��̬�ڴ� */
		BOOL	dynamicMemory;

		/* �����ת */
		BOOL	stride;
	} bp;
} config_s;


/* �ϵ� */
typedef struct bp_s {
	/* λ�� */
	DWORD	addrFrom, addrTo;
	DWORD	vaFrom, vaTo;

	/* ʹ�� RVA */
	BOOL	rva;

	int index;
	struct bp_s *next;
} bp_s;

/* ���α� */
typedef struct section_s {
	char	*name;

	/* λ�� */
	DWORD	addr, va, offset;
	/* ��С */
	DWORD	sizeImage, sizeRaw;

	/* ��̬���� */
	BOOL	dynamic;
	/* �����ļ� */
	BOOL	file;

	/* ���� */
	void	*copy;

	int index;
	struct section_s *next;
} section_s;

/* ����� */
typedef struct import_s {
	char	*dll;

	/* ���� */
	char	*function;
	DWORD	ordinal;

	/* ��ǰָ�� */
	DWORD	point;
	/* ʵ����� */
	DWORD	entry;
	/* HOOK ��� */
	DWORD	hook;

	/* IAT λ�� */
	DWORD	iatVa, iatOffset;
	/* IAT ԭʼ���� */
	DWORD	iatData;

	int index;
	struct import_s *next;
} import_s;

/* ģ�� */
typedef struct module_s {
	char	*path;

	DWORD	address;
	DWORD	size;

	int index;
	struct module_s *next;
} module_s;


/* ������Ϣ */
#if defined(_SNP_PROCESS_)
	/* Ĭ������ */
	config_s	CONFIG			= {FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, 0, {TRUE, TRUE, FALSE, TRUE}, {TRUE, 1, TRUE, TRUE}};

	runtime_s	RUNTIME;
	thread_s	THREAD_HEAD,	*THREAD		= &THREAD_HEAD;

	bp_s		BP_HEAD,		*BP			= &BP_HEAD;
	section_s	SECTION_HEAD,	*SECTION	= &SECTION_HEAD;
	import_s	IMPORT_HEAD,	*IMPORT		= &IMPORT_HEAD;
	module_s	MODULE_HEAD,	*MODULE		= &MODULE_HEAD;
#else
	extern runtime_s	RUNTIME;
	extern config_s		CONFIG;

	extern thread_s		*THREAD;

	extern bp_s			*BP;
	extern section_s	*SECTION;
	extern import_s		*IMPORT;
	extern module_s		*MODULE;
#endif


/* ����ָ�� */
#if defined(_SNP_PE_)
	BOOL (*pfRead)(void *, size_t);
	void (*pfSeek)(long, int);
	long (*pfTell)();
	DWORD (*pfOffset)(DWORD);
	BOOL (*pfEnd)();
#else
	extern BOOL (*pfRead)(void *, size_t);
	extern void (*pfSeek)(long, int);
	extern long (*pfTell)();
	extern DWORD (*pfOffset)(DWORD);
	extern BOOL (*pfEnd)();
#endif


/* ���� */
thNew(Process);

/* ������� */
BOOL procMapModule();
void procMapSection();
void procImport();
void procTitle(char *status);
void procClear();

/* �߳���� */
thread_s * threadGet(DWORD id);
thread_s * threadCreate(HANDLE handle, DWORD id, DWORD addr);
void threadExit(HANDLE handle);

/* �ڴ��д */
BOOL memoryRead(DWORD address, void *buffer, DWORD size);
BOOL memoryWrite(DWORD address, void *buffer, DWORD size);

/* ������� */
BOOL isPeModule(DWORD addr);

/* PE */
BOOL pe();
char * peHeader(BOOL refresh);
char * peSection();
void peImport();
void dump();

/* ��ַת�� */
DWORD va2rva(DWORD va);
DWORD va2addr(DWORD va);
DWORD va2offset(DWORD va);
DWORD rva2va(DWORD rva);
DWORD rva2addr(DWORD rva);
DWORD addr2va(DWORD addr);
DWORD addr2rva(DWORD addr);

/* ���� */
section_s * getSectionByVa(DWORD va);
section_s * getSectionByAddr(DWORD addr);
import_s * getImportByAddr(DWORD addr);
module_s * getModuleByAddr(DWORD addr);

/* ѭ���� */
void loop();
void loopTerminate();
void loopDisasm(BOOL disasm);


/* �����ȡ */
BOOL dataReadFile(void *buffer, size_t size);
BOOL dataReadMemory(void *buffer, size_t size);
void dataSeekFile(long offset, int mode);
void dataSeekMemory(long offset, int mode);
long dataTellFile();
long dataTellMemory();
DWORD dataOffsetFile(DWORD va);
DWORD dataOffsetMemory(DWORD va);
BOOL dataEndFile();
BOOL dataEndMemory();


#if defined(__cplusplus)
	}
#endif

#endif   /* _SNP_IMAGE_H_ */