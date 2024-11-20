#include "Archive.h"
#include "Diagnostic.h"
#include "FileIO.h"

typedef enum {
    TokenEndOfFile = 128,
    TokenIdentifier,
    TokenError,
} Token;

typedef struct {
    ArchiveRef Archive;
    Int32 NodeIndex;
    Char* Start;
    Char* End;
    Char* Cursor;
    Token Token;
    Char* TokenStart;
    Char* TokenEnd;
    Token NextToken;
    Char* NextTokenStart;
    Char* NextTokenEnd;
    Char* NameStart;
    Char* NameEnd;
    Bool NoLinebreak;
    Bool NoPeekNext;
    Bool NoCascade;
} ParseState;

static inline Void ParseNextToken(
    ParseState* State
);

static FORCE_INLINE Bool CharacterIsSymbol(
    Char Character
) {
    switch (Character) {
    case '<':
    case '>':
    case '=':
    case '"':
    case '/':
    case '!':
    case '?':
        return true;

    default:
        return false;
    }
}

static FORCE_INLINE Bool CharacterIsStartOfIdentifier(
    Char Character
) {
    return (
        ('a' <= Character && Character <= 'z') ||
        ('A' <= Character && Character <= 'Z') ||
        '_' == Character
        );
}

static FORCE_INLINE Bool CharacterIsContinuationOfIdentifier(
    Char Character
) {
    return (
        ('a' <= Character && Character <= 'z') ||
        ('A' <= Character && Character <= 'Z') ||
        ('0' <= Character && Character <= '9') ||
        '_' == Character
    );
}

static FORCE_INLINE Bool CharacterIsWhitespace(
    Char Character
) {
    return (Character == 0x09 || Character == 0x20);
}

static FORCE_INLINE Bool CharacterIsLinebreak(
    Char Character
) {
    return (
        Character == 0x0A ||
        Character == 0x0B ||
        Character == 0x0C ||
        Character == '\r'
    );
}

static FORCE_INLINE Void SkipUntilCharacter(
    ParseState* State,
    Char Terminator
) {
    if (*State->TokenStart == Terminator)
        return;

    while (State->Cursor < State->End) {
        if (State->NoLinebreak && CharacterIsLinebreak(*State->Cursor)) {
            break;
        }

        if (*State->Cursor == Terminator) {
            break;
        }

        State->Cursor += 1;
    }

    ParseNextToken(State);
}

static FORCE_INLINE Bool SkipSeparator(
    ParseState* State
) {
    Char* Start = State->Cursor;

    while (State->Cursor < State->End) {
        if (!(CharacterIsWhitespace(*State->Cursor)) &&
            !(CharacterIsLinebreak(*State->Cursor) && !State->NoLinebreak)) {
            break;
        }

        State->Cursor += 1;
    }

    return Start != State->Cursor;
}

static FORCE_INLINE Bool ConsumeToken(ParseState* State, Token Token) {
    if (State->Token == Token) {
        State->Cursor = State->TokenEnd;
        ParseNextToken(State);
        return true;
    }

    return false;
}

static inline Void ParseNextToken(
    ParseState* State
) {
    SkipSeparator(State);

    if (State->Cursor >= State->End) {
        State->Token = TokenEndOfFile;
        State->TokenStart = State->Cursor;
        State->TokenEnd = State->Cursor + 1;
        goto end;
    }

    if (CharacterIsSymbol(*State->Cursor)) {
        State->Token = (Token)*State->Cursor;
        State->TokenStart = State->Cursor;
        State->TokenEnd = State->Cursor + 1;
        State->Cursor += 1;
        goto end;
    }

    if (CharacterIsStartOfIdentifier(*State->Cursor)) {
        Char* Start = State->Cursor;
        do {
            State->Cursor += 1;
        } while (CharacterIsContinuationOfIdentifier(*State->Cursor) && State->Cursor < State->End);

        State->Token = TokenIdentifier;
        State->TokenStart = Start;
        State->TokenEnd = State->Cursor;
        goto end;
    }

    State->Token = TokenError;
    State->TokenStart = State->Cursor;
    State->TokenEnd = State->Cursor + 1;
    goto end;

end:
    if (!State->NoPeekNext) {
        ParseState Copy = *State;
        Copy.NoPeekNext = true;
        ParseNextToken(&Copy);
        State->NextToken = Copy.Token;
        State->NextTokenStart = Copy.TokenStart;
        State->NextTokenEnd = Copy.TokenEnd;
    }
}

