INCLUDES= `gtk-config --cflags` -DGTK -DSCI_LEXER -W -Wall -I .

noinst_LTLIBRARIES=libscintilla-widget.la libscintilla-lexers.la

libscintilla_lexers_la_LDFLAGS = --all-static
libscintilla_lexers_la_SOURCES = \
	LexCPP.cxx       \
	LexHTML.cxx      \
	LexLua.cxx       \
	LexOthers.cxx    \
	LexPerl.cxx      \
	LexPython.cxx    \
	LexSQL.cxx       \
	LexVB.cxx

libscintilla_widget_la_LDFLAGS = --all-static
libscintilla_widget_la_SOURCES= \
	DocumentAccessor.cxx \
	WindowAccessor.cxx \
	KeyWords.cxx \
	Document.cxx \
	CallTip.cxx \
	ScintillaBase.cxx \
	ContractionState.cxx \
	Editor.cxx \
	PropSet.cxx \
	PlatGTK.cxx \
	KeyMap.cxx \
	LineMarker.cxx \
	ScintillaGTK.cxx \
	CellBuffer.cxx \
	ViewStyle.cxx \
	Style.cxx \
	Indicator.cxx \
	AutoComplete.cxx \
	Accessor.h          Editor.h      PropSet.h          SVector.h \
	AutoComplete.h      Indicator.h   SciLexer.h         UniConversion.h \
	CallTip.h           KeyMap.h      ScintillaBase.h    ViewStyle.h \
	CellBuffer.h        KeyWords.h    Scintilla.h        WinDefs.h \
	ContractionState.h  LineMarker.h  ScintillaWidget.h  WindowAccessor.h \
	DocumentAccessor.h  Platform.h    SString.h \
	Document.h          PosRegExp.h   Style.h


