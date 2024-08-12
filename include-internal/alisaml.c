#pragma once

#include "./types.h"

#include "./alisaml.h"

#include "../include-external/ooxi-xml.c/xml.c"

#include <raylib.h>

#include <string.h>


static u1 allocateString(i8* to, const usize size) {
    if(size > 0) {
        printf("Borrowing %zu bytes for allocation, i hope you don't mind it. :D\n", size);
        
        to = (i8*) malloc(sizeof(i8) * size + 2);
        memset(to, 0, size);
    } else {
        return 0;
    }
}

AlisamlCtx initAlisaml(const u8* src) {
    AlisamlCtx ctx = {
        .tag_buffer_sz = 0,
        .canvas = {
            .roll_x = 0,
            .roll_y = 0,
            .res_x = 0,
            .res_y = 0
        },
        .seq = 0,
        .ctxmemusage = 0
    };


    // this.source = malloc(sizeof(u8) * (strlen(src) + 2));
    allocateString(ctx.source, strlen(src) + 2);
    ctx.document = xml_parse_document(src, strlen(src));
    if(ctx.document == 0) {
        printf("XML context init failed.\n");
        printf("XML context it's critical for the application. Aborting.\n");
        puts("Aborted (urmom overflow)");
        exit(2);
    }
    ctx.rootElement = xml_document_root(ctx.document);

    if(xml_node_children(ctx.rootElement) <= 1) {
        printf("XML source it's non-valid.\n");
        destroyAlisaml(&ctx);
        exit(2);
    }

    // Prepare tag array (allocating one by default)
    // NOTE(11.8.24): i'm unsure about this, fix this later.
    ctx.tag_buffer = realloc(ctx.tag_buffer, 1);
    ctx.tag_buffer_sz++;

    // Pick resolutions from XML
    u64 tempseq = ctx.seq;
    i32 result = 0;
    AlisamlTag tag;
    while(strcmp((tag = alisaml_gettagfromrootbysequence(&ctx, &result)).name, "AlisaContext")) {
        AlisamlTag nested;
        if(!strcmp(tag.name, "AlisaContext")) {
            u32 sx = 0, sy = 0;
            char cx[12] = {0}, cy[12] = {0};
            nested = alisaml_getsubtagfromsupertagbysequence(&ctx, &tag, &result);
            if(!strcmp(nested.name, "ScreenResolution")) {
                alisamlctx_pushtag(&ctx, &nested);
                printf("---Resolution: %s---\n", nested.value);

                i32 _ = 0, __ = 0;
                while(nested.value[_++] != 'x') {
                    cx[__++] = nested.value[_]; 
                }
                cx[__] = '\0';
                __ = 0;

                while(nested.value[_++] != 'x') {
                    cy[__++] = nested.value[_]; 
                }
                cy[__] = '\0';
                sx = atoi(cx); sy = atoi(cy);
            } else if(!strcmp(nested.name, "ScreenTitle")) {
                alisamlctx_pushtag(&ctx, &nested);
                allocateString(ctx.title, strlen(nested.value) + 2);
                // this.title = malloc(sizeof(char) * strlen(nested.value) + 1);
                printf("---Title: %s---\n", nested.value);
                strcpy(ctx.title, nested.value);
            } else if(!strcmp(nested.name, "ScreenTargetFPS")) {
                alisamlctx_pushtag(&ctx, &nested);
                printf("---Target fps: %s---\n", nested.value);
                ctx.targetfps = atoi(nested.value);
            }
        } else {
            printf("Tag <%s></%s> inside <AlisaContext></AlisaContext> is not allowed, ignoring.\n", nested.name, nested.name);
        }
        result = 0;
    }

    if(result != 0) {
        printf("Tag <AlisaContext></AlisaContext> wasn't specified and it's needed.\n");
        destroyAlisaml(&ctx);
        exit(2);
    }

    printf("Context started.\n");

    return ctx;
}

const v0 destroyAlisaml(AlisamlCtx* ctx) {
    printf("CTX memory usage in bytes: %zu.\n", alisamlctx_measureusage(ctx));

    if(ctx->tag_buffer_sz > 0) for(i32 i = 0; ctx->tag_buffer_sz > 0 && i <= ctx->tag_buffer_sz; i++) {
        alisaml_destroytag(ctx, &ctx->tag_buffer[i]);
    }

    if(ctx->title != 0) free(ctx->title);
    free(ctx->source);
    xml_document_free(ctx->document, false);
    ctx->rootElement = 0;
    free(ctx->tag_buffer);
    ctx->tag_buffer_sz = 0;

    printf("Context discarded.\n");

    CloseWindow();

    return;
}

