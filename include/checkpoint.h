/* AOS — include/checkpoint.h — Purpose: Header file */
/*
 * AOS — checkpoint.h
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#ifndef CHECKPOINT_H
#define CHECKPOINT_H

/* Simple checkpoint HAL API. Future work will integrate CRIU and
 * delta-based snapshots per branch. */

int cp_snapshot(const char *branch, const char *dest);
int cp_diff(const char *branch, const char *dest);
int cp_restore(const char *branch, const char *src);

#endif
