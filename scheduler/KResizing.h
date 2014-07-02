#ifndef _KRESIZING_H_
#define _KRESIZING_H_

#ifndef KR_POSITION
#define KR_POSITION
#define KR_LEFT 1
#define KR_TOP	1
#define KR_MID	2
#define KR_RIGHT 4
#define KR_BOT	4
#define KR_WIDTH 300
#define KR_HEIGHT 200
#endif

#define KR_SCALE(x) ((x)*12/7)
#define KR_UNSCALE(x) ((x)*7/12)

#pragma once

class CKResizing abstract
{
public:
	CKResizing(void);
	~CKResizing(void);
	void setSpaces(
		int left,
		int right, 
		int top, 
		int bot, 
		int horizontal,
		int vertical);
	//grid(cwnd*,0,1,0,1,1,1,wndrect);
	CRect grid(
		CWnd* wnd,
		int bcol,//begin col
		int ncol,//n col
		int brow,//begin row
		int nrow,//n row
		int tcol,//total col
		int trow,//total row
		CRect rect);
	CRect grid(CWnd* wnd, 
		int xpos, //left, mid, right
		int ypos, //top, mid,boot
		CRect fixedRect, 
		CRect rect);
	CRect grid(CWnd* wnd, 
		int xpos, //left, mid, right
		int ypos, //top, mid,boot
		int width, //=0 -> width=rect.width
		int height, //=0 ->height = rect.height
		CRect rect);
	void setMinRect(CRect rect);
	void setMaxRect(CRect rect);
	//return true if minRect.width<=rect.width<=maxRect.width
	//				and minRect.height<=rect.height<=maxRect.width
	BOOL isValidRect(CRect rect);
private:
	CRect minRect;
	CRect maxRect;
	CRect validRect;
	int leftSpace;
	int rightSpace;
	int topSpace;
	int botSpace;
	int xSpace;
	int ySpace;
public:
	void fixInvalidRect(CRect& rect);
};
#endif