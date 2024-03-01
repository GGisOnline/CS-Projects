#include "lib_tar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>


/**
 * Helper function
 * 
 ** @param: A buffer containing 512 characters (header included)
 ** @return: checksum
 */

int sum_calc(char *buffer){
    int sum = 0;
    for(int i = 0; i < 512; i++){
        if (i > 148 && i <= 155){}
        else{
            sum = buffer[i];
        }
    }
    return sum;     
}

long checksum(char* buffer){
    uint32_t sum = 0;
    int i = 0;
    while(i < 512) {
        if (i > 148 && i < 156) sum += ' ';
        else sum += buffer[i];
        i++;
    }
    sum -= 16;
    return sum; 
}

/**
 * Checks whether the archive is valid.
 *
 * Each non-null header of a valid archive has:
 *  - a magic value of "ustar" and a null,
 *  - a version value of "00" and no null,
 *  - a correct checksum
 *
 * @param tar_fd A file descriptor pointing to the start of a file supposed to contain a tar archive.
 *
 * @return a zero or positive value if the archive is valid, representing the number of non-null headers in the archive,
 *         -1 if the archive contains a header with an invalid magic value,
 *         -2 if the archive contains a header with an invalid version value,
 *         -3 if the archive contains a header with an invalid checksum value
 */
int check_archive(int tar_fd){

    int result = 0;
    char *buffer = (char*)malloc(sizeof(char)*512);
    if(!buffer) return EXIT_FAILURE;
    /*uint8_t *buf; DRAFT*/

    while(read(tar_fd, buffer, 512)){

        /*buf = malloc(sizeof(uint8_t)*512); DRAFT
        read(tar_fd, buf, 512); DRAFT*/

        /* Checking end of archive ('\0') */
        if (*buffer == AREGTYPE) break;

        tar_header_t *head = (tar_header_t*) buffer;
        tar_header_t *header = malloc(sizeof(tar_header_t));

        memcpy(header->magic, &buffer[257], TMAGLEN);
        memcpy(header->version, &buffer[263], TVERSLEN);
        memcpy(header->chksum, &buffer[148], 8); 
        memcpy(header->size, &buffer[124], 12);
        /*printf("%s\n",header->size); DRAFT*/

        /* Checking invalid magic value */
        if (strcmp(header->magic,TMAGIC) != 0) {
            free(buffer);
            free(header);
            /*free(buf); DRAFT*/
            lseek(tar_fd, 0, SEEK_SET); 
            return -1;
        }
        
        /* Checking invalid version value */
        if(header->version[0] != '0' && header->version[1] != '0'){
            free(buffer); 
            free(header);
            /*free(buf); DRAFT*/
            lseek(tar_fd, 0, SEEK_SET); 
            return -2;
        }

        /* Checking checksum */
        
        if(TAR_INT(header->chksum) != checksum(buffer)){
            free(buffer);
            free(header);
            /*free(buf); DRAFT*/
            lseek(tar_fd, 0, SEEK_SET); 
            return -3;
        }  
        
        /* GO to next header */
        if(head->typeflag == REGTYPE){
            size_t size = TAR_INT(header->size);
            if(size != 0) lseek(tar_fd, 512*(size/512 +1), SEEK_CUR);
        }
        free(header);
        result++;
    }

    /*free(buf); DRAFT*/
    free(buffer);
    lseek(tar_fd, 0, SEEK_SET);
    return result;
}


/**
 * Checks whether an entry exists in the archive.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 *
 * @return zero if no entry at the given path exists in the archive,
 *         any other value otherwise.
 */
