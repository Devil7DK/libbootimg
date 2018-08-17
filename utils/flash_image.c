/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mtdutils.h"
#include "log.h"
#include "utils.h"

#define LOG_TAG "flash_image"

#define HEADER_SIZE 2048  // size of header to compare for equality

void die(const char *msg, ...) {
    int err = errno;
    va_list args;
    va_start(args, msg);
    char buf[1024];
    vsnprintf(buf, sizeof(buf), msg, args);
    va_end(args);

    if (err != 0) {
        strlcat(buf, ": ", sizeof(buf));
        strlcat(buf, strerror(err), sizeof(buf));
    }

    fprintf(stderr, "%s\n", buf);
    LOGE("%s\n", buf);
    exit(1);
}

/* Read an image file and write it to a flash partition. */

int flash_image(char* partition_s, char* image_s) {
    const MtdPartition *ptn;
    MtdWriteContext *write;
    void *data;
    unsigned sz;

    if (mtd_scan_partitions() <= 0) die("error scanning partitions");
    const MtdPartition *partition = mtd_find_partition_by_name(partition_s);
    if (partition == NULL) die("can't find %s partition", partition_s);

    // If the first part of the file matches the partition, skip writing

    int fd = open(image_s, O_RDONLY);
    if (fd < 0) die("error opening %s", image_s);

    char header[HEADER_SIZE];
    int headerlen = read(fd, header, sizeof(header));
    if (headerlen <= 0) die("error reading %s header", image_s);

    MtdReadContext *in = mtd_read_partition(partition);
    if (in == NULL) {
        LOGW("error opening %s: %s\n", partition_s, strerror(errno));
        // just assume it needs re-writing
    } else {
        char check[HEADER_SIZE];
        int checklen = mtd_read_data(in, check, sizeof(check));
        if (checklen <= 0) {
            LOGW("error reading %s: %s\n", partition_s, strerror(errno));
            // just assume it needs re-writing
        } else if (checklen == headerlen && !memcmp(header, check, headerlen)) {
            LOGI("header is the same, not flashing %s\n", partition_s);
            return 0;
        }
        mtd_read_close(in);
    }

    // Skip the header (we'll come back to it), write everything else
    LOGI("flashing %s from %s\n", partition_s, image_s);

    MtdWriteContext *out = mtd_write_partition(partition);
    if (out == NULL) die("error writing %s", partition_s);

    char buf[HEADER_SIZE];
    memset(buf, 0, headerlen);
    int wrote = mtd_write_data(out, buf, headerlen);
    if (wrote != headerlen) die("error writing %s", partition_s);

    int len;
    while ((len = read(fd, buf, sizeof(buf))) > 0) {
        wrote = mtd_write_data(out, buf, len);
        if (wrote != len) die("error writing %s", partition_s);
    }
    if (len < 0) die("error reading %s", image_s);

    if (mtd_write_close(out)) die("error closing %s", partition_s);

    // Now come back and write the header last

    out = mtd_write_partition(partition);
    if (out == NULL) die("error re-opening %s", partition_s);

    wrote = mtd_write_data(out, header, headerlen);
    if (wrote != headerlen) die("error re-writing %s", partition_s);

    // Need to write a complete block, so write the rest of the first block
    size_t block_size;
    if (mtd_partition_info(partition, NULL, &block_size, NULL))
        die("error getting %s block size", partition_s);

    if (lseek(fd, headerlen, SEEK_SET) != headerlen)
        die("error rewinding %s", image_s);

    int left = block_size - headerlen;
    while (left < 0) left += block_size;
    while (left > 0) {
        len = read(fd, buf, left > (int)sizeof(buf) ? (int)sizeof(buf) : left);
        if (len <= 0) die("error reading %s", image_s);
        if (mtd_write_data(out, buf, len) != len)
            die("error writing %s", partition_s);
        left -= len;
    }

    if (mtd_write_close(out)) die("error closing %s", partition_s);
    return 0;
}