local nmap = require('config.utils').nmap

local pico_path = "PICO_SDK_PATH=(realpath ../3rdparty/pico-sdk)"

nmap('<leader>e', ':sp term://cmake --build build -j30<cr>')
nmap('<leader>bp', ':sp term://' .. pico_path .. ' cmake --fresh -B build && cmake --build build -j30<cr>')
nmap('<leader>E', ':sp term://./program.sh && ./reset.sh<cr>')
nmap('<leader>dr', ':sp term://./reset.sh<cr>')
nmap('<leader>de', ':sp term://./flash_erase.sh<cr>')
