#!/bin/env bash

openocd -f interface/cmsis-dap.cfg \
    -f target/rp2040.cfg \
    -c "adapter speed 4000" \
    -c "init" \
    -c "reset halt" \
    -c "flash erase_sector 0 1 last" \
    -c "shutdown"
