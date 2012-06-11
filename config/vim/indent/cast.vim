" Vim syntax file
" Language:     CAST configure file
" Maintainer:   Marko Mahnič
" Created:      2012-04-24

" Installation: put in ~/.vim/indent/cast.vim (see also :help mysyntaxfile)
" Use:
"   add a modeline to cast files, sth. like:
"      # vim: set ft=cast :
"   or use an autocmd like:
"      au BufRead,BufNewFile *.cast,*.incl		set filetype=cast

if exists("b:did_indent")
  finish
endif
let b:did_indent = 1

setlocal indentexpr=CastGetIndent(v:lnum)
setlocal indentkeys+==ENDIF

" Only define the function once.
if exists("*CastGetIndent")
  finish
endif

fun! CastGetIndent(lnum)
  let this_line = getline(a:lnum)

  " Find a non-blank line above the current line.
  let lnum = a:lnum
  let lnum = prevnonblank(lnum - 1)
  let previous_line = getline(lnum)

  " Hit the start of the file, use zero indent.
  if lnum == 0
    return 0
  endif

  let ind = indent(lnum)

  let cast_indent_comment_line = '^\s*#.*'
  let cast_multiline_open_regex = '=\s*<multiline>\s*$'
  let cast_multiline_close_regex = '^\s*</multiline>'

  let cast_indent_begin_regex = '^\s*\(IFEQ\|IFNEQ\|IFOPTANY\|IFOPTALL\|IFTRUE\|IFFALSE\|ELSE\)\s*'
  let cast_indent_end_regex = '^\s*\(ELSE\|ENDIF\)\s*'

  " Add
  if previous_line =~? cast_indent_comment_line " Handle comments
    let ind = ind
  else
    if previous_line =~? cast_indent_begin_regex
      let ind = ind + &sw
    endif
    if previous_line =~? cast_multiline_open_regex
      let ind = ind + &sw
    endif
  endif

  " Subtract
  if this_line =~? cast_indent_end_regex
    let ind = ind - &sw
  endif
  if this_line =~? cast_multiline_close_regex
    let ind = ind - &sw
  endif

  return ind
endfun
