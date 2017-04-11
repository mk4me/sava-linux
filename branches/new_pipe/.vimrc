set tabstop=4
set softtabstop=4
set shiftwidth=4
set noexpandtab

set smartindent

set nowrap

set colorcolumn=110
highlight ColorColumn ctermbg=darkgray

autocmd VimEnter * NERDTree
" przeskakiwanie miedzy buforami CTRL+SHIFT+W, puscic, potem strzalka

set makeprg=make\ -C\ /home/mc/SAVA_out/Release\ -j40\ --no-print-directory

syntax enable

"let $path.="include,~/programming/Work/SAVA/SourceCode/trunk/include,"
set path=$PWD/**

let g:ycm_global_ycm_extra_conf = "./.ycm_extra.py"

colo distinguished

nnoremap <F5> :!./run<cr>
nnoremap <F4> :wa <Bar> make<cr>
