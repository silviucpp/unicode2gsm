#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#define BOOST_TEST_MODULE unidecode2gsm_test

#include <boost/test/unit_test.hpp>
#include <iostream>

#include "unicode2gsm/unicode2gsm.h"

const char *kGsmAlphabet=u8"@£$¥èéùìòÇØøÅåΔ_ΦΓΛΩΠΨΣΘΞÆæßÉ!\"#¤%&'()*+,-./0123456789:;<=>?¡ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÑÜ§¿abcdefghijklmnopqrstuvwxyzäöñüà^{}\[~]|€";

BOOST_AUTO_TEST_SUITE(unidecode2gsm_test)

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
    const std::string kInString=u8"［ ］ ˜ ∼";
    const std::string kOutput1=u8"[ ] ~ ~";
    const std::string kOutput2=u8"( ) - -";

    BOOST_CHECK_EQUAL(unicode2gsm::init_transliteration_map(true), true);
    BOOST_CHECK_EQUAL(unicode2gsm::transliterate(kInString.c_str()), kOutput2);

    BOOST_CHECK_EQUAL(unicode2gsm::init_transliteration_map(false), true);
    BOOST_CHECK_EQUAL(unicode2gsm::transliterate(kInString.c_str()), kOutput1);
}

BOOST_AUTO_TEST_SUITE_END()
