/*
	$ move.c, 0.1.0113
*/

#include "base.h"


/*
	$ moveLeft			���ո��������

	@ size_t data

	# size_t			�����ƶ����� 0, ���򷵻��ƶ�����·�װ����
*/
size_t moveLeft(size_t data) {
	__asm {
			mov		eax, data

					; �ֽ�ո�λ��
			mov		edx, eax
			shr		edx, 24
			and		edx, 0x0F

					; (1, 1)
			cmp		edx, 1
			jz		edge
					; (1, 2)
			cmp		edx, 4
			je		edge
					; (1, 3)
			cmp		edx, 7
			je		edge

					; �����ƶ�
			dec		edx
			shl		edx, 24

					; �ϲ����
			and		eax, 0xFFFFFF
			or		eax, edx

			jmp		done

		edge:
					; ��߽�
			xor		eax, eax

		done:
	}
}

/*
	$ moveRight			���ո������ƶ�

	@ size_t data

	# size_t
*/
size_t moveRight(size_t data) {
	__asm {
			mov		eax, data

					; �ֽ�ո�λ��
			mov		edx, eax
			shr		edx, 24
			and		edx, 0x0F

					; (3, 1)
			cmp		edx, 3
			je		edge
					; (3, 2)
			cmp		edx, 6
			je		edge
					; (3, 3)
			cmp		edx, 9
			je		edge

					; �����ƶ�
			inc		edx
			shl		edx, 24

					; �ϲ����
			and		eax, 0xFFFFFF
			or		eax, edx

			jmp		done

		edge:
					; �ұ߽�
			xor		eax, eax

		done:
	}
}

/*
	$ moveUp			���ո������ƶ�

	@ size_t data

	# size_t
*/
size_t moveUp(size_t data) {
	__asm {
					; ���
			xor		eax, eax

					; Դ����
			mov		esi, data
			ror		esi, 24

					; �ո�λ��
			mov		edi, esi
			and		edi, 0x0F

					; (1, 1) (2, 1) (3, 1)
			cmp		edi, 4
			jl		done

					; ѭ������
			mov		ecx, 1

					; �����¿ո�λ��
			mov		edx, edi
			sub		edx, 3
			
					; �����¿ո�λ��
			mov		eax, edx

					; �ƶ�����
			rol		esi, 3

					; �ж��Ƿ��ƶ��� (1, 1)
			cmp		ecx, edx
			je		current

		fore:
			shl		eax, 3

			mov		ebx, esi
			and		ebx, 7

			or		eax, ebx

			rol		esi, 3

					; ������ո���λ��ǰ������
			inc		ecx
			cmp		ecx, edx

			jl		fore

		current:
					; ��λ�õ�����
			mov		edx, esi
			and		edx, 7

			rol		esi, 3
			inc		ecx

		next:
			shl		eax, 3

			mov		ebx, esi
			and		ebx, 7

			or		eax, ebx

			rol		esi, 3

					; �������¾�λ�ü������
			inc		ecx
			cmp		ecx, edi

			jl		next

					; �����¿ո�λ�õ�����
			shl		eax, 3
			or		eax, edx

					; �жϴ� (3, 3) �Ƴ�
			cmp		ecx, 9
			je		done

		left:
			shl		eax, 3

			mov		ebx, esi
			and		ebx, 7

			or		eax, ebx

			rol		esi, 3

					; ��������������
			inc		ecx
			cmp		ecx, 9

			jl		left

		done:
	}
}

/*
	$ moveDown			���ո������ƶ�

	@ size_t data

	# size_t
*/
size_t moveDown(size_t data) {
	__asm {
					; ���
			xor		eax, eax

					; Դ����
			mov		esi, data

					; �ո�λ��
			mov		edi, esi
			shr		edi, 24
			and		edi, 0x0F

					; (1, 3) (2, 3) (3, 3)
			cmp		edi, 6
			jg		done

					; ѭ������
			mov		ecx, 8

					; �����¿ո�λ��
			mov		edx, edi
			add		edx, 3

					; �����¿ո�λ��
			mov		eax, edx
			shl		eax, 27

					; �ж��Ƿ��ƶ��� (3, 3)
			dec		edx
			cmp		ecx, edx
			je		current

		fore:
			ror		eax, 3

			mov		ebx, esi
			and		ebx, 7

			or		eax, ebx

			shr		esi, 3

					; ������ո���λ�ú������
			dec		ecx
			cmp		ecx, edx

			jg		fore

		current:
					; ��λ�õ�����
			mov		edx, esi
			and		edx, 7

			shr		esi, 3
			dec		ecx

		next:
			ror		eax, 3

			mov		ebx, esi
			and		ebx, 7

			or		eax, ebx

			shr		esi, 3

					; �������¾�λ�ü������
			dec		ecx
			cmp		ecx, edi

			jge		next

					; �����¿ո�λ�õ�����
			ror		eax, 3
			or		eax, edx

					; �жϴ� (1, 1) �Ƴ�
			test	ecx, ecx
			jz		position

		left:
			ror		eax, 3

			mov		ebx, esi
			and		ebx, 7

			or		eax, ebx

			shr		esi, 3

					; ��������������
			loop	left

		position:
					; ��񻯽��
			rol		eax, 21

		done:
	}
}
