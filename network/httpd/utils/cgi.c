//*****************************************************************************
//
// cgifuncs.c - Helper functions related to CGI script parameter parsing.
//
// Copyright (c) 2008-2010 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 6459 of the DK-LM3S9B96 Firmware Package.
//
//*****************************************************************************

#include <string.h>
#include <lwiplib.h>
#include "lib/types.h"
#include "lib/ustdlib.h"
#include "cgi.h"

//*****************************************************************************
//
// Parses uri parameters
//
//*****************************************************************************
int
ParseUriParameters(char *pcUriParams, struct CGIParam *psCGIParams, int iMaxParams)
{
	char *pcPair;
	char *pcEquals;
	int  iLoop = 0;

	//
	// If we have no parameters at all, return immediately
	//
	if(pcUriParams && pcUriParams[0])
	{
		//
		// Get a pointer to our first parameter
		//
		pcPair = pcUriParams;

		//
		// Parse up to MAX_CGI_PARAMETERS from the passed string and ignore the
		// remainder (if any)
		//
		while((iLoop < iMaxParams) && pcPair)
		{
			//
			// Save the name of the parameter
			//
			psCGIParams[iLoop].pcParam = pcPair;

			//
			// Remember the start of this name=value pair
			//
			pcEquals = pcPair;

			//
			// Find the start of the next name=value pair and replace the delimiter
			// with a 0 to terminate the previous pair string.
			//
			pcPair = strchr(pcPair, '&');

			if(pcPair)
			{
				*pcPair = '\0';
				pcPair++;
			}
			else
			{
				//
				// We didn't find a new parameter so find the end of the URI and
				// replace the space with a '\0'
				//
				pcPair = strchr(pcEquals, ' ');

				if(pcPair)
				{
					*pcPair = '\0';
				}

				//
				// Revert to NULL so that we exit the loop as expected
				//
				pcPair = NULL;
			}

			//
			// Now find the '=' in the previous pair, replace it with '\0' and save
			// the parameter value string
			//
			pcEquals = strchr(pcEquals, '=');

			if(pcEquals)
			{
				*pcEquals = '\0';

				psCGIParams[iLoop].pcValue = pcEquals + 1;
			}
			else
			{
				psCGIParams[iLoop].pcValue = NULL;
			}

			iLoop++;
		}
	}

	return iLoop;
}

//*****************************************************************************
//
// Searches the list of parameters passed to a CGI handler and returns the
// index of a given parameter within that list.
//
//*****************************************************************************
int
FindCGIParameter(const char *pcToFind, struct CGIParam *psCGIParams, int iParamCount)
{
    int iLoop;

    //
    // Scan through all the parameters in the array.
    //
    for(iLoop = 0; iLoop < iParamCount; iLoop++)
    {
        //
        // Does the parameter name match the provided string?
        //
        if(strcmp(pcToFind, psCGIParams[iLoop].pcParam) == 0)
        {
            //
            // We found a match - return the index.
            //
            return(iLoop);
        }
    }

    //
    // If we drop out, the parameter was not found.
    //
    return(-1);
}

