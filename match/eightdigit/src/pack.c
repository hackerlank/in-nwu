/*
	$ pack.c, 0.1.0113
*/

#include "base.h"


/*
	$ pack				��װ����

	@ void *number

	# size_t
*/
size_t pack(void *number) {
	__asm {
			pushfd
			cld

					; ����Դ
			mov		esi, number
					; ���ָ���
			mov		ecx, 8

					; ��ȡ�ո�λ��
			lods	BYTE PTR [esi]
			and		eax, 0x0F

					; ��װ�ո�
			mov		edx, eax

		parse:
			lods	BYTE PTR [esi]

					; ��ո�λ
			dec		al
			and		al, 7

					; �ϲ�����
			shl		edx, 3
			or		dl, al

			loop	parse

					; ����
			mov		eax, edx

			popfd
	}
}

/*
	$ unpack			�������

	@ void *number
	@ size_t data
*/
void unpack(void *number, size_t data) {
	__asm {
			pushfd
			std

					; ��װ���
			mov		edx, data
					; ���ָ���
			mov		ecx, 8

					; ����Դ
			mov		edi, number
			add		edi, 8

		parse:
					; ȡ����ǰ����
			mov		al, dl
			and		al, 7
			inc		al

					; ��������
			stos	BYTE PTR [edi]

					; �Ƴ�����
			shr		edx, 3

			loop	parse

					; ����ո�λ��
			mov		al, dl
			and		al, 0x0F
			stos	BYTE PTR [edi]

			popfd
	}
}
