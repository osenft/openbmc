#!/bin/bash

# Present a "host booting" screen.
# The screen is updated every second and adds a dot ('.') for every
# iteration of the loop. It also fetches the State.Boot.Raw Value
# property (BIOS POST code) and prints it at the bottom right of
# the screen.

# Hide cursor
echo -en '\e[?25l'

# Clear screen and move to 0,0
echo -en '\033[2J'
echo -en '\033[0;0H'

echo -n "Please wait, system booting .."

while true; do
  # Print a single dot ('.')
  echo -n '.'

  # Save current cursor position
  echo -en "\033[s"

  # Jump to 24,60
  echo -en '\033[24;60H'

  # Fetch and print BIOS POST code
  printf '%02X' $(busctl get-property "xyz.openbmc_project.State.Boot.Raw" \
    /xyz/openbmc_project/state/boot/raw xyz.openbmc_project.State.Boot.Raw \
    Value | cut -d' ' -f2)

  # Restore cursor position
  echo -en '\033[u'

  # Only execute once a second
  sleep 1
done
