/*
******************************************************************************
**

**  File        : LinkerScript.ld
**
**
**
**  Abstract    : Linker script for STM32MP1 series
**                2048Kbytes FLASH and 33472Kbytes RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : STMicroelectronics STM32
**
**  Distribution: The file is distributed �as is,� without any warranty
**                of any kind.
**
*****************************************************************************
** @attention
**
** Copyright (c) 2019 STMicroelectronics.
** All rights reserved.
**
** This software component is licensed by ST under BSD 3-Clause license,
** the "License"; You may not use this file except in compliance with the
** License. You may obtain a copy of the License at:
**                        opensource.org/licenses/BSD-3-Clause
**
****************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = 0x10000;    /* end of RAM */
/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x800  /* 1 Kb */;      /* required amount of heap  */
_Min_Stack_Size = 0x800  /* 1 Kb */; /* required amount of stack */

/* Specify the memory areas */
/*RGB_BUFFERS    (rw)  : ORIGIN = 0xD1800000, LENGTH = 3000K*/
/*FLASH_BANK1 (rx)    : ORIGIN = 0x081E0000, LENGTH = 128K*/
MEMORY
{
ROM_ML    (rx)      : ORIGIN = 0x08080000, LENGTH = 1536K
ITCMRAM   (xrw)     : ORIGIN = 0x00000000, LENGTH = 64K
DTCMRAM   (xrw)     : ORIGIN = 0x20000000, LENGTH = 128K
RAM       (rw)      : ORIGIN = 0x24000000, LENGTH = 512K
SDRAM1    (rw)      : ORIGIN = 0xD0000000, LENGTH = 1573K
SDRAM2    (rw)      : ORIGIN = 0xD0180000, LENGTH = 1573K
SDRAM3    (rw)      : ORIGIN = 0xD0300000, LENGTH = 1573K
SDRAM4    (rw)      : ORIGIN = 0xD0480000, LENGTH = 1573K
SDRAM5    (rw)      : ORIGIN = 0xD0600000, LENGTH = 1573K
SDRAM6    (rw)      : ORIGIN = 0xD0780000, LENGTH = 1573K
EXT_RAM   (rw)      : ORIGIN = 0xD1800000, LENGTH = 1800K
DEMO_INFO (rx)      : ORIGIN = 0x0803F800, LENGTH = 2K
OPEN_AMP_RSC_TAB (xrw)	: ORIGIN = 0x38000000, LENGTH = 1K
OPEN_AMP_SHMEM (xrw)	: ORIGIN = 0x38000400, LENGTH = 63K
}

 __OPENAMP_region_start__  = ORIGIN(OPEN_AMP_SHMEM);
 __OPENAMP_region_end__ = ORIGIN(OPEN_AMP_SHMEM) + LENGTH(OPEN_AMP_SHMEM);

/* Define output sections */
SECTIONS
{
  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >ROM_ML
  
  /* The program code and other data goes into FLASH */
  .text :
  {
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;        /* define a global symbols at end of code */
  } >ROM_ML

  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } >ROM_ML

  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >ROM_ML
  .ARM : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >ROM_ML
  
  .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >ROM_ML
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >ROM_ML
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >ROM_ML

  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data : 
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >DTCMRAM AT> ROM_ML

  
  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >DTCMRAM

  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >DTCMRAM
   
  .openamp_section (NOLOAD) : 
  {
    . = ABSOLUTE(0x38000000);
    *(.resource_table)
  } > OPEN_AMP_RSC_TAB AT >ROM_ML
   
  
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
  flasher_code_section : { *(flasher_code_section) } >ROM_ML
  .DemoContextSection : { *(.DemoContextSection) } >DEMO_INFO
  heap_mem : { *(heap_mem) } >RAM
  .STemWinMemPool : { *(.STemWinMemPool) } >EXT_RAM
  .sdio_heap : { *(.sdio_heap) } >RAM
  .layer0_fb0 (NOLOAD) : { *(.layer0_fb0) } >SDRAM1
  .layer0_fb1 (NOLOAD) : { *(.layer0_fb1) } >SDRAM2
  .layer0_fb2 (NOLOAD) : { *(.layer0_fb2) } >SDRAM3
  .layer1_fb0 (NOLOAD) : { *(.layer1_fb0) } >SDRAM4
  .layer1_fb1 (NOLOAD) : { *(.layer1_fb1) } >SDRAM5
  .layer1_fb2 (NOLOAD) : { *(.layer1_fb2) } >SDRAM6
}


