#include "RSDK/Core/RetroEngine.hpp"

using namespace RSDK;

#if RETRO_REV0U
#include "Legacy/TextLegacy.cpp"
#endif

// From here: https://rosettacode.org/wiki/MD5#C

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/*
    My Stuff
*/

void RSDK::__gen_hash_md5(const char *name, void* val)
{
    uint32* hsh = (uint32*)val;
    strcpy(textBuffer, name);
    GenerateHashMD5(hsh, textBuffer, (int32)strlen(textBuffer));
}

/*
*/

typedef union uwb {
    unsigned w;
    unsigned char b[4];
} WBunion;

typedef unsigned digest[4];

unsigned f0(unsigned abcd[]) { return (abcd[1] & abcd[2]) | (~abcd[1] & abcd[3]); }

unsigned f1(unsigned abcd[]) { return (abcd[3] & abcd[1]) | (~abcd[3] & abcd[2]); }

unsigned f2(unsigned abcd[]) { return abcd[1] ^ abcd[2] ^ abcd[3]; }

unsigned f3(unsigned abcd[]) { return abcd[2] ^ (abcd[1] | ~abcd[3]); }

typedef unsigned (*DgstFctn)(unsigned a[]);

unsigned *calcKs(unsigned *k)
{
    double s, pwr;
    int32 i;

    pwr = pow(2, 32);
    for (i = 0; i < 64; i++) {
        s    = fabs(sin(1 + i));
        k[i] = (unsigned)(s * pwr);
    }
    return k;
}

unsigned kspace[64];
unsigned *k = calcKs(kspace);

// ROtate v Left by amt bits
unsigned rol(unsigned v, int16 amt)
{
    unsigned msk1 = (1 << amt) - 1;
    return ((v >> (32 - amt)) & msk1) | ((v << amt) & ~msk1);
}

unsigned *md5(unsigned *h, const char *msg, int32 mlen)
{
    static digest h0     = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476 };
    static DgstFctn ff[] = { &f0, &f1, &f2, &f3 };
    static int16 M[]     = { 1, 5, 3, 7 };
    static int16 O[]     = { 0, 1, 5, 0 };
    static int16 rot0[]  = { 7, 12, 17, 22 };
    static int16 rot1[]  = { 5, 9, 14, 20 };
    static int16 rot2[]  = { 4, 11, 16, 23 };
    static int16 rot3[]  = { 6, 10, 15, 21 };
    static int16 *rots[] = { rot0, rot1, rot2, rot3 };

    digest abcd;
    DgstFctn fctn;
    int16 m, o, g;
    unsigned f;
    int16 *rotn;
    union {
        unsigned w[16];
        char b[64];
    } mm;
    int32 os = 0;
    int32 grp, grps, q, p;
    unsigned char *msg2;

    if (k == NULL)
        k = calcKs(kspace);

    for (q = 0; q < 4; q++) h[q] = h0[q]; // initialize

    {
        grps = 1 + (mlen + 8) / 64;
        msg2 = (unsigned char *)malloc(64 * grps);
        memcpy(msg2, msg, mlen);
        msg2[mlen] = (unsigned char)0x80;
        q          = mlen + 1;
        while (q < 64 * grps) {
            msg2[q] = 0;
            q++;
        }
        {
            //            unsigned char t;
            WBunion u;
            u.w = 8 * mlen;
            //            t = u.b[0]; u.b[0] = u.b[3]; u.b[3] = t;
            //            t = u.b[1]; u.b[1] = u.b[2]; u.b[2] = t;
            q -= 8;
#if !RETRO_USE_ORIGINAL_CODE
            for (p = 0; p < 4; ++p) msg2[q + p] = (u.w >> (8 * p)) & 0xFF;
#else
            // This only works as intended on little-endian CPUs.
            memcpy(msg2 + q, &u.w, 4);
#endif
        }
    }

    for (grp = 0; grp < grps; grp++) {
#if !RETRO_USE_ORIGINAL_CODE
        memset(&mm, 0, sizeof(mm));
        for (p = 0; p < 64; ++p) mm.w[p / 4] |= msg2[os + p] << (8 * (p % 4));
#else
        // This only works as intended on little-endian CPUs.
        memcpy(mm.b, msg2 + os, 64);
#endif
        for (q = 0; q < 4; q++) abcd[q] = h[q];
        for (p = 0; p < 4; p++) {
            fctn = ff[p];
            rotn = rots[p];
            m    = M[p];
            o    = O[p];
            for (q = 0; q < 16; q++) {
                g = (m * q + o) % 16;
                f = abcd[1] + rol(abcd[0] + fctn(abcd) + k[q + 16 * p] + mm.w[g], rotn[q % 4]);

                abcd[0] = abcd[3];
                abcd[3] = abcd[2];
                abcd[2] = abcd[1];
                abcd[1] = f;
            }
        }
        for (p = 0; p < 4; p++) h[p] += abcd[p];
        os += 64;
    }

    if (msg2)
        free(msg2);

    return h;
}

