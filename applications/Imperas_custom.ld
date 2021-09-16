
SECTIONS
{
  . = 0xCCCC0000;
  .text : { *(.text) }
  .rodata : { *(.rodata) }
  .data : { *(.data ) }
  .bss : { *(.bss ) }
}
