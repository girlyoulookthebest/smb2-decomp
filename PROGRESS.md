# Decomp progress (GM2E8P)

Overall (from `ninja progress`, 2026-11-01): 3471 of 9357 functions matched across the game,
34.54% of code matched and 20.37% linked (109 of 527 files).

main_game.rel: complete. All 11 TUs are Matching and all 282 functions build from C; the module
links byte-identical to `mkb2.main_game.rel`.

main_loop.rel: 1,401,136 of 1,496,104 code bytes matched, 93.65%, 3189 of 3261 functions. All of
`.text` is now split into source files; the head of the module (heaps, pause menu, events, draw
handlers, adv and sel modes) was the last part to be split.

"Matching" = links byte-identical into the REL. "NonMatching" = decompiled C that
compiles but differs from the original in the functions listed.

| File | Text range | Status | Remaining |
|---|---|---|---|
| camera.c | | NonMatching (107/108) | g_camera_func2 (one extra fmr) |
| frustum.c | | Matching | |
| light.c | | NonMatching (22/23) | light_main (base register swap) |
| render.c | | Matching | |
| info.c | | Matching | |
| load.c | | Matching | |
| input.c | | Matching | |
| bitmap.c | | Matching | |
| perf.c | | Matching | |
| sound.c | 0x2C118-0x371C8 | NonMatching (101/106) | SoundGroupLoad, g_smth_calls_sndFXStartParaInfo, fn_1_2F8F0, fn_1_3061C (extra mr on table address only), SoundIcsReq (register allocation/scheduling only) |
| world_sound.c | 0x371C8-0x375A8 | Matching | |
| smgr.c | 0x375A8-0x39CE8 | NonMatching (21/22) | g_smgr_allocate_stereo (left/id and channelL swap r27/r29; decl order does not help) |
| adx.c | 0x39CE8-0x3BA00 | Matching | |
| window.c | 0x3BA00-0x40138 | Matching (29/29) | |
| dip.c | 0x40138-0x40484 | Matching | |
| pool.c | 0x40484-0x408A0 | Matching | |
| nl2ngc.c | 0x408A0-0x44DC8 | Matching (33/33) | needs mw_version GC/1.3.2r |
| mot_joint.c | 0x44DC8-0x456F4 | Matching (11/11) | GC/1.3.2r; SMB1 mot_joint + scale/linear/step keyframes |
| ball.c | 0x456F4-0x4CEDC | NonMatching (63/64) | fn_1_4AF48 (8-byte stack pad); ball_mode_funcs + data up to 0x2E700 not yet defined |
| ball_draw.c | 0x4CEDC-0x4F710 | NonMatching (19/20) | fn_1_4E810 (inlined fn_1_4E12C model in r30 instead of r29) |
| stcoli.c | 0x4F710-0x54B64 | NonMatching (29/34) | collide_ball_with_stage, collide_ball_with_tri_face, collide_ball_with_tri_edges, collide_ball_with_plane, raycast_tri (FP/GPR register allocation only; decl-order search does not help plane) |
| world.c | 0x54B64-0x55D34 | Matching | |
| keyframe.c | 0x55D34-0x55EB4 | NonMatching (0/1) | evaluate_stagedef_keyframe (FP register allocation in the Hermite case only) |
| stage.c | 0x55EB4-0x5913C | Matching (43/43) | SMB1 stage.c plus SMB2 seesaws, stage fog/light keyframe animation, model LOD selection |
| stage_draw.c | 0x5913C-0x5BB14 | Matching (24/24) | SMB1 stage_draw: stage models, goals, wormholes, generators, start marker, stcoli debug draw; SMB2 st348/meltingpot/totalitarianism/st340/trainworm special cases |
| shadows.c | 0x5BB14-0x5CB48 | Matching (9/9) | SMB2-only stage model/goal/wormhole shadows, render-to-texture per caster in single player; g_draw's declaration order was found by search, do not reorder |
| stage_name.c | 0x5CB48-0x5CF98 | Matching (7/7) | per-language /stgname/<lang>.str name lists; bss ends with 0x18 unreferenced bytes |
| stage_load.c | 0x5CF98-0x5FBE4 | Matching (5/5) | STAGE%03d.lz loader, offset-to-pointer fixups and model binding (file strings call it "stcoli.c"). fn_1_5CF50 is inlined here, so the true TU boundary is probably 0x5CF50; generator alpha fixup tests green.frames (original bug) |
| recplay.c | 0x5FBE4-0x63F8C | NonMatching (56/58) | replay recording/storage/playback; g_smth_with_replay_playback differs by register colouring only (id/replay swapped) |
| cmp_recplay.c | 0x63F8C-0x68078 | NonMatching (45/47) | replay compare/RLE/init; init_replay and fn_1_66EC4 differ by register colouring only; instruction selection and stack match |
| effect.c | 0x68078-0x6F824 | NonMatching (96/99) | effect pool and all effect types. `.rodata` 904/904 bytes. Left: effect_bns_stg_star_disp (GPR colouring, reversed), effect_commendfrag_tick (one scratch FPR), effect_particle_tick_coli (2 branch offsets). bgbtmfire_ptcl and piyo are SMB2-only |
| background.c | 0x6F824-0x739E8 | Matching (79/79) | world themes: per-theme init/tick/draw, model anim, flipbooks, shadow groups, texture scroll. `.rodata`, `.data` and `.bss` match. |
| bg_jun.c | 0x739E8-0x740D8 | Matching (6/6) | jungle theme: scrolling clouds. `include/background.h` now holds the shared theme declarations. |
| bg_snd.c | 0x740D8-0x76CE0 | Matching (14/14) | sand theme: sandstorm, heat haze render effect, fountain env-map callbacks. Some rodata floats are unreferenced and are forced by dead locals. |
| bg_wat.c | 0x76CE0-0x78948 | Matching (13/13) | water theme: caustics, lightmap env callbacks, bubble effect hook. SMB2 binds a second tex map and texMapId advances by 3. |
| bg_spa.c | 0x78948-0x79F98 | Matching (8/8) | space theme: meteors, Saturn indirect-warp env callback. Effect fields shifted +4 vs SMB1 from unk94 on; ET_METEO=8, ET_METEO_FIX=9. |
| bg_sun.c | 0x79F98-0x7A4FC | Matching (6/6) | sunset theme: scrolling cloud layers; timer boost 11s before time over. |
| bg_bns.c | 0x7A4FC-0x7B1CC | NonMatching (8/9) | bonus theme: starpoints, star-field effect; bg_bns_disp blocked on colouring only (target pool r29, ours r31; 17 reg diffs, instruction count equal) |
| bg_stm.c | 0x7B1CC-0x7BF10 | Matching (6/6) | storm theme: wrapped rain field, ripples spawned on raycast hits; adds mathutil_ceil |
| bg_mst.c | 0x7BF10-0x7D2D0 | Matching (8/8) | master theme, written from asm (SMB1 has no C for it): cloud layers, paradise clouds shedding mist, water env. Original caps counts at 8 though clouds[] holds 4 and paradises[] 16 |
| bg_pil.c | 0x7D2D0-0x7E700 | NonMatching (7/8) | pilot theme, written from asm: scrolling sea layers, reflection render effect, indirect water env; bg_pil_disp blocked on float colouring only (20 diffs: target `d` f1, ours f2) |
| bg_end.c | 0x7E700-0x7F278 | Matching (9/9) | ending theme: env-map water and clouds, paradise fountain effects |
| bg_lava.c | 0x7F278-0x83238 | Matching (24/24) | lava theme: scrolling lava, skinned fountains, fire bursts, screen glow, wrapping embers |
| bg_wat2.c | 0x83238-0x8A030 | NonMatching (42/43) | water theme 2: sea surface, caustics, poko lights, bubbles, sail and rope physics. fn_1_83B84 differs only in register allocation; .rodata and .data match |
| bg_pil2.c | 0x8A030-0x8C6D0 | NonMatching (11/14) | target theme: sea reflection, lamps, specular beams. Leftovers are register allocation: fn_1_8B45C (16) and fn_1_8B984 (67, also unwanted constant propagation of the zero Vecs), both with the dot-product FPR choice shared with bg_pil_disp; fn_1_8BCA0 (65, rodata base needs r31, no declaration order reaches it). .rodata and .data match |
| bg_spa2.c | 0x8C6D0-0x8E964 | Matching (34/34) | Coronation space bg: shadow pass, dome spec pass, cloud layers |
| bg_bil2.c | 0x8E964-0x8F1A4 | Matching (10/10) | Billiards bg: BILL_HIKARI light models, scrolling tex + indirect warp |
| bg_pot.c | 0x8F1A4-0x94B58 | Matching (49/49) | Boiling Pot bg: sun flare w/ Z-peek, leaves, pots+bubbles+glow, steam vents, haze quads, cloud texture render effect |
| bg_park2.c | 0x94B58-0x94E74 | Matching (7/7) | Second park theme (source name bg_park.c): PAR_PLANE models registered as a shadow group |
| bg_whale.c | 0x94E74-0x98F30 | NonMatching (34/35) | Whale theme: reflection render-to-texture, bone matrices, wave/scroll tev setup; fn_1_974DC is a 27-instruction register-allocation near-miss |
| bg_bubble.c | 0x98F30-0x9D904 | Matching (39/39) | Bubble theme: wind field, soap bubbles with frame-buffer refraction, laundry cloth bone physics, spa ripples tracked from ape bones, caustic water surface, soap film overlay |
| bg_gear.c | 0x9D904-0x9F340 | Matching (18/18) | Gear theme: river surface with mirrored off-screen reflection render (256x256 copy-to-texture), two scrolling ripple indirect layers, sun glint projected via lookat texture matrix |
| bg_jun2.c | 0x9F340-0x9FE94 | Matching (12/12) | Jungle 2 theme background |
| bg_guest.c | 0x9FE94-0xA0928 | Matching (8/8) | Background spectator apes (Billiards / Bonus themes) |
| bg_bns2.c | 0xA0928-0xA1B9C | Matching (21/21) | Bonus stage 2 background (glass, sky light, star flares, clouds) |
| bg_bow2_author.c | 0xA1B9C-0xA30C4 | Matching (16/16) | Bowser 2 "author" bg: AUT_PIPE_ skinned-bone rope chains w/ gravity, stage collision, spark effects |
| course.c | 0xA30C4-0xA5220 | Matching (47/47) | course script interpreter (IF/THEN commands, floor jumps), Challenge Mode progress/unlock bits, Story Mode world state + unlock bits, practice-mode stage info; .data/.rodata/.bss byte-identical |
| item.c | 0xA5220-0xA62B8 | Matching (16/16) | stage item pool (bananas etc.): per-type init/tick/disp/coli/dest/replay/debug function tables (coin, pilot2.rel, dogfight.rel), item group world-position + downward raycast, blob shadow requests, collision-header banana spawning; .data/.rodata/.bss byte-identical |
| item_coin.c | 0xA62B8-0xA6EE8 | Matching (8/8) | banana items (single + bunch): init/tick/disp/coli/dest/replay/debug, HUD banana effect spawn on collect, model LOD picker (g_something_with_coins); .data/.rodata byte-identical |
| obj_collision.c | 0xA6EE8-0xA7894 | Matching (7/7) | ball vs. stage-object collision helpers |
| stobj.c | 0xA7894-0xAA884 | Matching (44/44) | stage-object pool: spawn/tick/draw, bumpers, jamabars, bumper LOD table |
| stobj_goal.c | 0xAA884-0xAE1F0 | NonMatching (25/26) | goal tapes and goal bags: rope physics, party-ball open/confetti, reset on replay. stobj_goalbag_coli differs by 8 words |
| stobj_returngate.c | 0xAE1F0-0xAE518 | Matching (7/7) | return-gate stage objects (wormhole rows with flag 1): push the ball off the gate plane |
| sprite.c | 0xAE518-0xB3E14 | NonMatching (72/74) | sprite pool and tree draw, text/bmp sprite draw, font width tables, textdraw, screen fade, sprite draw-request buffer, UI widescreen matrix |
| textdraw.c | 0xB3E14-0xB7760 | NonMatching (20/21) | font loading, ASCII/SJIS text parsing and glyph drawing with outline/shadow, ARAM font page cache. textdraw_chara_load differs only by a four-register rotation: target font r31, i r30, data r29, bss r28; ours bss r31, font r30, i r29, data r28 |
| textbox.c | 0xB7760-0xBA01C | Matching (11/11) | message boxes: line queues, open/scroll/close states, frame and text drawing. The box array stays global because the HUD sprite code at 0xC34DC reads it. |
| hud.c | 0xBA01C-0xC2550 | Matching (68/68) | HUD sprites: banners, pause menu, score tally, continue/retry prompts, staff credits, save prompt |
| hud_monkey.c | 0xC2550-0xC3D84 | Matching (2/2) | monkey face icons by the lives counter |
| how_to.c | 0xC3D84-0xC7F74 | Matching (8/8) | how-to-play screen: page state machine, controller tooltips, rules pages, per-mode text tables |
| hud_ingame.c | 0xC7F74-0xCAE1C | NonMatching (22/23) | in-game HUD: score, timer, speed, stage name, banana and monkey counters, final stage banner |
| minimap.c | 0xCAE1C-0xCCAA0 | Matching (8/8) | circular stage minimap: open/close/zoom states, tilted stage render into a scissored viewport, ball icon and speech bubble, depth-quad clears |
| ord_tbl.c | 0xCCAA0-0xCD4C8 | Matching (15/15) | depth-ordered draw list: entries per view, node bump allocator (0x20000 pool, SMB1 ord_tbl.c). |
| ape_face.c | 0xCD4C8-0xCEDC0 | Matching (9/9) | ape face/eye model drawing and morphing; morph tables in `ape_face_rodata.inc`, `ape_face_data.inc` (`#pragma peephole off`) |
| replay_screen.c | 0xCEDC0-0xD1F0C | Matching (23/23) | ranking screen (falling letters, record rows, bars) and the replay stage-name banner; tables in `replay_screen_data.inc`. The file name is a placeholder (`#pragma peephole off`) |
| mot_ape.c | 0xD1F0C-0xD2550 | Matching (18/18) | ape LOD selection, head tracking and rotation helpers (`#pragma peephole off`) |
| interpolate_keyframes.c | 0xD2550-0xD26B8 | Matching (1/1) | Hermite curve evaluation over integer-frame keyframes (`#pragma peephole off`) |
| thread.c | 0xD26B8-0xD2A88 | Matching (6/6) | small cooperative thread lists: 16 priority heads, a free list, run/create/kill |
| nl2ngc_flags.c | 0xD2A88-0xD2B3C | Matching (3/3) | mask-and-set NL mesh flag words across a model |
| lzs_decompress.c | 0xD2B3C-0xD2CA4 | Matching (1/1) | LZSS decompressor, ring buffer is N+F-1 bytes (SMB1 lzs_decompress.c) |
| disc_load.c | 0xD2CA4-0xD3A40 | Matching (23/23) | Background disc queue reading files into an ARAM file cache in 0xF000 byte chunks, plus file_open/read/close. Original name is also load.c per its panic strings, but that name is taken by the asset loader TU |
| shadow_tex.c | 0xD3A40-0xD5CB0 | NonMatching (16/20) | blob shadow casters: request queue, per-caster culling and matrices, ramp textures, projected receiver draw, debug view. Needs `mw_version="GC/1.3.2r"` (no float pooling); `#pragma peephole off` |
| mini_mode.c | 0xD5CB0-0xD68D4 | Matching (16/16) | minigame sub mode: rel loading and callbacks, debug select menu, ending viewer (name from its OSPanic; SMB1 mini.c). The 0x170 .data table at the start is unreferenced by code and kept alive from a dead static (`#pragma peephole off`) |
| lens_flare.c | 0xD68D4-0xD7194 | Matching (7/7) | sun position, per-camera flare sprites and occlusion by balls, full-screen wash (SMB1 mini_2) |
| shadow_req.c | 0xD7194-0xD76A4 | Matching (5/5) | blob shadow request queue and drawing (SMB1 mini_3) |
| mouse.c | 0xD76A4-0xD7EBC | Matching (4/4) | debug cursor and sprite dragging (SMB1 mouse.c) |
| rend_efc.c | 0xD7EBC-0xDAC20 | Matching (23/23) | render effect slots, blur, motion blur, focus, kaleidoscope (SMB1 rend_efc.c) |
| rend_efc_mirror.c | 0xDAC20-0xDD65C | Matching (17/17) | flat and wavy reflection render effects: mirrored camera pass, tex copy, projected indirect tev |
| rend_efc_ripple.c | 0xDD65C-0xDF26C | Matching (11/11) | Pilot sea reflection: mirrored scene render, indirect water layers, ball ripples |
| rend_efc_wormhole.c | 0xDF26C-0xE2918 | Matching (22/22) | Wormhole surfaces: view through the destination rendered to texture, ripple particles, warp flash |
| fog.c | 0xE2918-0xE3908 | Matching (16/16) | Fog state per theme/stage, GX line drawing helpers |
| mini_commend.c | 0xE3908-0xE7680 | Matching (35/35) | Podium "commend" scene after mini games: label text, apes, confetti |
| memcard.c | 0xE7680-0xF00F0 | Matching (100/100) | memory-card state machine, save/load jobs, message and progress UI |
| memcard_mkb2.c | 0xF00F0-0xF0AEC | NonMatching (12/13) | save game data glue, stats blob, replay file packaging; g_some_replay_func differs by register placement only |
| DEMOPuts.c | 0xF0AEC-0xF1384 | Matching (5/5) | Dolphin SDK debug font and ROM font text; DEMOFont.c holds the bitmap |
| mini_ranking.c | 0xF1384-0xF99D4 | NonMatching (39/41) | ranking screen sprites and draws, record tables, name/time/character rows |
| preview.c | 0xF99D4-0xFA670 | NonMatching (9/10) | streamed preview textures from disc, quad draw, DIP_PREVIEW_SAVE frame dump |
| name_entry.c | 0xFA670-0xFD25C | NonMatching (30/32) | ranking name entry stage: buttons, score records, memcard records, name entry effects and stobjs |
| vibration.c | 0xFD25C-0xFD64C | Matching (6/6) | controller rumble event and command queue |
| avdisp_wrap.c | 0x1097A0-0x10AA54 | Matching (39/39) | avdisp heap setup, render state, deferred translucent models |
| wormhole.c | 0x10AA54-0x10B304 | Matching (9/9) | teleports a ball and its camera between wormholes |
| skeleton.c | 0x10B304-0x10EB4C | Partial (23/24) | Skeletal animation: bone channels, IK solver, MTA/string tables, ray/circle helpers |
| ape.c | 0x10EB4C-0x114C1C | Partial (63/66) | Ape lifecycle, model and MTA loading, drawing, per-frame body animation. Built with GC/1.3.2 |
| ape_anim.c | 0x114C1C-0x117DB0 | Partial (27/32) | MAL clip groups, per-bone keyframe sampling. Built with GC/1.3.2r |
| zquery.c | 0x117DB0-0x118274 | Matching | Queued GXPeekZ reads for occlusion tests. |
| ape_motion.c | 0x13FF3C-0x141204 | Matching | Per-animation state machines for the monkey in the ball. |
| rc_names.c | 0x6EC50-0x6ECD8 (data) | Matching | Data-only name table |
| scnplay.c | 0x141204-0x1439F4 | NonMatching (27/28) | Scene/cutscene player |
| scnplay_draw.c | 0x1439F4-0x143CC0 | Matching | Cutscene draw pass (own TU: its two floats precede scnplay_fx.c's pool, whose base is 0x661B8) |
| scnplay_fx.c | 0x143CC0-0x152BE8 | 121/124 | SOFTFX task list, one handler and draw per effect type; one TU with the former scnplay_softfx.c |
| unlock.c | 0x16C30C-0x16D428 | Matching | Play points, gifts and unlockables (all 49 functions) |
| scnplay_query.c | 0x15F720-0x15FB94 | Matching | Per-scene state queries for the cutscene player |
| scnplay_effect.c | 0x152BE8-0x15F720 | 132/137 | Scene effect callbacks (init/tick/disp/draw) plus .rodata 0x66408-0x66620 |
| dialog.c | 0x15FB94-0x162B3C | 28/30 | Cutscene dialog boxes, text reveal and layout |
| relocation.c | 0x0-0x380 | Matching | Module prolog/epilog and relocation hooks |
| main_loop.c | 0x380-0xE0C | NonMatching (13/14) | Module entry and main loop |
| alloc.c | 0xE0C-0x16E8 | Matching | Heap setup |
| mode.c | 0x16E8-0x2CA0 | Matching | Game mode dispatch |
| pause_menu.c | 0x2CA0-0x5038 | Matching | Pause menu |
| event.c | 0x5038-0x5388 | Matching | Event table |
| polydisp.c | 0x5388-0xA38C | NonMatching (34/35) | Mode draw handlers |
| adv.c | 0xA38C-0xEDC4 | Matching | Attract mode sub modes |
| adv_demo.c | 0xEDC4-0x121B4 | Matching | Attract mode intro cutscene |
| sel.c | 0x121B4-0x12D5C | Matching | Mode select sub modes |
| scene_sound.c | 0x162B3C-0x16373C | Matching | Per-cutscene streams, sound groups and volume fades |
| lens_flare2.c | 0x16373C-0x163B5C | Matching | Scene lens flare, caller-supplied view matrix |
| tennis_effect.c | 0x163B5C-0x169980 | NonMatching (55/56) | Tennis mini-game effects |
| ending.c | 0xFD64C-0x1082D4 | NonMatching (59/62) | Ending cutscene: state machine, characters, falling debris, screen-copy fade |
| ending_camera.c | 0x1082D4-0x108CF8 | Matching | Ending cutscene camera (chara/velocity eye and look-at modes) |
| ending_ballfrag.c | 0x108CF8-0x1090E0 | Matching | Ending ball fragment effect |
| ending_msg.c | 0x1090E0-0x1097A0 | Matching | Ending message tables and %-substitution |
| name_entry2.c | 0x118274-0x13FF3C | Matching | Name entry and rankings |
| author.c | 0x169980-0x16B00C | Matching | Author mode cutscene viewer and playback |
| quad_draw.c | 0x16B00C-0x16C30C | Matching | Flat coloured quads, single and batched |

## Next up
- textdraw_chara_load: structure identical, 56 words of register rotation. 4600 random declaration orders never beat it. Pulling the width code into `chara_width()` moved font/i above data but not above bss, so more source structure (a further inline or merged local) is missing.
- hud_ingame.c create_hud_stage_name_sprites: instruction stream identical, 67 words of r30/r31 swap between the .data and .rodata pool bases. All 120 orderings of the integer locals and dead trailing float statements leave it unchanged.
- sprite.c fn_1_AED58 and create_linked_sprite: inlined create_sprite. Target tests r3 before copying (`cmpwi r3,0; bge; li rX,0; b; mr r29,r3`), ours copies first (`mr rN,r3; cmpwi rN,0`).
- stobj_goalbag_coli (stobj_goal.c): one register swap left. Target has stobj r29, ball r28, count r28, step r29 in both ring loops. One count/step pair gets the loop registers right but swaps stobj and ball; two pairs (current source) fix stobj/ball but swap count/step in the first loop. All declaration orders/scopes, shared-variable partitions and rand/type/pid temporaries were tried. stobj.c, obj_collision.c and stobj_returngate.c already link from C; flip stobj_goal.c once this resolves.
- recplay colouring blockers above. init_replay and fn_1_66EC4 share one rotation: target ranks frames over the inlined size/tracks. Inline declaration order and inline shape changes did not fix it.
- Before flipping ball.c/ball_draw.c to Matching: define ball_mode_funcs and remaining ball.c .data, drop the bogus rodata symbols visual_ball_size/ball_friction, split the 2E8F0 symbol (size 4, plus 2E8F4/2E8F8), and confirm fn_1_ball_draw_stripped is stripped at link.
- shadow_tex.c leftovers: fn_1_D4870 (15 words, target alpha f1 and temps f2, mine swapped); g_some_shadow_draw_func (8 words, `GXColor color` sp+8 vs by-value temp sp+0xC, mine swapped); g_check_some_condition (7 words, base r5 and ret r4, mine swapped); fn_1_D53E0 (target keeps two dead `addi r28,r28,1` after the second-stage increments, ~35 dead-use forms all drop them, and const-flag `if`s are not folded in this TU).
- memcard_mkb2.c g_some_replay_func: 24 words. Target loads the .data pool base at entry into r29 (outSize r31, outBuffer r30); ours loads it at first use into r31. A live literal local moves the load to entry but adds a dead-looking addi and shifts regs; dead forms (unused locals, `if (0)`, inline with constant arg, expression statements, offset-0 table refs) all leave it unchanged.
- mini_ranking.c fn_1_F36F8 and fn_1_F6A9C: instruction stream identical, 53 words of r28/r29 swap (target data pool r29, rec r28). Moving the icon draw into inline fn_1_row_icon and declaring `i` first fixed the rodata pool and i. Temporarily flipping to Matching links with only these two functions differing, same size.
- preview.c fn_1_F9EAC: only entry order left (9 words). Target copies x and y in parameter order with the others; `x1 = x; y1 = y` places those two fmr after the int moves. Using x and y directly fixes that but ranks them below width/height.
- name_entry.c event_name_entry_init: only r27/r28 swapped between buttonModel and modelIter (10 words). effect_get_nameent_code_disp: one word, `add r3,r0,r3` vs `add r3,r3,r0`; every expression form that keeps the add before the subi and drops the extsh commutes the operands. Trailing 4 bytes of .data are padding.
- `skeleton.c` `fn_1_10E5B8`: 19 words; float register assignment only (t1/root/x colors), logic is right. Rodata is also 8 bytes short (0.0f at 0x654B8 plus pad).
- `ape.c` `g_load_ape_mtas`, `load_ape`, `g_load_mal_files_from_disc`: register assignment of the data, bss and rodata pool bases only; instructions match after renaming.
- `ape_anim.c` `g_alloc_some_ape_state_0x8`, `g_ape_anim_unk3`: `lwz r0,8(r3); mr r3,r0` in the clip size lookup. `g_set_ape_dance_state2`, `g_some_ape_anim_func2`, `fn_1_1175A0`: register choices.
- scnplay.c: fn_1_142720 differs by 7 words (register coloring in the case 17 colour packing)
- tennis_effect.c fn_1_166864: 66 words, float register assignment only: target keeps the racket/normal dot `d` in f1 with temps in f0, ours swaps them. Declaration order, an inline for the dot and `register` have no effect. Linked as Matching, the rel differs only in those 66 words, so the TU can flip as soon as this function matches.
- ending.c: fn_1_103E44 (21 words, swapped r28/r30 for player and i; declaration order has no effect), fn_1_104C6C (44 words, float temp in f4 instead of f0), fn_1_1053E4 (6 words, `x + (y + 0x4000)` reassociated by our build).
- `scnplay_effect.c`: `fn_1_153D8C`, `fn_1_15CF88`, `fn_1_15C3E4` are the same GPR rotation (target effect r29, model r30, pool r31), `fn_1_156A34` is a float rotation (13 words). Code is otherwise exact. `fn_1_15CCEC` needs its own asm block because the inline helper copies `b`.
- `dialog.c` `fn_1_161944` and `fn_1_1625F4` differ only in register assignment.
- `scnplay_effect.c`: `fn_1_152BE8` is one instruction long: target loads the table base straight into r31, a local pointer gives `addi r0; mr r31, r0`, and static or function-local tables do not pool.
- `scnplay_fx.c`: `fn_1_14695C`, `fn_1_147B5C`, `fn_1_14CA74` (register allocation only).
- `recplay.c` compiles 156 bytes over target in `.text`: 13 `_force_bss_order_*` helper functions at 12 bytes each, kept only to order `.bss`. Its `.data` is also 136 bytes against 129. These must go before it can link. `ape.c` has the same kind of helpers (`ape_bss_order`, `ape_data_order`, `ape_rodata_order`, `ape_unused_literal`, `ape_unused_strings`).
- `sound.c` `g_smth_called_by_event_tick_sound_2` shows 99.98% in `report.json`; `tools/cmpfn.py` reports a match, so only a relocation differs.
- Everything not listed in `config/GM2E8P/main_loop/splits.txt` is still assembly.

## main_game.rel

| File | Status | Notes |
|---|---|---|
| game_mode.c | Matching | Game mode state machine |
| challenge.c | Matching | Challenge mode |
| challenge_cam.c | Matching | Challenge camera |
| scen_mode.c | Matching | Story mode flow |
| scen_disp.c | Matching | Story mode drawing |
| scen_stage.c | Matching | Story stage select |
| scen_misc.c | Matching | Story mode helpers |
| scen_menu.c | Matching | Story mode menu |
| staff_roll.c | Matching | Staff roll minigame and results (52 functions) |
| game_continue.c | Matching | Continue screen |
| view_stage.c | Matching | Stage viewer |

## Other RELs

| Module | Status | Notes |
|---|---|---|
| rel_sample.rel | Matching | Relocation sample |
| sel_stage.rel | Matching | 3 TUs: sel_stage_rel, sel_stage, sel_stage_draw |
| exoption.rel | Matching | Replay viewer: exoption.c, exoption_hud.c |
| option.rel | Matching | Options menu: option_menu.c, option.c |
| pilot2.rel | Matching | Monkey Target: pilot2.c, pilot2_camera, pilot2_effect, pilot2_hud, pilot2_splash, pilot2_stage, pilot2_trail, item_pilot |
| dogfight.rel | Matching (REL byte-identical; 2 functions NonMatching in source) | Monkey Fight: dogfight.c, dogfight_1, dogfight_2, dogfight_3, dogfight_4, item_dogfight, dogfight_6, dogfight_7. Remaining: `fn_17_9D60` (dogfight_2, regs, 19 words; rodata 276 vs 280), `fn_17_E778` (dogfight_3, 8 words) |
| rel_mini_bowling.rel | Matching (REL byte-identical) | Monkey Bowling: bowling.c (main, 60 functions) and bowling_1-8.c |
| test_mode.rel | Matching (REL byte-identical; shadow_test.c NonMatching, 1 function) | Developer test menus: test_mode.c (93 functions), shadow_test, focus_test, indirect_test, mot_edit, adx_test, newmotion_test, motedit |
| race2.rel | Mostly Matching (REL byte-identical; 7/9 TUs Matching, 313/315 functions) | Monkey Race 2: `race2_1`–`race2_4`, `race2`, `race2_ape`, `race2_6` Matching; `race2_0` 47/48 (`fn_10_5968`, 1 word) and `race2_5` 73/74 (`fn_10_20E3C`, 1 word) link from target objects. `.data` strings `WINSTAGE_*` live in `race2_5`: no TU may start `.data` at 2D7C (mwcc 8-aligns `.data`). |
| golf2.rel | Matching (REL byte-identical; 7/7 TUs Matching, 145/145 functions) | Monkey Golf 2: `golf.c` (main mode tick, draw, turns, shot arrow, wipes; functions in include/golf2/golf_*.inc fragments), `golf_1` (ball physics, shot preview), `golf_2` (golfer swing poses), `golf_3`, `golf_4` (HUD: portraits, scorecard, shot panel, power gauge, wind), `golf_5` (camera), `golf_6` (ground pattern TEV). `fn_11_2E638`'s state is file statics pinned by an unreferenced function (as function-local statics MWCC lays them out in first-use order). golf_4 has stand-ins for two linker-stripped functions (rodata 0x2D8, 0x4DC) |
| boat.rel | Mostly Matching (REL byte-identical; 14/16 TUs Matching, 326/331 functions) | Monkey Boat: `boat_0`–`boat_15`. Matching except `boat_5.c` (`fn_14_115F4`, `fn_14_11818`, `fn_14_11CE8`, `fn_14_11F0C`) and `boat_6.c` (`fn_14_13144`) |

## Where things live
- File boundaries: `config/GM2E8P/main_loop/splits.txt`
- Matching/NonMatching flags: `configure.py`
- Per-function check: `python3 -I tools/cmpfn.py build/GM2E8P/main_loop/obj/main_loop/X.o build/GM2E8P/src/main_loop/X.o`
- Full check: `ninja && build/tools/dtk shasum -c config/GM2E8P/build.sha1`
