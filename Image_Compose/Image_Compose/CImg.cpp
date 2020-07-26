#include "CImg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif




CImg::CImg()
{
	m_pBMIH = NULL;
	m_lpvColorTable = NULL;

	m_lpData = NULL;
}

BOOL CImg::operator == (CImg& gray)
{
	int nHeight = GetHeight();
	int nWidth = GetWidthPixel();

	if(nHeight != gray.GetHeight())
		return false;

	if(nWidth != gray.GetWidthPixel())
		return false;


	for(int i=0; i<nHeight; i++)
	{
		for(int j=0; j<nWidth; j++)
		{
			if( GetGray(j, i) != gray.GetGray(j, i) )
				return false;
		}
	}

	return true;
}

CImg CImg::operator & (CImg& gray)
{
	CImg grayRet = *this;
	
	int nHeight = GetHeight();
	int nWidth = GetWidthPixel();
	
	if((nHeight != gray.GetHeight())||(nWidth != gray.GetWidthPixel()))
	{
		return grayRet;
	}

	for(int i=0; i<nHeight; i++)
	{
		for(int j=0; j<nWidth; j++)
		{
			if(gray.GetGray(j, i) == 255)
				grayRet.SetPixel(j, i, RGB(255, 255, 255));
		}
	}

	return grayRet;	
}

CImg CImg::operator | (CImg& gray)
{
	CImg grayRet = *this;
	
	int nHeight = GetHeight();
	int nWidth = GetWidthPixel();
	
	if((nHeight != gray.GetHeight())||(nWidth != gray.GetWidthPixel()))
	{

		return grayRet;
	}

	for(int i=0; i<nHeight; i++)
	{
		for(int j=0; j<nWidth; j++)
		{
			if(gray.GetGray(j, i) == 0)
				grayRet.SetPixel(j, i, RGB(0, 0, 0));
		}
	}

	return grayRet;
}

CImg CImg::operator + (CImg gray)
{
	CImg grayRet;
	grayRet = *this;
	BYTE dgray;

	int nHeight = GetHeight();
	int nWidth = GetWidthPixel();

	int i, j;
	for(j=0; j<nHeight; j++)
	{
		for(i=0; i<nWidth; i++)
		{
			dgray = (grayRet.GetGray(i,j)+gray.GetGray(i,j))/2;
			grayRet.SetPixel(i,j,RGB(dgray,dgray,dgray));

		}
	}
	
	
	
	return grayRet;
}


void CImg::operator = (CImg& gray)
{
	CleanUp();

	m_nColorTableEntries = gray.m_nColorTableEntries;


	if(gray.m_pBMIH != NULL)
	{
		m_pBMIH = (BITMAPINFOHEADER*)new BYTE[sizeof(BITMAPINFOHEADER) + m_nColorTableEntries*4];
		memcpy(m_pBMIH, gray.m_pBMIH, sizeof(BITMAPINFOHEADER) + m_nColorTableEntries*4);

		if(m_nColorTableEntries != 0)
		{
			m_lpvColorTable = m_pBMIH + 1;
		}
	}

	int nWidthBytes = WIDTHBYTES((m_pBMIH->biWidth)*m_pBMIH->biBitCount);
	m_lpData = new LPBYTE[m_pBMIH->biHeight];
	for(int i=0; i<m_pBMIH->biHeight; i++)
	{
		m_lpData[i] = new BYTE[nWidthBytes];
		memcpy(m_lpData[i], gray.m_lpData[i], nWidthBytes);
	}
}

