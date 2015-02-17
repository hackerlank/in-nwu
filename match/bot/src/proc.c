/*
	$ Robot for LinLink   (C) 2005-2013 MF
	$ proc.c
*/

#include <Windows.h>
#include <Psapi.h>

#include "base.h"
#include "game.h"


/* ���������� */
#define COUNT		1024
/* ������·�� */
#define BUFFER		(MAX_PATH * 2)


/* ��Ϸ������ */
HWND	form	= NULL;
/* ��Ϸ���̾�� */
HANDLE	process	= NULL;


/* ö�ٽ��� */
static DWORD processFind() {
	DWORD	pid[COUNT];
	char	name[BUFFER];

	HANDLE	handle;
	DWORD	i, size, length;

	/* ö�ٽ��� */
	if(EnumProcesses(pid, COUNT, &size) == FALSE) {
		return 0;
	}

	/* ���������Ŀ */
	if(size) {
		size = size / sizeof(DWORD);
	} else {
		return 0;
	}

	/* ��ȡ����·�� */
	for(i = 0; i < size; i++) {
		if((handle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid[i])) == NULL) {
			continue;
		}
		
		ZeroMemory(&name, sizeof name);
		GetProcessImageFileName(handle, name, BUFFER);

		CloseHandle(handle);

		if((length = strlen(name)) == 0) {
			continue;
		}
		if(length < strlen(GAME_PROCESS)) {
			continue;
		}

		/* �Ƚ��ļ��� */
		if(lstrcmpi(name + length - strlen(GAME_PROCESS), GAME_PROCESS) == 0) {
			return pid[i];
		}
	}

	return 0;
}

/* ���ӽ��� */
static int processAttach(DWORD pid) {
	/* ��Ŀ����� */
	if((process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid)) == NULL) {
		return 0;
	}

	/* ���ӵ��� */
	if(DebugActiveProcess(pid) == FALSE) {
		return 0;
	}

	return 1;
}

/* ������֤ */
static int processValidate() {
	if(memoryEqualV(GAME_CLICK_DATA) == 0) {
		return 0;
	}
	if(memoryEqualV(GAME_HINT_1_DATA) == 0) {
		return 0;
	}
	if(memoryEqualV(GAME_HINT_2_DATA) == 0) {
		return 0;
	}
	if(memoryEqualV(GAME_HINT_3_DATA) == 0) {
		return 0;
	}
	if(memoryEqualV(GAME_HINT_4_DATA) == 0) {
		return 0;
	}
	if(memoryEqualV(GAME_HINT_F_DATA) == 0) {
		return 0;
	}
	if(memoryEqualV(GAME_HINT_S_DATA) == 0) {
		return 0;
	}
	if(memoryEqualV(GAME_LIFE_1_DATA) == 0) {
		return 0;
	}
	if(memoryEqualV(GAME_LIFE_2_DATA) == 0) {
		return 0;
	}
	if(memoryEqualV(GAME_LIFE_3_DATA) == 0) {
		return 0;
	}
	if(memoryEqualV(GAME_LIFE_4_DATA) == 0) {
		return 0;
	}
	if(memoryEqualV(GAME_PASS_1_DATA) == 0) {
		return 0;
	}
	if(memoryEqualV(GAME_PASS_2_DATA) == 0) {
		return 0;
	}

	return 1;
}

/* ��ȡ������ */
static int processWindow(DWORD pid) {
	DWORD processid;

	if((form = FindWindow(TEXT("HGE__WNDCLASS"), NULL)) == NULL) {
		return 0;
	}

	/* ��ȡ���� pid */
	GetWindowThreadProcessId(form, &processid);

	if(pid == processid) {
		return 1;
	} else {
		return 0;
	}
}

/* ��װ�ϵ� */
static int processInstall() {
	/* NOP */
	if(memorySet(GAME_HINT_1_SIZE, 0x90) == 0) {
		return 0;
	}
	if(memorySet(GAME_HINT_2_SIZE, 0x90) == 0) {
		return 0;
	}
	if(memorySet(GAME_HINT_3_SIZE, 0x90) == 0) {
		return 0;
	}
	if(memorySet(GAME_HINT_4_SIZE, 0x90) == 0) {
		return 0;
	}
	if(memorySet(GAME_LIFE_1_SIZE, 0x90) == 0) {
		return 0;
	}
	if(memorySet(GAME_LIFE_2_SIZE, 0x90) == 0) {
		return 0;
	}
	if(memorySet(GAME_LIFE_3_SIZE, 0x90) == 0) {
		return 0;
	}
	if(memorySet(GAME_LIFE_4_SIZE, 0x90) == 0) {
		return 0;
	}

	/* INT 3 */
	if(memorySet(GAME_CLICK_SIZE, 0xCC) == 0) {
		return 0;
	}
	if(memorySet(GAME_HINT_F_SIZE, 0xCC) == 0) {
		return 0;
	}
	if(memorySet(GAME_HINT_S_SIZE, 0xCC) == 0) {
		return 0;
	}
	if(memorySet(GAME_PASS_1_SIZE, 0xCC) == 0) {
		return 0;
	}
	if(memorySet(GAME_PASS_2_SIZE, 0xCC) == 0) {
		return 0;
	}

	return 1;
}


