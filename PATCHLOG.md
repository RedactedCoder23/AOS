## Patch Log

### Changes
- Fixed branch API: `branch_create` now returns the new branch id
- Added error codes for `branch_clone` and improved CLI messages
- Implemented adjacency matrix population in bare-metal branch graph
- Added `aos branch list` command
- Added simple `make test` target running branch demo

### Tests
- `make clean && make all`
- `make aos-cli`
- `make test`
- `make run` *(fails: qemu not installed)*
