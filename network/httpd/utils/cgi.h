
#ifndef __CGI_H__
#define __CGI_H__

//*****************************************************************************
//
// CGI parameters structure
//
//*****************************************************************************
struct CGIParam
{
	char *pcParam;
	char *pcValue;
};

//*****************************************************************************
//
// Prototypes of functions exported by this module.
//
//*****************************************************************************
int ParseUriParameters(char *pcUriParams, struct CGIParam *psCGIParams, int iMaxParams);
int FindCGIParameter(const char *pcToFind, struct CGIParam *psCGIParams, int iParamCount);
int IsValidHexDigit(const char cDigit);
unsigned char HexDigit(const char cDigit);
int DecodeHexEscape(const char *pcEncoded, char *pcDecoded);
unsigned long EncodeFormString(const char *pcDecoded, char *pcEncoded, unsigned long ulLen);
unsigned long DecodeFormString(const  char *pcEncoded, char *pcDecoded, unsigned long ulLen);
int CheckDecimalParam(const char *pcValue, long *plValue);
int CheckFixedDecimalParam(const char *pcValue, long *plValue);
int GetCGIParam(const char *pcName, struct CGIParam *psCGIParams, int iParamCount, long *plValue);
int GetFixedDecimalCGIParam(const char *pcName, struct CGIParam *psCGIParams, int iParamCount, long *plValue);
int GetIPCGIParam(const char *pcName, struct CGIParam *psCGIParams, int iParamCount, long *plValue);
int GetMacCGIParam(const char *pcName, struct CGIParam *psCGIParams, int iParamCount, unsigned char *pucMacAddr);

#endif // __CGI_H__
