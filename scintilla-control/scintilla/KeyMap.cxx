// Scintilla source code edit control
/** @file KeyMap.cxx 
 ** Defines a mapping between keystrokes and commands.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include "Platform.h"

#include "Scintilla.h"

#include "KeyMap.h"

KeyMap::KeyMap() : kmap(0), len(0), alloc(0) {
	for (int i = 0; MapDefault[i].key; i++) {
		AssignCmdKey(MapDefault[i].key, 
			MapDefault[i].modifiers,
             		MapDefault[i].msg);
	}
}

KeyMap::~KeyMap() {
	Clear();
}

void KeyMap::Clear() {
	delete []kmap;
	kmap = 0;
	len = 0;
	alloc = 0;
}

void KeyMap::AssignCmdKey(int key, int modifiers, unsigned int msg) {
	if ((len+1) >= alloc) {
		KeyToCommand *ktcNew = new KeyToCommand[alloc + 5];
		if (!ktcNew)
			return;
		for (int k=0;k<len;k++)
			ktcNew[k] = kmap[k];
		alloc += 5;
		delete []kmap;
		kmap = ktcNew;
	}
	for (int keyIndex = 0; keyIndex < len; keyIndex++) {
		if ((key == kmap[keyIndex].key) && (modifiers == kmap[keyIndex].modifiers)) {
			kmap[keyIndex].msg = msg;
			return;
		}
	}
	kmap[len].key = key;
	kmap[len].modifiers = modifiers;
	kmap[len].msg = msg;
	len++;
}

unsigned int KeyMap::Find(int key, int modifiers) {
	for (int i=0; i < len; i++) {
		if ((key == kmap[i].key) && (modifiers == kmap[i].modifiers)) {
			return kmap[i].msg;
		}
	}
	return 0;
}

KeyToCommand KeyMap::MapDefault[] = {
    {SCK_DOWN,		SCI_NORM,	SCI_LINEDOWN},
    {SCK_DOWN,		SCI_SHIFT,	SCI_LINEDOWNEXTEND},
    {SCK_DOWN,		SCI_CTRL,	SCI_LINESCROLLDOWN},
    {SCK_UP,			SCI_NORM,	SCI_LINEUP},
    {SCK_UP,			SCI_SHIFT,	SCI_LINEUPEXTEND},
    {SCK_UP,			SCI_CTRL,	SCI_LINESCROLLUP},
    {SCK_LEFT,		SCI_NORM,	SCI_CHARLEFT},
    {SCK_LEFT,		SCI_SHIFT,	SCI_CHARLEFTEXTEND},
    {SCK_LEFT,		SCI_CTRL,	SCI_WORDLEFT},
    {SCK_LEFT,		SCI_CSHIFT,	SCI_WORDLEFTEXTEND},
    {SCK_RIGHT,		SCI_NORM,	SCI_CHARRIGHT},
    {SCK_RIGHT,		SCI_SHIFT,	SCI_CHARRIGHTEXTEND},
    {SCK_RIGHT,		SCI_CTRL,	SCI_WORDRIGHT},
    {SCK_RIGHT,		SCI_CSHIFT,	SCI_WORDRIGHTEXTEND},
    {SCK_HOME, 		SCI_NORM, 	SCI_VCHOME},
    {SCK_HOME, 		SCI_SHIFT, 	SCI_VCHOMEEXTEND},
    {SCK_HOME, 		SCI_CTRL, 	SCI_DOCUMENTSTART},
    {SCK_HOME, 		SCI_CSHIFT, 	SCI_DOCUMENTSTARTEXTEND},
    {SCK_END,	 	SCI_NORM, 	SCI_LINEEND},
    {SCK_END,	 	SCI_SHIFT, 	SCI_LINEENDEXTEND},
    {SCK_END, 		SCI_CTRL, 	SCI_DOCUMENTEND},
    {SCK_END, 		SCI_CSHIFT, 	SCI_DOCUMENTENDEXTEND},
    {SCK_PRIOR,		SCI_NORM, 	SCI_PAGEUP},
    {SCK_PRIOR,		SCI_SHIFT, 	SCI_PAGEUPEXTEND},
    {SCK_NEXT, 		SCI_NORM, 	SCI_PAGEDOWN},
    {SCK_NEXT, 		SCI_SHIFT, 	SCI_PAGEDOWNEXTEND},
    {SCK_DELETE, 	SCI_NORM,	SCI_CLEAR},
    {SCK_DELETE, 	SCI_SHIFT,	SCI_CUT},
    {SCK_DELETE, 	SCI_CTRL,	SCI_DELWORDRIGHT},
    {SCK_INSERT, 		SCI_NORM,	SCI_EDITTOGGLEOVERTYPE},
    {SCK_INSERT, 		SCI_SHIFT,	SCI_PASTE},
    {SCK_INSERT, 		SCI_CTRL,	SCI_COPY},
    {SCK_ESCAPE,  	SCI_NORM,	SCI_CANCEL},
    {SCK_BACK,		SCI_NORM, 	SCI_DELETEBACK},
    {SCK_BACK,		SCI_SHIFT, 	SCI_DELETEBACK},
    {SCK_BACK,		SCI_CTRL, 	SCI_DELWORDLEFT},
    {SCK_BACK, 		SCI_ALT,	SCI_UNDO},
    {'Z', 			SCI_CTRL,	SCI_UNDO},
    {'Y', 			SCI_CTRL,	SCI_REDO},
    {'X', 			SCI_CTRL,	SCI_CUT},
    {'C', 			SCI_CTRL,	SCI_COPY},
    {'V', 			SCI_CTRL,	SCI_PASTE},
    {'A', 			SCI_CTRL,	SCI_SELECTALL},
    {SCK_TAB,		SCI_NORM,	SCI_TAB},
    {SCK_TAB,		SCI_SHIFT,	SCI_BACKTAB},
    {SCK_RETURN, 	SCI_NORM,	SCI_NEWLINE},
    {SCK_ADD, 		SCI_CTRL,	SCI_ZOOMIN},
    {SCK_SUBTRACT,	SCI_CTRL,	SCI_ZOOMOUT},
    {SCK_DIVIDE,	SCI_CTRL,	SCI_SETZOOM},
    //'L', 			SCI_CTRL,		SCI_FORMFEED,
    {'L', 			SCI_CTRL,	SCI_LINECUT},
    {'L', 			SCI_CSHIFT,	SCI_LINEDELETE},
    {'T', 			SCI_CTRL,	SCI_LINETRANSPOSE},
    {'U', 			SCI_CTRL,	SCI_LOWERCASE},
    {'U', 			SCI_CSHIFT,	SCI_UPPERCASE},
    {0,0,0},
};

