#pragma once

#include "types.h"

typedef struct ooxi_xml_string {
    u8 const* buffer;
    usize length;
} XmlString;

typedef struct ooxi_xml_node {
    XmlString* name, *content;
    XmlNode** children;
} XmlNode;

typedef struct ooxi_xml_document {
    u8* buffer;
    usize length;

    XmlNode* root;
} XmlDocument;

typedef struct ooxi_xml_parser {
    u8* buffer;
    usize position;
    usize length;
} XmlParser;

typedef enum ooxi_xml_parser_offset {
    NOCHARACTER = -1,
    CURRENTCHARACTER = 0,
    NEXTCHARACTER = 1
} XmlParserOffset;


usize XmlGetZeroTerminatedArrayElements(XmlNode** nodes);
u1 XmlStringEquals(XmlString* stra, XmlString* strb);
u8* XmlStringClone(const XmlString* s, u8* to);
u1 XmlStringFree(XmlString* str);
u1 XmlNodeFree(XmlNode* node);
v0 XmlParserInfo(XmlParser* parser, char const* msg);
v0 XmlParserError(XmlParser* parser, XmlParserOffset offset, char const* msg);
u8 XmlParserPeek(XmlParser* parser, usize where);
u1 XmlParserConsume(XmlParser* parser, const usize qua);
u1 XmlSkipWhitespace(XmlParser* parser);
u1 XmlParseTagEnd(XmlParser* parser, XmlString* to);
u1 XmlParseTagOpen(XmlParser* parser, XmlString* to);
u1 XmlParseTagClose(XmlParser* parser, XmlString* to);