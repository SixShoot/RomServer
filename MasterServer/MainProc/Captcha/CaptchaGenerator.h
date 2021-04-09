#pragma once

#ifndef __CAPTCHA_GEN_H__
#define __CAPTCHA_GEN_H__

#include <vector>
#include <map>
#include <string>

#pragma managed(push, off)
// ************************************************************************************************************************************************************
class CSimpleBitmapImage
{
protected:
	int				m_Width;
	int				m_Height;
	void*			m_Image;
	unsigned long	m_ImageSize;

public:
	CSimpleBitmapImage();
	virtual ~CSimpleBitmapImage();

public:
	bool LoadFromFile(const char* pszFileName);
	void Clear();
	bool GetPixel(int iX, int iY, unsigned long* pdwValue);
	bool SetPixel(int iX, int iY, unsigned long dwValue);
	void ChangePixelColor(unsigned long dwOldColor, unsigned long dwNewColor);

public:
	int				GetWidth()				{return m_Width;}
	void			SetWidth(int value)		{m_Width = value;}
	int				GetHeight()				{return m_Height;}
	void			SetHeight(int value)	{m_Height = value;}
	void*			GetImage()				{return m_Image;}
	unsigned long	GetImageSize()			{return m_ImageSize;}

public:
	__declspec(property(get = GetWidth		, put = SetWidth	)) int				Width;
	__declspec(property(get = GetHeight		, put = SetHeight	)) int				Height;
	__declspec(property(get = GetImage							)) void*			Image;
	__declspec(property(get = GetImageSize						)) unsigned long	ImageSize;
};

// ************************************************************************************************************************************************************
class CCaptchaGenerator
{
protected:
	bool								m_ResourceLoaded;
	std::string							m_ResourcePath;
	std::string							m_CharFileNamePrefix;
	std::string							m_BackgroudFileNamePrefix;
	std::string							m_NoiseFileNamePrefix;
	int									m_BackgroudImgCount;
	int									m_NoiseImgCount;
	std::string							m_ResultString;
	int									m_ResultMaxCharCount;
	int									m_ResultAlphabetCount;			//How many alphabets in result image, -1 = all chars random with numbers and alphabets.

	void*								m_RawImage;
	unsigned long						m_RawImageSize;
	unsigned long						m_RawMemorySize;

	std::vector<char>					m_AvalibleAlphabet;
	std::vector<char>					m_AvalibleNumber;
	std::vector<char>					m_AvalibleChars;

	std::map<int, CSimpleBitmapImage*>	m_Chars;
	std::vector<CSimpleBitmapImage*>	m_Backgrouds;
	std::vector<CSimpleBitmapImage*>	m_Noises;

public:
	CCaptchaGenerator(const char* pszResourcePath, const char* pszCharFileNamePrefix, const char* pszBackgroudFileNamePrefix, const char* pszNoiseFileNamePrefix, int iBackgroudImgCount, int iNoiseImgCount, int iResultMaxCharCount = 4, int iResultAlphabetCount = 2);
	virtual ~CCaptchaGenerator();

protected:
	void			LoadResource();
	void			ClearResource();
	void			ClearResultImage();
	void			GenerateResultString();
	unsigned long	AlphaBlending(unsigned long dwPix1, unsigned long dwPix2, unsigned char Alpha);
	unsigned long	AlphaBlending(unsigned long dwPix1, unsigned long dwPix2);

public:
	bool			Refresh();

public:
	const char*		GetResultString()	{return m_ResultString.c_str();}
	void*			GetRawImage()		{return m_RawImage;}	
	unsigned long	GetRawImageSize()	{return m_RawImageSize;}

public:
	__declspec(property(get = GetResultString	)) const char*		ResultString;
	__declspec(property(get = GetRawImage		)) void*			RawImage;
	__declspec(property(get = GetRawImageSize	)) unsigned long	RawImageSize;
};
// ************************************************************************************************************************************************************
#pragma managed(pop)

#endif