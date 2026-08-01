# AGENTS.md

## Overview

Sample applications project for CH552E, microcontroller manufactured by WCH.

## Usage

CMake target `target` is configured.

```sh
cmake --preset target
```

```sh
cmake --build --preset target 
```

## Flash

You can use `wchisp` with:

```sh
wchisp flash build/target/path/to/binary.ihx
```

You can see help:

```sh
wchisp --help
wchisp flash --help
```

**NOTE**:  
To write the binary, the MCU must be in DFU mode.
If the write operation fails, please ask the user to verify that the MCU is in DFU mode.