int procMount() {
	DWORD	pid;
	int		success = 0;

	do {
		/* �� pid */
		if((pid = processFind()) == 0) {
			WARNING("û���ҵ���Ϸ���̣�������Ϸ�Ƿ��������С�");			break;
		}

		/* ���ӵ����� */
		if(processAttach(pid) == 0) {
			WARNING("�޷����ص���Ϸ�����ϣ����鱾���������Ȩ�ޡ�");		break;
		}

		/* ��֤���� */
		if(processValidate() == 0) {
			WARNING("�ҵ�����Ϸ����δͨ��������֤�������ǰ汾����ȷ��");		break;
		}

		/* ��ȡ������ */
		if(processWindow(pid) == 0) {
			WARNING("�޷��ҵ���Ϸ���̵������ڣ������ǰ汾����ȷ��");			break;
		}

		/* ��װ�ϵ� */
		if(processInstall() == 0) {
			WARNING("�޸���Ϸ��������ʧ�ܣ���Ϸ�����޷��������С�");			break;
		}

		success = 1;
	} while(0);
	
	/* �ɹ� */
	if(success) {
		buttonRun(1);
		return 1;
	}
	
	/* ����� */
	if(pid) {
		DebugSetProcessKillOnExit(FALSE);
		DebugActiveProcessStop(pid);

		DebugSetProcessKillOnExit(TRUE);
	}

	if(process) {
		CloseHandle(process);
	}

	form	= NULL;
	process	= NULL;

	buttonLoad(1);

	return 0;
}


void procAddress(size_t address, CONTEXT *reg) {
	switch(address) {
		/* ������� */
		case GAME_CLICK_ADDR:
			reg->Esi = reg->Ecx;
			reg->Eip = GAME_CLICK_ADDR + 2;

			if(bpclick == -1) {
				/* ������Ϣ */
				SEND(Run);

				/* ���� Esi */
				bpclick = (int) reg->Ecx;

				/* �ȴ�����ص� */
				WAIT(Stop);
			}

			break;

		/* �ɹ���ȡ��ʾ */
		case GAME_HINT_S_ADDR:
			reg->Eax = reg->Ebx;
			reg->Eip = GAME_HINT_S_ADDR + 2;

			/* �������� */
			x1 = (int) reg->Esi;
			y1 = (int) reg->Ebp;
			x2 = (int) reg->Edi;
			y2 = (int) reg->Ebx;

			SEND(Run);
			break;

		/* ��ȡʧ�� */
		case GAME_HINT_F_ADDR:
			reg->Eax = 0;
			reg->Eip = GAME_HINT_F_ADDR + 2;

			SEND(Stop);
			break;

		/* ���ؽ��� */
		case GAME_PASS_1_ADDR:
		case GAME_PASS_2_ADDR:
			reg->Ecx = reg->Esp;
			reg->Eip = address + 2;

			SEND(Stop);
			break;
	}
}


void procRestore() {
	/* ��� */
	DebugSetProcessKillOnExit(FALSE);

	/* �ָ��ڴ����� */
	memoryWriteV(GAME_CLICK_DATA);
	memoryWriteV(GAME_HINT_1_DATA);
	memoryWriteV(GAME_HINT_2_DATA);
	memoryWriteV(GAME_HINT_3_DATA);
	memoryWriteV(GAME_HINT_4_DATA);
	memoryWriteV(GAME_HINT_F_DATA);
	memoryWriteV(GAME_HINT_S_DATA);
	memoryWriteV(GAME_LIFE_1_DATA);
	memoryWriteV(GAME_LIFE_2_DATA);
	memoryWriteV(GAME_LIFE_3_DATA);
	memoryWriteV(GAME_LIFE_4_DATA);
	memoryWriteV(GAME_PASS_1_DATA);
	memoryWriteV(GAME_PASS_2_DATA);
}
