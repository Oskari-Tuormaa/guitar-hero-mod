local nmap = require('config.utils').nmap

nmap('<leader>e', ':sp term://cmake --build build -j30<cr>')
nmap('<leader>E', ':sp term://./program.sh<cr>')
nmap('<leader>bp', ':sp term://cmake -B build && cmake --build build -j30<cr>')
nmap('<leader>dr', ':sp term://./reset.sh<cr>')
