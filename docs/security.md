# Security Model

## Branch Ownership
Each branch is assigned an `owner_uid` when created. All kernel branch
syscalls verify that the calling UID matches the branch owner. Listing
branches only returns entries owned by the caller.

HTTP endpoints mirror this behaviour. If `request.remote_user` does not
match the branch owner, the service returns HTTP 403.

## Credential Vault ACLs
`ai_cred_manager.py` now accepts an optional `--group` parameter on
`set`, `get` and `list`. Only users that belong to the specified POSIX
group may perform the action. Group membership is checked via the `grp`
module.

## Audit Log Format
Audit events are recorded as newline-delimited JSON in
`/var/log/aos-audit.log` with the fields `timestamp`, `user`, `action`,
`resource` and `result`. Use `aos-audit show` with filtering options to
inspect the log.