CImg::CImg(CImg& gray)
{	
	m_pBMIH = NULL;
	m_lpvColorTable = NULL;

	m_nColorTableEntries = gray.m_nColorTableEntries;
	
	if(gray.m_pBMIH != NULL)
	{
		m_pBMIH = (BITMAPINFOHEADER*)new BYTE[sizeof(BITMAPINFOHEADER) + m_nColorTableEntries*4];
		memcpy(m_pBMIH, gray.m_pBMIH, sizeof(BITMAPINFOHEADER) + m_nColorTableEntries*4);

		if(m_nColorTableEntries != 0)
		{
			m_lpvColorTable = m_pBMIH + 1;
		}
	}

	int nWidthBytes = WIDTHBYTES((m_pBMIH->biWidth)*m_pBMIH->biBitCount);
	m_lpData = new LPBYTE[m_pBMIH->biHeight];
	for(int i=0; i<m_pBMIH->biHeight; i++)
	{
		m_lpData[i] = new BYTE[nWidthBytes];
		memcpy(m_lpData[i], gray.m_lpData[i], nWidthBytes);
	}
}

CImg::~CImg()
{
	CleanUp();
}

void CImg::CleanUp()
{	
	if(m_lpData != NULL)
	{
		int nWidthBytes = WIDTHBYTES((m_pBMIH->biWidth)*m_pBMIH->biBitCount);
		for(int i=0; i<m_pBMIH->biHeight; i++)
		{
			delete[] m_lpData[i];
		}
		delete[] m_lpData;
	}

	if(m_pBMIH != NULL)
	{
		delete[] m_pBMIH;
		m_pBMIH = NULL;
	}
}

void CImg::ImResize(int nHeight, int nWidth)
{
	int i; 
	for(i=0; i<m_pBMIH->biHeight; i++)
	{
		delete[] m_lpData[i];
	}
	delete[] m_lpData;

	m_pBMIH->biHeight = nHeight;
	m_pBMIH->biWidth = nWidth; 

	m_lpData = new LPBYTE [nHeight];
	int nWidthBytes = WIDTHBYTES((m_pBMIH->biWidth)*m_pBMIH->biBitCount);
	for(i=0; i<nHeight; i++)
	{
		m_lpData[i] = new BYTE [nWidthBytes];
	}
}
void CImg::InitPixels(BYTE color)
{

	int nHeight = GetHeight();
	int nWidth = GetWidthPixel();

	int i, j;
	if(m_lpData != NULL)
	{
		for(i=0; i<GetHeight(); i++)
		{
			for(j=0; j<GetWidthPixel(); j++)	
			{
				SetPixel(j, i, RGB(color, color, color));
			}//for j
		}//for i
	}
}

BOOL CImg::AttachFromFile(LPCTSTR lpcPathName)
{
	CFile file;
	if(!file.Open(lpcPathName, CFile::modeRead|CFile::shareDenyWrite))
		return FALSE;

	BOOL bSuc = AttachFromFile(file);

	file.Close();
	return bSuc;
}

BOOL CImg::AttachFromFile(CFile &file)
{
	LPBYTE  *lpData;
	BITMAPINFOHEADER *pBMIH;
	LPVOID lpvColorTable = NULL;
	int nColorTableEntries;

	BITMAPFILEHEADER bmfHeader;

	if(!file.Read(&bmfHeader, sizeof(bmfHeader)))
		return FALSE;

	
	if(bmfHeader.bfType != MAKEWORD('B', 'M'))
	{
		return FALSE;
	}

	pBMIH = (BITMAPINFOHEADER*)new BYTE[bmfHeader.bfOffBits - sizeof(bmfHeader)];
	if(!file.Read(pBMIH, bmfHeader.bfOffBits - sizeof(bmfHeader)))
	{
		delete pBMIH;
		return FALSE;
	}

	nColorTableEntries = 
		(bmfHeader.bfOffBits - sizeof(bmfHeader) - sizeof(BITMAPINFOHEADER))/sizeof(RGBQUAD);
	if(nColorTableEntries > 0)
	{
		lpvColorTable = pBMIH + 1;
	}

	pBMIH->biHeight = abs(pBMIH->biHeight);

	int nWidthBytes = WIDTHBYTES((pBMIH->biWidth)*pBMIH->biBitCount);

	lpData = new LPBYTE[(pBMIH->biHeight)];
	for(int i=0; i<(pBMIH->biHeight); i++)
	{
		lpData[i] = new BYTE[nWidthBytes];
		file.Read(lpData[i], nWidthBytes);
		
	}

	CleanUp();

	m_lpData = lpData;
	m_pBMIH = pBMIH;
	m_lpvColorTable = lpvColorTable;
	m_nColorTableEntries = nColorTableEntries;
	

	return TRUE;
}