int exists(int tar_fd, char *path){

    char *buffer = (char*)malloc(sizeof(char)*512);
    if(!buffer) return EXIT_FAILURE;
    /*uint8_t *buf; DRAFT*/

    while(read(tar_fd, buffer, 512)){

        /*buf = malloc(sizeof(uint8_t)*512); DRAFT*/
        /*read(tar_fd, buf, 512); DRAFT*/

        tar_header_t *head = (tar_header_t*) buffer;
        tar_header_t *header = malloc(sizeof(tar_header_t));

        memcpy(header->name, &buffer[0], sizeof(header->name));
        memcpy(header->magic, &buffer[257], TMAGLEN);
        memcpy(header->version, &buffer[263], TVERSLEN);
        memcpy(header->chksum, &buffer[148], 8); 
        memcpy(header->size, &buffer[124], 12);
        /*memcpy(head->typeflag, &buffer[156], 0); DRAFT*/
        /*printf("%s\n",header->size); DRAFT*/

        /* If exists */
        if(strncmp(header->name, path, strlen(path) - 1) == 0){
            free(buffer);
            free(header);
            lseek(tar_fd, 0, SEEK_SET);
            return 1;
        }

        /* GO to next header */
        if(head->typeflag == REGTYPE){
            size_t size = TAR_INT(header->size);
            if(size!=0) lseek(tar_fd, 512*(size/512 +1), SEEK_CUR);
        }
    }

    free(buffer);
    /*free(header); DRAFT*/
    lseek(tar_fd, 0, SEEK_SET);
    return 0;
}

/**
 * Checks whether an entry exists in the archive and is a directory.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 *
 * @return zero if no entry at the given path exists in the archive or the entry is not a directory,
 *         any other value otherwise.
 */
int is_dir(int tar_fd, char *path){

    if(path == NULL) return 0;
    char *buffer = (char *) malloc(sizeof(char)*512);
    if(!buffer) return EXIT_FAILURE;
    /*uint8_t *buf; DRAFT*/

    while(read(tar_fd, buffer, 512)){

        /*buf = malloc(sizeof(uint8_t)*512); DRAFT*/
        /*read(tar_fd, buf, 512); DRAFT*/

        tar_header_t *head = (tar_header_t*) buffer;
        tar_header_t *header = malloc(sizeof(tar_header_t));

        memcpy(header->name, &buffer[0], sizeof(header->name));
        memcpy(header->magic, &buffer[257], TMAGLEN);
        memcpy(header->version, &buffer[263], TVERSLEN);
        memcpy(header->chksum, &buffer[148], 8); 
        memcpy(header->size, &buffer[124], 12);
        
        /* If directory exists */
        if(strcmp(header->name, path) == 0){
            if (head->typeflag == DIRTYPE){
                free(buffer);
                free(header);
                lseek(tar_fd, 0, SEEK_SET);
                return 1;
            }
        }

        /* GO to next header */
        if(head->typeflag == REGTYPE){
            size_t size = TAR_INT(header->size);
            if(size!=0) lseek(tar_fd, 512*(size/512 +1), SEEK_CUR);
        }
    }

    free(buffer);
    /*free(header); DRAFT*/
    lseek(tar_fd, 0, SEEK_SET);
    return 0;
}

/**
 * Checks whether an entry exists in the archive and is a file.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 *
 * @return zero if no entry at the given path exists in the archive or the entry is not a file,
 *         any other value otherwise.
 */
int is_file(int tar_fd, char *path){
    
    if(path == NULL) return 0;
    char *buffer = (char *) malloc(sizeof(char)*512);
    if(!buffer) return EXIT_FAILURE;
    /*uint8_t *buf; DRAFT*/

    while(read(tar_fd, buffer, 512)){

        /*buf = malloc(sizeof(uint8_t)*512); DRAFT*/
        /*read(tar_fd, buf, 512); DRAFT*/

        tar_header_t *head = (tar_header_t*) buffer;
        tar_header_t *header = malloc(sizeof(tar_header_t));
        
        memcpy(header->name, &buffer[0], sizeof(header->name));
        memcpy(header->magic, &buffer[257], TMAGLEN);
        memcpy(header->version, &buffer[263], TVERSLEN);
        memcpy(header->chksum, &buffer[148], 8); 
        memcpy(header->size, &buffer[124], 12);
        
        /* If file exists */
        if(strcmp(header->name, path) == 0){
        if (head->typeflag == REGTYPE || head->typeflag == AREGTYPE){
            free(buffer);
            free(header);
            lseek(tar_fd, 0, SEEK_SET);
            return 1;
            }
        }

        /* GO to next header */
        if(head->typeflag == REGTYPE){
            size_t size = TAR_INT(header->size);
            if(size!=0) lseek(tar_fd, 512*(size/512 +1), SEEK_CUR);
        }
    }

    free(buffer);
    /*free(header); DRAFT*/
    lseek(tar_fd, 0, SEEK_SET);
    return 0;
}

