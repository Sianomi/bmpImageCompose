
// Image_ComposeDoc.cpp: CImageComposeDoc 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Image_Compose.h"
#endif

#include "Image_ComposeDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CImageComposeDoc

IMPLEMENT_DYNCREATE(CImageComposeDoc, CDocument)

BEGIN_MESSAGE_MAP(CImageComposeDoc, CDocument)
END_MESSAGE_MAP()


// CImageComposeDoc 생성/소멸

CImageComposeDoc::CImageComposeDoc() noexcept
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.
	m_Img = new CImg;
	m_MImg = new CImg;
	m_OImg = new CImg;

}

CImageComposeDoc::~CImageComposeDoc()
{
}

BOOL CImageComposeDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CImageComposeDoc serialization

void CImageComposeDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CImageComposeDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CImageComposeDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CImageComposeDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CImageComposeDoc 진단

#ifdef _DEBUG
void CImageComposeDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CImageComposeDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CImageComposeDoc 명령


BOOL CImageComposeDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	m_Img->AttachFromFile(lpszPathName);

	return true;
}

BOOL CImageComposeDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	return 0;
}

void CImageComposeDoc::OnFrameComb()
{
	CFile File;
	CFileDialog OpenDlg(TRUE);
	COLORREF maskvalue;

	AfxMessageBox(L"합성할 영상을 입력하시오");
	if (OpenDlg.DoModal() == IDOK) { // 합성할 영상을 입력
		File.Open(OpenDlg.GetPathName(), CFile::modeRead);
		m_OImg->AttachFromFile(File);
		File.Close();
	}
	AfxMessageBox(L"입력 영상의 마스크 영상을 입력하시오");
	if (OpenDlg.DoModal() == IDOK) { // 입력 영상의 마스크 영상
		File.Open(OpenDlg.GetPathName(), CFile::modeRead);
		m_MImg->AttachFromFile(File);
		File.Close();
	}

	for (int y = 0;y < 512 ; y++) {
		for (int x = 0; x < 512; x++)
		{
			maskvalue = 0xffffff - m_MImg->GetPixel(x, y);
			maskvalue = (m_Img->GetPixel(x,y) & m_MImg->GetPixel(x, y)) | (m_OImg->GetPixel(x, y) & maskvalue);
			m_Img->SetPixel(x, y, maskvalue);
		}
	}
	CString temp = CTime::GetCurrentTime().Format(_T("..\\SampleImage\\%Y-%m-%d %H.%M.%S.bmp"));
	m_Img->SaveToFile(temp);
}