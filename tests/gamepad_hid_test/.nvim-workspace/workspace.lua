local nmap = require("config.utils").nmap

nmap("<leader>e", ":sp term://cmake --build build<cr>")
nmap("<leader>E", ":sp term://./program.sh<cr>")