//*****************************************************************************
//
// Determines whether a given character is a valid hexadecimal digit.
//
//*****************************************************************************
int
IsValidHexDigit(const char cDigit)
{
    if(((cDigit >= '0') && (cDigit <= '9')) ||
       ((cDigit >= 'a') && (cDigit <= 'f')) ||
       ((cDigit >= 'A') && (cDigit <= 'F')))
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

//*****************************************************************************
//
// Converts an ASCII character to a hex digit value.
//
//*****************************************************************************
unsigned char
HexDigit(const char cDigit)
{
	if((cDigit >= '0') && (cDigit <= '9'))
	{
		return (cDigit - '0');
	}
	else
	{
		if((cDigit >= 'a') && (cDigit <= 'f'))
		{
			return ((cDigit - 'a') + 10);
		}
		else
		{
			if((cDigit >= 'A') && (cDigit <= 'F'))
			{
				return ((cDigit - 'A') + 10);
			}
		}
	}

	//
	// If we get here, we were passed an invalid hex digit so return 0xFF.
	//
	return(0xFF);
}

//*****************************************************************************
//
// Decodes a single %xx escape sequence as an ASCII character.
//
//*****************************************************************************
int
DecodeHexEscape(const char *pcEncoded, char *pcDecoded)
{
    if((pcEncoded[0] != '%') || !IsValidHexDigit(pcEncoded[1]) ||
       !IsValidHexDigit(pcEncoded[2]))
    {
        return(false);
    }
    else
    {
        *pcDecoded = ((HexDigit(pcEncoded[1]) * 16) +
                      HexDigit(pcEncoded[2]));
        return(true);
    }
}

//*****************************************************************************
//
// Encodes a string for use within an HTML tag, escaping non alphanumeric
// characters.
//
//*****************************************************************************
unsigned long
EncodeFormString(const char *pcDecoded, char *pcEncoded,
                 unsigned long ulLen)
{
    unsigned long ulLoop;
    unsigned long ulCount;

    //
    // Make sure we were not passed a tiny buffer.
    //
    if(ulLen <= 1)
    {
        return(0);
    }

    //
    // Initialize our output character counter.
    //
    ulCount = 0;

    //
    // Step through each character of the input until we run out of data or
    // space to put our output in.
    //
    for(ulLoop = 0; pcDecoded[ulLoop] && (ulCount < (ulLen - 1)); ulLoop++)
    {
        switch(pcDecoded[ulLoop])
        {
            //
            // Pass most characters without modification.
            //
            default:
            {
                pcEncoded[ulCount++] = pcDecoded[ulLoop];
                break;
            }

            case '\'':
            {
                ulCount += usnprintf(&pcEncoded[ulCount], (ulLen - ulCount),
                                     "&#39;");
                break;
            }
        }
    }

    return(ulCount);
}

//*****************************************************************************
//
// Decodes a string encoded as part of an HTTP URI.
//
//*****************************************************************************
unsigned long
DecodeFormString(const  char *pcEncoded, char *pcDecoded,
                 unsigned long ulLen)
{
    unsigned long ulLoop;
    unsigned long ulCount;
    int bValid;

    ulCount = 0;
    ulLoop = 0;

    //
    // Keep going until we run out of input or fill the output buffer.
    //
    while(pcEncoded[ulLoop] && (ulCount < (ulLen - 1)))
    {
        switch(pcEncoded[ulLoop])
        {
            //
            // '+' in the encoded data is decoded as a space.
            //
            case '+':
            {
                pcDecoded[ulCount++] = ' ';
                ulLoop++;
                break;
            }

            //
            // '%' in the encoded data indicates that the following 2
            // characters indicate the hex ASCII code of the decoded character.
            //
            case '%':
            {
                if(pcEncoded[ulLoop + 1] && pcEncoded[ulLoop + 2])
                {
                    //
                    // Decode the escape sequence.
                    //
                    bValid = DecodeHexEscape(&pcEncoded[ulLoop],
                                             &pcDecoded[ulCount]);

                    //
                    // If the escape sequence was valid, move to the next
                    // output character.
                    //
                    if(bValid)
                    {
                        ulCount++;
                    }

                    //
                    // Skip past the escape sequence in the encoded string.
                    //
                    ulLoop += 3;
                }
                else
                {
                    //
                    // We reached the end of the string partway through an
                    // escape sequence so just ignore it and return the number
                    // of decoded characters found so far.
                    //
                    pcDecoded[ulCount] = '\0';
                    return(ulCount);
                }
                break;
            }

            //
            // For all other characters, just copy the input to the output.
            //
            default:
            {
                pcDecoded[ulCount++] = pcEncoded[ulLoop++];
                break;
            }
        }
    }

    //
    // Terminate the string and return the number of characters we decoded.
    //
    pcDecoded[ulCount] = '\0';
    return(ulCount);
}

//*****************************************************************************
//
// Ensures that a string passed represents a valid decimal number and,
// if so, converts that number to a long.
//
//*****************************************************************************
int
CheckDecimalParam(const char *pcValue, long *plValue)
{
    unsigned long ulLoop;
    int bStarted;
    int bFinished;
    int bNeg;
    long lAccum;

    if((pcValue == 0) || (*pcValue == 0))
    {
    	return(false);
    }

    //
    // Check that the string is a valid decimal number.
    //
    bStarted = false;
    bFinished = false;
    bNeg = false;
    ulLoop = 0;
    lAccum = 0;

    while(pcValue[ulLoop])
    {
        //
        // Ignore whitespace before the string.
        //
        if(!bStarted)
        {
            if((pcValue[ulLoop] == ' ') || (pcValue[ulLoop] == '\t'))
            {
                ulLoop++;
                continue;
            }

            //
            // Ignore a + or - character as long as we have not started.
            //
            if((pcValue[ulLoop] == '+') || (pcValue[ulLoop] == '-'))
            {
                //
                // If the string starts with a '-', remember to negate the
                // result.
                //
                bNeg = (pcValue[ulLoop] == '-') ? true : false;
                bStarted = true;
                ulLoop++;
            }
            else
            {
                //
                // We found something other than whitespace or a sign character
                // so we start looking for numerals now.
                //
                bStarted = true;
            }
        }

        if(bStarted)
        {
            if(!bFinished)
            {
                //
                // We expect to see nothing other than valid digit characters
                // here.
                //
                if((pcValue[ulLoop] >= '0') && (pcValue[ulLoop] <= '9'))
                {
                    lAccum = (lAccum * 10) + (pcValue[ulLoop] - '0');
                }
                else
                {
                    //
                    // Have we hit whitespace?  If so, check for no more
                    // characters until the end of the string.
                    //
                    if((pcValue[ulLoop] == ' ') || (pcValue[ulLoop] == '\t'))
                    {
                        bFinished = true;
                    }
                    else
                    {
                        //
                        // We got something other than a digit or whitespace so
                        // this makes the string invalid as a decimal number.
                        //
                        return(false);
                    }
                }
            }
            else
            {
                //
                // We are scanning for whitespace until the end of the string.
                //
                if((pcValue[ulLoop] != ' ') && (pcValue[ulLoop] != '\t'))
                {
                    //
                    // We found something other than whitespace so the string
                    // is not valid.
                    //
                    return(false);
                }
            }

            //
            // Move on to the next character in the string.
            //
            ulLoop++;
        }
    }

    //
    // If we drop out of the loop, the string must be valid.  All we need to do
    // now is negate the accumulated value if the string started with a '-'.
    //
    *plValue =  bNeg ? -lAccum : lAccum;
    return(true);
}


int
CheckFixedDecimalParam(const char *pcValue, long *plValue)
{
	unsigned long ulLoop;
	int bStarted;
	int bFinished;
	int bNeg;
	long lAccum;

	int bPointReached;
	long lDigitAfterPoint;

	if((pcValue == 0) || (*pcValue == 0))
    {
    	return(false);
    }

	//
	// Check that the string is a valid decimal number.
	//
	bStarted = false;
	bFinished = false;
	bNeg = false;
	ulLoop = 0;
	lAccum = 0;

	bPointReached    = false;
	lDigitAfterPoint = 0;

	while(pcValue[ulLoop])
	{
		//
		// Ignore whitespace before the string.
		//
		if(!bStarted)
		{
			if((pcValue[ulLoop] == ' ') || (pcValue[ulLoop] == '\t'))
			{
				ulLoop++;
				continue;
			}

			//
			// Ignore a + or - character as long as we have not started.
			//
			if((pcValue[ulLoop] == '+') || (pcValue[ulLoop] == '-'))
			{
				//
				// If the string starts with a '-', remember to negate the
				// result.
				//
				bNeg = (pcValue[ulLoop] == '-') ? true : false;
				bStarted = true;
				ulLoop++;
			}
			else
			{
				//
				// We found something other than whitespace or a sign character
				// so we start looking for numerals now.
				//
				bStarted = true;
			}
		}

		if(bStarted)
		{
			if(!bFinished)
			{
				//
				// We expect to see nothing other than valid digit characters
				// here.
				//


				if((pcValue[ulLoop] >= '0') && (pcValue[ulLoop] <= '9'))
				{
					if((lDigitAfterPoint < 1) || !bPointReached)
						lAccum = (lAccum * 10) + (pcValue[ulLoop] - '0');
					if(bPointReached == true)
						lDigitAfterPoint++;
				}
				else if((pcValue[ulLoop] == '.' || pcValue[ulLoop] == ',') && !bPointReached)
				{
					bPointReached = true;
				}
				else if(!bPointReached &&
						((pcValue[ulLoop] == '%') &&
						(pcValue[ulLoop+1] == '2') &&
						((pcValue[ulLoop+2] == 'C') ||
								(pcValue[ulLoop+2] == 'c'))))
				{
					ulLoop += 2;
					bPointReached = true;
				}
				else
				{
					//
					// Have we hit whitespace?  If so, check for no more
					// characters until the end of the string.
					//
					if((pcValue[ulLoop] == ' ') || (pcValue[ulLoop] == '\t'))
					{
						bFinished = true;
					}
					else
					{
						//
						// We got something other than a digit or whitespace so
						// this makes the string invalid as a decimal number.
						//
						return(false);
					}
				}
			}
			else
			{
				//
				// We are scanning for whitespace until the end of the string.
				//
				if((pcValue[ulLoop] != ' ') && (pcValue[ulLoop] != '\t'))
				{
					//
					// We found something other than whitespace so the string
					// is not valid.
					//
					return(false);
				}
			}

			//
			// Move on to the next character in the string.
			//
			ulLoop++;
		}
	}

	if(!lDigitAfterPoint)
		lAccum *= 10;
	//
	// If we drop out of the loop, the string must be valid.  All we need to do
	// now is negate the accumulated value if the string started with a '-'.
	//
	*plValue =  bNeg ? -lAccum : lAccum;
	return(true);
}


int
CheckIPParam(const char *pcValue, long *plValue)
{
	unsigned long ulLoop;
	int bStarted;
	int bFinished;

	int   point_cnt;
	long  group_acc;
	long  lAccum;

	if((pcValue == 0) || (*pcValue == 0))
    {
    	return(false);
    }

	//
	// Check that the string is a valid decimal number.
	//
	ulLoop    = 0;
	bStarted  = false;
	bFinished = false;

	point_cnt = 0;
	group_acc = 0;
	lAccum    = 0;

	while(pcValue[ulLoop])
	{
		//
		// Ignore whitespace before the string.
		//
		if(!bStarted)
		{
			if((pcValue[ulLoop] == ' ') || (pcValue[ulLoop] == '\t'))
			{
				ulLoop++;
				continue;
			}
			//
			// We found something other than whitespace or a sign character
			// so we start looking for numerals now.
			//
			bStarted  = true;
		}

		if(bStarted)
		{
			if(!bFinished)
			{
				//
				// We expect to see nothing other than valid digit characters
				// here.
				//
				if((pcValue[ulLoop] >= '0') && (pcValue[ulLoop] <= '9'))
				{
					group_acc = (group_acc * 10) + (pcValue[ulLoop] - '0');
				}
				else if((pcValue[ulLoop] == '.'))
				{
					if((group_acc > 255) || (point_cnt > 3))
						return (false);
					lAccum |= group_acc << (8*(3-point_cnt));
					point_cnt += 1;
					group_acc  = 0;
				}
				else
				{
					//
					// Have we hit whitespace?  If so, check for no more
					// characters until the end of the string.
					//
					if((pcValue[ulLoop] == ' ') || (pcValue[ulLoop] == '\t'))
					{
						bFinished = true;
					}
					else
					{
						//
						// We got something other than a digit or whitespace so
						// this makes the string invalid as a decimal number.
						//
						return (false);
					}
				}
			}
			else
			{
				//
				// We are scanning for whitespace until the end of the string.
				//
				if((pcValue[ulLoop] != ' ') && (pcValue[ulLoop] != '\t'))
				{
					//
					// We found something other than whitespace so the string
					// is not valid.
					//
					return (false);
				}
			}

			//
			// Move on to the next character in the string.
			//
			ulLoop++;
		}
	}

	if(point_cnt != 3 || (group_acc > 255))
		return (false);

	lAccum |= group_acc & 0xff;

	//
	// If we drop out of the loop, the string must be valid.  All we need to do
	// now is negate the accumulated value if the string started with a '-'.
	//
	*plValue =  lAccum;
	return (true);
}


int
CheckMacParam(const char *pcValue, unsigned char *pucMacAddr)
{
	unsigned long ulLoop;
	int bStarted;
	int bFinished;

	int           value;
	unsigned char group_digit_cnt;
	unsigned char group_index;

    if((pcValue == 0) || (*pcValue == 0))
    {
    	return(false);
    }

	//
	// Check that the string is a valid decimal number.
	//
	ulLoop    = 0;
	bStarted  = false;
	bFinished = false;

	value = 0;
	group_index = 0;
	group_digit_cnt = 0;

	for(int i = 0; i < 6; i++)
		pucMacAddr[i] = 0;


	while(pcValue[ulLoop])
	{
		//
		// Ignore whitespace before the string.
		//
		if(!bStarted)
		{
			if((pcValue[ulLoop] == ' ') || (pcValue[ulLoop] == '\t'))
			{
				ulLoop++;
				continue;
			}
			//
			// We found something other than whitespace or a sign character
			// so we start looking for numerals now.
			//
			bStarted  = true;
		}

		if(bStarted)
		{
			if(!bFinished)
			{
				//
				// We expect to see nothing other than valid digit characters
				// here.
				//
				if(IsValidHexDigit(pcValue[ulLoop]))
				{
					if(++group_digit_cnt > 2)
						return (false);

					value = HexDigit(pcValue[ulLoop]);

					if(group_digit_cnt == 1)
						pucMacAddr[group_index]  = value * 16;
					else
						pucMacAddr[group_index] += value;
				}
				else if(pcValue[ulLoop] == ':')
				{
					if(group_digit_cnt != 2)
						return (false);

					if(++group_index > 5)
						return (false);

					group_digit_cnt = 0;
				}
				else
				{
					//
					// Have we hit whitespace?  If so, check for no more
					// characters until the end of the string.
					//
					if((pcValue[ulLoop] == ' ') || (pcValue[ulLoop] == '\t'))
					{
						bFinished = true;
					}
					else
					{
						//
						// We got something other than a digit or whitespace so
						// this makes the string invalid as a decimal number.
						//
						return (false);
					}
				}
			}
			else
			{
				//
				// We are scanning for whitespace until the end of the string.
				//
				if((pcValue[ulLoop] != ' ') && (pcValue[ulLoop] != '\t'))
				{
					//
					// We found something other than whitespace so the string
					// is not valid.
					//
					return (false);
				}
			}

			//
			// Move on to the next character in the string.
			//
			ulLoop++;
		}
	}

	if(group_index < 5)
		return (false);

	// If we drop out of the loop, the string must be valid.  All we need to do
	// now is negate the accumulated value if the string started with a '-'.
	//

	return (true);
}

//*****************************************************************************
//
// Searches the list of parameters passed to a CGI handler for a parameter
// with the given name and, if found, reads the parameter value as a decimal
// number.
//
//*****************************************************************************
int
GetCGIParam(const char *pcName, struct CGIParam *psCGIParams, int iParamCount, long *plValue)
{
	int iParam;

	//
	// Is the parameter we are looking for in the list?
	//
	iParam = FindCGIParameter(pcName, psCGIParams, iParamCount);

	if(iParam != -1)
	{
		//
		// We found the parameter so now get its value.
		//
		if(CheckDecimalParam(psCGIParams[iParam].pcValue, plValue))
		{
			//
			// All is well - return the parameter value.
			//
			return(1);
		}
	}

	//
	// If we reach here, there was a problem so return 0 and set the error
	// flag.
	//
	return(0);
}

int
GetFixedDecimalCGIParam(const char *pcName, struct CGIParam *psCGIParams, int iParamCount, long *plValue)
{
	int iParam;

	//
	// Is the parameter we are looking for in the list?
	//
	iParam = FindCGIParameter(pcName, psCGIParams, iParamCount);

	if(iParam != -1)
	{
		//
		// We found the parameter so now get its value.
		//
		if(CheckFixedDecimalParam(psCGIParams[iParam].pcValue, plValue))
		{
			//
			// All is well - return the parameter value.
			//
			return(1);
		}
	}

	//
	// If we reach here, there was a problem so return 0 and set the error
	// flag.
	//
	return(0);
}


int
GetIPCGIParam(const char *pcName, struct CGIParam *psCGIParams, int iParamCount, long *plValue)
{
    int iParam;

    //
    // Is the parameter we are looking for in the list?
    //
    iParam = FindCGIParameter(pcName, psCGIParams, iParamCount);

    if(iParam != -1)
    {
        //
        // We found the parameter so now get its value.
        //
        if(CheckIPParam(psCGIParams[iParam].pcValue, plValue))
        {
            //
            // All is well - return the parameter value.
            //
            return(1);
        }
    }

    //
    // If we reach here, there was a problem so return 0 and set the error
    // flag.
    //
    return(0);
}


int
GetMacCGIParam(const char *pcName, struct CGIParam *psCGIParams, int iParamCount, unsigned char *pucMacAddr)
{
	int iParam;

	//
	// Is the parameter we are looking for in the list?
	//
	iParam = FindCGIParameter(pcName, psCGIParams, iParamCount);

	if(iParam != -1)
	{
		//
		// We found the parameter so now get its value.
		//
		if(CheckMacParam(psCGIParams[iParam].pcValue, pucMacAddr))
		{
			//
			// All is well - return the parameter value.
			//
			return (1);
		}
	}

	return(0);
}

