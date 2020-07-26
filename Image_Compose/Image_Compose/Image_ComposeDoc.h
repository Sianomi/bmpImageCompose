
// Image_ComposeDoc.h: CImageComposeDoc 클래스의 인터페이스
//

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
#pragma once

#include "pch.h"
#include "CImg.h"
#include <ctime>

class CImageComposeDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CImageComposeDoc() noexcept;
	DECLARE_DYNCREATE(CImageComposeDoc)

// 특성입니다.
public:

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 구현입니다.
public:
	virtual ~CImageComposeDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	CImg* m_Img;
	CImg* m_MImg;
	CImg* m_OImg;
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	void OnFrameComb();
#ifdef SHARED_HANDLERS
	// 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
