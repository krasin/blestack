SOC                     = NRF51822_QFAA_CA
SOC_VARIANT             = xxaa
BOARD                   = BOARD_PCA10001
USE_SOFTDEVICE          = blank

PROJECT_TARGET          = raw-ble-scan

PROJECT_INCLUDE_PATHS   = ../log
PROJECT_C_SOURCE_PATHS  = ../log
PROJECT_C_SOURCE_FILES  = main.c \
                            log.c \
                            nrf_delay.c \
                            simple_uart.c

HEAP_SIZE  = 0
STACK_SIZE = 1024

PROGRAMMER      = python ../segger.py
SCRIPTS_PATH    = ../scripts

include ../Makefile.common