static FORCE_INLINE Bool TryParseAttribute(
    ParseState* State,
    Bool Append
) {
    ParseState CurrentState = *State;
    Char* AttributeStart = CurrentState.TokenStart;
    Char* AttributeEnd = CurrentState.TokenEnd;

    if (!ConsumeToken(&CurrentState, TokenIdentifier)) {
        // NOTE: We just allow to use empty identifiers so invalid file structures still parse well
        if (*AttributeStart == '=' && State->NameStart != NULL) {
            AttributeStart = CurrentState.NameStart;
            AttributeEnd = CurrentState.NameEnd;
        } else {
            goto error;
        }
    }

    if (!ConsumeToken(&CurrentState, '=')) goto error;

    Char* LiteralStart = CurrentState.TokenStart;
    Bool NoLinebreak = CurrentState.NoLinebreak;
    CurrentState.NoLinebreak = true;

    if (!ConsumeToken(&CurrentState, '"')) goto error;

    SkipUntilCharacter(&CurrentState, '"');

    Char* LiteralEnd = CurrentState.TokenEnd;

    if (!ConsumeToken(&CurrentState, '"')) goto error;

    CurrentState.NoLinebreak = NoLinebreak;
    /*
    ParseState NextState = CurrentState;
    NextState.NoCascade = true;
    if (!TryParseAttribute(&NextState, false)) {
        if (CurrentState.Token == '>' ||
            (CurrentState.Token == '/' && CurrentState.NextToken == '>')) {
            if (Append) {
                ArchiveNodeAddAttribute(
                    CurrentState.Archive,
                    CurrentState.NodeIndex,
                    AttributeStart,
                    (Int32)(AttributeEnd - AttributeStart),
                    LiteralStart + 1,
                    (Int32)(LiteralEnd - LiteralStart - 2)
                );
            }

            *State = CurrentState;
            return true;
        }

        NoLinebreak = CurrentState.NoLinebreak;
        CurrentState.NoLinebreak = true;

        SkipUntilCharacter(&CurrentState, '"');

        LiteralEnd = CurrentState.TokenEnd;

        if (!ConsumeToken(&CurrentState, '"')) goto error;

        CurrentState.NoLinebreak = NoLinebreak;
    }
    */
    if (Append) {
        ArchiveNodeAddAttribute(
            CurrentState.Archive,
            CurrentState.NodeIndex,
            AttributeStart,
            (Int32)(AttributeEnd - AttributeStart),
            LiteralStart + 1,
            (Int32)(LiteralEnd - LiteralStart - 2)
        );
    }

    *State = CurrentState;
    return true;

error:
    return false;
}

Bool ArchiveParseFromSource(
    ArchiveRef Archive,
    CString Source,
    Int32 Length,
    Bool IgnoreErrors
) {
    ParseState State = { 0 };
    State.Start = Source;
    State.End = Source + Length;
    State.Cursor = Source;
    State.Archive = Archive;
    State.NodeIndex = -1;

    ParseNextToken(&State);

    while (State.Token != TokenEndOfFile && State.Token != TokenError) {
        if (!ConsumeToken(&State, '<')) {
            if (!IgnoreErrors) Error("Expected '<' found '%c'", *State.Cursor);
            goto error;
        }

        // <!> 
        if (ConsumeToken(&State, '!')) {
            SkipUntilCharacter(&State, '>');

            if (!ConsumeToken(&State, '>')) {
                if (!IgnoreErrors) Error("Expected '>' found '%c'", *State.Cursor);
                goto error;
            }

            continue;
        }

        // <?> 
        if (ConsumeToken(&State, '?')) {
            SkipUntilCharacter(&State, '>');

            if (!ConsumeToken(&State, '>')) {
                if (!IgnoreErrors) Error("Expected '>' found '%c'", *State.Cursor);
                goto error;
            }

            continue;
        }

        if (ConsumeToken(&State, '/')) {
            State.NameStart = State.TokenStart;
            State.NameEnd = State.TokenEnd;

            if (!ConsumeToken(&State, TokenIdentifier)) {
                if (!IgnoreErrors) Error("Expected <identifier> found '%c'", *State.Cursor);
                goto error;
            }

            if (!ConsumeToken(&State, '>')) {
                if (!IgnoreErrors) Error("Expected '>' found '%c'", *State.Cursor);
                goto error;
            }

            if (State.NodeIndex < 0)
                goto error;

            ArchiveStringRef Name = ArchiveNodeGetName(Archive, State.NodeIndex);
            if (!IgnoreErrors && memcmp(Name->Data, State.NameStart, State.NameEnd - State.NameStart) != 0)
                goto error;

            State.NodeIndex = ArchiveNodeGetParent(Archive, State.NodeIndex);
            continue;
        }

        State.NameStart = State.TokenStart;
        State.NameEnd = State.TokenEnd;
        State.NodeIndex = ArchiveAddNode(Archive, State.NodeIndex, State.NameStart, (Int32)(State.NameEnd - State.NameStart));

        if (!ConsumeToken(&State, TokenIdentifier)) 
            goto error;

        if (State.Token == '=') {
            if (!TryParseAttribute(&State, true))
                goto error;
        }

        while (State.Token == TokenIdentifier) {
            if (!TryParseAttribute(&State, true)) 
                goto error;
        }

        if (ConsumeToken(&State, '>')) {
            ParseState CurrentState = State;
            if (ConsumeToken(&CurrentState, '/') && ConsumeToken(&CurrentState, '/')) {
                SkipUntilCharacter(&CurrentState, '\n');
                State = CurrentState;
            }

            continue;
        }

        ParseState CurrentState = State;
        if (ConsumeToken(&CurrentState, '/') && ConsumeToken(&CurrentState, '>')) {
            if (CurrentState.NodeIndex < 0)
                goto error;

            CurrentState.NodeIndex = ArchiveNodeGetParent(Archive, CurrentState.NodeIndex);
            State = CurrentState;
            continue;
        }

        goto error;
    }

    if (!IgnoreErrors && State.Token == TokenError) {
        Error("Expected '<' found '%c'", *State.Cursor);
    }

    return true;

error:
    return false;
}