char RSDK::textBuffer[0x400];
// Buffer is expected to be at least 16 bytes long
void RSDK::GenerateHashMD5(uint32 *buffer, char *textBuffer, int32 textBufferLen)
{
    digest h; // storage var
    uint8 *buf  = (uint8 *)buffer;
    unsigned *d = md5(h, textBuffer, textBufferLen);
    WBunion u;

    for (int32 i = 0; i < 4; ++i) {
        u.w = d[i];
        for (int32 c = 0; c < 4; ++c) buf[(i << 2) + c] = u.b[c];
    }
}

uint8_t crc_reflect_u8(const uint8_t u8)
{
    uint8_t result = 0;
    
    for(uint8_t i = 0; i != 8; ++i)
    {
        uint8_t bit = (u8>>i)&1;
        result |= (bit<<(7-i));
    }

    return result;
}

uint32_t crc_reflect_u32(const uint32_t u32)
{
    uint32_t result = 0;
    
    for(uint8_t i = 0; i != 32; ++i)
    {
        uint32_t bit = (u32>>i)&1;
        result |= (bit<<(31-i));
    }
    
    return result;
}

void RSDK::GenerateHashCRC(uint32 *id, char *inputString)
{
    //*id = -1;
    //if (!inputString)
    //    return;
    //
    //int32 len = 0;
    //do ++len;
    //while (inputString[len]);
    //
    //for (uint32 i = 0; i < len; i++) {
    //    *id = crc32_t[(uint8)(*id ^ *inputString)] ^ (*id >> 8);
    //    inputString++;
    //}
    //*id = ~*id;
    
    (*id) = 0xFFFFFFFF;
    const uint32_t size = strlen(inputString);
    for(uint32_t i = 0; i < size; ++i)
    {
        char c = inputString[i];
        c = crc_reflect_u8(c);
        
        for(uint8_t j = 0; j < 8; ++j)
        {
            const uint32_t bit = (c >> (7-j)) & 1;
            const uint32_t msb = ((*id) >> 31) & 1;
            (*id) <<= 1;
            
            if(bit ^ msb)
                (*id) ^= 0x04C11DB7;
        }
    }

    (*id) = crc_reflect_u32((*id));
    (*id) ^= 0xFFFFFFFF;
}

uint8 utf8CharSizes[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                          1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                          2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6 };

void RSDK::SetString(String *string, const char *text)
{
    if (!*text)
        return;

    int32 newLength = 0;
    for (int32 c = 0; text[c]; ++newLength) c += utf8CharSizes[*text & 0xFF];

    if (!newLength)
        return;

    if (string->size < newLength || !string->chars) {
        string->size = newLength;
        AllocateStorage((void **)&string->chars, sizeof(uint16) * string->size, DATASET_STR, false);
    }

    string->length = newLength;
    for (int32 pos = 0; pos < string->length; ++pos) {
        uint16 c = 0;
        switch (utf8CharSizes[*text & 0xFF]) {
            default: break;

            case 1:
                c = text[0];
                ++text;
                break;

            case 2:
                c = (text[1] & 0x3F) | ((text[0] & 0x1F) << 6);
                text += 2;
                break;

            case 3:
                c = (text[2] & 0x3F) | ((text[1] & 0x3F) << 6) | (text[0] << 12);
                text += 3;
                break;

            case 4:
                c = (text[3] & 0x3F) | ((text[2] & 0x3F) << 6) | (text[1] << 12);
                text += 4;
                break;

            case 5: text += 5; break;

            case 6: text += 6; break;
        }

        string->chars[pos] = c;
    }
}

