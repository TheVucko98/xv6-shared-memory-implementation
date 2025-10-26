# OS2025 - Home Assignment 1: Theme Color Customization in xv6

## Overview
This project modifies the xv6 operating system to support dynamic theme changes, allowing users to customize the foreground and background colors of the entire screen via an interactive table menu.
The new functionality is integrated carefully to ensure it does not interfere with normal system operations or performance.

## Features
- **Display/Hide Theme Table:** `ALT + T`
- **Navigate Table:**
  - `w` – move up
  - `s` – move down
- **Color Combinations:**
  1. Light Gray + Black (`lgr blk`)
  2. Magenta + White (`mgt WHT`)
  3. Red + Bright Cyan (`red CYN`)
  4. White + Yellow (`WHT YWL`)
- **Highlighting:** Selected item shows black text on green background.
- **Persistence:** Last selected theme is remembered and pre-selected on reopening.
- **Dynamic Positioning:** Table appears near the cursor, adjusting left/right if needed.

## Installation
1. Clone the repository:
   ```bash
   git clone <repo-link>
   cd <repo-name>
   cd projectName
   make qemu
