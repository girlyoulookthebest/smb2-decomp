#!/usr/bin/env python3

###
# Generates build files for the project.
# This file also includes the project configuration,
# such as compiler flags and the object matching status.
#
# Usage:
#   python3 configure.py
#   ninja
#
# Append --help to see available options.
###

import argparse
import sys
from pathlib import Path
from typing import Any, Dict, List

from tools.project import (
    Object,
    ProgressCategory,
    ProjectConfig,
    calculate_progress,
    generate_build,
    is_windows,
)

# Game versions
DEFAULT_VERSION = 0
VERSIONS = [
    "GM2E8P",  # 0
]

parser = argparse.ArgumentParser()
parser.add_argument(
    "mode",
    choices=["configure", "progress"],
    default="configure",
    help="script mode (default: configure)",
    nargs="?",
)
parser.add_argument(
    "-v",
    "--version",
    choices=VERSIONS,
    type=str.upper,
    default=VERSIONS[DEFAULT_VERSION],
    help="version to build",
)
parser.add_argument(
    "--build-dir",
    metavar="DIR",
    type=Path,
    default=Path("build"),
    help="base build directory (default: build)",
)
parser.add_argument(
    "--binutils",
    metavar="BINARY",
    type=Path,
    help="path to binutils (optional)",
)
parser.add_argument(
    "--compilers",
    metavar="DIR",
    type=Path,
    help="path to compilers (optional)",
)
parser.add_argument(
    "--map",
    action="store_true",
    help="generate map file(s)",
)
parser.add_argument(
    "--debug",
    action="store_true",
    help="build with debug info (non-matching)",
)
if not is_windows():
    parser.add_argument(
        "--wrapper",
        metavar="BINARY",
        type=Path,
        help="path to wibo or wine (optional)",
    )
parser.add_argument(
    "--dtk",
    metavar="BINARY | DIR",
    type=Path,
    help="path to decomp-toolkit binary or source (optional)",
)
parser.add_argument(
    "--objdiff",
    metavar="BINARY | DIR",
    type=Path,
    help="path to objdiff-cli binary or source (optional)",
)
parser.add_argument(
    "--sjiswrap",
    metavar="EXE",
    type=Path,
    help="path to sjiswrap.exe (optional)",
)
parser.add_argument(
    "--ninja",
    metavar="BINARY",
    type=Path,
    help="path to ninja binary (optional)",
)
parser.add_argument(
    "--verbose",
    action="store_true",
    help="print verbose output",
)
parser.add_argument(
    "--non-matching",
    dest="non_matching",
    action="store_true",
    help="builds equivalent (but non-matching) or modded objects",
)
parser.add_argument(
    "--warn",
    dest="warn",
    type=str,
    choices=["all", "off", "error"],
    help="how to handle warnings",
)
parser.add_argument(
    "--no-progress",
    dest="progress",
    action="store_false",
    help="disable progress calculation",
)
args = parser.parse_args()

config = ProjectConfig()
config.version = str(args.version)
version_num = VERSIONS.index(config.version)

# Apply arguments
config.build_dir = args.build_dir
config.dtk_path = args.dtk
config.objdiff_path = args.objdiff
config.binutils_path = args.binutils
config.compilers_path = args.compilers
config.generate_map = args.map
config.non_matching = args.non_matching
config.sjiswrap_path = args.sjiswrap
config.ninja_path = args.ninja
config.progress = args.progress
if not is_windows():
    config.wrapper = args.wrapper
# Don't build asm unless we're --non-matching
if not config.non_matching:
    config.asm_dir = None

# Tool versions
config.binutils_tag = "2.42-2"
config.compilers_tag = "20251118"
config.dtk_tag = "v1.8.3"
config.objdiff_tag = "v3.6.1"
config.sjiswrap_tag = "v1.2.2"
config.wibo_tag = "1.0.3"

# Project
config.config_path = Path("config") / config.version / "config.yml"
config.check_sha_path = Path("config") / config.version / "build.sha1"
config.asflags = [
    "-mgekko",
    "--strip-local-absolute",
    "-I include",
    f"-I build/{config.version}/include",
    f"--defsym BUILD_VERSION={version_num}",
]
config.ldflags = [
    "-fp hardware",
    "-nodefaults",
]
if args.debug:
    config.ldflags.append("-g")  # Or -gdwarf-2 for Wii linkers
