
#pragma once

#include "pch.h"
#include "math.h"
#include <iostream>

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)


class CImg  
{
public:
	CImg();
	CImg(CImg& gray);


	void operator = (CImg& gray); 
	BOOL operator == (CImg& gray);
	CImg operator & (CImg& gray); 
	CImg operator | (CImg& gray); 
	CImg operator + (CImg gray); 
	CImg operator - (CImg& gray); 
	

	virtual ~CImg();

public:	
	
	BOOL IsValidate() { return m_pBMIH != NULL; }

	void InitPixels(BYTE color);


	BOOL AttachFromFile(LPCTSTR lpcPathName);
	BOOL AttachFromFile(CFile &file);

	BOOL SaveToFile(LPCTSTR lpcPathName);
	BOOL SaveToFile(CFile &file);


	BOOL Draw(CDC* pDC);
	BOOL Draw(CDC* pDC, CRect rt);


	void SetPixel(int x, int y, COLORREF color);

	COLORREF GetPixel(int x, int y);

	BYTE GetGray(int x, int y);



	int GetWidthByte();

	int GetWidthPixel();

	int GetHeight();


	void ImResize(int nHeight, int nWidth);
	void GenHist(double * hist, int n = 256);
	void GenHistrgb(double *histr,double *histg,double *histb);
public:

	void Circle(int x, int y, int r = 5){};

public:

	BOOL IsBinaryImg();

	BOOL IsIndexedImg();

	bool Index2Gray();

	LPVOID GetColorTable(){return m_lpvColorTable;}
	int GetColorTableEntriesNum(){return m_nColorTableEntries;}
private:
	void CleanUp();	

public:

	BITMAPINFOHEADER *m_pBMIH;
	LPBYTE  *m_lpData;
	int corners[4][2];

protected:
	int m_nColorTableEntries;
	LPVOID m_lpvColorTable;
public:
	BOOL DrawPart(CDC* pDC, int x_order, int y_order, int size);
	int DetectThreshold(int iter);
	    
	void Threshold(CImg* pImg, int thres);
};

inline int CImg::GetWidthByte()
{
	return WIDTHBYTES((m_pBMIH->biWidth)*m_pBMIH->biBitCount);
}

inline int CImg::GetWidthPixel()
{
	return m_pBMIH->biWidth;
}

inline int CImg::GetHeight()
{
	return m_pBMIH->biHeight;
}

inline BYTE CImg::GetGray(int x, int y)
{
	COLORREF ref = GetPixel(x, y);
	BYTE r, g, b, byte;
	r = GetRValue(ref);
	g = GetGValue(ref);
	b = GetBValue(ref);

	if(r == g && r == b)
		return r;

	double dGray = (0.30*r + 0.59*g + 0.11*b);

	byte =  (int)dGray;
				
	return byte;
}

inline COLORREF CImg::GetPixel(int x, int y)
{
	if(m_pBMIH->biBitCount == 8)
	{
		BYTE byte = m_lpData[m_pBMIH->biHeight - y - 1][x];
		return RGB(byte, byte, byte);
	}
	else if(m_pBMIH->biBitCount == 1)
	{
		BYTE ret = (1<<(7-x%8) & m_lpData[m_pBMIH->biHeight - y - 1][x/8]);

		RGBQUAD *p = (RGBQUAD*)m_lpvColorTable;
		if(p[0].rgbBlue != 0)
			ret = !ret;

		if(ret)
			return RGB(255, 255, 255);	
		else 
			return RGB(0, 0, 0);
	}
	else if(m_pBMIH->biBitCount == 24)	
	{
		COLORREF color = RGB(m_lpData[m_pBMIH->biHeight - y - 1][x*3 + 2], 
		m_lpData[m_pBMIH->biHeight - y - 1][x*3 + 1],
		m_lpData[m_pBMIH->biHeight - y - 1][x*3]);
		return color;
	}
	else
	{
		throw "not support now";
		return 0;
	}
}

inline BOOL CImg::IsBinaryImg()
{
	int i,j;
	
	for(i = 0; i < m_pBMIH->biHeight; i++)
	{
		for(j = 0; j < m_pBMIH->biWidth; j++)
		{
			if( (GetGray(j, i) != 0) && (GetGray(j, i) != 255) )
				return FALSE;
		}
	}

	return TRUE;
}

inline BOOL CImg::IsIndexedImg()
{
	if ((m_lpvColorTable != NULL)&&(m_nColorTableEntries!=0)) {
		return true;
	}
	else {
		return false;
	}
}
