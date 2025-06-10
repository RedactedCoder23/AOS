/* kernel.ld */
ENTRY(_start)
PHDRS {
  text PT_LOAD FLAGS(5); /* RX */
  data PT_LOAD FLAGS(6); /* RW */
}

SECTIONS {
  . = 0x00100000;    /* 1 MiB */
  .text : { *(.text) } :text
  .rodata : { *(.rodata) } :text
  .data : { *(.data) } :data
  .bss : { __bss_start = .; *(.bss); __bss_end = .; } :data
  . = ALIGN(0x1000);
  ipc_shared = .;
  . += 0x1000; /* reserve one page for IPC ring */
}
