local nmap = require('config.utils').nmap

nmap('<leader>e', ':sp term://./program.sh<cr>')
nmap('<leader>E', ':sp term://cmake --build build -j30<cr>')
nmap('<leader>dr', ':sp term://./reset.sh<cr>')