AlisamlTag alisaml_gettagfromrootbysequence(AlisamlCtx* ctx, i32* result) {
    AlisamlTag ret;
    u64 toalloc = 0;

    ret.node = xml_node_child(ctx->rootElement, ctx->seq++);
    if(ret.node == 0) *result = -69;

    ret.inner_name = xml_node_name(ret.node);
    toalloc = sizeof(u8) * xml_string_length(ret.inner_name) + 2;
    // this.name = malloc(toalloc);
    allocateString(ret.name, toalloc);
    xml_string_copy(ret.inner_name, ret.name, xml_string_length(ret.inner_name));
    ret.inner_value = xml_node_content(ret.node);
    ctx->ctxmemusage += toalloc;

    toalloc = sizeof(u8) * xml_string_length(ret.inner_value) + 2;
    // this.value = malloc(toalloc);
    allocateString(ret.value, toalloc);
    xml_string_copy(ret.inner_value, ret.value, xml_string_length(ret.inner_value));
    ret.reserved = 0;
    ctx->ctxmemusage += toalloc;

    return ret;
}

const v0 alisaml_destroytag(AlisamlCtx* ctx, AlisamlTag* tag) {
    free(tag->name);
    free(tag->value);
    xml_string_free(tag->inner_name);
    xml_string_free(tag->inner_value);
    tag->reserved = 0;
}

const u1 alisaml_parsetag(AlisamlCtx* ctx, AlisamlTag* tag) {
    AlisamlTagNameEnum tagkind = alisaml_enum_strtotagkind(tag->name);

    switch(tagkind) {
        case AML_TAG_TEST: {
            if(!alisamlctx_pushtag(ctx, tag)) {
                printf("Failed to push tag %s.\n", tag->name);
                return False;
            }

            // This tag has no effects
        } break;

        case AML_TAG_TEXT: {
            if(!alisamlctx_pushtag(ctx, tag)) {
                printf("Failed to push tag %s.\n", tag->name);
                return False;
            }

            // This tag has effects
            return alisaml_texttag_commit(ctx, *tag);
        } break;

        case AML_TAG_ALISACONTEXT:
        case AML_TAG_SCREENRESOLUTION:
        case AML_TAG_SCREENTITLE:
        case AML_TAG_SCREENTARGETFPS:
        case AML_TAG_HEAD:
        case AML_TAG_BODY:
        case AML_TAG_META:

        // This is already parsed by the initAlisaml function.
        case AML_TAG_ALISAML:
        default: {
            printf("Unimplemented tag parsing: %s.\n", tag->name);
            return False;
        } break;
    }
}

const u8* alisaml_enum_tagkindtostr(AlisamlTagNameEnum a) {
    switch(a) {
        #define alisaml_enum_tagkindtostr_incase(e, v) case e: { return v; } break;

        alisaml_enum_tagkindtostr_incase(AML_TAG_ALISAML, "Alisaml")
        alisaml_enum_tagkindtostr_incase(AML_TAG_TEST, "Test")
        alisaml_enum_tagkindtostr_incase(AML_TAG_ALISACONTEXT, "AlisaContext")
        alisaml_enum_tagkindtostr_incase(AML_TAG_SCREENRESOLUTION, "ScreenResolution")
        alisaml_enum_tagkindtostr_incase(AML_TAG_SCREENTITLE, "ScreenTitle")
        alisaml_enum_tagkindtostr_incase(AML_TAG_SCREENTARGETFPS, "ScreenTargetFPS")
        alisaml_enum_tagkindtostr_incase(AML_TAG_HEAD, "head")
        alisaml_enum_tagkindtostr_incase(AML_TAG_BODY, "body")
        alisaml_enum_tagkindtostr_incase(AML_TAG_META, "meta")
        alisaml_enum_tagkindtostr_incase(AML_TAG_TEXT, "Text")

        default: {
            return "<unknown>";
        } break;
    }

    return "<unreachable>";
}

const AlisamlTagNameEnum alisaml_enum_strtotagkind(const u8* a) {
    if(!strcmp(a, "Alisaml")) { return AML_TAG_ALISAML; }
    else if(!strcmp(a, "Test")) { return AML_TAG_TEST; }
    else if(!strcmp(a, "AlisaContext")) { return AML_TAG_ALISACONTEXT; }
    else if(!strcmp(a, "ScreenResolution")) { return AML_TAG_SCREENRESOLUTION; }
    else if(!strcmp(a, "ScreenTitle")) { return AML_TAG_SCREENTITLE; }
    else if(!strcmp(a, "ScreenTargetFPS")) { return AML_TAG_SCREENTARGETFPS; }
    else if(!strcmp(a, "head")) { return AML_TAG_HEAD; }
    else if(!strcmp(a, "body")) { return AML_TAG_BODY; }
    else if(!strcmp(a, "meta")) { return AML_TAG_META; }
    else if(!strcmp(a, "Text")) { return AML_TAG_TEXT; }
}

