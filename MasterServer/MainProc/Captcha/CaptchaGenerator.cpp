#include "CaptchaGenerator.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "lzo/lzo1x.h"

#pragma managed(push, off)
// ************************************************************************************************************************************************************
struct MyPixel
{
	union
	{
		DWORD dwARGB;

		struct
		{
			BYTE B;
			BYTE G;
			BYTE R;
			BYTE A;
		}Bytes;
	};

	MyPixel()
	{
		memset(this, 0, sizeof(*this));
	}
};

// ************************************************************************************************************************************************************
//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
CSimpleBitmapImage::CSimpleBitmapImage()
{
	m_Width			= 0;
	m_Height		= 0;
	m_Image			= NULL;
	m_ImageSize		= 0;
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
CSimpleBitmapImage::~CSimpleBitmapImage()
{
	Clear();
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
bool CSimpleBitmapImage::LoadFromFile(const char* pszFileName)
{
	bool Result = false;	

	Clear();

	HANDLE hFile = CreateFileA(
		pszFileName,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if( hFile != INVALID_HANDLE_VALUE )
	{
		BITMAPFILEHEADER bmfHeader;
		BITMAPINFOHEADER bi;

		unsigned long dwBytesRead = 0;
		ReadFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesRead, NULL);
		ReadFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesRead, NULL);

		m_ImageSize = ((bi.biWidth * bi.biBitCount + 31) / 32) * 4 * bi.biHeight;

		m_Image = new unsigned char[m_ImageSize];
		SecureZeroMemory(m_Image, m_ImageSize);

		ReadFile(hFile, (LPSTR)m_Image, m_ImageSize, &dwBytesRead, NULL);

		m_Width	 = bi.biWidth;
		m_Height = bi.biHeight;

		CloseHandle(hFile);

		Result = true;
	}

	return Result;
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void CSimpleBitmapImage::Clear()
{
	m_Width		= 0;
	m_Height	= 0;	
	m_ImageSize = 0;

	if (m_Image)
	{
		delete m_Image;
		m_Image	= NULL;
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
bool CSimpleBitmapImage::GetPixel(int iX, int iY, unsigned long* pdwValue)
{
	unsigned long* ptr = (unsigned long*)m_Image;;

	if( iX > m_Width )
	{
		//*pdwValue = 0 | 0xff000000;
		*pdwValue = 0;
		return false;
	}

	if( iY > m_Height )
	{
		//*pdwValue = 0 | 0xff000000;
		*pdwValue = 0;
		return false;
	}
	//int iOffsetY = m_Height - iY - 1;
	//ptr = ptr + ( m_Width * iOffsetY ) + iX;
	ptr = ptr + ( m_Width * iY ) + iX;

	//*pdwValue = *ptr | 0xff000000;
	*pdwValue = *ptr;

	return true;
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
bool CSimpleBitmapImage::SetPixel(int iX, int iY, unsigned long dwValue)
{
	unsigned long* ptr = (unsigned long*)m_Image;

	if( iX > m_Width )
	{
		return false;
	}

	if( iY > m_Height )
	{
		return false;
	}

	//int iOffsetY = m_Height - iY - 1;
	//ptr = ptr + ( m_Width * iOffsetY ) + iX;
	ptr = ptr + ( m_Width * iY ) + iX;

	*ptr = dwValue;

	return true;
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void CSimpleBitmapImage::ChangePixelColor(unsigned long dwOldColor, unsigned long dwNewColor)
{
	unsigned long dwPixel = 0;
	MyPixel p, pOld, pNew;
	pOld.dwARGB = dwOldColor;
	pNew.dwARGB = dwNewColor;

	int iDeltaR = pNew.Bytes.R - pOld.Bytes.R;
	int iDeltaG = pNew.Bytes.G - pOld.Bytes.G;
	int iDeltaB = pNew.Bytes.B - pOld.Bytes.B;

	int iR = (pOld.Bytes.R == 0) ? 1 : pOld.Bytes.R;
	int iG = (pOld.Bytes.G == 0) ? 1 : pOld.Bytes.G;
	int iB = (pOld.Bytes.B == 0) ? 1 : pOld.Bytes.B;

	float fRatioR = iDeltaR / (float)iR;
	float fRatioG = iDeltaG / (float)iG;
	float fRatioB = iDeltaB / (float)iB;

	for (int y = 0 ; y < m_Height; ++y)
	{
		for (int x = 0 ; x < m_Width; ++x)
		{
			if (GetPixel(x, y, &dwPixel))
			{
				p.dwARGB = dwPixel;

				int iR = (int)(p.Bytes.R * (1 + fRatioR));
				int iG = (int)(p.Bytes.G * (1 + fRatioG));
				int iB = (int)(p.Bytes.B * (1 + fRatioB));

				if (iR > 0X000000FF)
					iR = 0X000000FF;
				if (iG > 0X000000FF)
					iG = 0X000000FF;
				if (iB > 0X000000FF)
					iB = 0X000000FF;

				p.Bytes.R = (unsigned char)iR;
				p.Bytes.G = (unsigned char)iG;
				p.Bytes.B = (unsigned char)iB;
				SetPixel(x, y, p.dwARGB);
			}
		}
	}
}
// ************************************************************************************************************************************************************
//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
CCaptchaGenerator::CCaptchaGenerator(const char* pszResourcePath, const char* pszCharFileNamePrefix, const char* pszBackgroudFileNamePrefix, const char* pszNoiseFileNamePrefix, int iBackgroudImgCount, int iNoiseImgCount, int iResultMaxCharCount, int iResultAlphabetCount)
{
	m_ResourceLoaded			= false;
	m_ResourcePath				= pszResourcePath;
	m_CharFileNamePrefix		= pszCharFileNamePrefix;
	m_BackgroudFileNamePrefix	= pszBackgroudFileNamePrefix;
	m_NoiseFileNamePrefix		= pszNoiseFileNamePrefix;
	m_BackgroudImgCount			= iBackgroudImgCount;
	m_NoiseImgCount				= iNoiseImgCount;
	m_ResultString				= "";
	m_ResultMaxCharCount		= iResultMaxCharCount;
	m_ResultAlphabetCount		= iResultAlphabetCount;

	m_RawImage					= NULL;
	m_RawImageSize				= 0;
	m_RawMemorySize				= 0;

	if (m_ResultAlphabetCount > m_ResultMaxCharCount)
	{
		m_ResultAlphabetCount = m_ResultMaxCharCount;
	}

	char ch = '0';
	for (int i = 0; i < 10; ++i)
	{
		if (ch != '0')
		{
			m_AvalibleNumber.push_back(ch);
			m_AvalibleChars.push_back(ch);
		}
		ch++;
	}

	ch = 'A';
	for (int i = 0; i < 26; ++i)
	{
		if (ch != 'B' && ch != 'D' && ch != 'I' && ch!= 'O' && ch!= 'Q' && ch!= 'T')
		{
			m_AvalibleAlphabet.push_back(ch);
			m_AvalibleChars.push_back(ch);
		}
		ch++;
	}


	srand(time(NULL));
	LoadResource();
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
CCaptchaGenerator::~CCaptchaGenerator()
{
	ClearResultImage();
	ClearResource();
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void CCaptchaGenerator::LoadResource()
{
	ClearResource();

	bool m_ResourceLoaded = true;

	char Buff[MAX_PATH];

	char ch = '0';
	for (int i = 0; i < 10; ++i)
	{
		sprintf(Buff, "%s/%s%c.bmp", m_ResourcePath.c_str(), m_CharFileNamePrefix.c_str(), ch);
		CSimpleBitmapImage* img = new CSimpleBitmapImage();			
		img->LoadFromFile(Buff);
		img->ChangePixelColor(0X00FFFFFF, 0X00FFEC8B);
		m_Chars[ch] = img;
		ch++;
	}

	ch = 'A';
	for (int i = 0; i < 26; ++i)
	{
		sprintf(Buff, "%s/%s%c.bmp", m_ResourcePath.c_str(), m_CharFileNamePrefix.c_str(), ch);
		CSimpleBitmapImage* img = new CSimpleBitmapImage();
		img->LoadFromFile(Buff);
		img->ChangePixelColor(0X00FFFFFF, 0X007FFFD4);
		m_Chars[ch] = img;
		ch++;
	}

	for (int i = 1; i <= m_BackgroudImgCount; ++i)
	{		
		sprintf(Buff, "%s/%s%d.bmp", m_ResourcePath.c_str(), m_BackgroudFileNamePrefix.c_str(), i);
		CSimpleBitmapImage* img = new CSimpleBitmapImage();
		img->LoadFromFile(Buff);
		m_Backgrouds.push_back(img);
	}

	for (int i = 1; i <= m_NoiseImgCount; ++i)
	{		
		sprintf(Buff, "%s/%s%d.bmp", m_ResourcePath.c_str(), m_NoiseFileNamePrefix.c_str(), i);
		CSimpleBitmapImage* img = new CSimpleBitmapImage();
		img->LoadFromFile(Buff);
		m_Noises.push_back(img);
	}
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void CCaptchaGenerator::ClearResource()
{
	CSimpleBitmapImage* pImg = NULL;

	for (std::map<int, CSimpleBitmapImage*>::iterator itChar = m_Chars.begin(); itChar != m_Chars.end(); itChar++)
	{
		pImg = itChar->second;
		if (pImg)
		{
			delete pImg;
		}
	}

	for (int i = 0 ; i < (int)m_Backgrouds.size(); ++i)
	{
		pImg = m_Backgrouds[i];

		if (pImg)
		{
			delete pImg;
		}
	}

	for (int i = 0 ; i < (int)m_Noises.size(); ++i)
	{
		pImg = m_Noises[i];

		if (pImg)
		{
			delete pImg;
		}
	}

	m_Chars.clear();
	m_Backgrouds.clear();
	m_Noises.clear();
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void CCaptchaGenerator::ClearResultImage()
{
	if (m_RawImage)
	{
		delete m_RawImage;
		m_RawImage = NULL;
	}
	m_RawImageSize = 0;
	m_RawMemorySize = 0;
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
void CCaptchaGenerator::GenerateResultString()
{	
	//------------------------------------------------------------------------------
	//random pick up chars
	std::vector<char> vecPickedChars;

	if (m_ResultAlphabetCount < 0)
	{
		for (int i = 0; i < m_ResultMaxCharCount; ++i)
		{
			vecPickedChars.push_back(m_AvalibleChars[rand() % m_AvalibleChars.size()]);
		}
	}
	else
	{
		for (int i = 0; i < m_ResultAlphabetCount; ++i)
		{
			vecPickedChars.push_back(m_AvalibleAlphabet[rand() % m_AvalibleAlphabet.size()]);
		}

		for (int i = 0; i < (m_ResultMaxCharCount - m_ResultAlphabetCount); ++i)
		{
			vecPickedChars.push_back(m_AvalibleNumber[rand() % m_AvalibleNumber.size()]);
		}
	}
	//------------------------------------------------------------------------------

	//------------------------------------------------------------------------------
	//decide position of each char
	std::map<int, char> mapChars;
	std::map<int, char>::iterator itChars;

	for (int i = 0; i < (int)vecPickedChars.size(); ++i)
	{
		int iPos = 0;
		do 
		{
			iPos = rand();
			itChars = mapChars.find(iPos);
		} while (itChars != mapChars.end());

		mapChars[iPos] = vecPickedChars[i];
	}

	m_ResultString = "";

	for (itChars = mapChars.begin(); itChars != mapChars.end(); itChars++)
	{
		m_ResultString += itChars->second;
	}
	//------------------------------------------------------------------------------
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
unsigned long CCaptchaGenerator::AlphaBlending(unsigned long dwPix1, unsigned long dwPix2, unsigned char Alpha)
{
	float fAlpha = Alpha / (float)255.0;

	if (fAlpha > 1)
	{
		fAlpha = 1;
	}

	MyPixel p1, p2, Result;
	p1.dwARGB = dwPix1;
	p2.dwARGB = dwPix2;

	Result.Bytes.A = 0xFF;
	Result.Bytes.R = (unsigned char)(p1.Bytes.R * fAlpha + p2.Bytes.R * (1 - fAlpha));
	Result.Bytes.G = (unsigned char)(p1.Bytes.G * fAlpha + p2.Bytes.G * (1 - fAlpha));
	Result.Bytes.B = (unsigned char)(p1.Bytes.B * fAlpha + p2.Bytes.B * (1 - fAlpha));

	return Result.dwARGB;
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
unsigned long CCaptchaGenerator::AlphaBlending(unsigned long dwPix1, unsigned long dwPix2)
{
	MyPixel p1;
	p1.dwARGB = dwPix1;

	return AlphaBlending(dwPix1, dwPix2, p1.Bytes.A);
}

//-----------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------
bool CCaptchaGenerator::Refresh()
{
	GenerateResultString();

	//------------------------------------------------------------------------------
	//choose background and noise image
	int iRandom = 0;
	CSimpleBitmapImage* imgBg = NULL;
	CSimpleBitmapImage* imgNs = NULL;

	iRandom = rand() % m_BackgroudImgCount;
	if ((iRandom >= 0) && (iRandom < (int)m_Backgrouds.size()))
	{
		imgBg = m_Backgrouds[iRandom];
	}

	iRandom = rand() % m_NoiseImgCount;
	if ((iRandom >= 0) && (iRandom < (int)m_Noises.size()))
	{
		imgNs = m_Noises[iRandom];
	}
	//------------------------------------------------------------------------------

	//------------------------------------------------------------------------------
	//image check
	if (imgBg == NULL || imgNs == NULL)
	{
		return false;
	}

	if ((imgBg->Width != imgNs->Width) || (imgBg->Height != imgNs->Height))
	{
		return false;
	}

	if (m_ResultString.length() != m_ResultMaxCharCount)
	{
		return false;
	}

	for (int i = 0; i < (int)m_ResultString.length(); ++i)
	{
		CSimpleBitmapImage* imgCh = m_Chars[m_ResultString[i]];

		if (imgCh == NULL)
			return false;
	}
	//------------------------------------------------------------------------------

	
	try
	{
		//------------------------------------------------------------------------------
		//allocate memory for image data
		//int iImageSize = imgBg->ImageSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		//16bits bmp
		unsigned long dwImageSize = (imgBg->Width * imgBg->Height * 2) + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(unsigned long) * 3;

		if (dwImageSize > m_RawMemorySize)
		{
			ClearResultImage();
			m_RawImage = new BYTE[dwImageSize];
			m_RawMemorySize = dwImageSize;
		}

		m_RawImageSize = dwImageSize;	
		SecureZeroMemory(m_RawImage, m_RawMemorySize);
		PBYTE pTraveler = (PBYTE)m_RawImage;
		//------------------------------------------------------------------------------

		//------------------------------------------------------------------------------
		//fill bitmap file header
		BITMAPFILEHEADER* pbmfHeader = (BITMAPFILEHEADER*) pTraveler;
		pbmfHeader->bfType		= 0x4D42;
		pbmfHeader->bfOffBits	= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		pbmfHeader->bfSize		= m_RawImageSize;
		pTraveler += sizeof(BITMAPFILEHEADER);
		//------------------------------------------------------------------------------

		//------------------------------------------------------------------------------
		//fill bitmap info header
		BITMAPINFOHEADER* pbi = (BITMAPINFOHEADER*)pTraveler;
		pbi->biSize			 = sizeof(BITMAPINFOHEADER);
		pbi->biWidth		 = imgBg->Width;
		pbi->biHeight		 = imgBg->Height;
		pbi->biPlanes		 = 1;
		pbi->biBitCount		 = 16;
		pbi->biCompression   = BI_BITFIELDS;
		pbi->biXPelsPerMeter = 2834;
		pbi->biYPelsPerMeter = 2834;
		pTraveler += sizeof(BITMAPINFOHEADER);

		//set bitmap bits to indicate RGB 565
		unsigned long *ptr = (unsigned long *)(pTraveler);
		ptr[0] = 0xF800;
		ptr[1] = 0x07E0;
		ptr[2] = 0x001F;
		pTraveler += sizeof(unsigned long) * 3;
		//------------------------------------------------------------------------------

		//------------------------------------------------------------------------------
		//fill the result image data	
		if (imgBg != NULL && imgNs != NULL)
		{
			for (int y = 0; y < imgBg->Height; ++y)
			{
				for (int i = 0; i < (int)m_ResultString.length(); ++i)
				{
					CSimpleBitmapImage* imgCh = m_Chars[m_ResultString[i]];

					if (imgCh != NULL)
					{
						int iXBegin = imgCh->Width * i;
						int iXEnd   = imgCh->Width * (i + 1);

						//prevent out of range
						if ((iXBegin <= imgBg->Width) && (iXEnd <= imgBg->Width))
						{
							for (int x = iXBegin; x < iXEnd; ++x)
							{
								MyPixel p1, p2, p3, pResult;
								imgCh->GetPixel(x - iXBegin, y, &p1.dwARGB);
								imgBg->GetPixel(x, y, &p2.dwARGB);
								imgNs->GetPixel(x, y, &p3.dwARGB);

								if (p1.Bytes.A < 50)
								{
									p1.Bytes.A = 50;
								}

								pResult.dwARGB = AlphaBlending(p1.dwARGB, p2.dwARGB);

								p3.Bytes.A = (unsigned char)(p3.Bytes.A * (float)0.5);

								pResult.dwARGB = AlphaBlending(p3.dwARGB, pResult.dwARGB);

								unsigned short* pPixel = (unsigned short*)pTraveler;

								//RGB565
								(*pPixel) = (pResult.Bytes.B >> 3) + ((pResult.Bytes.G >> 2)<<5) + ((pResult.Bytes.R >> 3)<<11);

								pTraveler += sizeof(unsigned short);
							}
						}
					}
				}
			}
		}
		//------------------------------------------------------------------------------
	}
	catch(...)
	{
		return false;
	}

	return true;
}

// ************************************************************************************************************************************************************
#pragma managed(pop)