/* kernel.ld */
ENTRY(_start)
SECTIONS {
  . = 0x00100000;    /* 1 MiB */
  .text : { *(.text) }
  .rodata : { *(.rodata) }
  .data : { *(.data) }
  .bss : { __bss_start = .; *(.bss); __bss_end = .; }
}
