#include "stdafx.h"
#include "KResizing.h"

CKResizing::CKResizing(void)
{
	setSpaces(0,0,0,0,0,0);
	minRect.SetRectEmpty();
	maxRect.SetRectEmpty();
	validRect.SetRect(0,0,KR_WIDTH,KR_HEIGHT);
}

CKResizing::~CKResizing(void)
{
}
void CKResizing::setSpaces(
						   int left=0,
						   int right=0, 
						   int top=0, 
						   int bot=0, 
						   int horizontal=0, 
						   int vertical=0)
{
	leftSpace=left;
	rightSpace=right;
	topSpace=top;
	botSpace=bot;
	xSpace=horizontal;
	ySpace=vertical;
}

CRect CKResizing::grid(
					  CWnd* wnd, 
					  int bcol,//begin col
					  int ncol,//n col
					  int brow,//begin row
					  int nrow,//n row
					  int tcol,//total col
					  int trow,//total row
					  CRect rect)
{
	int x,y,nx,ny;
	if (tcol==0 || trow==0)
		return CRect(0,0,0,0);

	nx=(rect.Width() -leftSpace-rightSpace+xSpace)/tcol;
	//nx=(nx-leftSpace-rightSpace+xSpace)/tcol;
	ny=(rect.Height() -topSpace-botSpace+ySpace)/trow;
	//ny=(ny-topSpace=botSpace+ySpace)/trow;
	x=nx * bcol + leftSpace + rect.left;
	y=ny * brow + topSpace + rect.top;
	nx=nx * ncol - xSpace;
	ny=ny * nrow - ySpace;

	if(wnd!=0)
		wnd->MoveWindow(x,y,nx,ny);

	return CRect(x,y,x+nx,y+ny);
}

CRect CKResizing::grid(CWnd *wnd, int xpos, int ypos, int width, int height, CRect rect)
{
	int x,y;
	CRect temRect;

	if(width==0) width=rect.Width() - leftSpace -rightSpace;
	if(height==0) height=rect.Height() - topSpace - botSpace;

	temRect.SetRect(0,0,width,height);

	switch(xpos)
	{
	case KR_LEFT:
		x=leftSpace;
		break;
	case KR_MID:
		x=(rect.Width() - width)/2;
		break;
	default:
		x=rect.Width() - width - rightSpace;
	}
	x += rect.left;

	switch(ypos)
	{
	case KR_TOP:
		y=topSpace;
		break;
	case KR_MID:
		y=(rect.Height() - height)/2;
		break;
	default:
		y=rect.Height() - height - botSpace;
	}
	y += rect.top;

	temRect.MoveToXY(x,y);

	if(wnd!=0)
		wnd->MoveWindow(&temRect);

	return temRect;
}

CRect CKResizing::grid(CWnd *wnd, int xpos, int ypos, CRect fixedRect, CRect rect)
{
	int x,y;

	switch(xpos)
	{
	case KR_LEFT:
		x=leftSpace;
		break;
	case KR_MID:
		x=(rect.Width() - fixedRect.Width())/2;
		break;
	default:
		x=rect.Width() - fixedRect.Width() - rightSpace;
	}
	x += rect.left;

	switch(ypos)
	{
	case KR_TOP:
		y=topSpace;
		break;
	case KR_MID:
		y=(rect.Height() - fixedRect.Height())/2;
		break;
	default:
		y=rect.Height() - fixedRect.Height() - botSpace;
	}
	y += rect.top;

	fixedRect.MoveToXY(x,y);

	if(wnd!=0)
		wnd->MoveWindow(&fixedRect);

	return fixedRect;
}

void CKResizing::setMinRect(CRect rect)
{
	minRect=rect;
}
void CKResizing::setMaxRect(CRect rect)
{
	maxRect=rect;
}
BOOL CKResizing::isValidRect(CRect rect)
{
	BOOL retval=TRUE;
	validRect=rect;
	if(!minRect.IsRectEmpty())
	{
		if(rect.Width() < minRect.Width())
		{
			validRect.right=validRect.left+minRect.Width();
			retval = FALSE;
		}
		if(rect.Height() < minRect.Height())
		{
			validRect.bottom=validRect.top+minRect.Height();
			retval = FALSE;
		}
	}

	if(!maxRect.IsRectEmpty())
	{
		if(rect.Width() > maxRect.Width())
		{
			validRect.right=validRect.left+maxRect.Width();
			retval = FALSE;
		}
		if(rect.Height() > maxRect.Height())
		{
			validRect.bottom=validRect.top+maxRect.Height();
			retval = FALSE;
		}
	}
	return retval;
}
void CKResizing::fixInvalidRect(CRect& rect)
{
	rect=validRect;
}
