#pragma once

#define KK_MAX_NCOL	20
#define KK_CHAR_WIDTH 5	//width of a char tinh bang pixel
#define KK_H_WIDTH 25	//width of a heading char tinh bang pixel

class listExt
{
public:
	listExt(void);
	~listExt(void);
//constructor
	listExt(CListCtrl* ptrCList)
	{
		cList=ptrCList;
		listExt();
	}
//methods
	void	add_item(int row, int col, CString content);
	void	add_row(int row, CString* contents, int n_col);
	//add item  vao cuoi list
	void	add_item(int col, CString content);
	void	add_row(CString* contents, int n_col);

	void	add_col(int nCol, CString heading);
	//add 1 col vao cuoi
	void	add_col(CString heading);
	void	create_cols(CString* col_headings,int n_col);
	void	del_row(int row);
	void	edit_item(int row, int col, CString content);
	void	edit_row(int row, CString* contents, int n_col);

	void	chose_clist(CListCtrl *ptrClist)
	{
		cList=ptrClist;
	}
	void	set_lvi(LVITEM &_lvi);
	void	set_lvc(LVCOLUMN &_lvc);
	void	clean();
//variants
	CListCtrl	*cList;
	LVITEM		lvi;
	LVCOLUMN	lvc;
	int			colsW[KK_MAX_NCOL];
	bool		col_auto_resize;
	int			total_row;
	int			total_col;
};