/**
 * Checks whether an entry exists in the archive and is a symlink.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 * @return zero if no entry at the given path exists in the archive or the entry is not symlink,
 *         any other value otherwise.
 */
int is_symlink(int tar_fd, char *path){

    char *buffer = (char *) malloc(sizeof(char)*512);
    if(!buffer) return EXIT_FAILURE;
    /*uint8_t *buf; DRAFT*/

    while(read(tar_fd, buffer, 512)){

        /*buf = malloc(sizeof(uint8_t)*512); DRAFT*/
        /*read(tar_fd, buf, 512); DRAFT*/

        tar_header_t *head = (tar_header_t*) buffer;
        tar_header_t *header = malloc(sizeof(tar_header_t));
        
        memcpy(header->name, &buffer[0], sizeof(header->name));
        memcpy(header->magic, &buffer[257], TMAGLEN);
        memcpy(header->version, &buffer[263], TVERSLEN);
        memcpy(header->chksum, &buffer[148], 8); 
        memcpy(header->size, &buffer[124], 12);
        
        /* If symlink exists */
        if(strcmp(header->name, path) == 0){
            if (head->typeflag == SYMTYPE || head->typeflag == LNKTYPE){
                free(buffer);
                free(header);
                lseek(tar_fd, 0, SEEK_SET);
                return 1;
            }
        }

        /* GO to next header */
        if(head->typeflag == REGTYPE){
            size_t size = TAR_INT(header->size);
            if(size!=0) lseek(tar_fd, 512*(size/512 +1), SEEK_CUR);
        }
    }

    free(buffer);
    /*free(header); DRAFT*/
    lseek(tar_fd, 0, SEEK_SET);
    return 0;
}


/**
 * Lists the entries at a given path in the archive.
 * list() does not recurse into the directories listed at the given path.
 *
 * Example:
 *  dir/          list(..., "dir/", ...) lists "dir/a", "dir/b", "dir/c/" and "dir/e/"
 *   ├── a
 *   ├── b
 *   ├── c/
 *   │   └── d
 *   └── e/
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive. If the entry is a symlink, it must be resolved to its linked-to entry.
 * @param entries An array of char arrays, each one is long enough to contain a tar entry path.
 * @param no_entries An in-out argument.
 *                   The caller set it to the number of entries in `entries`.
 *                   The callee set it to the number of entries listed.
 *
 * @return zero if no directory at the given path exists in the archive,
 *         any other value otherwise.
 */
