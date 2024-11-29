#include <Carbon/Carbon.h> /* For kVK_ constants, and TIS functions. */
#include "./CGSInternal/CGSHotKeys.h"

typedef enum {
    kMFSHMoveLeftASpace = 79,
    kMFSHMoveRightASpace = 81,
} MFSymbolicHotkey;


CG_EXTERN CGError CGSGetSymbolicHotKeyValue(CGSSymbolicHotKey hotKey, UniChar *outKeyEquivalent, UniChar *outVirtualKeyCode, CGSModifierFlags *outModifiers);
CG_EXTERN CGError CGSSetSymbolicHotKeyValue(CGSSymbolicHotKey hotKey, UniChar keyEquivalent, CGKeyCode virtualKeyCode, CGSModifierFlags modifiers);

CGEventFlags getModifierFlags(void) {
    CGEventRef flagEvent = CGEventCreate(NULL);
    CGEventFlags flags = CGEventGetFlags(flagEvent);
    CFRelease(flagEvent);
    return flags;
}

static void postKeyboardEventsForSymbolicHotkey(CGKeyCode keyCode, CGSModifierFlags modifierFlags) {
    CGEventTapLocation tapLoc = kCGSessionEventTap;
    
    // Create key events
    CGEventRef keyDown = CGEventCreateKeyboardEvent(NULL, keyCode, true);
    CGEventRef keyUp = CGEventCreateKeyboardEvent(NULL, keyCode, false);
    CGEventSetFlags(keyDown, (CGEventFlags)modifierFlags);
    CGEventFlags originalModifierFlags = getModifierFlags();
    CGEventSetFlags(keyUp, originalModifierFlags); // Restore original keyboard modifier flags state on key up. This seems to fix `[Modifiers getCurrentModifiers]`
    
    // Send key events
    CGEventPost(tapLoc, keyDown);
    CGEventPost(tapLoc, keyUp);
    
    CFRelease(keyDown);
    CFRelease(keyUp);
}

static void postSymbolicHotkey(MFSymbolicHotkey shk) {    
    UniChar keyEquivalent;
    CGKeyCode keyCode;
    CGSModifierFlags modifierFlags;
    CGSGetSymbolicHotKeyValue((CGSSymbolicHotKey)shk, &keyEquivalent, &keyCode, &modifierFlags);

    Boolean hotkeyIsEnabled = CGSIsSymbolicHotKeyEnabled((CGSSymbolicHotKey)shk);

    if (!hotkeyIsEnabled) {
        CGSSetSymbolicHotKeyEnabled((CGSSymbolicHotKey)shk, true);
    }

    postKeyboardEventsForSymbolicHotkey(keyCode, modifierFlags);
}

