#pragma once

// This is a rewritten version of the original xml parser by ooxi.
// Original license:
// Copyright (c) 2012 ooxi/xml.c
//     https://github.com/ooxi/xml.c
// 
// This software is provided 'as-is', without any express or implied warranty. In
// no event will the authors be held liable for any damages arising from the use of
// this software.
// 
// Permission is granted to anyone to use this software for any purpose, including
// commercial applications, and to alter it and redistribute it freely, subject to
// the following restrictions:
// 
//  1. The origin of this software must not be misrepresented; you must not claim
//     that you wrote the original software. If you use this software in a product,
//     an acknowledgment in the product documentation would be appreciated but is
//     not required.
// 
//  2. Altered source versions must be plainly marked as such, and must not be
//     misrepresented as being the original software.
// 
//  3. This notice may not be removed or altered from any source distribution.

#include "types.h"


typedef struct ooxi_xml_string {
    u8 const* buffer;
    usize length;
} XmlString;

typedef struct ooxi_xml_node {
    XmlString* name, *content;
    struct ooxi_xml_node** children;
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
u1 XmlStringClone(const XmlString* s, u8* to);
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
u1 XmlParseContent(XmlParser* parser, XmlString* to);
u1 XmlParseNode(XmlParser* parser, XmlNode* to);
u1 XmlParseDocument(u8* buffer, const usize length, XmlDocument* to);
u1 XmlOpenDocument(FILE* source, XmlDocument* to);
v0 XmlDocumentFree(XmlDocument* document, const u1 freebuffer);

XmlNode* XmlDocumentRoot(const XmlDocument* document);
XmlString* XmlNodeName(const XmlNode* node);
XmlString* XmlNodeContent(XmlNode* node);
const usize XmlNodeChildren(XmlNode* node);
u1 XmlEasyChild(XmlNode* node, const u8* childname, XmlNode* to, ...);
u1 XmlEasyName(XmlNode* node, u8* to);
u1 XmlEasyContent(XmlNode* node, u8* to);
usize XmlStringLength(XmlString* string);
const v0 XmlStringCopy(XmlString* string, u8* buffer, usize length);
u1 XmlNodeChild(XmlNode* node, usize child, XmlNode* to);