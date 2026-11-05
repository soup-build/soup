#!/bin/sh
# Extract OpenCode credentials from the host into a portable JSON file.
#
# Runs on the HOST before the container starts, usually via the project's
# devcontainer.json `initializeCommand`.
#
# This uses `opencode auth export`, which reads credentials through OpenCode's
# own database connection and emits its supported, schema-agnostic export
# format. That is deliberately safer than reading the SQLite file directly:
#   - it takes a consistent point-in-time read via the running service,
#   - it never inspects or locks the database file ourselves, so it cannot
#     corrupt a database the host is actively writing, and
#   - it is version-stable, so a host/container version mismatch does not break
#     on a schema change the way raw SQL would.
#
# Usage:
#   extract-auth.sh [output-file]
set -eu

OUT="${1:-${OPENCODE_AUTH_SEED_FILE:-.devcontainer/opencode-auth-seed.json}}"

log() { printf 'opencode-extract-auth: %s\n' "$*" >&2; }

mkdir -p "$(dirname "${OUT}")"

# Export into a private file before publishing it. Concurrent devcontainer
# starts can then mount either complete snapshot, never a partially written one.
umask 077
TMP="$(mktemp "${OUT}.tmp.XXXXXXXX")"
trap 'rm -f "${TMP}"' EXIT
trap 'exit 1' HUP INT TERM

OPENCODE="$(command -v opencode || true)"
if [ -z "${OPENCODE}" ] && [ -n "${HOME:-}" ] && [ -x "${HOME}/.opencode/bin/opencode" ]; then
  OPENCODE="${HOME}/.opencode/bin/opencode"
fi

if [ -z "${OPENCODE}" ]; then
  log "opencode was not found on the host; writing an empty seed"
  printf '[]\n' >"${TMP}"
else
  # `--standalone` runs against a private server so the export does not depend
  # on (or disturb) the shared background service the host may be using.
  if ! "${OPENCODE}" auth export --standalone >"${TMP}"; then
    log "auth export failed; writing an empty seed"
    printf '[]\n' >"${TMP}"
  fi
fi

mv -f "${TMP}" "${OUT}"
printf 'opencode-extract-auth: wrote %s bytes to %s\n' "$(wc -c <"${OUT}")" "${OUT}"
