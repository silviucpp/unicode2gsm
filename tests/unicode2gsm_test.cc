#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#define BOOST_TEST_MODULE unicode2gsm_test

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <regex>

#include "unicode2gsm/unicode2gsm.h"
#include "unicode2gsm/common.h"
#include "unicode2gsm/charset.h"

const char *kGsmAlphabet=u8"@£$¥èéùìòÇØøÅåΔ_ΦΓΛΩΠΨΣΘΞÆæßÉ!\"#¤%&'()*+,-./0123456789:;<=>?¡ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÑÜ§¿abcdefghijklmnopqrstuvwxyzäöñüà^{}\[~]|€";

std::string replace(const std::string& original, const std::string& find, const std::string& replace)
{
    return std::regex_replace(original, std::regex(find), replace);
}

BOOST_AUTO_TEST_SUITE(unicode2gsm_test)

BOOST_AUTO_TEST_CASE(gsm_alphabet_no_extended_transliteration) 
{
    BOOST_CHECK_EQUAL(unicode2gsm::init_transliteration_map(false), true);
    BOOST_CHECK_EQUAL(unicode2gsm::requires_transliteration(kGsmAlphabet), false);
    BOOST_CHECK_EQUAL(unicode2gsm::transliterate(kGsmAlphabet), std::string(kGsmAlphabet));
}

BOOST_AUTO_TEST_CASE(gsm_alphabet_with_extended_transliteration) 
{
    const std::string kGsmAlphabetTransliterated=u8"@£$¥èéùìòÇØøÅåΔ_ΦΓΛΩΠΨΣΘΞÆæßÉ!\"#¤%&'()*+,-./0123456789:;<=>?¡ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÑÜ§¿abcdefghijklmnopqrstuvwxyzäöñüà^()\(-)|€";

    BOOST_CHECK_EQUAL(unicode2gsm::init_transliteration_map(true), true);
    BOOST_CHECK_EQUAL(unicode2gsm::requires_transliteration(kGsmAlphabet), true);
    BOOST_CHECK_EQUAL(unicode2gsm::transliterate(kGsmAlphabet), kGsmAlphabetTransliterated);
}

BOOST_AUTO_TEST_CASE(transliterate) 
{
    const std::string kInString=u8"［ ］ ˜ 〜";
    const std::string kOutput1=u8"[ ] ~ ~";
    const std::string kOutput2=u8"( ) - -";

    BOOST_CHECK_EQUAL(unicode2gsm::init_transliteration_map(true), true);
    BOOST_CHECK_EQUAL(unicode2gsm::transliterate(kInString.c_str()), kOutput2);

    BOOST_CHECK_EQUAL(unicode2gsm::init_transliteration_map(false), true);
    BOOST_CHECK_EQUAL(unicode2gsm::transliterate(kInString.c_str()), kOutput1);
}

BOOST_AUTO_TEST_CASE(check_transliteration_lookup_table) 
{
    const char* lookup_table[unicode2gsm::kMaxCodepointValue] = {nullptr};
    bool gsm_lookup_table[unicode2gsm::kMaxCodepointValue] = {false};

    BOOST_CHECK_EQUAL(unicode2gsm::init_lookup_tables(false, lookup_table, gsm_lookup_table, unicode2gsm::kMaxCodepointValue), true);

    for(unicode2gsm::unicode_char i = 0; i < unicode2gsm::kMaxCodepointValue; i++)
    {
        const char* replacement = lookup_table[i];

        if(replacement == nullptr)
            continue;

        while (*replacement)
        {
            uint32_t step_size = unicode2gsm::GetCodePointSize(*replacement);
            unicode2gsm::unicode_char codepoint = unicode2gsm::GetCodePoint(replacement, step_size);

            if(!gsm_lookup_table[codepoint])
                BOOST_FAIL(std::string("not valid gsm replacement: ")+lookup_table[i]);

            replacement+=step_size;
        }
    }
}

BOOST_AUTO_TEST_CASE(new_line_and_tabs_optimizations)
{
    BOOST_CHECK_EQUAL(unicode2gsm::init_transliteration_map(false), true);

    const std::string str_1 = "string\rstring2";
    const std::string str_2 = "string\nstring2";
    const std::string str_3 = "string\r\nstring2";
    const std::string str_4 = "string\r\n";
    const std::string str_5 = "string\nstring2\rstring3\r\nstring4\fstring5\tstring6";
    const std::string str_6 = u8"@\n£\r\n$\t¥\f˜〜\r";

    BOOST_CHECK_EQUAL(unicode2gsm::requires_transliteration(str_1.c_str()), false);
    BOOST_CHECK_EQUAL(unicode2gsm::requires_transliteration(str_2.c_str()), false);
    BOOST_CHECK_EQUAL(unicode2gsm::requires_transliteration(str_3.c_str()), true);
    BOOST_CHECK_EQUAL(unicode2gsm::requires_transliteration(str_4.c_str()), true);
    BOOST_CHECK_EQUAL(unicode2gsm::requires_transliteration(str_5.c_str()), true);
    BOOST_CHECK_EQUAL(unicode2gsm::requires_transliteration(str_6.c_str()), true);

    BOOST_CHECK_EQUAL(unicode2gsm::requires_transliteration(str_1.c_str(), str_1.length()), false);
    BOOST_CHECK_EQUAL(unicode2gsm::requires_transliteration(str_2.c_str(), str_2.length()), false);
    BOOST_CHECK_EQUAL(unicode2gsm::requires_transliteration(str_3.c_str(), str_3.length()), true);
    BOOST_CHECK_EQUAL(unicode2gsm::requires_transliteration(str_4.c_str(), str_4.length()), true);
    BOOST_CHECK_EQUAL(unicode2gsm::requires_transliteration(str_5.c_str(), str_5.length()), true);
    BOOST_CHECK_EQUAL(unicode2gsm::requires_transliteration(str_6.c_str(), str_6.length()), true);

    BOOST_CHECK_EQUAL(unicode2gsm::transliterate(str_1.c_str()), str_1);
    BOOST_CHECK_EQUAL(unicode2gsm::transliterate(str_2.c_str()), str_2);
    BOOST_CHECK_EQUAL(unicode2gsm::transliterate(str_3.c_str()), replace(str_3, "\r\n", "\n"));
    BOOST_CHECK_EQUAL(unicode2gsm::transliterate(str_4.c_str()), replace(str_4, "\r\n", "\n"));
    BOOST_CHECK_EQUAL(unicode2gsm::transliterate(str_5.c_str()), "string\nstring2\rstring3\nstring4 string5 string6");
    BOOST_CHECK_EQUAL(unicode2gsm::transliterate(str_6.c_str()), u8"@\n£\n$ ¥ ~~\r");
}

BOOST_AUTO_TEST_SUITE_END()
