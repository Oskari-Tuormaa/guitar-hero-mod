#!/bin/env bash

cmake --build build -j30 && \
    openocd -f interface/cmsis-dap.cfg \
        -f target/rp2040.cfg \
        -c "adapter speed 4000" \
        -c "init" \
        -c "reset halt" \
		-c "program build/gamepad_hid_test.elf verify" \
		-c "reset run" \
		-c "shutdown"
