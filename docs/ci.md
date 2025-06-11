# Continuous Integration

`make fast-test` runs only Python unit tests and is used on non-Linux runners.
The full workflow uses `verify_all.sh` which builds the project, runs tests and
executes the demo container.
