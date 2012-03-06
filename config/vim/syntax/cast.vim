" Vim syntax file
" Language:     CAST configure file
" Maintainer:   Marko Mahnič
" Created:      2012-01-20
" Last changed: 2012-02-15

" Installation: put in ~/.vim/syntax/cast.vim (see also :help mysyntaxfile)
" Use:
"   add a modeline to cast files, sth. like:
"      # vim: set ft=cast :
"   or use an autocmd like:
"      au BufRead,BufNewFile *.cast,*.incl		set filetype=cast

" Quit when a (custom) syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn keyword	castTodo	contained TODO FIXME XXX
syn match       castBegin       "^" skipwhite nextgroup=castComponent,castArchComponentLang,castComment,castSetvar,castSubarch,castConditional,castEndConditional,castHostname,castInclude

syn keyword     castSubarch       contained SUBARCHITECTURE nextgroup=castSubarchId skipwhite
syn match       castSubarchId     contained "[-_a-zA-Z0-9.@]\+" nextgroup=castComment skipwhite

syn keyword     castComponent     contained COMPONENT nextgroup=castComponentLang skipwhite
syn keyword     castComponentLang contained CPP JAVA PYTHON nextgroup=componentId skipwhite
syn match       componentId       contained "[-_a-zA-Z0-9.@]\+" nextgroup=componentClass skipwhite
syn match       componentClass    contained "[-_a-zA-Z0-9.@]\+" nextgroup=componentParams skipwhite
syn match       componentParams   contained ".*$" contains=castString,castComment

syn keyword     castArchComponentLang contained CPP JAVA PYTHON nextgroup=castArchComponentType,castArchBaseType skipwhite
syn keyword     castArchComponentType contained MG UM skipwhite nextgroup=componentId
syn keyword     castArchBaseType      contained TM WM skipwhite nextgroup=componentClass

syn keyword     castInclude       contained INCLUDE  skipwhite nextgroup=castAny
syn keyword     castHostname      contained HOSTNAME skipwhite nextgroup=componentParams
syn keyword     castSetvar          contained SETVAR VARDEFAULT SETOPT OPTDEFAULT skipwhite nextgroup=castSetvarVariable
syn match       castSetvarVariable  contained "[-_a-zA-Z0-9.@]\+" skipwhite nextgroup=castSetvarEqual
syn match       castSetvarEqual     contained "=" skipwhite nextgroup=castSetvarValue,castMultiLine
syn match       castSetvarValue     contained "\S\(.*\S\)\?\ze\s*" skipwhite contains=castComment
syn keyword     castConditional     contained IFEQ IFNEQ IFOPTALL IFOPTANY IFTRUE IFFALSE  nextgroup=castAny
syn keyword     castEndConditional  contained ELSE ENDIF  nextgroup=castAny
syn match       castAny             contained ".*$" contains=castComment

" Avoid matching "text#text", used in /etc/disktab and /etc/gettytab
syn match	castComment	"#.*" contains=castTodo
syn match	castBolComment	"^\s*#.*" contained contains=castTodo
syn region	castString      start=+"+  end=+"+ oneline contained
syn region      castMultiLine   contained matchgroup=Special start="<multiline>" end="^\s*\zs</multiline>" contains=castString,castBolComment

" Define the default highlighting.
" Only used when an item doesn't have highlighting yet
hi def link castComment		Comment
hi def link castBolComment	Comment
hi def link castEolComment	Comment
hi def link castTodo		Todo
hi def link castString		String

hi def link castSubarch 	Keyword
hi def link castSubarchId	Title

hi def link castHostname	Keyword
hi def link castInclude		Keyword

hi def link castComponent     Type
hi def link castComponentLang Type
hi def link castComponentType Type
hi def link componentId       Title
hi def link componentClass    Type

hi def link castArchComponentLang Type
hi def link castArchBaseType Keyword
hi def link castArchComponentType Keyword

hi def link castSetvar Keyword
hi def link castSetvarVariable Normal
hi def link castSetvarEqual    Normal
hi def link castSetvarValue    String
hi def link castMultiLine      Normal


hi def link castConditional keyword
hi def link castEndConditional keyword

let b:current_syntax = "cast"

" vim: ts=8 sw=2
