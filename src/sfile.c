
/*
 * sfile.c
 *
 * This module handles conversion of an s-file into several different formats.
 *
 * main.c uses CreateSFile(), DestroySFile().
 * monitor.c uses CreateSFile(), DestroySFile().
 *
 * public routines:
 *		CreateSFile() create an s-file struct describing the s-file.
 *		DestroySFile() destroy a previously created s-file struct.
 * private routines:
 *		GetByte() convert an ASCII hex number to a normal hex number.
 *
 */
 

#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "sfile.h"


/* Create an sfile structure describing the parsed s-file. The data buffer in
 * the structure can have the following format:
 *		[line length] [memory addr] <data>
 * It all depends on the options variable (check sfile.h for options) */
/* NOTE: This routine probably contains bugs. SEARCH AND DESTROY! */
struct sfile *CreateSFile(char *file, int options) {
	unsigned int filelen, buflen, memaddr1, memaddr2, holelen;
	int fd;
	int holeh;		/* help variable for calculating memory holes */
	caddr_t sfile;
	unsigned char linelen, csum;
	unsigned char addr[2];
	int i, j;
	struct sfile *sf;
	char *tmp_ptr;

	/* create sfile structure */
	sf = malloc(sizeof(struct sfile));
	if (sf == NULL)
		return NULL;
	sf->data = NULL;
	sf->dsize = 0;
	sf->options = options;

	/* mmap the file */
	fd = open(file, O_RDONLY);
	if (fd == -1) {
		printf(": ");
		fflush(stdout);
		perror("");
		goto fail;
	}
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
				tmp_ptr = realloc(sf->data, buflen+1);	/* expand buffer */
				if (!tmp_ptr)
					goto fail_mem;
				sf->data = tmp_ptr;
				
				sf->data[buflen] = GetByte(sfile+i+2);	/* data+addr len */
				if (!(options & SFILE_ADDR_ON))			/* only data length */
					sf->data[buflen] -= 2;
				buflen++;
			}

			/* parse memory load address */
			if (options & SFILE_ADDR_ON) {
				tmp_ptr = realloc(sf->data, buflen+2);	/* expand buffer */
				if (!tmp_ptr)
					goto fail_mem;
				sf->data = tmp_ptr;
				
				sf->data[buflen] = GetByte(sfile+i+4);
				sf->data[buflen+1] = GetByte(sfile+i+6);
 				
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

					tmp_ptr = realloc(sf->data, buflen+holelen);/* expand buf */
					if (!tmp_ptr)
						goto fail_mem;
					sf->data = tmp_ptr;
					
					for (j=0; j<holelen; j++, buflen++)
						sf->data[buflen] = 0;

					memaddr1 = memaddr2 + linelen-3;
				}
			}

			/* parse data */
			tmp_ptr = realloc(sf->data, buflen+linelen-3);		/* expand buf */
			if (!tmp_ptr)
				goto fail_mem;
			sf->data = tmp_ptr;
			for (j=0; j<linelen-3; j++, buflen++)
				sf->data[buflen] = GetByte(sfile+i+8+j*2);

		} else if (sfile[i] == 'S' && sfile[i+1] == '9') {	/* term. record */
			/* ... */
		} else {
			printf(": S-file format error\n");
			goto fail;
		}
			
		i += (linelen+2)*2+1;
	}
	
	sf->dsize = buflen;
	return sf;

  fail_mem:
	perror(": CreateSFile(): ");
  fail:
  	DestroySFile(sf);
	return NULL;
}


/* Just a free() wrapper. */
void DestroySFile(struct sfile *sf) {
	if (sf) {			/* so we can call the routine with a NULL var */
		free(sf->data);
		free(sf);
	}
}


/* Can you see the segfault? Fixit later. */
unsigned char GetByte(char *str) {
	unsigned char c1, c2;

	c1 = str[0];			/* HERE: what if str == NULL */
	c2 = str[1];			/* AND HERE: what if str is only one byte */
	if (c1 > '9') c1 -= 7;
	if (c2 > '9') c2 -= 7;
	c1 -= 0x30;
	c2 -= 0x30;

	return (c2 | (c1<<4));
}
