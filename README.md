# Caps Block

[![Build Status](https://github.com/jdknight/capsblock/actions/workflows/build.yml/badge.svg)](https://github.com/jdknight/capsblock/actions/workflows/build.yml)

## Overview

Provides the ability in Windows to help force the Caps Lock key in a toggled
off state while still allowing programs to use the key.

This application was created with the goal of supporting the Caps Lock key
as a voice key for various voice-supporting applications. Users can use the
key to trigger a voice event, but also reduce the chance that the Caps Lock
key is left enabled.

## Install

1. Extract the `capsblock-portable.zip` package and place the contents in
   any desired directory (e.g. `C:\capsblock`).
2. *(optional)* Run the `install.bat` script to have the program start after
   logging in.
3. Run `capsblock.exe`, which will run in the background.

Users can also use `shutdown.bat` to terminate the program (or use the task
manager, if preferred). Also, an `uninstall.bat` script is provided to
unregister the application from starting when the user logs in.