const u1 alisamlctx_pushtag(AlisamlCtx* ctx, const AlisamlTag* tag) {
    ctx->tag_buffer[ctx->tag_buffer_sz++] = *tag;
    ctx->ctxmemusage += sizeof(AlisamlTag);
    return ctx->tag_buffer[ctx->tag_buffer_sz].reserved_0 == 69;
}

const AlisamlTag alisamlctx_poptag(AlisamlCtx* ctx) {
    // TODO(11.8.24): Copy strings instead of pointers.

    AlisamlTag ret;
    AlisamlTag* result = &ctx->tag_buffer[ctx->tag_buffer_sz--];
    ret.inner_name = result->inner_name;
    ret.inner_value = result->inner_value;
    ret.reserved_0 = result->reserved_0;
    ret.reserved = result->reserved;
    ret.node = result->node;
    ret.name = result->name;
    ret.value = result->value;

    ctx->ctxmemusage -= sizeof(*result);

    return ret;
}

const u1 alisaml_texttag_commit(AlisamlCtx* ctx, AlisamlTag tag) {
    const i32 DEFAULT_TEXTSIZE = 20;
    const Color DEFAULT_TEXTCOLOUR = { 0x0a, 0x0a, 0x0a, 0xff };

    DrawText(tag.value, ctx->canvas.roll_x, ctx->canvas.roll_y, DEFAULT_TEXTSIZE, DEFAULT_TEXTCOLOUR);

    ctx->canvas.roll_x =
      ctx->canvas.roll_x + 20 <= GetScreenWidth()
        ? ctx->canvas.roll_x + 20
        : ctx->canvas.roll_x
    ;

    ctx->canvas.roll_y =
      ctx->canvas.roll_y + 20 <= GetScreenWidth()
        ? ctx->canvas.roll_y + 20
        : ctx->canvas.roll_y
    ;

    return True;
}

const u64 alisamlctx_measureusage(AlisamlCtx* ctx) {
    printf("Total tags stored: %zu.\n", ctx->tag_buffer_sz);
    u64 usage = sizeof(AlisamlCtx);

    // canvas field
    usage += sizeof(usize) * 2;
    usage += sizeof(u32) * 2;

    // Get tag total size
    if(ctx->tag_buffer_sz > 0) for(i32 i = 0; i <= ctx->tag_buffer_sz; i++) {
        usage += sizeof(usize);
        usage += sizeof(v0*);
    }

    usage += strlen(ctx->source);
    usage += sizeof(usize) * 3;

    usage += strlen(ctx->title);
    usage += sizeof(u32);

    return usage;
}

const u1 alisamlctx_parsesource(AlisamlCtx* ctx) {
    i32 result = 0;
    AlisamlTag tag = alisaml_gettagfromrootbysequence(ctx, &result);
    if(result != -69) {
        if(!alisamlctx_pushtag(ctx, &tag)) return False;

        if(alisaml_parsetag(ctx, &tag)) return False;
    } 

    return True;
}

AlisamlTag alisaml_getsubtagfromsupertagbysequence(AlisamlCtx* ctx, AlisamlTag* super, i32* result) {
    AlisamlTag this;
    u64 toalloc = 0;
    this.node = xml_node_child(super->node, super->seq++);
    if(this.node == 0) *result = -68;

    this.inner_name = xml_node_name(this.node);
    toalloc = sizeof(u8) * xml_string_length(this.inner_name) + 2;
    // this.name = malloc(toalloc);
    allocateString(this.name, toalloc + 2);
    xml_string_copy(this.inner_name, this.name, xml_string_length(this.inner_name));
    this.inner_value = xml_node_content(this.node);
    ctx->ctxmemusage += toalloc;

    toalloc = sizeof(u8) * xml_string_length(this.inner_value) + 2;
    // this.value = malloc(toalloc);
    allocateString(this.name, toalloc + 2);
    xml_string_copy(this.inner_value, this.value, xml_string_length(this.inner_value));
    this.reserved = 0;
    ctx->ctxmemusage += toalloc;

    return this;
}