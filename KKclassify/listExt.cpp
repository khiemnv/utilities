#include "StdAfx.h"
#include "listExt.h"

listExt::listExt(void)
{
lvc.mask = LVCF_TEXT;
lvc.cx=LVCF_DEFAULTWIDTH;
lvc.iSubItem = -1;

lvi.state=LVIS_SELECTED | LVIS_FOCUSED;
lvi.mask = LVIF_TEXT;

//auto resize the col width
col_auto_resize=true;
total_row=0;
total_col=0;
}

listExt::~listExt(void)
{
}
void listExt::add_item(int row, int col, CString content)
{		
	BOOL	retval;
	lvi.iItem = row;
	lvi.iSubItem = col;
	lvi.pszText = (LPTSTR)(LPCTSTR)(content);

	if(col!=0)
		retval=cList->SetItem(&lvi);
	else
		retval=cList->InsertItem(&lvi);
	//auto resize the col width
	if(col_auto_resize)
	{
		int width=content.GetLength()*KK_CHAR_WIDTH+KK_H_WIDTH;
		if((colsW[col]<width)==col_auto_resize)
		{
			colsW[col]=width;
			cList->SetColumnWidth(col,colsW[col]);
		}
	}
	//inc total row khi add them dong moi
	if(total_row<=retval) 
		total_row++;
}

void listExt::add_row(int row, CString* contents, int n_col)
{
	for(int i=0;i<n_col;i++)
		add_item(row,i,contents[i]);
	/*row++;*/
}
void listExt::edit_item(int row, int col, CString content)
{
// 	LVITEM Item;
// 
// 	Item.state=LVIS_SELECTED | LVIS_FOCUSED;
// 	/*LVNI_FOCUSED   | LVNI_SELECTED;*/
// 	Item.mask = LVIF_TEXT;
// 	Item.iItem = row;	
// 	Item.iSubItem = col;	
// 	Item.pszText = (LPTSTR)(LPCTSTR)(content);
// 	cList->SetItem(&Item);

	cList->SetItemText(row,col,content);	
}
void listExt::edit_row(int row, CString* contents, int n_col)
{
	for(int i=0;i<n_col;i++)
		edit_item(row,i,contents[i]);
}

void	listExt::add_col(int nCol, CString heading)
{
	BOOL	retval;
	/*lvc.pszText= (LPTSTR)(LPCTSTR)(heading);*/
// 	if(col_auto_resize)
// 		lvc.cx=heading.GetLength()*KK_CHAR_WIDTH+KK_H_WIDTH;
// 	retval=cList->InsertColumn(nCol,&lvc);
	retval=cList->InsertColumn(nCol,heading,LVCFMT_LEFT,LVCF_MINWIDTH);
	//inc total col neu add col moi
	if(total_col<=retval)
		total_col++;
}
void	listExt::create_cols(CString* col_headings,int n_col)
{
	for (int i=0; i<n_col; i++)
		add_col(i,col_headings[i]);
}

void	listExt::set_lvi(LVITEM &_lvi)
{
	lvi=_lvi;
}
void	listExt::set_lvc(LVCOLUMN &_lvc)
{
	lvc=_lvc;
}
void	listExt::clean()
{
	cList->DeleteAllItems();
}
void listExt::del_row(int row)
{
	cList->DeleteItem(row);
	total_row--;
}
void	listExt::add_item(int col, CString content)
{
	add_item(total_row,col,content);
}
void	listExt::add_row(CString* contents, int n_col)
{
	add_row(total_row,contents,n_col);
}
void	listExt::add_col(CString heading)
{
	add_col(total_col,heading);
}