
// https://github.com/minutis-redcall/app/blob/fe57f22f5d3fad42e5968aca4b8fe6ad0ef98c77/symfony/src/Tools/GSM.php
// https://github.com/BenMorel/GsmCharsetConverter/blob/master/src/Charset.php
// https://unicode-table.com/en/
// https://www.ssec.wisc.edu/~tomw/java/unicode.html

#include "unicode2gsm/unicode2gsm.h"
#include <map>

#define MAX_CODEPOINT_VALUE 65536

namespace unicode2gsm{

namespace {

// lookup tables

const char* kTransliterationLookupTable[MAX_CODEPOINT_VALUE] = {nullptr};
bool kGsmLookupTable[MAX_CODEPOINT_VALUE] = {false};

// gsm alphabet (For each gsm7 code value, this tables gives the equivalent UTF-8 code point.)

const uint32_t kGsm7BitAlphabetSize = 128;

const unicode_char gsm7bits_to_unicode[kGsm7BitAlphabetSize] = {
      '@', 0xa3,  '$', 0xa5, 0xe8, 0xe9, 0xf9, 0xec, 0xf2, 0xc7, '\n', 0xd8, 0xf8, '\r', 0xc5, 0xe5,
    0x394,  '_',0x3a6,0x393,0x39b,0x3a9,0x3a0,0x3a8,0x3a3,0x398,0x39e,    0, 0xc6, 0xe6, 0xdf, 0xc9,
      ' ',  '!',  '"',  '#', 0xa4,  '%',  '&', '\'',  '(',  ')',  '*',  '+',  ',',  '-',  '.',  '/',
      '0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  ':',  ';',  '<',  '=',  '>',  '?',
     0xa1,  'A',  'B',  'C',  'D',  'E',  'F',  'G',  'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O',
      'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W',  'X',  'Y',  'Z', 0xc4, 0xd6, 0xd1, 0xdc, 0xa7,
     0xbf,  'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o',
      'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z', 0xe4, 0xf6, 0xf1, 0xfc, 0xe0,
};

const unicode_char gsm7bits_extend_to_unicode[] = {
    '\f',
    '^',
    '\\',
    '|',
    0x20ac //€
};

// idea is that GSM extended alphabet symbols are escaped and will consume 2 chars. The following symbols have a decent
// replacement so we can transliterate them (optional)

std::map<unicode_char, const char*> kGsmExtended2UnicodeOptional = {
    {'{', "("} ,
    {'}', ")"},
    {'[', "("},
    {'~', "-"},
    {']', ")"}
};

// transliteration map with UTF-8 code point and the associated ascii string to be converted into.

std::map<unicode_char, const char*> kTransliterationMap = {
    {0x0020, " "}, //Basic latin space
    {0x2013, "-"}, //–
    {0x2014, "-"}, //—
    {0x20B9, "Rs"}, //₹
    {0x20B4, "UAH"}, //₴
    {0x20BD, "p"}, //₽
    {0x00B7, "."}, //·
    {0x0463, ""}, //ѣ
    {0x0462, ""}, //Ѣ
    {0x007C, "|"}, //|
    {0x0301, ""}, ///́
    {0x042C, ""}, //Ь
    {0x044C, ""}, //ь
    {0x042A, ""}, //Ъ
    {0x044A, ""}, //ъ
    {0x00BA, "0"}, //º
    {0x00B0, "0"}, //°
    {0x00B9, "1"}, //¹
    {0x00B2, "2"}, //²
    {0x00B3, "3"}, //³
    {0x01FD, "ae"}, //ǽ
    {0x0153, "oe"}, //œ
    {0x00C0, "A"}, //À
    {0x00C1, "A"}, //Á
    {0x00C2, "A"}, //Â
    {0x00C3, "A"}, //Ã
    {0x01FA, "A"}, //Ǻ
    {0x0100, "A"}, //Ā
    {0x0102, "A"}, //Ă
    {0x0104, "A"}, //Ą
    {0x01CD, "A"}, //Ǎ
    {0x0410, "A"}, //А
    {0x0391, "A"}, //Α
    {0x00E1, "a"}, //á
    {0x00E2, "a"}, //â
    {0x00E3, "a"}, //ã
    {0x01FB, "a"}, //ǻ
    {0x0101, "a"}, //ā
    {0x0103, "a"}, //ă
    {0x0105, "a"}, //ą
    {0x01CE, "a"}, //ǎ
    {0x00AA, "a"}, //ª
    {0x0430, "a"}, //а
    {0x0411, "B"}, //Б
    {0x0431, "b"}, //б
    {0x0106, "C"}, //Ć
    {0x0108, "C"}, //Ĉ
    {0x010A, "C"}, //Ċ
    {0x010C, "C"}, //Č
    {0x040B, "C"}, //Ћ
    {0x00E7, "c"}, //ç
    {0x0107, "c"}, //ć
    {0x0109, "c"}, //ĉ
    {0x010B, "c"}, //ċ
    {0x010D, "c"}, //č
    {0x045B, "c"}, //ћ
    {0x0414, "D"}, //Д
    {0x0434, "d"}, //д
    {0x00D0, "Dj"}, //Ð
    {0x010E, "Dj"}, //Ď
    {0x0110, "Dj"}, //Đ
    {0x0402, "Dj"}, //Ђ
    {0x00F0, "dj"}, //ð
    {0x010F, "dj"}, //ď
    {0x0111, "dj"}, //đ
    {0x0452, "dj"}, //ђ
    {0x00C8, "E"}, //È
    {0x00CA, "E"}, //Ê
    {0x00CB, "E"}, //Ë
    {0x0112, "E"}, //Ē
    {0x0114, "E"}, //Ĕ
    {0x0116, "E"}, //Ė
    {0x0118, "E"}, //Ę
    {0x011A, "E"}, //Ě
    {0x0415, "E"}, //Е
    {0x0401, "E"}, //Ё
    {0x042C, "E"}, //Ь
    {0x042D, "E"}, //Э
    {0x042D, "E"}, //Э
    {0x0404, "E"}, //Є
    {0x0466, "E"}, //Ѧ
    {0x1EC4, "E"}, //Ễ
    {0x00EA, "e"}, //ê
    {0x00EB, "e"}, //ë
    {0x0113, "e"}, //ē
    {0x0115, "e"}, //ĕ
    {0x0117, "e"}, //ė
    {0x0119, "e"}, //ę
    {0x011B, "e"}, //ě
    {0x0435, "e"}, //е
    {0x0451, "e"}, //ё
    {0x044C, "e"}, //ь
    {0x044D, "e"}, //э
    {0x044D, "e"}, //э
    {0x0454, "e"}, //є
    {0x0467, "e"}, //ѧ
    {0x0259, "e"}, //ə
    {0x0258, "e"}, //ɘ
    {0x1EC5, "e"}, //ễ
    {0x0424, "F"}, //Ф
    {0x0192, "f"}, //ƒ
    {0x0444, "f"}, //ф
    {0x011C, "G"}, //Ĝ
    {0x011E, "G"}, //Ğ
    {0x0120, "G"}, //Ġ
    {0x0122, "G"}, //Ģ
    {0x0413, "G"}, //Г
    {0x0490, "G"}, //Ґ
    {0x011D, "g"}, //ĝ
    {0x011F, "g"}, //ğ
    {0x0121, "g"}, //ġ
    {0x0123, "g"}, //ģ
    {0x0433, "g"}, //г
    {0x0491, "g"}, //ґ
    {0x0124, "H"}, //Ĥ
    {0x0126, "H"}, //Ħ
    {0x0125, "h"}, //ĥ
    {0x0127, "h"}, //ħ
    {0x00CC, "I"}, //Ì
    {0x00CD, "I"}, //Í
    {0x00CE, "I"}, //Î
    {0x00CF, "I"}, //Ï
    {0x0128, "I"}, //Ĩ
    {0x012A, "I"}, //Ī
    {0x012C, "I"}, //Ĭ
    {0x01CF, "I"}, //Ǐ
    {0x012E, "I"}, //Į
    {0x0130, "I"}, //İ
    {0x0418, "I"}, //И
    {0x0419, "I"}, //Й
    {0x0406, "I"}, //І
    {0x00ED, "i"}, //í
    {0x00EE, "i"}, //î
    {0x00EF, "i"}, //ï
    {0x0129, "i"}, //ĩ
    {0x012B, "i"}, //ī
    {0x012D, "i"}, //ĭ
    {0x01D0, "i"}, //ǐ
    {0x012F, "i"}, //į
    {0x0131, "i"}, //ı
    {0x0438, "i"}, //и
    {0x0439, "i"}, //й
    {0x0456, "i"}, //і
    {0x0134, "J"}, //Ĵ
    {0x0135, "j"}, //ĵ
    {0x0136, "K"}, //Ķ
    {0x041A, "K"}, //К
    {0x0137, "k"}, //ķ
    {0x043A, "k"}, //к
    {0x0425, "Kh"}, //Х
    {0x0445, "kh"}, //х
    {0x0139, "L"}, //Ĺ
    {0x013B, "L"}, //Ļ
    {0x013D, "L"}, //Ľ
    {0x013F, "L"}, //Ŀ
    {0x0141, "L"}, //Ł
    {0x041B, "L"}, //Л
    {0x013A, "l"}, //ĺ
    {0x013C, "l"}, //ļ
    {0x013E, "l"}, //ľ
    {0x0140, "l"}, //ŀ
    {0x0142, "l"}, //ł
    {0x043B, "l"}, //л
    {0x041C, "M"}, //М
    {0x043C, "m"}, //м
    {0x0143, "N"}, //Ń
    {0x0145, "N"}, //Ņ
    {0x0147, "N"}, //Ň
    {0x041D, "N"}, //Н
    {0x2116, "N"}, //№
    {0x0144, "n"}, //ń
    {0x0146, "n"}, //ņ
    {0x0148, "n"}, //ň
    {0x0149, "n"}, //ŉ
    {0x043D, "n"}, //н
    {0x00D2, "O"}, //Ò
    {0x00D3, "O"}, //Ó
    {0x00D4, "O"}, //Ô
    {0x00D5, "O"}, //Õ
    {0x014C, "O"}, //Ō
    {0x014E, "O"}, //Ŏ
    {0x01D1, "O"}, //Ǒ
    {0x0150, "O"}, //Ő
    {0x01A0, "O"}, //Ơ
    {0x01FE, "O"}, //Ǿ
    {0x041E, "O"}, //О
    {0x0460, "O"}, //Ѡ
    {0x046A, "O"}, //Ѫ
    {0x1EDC, "O"}, //Ờ
    {0x00F3, "o"}, //ó
    {0x00F4, "o"}, //ô
    {0x00F5, "o"}, //õ
    {0x014D, "o"}, //ō
    {0x014F, "o"}, //ŏ
    {0x01D2, "o"}, //ǒ
    {0x0151, "o"}, //ő
    {0x01A1, "o"}, //ơ
    {0x01FF, "o"}, //ǿ
    {0x00BA, "o"}, //º
    {0x043E, "o"}, //о
    {0x0461, "o"}, //ѡ
    {0x046B, "o"}, //ѫ
    {0x1EDD, "o"}, //ờ
    {0x041F, "P"}, //П
    {0x043F, "p"}, //п
    {0x0154, "R"}, //Ŕ
    {0x0156, "R"}, //Ŗ
    {0x0158, "R"}, //Ř
    {0x0420, "R"}, //Р
    {0x0155, "r"}, //ŕ
    {0x0157, "r"}, //ŗ
    {0x0159, "r"}, //ř
    {0x0440, "r"}, //р
    {0x015A, "S"}, //Ś
    {0x015C, "S"}, //Ŝ
    {0x015E, "S"}, //Ş
    {0x0218, "S"}, //Ș
    {0x0160, "S"}, //Š
    {0x0421, "S"}, //С
    {0x015B, "s"}, //ś
    {0x015D, "s"}, //ŝ
    {0x015F, "s"}, //ş
    {0x0219, "s"}, //ș
    {0x0161, "s"}, //š
    {0x017F, "s"}, //ſ
    {0x0441, "s"}, //с
    {0x0162, "T"}, //Ţ
    {0x021A, "T"}, //Ț
    {0x0164, "T"}, //Ť
    {0x0166, "T"}, //Ŧ
    {0x0422, "T"}, //Т
    {0x0163, "t"}, //ţ
    {0x021B, "t"}, //ț
    {0x0165, "t"}, //ť
    {0x0167, "t"}, //ŧ
    {0x0442, "t"}, //т
    {0x0426, "Tc"}, //Ц
    {0x0446, "tc"}, //ц
    {0x00D9, "U"}, //Ù
    {0x00DA, "U"}, //Ú
    {0x00DB, "U"}, //Û
    {0x0168, "U"}, //Ũ
    {0x016A, "U"}, //Ū
    {0x016C, "U"}, //Ŭ
    {0x016E, "U"}, //Ů
    {0x0170, "U"}, //Ű
    {0x0172, "U"}, //Ų
    {0x01AF, "U"}, //Ư
    {0x01D3, "U"}, //Ǔ
    {0x01D5, "U"}, //Ǖ
    {0x01D7, "U"}, //Ǘ
    {0x01D9, "U"}, //Ǚ
    {0x01DB, "U"}, //Ǜ
    {0x0423, "U"}, //У
    {0x040E, "U"}, //Ў
    {0x00FA, "u"}, //ú
    {0x00FB, "u"}, //û
    {0x0169, "u"}, //ũ
    {0x016B, "u"}, //ū
    {0x016D, "u"}, //ŭ
    {0x016F, "u"}, //ů
    {0x0171, "u"}, //ű
    {0x0173, "u"}, //ų
    {0x01B0, "u"}, //ư
    {0x01D4, "u"}, //ǔ
    {0x01D6, "u"}, //ǖ
    {0x01D8, "u"}, //ǘ
    {0x01DA, "u"}, //ǚ
    {0x01DC, "u"}, //ǜ
    {0x0443, "u"}, //у
    {0x045E, "u"}, //ў
    {0x0412, "V"}, //В
    {0x0432, "v"}, //в
    {0x00DD, "Y"}, //Ý
    {0x0178, "Y"}, //Ÿ
    {0x0176, "Y"}, //Ŷ
    {0x1EF2, "Y"}, //Ỳ
    {0x042B, "Y"}, //Ы
    {0x00FD, "y"}, //ý
    {0x00FF, "y"}, //ÿ
    {0x0177, "y"}, //ŷ
    {0x1EF3, "y"}, //ỳ
    {0x044B, "y"}, //ы
    {0x0174, "W"}, //Ŵ
    {0x0175, "w"}, //ŵ
    {0x0179, "Z"}, //Ź
    {0x017B, "Z"}, //Ż
    {0x017D, "Z"}, //Ž
    {0x0417, "Z"}, //З
    {0x017A, "z"}, //ź
    {0x017C, "z"}, //ż
    {0x017E, "z"}, //ž
    {0x0437, "z"}, //з
    {0x01FC, "AE"}, //Ǽ
    {0x0132, "IJ"}, //Ĳ
    {0x0133, "ij"}, //ĳ
    {0x0152, "OE"}, //Œ
    {0x0427, "Ch"}, //Ч
    {0x0447, "ch"}, //ч
    {0x042E, "Iu"}, //Ю
    {0x044E, "iu"}, //ю
    {0x042F, "Ia"}, //Я
    {0x044F, "ia"}, //я
    {0x0407, "Ji"}, //Ї
    {0x0457, "ji"}, //ї
    {0x0428, "Sh"}, //Ш
    {0x0448, "sh"}, //ш
    {0x0429, "Shch"}, //Щ
    {0x0449, "shch"}, //щ
    {0x0416, "Zh"}, //Ж
    {0x0436, "zh"}, //ж
    {0x0455, "dz"}, //ѕ
    {0x045F, "dz"}, //џ
    {0x0405, "Dz"}, //Ѕ
    {0x040F, "Dz"}, //Џ
    {0x0458, "j"}, //ј
    {0x0459, "lj"}, //љ
    {0x0409, "Lj"}, //Љ
    {0x045A, "nj"}, //њ
    {0x040A, "Nj"}, //Њ
    {0x045C, "kj"}, //ќ
    {0x040C, "Kj"}, //Ќ
    {0x0469, "je"}, //ѩ
    {0x0468, "Je"}, //Ѩ
    {0x046D, "jo"}, //ѭ
    {0x046C, "Jo"}, //Ѭ
    {0x046F, "ks"}, //ѯ
    {0x046E, "Ks"}, //Ѯ
    {0x0471, "ps"}, //ѱ
    {0x0470, "Ps"}, //Ѱ
    {0x0465, "je"}, //ѥ
    {0x0464, "Je"}, //Ѥ
    {0xA657, "ja"}, //ꙗ
    {0xA656, "ja"}, //Ꙗ
    {0x00AB, "\""}, //«
    {0x00BB, "\""}, //»
    {0x2019, "'"}, //’
    {0x0060, "'"}, //`
    {0X0000, " "}, //Null
    {0X0003, ""}, //End of Text
    {0X0004, ""}, //End of Transmission
    {0X0009, " "}, //Horizontal Tabulation
    {0X0010, ""}, //Data Link Escape
    {0X0011, ""}, //Device Control One
    {0X0012, ""}, //Device Control Two
    {0X0013, ""}, //Device Control Three
    {0X0014, ""}, //Device Control Four
    {0X0017, ""}, //End of Transmission Block
    {0X0019, ""}, //End of Medium
    {0X0080, ""}, //Control
    {0X008D, ""}, //Reverse Line Feed
    {0X0090, ""}, //Device Control String
    {0X009B, ""}, //Control Sequence Introducer
    {0X009F, ""}, //Application Program Command
    {0X00A0, "'"}, //No-Break Space
    {0X00B4, "'"}, //´
    {0X00BC, "1/4"}, //¼
    {0X00BD, "1/2"}, //½
    {0X00BE, "3/4"}, //¾
    {0X00F7, "/"}, //÷
    {0X01C3, "!"}, //ǃ
    {0X0262, "G"}, //ɢ
    {0X026A, "I"}, //ɪ
    {0X0274, "N"}, //ɴ
    {0X0280, "R"}, //ʀ
    {0X028F, "Y"}, //ʏ
    {0X0299, "B"}, //ʙ
    {0X029C, "H"}, //ʜ
    {0X029F, "L"}, //ʟ
    {0X02B9, "'"}, //ʹ
    {0X02BA, "\""}, //ʺ
    {0X02BB, "'"}, //ʻ
    {0X02BC, "'"}, //ʼ
    {0X02BD, "'"}, //ʽ
    {0X02C6, "^"}, //ˆ
    {0X02C8, "'"}, //ˈ
    {0X02CA, "'"}, //ˊ
    {0X02CB, "'"}, //ˋ
    {0X02D0, ":"}, //ː
    {0X02D6, "+"}, //˖
    {0X02DC, "~"}, //˜
    {0X02EE, "\""}, //ˮ
    {0X02F7, "~"}, //˷
    {0X02F8, ":"}, //˸
    {0X0302, "^"}, //Combining Circumflex Accent
    {0X0303, "~"}, //Combining Tilde
    {0X0313, "'"}, //Combining Comma Above
    {0X0314, "'"}, //Combining Reversed Comma Above
    {0X0326, ","}, //Combining Comma Below
    {0X0330, "~"}, //Combining Tilde Below
    {0X0332, "_"}, //Combining Low Line
    {0X0334, "~"}, //Combining Tilde Overlay
    {0X0337, "/"}, //Combining Short Solidus Overlay
    {0X0338, "/"}, //Combining Long Solidus Overlay
    {0X0347, "="}, //Combining Equals Sign Below
    {0X1D00, "A"}, //ᴀ
    {0X1D04, "C"}, //ᴄ
    {0X1D05, "D"}, //ᴅ
    {0X1D07, "E"}, //ᴇ
    {0X1D0A, "J"}, //ᴊ
    {0X1D0B, "K"}, //ᴋ
    {0X1D0D, "M"}, //ᴍ
    {0X1D0F, "O"}, //ᴏ
    {0X1D18, "P"}, //ᴘ
    {0X1D1B, "T"}, //ᴛ
    {0X1D1C, "U"}, //ᴜ
    {0X1D20, "V"}, //ᴠ
    {0X1D21, "W"}, //ᴡ
    {0X1D22, "Z"}, //ᴢ
    {0X1DCD, "^"}, //Combining Double Circumflex Above
    {0X2000, "'"}, //En quad
    {0X2001, " "}, //Em quad
    {0X2002, " "}, //En space
    {0X2003, " "}, //Em space
    {0X2004, " "}, //Three-per-em space
    {0X2005, " "}, //Four-per-em space
    {0X2006, " "}, //Six-per-em space
    {0X2007, " "}, //Figure space
    {0X2008, " "}, //Punctuation space
    {0X2009, " "}, //Thin Space
    {0X200A, " "}, //Hair Space
    {0X2010, "-"}, //‐
    {0X2015, "-"}, //―
    {0X2017, "_"}, //‗
    {0X2018, "'"}, //‘
    {0X201A, "'"}, //‚
    {0X201B, "'"}, //‛
    {0X201C, "\""}, //“
    {0X201D, "\""}, //”
    {0X201E, "\""}, //„
    {0X201F, "\""}, //‟
    {0X2026, "..."}, //…
    {0X2028, " "}, //Line Separator
    {0X2029, " "}, //Paragraph Separator
    {0X202F, " "}, //Narrow No-Break Space
    {0X2039, ">"}, //‹
    {0X203A, "<"}, //›
    {0X203C, "!!"}, //‼
    {0X2043, "-"}, //⁃
    {0X2044, "/"}, //⁄
    {0X204E, "*"}, //⁎
    {0X204F, ";"}, //⁏
    {0X205F, " "}, //Medium Mathematical Space
    {0X2060, " "}, //⁠Word Joiner
    {0X20D2, "|"}, //Combining Long Vertical Line Overlay
    {0X20D3, "|"}, //Combining Short Vertical Line Overlay
    {0X20E5, "\\"}, //Combining Reverse Solidus Overlay
    {0X2215, "/"}, //∕
    {0X2217, "*"}, //∗
    {0X2223, "|"}, //∣
    {0X223C, "~"}, //∼
    {0X229B, "*"}, //⊛
    {0X239C, "|"}, //⎜
    {0X239F, "|"}, //⎟
    {0X23B8, "|"}, //⎸
    {0X23B9, "|"}, //⎹
    {0X23BC, "-"}, //⎼
    {0X23BD, "-"}, //⎽
    {0X23D0, "|"}, //⏐
    {0X2722, "*"}, //✢
    {0X2723, "*"}, //✣
    {0X2724, "*"}, //✤
    {0X2725, "*"}, //✥
    {0X2731, "*"}, //✱
    {0X2732, "*"}, //✲
    {0X2733, "*"}, //✳
    {0X273A, "*"}, //✺
    {0X273B, "*"}, //✻
    {0X273C, "*"}, //✼
    {0X273D, "*"}, //✽
    {0X2743, "*"}, //❃
    {0X2749, "*"}, //❉
    {0X274A, "*"}, //❊
    {0X274B, "*"}, //❋
    {0X275B, "'"}, //❛
    {0X275C, "'"}, //❜
    {0X275D, "\""}, //❝
    {0X275E, "\""}, //❞
    {0X2768, "("}, //❨
    {0X2769, ")"}, //❩
    {0X276A, "("}, //❪
    {0X276B, ")"}, //❫
    {0X2774, "{"}, //❴
    {0X2775, "}"}, //❵
    {0X27EE, "("}, //⟮
    {0X27EF, ")"}, //⟯
    {0X2982, ":"}, //⦂
    {0X2985, "("}, //⦅
    {0X2986, ")"}, //⦆
    {0X29C6, "*"}, //⧆
    {0X29F5, "\\"}, //⧵
    {0X29F8, "/"}, //⧸
    {0X29F9, "\\"}, //⧹
    {0X3000, " "}, //
    {0X3002, "."}, //。
    {0X301D, "\""}, //〝
    {0X301E, "\""}, //〞
    {0XA730, "F"}, //ꜰ
    {0XA731, "S"}, //ꜱ
    {0XA789, ":"}, //꞉
    {0XA78A, "="}, //꞊
    {0XFE10, "'"}, //︐
    {0XFE11, "'"}, //︑
    {0XFE13, ":"}, //︓
    {0XFE14, ";"}, //︔
    {0XFE15, "!"}, //︕
    {0XFE16, "?"}, //︖
    {0XFE50, ","}, //﹐
    {0XFE51, ","}, //﹑
    {0XFE52, "."}, //﹒
    {0XFE54, ";"}, //﹔
    {0XFE56, "?"}, //﹖
    {0XFE57, "!"}, //﹗
    {0XFE59, "("}, //﹙
    {0XFE5A, ")"}, //﹚
    {0XFE5B, "{"}, //﹛
    {0XFE5C, "}"}, //﹜
    {0XFE5F, "#"}, //﹟
    {0XFE60, "&"}, //﹠
    {0XFE61, "*"}, //﹡
    {0XFE62, "+"}, //﹢
    {0XFE63, "-"}, //﹣
    {0XFE64, "<"}, //﹤
    {0XFE65, ">"}, //﹥
    {0XFE66, "="}, //﹦
    {0XFE68, "\\"}, //﹨
    {0XFE69, "$"}, //﹩
    {0XFE6A, "%"}, //﹪
    {0XFE6B, "@"}, //﹫
    {0XFF01, "!"}, //！
    {0XFF02, "\""}, //＂
    {0XFF03, "#"}, //＃
    {0XFF04, "$"}, //＄
    {0XFF05, "%"}, //％
    {0XFF06, "&"}, //＆
    {0XFF07, "'"}, //＇
    {0XFF08, "("}, //（
    {0XFF09, ")"}, //）
    {0XFF0A, "*"}, //＊
    {0XFF0B, "+"}, //＋
    {0XFF0C, ","}, //，
    {0XFF0D, "-"}, //－
    {0XFF0E, "."}, //．
    {0XFF0F, "/"}, //／
    {0XFF10, "0"}, //０
    {0XFF11, "1"}, //１
    {0XFF12, "2"}, //２
    {0XFF13, "3"}, //３
    {0XFF14, "4"}, //４
    {0XFF15, "5"}, //５
    {0XFF16, "6"}, //６
    {0XFF17, "7"}, //７
    {0XFF18, "8"}, //８
    {0XFF19, "9"}, //９
    {0XFF1A, ":"}, //：
    {0XFF1B, ";"}, //；
    {0XFF1C, "<"}, //＜
    {0XFF1D, "="}, //＝
    {0XFF1E, ">"}, //＞
    {0XFF1F, "?"}, //？
    {0XFF20, "@"}, //＠
    {0XFF21, "A"}, //Ａ
    {0XFF22, "B"}, //Ｂ
    {0XFF23, "C"}, //Ｃ
    {0XFF24, "D"}, //Ｄ
    {0XFF25, "E"}, //Ｅ
    {0XFF26, "F"}, //Ｆ
    {0XFF27, "G"}, //Ｇ
    {0XFF28, "H"}, //Ｈ
    {0XFF29, "I"}, //Ｉ
    {0XFF2A, "J"}, //Ｊ
    {0XFF2B, "K"}, //Ｋ
    {0XFF2C, "L"}, //Ｌ
    {0XFF2D, "M"}, //Ｍ
    {0XFF2E, "N"}, //Ｎ
    {0XFF2F, "O"}, //Ｏ
    {0XFF30, "P"}, //Ｐ
    {0XFF31, "Q"}, //Ｑ
    {0XFF32, "R"}, //Ｒ
    {0XFF33, "S"}, //Ｓ
    {0XFF34, "T"}, //Ｔ
    {0XFF35, "U"}, //Ｕ
    {0XFF36, "V"}, //Ｖ
    {0XFF37, "W"}, //Ｗ
    {0XFF38, "X"}, //Ｘ
    {0XFF39, "Y"}, //Ｙ
    {0XFF3A, "Z"}, //Ｚ
    {0XFF3B, "["}, //［
    {0XFF3C, "\\"}, //＼
    {0XFF3D, "]"}, //］
    {0XFF3E, "^"}, //＾
    {0XFF3F, "_"}, //＿
    {0XFF5B, "{"}, //｛
    {0XFF5C, "|"}, //｜
    {0XFF5D, "}"}, //｝
    {0XFF5E, "~"}, //～
    {0XFF61, "."}, //｡
    {0XFF64, ","} //､
};
    
// helper functions

inline uint32_t do_transliteration(const char* utf8, std::string& output)
{
    uint32_t step_size = GetCodePointSize(*utf8);
    unicode_char codepoint = GetCodePoint(utf8, step_size);
    
    const char* replacement = nullptr;
    
    if(codepoint < MAX_CODEPOINT_VALUE)
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
    
    if(codepoint >= MAX_CODEPOINT_VALUE || !kGsmLookupTable[codepoint])
        return false;
    
    return true;
}

}

bool init_transliteration_map(bool transliterate_gsm_extended)
{
    //cleanup tables (in case transliteration settings are updated during execution.)

    memset(kGsmLookupTable, 0, sizeof(kGsmLookupTable));
    memset(kTransliterationLookupTable, 0, sizeof(kTransliterationLookupTable));
    
    // build transliteration lookup table
    
    for(auto it : kTransliterationMap)
    {
        if(it.first >= MAX_CODEPOINT_VALUE)
            return false;
        
        kTransliterationLookupTable[it.first] = it.second;
    }
    
    // build gsm lookup table
    
    for(size_t i = 0; i < kGsm7BitAlphabetSize; i++)
        kGsmLookupTable[gsm7bits_to_unicode[i]] = true;
    
    size_t gsm_extended_alphabet_size = sizeof(gsm7bits_extend_to_unicode)/sizeof(unicode_char);
    
    for(size_t i = 0; i < gsm_extended_alphabet_size; i++)
        kGsmLookupTable[gsm7bits_extend_to_unicode[i]] = true;
    
    // check if we should avoid transliteration for all extended GSM alphabet symbols. Idea is that GSM extended
    // alphabet symbols are escaped and will consume 2 chars. Some of them have a decent replacement so transliteration
    // for these is optional.
    
    if(transliterate_gsm_extended)
    {
        for(auto it : kGsmExtended2UnicodeOptional)
        {
            for(int i = 0; i < MAX_CODEPOINT_VALUE; i++)
                if(kTransliterationLookupTable[i] != nullptr && kTransliterationLookupTable[i][0] == it.first)
                    kTransliterationLookupTable[i] = it.second;
            
            kTransliterationLookupTable[it.first] = it.second;
        }
    }
    else
    {
        for(auto it : kGsmExtended2UnicodeOptional)
            kGsmLookupTable[it.first] = true;
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