BOOL CImg::SaveToFile(LPCTSTR lpcPathName)
{
	if(!IsValidate())
		return FALSE;

	CFile file;
	if(!file.Open(lpcPathName, CFile::modeRead|CFile::modeWrite|CFile::modeCreate))
	{
		return FALSE;
	}

	BOOL bSuc = SaveToFile(file);
	file.Close();

	return bSuc;
}
BOOL CImg::SaveToFile(CFile &file)
{	
	if(!IsValidate())
		return FALSE;	
	
		
	BITMAPFILEHEADER bmfHeader = { 0 };
	int nWidthBytes = WIDTHBYTES((m_pBMIH->biWidth)*m_pBMIH->biBitCount);


	bmfHeader.bfType = MAKEWORD('B', 'M');
	bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) 
				+ sizeof(BITMAPINFOHEADER) + m_nColorTableEntries*4;

	bmfHeader.bfSize = bmfHeader.bfOffBits + m_pBMIH->biHeight * nWidthBytes;
	
	file.Write(&bmfHeader, sizeof(bmfHeader));
	file.Write(m_pBMIH, sizeof(BITMAPINFOHEADER) + m_nColorTableEntries*4);

	for(int i=0; i<m_pBMIH->biHeight; i++)
	{
		file.Write(m_lpData[i], nWidthBytes);
	}
	

	return TRUE;
}
BOOL CImg::Draw(CDC* pDC)
{
	if (m_pBMIH == NULL)
		return FALSE;

	for (int i = 0; i < m_pBMIH->biHeight; i++)
	{

		::SetDIBitsToDevice(*pDC, 0/*0*/, 0/*0*/, m_pBMIH->biWidth,
			m_pBMIH->biHeight, 0/*0*/, 0/*0*/, i, 1, m_lpData[i], (BITMAPINFO*)m_pBMIH, DIB_RGB_COLORS);
	}
	return TRUE;
}


