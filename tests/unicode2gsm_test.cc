#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#define BOOST_TEST_MODULE unicode2gsm_test

#include <boost/test/unit_test.hpp>
#include <iostream>

#include "unicode2gsm/unicode2gsm.h"
#include "unicode2gsm/common.h"
#include "unicode2gsm/charset.h"

const char *kGsmAlphabet=u8"@£$¥èéùìòÇØøÅåΔ_ΦΓΛΩΠΨΣΘΞÆæßÉ!\"#¤%&'()*+,-./0123456789:;<=>?¡ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÑÜ§¿abcdefghijklmnopqrstuvwxyzäöñüà^{}\[~]|€";

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

BOOST_AUTO_TEST_SUITE_END()
