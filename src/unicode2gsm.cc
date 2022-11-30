
// https://github.com/minutis-redcall/app/blob/fe57f22f5d3fad42e5968aca4b8fe6ad0ef98c77/symfony/src/Tools/GSM.php
// https://github.com/BenMorel/GsmCharsetConverter/blob/master/src/Charset.php
// https://unicode-table.com/en/
// https://www.ssec.wisc.edu/~tomw/java/unicode.html

#include "unicode2gsm/unicode2gsm.h"
#include "unicode2gsm/charset.h"

#include <string.h>

namespace unicode2gsm{

namespace {

// lookup tables

const char* kTransliterationLookupTable[kMaxCodepointValue] = {nullptr};
size_t kTransliterationLookupSizeTable[kMaxCodepointValue] = {0};

bool kGsmLookupTable[kMaxCodepointValue] = {false};

// helper functions

inline uint32_t do_transliteration(const char* utf8, std::string& output)
{
    uint32_t codepoint_size = GetCodePointSize(*utf8);
    unicode_char codepoint = GetCodePoint(utf8, codepoint_size);

    const char* replacement = nullptr;

    //detect \r\n and replace with \n
    if(codepoint == '\n' && output.empty() == false && output.back() == '\r')
    {
        output[output.size() - 1] = '\n';
    }
    else
    {
        if(codepoint < kMaxCodepointValue)
            replacement = kTransliterationLookupTable[codepoint];

        if(replacement != nullptr)
        {
            size_t size = kTransliterationLookupSizeTable[codepoint];

            if(size > 0)
                output.append(replacement, size);
        }
        else
        {
            // unsupported unicode symbol or symbol not found into our replacement table. copy back as it is
            output.append(utf8, codepoint_size);
        }
    }

    return codepoint_size;
}

inline bool next_codepoint_is_gsm7(const char* utf8, uint32_t & codepoint_size)
{
    codepoint_size = GetCodePointSize(*utf8);

    unicode_char codepoint = GetCodePoint(utf8, codepoint_size);

    if(codepoint >= kMaxCodepointValue || !kGsmLookupTable[codepoint])
        return false;

    return true;
}

}

bool init_transliteration_map(bool transliterate_gsm_extended)
{
    memset(kTransliterationLookupSizeTable, 0, sizeof(kTransliterationLookupSizeTable));

    if(!init_lookup_tables(transliterate_gsm_extended, kTransliterationLookupTable, kGsmLookupTable, kMaxCodepointValue))
        return false;

    for(unicode_char i = 0; i < kMaxCodepointValue; i++)
    {
        const char* replacement = kTransliterationLookupTable[i];

        if(replacement != nullptr)
        {
            uint32_t size = 0;

            while (*replacement != 0)
            {
                size_t codepoint_size = GetCodePointSize(*replacement);
                size+=codepoint_size;
                replacement+=codepoint_size;
            }

            kTransliterationLookupSizeTable[i] = size;
        }
    }

    return true;
}

bool requires_transliteration(const char* utf8)
{
    uint32_t step_size;

    while (*utf8)
    {
        if(!next_codepoint_is_gsm7(utf8, step_size))
            return true;

        if(*utf8 == '\r' && *(utf8+step_size) == '\n')
            return true;

        utf8+=step_size;
    }

    return false;
}

bool requires_transliteration(const char* utf8, size_t length)
{
    uint32_t step_size;

    while (length > 0)
    {
        if(!next_codepoint_is_gsm7(utf8, step_size))
            return true;

        length-=step_size;

        if(*utf8 == '\r' && length > 0 && utf8[1] == '\n')
            return true;

        utf8+=step_size;
    }

    return false;
}

std::string transliterate(const char* utf8)
{
    std::string output;

    while (*utf8)
        utf8 += do_transliteration(utf8, output);

    return output;
}

std::string transliterate(const char* utf8, size_t length)
{
    std::string output;
    output.reserve(length);

    while (length > 0)
    {
        uint32_t step_size = do_transliteration(utf8, output);
        utf8+=step_size;
        length-=step_size;
    }

    return output;
}

}