BOOL CImg::Draw(CDC* pDC, CRect rt)
{
	if(m_pBMIH == NULL)
		return FALSE;

	if(rt.IsRectNull())
	{
		rt.right = m_pBMIH->biWidth;
		rt.bottom = m_pBMIH->biHeight;
	}

	if(rt.Width() > m_pBMIH->biWidth)
	{
		rt.right = rt.left + m_pBMIH->biWidth;
	}

	if(rt.Height() > m_pBMIH->biHeight)
	{
		rt.bottom = rt.top + m_pBMIH->biHeight;
	}

	for(int i=0; i<m_pBMIH->biHeight; i++)
	{
	
			::SetDIBitsToDevice(*pDC, rt.left, rt.top, rt.Width(), 
				rt.Height(), 0, 0, i, 1, m_lpData[i], (BITMAPINFO*)m_pBMIH, DIB_RGB_COLORS);
	
	}

	return TRUE;
}
void CImg::SetPixel(int x, int y, COLORREF color)
{
	if(m_pBMIH->biBitCount == 8)		
	{
		m_lpData[m_pBMIH->biHeight - y - 1][x] = GetRValue(color);
	}
	else if(m_pBMIH->biBitCount == 1)	
	{
		BYTE Color = GetRValue(color);

		RGBQUAD *p = (RGBQUAD*)m_lpvColorTable;
		if(p[0].rgbBlue != 0)
			Color = !Color;

		y = m_pBMIH->biHeight - y - 1;
		
		if (Color == 0) 
		{
			if(x % 8==0)
			{
				m_lpData[y][x/8] &= 0x7F;  
			}
			else if(x % 8==1)
			{
				m_lpData[y][x/8] &= 0xBF;
			}
			else if(x % 8==2)
			{
				m_lpData[y][x/8] &= 0xDF;
			}
			else if(x % 8==3)
			{
				m_lpData[y][x/8] &= 0xEF;
			}
			else if(x % 8==4)
			{
				m_lpData[y][x/8] &= 0xF7;
			}
			else if(x % 8==5)
			{
				m_lpData[y][x/8] &= 0xFB;
			}
			else if(x % 8==6)
			{
				m_lpData[y][x/8] &= 0xFD;
			}
			else if(x % 8==7)
			{
				m_lpData[y][x/8] &= 0xFE;
			}
		}
		else // 白色点
		{
			if(x % 8==0)
			{
				m_lpData[y][x/8] |= 0x80;
			}
			else if(x % 8==1)
			{
				m_lpData[y][x/8] |= 0x40;
			}
			else if(x % 8==2)
			{
				m_lpData[y][x/8] |= 0x20;
			}
			else if(x % 8==3)
			{
				m_lpData[y][x/8] |= 0x10;
			}
			else if(x % 8==4)
			{
				m_lpData[y][x/8] |= 0x08;
			}
			else if(x % 8==5)
			{
				m_lpData[y][x/8] |= 0x04;
			}
			else if(x % 8==6)
			{
				m_lpData[y][x/8] |= 0x02;
			}
			else if(x % 8==7)
			{
				m_lpData[y][x/8] |= 0x01;
			}
		}
	}
	else if(m_pBMIH->biBitCount == 24)
	{
		BYTE byte1=GetBValue(color);
		BYTE byte2=GetGValue(color);
		BYTE byte3=GetRValue(color);
		m_lpData[m_pBMIH->biHeight - y - 1][x*3] = GetBValue(color);
		m_lpData[m_pBMIH->biHeight - y - 1][x*3 + 1] = GetGValue(color);
		m_lpData[m_pBMIH->biHeight - y - 1][x*3 + 2] = GetRValue(color);
		int a=1;
	}
}

bool CImg::Index2Gray()
{
	int i;

	if (!IsIndexedImg()) return false;
	RGBQUAD *table = (RGBQUAD*)m_lpvColorTable;

	m_pBMIH->biBitCount = 8;

	for (i=0; i<GetHeight(); i++)
	{
		for (int j=0; j<GetWidthPixel(); j++)
		{
			RGBQUAD rgb = *(table+GetGray(j, i));
			BYTE gray = rgb.rgbBlue * 0.114 + rgb.rgbGreen * 0.587 + rgb.rgbRed * 0.299 + 0.5;
			SetPixel(j, i, RGB(gray, gray, gray));
		}
	}
	
	for (i=0; i<256; i++)
	{
		(table + i)->rgbBlue = i;
		(table + i)->rgbGreen = i;
		(table + i)->rgbRed = i;
		(table + i)->rgbReserved = 0;
	}

	m_nColorTableEntries = 256;
	return true;
}


CImg CImg::operator - (CImg &gray)
{
	CImg grayRet;
	grayRet = *this;

	int nHeight = GetHeight();
	int nWidth = GetWidthPixel();

	int i, j, color;

	for(j=0; j<nHeight; j++)
	{
		for(i=0; i<nWidth; i++)
		{
			color=(grayRet.GetPixel(i,j)-gray.GetPixel(i,j)+255)/2;
			grayRet.SetPixel(i,j,0);

		}
	}
			
	return grayRet;
}



BOOL CImg::DrawPart(CDC* pDC, int x_order, int y_order, int size)
{
	return 0;
}


void CImg::GenHist(double * pdHist, int n)
{

	double dDivider;

	memset(pdHist, 0, n * sizeof(double));
	dDivider = 256.0 / (double)n;
	
	BYTE bGray;	
	for (int i=0; i<m_pBMIH->biHeight; i++)
	{
		for (int j=0; j<m_pBMIH->biWidth; j++)
		{
			bGray = GetGray(j, i);
			pdHist[(int)(bGray / dDivider)]++;
		}
	};

	UINT square = m_pBMIH->biWidth * m_pBMIH->biHeight;

	for (int k=0; k<n; k++) 
	{
		pdHist[k]=pdHist[k]/square;
	}
}



