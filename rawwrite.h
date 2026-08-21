#ifndef RAWWRITE_H
#define RAWWRITE_H

void rw_selectDrive(unsigned char dosDriveNumber);
void rw_flush();
void rw_write(char* data, unsigned long size);

#endif