if args.map:
    config.ldflags.append("-mapunused")
    # config.ldflags.append("-listclosure") # For Wii linkers

# Use for any additional files that should cause a re-configure when modified
config.reconfig_deps = []

# Optional numeric ID for decomp.me preset
# Can be overridden in libraries or objects
config.scratch_preset_id = None

# Base flags, common to most GC/Wii games.
# Generally leave untouched, with overrides added below.
cflags_base = [
    "-nodefaults",
    "-proc gekko",
    "-align powerpc",
    "-enum int",
    "-fp hardware",
    "-Cpp_exceptions off",
    "-O4,p",
    "-inline auto",
    '-pragma "cats off"',
    '-pragma "warn_notinlined off"',
    "-maxerrors 1",
    "-nosyspath",
    "-RTTI off",
    "-fp_contract on",
    "-str reuse",
    "-multibyte",  # For Wii compilers, replace with `-enc SJIS`
    "-i include",
    f"-i build/{config.version}/include",
    f"-DBUILD_VERSION={version_num}",
    f"-DVERSION_{config.version}",
]

# Debug flags
if args.debug:
    # Or -sym dwarf-2 for Wii compilers
    cflags_base.extend(["-sym on", "-DDEBUG=1"])
else:
    cflags_base.append("-DNDEBUG=1")

# Warning flags
if args.warn == "all":
    cflags_base.append("-W all")
elif args.warn == "off":
    cflags_base.append("-W off")
elif args.warn == "error":
    cflags_base.append("-W error")

# Metrowerks library flags
cflags_runtime = [
    *cflags_base,
    "-use_lmw_stmw on",
    "-str reuse,pool,readonly",
    "-gccinc",
    "-common off",
    "-inline auto",
]

# REL flags
# Verified against main_loop camera.c with GC/1.3.2 (1.3 and 2.0 also match);
# ",p" (optimize for speed) inlines register saves instead of calling _savegpr.
# -fp_contract off: event_camera_tick keeps `2.0 * r + x` unfused (fused ops there come from inline asm).
cflags_rel = [
    *[flag for flag in cflags_base if flag not in ("-O4,p", "-fp_contract on")],
    "-O1,p",
    "-fp_contract off",
    "-sdata 0",
    "-sdata2 0",
]

config.linker_version = "GC/1.3.2"


# Helper function for Dolphin libraries
def DolphinLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_base,
        "progress_category": "sdk",
        "objects": objects,
    }


# Helper function for REL script objects
def Rel(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/1.3.2",
        "cflags": cflags_rel,
        "progress_category": "game",
        "objects": objects,
    }


Matching = True                   # Object matches and should be linked
NonMatching = False               # Object does not match and should not be linked
Equivalent = config.non_matching  # Object should be linked when configured with --non-matching


# Object is only matching for specific versions
def MatchingFor(*versions):
    return config.version in versions


