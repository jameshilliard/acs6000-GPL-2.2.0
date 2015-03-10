#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <types.h>
#include <flashhdr.h>
#include <checksum.h>

#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/bio.h>

#include "fdt.h"

static FILE *filemsg;
static char buffer[1024];
static long signaturePos = 0;
static unsigned char signature[SIGNATURE_METHOD_SHA1_RSA_SIGNSIZE];

#include "oem_product_table.h"

static int verify_avocent_header (FILE *filein, int family, int type,
	unsigned long *filepos, unsigned long *filesize)
{
	DsFlashFileHdr1 hdr;
	DsFlashHdr khdr;
	DsHwPartNumber *pn;
	OptRecHdr *opt;
	int i, j, oemfd=-1, wrongoem=1;
	unsigned short val, oemval = 0, prodFamily = 0, checksumType = 0;
	unsigned long crc, len, pos;
	char *pcrc = "checksum";
	int oemCount = sizeof( oemTable ) / sizeof( struct OemTable );
	int prodCount = sizeof( prodTable ) / sizeof( struct ProductTable );
	char oemstr[] = "0";

	/* read file header */
	fseek (filein, 0, SEEK_SET);
	fread (&hdr, 1, sizeof(DsFlashFileHdr1), filein);

	/* Check file header */
	if (ntohl(hdr.hdrMagic) != DS_FLASH_HDR1_MAGIC) {
		fprintf(filemsg, "Bad Magic Number [%08x].\n", ntohl(hdr.hdrMagic));
		return 0;
	}
	if (ntohl(hdr.hdrVers) != 2) {
		fprintf(filemsg, "Header v%d not supported.\n", ntohl(hdr.hdrVers));
		return 0;
	}

	/* Print information about the header */
	fprintf(filemsg, "    File version %s.\n", hdr.flashFileVersion.version);
	fprintf(filemsg, "    Created on %s at %s.\n",
		hdr.image_create_date, hdr.image_create_time);
	if (ntohl(hdr.optRecLength) > 1024) {
		fprintf(filemsg, "    Option block is too long.\n");
	} else {
		fputs  ("    Option headers: ", filemsg);
		fseek (filein, ntohl(hdr.optRecOffset), SEEK_SET);
		fread (buffer, 1, ntohl(hdr.optRecLength), filein);

		for (i = 0; i < ntohl(hdr.optRecLength); ) {
			opt = (OptRecHdr *)(buffer + i);
			if (ntohs(opt->recType) == OPTREC_TYPE_END) {
				break;
			}
			switch (ntohs(opt->recType)) {
			case OPTREC_TYPE_OEM:
				fputs ("OEM", filemsg);
				for (j = 0; j < ntohs(opt->recLength); j ++) {
					oemval *= 0x100;
					oemval += ((unsigned char *)(opt + 1))[j];
				}
				for (j = 0; j < oemCount; j ++) {
					if (oemTable[j].code == oemval) {
						fprintf(filemsg, "[%s] ", oemTable[j].name);
						break;
					}
				}
				if (j == oemCount) {
					fprintf(filemsg, "[%d] ", oemval);
				}
				break;
			case OPTREC_TYPE_FAMILY:
				fputs ("FAMILY", filemsg);
				for (j = 0; j < ntohs(opt->recLength); j ++) {
					prodFamily *= 0x100;
					prodFamily += ((unsigned char *)(opt + 1))[j];
				}
				for (j = 0; j < prodCount; j ++) {
					if (prodTable[j].code == prodFamily) {
						fprintf(filemsg, "[%s] ", prodTable[j].name);
						break;
					}
				}
				if (j == prodCount) {
					fprintf(filemsg, "[%d] ", prodFamily);
				}
				break;
			case OPTREC_TYPE_CHECKSUM:
				fputs ("CHECKSUM", filemsg);
				val = 0;
				for (j = 0; j < ntohs(opt->recLength); j ++) {
					val *= 0x100;
					val += ((unsigned char *)(opt + 1))[j];
				}
				switch (val) {
				case 0:
					fputs ("[cksum] ", filemsg);
					pcrc = "checksum";
					break;
				case 1:
					fputs ("[crc32] ", filemsg);
					pcrc = "CRC32";
					break;
				case 2:
					fputs ("[crc16] ", filemsg);
					pcrc = "CRC16";
					break;
				default:
					fprintf(filemsg, "[%d] ", val);
				}
				checksumType = val;
				break;
			case OPTREC_TYPE_SIGNATURE:
				fputs ("SIGNATURE", filemsg);
				val = ((unsigned char *)(opt + 1))[0] * 0x100 +
					((unsigned char *)(opt + 1))[1];
				switch(val) {
				case SIGNATURE_METHOD_SHA1_RSA:
					fprintf (filemsg, "[SHA1+RSA] ");
					signaturePos = ntohl(hdr.optRecOffset) +
						((char *)opt - buffer) +
						sizeof(OptRecHdr) + 2 +
						SIGNATURE_METHOD_SHA1_RSA_KEYSIZE;
					break;
				default:
					fprintf (filemsg, " [%d] ", val);
					break;
				}
				break;
			default:
				fprintf (filemsg, "unknown(%d)", ntohs(opt->recType));
				break;
			}
			i += sizeof(OptRecHdr) + ntohs(opt->recLength);
		}
		fputs("\n", filemsg);
	}

	//check board for OEM
	if (((oemfd = open("/proc/sys/avocent/bdinfo/oem", O_RDONLY)) < 0) ||
		(read(oemfd, oemstr, 1) < 1)) {
		printf("Couldn't check board for OEM release...\n");
		if (oemfd != -1) close(oemfd);
		return 0;
	}
	switch (oemval) {
	case OPTREC_OEM_AVOCENT:
		if (atoi(oemstr) == 0) wrongoem = 0;
		break;
	case OPTREC_OEM_FCL:
		if (atoi(oemstr) == 1) wrongoem = 0;
		break;
	}
	if (wrongoem) {
		printf("\nHardware doesn't support this firmware (wrong OEM code).\n\n");
		return 0;
	}

	//check product family
	if (prodFamily != family) {
		fprintf(filemsg, "\nHardware doesn't support this firmware (wrong product family).\n\n");
		return 0;
	}

	if (signaturePos) {
		SHA_CTX ctx;
		unsigned char hash[ SHA_DIGEST_LENGTH ];
		long pos;
		RSA* rsa;
		FILE *filekey;

		fputs("    Checking encrypted signature ... ", filemsg);
		SHA1_Init( &ctx );
		fseek(filein, 0, SEEK_SET);
		pos = 0;
		while ((i = fread(buffer, 1, 1024, filein)) > 0) {
			if (pos <= signaturePos && pos + i > signaturePos) {
				memset(buffer + signaturePos - pos, 0,
					(pos + i <= signaturePos + SIGNATURE_METHOD_SHA1_RSA_SIGNSIZE) ?
						i - (signaturePos - pos) :
						SIGNATURE_METHOD_SHA1_RSA_SIGNSIZE);
			} else if (pos > signaturePos && pos <= signaturePos + SIGNATURE_METHOD_SHA1_RSA_SIGNSIZE) {
				memset(buffer, 0,
					(pos + i <= signaturePos + SIGNATURE_METHOD_SHA1_RSA_SIGNSIZE) ?
						i : SIGNATURE_METHOD_SHA1_RSA_SIGNSIZE + (signaturePos - pos));
			}
			SHA1_Update( &ctx, buffer, i);
			pos += i;
		}
		SHA1_Final( hash, &ctx );
		/* get public key */
		fseek(filein, signaturePos - SIGNATURE_METHOD_SHA1_RSA_KEYSIZE, SEEK_SET);
		rsa = d2i_RSA_PUBKEY_fp( filein, NULL);
		if (rsa == NULL) {
			fputs("could not extract public key.\n", filemsg);
			return 0;
		}
		/* get signature */
		fseek(filein, signaturePos, SEEK_SET);
		fread(signature, 1, SIGNATURE_METHOD_SHA1_RSA_SIGNSIZE, filein);
		/* validate */
		if (RSA_verify( NID_sha1, hash, SHA_DIGEST_LENGTH, signature,
			SIGNATURE_METHOD_SHA1_RSA_SIGNSIZE, rsa ) == 0 ) {
			fputs("could not validate.\n", filemsg);
			return 0;
		}
		fputs("validated.\n", filemsg);

		fputs("    Checking if it is a trusted key ... ", filemsg);
		filekey = fopen("/etc/CA/pubkey.pem", "r");
		if (! filekey) {
			fputs("key not found.\n", filemsg);
		} else {
			EVP_PKEY* trusted_key = PEM_read_PUBKEY( filekey, NULL, NULL, NULL );
			if (! trusted_key) {
				fputs("could not read key.\n", filemsg);
			} else {
				RSA* trusted_rsa = trusted_key->pkey.rsa;
				if ( ( BN_cmp( rsa->n, trusted_rsa->n ) == 0 ) &&
					( BN_cmp( rsa->e, trusted_rsa->e )) == 0 ) {
					fputs("OK.\n", filemsg);
				} else {
					fputs("not a trusted key.\n", filemsg);
				}
				EVP_PKEY_free( trusted_key );
			}
			fclose(filekey);
		}
	}

	if (ntohl(hdr.partNumCount) * sizeof(DsHwPartNumber) > 1024) {
		fputs("    Part Number block is too long.\n", filemsg);
	} else {
		fputs  ("    Part Numbers: ", filemsg);
		fseek (filein, ntohl(hdr.partNumOffset), SEEK_SET);
		fread (buffer, 1, ntohl(hdr.partNumCount) *
			sizeof(DsHwPartNumber), filein);
		/* Check part numbers */
		pn = (DsHwPartNumber *)buffer;
		for (i = 0; i < ntohl(hdr.partNumCount); i ++) {
			fprintf(filemsg, "%s ", (pn + i)->partNumber);
		}
		fputs("\n", filemsg);
	}

	if(type == 0 || type == 1) {
		/* kernel */
		fprintf(filemsg, "    Kernel Image from %08x (%d bytes).\n",
			ntohl(hdr.zImageOffset), ntohl(hdr.zImageLength));
		fprintf(filemsg, "        checking %s ... ", pcrc);
		crc = 0;
		for (pos = 0; pos < ntohl(hdr.zImageLength); pos += 1024) {
			len = (ntohl(hdr.zImageLength) - pos < 1024) ?
				ntohl(hdr.zImageLength) - pos: 1024;
			fseek (filein, ntohl(hdr.zImageOffset) + pos, SEEK_SET);
			fread (buffer, 1, len, filein);
			crc = computeBufferChecksum(
				checksumType, crc, buffer, len);
		}
		if (ntohl(hdr.zImageChkSum) != crc) {
			fputs("NOK.\n", filemsg);
			return 0;
		}
		fputs("OK.\n", filemsg);
		fputs  ("        checking kernel header ... ", filemsg);
		fseek (filein, ntohl(hdr.zImageOffset), SEEK_SET);
		fread (&khdr, 1, sizeof(DsFlashHdr), filein);
		if (ntohl(khdr.magic) !=  DS_FLASH_MAGIC ) {
			fprintf(filemsg, "wrong magic number [%08x].\n", ntohl(khdr.magic));
			return 0;
		}
		if (strncmp(khdr.signature, DS_FLASH_SIGNATURE, 32)) {
			fprintf(filemsg, "invalid signature [%s].\n", khdr.signature);
			return 0;
		}
		if (ntohl(khdr.hdrVers) != 1) {
			fprintf(filemsg, "header v%d not supported.\n", ntohl(khdr.hdrVers));
			return 0;
		}
		crc = 0;
		for (pos = 0; pos < ntohl(khdr.flashLength); pos += 1024) {
			len = (ntohl(khdr.flashLength) - pos < 1024) ?
				ntohl(khdr.flashLength) - pos : 1024;
			fseek (filein, ntohl(hdr.zImageOffset) +
				ntohl(khdr.flashOffset) + pos, SEEK_SET);
			fread (buffer, 1, len, filein);
			for (i = 0; i < len; i ++) {
				crc += buffer[i];
			}
		}
		if (crc != ntohl(khdr.flashSum)) {
			fputs("checksum error.\n", filemsg);
			return 0;
		}
		fputs("OK.\n", filemsg);
		if (ntohl(khdr.flashOffset) + ntohl(khdr.flashLength) < 
			ntohl(hdr.zImageLength)) {
			struct fdt_header *fdt;
			fputs  ("        checking device tree... ", filemsg);
			fseek (filein, ntohl(hdr.zImageOffset) +
				ntohl(khdr.flashOffset) + 
				ntohl(khdr.flashLength), SEEK_SET);
			fread(buffer, 1, sizeof(struct fdt_header), filein);
			fdt = (struct fdt_header *) buffer;
        		if (ntohl(fdt->magic) == FDT_MAGIC) {
                		/* Complete tree */
                		if (ntohl(fdt->version) < FDT_FIRST_SUPPORTED_VERSION) {
					fputs("bad version.\n", filemsg);
					return 0;
				}
				if (ntohl(fdt->last_comp_version) > FDT_LAST_SUPPORTED_VERSION) {
					fputs("bad version.\n", filemsg);
					return 0;
				}
			} else if (ntohl(fdt->magic) == SW_MAGIC) {
				/* Unfinished sequential-write blob */
				if (ntohl(fdt->size_dt_struct) == 0) {
					fputs("bad state.\n", filemsg);
					return 0;
				}
			} else {
				fputs("bad magic number.\n", filemsg);
				return 0;
			}
			if (ntohl(khdr.flashOffset) + 
				ntohl(khdr.flashLength) + 
				ntohl(fdt->totalsize) > 
				ntohl(hdr.zImageLength)) {
				fputs("incomplete.\n", filemsg);
				return 0;
			} 
			fputs("OK.\n", filemsg);
		}
		if (type) {
			*filepos = ntohl(hdr.zImageOffset);
			*filesize = ntohl(hdr.zImageLength);
			return 1;
		}
	}
	if (type == 0 || type == 2) {
		/* filesystem */
		fprintf(filemsg, "    CRAM filesystem from %08x (%d bytes).\n",
			ntohl(hdr.cramfsOffset), ntohl(hdr.cramfsLength));
		fprintf(filemsg, "        checking %s ... ", pcrc);
		crc = 0;
		for (pos = 0; pos < ntohl(hdr.cramfsLength); pos += 1024) {
			len = (ntohl(hdr.cramfsLength) - pos < 1024) ?
				ntohl(hdr.cramfsLength) - pos: 1024;
			fseek (filein, ntohl(hdr.cramfsOffset) + pos, SEEK_SET);
			fread (buffer, 1, len, filein);
			crc = computeBufferChecksum(
				checksumType, crc, buffer, len);
		}
		if (ntohl(hdr.cramfsChkSum) != crc) {
			fputs("NOK.\n", filemsg);
			return 0;
		}
		fputs("OK.\n", filemsg);
		if (type) {
			*filepos = ntohl(hdr.cramfsOffset);
			*filesize = ntohl(hdr.cramfsLength);
			return 1;
		}
	}
	if (type == 0 || type == 3) {
		/* boot */
		fprintf(filemsg, "    RMON from %08x (%d bytes).\n",
			ntohl(hdr.rmonOffset), ntohl(hdr.rmonLength));
		fprintf(filemsg, "        checking %s ... ", pcrc);
		crc = 0;
		for (pos = 0; pos < ntohl(hdr.rmonLength); pos += 1024) {
			len = (ntohl(hdr.rmonLength) - pos < 1024) ?
				ntohl(hdr.rmonLength) - pos: 1024;
			fseek (filein, ntohl(hdr.rmonOffset) + pos, SEEK_SET);
			fread (buffer, 1, len, filein);
			crc = computeBufferChecksum(
				checksumType, crc, buffer, len);
		}
		if (ntohl(hdr.rmonChkSum) != crc) {
			fputs("NOK.\n", filemsg);
			return 0;
		}
		fputs("OK.\n", filemsg);
		if (type) {
			*filepos =  ntohl(hdr.rmonOffset);
			*filesize = ntohl(hdr.rmonLength);
			return 1;
		}
	}
	return 1;
}