void RSDK::AppendText(String *string, const char *appendString)
{
    if (!*appendString)
        return;

    int32 len     = 0;
    const char *textBuf = appendString;
    int32 pos;
    for (pos = 0; *textBuf; ++len) pos += utf8CharSizes[*textBuf++ & 0xFF];
    (void)pos;

    if (!len)
        return;

    int32 newSize = len + string->size;
    if (string->size < newSize || !string->chars) {
        if (string->chars) {
            uint16 *charsStore = string->chars;
            AllocateStorage((void **)&string->chars, sizeof(uint16) * newSize, DATASET_STR, false);

            for (int32 c = 0; c < string->length; ++c) string->chars[c] = charsStore[c];
            charsStore = NULL;
        }
        else {
            AllocateStorage((void **)&string->chars, sizeof(uint16) * newSize, DATASET_STR, false);
        }

        string->size = newSize;
    }

    for (int32 c = string->length; c < string->length + len; ++c) {
        uint16 curChar = 0;
        switch (utf8CharSizes[*appendString & 0xFF]) {
            default: break;

            case 1:
                curChar = appendString[0];
                ++appendString;
                break;

            case 2:
                curChar = (appendString[1] & 0x3F) | ((appendString[0] & 0x1F) << 6);
                appendString += 2;
                break;

            case 3:
                curChar = (appendString[2] & 0x3F) | ((appendString[1] & 0x3F) << 6) | (appendString[0] << 12);
                appendString += 3;
                break;

            case 4:
                curChar = (appendString[3] & 0x3F) | ((appendString[2] & 0x3F) << 6) | (appendString[1] << 12);
                appendString += 4;
                break;

            case 5: appendString += 5; break;

            case 6: appendString += 6; break;
        }

        string->chars[c] = curChar;
    }

    string->length = newSize;
}

void RSDK::AppendString(String *string, String *appendString)
{
    uint32 newSize = appendString->length + string->length;

    if (string->size < newSize || !string->chars) {
        if (string->chars) {
            uint16 *charsStore = string->chars;
            AllocateStorage((void **)&string->chars, sizeof(uint16) * newSize, DATASET_STR, false);

            for (int32 c = 0; c < string->length; ++c) string->chars[c] = charsStore[c];
            charsStore = NULL;
        }
        else {
            AllocateStorage((void **)&string->chars, sizeof(uint16) * newSize, DATASET_STR, false);
        }

        string->size = newSize;
    }

    int32 startOffset = string->length;
    string->length += appendString->length;
    for (int32 c = 0, pos = startOffset; pos < string->length; ++pos, ++c) string->chars[pos] = appendString->chars[c];
}

bool32 RSDK::CompareStrings(String *string1, String *string2, bool32 exactMatch)
{
    if (string1->length != string2->length)
        return false;

    if (exactMatch) { // each character has to match
        for (int32 i = 0; i < string1->length; ++i) {
            if (string1->chars[i] != string2->chars[i])
                return false;
        }
    }
    else { // ignore case sensitivity when matching
        if (string1->length <= 0)
            return true;

        for (int32 i = 0; i < string1->length; ++i) {
            if (string1->chars[i] != string2->chars[i]) {
                if (string1->chars[i] != (string2->chars[i] + 0x20) && string1->chars[i] != (string2->chars[i] - 0x20))
                    return false;
            }
        }
    }

    return true;
}

void RSDK::InitStringList(String *stringList, int32 size)
{
    uint16 *text = NULL;

    AllocateStorage((void **)&text, sizeof(uint16) * size, DATASET_STR, false);

    for (int32 c = 0; c < size && c < stringList->length; ++c) text[c] = stringList->chars[c];

    CopyStorage((uint32 **)&stringList->chars, (uint32 **)&text);
    stringList->size = size;
    if (stringList->length > (uint16)size)
        stringList->length = size;
}

