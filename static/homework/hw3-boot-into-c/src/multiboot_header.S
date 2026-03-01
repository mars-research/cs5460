.section .multiboot_header
.p2align 3
header_start:
    .long 0xe85250d6
    .long 0
    .long header_end - header_start

    .long -(0xe85250d6 + 0 + (header_end - header_start))

    .short 0
    .short 0
    .short 8
header_end: