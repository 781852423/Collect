#include "stdafx.h"
#include "Utilties.h"
#include <fstream>
using namespace std;

wchar_t* c2w(const char* str)
{
	if (str == NULL)
	{
		return L"";
	}

	int length = strlen(str) + 1;
	wchar_t *t = (wchar_t*)malloc(sizeof(wchar_t)*length);
	memset(t, 0, length*sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, str, strlen(str), t, length);
	return t;
}


BOOL SaveBitmap(const BITMAP &bm, HDC hDC, HBITMAP hBitmap, LPCTSTR szFileName)
{

	int nBitPerPixel = bm.bmBitsPixel;//�����ɫģʽ
	int nW = bm.bmWidth;
	int nH = bm.bmHeight;
	int nPalItemC = bm.bmPlanes;	//��ɫ����ĸ���
	int nBmpInfSize = sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*nPalItemC;//λͼ��Ϣ�Ĵ�С
	int nDataSize = (nBitPerPixel*nW + 31) / 32 * 4 * nH;//λͼ���ݵĴ�С
	//��ʼ��λͼ��Ϣ
	BITMAPFILEHEADER bfh = { 0 };//λͼ�ļ�ͷ
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER)+nBmpInfSize;//sizeof(BITMAPINFOHEADER);//��λͼ���ݵ�ƫ����
	bfh.bfSize = bfh.bfOffBits + nDataSize;//�ļ��ܵĴ�С
	bfh.bfType = (WORD)0x4d42;//λͼ��־
	char * p = new char[nBmpInfSize + nDataSize];//�����ڴ�λͼ���ݿռ�(������Ϣͷ)
	memset(p, 0, nBmpInfSize);//����Ϣͷ�����ݳ�ʼ��Ϊ0
	LPBITMAPINFOHEADER pBih = (LPBITMAPINFOHEADER)p;//λͼ��Ϣͷ
	pBih->biCompression = BI_RGB;
	pBih->biBitCount = nBitPerPixel;//ÿ��ͼԪ����ʹ�õ�λ��
	pBih->biHeight = nH;//�߶�
	pBih->biWidth = nW;//���
	pBih->biPlanes = 1;
	pBih->biSize = sizeof(BITMAPINFOHEADER);
	pBih->biSizeImage = nDataSize;//ͼ�����ݴ�С
	char *pData = p + nBmpInfSize;
	//DDBת��ΪDIB
	::GetDIBits(hDC, hBitmap, 0, nH, pData, (LPBITMAPINFO)pBih, DIB_RGB_COLORS);//��ȡλͼ����
	ofstream ofs(szFileName, ios::binary);
	if (ofs.fail())
		return FALSE;
	ofs.write((const char*)&bfh, sizeof(BITMAPFILEHEADER));//д��λͼ�ļ�ͷ
	ofs.write((const char*)pBih, nBmpInfSize);//д��λͼ��Ϣ����
	ofs.write((const char*)pData, nDataSize);//д��λͼ����
	return TRUE;
}


std::string GenerateFileName()
{
	std::string szRet = "";
	char timeBuffer[30];
	time_t nowtime = time(NULL);
	tm timeTemp;
	localtime_s(&timeTemp, &nowtime);
	sprintf_s(timeBuffer, "%04d-%02d-%02d_%02d-%02d-%02d", timeTemp.tm_year + 1900, timeTemp.tm_mon + 1,
		timeTemp.tm_mday, timeTemp.tm_hour, timeTemp.tm_min, timeTemp.tm_sec);
	szRet = timeBuffer;
	return szRet;
}


int  GetEncoderClsid(const   WCHAR*   format, CLSID*   pClsid)
{
	using namespace Gdiplus;

	UINT num = 0;                     // number of image encoders   
	UINT size = 0;                   // size of the image encoder array in bytes   

	ImageCodecInfo*   pImageCodecInfo = NULL;
	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;
		 
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return   -1;     // Failure   

	GetImageEncoders(num, size, pImageCodecInfo);
	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return   j;     // Success   
		}
	}
	free(pImageCodecInfo);
	return   -1;     // Failure   
}


void SaveJpg(Gdiplus::Bitmap* pImage, const WCHAR* pFileName)
{
	using namespace Gdiplus;
	EncoderParameters encoderParameters;
	CLSID jpgClsid;
	GetEncoderClsid(L"image/jpeg", &jpgClsid);
	encoderParameters.Count = 1;
	encoderParameters.Parameter[0].Guid = EncoderQuality;
	encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParameters.Parameter[0].NumberOfValues = 1;

	// Save the image as a JPEG with quality level 100.
	ULONG             quality;
	quality = 100;
	encoderParameters.Parameter[0].Value = &quality;
	
	Status status = pImage->Save(pFileName, &jpgClsid, &encoderParameters);
	if (status != Ok)
	{
		wprintf(L"%d  Attempt to save %s failed.\n", status, pFileName);
	}
}