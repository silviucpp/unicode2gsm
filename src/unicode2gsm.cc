
// https://github.com/minutis-redcall/app/blob/fe57f22f5d3fad42e5968aca4b8fe6ad0ef98c77/symfony/src/Tools/GSM.php
// https://github.com/BenMorel/GsmCharsetConverter/blob/master/src/Charset.php
// https://unicode-table.com/en/
// https://www.ssec.wisc.edu/~tomw/java/unicode.html

#include "unicode2gsm/unicode2gsm.h"
#include "unicode2gsm/charset.h"

namespace unicode2gsm{

namespace {

// lookup tables

const char* kTransliterationLookupTable[kMaxCodepointValue] = {nullptr};
bool kGsmLookupTable[kMaxCodepointValue] = {false};

// helper functions

inline uint32_t do_transliteration(const char* utf8, std::string& output)
{
    uint32_t step_size = GetCodePointSize(*utf8);
    unicode_char codepoint = GetCodePoint(utf8, step_size);

    const char* replacement = nullptr;

    if(codepoint < kMaxCodepointValue)
        replacement = kTransliterationLookupTable[codepoint];

    if(replacement != nullptr)
    {
        while (*replacement != 0)
        {
            output.push_back(*replacement);
            replacement++;
        }
    }
    else
    {
        // unsupported unicode symbol or symbol not found into our replacement table. copy back as it is
        output.append(utf8, step_size);
    }

    return step_size;
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
    return init_lookup_tables(transliterate_gsm_extended, kTransliterationLookupTable, kGsmLookupTable, kMaxCodepointValue);
}

bool requires_transliteration(const char* utf8)
{
    uint32_t step_size;

    while (*utf8)
    {
        if(!next_codepoint_is_gsm7(utf8, step_size))
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

        utf8+=step_size;
        length-=step_size;
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


