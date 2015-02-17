/*
	$ Europa Win32 GUI Interface  (C) 2005-2012 mfboy
	$ europa.h, 0.1.1104
*/

#ifndef _EUROPA_H_
#define _EUROPA_H_

#if defined(__cplusplus)
	extern "C" {
#endif


/* ��λ�ͳߴ����� */
typedef signed short int erp_loc_t;
typedef unsigned short int erp_size_t;

/* �������� */
struct erp_window_s;


/* �ؼ����� */
typedef enum { LABEL, BUTTON, EDIT, EDITAREA, RADIO, CHECK, COMBO, LIST } erp_object_e;

/* �ؼ��¼����� */
typedef enum { CLICK, SELECT, FOCUS } erp_event_e;


/* �ؼ������� */
typedef struct erp_object_s {
	/* ���� */
	erp_object_e type;

	/* �ؼ� ID �;�� */
	WORD	id;
	HWND	hwnd;

	/* λ�úʹ�С */
	erp_loc_t	x;
	erp_loc_t	y;
	erp_size_t	width;
	erp_size_t	height;

	/* �������� */
	BOOL	hidden;
	BOOL	disabled;
	char	*text;

	/* ���� */
	void (*hide)(BOOL hidden);
	void (*disable)(BOOL disabled);
	void (*caption)(char *text);

	/* �ص����� */
	void (*callback)(struct erp_window_s *, struct erp_object_s *, erp_event_e, DWORD arg);

	/* ���� */
	struct erp_object_s *next;
} erp_object_s;


/* ��ǩ */
typedef struct {
	erp_object_s object;
} erp_object_label_s;

/* ��ť */
typedef struct {
	erp_object_s object;

	/* ���� */
	void (*click)();
} erp_object_button_s;

/* �����ı��� */
typedef struct {
	erp_object_s object;

	/* ֻ�� */
	void (*readonly)(BOOL readonly);

	/* ���ü���ȡ�ı� */
	void (*setText)(char *format, ...);
	char * (*getText)();
} erp_object_edit_s;

/* �����ı��� */
typedef struct {
	erp_object_s object;

	/* ֻ�� */
	void (*readonly)(BOOL readonly);

	/* �������� */
	void (*setText)(char *format, ...);
	void (*appendText)(char *format, ...);
	/* ��ȡ���� */
	char * (*getText)();
} erp_object_editarea_s;

/* ��ѡ�� */
typedef struct {
	erp_object_s object;

	/* ���� */
	int		group;
	/* ��ѡ�� */
	BOOL	selected;

	/* ѡ�� */
	void (*select)();
} erp_object_radio_s;

/* ��ѡ�� */
typedef struct {
	erp_object_s object;

	/* ��ѡ�� */
	BOOL checked;

	/* ѡ�� */
	void (*check)(BOOL checked);
} erp_object_check_s;

/* ��Ͽ� */
typedef struct {
	erp_object_s object;

	/* ѡ���� */
	int		selected;
	/* ѡ���� */
	size_t	count;

	/* ��ӡ��޸Ļ�ɾ��ĳ�� */
	int (*append)(char *text);
	void (*remove)(int index);
	void (*clear)();

	/* ѡ�� */
	void (*select)(int index);
} erp_object_combo_s;

/* �б�� */
typedef struct {
	erp_object_s object;

	/* �� */
	char	**colTitle;
	size_t	*colWidth;
	size_t	colCount;

	/* ѡ���� */
	int		selected;
	/* �б����� */
	size_t	count;

	/* ��ӡ��޸Ļ�ɾ��ĳ�� */
	int (*append)(char **data);
	void (*modify)(int index, char **data);
	void (*remove)(int index);
	void (*clear)();

	/* ѡ�� */
	void (*select)(int index);
} erp_object_list_s;


/* ���� */
typedef struct erp_window_s {
	HWND		parent;
	HWND		hwnd;
	HINSTANCE	instance;

	char		*classname;

	/* �ڲ���Ϣ */
	struct {
		BOOL	initiated;		/* ��ʼ����� */
		DWORD	quit;			/* �˳���Ϣ */
	} s;

	/* ���ڴ�С */
	erp_size_t	width;
	erp_size_t	height;

	/* ���� */
	char		*title;

	/* ���ߴ��� */
	BOOL		tool;
	/* ������ʾ */
	BOOL		center;

	/* �������� */
	DWORD		param;
	/* ȡ������ */
	BOOL		cancel;

	/* �ص����� */
	void (*draw)(struct erp_window_s *);
	void (*loop)(struct erp_window_s *, HWND, UINT, WPARAM, LPARAM);
	void (*keyboard)(struct erp_window_s *, DWORD key);
	void * (*destroy)(struct erp_window_s *, DWORD arg);

	/* ���� */
	void (*quit)(DWORD arg);
	void (*hidden)(BOOL hidden);
	void (*disable)(BOOL disable);
	void (*caption)(char *format, ...);

	/* �ؼ� */
	erp_object_s *object;
} erp_window_s;



#if defined(_EUROPA_)
	/* ���� */
	static void erpWindowQuit(DWORD arg);
	static void erpWindowHidden(BOOL hidden);
	static void erpWindowDisable(BOOL disable);
	static void erpWindowCaption(char *format, ...);

	/* ������ */
	static void erpObjectHide(BOOL hidden);
	static void erpObjectDisable(BOOL disabled);
	static void erpObjectCaption(char *text);

	/* ��ť */
	static void erpButtonClick();

	/* �����ı��� */
	static void erpEditReadonly(BOOL readonly);
	static void erpEditSetText(char *format, ...);
	static char * erpEditGetText();

	/* �����ı��� */
	static void erpEditareaAppendText(char *format, ...);

	/* ��ѡ�� */
	static void erpRadioSelect();

	/* ��ѡ�� */
	static void erpCheckCheck(BOOL checked);

	/* ��Ͽ� */
	static int erpComboAppend(char *text);
	static void erpComboRemove(int index);
	static void erpComboClear();
	static void erpComboSelect(int index);

	/* �б�� */
	static int erpListAppend(char **data);
	static void erpListModify(int index, char **data);
	static void erpListRemove(int index);
	static void erpListClear();
	static void erpListSelect(int index);
#else
	/* this ָ�� */
	extern erp_object_s *erpThis;
#endif


/* ��ʼ�� */
void erpInit();
/* ���д��� */
void * erpWindow(erp_window_s *win);
/* �ؼ����� */
erp_object_s * erpDraw(erp_window_s *win, erp_object_e type, WORD id, void *callback, erp_loc_t x, erp_loc_t y, erp_size_t width, erp_size_t height, ...);



/* �ص����� */
#define ERP_CALLBACK(func) void func(erp_window_s *win, erp_object_s *obj, erp_event_e evt, DWORD arg)


/* ����ת�� */
#define P_OBJECT(p)		((erp_object_s *)			(p))
#define P_LABEL(p)		((erp_object_label_s *)		(p))
#define P_BUTTON(p)		((erp_object_button_s *)	(p))
#define P_EDIT(p)		((erp_object_edit_s *)		(p))
#define P_EDITAREA(p)	((erp_object_editarea_s *)	(p))
#define P_RADIO(p)		((erp_object_radio_s *)		(p))
#define P_CHECK(p)		((erp_object_check_s *)		(p))
#define P_COMBO(p)		((erp_object_combo_s *)		(p))
#define P_LIST(p)		((erp_object_list_s *)		(p))


/* �ؼ����� */
#define OBJECT(p)		P_OBJECT(erpThis = (erp_object_s *) p)
#define LABEL(p)		P_LABEL(erpThis = (erp_object_s *) p)
#define BUTTON(p)		P_BUTTON(erpThis = (erp_object_s *) p)
#define EDIT(p)			P_EDIT(erpThis = (erp_object_s *) p)
#define EDITAREA(p)		P_EDITAREA(erpThis = (erp_object_s *) p)
#define RADIO(p)		P_RADIO(erpThis = (erp_object_s *) p)
#define CHECK(p)		P_CHECK(erpThis = (erp_object_s *) p)
#define COMBO(p)		P_COMBO(erpThis = (erp_object_s *) p)
#define LIST(p)			P_LIST(erpThis = (erp_object_s *) p)


/* �ؼ����� */
#if defined(ERP_WINDOW)
	#define ERP_DRAW(type, tid, id, cb, x, y, w, h, ...) \
		(erp_object_##type##_s *) erpDraw(&ERP_WINDOW, tid, id, cb, x, y, w, h, __VA_ARGS__)

	#define DRAW_LABEL(id, x, y, w, h, t)				ERP_DRAW(label,		LABEL,		id, NULL,	x, y, w, h, t)
	#define DRAW_BUTTON(id, x, y, w, h, t, cb)			ERP_DRAW(button,	BUTTON,		id, cb,		x, y, w, h, t)
	#define DRAW_EDIT(id, x, y, w, h, t)				ERP_DRAW(edit,		EDIT,		id, NULL,	x, y, w, h, t)
	#define DRAW_EDITAREA(id, x, y, w, h, t)			ERP_DRAW(editarea,	EDITAREA,	id, NULL,	x, y, w, h, t)
	#define DRAW_RADIO(id, x, y, w, h, t, g, cb)		ERP_DRAW(radio,		RADIO,		id, cb,		x, y, w, h, t, g)
	#define DRAW_CHECK(id, x, y, w, h, t, cb)			ERP_DRAW(check,		CHECK,		id, cb,		x, y, w, h, t)
	#define DRAW_COMBO(id, x, y, w, h, cb)				ERP_DRAW(combo,		COMBO,		id, cb,		x, y, w, h)
	#define DRAW_LIST(id, x, y, w, h, ct, cw, cc, cb)	ERP_DRAW(list,		LIST,		id, cb,		x, y, w, h, ct, cw, cc)
#else
	#define ERP_DRAW(win, type, tid, id, cb, x, y, w, h, ...) \
		(erp_object_##type##_s *) erpDraw(&win, tid, id, cb, x, y, w, h, __VA_ARGS__)

	#define DRAW_LABEL(win, id, x, y, w, h, t)				ERP_DRAW(win, label,	LABEL,		id, NULL,	x, y, w, h, t)
	#define DRAW_BUTTON(win, id, x, y, w, h, t, cb)			ERP_DRAW(win, button,	BUTTON,		id, cb,		x, y, w, h, t)
	#define DRAW_EDIT(win, id, x, y, w, h, t)				ERP_DRAW(win, edit,		EDIT,		id, NULL,	x, y, w, h, t)
	#define DRAW_EDITAREA(win, id, x, y, w, h, t)			ERP_DRAW(win, editarea,	EDITAREA,	id, NULL,	x, y, w, h, t)
	#define DRAW_RADIO(win, id, x, y, w, h, t, g, cb)		ERP_DRAW(win, radio,	RADIO,		id, cb,		x, y, w, h, t, g)
	#define DRAW_CHECK(win, id, x, y, w, h, t, cb)			ERP_DRAW(win, check,	CHECK,		id, cb,		x, y, w, h, t)
	#define DRAW_COMBO(win, id, x, y, w, h, cb)				ERP_DRAW(win, combo,	COMBO,		id, cb,		x, y, w, h)
	#define DRAW_LIST(win, id, x, y, w, h, ct, cw, cc, cb)	ERP_DRAW(win, list,		LIST,		id, cb,		x, y, w, h, ct, cw, cc)
#endif


#if defined(__cplusplus)
	}
#endif

#endif   /* _EUROPA_H_ */