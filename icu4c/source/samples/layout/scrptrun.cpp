/*
 *******************************************************************************
 *
 *   Copyright (C) 1999-2001, International Business Machines
 *   Corporation and others.  All Rights Reserved.
 *
 *******************************************************************************
 *   file name:  scrptrun.cpp
 *
 *   created on: 10/17/2001
 *   created by: Eric R. Mader
 */

#include "unicode/utypes.h"
#include "unicode/uscript.h"

#include "scrptrun.h"

UBool ScriptRun::sameScript(int32_t scriptOne, int32_t scriptTwo)
{
    return scriptOne <= USCRIPT_INHERITED || scriptTwo <= USCRIPT_INHERITED || scriptOne == scriptTwo;
}

UBool ScriptRun::next()
{
    UErrorCode error = U_ZERO_ERROR;

    if (scriptEnd >= charLimit) {
        return false;
    }
    
    scriptCode = USCRIPT_COMMON;

    for (scriptStart = scriptEnd; scriptEnd < charLimit; scriptEnd += 1) {
        UChar   high = charArray[scriptEnd];
        UChar32 ch   = high;

        if (scriptEnd < charLimit - 1 && high >= 0xD800 && high <= 0xDBFF)
        {
            UChar low = charArray[scriptEnd + 1];

            if (low >= 0xDC00 && low <= 0xDFFF) {
                ch = (high - 0xD800) * 0x0400 + low - 0xDC00 + 0x10000;
                scriptEnd += 1;
            }
        }

        UScriptCode sc = uscript_getScript(ch, &error);

        if (sameScript(scriptCode, sc)) {
            if (scriptCode <= USCRIPT_INHERITED && sc > USCRIPT_INHERITED) {
                scriptCode = sc;
            }
        } else {
            // if the run broke on a surrogate pair,
            // end it before the high surrogate
            if (ch >= 0x10000) {
                scriptEnd -= 1;
            }

            break;
        }
    }

    return true;
}

