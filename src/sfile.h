
#ifndef SFILE_H
#define SFILE_H


/* Structure describing a parsed s-file. Is created by CreateSFile() and
 * destroyed by DestroySFile(). */
struct sfile {
	char *data;
	int dsize;
	int options;
};

/* Different ways to parse the sfile. */
#define SFILE_ADDR_ON		1		/* address info */
#define SFILE_LEN_ON		2		/* record length info */
#define SFILE_COMPRESSED	3		/* addr on, len on (1 | 2) = 3 */
#define SFILE_UNCOMPRESSED	4		/* addr off, len off */


struct sfile *CreateSFile(char *file, int options);
void DestroySFile(struct sfile *sf);
unsigned char GetByte(char *str);


#endif SFILE_H
