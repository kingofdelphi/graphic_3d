if &cp | set nocp | endif
map  
map  
let s:cpo_save=&cpo
set cpo&vim
vmap gx <Plug>NetrwBrowseXVis
nmap gx <Plug>NetrwBrowseX
vnoremap <silent> <Plug>NetrwBrowseXVis :call netrw#BrowseXVis()
nnoremap <silent> <Plug>NetrwBrowseX :call netrw#BrowseX(expand((exists("g:netrw_gx")? g:netrw_gx : '<cfile>')),netrw#CheckIfRemote())
cnoremap  <Home>
cnoremap  <Left>
cnoremap  <Del>
cnoremap  <End>
inoremap  
cnoremap  <Right>
cnoremap  <Down>
cnoremap  <Up>
inoremap  
cnoremap  <S-Right>
cnoremap  <S-Left>
let &cpo=s:cpo_save
unlet s:cpo_save
set autoindent
set background=dark
set backspace=indent,eol,start
set expandtab
set hlsearch
set incsearch
set matchpairs=(:),{:},[:],<:>
set ruler
set runtimepath=~/.vim,~/.vim/bundle/Vundle.vim,~/.vim/bundle/vim-autoformat,/usr/share/vim/vimfiles,/usr/share/vim/vim74,/usr/share/vim/vimfiles/after,~/.vim/after,~/.vim/bundle/Vundle.vim,~/.vim/bundle/Vundle.vim/after,~/.vim/bundle/vim-autoformat/after
set shiftwidth=4
set showcmd
set showmatch
set smartindent
set suffixes=.bak,~,.swp,.o,.info,.aux,.log,.dvi,.bbl,.blg,.brf,.cb,.ind,.idx,.ilg,.inx,.out,.toc,.png,.jpg
set tabstop=4
set ttimeoutlen=1
" vim: set ft=vim :
