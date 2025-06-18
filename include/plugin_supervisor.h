/*
 * AOS — plugin_supervisor.h
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#ifndef PLUGIN_SUPERVISOR_H
#define PLUGIN_SUPERVISOR_H

/* Supervises plugin lifecycle and capability checks. */
int ps_load(const char *path);
int ps_unload(const char *name);

#endif
