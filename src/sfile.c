
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "sfile.h"


int SendSFile(char *file, int options);
int ParseSFile(char *file, char *buf, int options);
unsigned char GetByte(char *str);

/* serial.c */
extern int SendData(char *buf, int len);


/* Send an S record file to the device fd. If len_on is set then the length of
 * a line is sent before the address and data. If addr_on is set then the
 * memory load address of every record is sent before the data. 
 *
 *		[line length] [memory addr] <data>
 */
int SendSFile(char *file, int options) {
	unsigned char *buf;
	int fd, len;
	
	/* get file length */
	fd = open(file, O_RDONLY);
	if (fd == -1)
		return -1;
	len = lseek(fd, 0, SEEK_END);
	close(fd);
	
	/* parse file */
	buf = malloc(len);
	if (buf == NULL)
		return -1;
	len = ParseSFile(file, buf, SFILE_COMPRESSED);
	if (len == -1) {
		free(buf);
		return -1;
	}

	/* send file */
	SendData(buf, len);

	free(buf);
	return 0;
}


int ParseSFile(char *file, char *buf, int options) {
	unsigned int filelen, buflen, memaddr1, memaddr2, holelen;
	int fd;
	int holeh;		/* help variable for calculating memory holes */
	caddr_t sfile;
	unsigned char linelen, csum;
	unsigned char addr[2];
	int i, j;

	/* mmap the file */
	fd = open(file, O_RDONLY);
	if (fd == -1) return -1;
	filelen = lseek(fd, 0, SEEK_END);
	sfile = mmap(0, filelen, PROT_READ, MAP_PRIVATE, fd, 0);
	
	for (i=buflen=holeh=memaddr1=memaddr2=0; i<filelen;) {
		/* parse record variables */
		linelen = GetByte(sfile+i+2);
		addr[1] = GetByte(sfile+i+4);
		addr[0] = GetByte(sfile+i+6);
		csum = GetByte(sfile+i+2+linelen*2);
		
		/* handle records */
		if (sfile[i] == 'S' && sfile[i+1] == '0') {			/* header record */
			/* ... */
		} else if (sfile[i] == 'S' && sfile[i+1] == '1') {	/* data record */
			/* parse line length */
			if (options & SFILE_LEN_ON) {
				buf[buflen] = GetByte(sfile+i+2);	/* data+addr length */
				if (!(options & SFILE_ADDR_ON))		/* only data length */
					buf[buflen] -= 2;
				buflen++;
			}

			/* parse memory load address */
			if (options & SFILE_ADDR_ON) {
				buf[buflen] = GetByte(sfile+i+4);
				buf[buflen+1] = GetByte(sfile+i+6);
 				
				buflen += 2;
			}

			/* fill holes with 0, if using uncompressed format */
			if (options & SFILE_UNCOMPRESSED) {
				if (!holeh) {
					memaddr1 = (GetByte(sfile+i+4)<<8 | GetByte(sfile+i+6)) +
								linelen-3;
					holeh++;
				} else {
					memaddr2 = GetByte(sfile+i+4)<<8 | GetByte(sfile+i+6);
					//printf("%02X %02X %02X\n", memaddr1, memaddr2, linelen-3);
					if (memaddr1 > memaddr2)
						break;
					holelen = memaddr2 - memaddr1;
					for (j=0; j<holelen; j++, buflen++)
						buf[buflen] = 0;

					memaddr1 = memaddr2 + linelen-3;
				}
			}

			/* parse data */
			for (j=0; j<linelen-3; j++, buflen++)
				buf[buflen] = GetByte(sfile+i+8+j*2);

		} else if (sfile[i] == 'S' && sfile[i+1] == '9') {	/* term. record */
			/* ... */
		} else {
			printf("S-file format error\n");
			return -1;
		}
			
		i += (linelen+2)*2+1;
	}
	
	return buflen;
}


unsigned char GetByte(char *str) {
	unsigned char c1, c2;

	c1 = str[0];
	c2 = str[1];
	if (c1 > '9') c1 -= 7;
	if (c2 > '9') c2 -= 7;
	c1 -= 0x30;
	c2 -= 0x30;

	return (c2 | (c1<<4));
}
