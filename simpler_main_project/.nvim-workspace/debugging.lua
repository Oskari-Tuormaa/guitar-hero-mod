local dap = require('dap')

dap.adapters.cppdbg = {
    id = 'cppdbg',
    type = 'executable',
    command = '/home/oskari/prog/cpptools/extension/debugAdapters/bin/OpenDebugAD7',
}

local exec_name = 'guitar-hero-mod.elf'

dap.configurations.c = {
    {
        name = "Just debug",
        type = "cppdbg",
        request = "launch",
        MIMode = "gdb",
        miDebuggerServerAddress = "localhost:3333",
        miDebuggerPath = "/usr/bin/arm-none-eabi-gdb",
        cwd = "${workspaceFolder}",
        program = "${workspaceFolder}/build/" .. exec_name,

        stopAtEntry = false,
        postRemoteConnectCommands = {
            {
                text = "monitor reset halt",
            },
            {
                text = "break main",
            },
        },
    },
    {
        name = "Flash and debug",
        type = "cppdbg",
        request = "launch",
        MIMode = "gdb",
        miDebuggerServerAddress = "localhost:3333",
        miDebuggerPath = "/usr/bin/arm-none-eabi-gdb",
        cwd = "${workspaceFolder}",
        program = "${workspaceFolder}/build/" .. exec_name,

        stopAtEntry = false,
        postRemoteConnectCommands = {
            {
                text = "monitor reset halt",
            },
            {
                text = "load",
            },
            {
                text = "break main",
            },
        },
    },
}
