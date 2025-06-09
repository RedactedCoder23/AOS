# Generating Command Mappings

The `generate_aos_mappings.py` helper converts `mappings.json` into C source
files used by the interpreter. It can also validate the JSON without writing any
files.

```
python3 scripts/generate_aos_mappings.py [--validate] [--outdir DIR]
```

Options:
- `--validate`  Only validate the mappings and exit with status 0 on success.
- `--outdir DIR`  Directory where `command_map.c`, `commands.c` and
  `category_defs.h` are written. Defaults to `gen/`.

Errors during parsing or file generation print a message and exit with a non-zero
status.
