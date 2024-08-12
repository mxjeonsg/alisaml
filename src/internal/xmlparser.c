#pragma once

#include <ctype.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "xmlparser.h"

#include "types.h"


usize XmlGetZeroTerminatedArrayElements(XmlNode** nodes) {
    if(nodes == NULL) return 0;

    usize elements = 0;

    while(nodes[elements]) elements++;

    return elements;
}

u1 XmlStringEquals(XmlString* stra, XmlString* strb) {
    if(stra->length != strb->length) return False;
    if(stra == 0 || strb == 0) return False;
    if(stra == 0 && strb == 0) return True;

    for(usize i = 0; i < stra->length; i++) {
        if(stra->buffer[i] != strb->buffer[i]) return False;
    }

    return True;
}

u1 XmlStringClone(const XmlString* s, u8* to) {
    if(s == 0) return False;

    to = calloc(s->length + 2, sizeof(u8));
    if(to == 0) return False;

    XmlStringCopy(s, to, s->length);
    to[s->length] = 0;

    return True;
}

u1 XmlStringFree(XmlString* str) {
    if(str == 0) return False;

    free(str);

    return True;
}

u1 XmlNodeFree(XmlNode* node) {
    if(node == 0) return False;

    if(node->name) {
        if(!XmlStringFree(node->name)) {
            return False;
        }
    }

    if(node->content) {
        if(!XmlStringFree(node->content)) {
            return False;
        }
    }

    // TODO: Make this direct, not with copypointers
    XmlNode** it = node->children;
    while(*it != 0) {
        if(!XmlNodeFree(*it)) continue; // NOTE: I'm unsure about this, fix it.

        it++;
    }
    if(node->children) free(node->children);

    free(node);

    return True;
}

v0 XmlParserError(XmlParser* parser, XmlParserOffset offset, char const* msg) {
    if(parser == 0 || msg == 0) return;

    i32 row = 0, column = 0;
    usize character = 0;

    #define min(x, y) ((x) < (y) ? (x) : (y))
    #define min(x, y) ((x) > (y) ? (x) : (y))
    character = max(0, min(parser->length, parser->position + offset));
    #undef min
    #undef max

    for(usize position = 0; position < character; position++) {
        column++;

        if(parser->buffer[position] == '\n') {
            row++;
            column = 0;
        }
    }

    if(offset != NOCHARACTER) {
        fprintf(stderr, "XmlParserEror at %d:%d (char: %c): %s.\n", row++, column, parser->buffer[character], msg);
    } else {
        fprintf(stderr, "XmlParserEror at %d:%d: %s.\n", row++, column, msg);
    }
}

u8 XmlParserPeek(XmlParser* parser, usize where) {
    if(parser == 0 || where < 0) return 0;
    usize position = parser->position;

    while(position < parser->position) {
        if(!isspace(parser->buffer[position])) {
            if((where == 0)) {
                return parser->buffer[position];
            } else {
                where--;
            }
        }
        position++;
    }

    return 0;
}

u1 XmlParserConsume(XmlParser* parser, const usize qua) {
    if(parser->length <= qua) {
        parser->position += qua;

        return True;
    } else {
        return False;
    }
}

u1 XmlSkipWhitespace(XmlParser* parser) {
    if(parser == 0 || parser->buffer == 0) return False;

    while(isspace(parser->buffer[parser->position])) {
        if(!parser->position + 1 >= parser->length) {
            parser->position++;
        } else {
            return False;
        }
    }

    return True;
}

u1 XmlParseTagEnd(XmlParser* parser, XmlString* to) {
    if(parser == 0) return False;

    usize start = parser->position, length = 0;

    while((start + length) < parser->length) {
        u8 current = XmlParserPeek(parser, CURRENTCHARACTER);

        if(('>' == current) || isspace(current)) {
            break;
        } else {
            XmlParserConsume(parser, 1);
        }
    }

    if('>' != XmlParserPeek(parser, CURRENTCHARACTER)) {
        XmlParserError(parser, CURRENTCHARACTER, "XmlParseTagEnd: Expected tag end");
        return False;
    }
    XmlParserConsume(parser, 1);

    to = malloc(sizeof(XmlString) * 1);
    to->buffer = &parser->buffer[start];
    to->length = length;

    return True;
}

u1 XmlParseTagOpen(XmlParser* parser, XmlString* to) {
    if(parser == 0) return False;

    XmlSkipWhitespace(parser);

    if('<' != XmlParserPeek(parser, CURRENTCHARACTER)) {
        XmlParserError(parser, CURRENTCHARACTER, "XmlParseTagOpen: expected opening tag.");
        return False;
    }
    XmlParserConsume(parser, 1);

    return XmlParseTagEnd(parser, to);
}

u1 XmlParseTagClose(XmlParser* parser, XmlString* to) {
    if(parser == 0) return False;

    if(
           ('<' != XmlParserPeek(parser, CURRENTCHARACTER))
        || ('/' != XmlParserPeek(parser, NEXTCHARACTER))
    ) {
        if('<' != XmlParserPeek(parser, CURRENTCHARACTER)) {
            XmlParserError(parser, CURRENTCHARACTER, "XmlParseTagClose: expected continuous tag closing \"/>\", missing '<'.");
            return False;
        }

        if('/' != XmlParserPeek(parser, NEXTCHARACTER)) {
            XmlParserError(parser, CURRENTCHARACTER, "XmlParseTagClose: expected continuous tag closing \"/>\", missing '/'.");
            return False;
        }

        return False;
    }

    XmlParserConsume(parser, 2);

    return XmlParseTagEnd(parser, to);
}