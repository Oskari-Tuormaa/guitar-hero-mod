#!/bin/env bash

cmake --build build -j30 && \
    openocd -f interface/cmsis-dap.cfg \
        -f target/rp2040.cfg \
        -c "adapter speed 4000" \
        -c "init" \
		-c "program build/fret_button_read_test.elf verify" \
		-c "reset run" \
		-c "shutdown"
