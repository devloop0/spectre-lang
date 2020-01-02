" Vim syntax file
" Language: Spectre
" Maintainer: devloop0

if exists("b:current_syntax")
	finish
endif

syn keyword spConditional if else switch
syn keyword spLabel case default
syn keyword spRepeat for while do
syn keyword spPrimitiveTypes byte bool char short int long float double void fn
syn keyword spPrimitiveBools true false
syn keyword spTypeModifiers unsigned signed const type auto
syn keyword spStorageClassModifier static access
syn keyword spStatement break continue return __asm__
syn keyword spStructure struct union
syn keyword spInclude import include
syn keyword spDefine constexpr
syn keyword spOperator sizeof alignof new resv stk delete as

syn keyword spKeyword func using namespace

syn region spCBlockComment start="/\*\%(!\|\*[*/]\@!\)\@!" end="\*/"
syn region spCLineComment start="//" end="$" 
syn region spPythonLineComment start="#" end="$"

syn match spEscape display contained "\\\(x\x+\|\o{1,3}\|.\)"

syn region spString start=+"+ skip=+\\\\\|\\"+ end=+"+ contains=spEscape

syn match spCharacter "'\([^\\]\|\\\(.\|x\x+\|\o{1,3}\)\)'"

syn match spDecNumber display "\<[0-9][0-9']*\%(_\%([fFdDlLsS]\|[uU]\%([sS]\|[iI]\|[lL]\)\)\)\="
syn match spHexNumber display "\<0[xX][0-9a-fA-F']\+\%(_\%([fFdDlLsS]\|[uU]\%([sS]\|[iI]\|[lL]\)\)\)\="
syn match spOctNumber display "\<0[oO][0-7']\+\%(_\%([fFdDlLsS]\|[uU]\%([sS]\|[iI]\|[lL]\)\)\)\="
syn match spBinNumber display "\<0[bB][01']\+\%(_\%([fFdDlLsS]\|[uU]\%([sS]\|[iI]\|[lL]\)\)\)\="

syn match spFloat display "\<[0-9][0-9']*\.\%([0-9']*\%([eE][+-]\=[0-9']\+\)\=\)\=\%(_\%([fFdDlLsS]\|[uU]\%([sS]\|[iI]\|[lL]\)\)\)\="
syn match spFloat2 display "\.[0-9]\%([0-9']*\%([eE][+-]\=[0-9']\+\)\=\)\=\%(_\%([fFdDlLsS]\|[uU]\%([sS]\|[iI]\|[lL]\)\)\)\=\>"

hi def link spConditional Conditional
hi def link spRepeat Repeat
hi def link spKeyword Keyword
hi def link spLabel Label
hi def link spPrimitiveBools Boolean
hi def link spPrimitiveTypes Type
hi def link spTypeModifiers Type
hi def link spStorageClassModifier StorageClass
hi def link spStructure Structure
hi def link spInclude Include
hi def link spDefine Define
hi def link spOperator Operator
hi def link spStatement Statement

hi def link spDecNumber Number
hi def link spHexNumber Number
hi def link spOctNumber Number
hi def link spBinNumber Number

hi def link spFloat Float
hi def link spFloat2 Float

hi def link spString String

hi def link spCharacter Character

hi def link spCLineComment Comment
hi def link spPythonLineComment Comment
hi def link spCBlockComment Comment

hi def link spEscape Special

let b:current_syntax = "spectre"
