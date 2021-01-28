#!/bin/bash

# Load ${LENGTH} from ${EEPROM} at ${OFFSET} into ${FRUID}, but only if
# ${SENTINEL} does not exist.

# This script supports the following environment variables:
# FRUID     Required: Hex FRU ID
# EEPROM    Required: Path to the EEPROM file
# OFFSET    Optional: Offset within EEPROM file
# LENGTH    Optional: Length to read from EEPROM file
# SENTINEL  Optional: Sentinel file. If this file exists, the FRU is not
#                     loaded
#
# Note that OFFSET and LENGTH must either both be set or both not set.

DD='/bin/dd'
TMPDIR='/tmp'

Say() {
  printf '%s\n' "$*"
}

Die() {
  Say "$@" >&2
  exit 1
}

Success() {
  Say "$@" >&2
  exit 0
}

DumpVarRef() {
  local id
  for id in "$@"; do
    Say "${id}=${!id}"
  done
}

SanityCheckVars() {
  local vars=(
    FRUID
    EEPROM
    OFFSET
    LENGTH
    SENTINEL
  )

  if [[ -n ${DEBUG:-} ]]; then
    DumpVarRef "${vars[@]}"
  fi

  # Check if we have a FRUID. If we don't, return an error.
  [[ -n ${FRUID:-} ]] ||
    Die 'Must specify FRUID.'

  # Check if EEPROM file exists. If it does not, return an error.
  [[ -n ${EEPROM:-} ]] ||
    Die 'Must specify EEPROM.'
  [[ -e ${EEPROM} ]] ||
    Die "EEPROM file ${EEPROM} does not exist."
}

ExtractEeprom() {
  local file=$1
  local dd_args=(
    "if=${EEPROM}"
    "skip=${OFFSET}"
    "count=${LENGTH}"
    "of=${file}"
    "bs=1"
  )
  ${DD} "${dd_args[@]}" ||
    Die "Error code $? while running ${DD} ${dd_args[*]}."
}

DeleteTmpFile() {
  rm -f "${TMPFILE}"
}

Main() {
  # Sanity check variables
  SanityCheckVars

  # Check if sentinel exists. If it does, return success right away.
  if [[ -f ${SENTINEL} ]]; then
    Success "Skipping EEPROM ${EEPROM}."
  fi

  # Default to reading the whole EEPROM
  local file=${EEPROM}

  # Check if we have an offset and length. If so, extract that chunk
  # out of the EEPROM file. Set the FILE variable.
  if [[ -n ${OFFSET:-} && -n ${LENGTH:-} ]]; then
    # Set path to temporary file to be deleted at exit
    TMPFILE="${TMPDIR}/eeprom-$FRUID"
    trap DeleteTmpFile EXIT

    file=${TMPFILE}
    ExtractEeprom "${file}"
  fi

  # Call phosphor-read-eeprom
  Say "Loading EEPROM ${file} ..."
  phosphor-read-eeprom --fruid "${FRUID}" --eeprom "${file}"
}

# Exit without running main if this script was sourced
return 0 2>/dev/null || Main "$@"
