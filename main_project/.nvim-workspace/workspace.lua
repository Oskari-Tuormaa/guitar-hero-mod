local nmap = require('config.utils').nmap

nmap('<leader>e', ':sp term://cmake --build build -j30<cr>')
nmap('<leader>E', ':sp term://./program.sh<cr>')
nmap('<leader>dr', ':sp term://./reset.sh<cr>')
nmap('<leader>de', ':sp term://./flash_erase.sh<cr>')