void RSDK::LoadStringList(String *stringList, const char *filePath, uint32 charSize)
{
    char fullFilePath[0x40];
    sprintf_s(fullFilePath, sizeof(fullFilePath), "Data/Strings/%s", filePath);

    FileInfo info;
    InitFileInfo(&info);
    if (LoadFile(&info, fullFilePath, FMODE_RB)) {
#if RETRO_REV02
        uint16 header = ReadInt16(&info);
        if (header == 0xFEFF) {
            // UTF-16
            InitStringList(stringList, (info.fileSize >> 1) - 1);
#if !RETRO_USE_ORIGINAL_CODE
            for (int32 c = 0; c < stringList->size; ++c) stringList->chars[c] = ReadInt16(&info);
#else
            // This only works as intended on little-endian CPUs.
            ReadBytes(&info, stringList->chars, stringList->size * sizeof(uint16));
#endif
            stringList->length = stringList->size;
        }
        else {
            // UTF-8
            if (header == 0xEFBB)
                ReadInt8(&info);
            else
                Seek_Set(&info, 0);

            InitStringList(stringList, info.fileSize);

            for (int32 pos = 0; pos < info.fileSize; ++pos) {
                int32 curChar = 0;

                uint8 bit = ReadInt8(&info);
                switch (utf8CharSizes[bit]) {
                    case 1: curChar = bit; break;
                    case 2:
                        curChar = ((bit & 0x1F) << 6);
                        curChar |= (ReadInt8(&info) & 0x3F);
                        break;

                    case 3:
                        curChar = (bit << 12);
                        curChar |= ((ReadInt8(&info) & 0x3F) << 6);
                        curChar |= ReadInt8(&info) & 0x3F;
                        break;

                    case 4:
                        curChar = ReadInt8(&info) << 12;
                        curChar |= ((ReadInt8(&info) & 0x3F) << 6);
                        curChar |= ReadInt8(&info) & 0x3F;
                        break;

                    case 5:
                        pos += 4;
                        Seek_Cur(&info, 4);
                        break;

                    case 6:
                        pos += 5;
                        Seek_Cur(&info, 5);
                        break;

                    default: break;
                }

                stringList->chars[stringList->length++] = curChar;
            }
        }
#else
        switch (charSize) {
            default:
            case 8: // ASCII
                if (stringList->size < info.fileSize) {
                    stringList->size = info.fileSize;
                    AllocateStorage((void **)&stringList->chars, sizeof(uint16) * stringList->size, DATASET_STR, false);
                }
                stringList->length = info.fileSize;
                InitStringList(stringList, info.fileSize);

                for (int32 c = 0; c < stringList->length; ++c) stringList->chars[c] = ReadInt8(&info);
                break;

            case 16: // UTF-16
                if (stringList->size < info.fileSize) {
                    stringList->size = info.fileSize >> 1;
                    AllocateStorage((void **)&stringList->chars, sizeof(uint16) * stringList->size, DATASET_STR, false);
                }
                stringList->length = info.fileSize >> 1;
                InitStringList(stringList, info.fileSize >> 1);

                for (int32 c = 0; c < stringList->length; ++c) stringList->chars[c] = ReadInt16(&info);
                break;
        }
#endif

        CloseFile(&info);
    }
}

bool32 RSDK::SplitStringList(String *splitStrings, String *stringList, int32 startStringID, int32 stringCount)
{
    if (!stringList->size || !stringList->chars)
        return false;

    int32 lastCharPos = 0;
    int32 curStringID = 0;

    bool32 hasSplitString = false;
    for (int32 curCharPos = 0; curCharPos < stringList->length && stringCount > 0; ++curCharPos) {
        if (stringList->chars[curCharPos] == '\n') {
            if (curStringID < startStringID) {
                lastCharPos = curCharPos;
            }
            else {
                uint16 length = curCharPos - lastCharPos;
                if (splitStrings->size < length) {
                    splitStrings->size = length;
                    AllocateStorage((void **)&splitStrings->chars, sizeof(uint16) * length, DATASET_STR, true);
                }
                splitStrings->length = length;

                for (int32 i = 0; i < splitStrings->length; ++i) splitStrings->chars[i] = stringList->chars[lastCharPos++];

                ++splitStrings;
                --stringCount;
                hasSplitString = true;
            }

            ++curStringID;
            ++lastCharPos;
        }
    }

    return hasSplitString;
}

#if RETRO_REV0U
int32 RSDK::FindStringToken(const char *string, const char *token, uint8 stopID)
{
    int32 tokenCharID  = 0;
    bool32 tokenMatch  = true;
    int32 stringCharID = 0;
    int32 foundTokenID = 0;

    while (string[stringCharID]) {
        tokenCharID = 0;
        tokenMatch  = true;
        while (token[tokenCharID]) {
            if (!string[tokenCharID + stringCharID])
                return -1;

            if (string[tokenCharID + stringCharID] != token[tokenCharID])
                tokenMatch = false;

            ++tokenCharID;
        }
        if (tokenMatch && ++foundTokenID == stopID)
            return stringCharID;

        ++stringCharID;
    }
    return -1;
}
#endif