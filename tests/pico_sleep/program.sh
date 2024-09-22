#!/bin/env bash

cmake --build build -j30 && \
    openocd -f interface/cmsis-dap.cfg \
        -f target/rp2040.cfg \
        -c "adapter speed 4000" \
        -c "init" \
		-c "program build/pico_sleep_test.elf verify" \
		-c "reset run" \
		-c "shutdown"