config.warn_missing_config = True
config.warn_missing_source = False
config.libs = [
    {
        "lib": "Runtime.PPCEABI.H",
        "mw_version": config.linker_version,
        "cflags": cflags_runtime,
        "progress_category": "sdk",  # str | List[str]
        "objects": [
            Object(NonMatching, "Runtime.PPCEABI.H/global_destructor_chain.c"),
            Object(NonMatching, "Runtime.PPCEABI.H/__init_cpp_exceptions.cpp"),
        ],
    },
    Rel(
        "main_loop",
        [
            Object(Matching, "main_loop/relocation.c", mw_version="GC/1.3.2r"),
            Object(NonMatching, "main_loop/main_loop.c"),
            Object(Matching, "main_loop/alloc.c"),
            Object(Matching, "main_loop/mode.c", mw_version="GC/1.3.2r"),
            Object(Matching, "main_loop/pause_menu.c", mw_version="GC/1.3.2r"),
            Object(Matching, "main_loop/event.c", mw_version="GC/1.3.2r"),
            Object(NonMatching, "main_loop/polydisp.c"),
            Object(Matching, "main_loop/adv.c"),
            Object(Matching, "main_loop/adv_demo.c"),
            Object(Matching, "main_loop/sel.c", mw_version="GC/1.3.2r"),
            Object(Matching, "main_loop/camera.c"),
            Object(Matching, "main_loop/frustum.c"),
            # light.c only matches with the 1.3.2r compiler (it doesn't pool float constants).
            Object(NonMatching, "main_loop/light.c", mw_version="GC/1.3.2r"),
            Object(Matching, "main_loop/render.c", mw_version="GC/1.3.2r"),
            Object(Matching, "main_loop/info.c"),
            Object(Matching, "main_loop/load.c"),
            Object(Matching, "main_loop/input.c"),
            Object(Matching, "main_loop/bitmap.c"),
            Object(Matching, "main_loop/perf.c", mw_version="GC/1.3.2r"),
            Object(NonMatching, "main_loop/sound.c"),
            Object(Matching, "main_loop/world_sound.c"),
            Object(NonMatching, "main_loop/smgr.c"),
            Object(Matching, "main_loop/adx.c"),
            Object(Matching, "main_loop/window.c"),
            Object(Matching, "main_loop/dip.c"),
            Object(Matching, "main_loop/pool.c"),
            Object(Matching, "main_loop/nl2ngc.c", mw_version="GC/1.3.2r"),
            Object(Matching, "main_loop/mot_joint.c", mw_version="GC/1.3.2r"),
            Object(Matching, "main_loop/ball.c"),
            Object(Matching, "main_loop/ball_draw.c"),
            Object(NonMatching, "main_loop/stcoli.c"),
            Object(Matching, "main_loop/world.c"),
            Object(NonMatching, "main_loop/keyframe.c"),
            Object(Matching, "main_loop/stage.c"),
            Object(Matching, "main_loop/stage_draw.c"),
            Object(Matching, "main_loop/shadows.c"),
            Object(Matching, "main_loop/stage_name.c"),
            Object(Matching, "main_loop/stage_load.c"),
            Object(NonMatching, "main_loop/recplay.c"),
            Object(NonMatching, "main_loop/cmp_recplay.c"),
            Object(NonMatching, "main_loop/effect.c"),
            Object(Matching, "main_loop/background.c"),
            Object(Matching, "main_loop/bg_jun.c"),
            Object(Matching, "main_loop/bg_snd.c"),
            Object(Matching, "main_loop/bg_wat.c"),
            Object(Matching, "main_loop/bg_spa.c"),
            Object(Matching, "main_loop/bg_sun.c"),
            Object(NonMatching, "main_loop/bg_bns.c"),
            Object(Matching, "main_loop/bg_stm.c"),
            Object(Matching, "main_loop/bg_mst.c"),
            Object(NonMatching, "main_loop/bg_pil.c"),
            Object(Matching, "main_loop/bg_end.c"),
            Object(Matching, "main_loop/bg_lava.c"),
            Object(NonMatching, "main_loop/bg_wat2.c"),
            Object(NonMatching, "main_loop/bg_pil2.c"),
            Object(Matching, "main_loop/bg_spa2.c"),
            Object(Matching, "main_loop/bg_bil2.c"),
            Object(Matching, "main_loop/bg_pot.c"),
            Object(Matching, "main_loop/bg_park2.c"),
            Object(NonMatching, "main_loop/bg_whale.c"),
            Object(Matching, "main_loop/bg_bubble.c"),
            Object(Matching, "main_loop/bg_gear.c"),
            Object(Matching, "main_loop/bg_jun2.c"),
            Object(Matching, "main_loop/bg_guest.c"),
            Object(Matching, "main_loop/bg_bns2.c"),
            Object(Matching, "main_loop/bg_bow2_author.c"),
            Object(Matching, "main_loop/course.c"),
            Object(Matching, "main_loop/item.c"),
            Object(Matching, "main_loop/item_coin.c"),
            Object(Matching, "main_loop/obj_collision.c"),
            Object(Matching, "main_loop/stobj.c"),
            Object(NonMatching, "main_loop/stobj_goal.c"),
            Object(Matching, "main_loop/stobj_returngate.c"),
            Object(NonMatching, "main_loop/sprite.c"),
            Object(NonMatching, "main_loop/textdraw.c"),
            Object(Matching, "main_loop/textbox.c"),
            Object(Matching, "main_loop/hud.c"),
            Object(Matching, "main_loop/hud_monkey.c"),
            Object(Matching, "main_loop/how_to.c"),
            Object(NonMatching, "main_loop/hud_ingame.c"),
            Object(Matching, "main_loop/minimap.c"),
            Object(Matching, "main_loop/ord_tbl.c"),
            Object(Matching, "main_loop/ape_face.c"),
            Object(Matching, "main_loop/replay_screen.c"),
            Object(Matching, "main_loop/mot_ape.c"),
            Object(Matching, "main_loop/interpolate_keyframes.c"),
            Object(Matching, "main_loop/thread.c"),
            Object(Matching, "main_loop/nl2ngc_flags.c"),
            Object(Matching, "main_loop/lzs_decompress.c"),
            Object(Matching, "main_loop/disc_load.c"),
            Object(NonMatching, "main_loop/shadow_tex.c", mw_version="GC/1.3.2r"),
            Object(Matching, "main_loop/mini_mode.c"),
            Object(Matching, "main_loop/lens_flare.c"),
            Object(Matching, "main_loop/shadow_req.c"),
            Object(Matching, "main_loop/mouse.c"),
            Object(Matching, "main_loop/rend_efc.c"),
            Object(Matching, "main_loop/rend_efc_mirror.c"),
            Object(Matching, "main_loop/rend_efc_ripple.c"),
            Object(Matching, "main_loop/rend_efc_wormhole.c"),
            Object(Matching, "main_loop/fog.c"),
            Object(Matching, "main_loop/mini_commend.c"),
            Object(Matching, "main_loop/memcard.c"),
            Object(NonMatching, "main_loop/memcard_mkb2.c"),
            Object(Matching, "main_loop/DEMOPuts.c"),
            Object(Matching, "main_loop/DEMOFont.c"),
            Object(NonMatching, "main_loop/mini_ranking.c"),
            Object(NonMatching, "main_loop/preview.c"),
            Object(NonMatching, "main_loop/name_entry.c"),
            Object(Matching, "main_loop/vibration.c"),
            Object(NonMatching, "main_loop/ending.c"),
            Object(Matching, "main_loop/ending_camera.c"),
            Object(Matching, "main_loop/ending_ballfrag.c"),
            Object(Matching, "main_loop/ending_msg.c"),
            Object(Matching, "main_loop/avdisp_wrap.c"),
            Object(Matching, "main_loop/wormhole.c"),
            Object(NonMatching, "main_loop/skeleton.c", mw_version="GC/1.3.2r"),
            Object(NonMatching, "main_loop/ape.c"),
            Object(NonMatching, "main_loop/ape_anim.c", mw_version="GC/1.3.2r"),
            Object(Matching, "main_loop/zquery.c"),
            Object(Matching, "main_loop/name_entry2.c"),
            Object(Matching, "main_loop/ape_motion.c"),
            Object(Matching, "main_loop/rc_names.c"),
            Object(NonMatching, "main_loop/scnplay.c"),
            Object(Matching, "main_loop/scnplay_draw.c"),
            Object(NonMatching, "main_loop/scnplay_fx.c"),
            Object(NonMatching, "main_loop/scnplay_effect.c"),
            Object(Matching, "main_loop/scnplay_query.c"),
            Object(NonMatching, "main_loop/dialog.c"),
            Object(Matching, "main_loop/scene_sound.c"),
            Object(Matching, "main_loop/lens_flare2.c"),
            Object(NonMatching, "main_loop/tennis_effect.c"),
            Object(Matching, "main_loop/author.c"),
            Object(Matching, "main_loop/quad_draw.c"),
            Object(Matching, "main_loop/unlock.c"),
        ],
    ),
    Rel(
        "main_game",
        [
            Object(Matching, "main_game/game_mode.c"),
            Object(Matching, "main_game/challenge.c"),
            Object(Matching, "main_game/challenge_cam.c"),
            Object(Matching, "main_game/scen_mode.c"),
            Object(Matching, "main_game/scen_disp.c"),
            Object(Matching, "main_game/scen_stage.c"),
            Object(Matching, "main_game/scen_misc.c"),
            Object(Matching, "main_game/scen_menu.c"),
            Object(Matching, "main_game/staff_roll.c"),
            Object(Matching, "main_game/game_continue.c"),
            Object(Matching, "main_game/view_stage.c"),
        ],
    ),
    Rel(
        "rel_sample",
        [
            Object(Matching, "rel_sample/rel_sample.c"),
        ],
    ),
    Rel(
        "sel_ngc",
        [
            Object(Matching, "sel_ngc/sel_ngc_rel.c"),
            Object(NonMatching, "sel_ngc/sel_ngc.c"),
            Object(NonMatching, "sel_ngc/sel_ngc_sprite.c"),
            Object(NonMatching, "sel_ngc/sel_ngc_bg.c"),
            Object(NonMatching, "sel_ngc/sel_ngc_preview.c"),
            Object(NonMatching, "sel_ngc/sel_ngc_option.c"),
        ],
    ),
    Rel(
        "sel_stage",
        [
            Object(Matching, "sel_stage/sel_stage_rel.c"),
            Object(Matching, "sel_stage/sel_stage.c"),
            Object(Matching, "sel_stage/sel_stage_draw.c"),
        ],
    ),
    Rel(
        "exoption",
        [
            Object(Matching, "exoption/exoption.c"),
            Object(Matching, "exoption/exoption_hud.c"),
        ],
    ),
    Rel(
        "option",
        [
            Object(Matching, "option/option_menu.c"),
            Object(Matching, "option/option.c"),
        ],
    ),
    Rel(
        "pilot2",
        [
            Object(Matching, "pilot2/pilot2.c"),
            Object(Matching, "pilot2/pilot2_camera.c"),
            Object(Matching, "pilot2/pilot2_effect.c"),
            Object(Matching, "pilot2/pilot2_hud.c"),
            Object(Matching, "pilot2/pilot2_trail.c"),
            Object(Matching, "pilot2/item_pilot.c"),
            Object(Matching, "pilot2/pilot2_splash.c"),
            Object(Matching, "pilot2/pilot2_stage.c"),
        ],
    ),
    Rel(
        "rel_mini_bowling",
        [
            Object(Matching, "rel_mini_bowling/bowling.c"),
            Object(Matching, "rel_mini_bowling/bowling_1.c"),
            Object(Matching, "rel_mini_bowling/bowling_2.c"),
            Object(Matching, "rel_mini_bowling/bowling_3.c"),
            Object(Matching, "rel_mini_bowling/bowling_4.c"),
            Object(Matching, "rel_mini_bowling/bowling_5.c"),
            Object(Matching, "rel_mini_bowling/bowling_6.c"),
            Object(Matching, "rel_mini_bowling/bowling_7.c"),
            Object(Matching, "rel_mini_bowling/bowling_8.c"),
        ],
    ),
    Rel(
        "test_mode",
        [
            Object(Matching, "test_mode/test_mode.c"),
            Object(Matching, "test_mode/shadow_test.c"),
            Object(Matching, "test_mode/focus_test.c"),
            Object(Matching, "test_mode/indirect_test.c"),
            Object(Matching, "test_mode/mot_edit.c"),
            Object(Matching, "test_mode/adx_test.c"),
            Object(Matching, "test_mode/newmotion_test.c"),
            Object(Matching, "test_mode/motedit.c"),
        ],
    ),
    Rel(
        "baseball",
        [
            Object(Matching, "baseball/baseball_0.c"),
            Object(Matching, "baseball/baseball_0b.c"),
            Object(Matching, "baseball/baseball_1.c"),
            Object(Matching, "baseball/baseball_2.c"),
            Object(Matching, "baseball/baseball_3.c"),
            Object(Matching, "baseball/baseball_4.c"),
            Object(Matching, "baseball/baseball_5.c"),
            Object(Matching, "baseball/baseball_6.c"),
            Object(Matching, "baseball/baseball_7.c"),
            Object(Matching, "baseball/baseball_8.c"),
            Object(Matching, "baseball/baseball_9.c"),
            Object(Matching, "baseball/baseball_10.c"),
            Object(Matching, "baseball/baseball_11.c"),
            Object(Matching, "baseball/baseball_12.c"),
            Object(Matching, "baseball/baseball_13.c"),
            Object(Matching, "baseball/baseball_14.c"),
            Object(Matching, "baseball/baseball_15.c"),
            Object(Matching, "baseball/baseball_16.c"),
            Object(Matching, "baseball/baseball_17.c"),
            Object(Matching, "baseball/baseball_18.c"),
            Object(Matching, "baseball/baseball_19.c"),
            Object(Matching, "baseball/baseball_20.c"),
            Object(Matching, "baseball/baseball_21.c"),
            Object(Matching, "baseball/baseball_22.c"),
            Object(Matching, "baseball/baseball_23.c"),
            Object(Matching, "baseball/baseball_24.c"),
            Object(Matching, "baseball/baseball_25.c"),
            Object(Matching, "baseball/baseball_26.c"),
            Object(Matching, "baseball/baseball_27.c"),
            Object(Matching, "baseball/baseball_28.c"),
            Object(Matching, "baseball/baseball_30.c"),
            Object(Matching, "baseball/baseball_32.c"),
            Object(Matching, "baseball/baseball_33.c"),
            Object(Matching, "baseball/baseball_34.c"),
        ],
    ),
    Rel(
        "boat",
        [
            Object(Matching, "boat/boat_0.c"),
            Object(Matching, "boat/boat_1.c"),
            Object(Matching, "boat/boat_2.c"),
            Object(Matching, "boat/boat_3.c"),
            Object(Matching, "boat/boat_4.c"),
            Object(Matching, "boat/boat_5.c"),
            Object(Matching, "boat/boat_6.c"),
            Object(Matching, "boat/boat_7.c"),
            Object(Matching, "boat/boat_8.c"),
            Object(Matching, "boat/boat_9.c"),
            Object(Matching, "boat/boat_10.c"),
            Object(Matching, "boat/boat_11.c"),
            Object(Matching, "boat/boat_12.c"),
            Object(Matching, "boat/boat_13.c"),
            Object(Matching, "boat/boat_14.c"),
            Object(Matching, "boat/boat_15.c"),
        ],
    ),
    Rel(
        "race2",
        [
            Object(Matching, "race2/race2_0.c"),
            Object(Matching, "race2/race2_1.c"),
            Object(Matching, "race2/race2_2.c"),
            Object(Matching, "race2/race2_3.c"),
            Object(Matching, "race2/race2_4.c"),
            Object(Matching, "race2/race2.c"),
            Object(Matching, "race2/race2_ape.c"),
            Object(Matching, "race2/race2_5.c"),
            Object(Matching, "race2/race2_6.c"),
        ],
    ),
    Rel(
        "dogfight",
        [
            Object(Matching, "dogfight/dogfight.c"),
            Object(Matching, "dogfight/dogfight_1.c"),
            Object(Matching, "dogfight/dogfight_2.c"),
            Object(Matching, "dogfight/dogfight_3.c"),
            Object(Matching, "dogfight/dogfight_4.c"),
            Object(Matching, "dogfight/item_dogfight.c"),
            Object(Matching, "dogfight/dogfight_6.c"),
            Object(Matching, "dogfight/dogfight_7.c"),
        ],
    ),
]