void CImg::GenHistrgb(double * pdHistr,double * pdHistg,double * pdHistb)
{

	double dDivider;
	COLORREF ref;
	BYTE r,g,b;
	int n=256;

	memset(pdHistr, 0, n * sizeof(double));
	memset(pdHistg, 0, n * sizeof(double)); 
	memset(pdHistb, 0, n * sizeof(double));

	dDivider = 256.0 / (double)n;
	

	for (int i=0; i<m_pBMIH->biHeight; i++)
	{
		for (int j=0; j<m_pBMIH->biWidth; j++)
		{
			ref = GetPixel(j , i);
			r = GetRValue(ref);
			g = GetGValue(ref);
			b = GetBValue(ref);
			pdHistr[(int)(r / dDivider)]++;
			pdHistg[(int)(g / dDivider)]++;
			pdHistb[(int)(b / dDivider)]++;
		}
	};

	UINT square = m_pBMIH->biWidth * m_pBMIH->biHeight;

	for (int k=0; k<n; k++) 
	{
		pdHistr[k]=pdHistr[k]/square;
		pdHistg[k]=pdHistg[k]/square;
		pdHistb[k]=pdHistb[k]/square;
	}
}
          
int CImg::DetectThreshold(int iter)
{

	int nThreshold;
	
	int nHistogram[256] = { 0 };
	int i, j;
	
	BYTE bt;
	int nDiffRet;
	int nMin = 255;
	int nMax = 0;
	int width=GetWidthPixel();
	int height=GetHeight();
	int nMaxIter=100;

	for(j = 0; j < height; j ++)
	{
		for(i=0; i<width; i++)
		{
			bt = GetGray(i,j);
			
			if(bt < nMin)
				nMin = bt;
			if(bt > nMax)
				nMax = bt;
			
			nHistogram[bt] ++;
			
		}
	}
	
	int nTotalGray = 0;
	int nTotalPixel = 0;
	int nNewThreshold = (nMax + nMin)/2;
	
	nDiffRet = nMax - nMin;

	if (nMax == nMin)
		nThreshold = nNewThreshold;

	else
	{
		nThreshold = 0;
		
		for(int nIterationTimes = 0; nThreshold != nNewThreshold && nIterationTimes < nMaxIter; nIterationTimes ++)
		{
			nThreshold = nNewThreshold;
			nTotalGray = 0;
			nTotalPixel = 0;

			for(i=nMin; i<nThreshold; i++)
			{
				nTotalGray += nHistogram[i]*i;
				nTotalPixel += nHistogram[i];
			}
			int nMean1GrayValue = nTotalGray/nTotalPixel;


			nTotalGray = 0;
			nTotalPixel = 0;
			
			for(i=nThreshold + 1; i<=nMax; i++)
			{
				nTotalGray += nHistogram[i]*i;
				nTotalPixel += nHistogram[i];
			}
			int nMean2GrayValue = nTotalGray/nTotalPixel;
	
			nNewThreshold = (nMean1GrayValue + nMean2GrayValue)/2; //计算出新的阈值
			nDiffRet = abs(nMean1GrayValue - nMean2GrayValue);
		}




	}
	return nThreshold;
}


void CImg::Threshold(CImg* pImg, int thres)
{
	int width=GetWidthPixel();
	int height=GetHeight();
	BYTE bt;

	for(int j = 0; j < height; j ++)
	{
		for(int i=0; i<width; i++)
		{
			bt = GetGray(i,j);
			
			if(bt < thres)
				bt=0;
			else
				bt = 255;
			
			pImg->SetPixel(i,j,RGB(bt,bt,bt));
		}
	}

}