int main(int argc,char **argv)
{
	FILE *filein = NULL, *fileout = NULL, *fileaux = NULL;
	struct stat filein_stat;
	int ret;
	unsigned long dataout, sizeout, filesize, nbytes;
	int type, family;

	if (strstr(argv[0], "checkKernel")) {
		type = 1;
	} else if (strstr(argv[0], "checkFileSystem")) {
		type = 2;
	} else if (strstr(argv[0], "checkBoot")) {
		type = 3;
	} else {
		type = 0;
	}

	if (argc < 2) {
		switch (type) {
		case 0:
			printf("\nUsage: %s <image file>\n", argv[0]);
			printf("   Validates the image in <image file>\n\n");
			break;
		case 1:
			printf("\nUsage: %s <image file> [<kernel file>]\n", argv[0]);
			printf("   Extracts the kernel from the image\n");
			break;
		case 2:
			printf("\nUsage: %s <image file> [<file system file>]\n", argv[0]);
			printf("   Extracts the file system from the image\n\n");
			break;
		case 3:
			printf("\nUsage: %s <image file> [<boot file>]\n", argv[0]);
			printf("   Extracts the boot code from the image\n\n");
			break;
		}
		exit(1);
	}
	if (argc < 3) {
		fileout=stdout;
		filemsg=stderr;
	} else {
		fileout=NULL;
		filemsg=stdout;
	}
	/* Check for input file size */
	ret = stat(argv[1], &filein_stat);
	if (ret != 0) {
		fprintf(filemsg, "File %s does not exist!\n", argv[1]);
		goto exit_error;
	}
	filein = fopen(argv[1], "r");
	if (filein == NULL) {
		perror("Error: ");
		fprintf(filemsg, "File %s could not be opened!\n", argv[1]);
		goto exit_error;

	}
	if (argc >= 3) {
		fileout = fopen(argv[2], "w");
		if (fileout == NULL) {
			perror("Error: ");
			fprintf(filemsg, "File %s could not be opened!\n", argv[2]);
			goto exit_error;
		}
	}

	fileaux = fopen("/proc/sys/avocent/bdinfo/family", "r");
	if (fileaux == NULL) {
		perror("Error: ");
		fprintf(filemsg, "Cannot check the appliance family code\n");
		goto exit_error;
	}
	fread(buffer, 1, 10, fileaux);
	fclose(fileaux);
	family = atoi(buffer);

	if (! verify_avocent_header (filein, family, type, &dataout, &sizeout)) {
		goto exit_error;
	}
	if (type) {
		if (dataout + sizeout > filein_stat.st_size) {
			fprintf(filemsg, "Error in input file size.\n");
			goto exit_error;
		}
		if (fseek (filein, dataout, SEEK_SET) != 0) {
			goto exit_error;
		}
		filesize = 0;
		while ((nbytes=fread (buffer, 1, 1024, filein)) != 0) {
			if (filesize + nbytes < sizeout) {
				fwrite (buffer, 1, nbytes, fileout);
				filesize += nbytes;
			} else {
				fwrite (buffer, 1, sizeout - filesize, fileout);
				break;
			}
		}
		fclose(filein);
		if (argc >= 3) {
			fprintf(filemsg, "File %s generated, address %08x length %08x\n",
				argv[2], (unsigned int)dataout - (unsigned int)buffer,
				sizeout);
			fclose(fileout);
		}
	}
	exit(0);

exit_error:
	if (filein) {
		fclose(filein);
	}
	if (argc >= 3 && fileout) {
		fclose(fileout);
	}
	exit(1);
}

