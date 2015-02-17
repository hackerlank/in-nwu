/*
	$ flag.c, 0.1.0113
*/

#include "base.h"


/* ��־��Ϣ�ռ� */
static void *flag = NULL;


/*
	$ flagInit			��ʼ����־��Ϣ�ռ�

	# int
*/
void flagInit() {
	if(flag) {
		flagClear();
		return;
	}

	if((flag = calloc(0xFFFFFF, 4)) == NULL) {
		exit(1);
	}
}

/*
	$ flagClear			������б�־��Ϣ
*/
void flagClear() {
	__asm {
		pushfd
		cld

				; ��־λ�ռ�
		mov		edi, flag
				; �����ֽ���
		mov		ecx, 0xFFFFFF

				; д�������
		xor		eax, eax

				; ��տռ�
		rep		stos DWORD PTR [edi]

		popfd
	}
}

/*
	$ flagSet			����־λ

	@ size_t data
	@ size_t direction

	# int				�����־λ������, ���ط� 0, �������ñ�־λ������ 0
*/
int flagSet(size_t data, size_t direction) {
	__asm {
			mov		ecx, data
			mov		esi, ecx

					; ��������
			and		esi, 0xFFFFFF
			shl		esi, 2

					; ��λ�ڴ�λ��
			add		esi, flag

					; ��ȡ��־˫��
			mov		eax, [esi]

					; ����ƫ����
			shr		ecx, 24
			and		ecx, 0x0F

					; ���ģ��
			mov		edx, 7

					; ����ģ��
			mov		ebx, direction
			and		ebx, 7

					; ģ����λ
			dec		ecx
			jz		check

		move:
			shl		edx, 3
			shl		ebx, 3

					; ��λ���
			loop	move

		check:
					; ����־λ
			test	eax, edx
			jnz		done

					; ����λ��Ϣ
			or		eax, ebx
					; ����λ��Ϣ
			mov		[esi], eax

			xor		eax, eax

		done:
	}
}

/*
	$ flagGet			��ȡ��־λ

	@ size_t data

	# int				���ڵ��ƶ���ʽ, ��δ���ʹ����� 0
*/
size_t flagGet(size_t data) {
	__asm {
			mov		ecx, data
			mov		ebx, ecx

					; ��������
			and		ebx, 0xFFFFFF
			shl		ebx, 2

					; ��λ�ڴ�λ��
			add		ebx, flag

					; ��ȡ��־˫��
			mov		eax, [ebx]

					; ����ƫ����
			shr		ecx, 24
			and		ecx, 0x0F

					; �жϱ�־λ�Ƿ���Ҫ��λ
			dec		ecx
			jz		done

		move:
			shr		eax, 3
			loop	move

		done:
			and		eax, 7
	}
}
