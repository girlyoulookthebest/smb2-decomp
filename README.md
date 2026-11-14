Super Monkey Ball 2 decompilation
=================================

A decompilation of *Super Monkey Ball 2* for the Nintendo GameCube, USA
release (**GM2E8P**, disc 0 revision 0). It rebuilds `main.dol` and all 20
REL modules **byte-identical** to the originals (21/21 files match
`config/GM2E8P/build.sha1`). Code that isn't decompiled yet is assembled from
disassembly that [decomp-toolkit](https://github.com/encounter/decomp-toolkit)
generates from your own disc at build time.

This repository does **not** contain any game assets, executables or
disassembly. You need your own copy of the game.

The same sources are compiled natively by the PC port,
[girlyoulookthebest/smb2-pc](https://github.com/girlyoulookthebest/smb2-pc).

Status
------

From `ninja progress` (2026-11-13):

| | Matched code | Functions |
|---|---|---|
| RELs that are being decompiled ("Game Code") | 91.96 % | 5796 / 5969 |
| All RELs | 63.32 % | 5796 / 7396 |
| `main.dol` (SDK, libraries and part of the engine) | 0 % (assembly) | 0 / 1961 |
| Everything | 56.42 % | 5796 / 9357 |

Per module (matched code):

- 100 %: `main_game.rel` (Story/Challenge flow), `sel_stage.rel`,
  `exoption.rel`, `option.rel`, `test_mode.rel`, `rel_sample.rel`, and the
  games `race2.rel` (Monkey Race), `pilot2.rel` (Monkey Target),
  `rel_mini_bowling.rel`, `boat.rel`, `dogfight.rel` and `baseball.rel`
- 94 %: `main_loop.rel`, the game engine (3193 of 3261 functions)
- 23 %: `sel_ngc.rel` (mode select menus; its C sources build for the PC port)
- not started: `main.dol`, and the RELs of Monkey Fight, Billiards, Golf,
  Shot, Soccer and Tennis

Per-file status and notes are in [PROGRESS.md](PROGRESS.md). "Matching" there
means the file links byte-identical into its module; "NonMatching" means the C
compiles and runs but some functions still differ, and the module uses the
original assembly for that file.

Building
--------

Requirements: Python 3 and [ninja](https://ninja-build.org/). On Linux and
macOS, [wibo](https://github.com/decompals/wibo) (a minimal Win32 wrapper for
the Metrowerks compilers) is downloaded automatically, as are decomp-toolkit,
the compilers, [sjiswrap](https://github.com/encounter/sjiswrap) and
[objdiff-cli](https://github.com/encounter/objdiff) (into `build/`). Windows
works natively. See [docs/dependencies.md](docs/dependencies.md).

1. Clone the repository:

   ```sh
   git clone https://github.com/girlyoulookthebest/smb2-decomp.git decomp
   cd decomp
   ```

2. Copy your disc image into `orig/GM2E8P/` (ISO/GCM, RVZ, WIA, WBFS, CISO,
   NFS, GCZ and TGC all work). After the first build it can be deleted again.

3. Configure and build:

   ```sh
   python3 configure.py
   ninja
   ```

   The build ends with a check of every output file against
   `config/GM2E8P/build.sha1` (`21 files OK`). To run the check alone:
   `build/tools/dtk shasum -c config/GM2E8P/build.sha1`.

`ninja progress` prints the numbers above. For diffing, open the project
directory in [objdiff](https://github.com/encounter/objdiff) after the first
build (it reads `objdiff.json`).

The decomp-toolkit configuration lives in `config/GM2E8P/` (`config.yml`,
`symbols.txt` and `splits.txt` per module) and `configure.py`; the files in
[docs/](docs/) describe the format.

PC port
-------

The [PC port](https://github.com/girlyoulookthebest/smb2-pc) compiles these C sources
for 32-bit and 64-bit Windows. Everything it needs on top of the GameCube code
is either inside `#ifndef __MWERKS__` blocks or uses macros from `types.h`
(for example `DPTR` for 32-bit pointer fields in disc-layout structs on the
64-bit build) that expand to nothing under the GameCube compiler. The
GameCube build therefore stays byte-identical: every change is checked with
`ninja` (21 files OK).

Credits
-------

- [decomp-toolkit](https://github.com/encounter/decomp-toolkit),
  [dtk-template](https://github.com/encounter/dtk-template) (the project
  scaffold this repository started from, CC0 1.0),
  [objdiff](https://github.com/encounter/objdiff),
  [sjiswrap](https://github.com/encounter/sjiswrap) and
  [wibo](https://github.com/decompals/wibo).
- The [Super Monkey Ball decompilation](https://github.com/camthesaxman/smb-decomp)
  (GMBE8P), used as a reference for the code SMB2 shares with the first game.
- [decomp.me](https://decomp.me) and the GC/Wii decompilation community.

Legal
-----

This is an unofficial fan project. It is not affiliated with, endorsed by or
sponsored by SEGA or Nintendo. *Super Monkey Ball* is a trademark of SEGA.
No game assets or original code are included; building requires your own
copy of the game.

License
-------

Released under the [CC0 1.0 Universal](LICENSE) public domain dedication.
