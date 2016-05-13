#define BLOCK_SIZE 512

void mac_fd_read(int offset_from, int offset_to, int blk_count);

void mac_fd_write(int offset_from, int offset_to, int blk_count);