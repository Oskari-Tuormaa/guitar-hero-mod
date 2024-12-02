local nmap = require('config.utils').nmap

local pico_path = "PICO_SDK_PATH=(realpath ../3rdparty/pico-sdk)"
local cmake_conf_flags = ""

nmap('<leader>e', '<cmd>sp term://cmake --build build -j30<cr>')
nmap('<leader>E', '<cmd>sp term://./program.sh && ./reset.sh<cr>')
nmap('<leader>dp', '<cmd>sp term://' .. pico_path .. ' cmake --fresh -B build ' .. cmake_conf_flags .. ' && cmake --build build -j30<cr>')
nmap('<leader>dr', '<cmd>sp term://./reset.sh<cr>')
nmap('<leader>de', '<cmd>sp term://./flash_erase.sh<cr>')
