#!/bin/bash
#
# TYAN common GPIO functions.

# Offset to be used against GPIO_BASE
declare -A GPIO_NAMES_TO_OFFSET=(
  ['PGOOD']=$((3 * 8 + 2))                  # GPIO D2
  ['BMC_PWRBTN_OUT_N']=$((4 * 8 + 1))       # GPIO E1
  ['BMC_RSTBTN_OUT_N']=$((4 * 8 + 3))       # GPIO E3
  ['BIOS_SPI_SEL_PCH']=$((9 * 8 + 0))       # GPIO J0
  ['BIOS_SPI_SEL_BMC']=$((9 * 8 + 1))       # GPIO J1
)

# 1 is active_low 0 is active_high
declare -A GPIO_NAMES_TO_ACTIVE_LOW=(
  ['PGOOD']=0
  ['BMC_PWRBTN_OUT_N']=1
  ['BMC_RSTBTN_OUT_N']=1
  ['BIOS_SPI_SEL_PCH']=0
  ['BIOS_SPI_SEL_BMC']=0
)

GPIO_BASE=

##################################################
# Get GPIO_BASE
# Global: GPIO_BASE
# Returns:
#   non-zero if cannot get GPIO base, 0 if success
##################################################
get_gpio_base() {
  if (( $# != 0 )); then
    echo "Usage: get_gpio_base" >&2
    return 1
  fi

  local gpiochip
  gpiochip=$(grep -H '1e780000.gpio' /sys/class/gpio/gpiochip*/label)

  local sysfspath=${gpiochip%/label:*}

  if [[ -z ${sysfspath} ]]; then
    echo "Unable to find gpiochip ${gpiochip} path!" >&2
    return 2
  fi

  if ! GPIO_BASE=$(cat "${sysfspath}/base" 2>/dev/null); then
    echo "Failed to get gpio base for ${gpiochip}!" >&2
    return 2
  fi
}

##################################################
# Initializes the gpio state
# This operation is idempotent and can be applied
# repeatedly to the same gpio. It will make sure the
# gpio ends up in the initialized state even if it
# was.
# Arguments:
#   $1: GPIO name
# Return:
#   0 if success, non-zero if error
##################################################
init_gpio() {
  if (( $# != 1 )); then
    echo "Usage: init_gpio name" >&2
    return 1
  fi

  local name=$1

  local offset=${GPIO_NAMES_TO_OFFSET["${name}"]}
  if [[ -z ${offset} ]]; then
    echo "Missing offset info for: ${name}" >&2
    return 2
  fi

  local active_low=${GPIO_NAMES_TO_ACTIVE_LOW["${name}"]}
  if [[ -z ${active_low} ]]; then
    echo "Missing active_low info for: ${name}" >&2
    return 2
  fi

  if [[ -z ${GPIO_BASE:-} ]]; then
    if ! get_gpio_base; then
      return 3
    fi
  fi

  local abs=$((GPIO_BASE + offset))
  if [[ ! -e "/sys/class/gpio/gpio${abs}" ]]; then
    echo "${abs}" >'/sys/class/gpio/export'
  fi
  echo "${active_low}" >"/sys/class/gpio/gpio${abs}/active_low"
}

##################################################
# Release the gpio
# This operation is idempotent and can be applied
# repeatedly to the same gpio. It will make sure the
# gpio ends up in the released state even if it
# was.
# Arguments:
#   $1: GPIO name
# Return:
#   0 if success, non-zero if error
##################################################
release_gpio() {
  if (( $# != 1 )); then
    echo "Usage: release_gpio name" >&2
    return 1
  fi

  local name=$1

  local offset=${GPIO_NAMES_TO_OFFSET["${name}"]}
  if [[ -z ${offset} ]]; then
    echo "Missing offset info for: ${name}" >&2
    return 2
  fi

  if [[ -z ${GPIO_BASE:-} ]]; then
    if ! get_gpio_base; then
      return 3
    fi
  fi

  local abs=$((GPIO_BASE + offset))
  if [[ -e "/sys/class/gpio/gpio${abs}" ]]; then
    echo "${abs}" >'/sys/class/gpio/unexport'
  fi
}

##################################################
# Set output GPIO direction.
# Arguments:
#   $1: GPIO name
#   $2: GPIO direction, "high" or "low"
# Return:
#   0 if success, non-zero if error
##################################################
set_gpio_direction() {
  if (( $# != 2 )); then
    echo 'Usage: set_gpio_direction name direction' >&2
    return 1
  fi

  local name=$1
  local direction=$2

  local offset=${GPIO_NAMES_TO_OFFSET["${name}"]}
  if [[ -z ${offset} ]]; then
    echo "Missing offset info for: ${name}" >&2
    return 2
  fi

  if [[ -z ${GPIO_BASE:-} ]]; then
    if ! get_gpio_base; then
      return 3
    fi
  fi

  init_gpio "${name}" || return
  local abs=$((GPIO_BASE + offset))
  echo "${direction}" >"/sys/class/gpio/gpio${abs}/direction"
  echo "Set gpio ${name} #${abs} to direction ${direction}" >&2
}

##################################################
# Get GPIO value
# Arguments:
#   $1: GPIO name
# Return:
#   0 if success, non-zero if error
#   stdout: The value of the gpio
##################################################
get_gpio_value() {
  if (( $# != 1 )); then
    echo 'Usage: get_gpio_value name' >&2
    return 1
  fi

  local name=$1

  local offset=${GPIO_NAMES_TO_OFFSET["${name}"]}
  if [[ -z ${offset} ]]; then
    echo "Missing offset info for: ${name}" >&2
    return 2
  fi

  if [[ -z ${GPIO_BASE:-} ]]; then
    if ! get_gpio_base; then
      return 3
    fi
  fi

  init_gpio "${name}" || return
  local abs=$((GPIO_BASE + offset))
  cat "/sys/class/gpio/gpio${abs}/value"
}
