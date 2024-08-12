#pragma once

#include "../include-external/ooxi-xml.c/xml.h"

#include "./types.h"

#include <string.h>



// This represents a tag
typedef struct alisamlTagObject {
    usize reserved_0;
    struct xml_node*   node;
    struct xml_string* inner_name;
    u8*                name;
    struct xml_string* inner_value;
    u8*                value;
    u1                 amContainer;
    usize              seq;
    void*              reserved; // This will be for tag attributes
} AlisamlTag;

// This is the context object (xml parsing and xml subset)
typedef struct alisamlCtxObject {
    u8*                  source;
    struct xml_document* document;
    struct xml_node*     rootElement;
    AlisamlTag*          tag_buffer;
    usize                tag_buffer_sz;
    u8*                  title;
    u32                  targetfps;

    usize                seq;
    usize                ctxmemusage;

    struct alusamlCtxNestedObjectCanvas {
        u32 res_x, res_y;
        i32 roll_x, roll_y;
    } canvas;
} AlisamlCtx;

// Enumerator for all tag kinds
typedef enum alisamlTagNameEnumList {
    AML_TAG_ALISAML,
    AML_TAG_TEST,
    AML_TAG_ALISACONTEXT,
    AML_TAG_SCREENRESOLUTION,
    AML_TAG_SCREENTITLE,
    AML_TAG_SCREENTARGETFPS,
    AML_TAG_HEAD,
    AML_TAG_BODY,
    AML_TAG_META,
    AML_TAG_TEXT,
} AlisamlTagNameEnum;

// ### This inits the context object and returns it.
// Receives:
// - const u8* src -> The Alisaml source as string.
//
// Returns: AlisamlCtx -> The context object.
AlisamlCtx initAlisaml(const u8* src);

// ### This destroys the context.
// Receives:
// - AlisamlCtx* ctx -> The context object.
//
// Returns: nothing xd
const v0 destroyAlisaml(AlisamlCtx* ctx);

// ### This gets the next tag on the next line (by order)
// Receives:
// - AlisamlCtx* ctx -> The context object.
//
// Returns: AlisamlTag -> A tag object
AlisamlTag alisaml_gettagfromrootbysequence(AlisamlCtx* ctx, i32* result);

AlisamlTag alisaml_getsubtagfromsupertagbysequence(AlisamlCtx* ctx, AlisamlTag* tag, i32* result);

// ### This destroys a tag object
// Receives:
// - AlisamlCtx* ctx -> The context object.
// - AlisamlTag* tag -> The tag object to destroy.
//
// Returns: nothing xd
const v0 alisaml_destroytag(AlisamlCtx* ctx, AlisamlTag* tag);

// ### This parses and executes or save any behaviour a tag carries on
// Receives:
// - AlisamlCtx* ctx -> The context object
// - AlisamlTag* tag -> The spoken tag
//
// Returns: u1 -> If parsing the tag failed or succeded
const u1 alisaml_parsetag(AlisamlCtx* ctx, AlisamlTag* tag);

// ### This gets an enumeration and returns its string version.
// Receives:
// - AlisamlTagNameEnum a -> The enum
//
// Returns: const u8* -> The string belonging to that enum.
const u8* alisaml_enum_tagkindtostr(AlisamlTagNameEnum a);

// ### This gets a string and returns its enum version.
// Receives:
// - const u8* a -> The string
//
// Returns: const AlisamlTagNameEnum -> The enum belonging to that string.
const AlisamlTagNameEnum alisaml_enum_strtotagkind(const u8* a);

// ### This pushes the given tag object to the Alisaml context object.
// Receives:
// - AlisamlCtx* ctx -> Alisaml context object
// - const AlisamlTag* tag -> The tag to push
//
//Returns: u1 -> A boolean telling if succeded or not.
const u1 alisamlctx_pushtag(AlisamlCtx* ctx, const AlisamlTag* tag);

// ### This does the opposite to `alisamlctx_pushtag`, pops instead of pushing.
// Receives:
// - AlisamlCtx* ctx -> Alisaml context object
//
// Returns: AlisamlTag -> the popped tag object.
const AlisamlTag alisamlctx_poptag(AlisamlCtx* ctx);

// #### This is related to tag behaviours.
// ### This performs the text tag behaviour.
// Receives:
// - AlisamlCtx* ctx -> Alisaml context object
// - AlisamlTag tag -> The mentioned tag object.
//
// Returns: u1 -> Succeded or not (as a boolean)
const u1 alisaml_texttag_commit(AlisamlCtx* ctx, AlisamlTag tag);

// ### This calculates how much memory in bytes does the context object use.
// Receives:
// - AlisamlCtx* ctx -> Alisaml context object
//
// Returns: u64 -> The total usage
const u64 alisamlctx_measureusage(AlisamlCtx* ctx);

// ## This is stupidly prone to give shy sigsegv's.
// ### I'm already bored to document this, i'll do it later.
const u1 alisamlctx_parsesource(AlisamlCtx* this);

// #### This should be hidden, but this is C bitch.
// ### This allocates memory for a string filling with zeroes meanwhile.
// Receives:
// - i8* to -> The pointer to malloc.
// - const usize size -> How much do you want to allocate?
//
// Returns: u1 -> A boolean telling if succeded or failed
//
// Notes:
// - The reason to pass the pointer instead of returning one its segv security.
// - The developer should change u1 to v0 because they ignore the boolean anyway.
static u1 allocateString(i8* to, const usize size);