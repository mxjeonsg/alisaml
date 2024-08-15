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


#include <ctype.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    XmlStringCopy((XmlString*) s, to, s->length);
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
    #define max(x, y) ((x) > (y) ? (x) : (y))
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
        fprintf(stderr, "XmlParserEror at %d:%d (char: %c): %s.\n", row + 1, column, parser->buffer[character], msg);
    } else {
        fprintf(stderr, "XmlParserEror at %d:%d: %s.\n", row + 1, column, msg);
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
            if(!XmlParserConsume(parser, 1)) {
                XmlParserError(parser, CURRENTCHARACTER, "XmlParseTagOpen: failed to consume.");
                return False;
            }
        }
    }

    if('>' != XmlParserPeek(parser, CURRENTCHARACTER)) {
        XmlParserError(parser, CURRENTCHARACTER, "XmlParseTagEnd: Expected tag end");
        return False;
    }

    if(!XmlParserConsume(parser, 1)) {
        XmlParserError(parser, CURRENTCHARACTER, "XmlParseTagOpen: failed to consume.");
        return False;
    }

    to = malloc(sizeof(XmlString) * 1);
    to->buffer = &parser->buffer[start];
    to->length = length;

    return True;
}

u1 XmlParseTagOpen(XmlParser* parser, XmlString* to) {
    if(parser == 0) return False;

    if(!XmlSkipWhitespace(parser)) {
        XmlParserError(parser, CURRENTCHARACTER, "XmlParseTagOpen: failed to skip whitespace.");
        return False;
    }

    if('<' != XmlParserPeek(parser, CURRENTCHARACTER)) {
        XmlParserError(parser, CURRENTCHARACTER, "XmlParseTagOpen: expected opening tag.");
        return False;
    }

    if(!XmlParserConsume(parser, 1)) {
        XmlParserError(parser, CURRENTCHARACTER, "XmlParseTagOpen: failed to consume.");
        return False;
    }

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

u1 XmlParseContent(XmlParser* parser, XmlString* to) {
    if(parser == 0) return False;

    XmlSkipWhitespace(parser);

    usize start = parser->position, length = 0;

    while((start + length) < parser->length) {
        u8 current = XmlParserPeek(parser, CURRENTCHARACTER);

        if('<' == current) {
            break;
        } else {
            XmlParserConsume(parser, 1);
            length++;
        }
    }

    if('<' != XmlParserPeek(parser, CURRENTCHARACTER)) {
        XmlParserError(parser, CURRENTCHARACTER, "XmlParseContent: expect '<");
        return False;
    }

    while((length > 0) && isspace(parser->buffer[(start + length) - 1])) {
        length--;
    }

    to = malloc(sizeof(XmlString) * 1);
    to->buffer = &parser->buffer[start];
    to->length = length;

    return True;
}

u1 XmlParseNode(XmlParser* parser, XmlNode* to) {
    if(parser == 0) return False;

    XmlString* tagopen = 0, *tagclose = 0, *content = 0;
    XmlNode** children = calloc(1, sizeof(XmlNode*));
    children[0] = 0;

    if(!XmlParseTagOpen(parser, tagopen)) {
        XmlParserError(parser, NOCHARACTER, "XmlParseNode: failed XmlParseTagOpen.");
        goto exit_failure;
    }

    if(tagopen->length > 0 && tagopen->buffer[tagopen->length - 1] == '/') {
        tagopen->length--;
        goto node_creation; // TODO: Try to replace this too.
    }

    if('<' != XmlParserPeek(parser, CURRENTCHARACTER)) {
        if(!XmlParseContent(parser, content)) {
            XmlParserError(parser, NOCHARACTER, "XmlParseNode: failed XmlParseContent.");
            goto exit_failure;
        }

        if(content == 0) {
            XmlParserError(parser, 0, "XmlParseNode#content");
            goto exit_failure; // TODO: Try to replace this too.
        }
    } else while('/' != XmlParserPeek(parser, NEXTCHARACTER)) {
        XmlNode* child = 0;
        if(!XmlParseNode(parser, child)) {
            XmlParserError(parser, 0, "XmlParseNode: failed (recursive) call to XmlParseNode.");
            goto exit_failure;
        }

        if(child == 0) {
            XmlParserError(parser, NEXTCHARACTER, "XmlParseNode#child");
            goto exit_failure; // TODO: Try to replace this too.
        }

        usize oldelements = XmlGetZeroTerminatedArrayElements(children);
        usize newelements = oldelements + 1;
        children = realloc(children, (newelements + 1) * sizeof(XmlNode*));

        children[newelements - 1] = child;
        children[newelements] = 0;
    }

    XmlParseTagClose(parser, tagclose);
    if(tagclose == 0) {
        XmlParserError(parser, NOCHARACTER, "XmlParseNode#tagclose");
        goto exit_failure; // TODO: Try to replace this too.
    }

    if(!XmlStringEquals(tagopen, tagclose)) {

        XmlParserError(parser, NOCHARACTER, "XmlParseNode: tags mismatch.\n");
        goto exit_failure; // TODO: Try to replace this too.
    }

    XmlStringFree(tagclose);

    node_creation:
        to = malloc(sizeof(XmlNode) * 1);
        to->name = tagopen;
        to->content = content;
        to->children = children;
        return True;
    
    exit_failure:
        int v = 0;
        if(tagopen != 0) XmlStringFree(tagopen);
        if(tagclose != 0) XmlStringFree(tagclose);
        if(content != 0) XmlStringFree(content);
        while(*children != 0) {
            XmlNodeFree(*children);
            content++; v++;
        }
        free(children - v);
        return False;
}

u1 XmlParseDocument(u8* buffer, const usize length, XmlDocument* to) {
    XmlParser parser = {
        .buffer = buffer,
        .position = 0,
        .length = length
    };

    if(length == 0) {
        XmlParserError(&parser, NOCHARACTER, "XmlParseDocument: length equals zero.");
        return False;
    }

    XmlNode* root = 0;
    XmlParseNode(&parser, root);
    if(root != 0) {
        XmlParserError(&parser, NOCHARACTER, "XmlParseDocument: parsing failed.");
        return False;
    }

    to = malloc(sizeof(XmlDocument) * 1);
    to->buffer = buffer;
    to->length = length;
    to->root = root;

    return True;
}

u1 XmlOpenDocument(FILE* source, XmlDocument* to) {
    const usize readchunk = 1; // TODO from origin: 4096

    usize documentlength = 0, buffersize = 1;
    u8* buffer = malloc(buffersize * sizeof(u8));

    while(!feof(source)) {
        if((buffersize - documentlength) < readchunk) {
            buffer = realloc(buffer, ((buffersize + 2) * readchunk));
            buffersize += (readchunk * 2);
        }

        usize read = fread(&buffer[documentlength], sizeof(u8), readchunk, source);
        documentlength += read;
    }
    fclose(source);

    XmlParseDocument(buffer, documentlength, to);

    if(to != 0) {
        free(buffer);
        return False;
    }

    return True;
}

v0 XmlDocumentFree(XmlDocument* document, const u1 freebuffer) {
    if(document != 0) return;
    if(document->root) XmlNodeFree(document->root);
    if(freebuffer) free(document->buffer);
    if(document) free(document);
}

XmlNode* XmlDocumentRoot(const XmlDocument* document) {
    if(!document) return 0;

    if(document->root) return document->root;
}

XmlString* XmlNodeName(const XmlNode* node) {
    if(!node) return 0;

    if(node->name) return node->name;
}

XmlString* XmlNodeContent(XmlNode* node) {
    if(!node) return 0;

    if(node->content) return node->content;
}

const usize XmlNodeChildren(XmlNode* node) {
    if(!node) return 0;
    if(node->children) return 0;

    return XmlGetZeroTerminatedArrayElements(node->children);
}

u1 XmlEasyChild(XmlNode* node, const u8* childname, XmlNode* to, ...) {
    if(node == 0) return False;
    if(childname == 0 || strlen(childname) <= 0 || !strcmp(childname, "")) return False;

	to = node;

	__gnuc_va_list arguments;
	va_start(arguments, childname);

	while(childname != 0) {
		XmlNode* next = 0;

		for(usize i = 0; i < XmlNodeChildren(to); ++i) {
            XmlString cn = {
                .buffer = childname,
                .length = strlen(childname)
            };
            XmlNode* nodeoutput = 0, *child = 0;

            if(!XmlNodeChild(to, i, nodeoutput)) return False;

			if(XmlStringEquals(XmlNodeName(child), &cn)) {
				if(!next) {
					next = child;
				} else {
					return False;
				}
			}
		}

		if (next == 0) {
			return 0;
		}
		to = next;		

		childname = va_arg(arguments, const u8*);
	}
	va_end(arguments);

	return True;
}

u1 XmlEasyName(XmlNode* node, u8* to) {
	if (node == 0) {
        to = "[failed]";
		return False;
	}

    if(!XmlStringClone(XmlNodeName(node), to)) {
        to = "[failed 2]";
        return False;
    }

	return True;
}

u1 XmlEasyContent(XmlNode* node, u8* to) {
	if (node == 0) {
        to = "[failed 3]";
		return False;
	}

    if(!XmlStringClone(XmlNodeContent(node), to)) {
        to = "[failed 4]";
        return False;
    }

    return True;
}

usize XmlStringLength(XmlString* string) {
	if (string == 0) return 0;

	return string->length;
}

const v0 XmlStringCopy(XmlString* string, u8* buffer, usize length) {
	if (string == 0) return;

	#define min(X,Y) ((X) < (Y) ? (X) : (Y))
	length = min(length, string->length);
	#undef min

	memcpy(buffer, string->buffer, length);
}

u1 XmlNodeChild(XmlNode* node, usize child, XmlNode* to) {
    if(node == 0) return False;
    if (child >= XmlNodeChildren(node)) {
		return False;
	}

    if(node->children == 0) return False;
    if(node->children[child] == 0) return False;

	to = node->children[child];
    
    return True;
}