# Optional callback to adjust link order. This can be used to add, remove, or reorder objects.
# This is called once per module, with the module ID and the current link order.
#
# For example, this adds "dummy.c" to the end of the DOL link order if configured with --non-matching.
# "dummy.c" *must* be configured as a Matching (or Equivalent) object in order to be linked.
def link_order_callback(module_id: int, objects: List[str]) -> List[str]:
    # Don't modify the link order for matching builds
    if not config.non_matching:
        return objects
    if module_id == 0:  # DOL
        return objects + ["dummy.c"]
    return objects


# Uncomment to enable the link order callback.
# config.link_order_callback = link_order_callback


# Optional extra categories for progress tracking
# Adjust as desired for your project
config.progress_categories = [
    ProgressCategory("game", "Game Code"),
    ProgressCategory("sdk", "SDK Code"),
]
config.progress_each_module = args.verbose
# Optional extra arguments to `objdiff-cli report generate`
config.progress_report_args = [
    # Marks relocations as mismatching if the target value is different
    # Default is "functionRelocDiffs=none", which is most lenient
    # "--config functionRelocDiffs=data_value",
]

if args.mode == "configure":
    # Write build.ninja and objdiff.json
    generate_build(config)
elif args.mode == "progress":
    # Print progress information
    calculate_progress(config)
else:
    sys.exit("Unknown mode: " + args.mode)