int list(int tar_fd, char *path, char **entries, size_t *no_entries){

    if(!exists(tar_fd, path)) return 0;
    char *buffer = (char *)malloc(sizeof(char)*512);
    char *buf = (char *)malloc(sizeof(char)*100);
    if(!buffer || !buf) return EXIT_FAILURE;
    /*uint8_t *buf; DRAFT*/
    
    strcpy(buf, path);

    /* If the entry is not in dir */
    if(!is_dir(tar_fd, buf)){
        lseek(tar_fd, 0, SEEK_SET);
        free(buffer);
        free(buf);
        /*free(header); DRAFT*/
        *no_entries = 0;
        return 0;
    }

    /* If the entry is a symlink TODO*/
    /*if(is_symlink(tar_fd, path)){*/
        /*while(read(tar_fd, buffer, 512)){*/

            /*buf = malloc(sizeof(uint8_t)*512); DRAFT*/
            /*read(tar_fd, buf, 512); DRAFT*/

            /*tar_header_t *head = (tar_header_t*) buffer;
            tar_header_t *header = malloc(sizeof(tar_header_t));

            memcpy(header->name, &buffer[0], sizeof(header->name));
            memcpy(header->size, &buffer[124], 12);
            memcpy(header->magic, &buffer[257], TMAGLEN);
            memcpy(header->version, &buffer[263], TVERSLEN);
            memcpy(header->chksum, &buffer[148], 8); 
            memcpy(header->linkname, &buffer[157], sizeof(header->linkname));
            
            if(strcmp(header->name, path) == 0){
                lseek(tar_fd, 0, SEEK_SET);
                int result = list(tar_fd, header->linkname, entries, no_entries);
                free(buffer);
                free(header);
                return result;
            }*/
            
            /* GO to next header */
            /*if(head->typeflag == REGTYPE){
                size_t size = TAR_INT(header->size);
                if(size!=0) lseek(tar_fd, 512*(size/512 +1), SEEK_CUR);
            }
        }
    }*/

    /* ELSE TODO*/
    /*int result = 0; DRAFT

    lseek(tar_fd, 0, SEEK_SET);

    while(read(tar_fd, buffer, 512)){

        tar_header_t *head = (tar_header_t*) buffer;
        tar_header_t *header = malloc(sizeof(tar_header_t));

        memcpy(header->name, &buffer[0], sizeof(header->name));
        memcpy(header->size, &buffer[124], 12);
        memcpy(header->magic, &buffer[257], TMAGLEN);
        memcpy(header->version, &buffer[263], TVERSLEN);
        memcpy(header->chksum, &buffer[148], 8); 
        memcpy(header->linkname, &buffer[157], sizeof(header->linkname));*/


        
        /* GO to next header */
        /*if(head->typeflag == REGTYPE){
            size_t size = TAR_INT(header->size);
            if(size!=0) lseek(tar_fd, 512*(size/512 +1), SEEK_CUR);
        }
    }*/

    free(buffer);
    /*free(header); DRAFT*/
    lseek(tar_fd, 0, SEEK_SET);
    return 1;
}

/**
 * Reads a file at a given path in the archive.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive to read from.  If the entry is a symlink, it must be resolved to its linked-to entry.
 * @param offset An offset in the file from which to start reading from, zero indicates the start of the file.
 * @param dest A destination buffer to read the given file into.
 * @param len An in-out argument.
 *            The caller set it to the size of dest.
 *            The callee set it to the number of bytes written to dest.
 *
 * @return -1 if no entry at the given path exists in the archive or the entry is not a file,
 *         -2 if the offset is outside the file total length,
 *         zero if the file was read in its entirety into the destination buffer,
 *         a positive value if the file was partially read, representing the remaining bytes left to be read to reach
 *         the end of the file.
 *
 */
ssize_t read_file(int tar_fd, char *path, size_t offset, uint8_t *dest, size_t *len) {
    if(!exists(tar_fd, path)){
        return -1;
    }
    char* buffer = malloc(sizeof(char)*512);
    if(!buffer) return EXIT_FAILURE;
    if(is_file(tar_fd, path)){
        while(read(tar_fd, buffer, 512)){
            tar_header_t *head = (tar_header_t*)buffer;
            if(strcmp(head->name, path) == 0 && head->typeflag == REGTYPE) 
            {
                break;
            }    
        }   
        tar_header_t *header = (tar_header_t*)buffer;
        if(offset >= TAR_INT(header->size)){
            free(buffer);
            lseek(tar_fd, 0, SEEK_SET);
            return -2;
        }
        if (*len == 0) {
            free(buffer);
            lseek(tar_fd, 0, SEEK_SET);
            return TAR_INT(header->size);
        }
        uint8_t *file = malloc(sizeof(char)*TAR_INT(header->size));
        read(tar_fd, file, *len);
        memcpy(dest, &file[offset], TAR_INT(header->size));
        free(buffer);
        free(file);
        lseek(tar_fd, 0, SEEK_SET);
        return 0;
    }
    free(buffer);
    lseek(tar_fd, 0, SEEK_SET);
    return -1;
}
    
