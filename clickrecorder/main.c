//
//  main.c
//  clickrecorder
//
//  Created by jens on 04.06.15.
//  Copyright (c) 2015 lea.io. All rights reserved.
//
// as of: https://danielbeard.wordpress.com/2010/10/29/listening-for-global-keypresses-in-osx/

#include <stdio.h>
#include <ApplicationServices/ApplicationServices.h>

#include <Security/Authorization.h>

#include <Security/AuthorizationTags.h>

//
//
//int read (long,StringPtr,int);
//
//int write (long,StringPtr,int);



CGEventRef
myCGEventCallback(CGEventTapProxy proxy, CGEventType type,
                  CGEventRef event, void *refcon)
{
  // Paranoid sanity check.
//  if ((type != kCGEventLeftMouseDown) && (type != kCGEventLeftMouseUp))
//    return event;
  
  int upDown = 0;
  upDown = type == kCGEventLeftMouseDown | type == kCGEventRightMouseDown ? 1 : 0;
//  upDown = type = kCGEventLeftMouseUp | type == kCGEventRightMouseUp ? 0 : 1;
  
  // The incoming keycode.
  // CGKeyCode keycode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
  
  CGMouseButton button = (CGMouseButton)CGEventGetIntegerValueField(event, kCGMouseEventButtonNumber);
  int clickState = (int)CGEventGetIntegerValueField(event, kCGMouseEventClickState);
  
  CGEventTimestamp time = CGEventGetTimestamp(event);
  
  //Keypress code goes here.
  printf("button %llu\t%u, %i, %i, %i\n", time, type, button, clickState, upDown);
  
  // We must return the event for it to be useful.
  return event;
}

int
run(void)
{
  CFMachPortRef      eventTap;
  CGEventMask        eventMask;
  CFRunLoopSourceRef runLoopSource;
  
  // Create an event tap. We are interested in key presses.
  //  eventMask = ((1 << kCGEventKeyDown) | (1 << kCGEventKeyUp));
  eventMask = ((1 << kCGEventLeftMouseDown) | (1 << kCGEventRightMouseDown) | (1 << kCGEventLeftMouseUp) | (1 << kCGEventRightMouseUp));

  eventTap = CGEventTapCreate(
                              kCGSessionEventTap,
                              kCGTailAppendEventTap,
                              kCGEventTapOptionDefault,
                              eventMask, myCGEventCallback, NULL);
  if (!eventTap) {
    fprintf(stderr, "failed to create event tap\n");
    exit(1);
  }
  
  // Create a run loop source.
  runLoopSource = CFMachPortCreateRunLoopSource(
                                                kCFAllocatorDefault, eventTap, 0);
  
  // Add to the current run loop.
  CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource,
                     kCFRunLoopCommonModes);
  
  // Enable the event tap.
  CGEventTapEnable(eventTap, true);
  
  // Set it all running.
  CFRunLoopRun();
  
  exit(0);
}



int main() {
  Boolean isTrusted = false;
  if (AXIsProcessTrustedWithOptions != NULL) {
    // 10.9 and later
    const void * keys[] = { kAXTrustedCheckOptionPrompt };
    const void * values[] = { kCFBooleanTrue };
    
    CFDictionaryRef options = CFDictionaryCreate(
                                                 kCFAllocatorDefault,
                                                 keys,
                                                 values,
                                                 sizeof(keys) / sizeof(*keys),
                                                 &kCFCopyStringDictionaryKeyCallBacks,
                                                 &kCFTypeDictionaryValueCallBacks);
    
    isTrusted = AXIsProcessTrustedWithOptions(options);
  }
  
  printf("trusted? %i\n", isTrusted);
  
  run();
  
}