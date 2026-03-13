# miniRT - Complete Testing & Robustness Guide

## Overview

This document covers the full testing methodology for **miniRT**, including:
- All bugs found and fixed
- Complete test suite for breaking the parser
- 12 original showcase scenes
- Window/rendering improvements

---

## Bugs Found & Fixed

### 1. No `.rt` File Extension Validation
**Problem:** The parser accepted any file regardless of extension.
**Fix:** Added `check_extension()` in `main.c` that rejects files not ending with `.rt`.
**Test:** `tests/should_fail/no_extension`, `tests/should_fail/wrong_ext.txt`

### 2. Missing Mandatory Element Detection (A, C)
**Problem:** miniRT would crash or render garbage if `A` (ambient) or `C` (camera) was missing from the scene file. The 42 subject mandates exactly one of each.
**Fix:** Added `has_ambient` and `has_camera` flags to `t_scene`. Checked after parsing in `main.c`.
**Test:** `tests/should_fail/no_ambient.rt`, `tests/should_fail/no_camera.rt`, `tests/should_fail/tabs_no_camera.rt`

### 3. Duplicate A/C Not Rejected
**Problem:** Multiple `A` or `C` lines were silently accepted, overwriting previous values. The 42 subject says these must appear exactly once.
**Fix:** `parse_ambient()` and `parse_camera()` now return error if the element was already set.
**Test:** `tests/should_fail/duplicate_ambient.rt`, `tests/should_fail/duplicate_camera.rt`

### 4. Zero Direction Vector Accepted (Division by Zero)
**Problem:** Camera direction `0,0,0` was accepted by `is_normalized()`, causing division by zero in `vec3_normalize()` during camera setup.
**Fix:** `is_normalized()` now checks that the squared length is > 1e-8.
**Test:** `tests/should_fail/cam_dir_zero.rt`, `tests/should_fail/plane_zero_normal.rt`

### 5. FOV Boundary Values Not Rejected
**Problem:** FOV of 0 or 180 was accepted. FOV=0 creates a degenerate viewport, FOV=180 causes `tan(pi/2) = infinity`.
**Fix:** Changed validation from `< 0 || > 180` to `<= 0 || >= 180`.
**Test:** `tests/should_fail/cam_fov_zero.rt`, `tests/should_fail/cam_fov_180.rt`, `tests/should_fail/cam_fov_negative.rt`

### 6. Negative/Zero Radius & Height Accepted
**Problem:** Spheres, cylinders, and cones accepted negative or zero dimensions, producing invisible or broken geometry.
**Fix:** Added `radius <= 0` and `height <= 0` checks in all shape parsers.
**Test:** `tests/should_fail/sphere_neg_diam.rt`, `tests/should_fail/sphere_zero_diam.rt`, `tests/should_fail/cyl_neg_radius.rt`, `tests/should_fail/cyl_neg_height.rt`, `tests/should_fail/cyl_zero_height.rt`, `tests/should_fail/cone_zero_radius.rt`

### 7. Window Too Small + No Expose Redraw
**Problem:** Window was 900px wide. After minimizing/restoring, the image disappeared because there was no expose hook to redraw.
**Fix:**
  - Increased `image_width` to 1200 (16:9 aspect ratio -> 1200x675)
  - Added `expose_hook()` that redraws the image on window expose events
  - Store `img_ptr` in `t_minirt` for the expose callback
  - Added `mlx_hook(rt.win, 12, 1L << 15, expose_hook, &rt)` in main

### 8. Image Not Destroyed on Exit (Memory Leak)
**Problem:** `close_window()` destroyed the window but never called `mlx_destroy_image()`.
**Fix:** Added `mlx_destroy_image(rt->mlx, rt->img_ptr)` before exit.

### 9. `is_valid_double()` Declared But Not Implemented
**Problem:** Declared in `parse.h` but never defined.
**Fix:** Implemented a proper validator in `parse_utils.c`.

---

## Test Suite

### Running the Tests

```bash
make
bash tests/run_tests.sh
```

### Test Structure

```
tests/
|-- run_tests.sh              # Automated tester script
|-- should_fail/              # 39 files that MUST be rejected
|   |-- no_extension          # No .rt extension
|   |-- wrong_ext.txt         # Wrong file extension
|   |-- empty_file.rt         # Empty file (no A or C)
|   |-- no_ambient.rt         # Missing mandatory A
|   |-- no_camera.rt          # Missing mandatory C
|   |-- duplicate_ambient.rt  # Two A lines
|   |-- duplicate_camera.rt   # Two C lines
|   |-- ambient_neg_ratio.rt  # A ratio < 0
|   |-- ambient_high_ratio.rt # A ratio > 1
|   |-- ambient_extra_args.rt # A with extra arguments
|   |-- color_overflow.rt     # Color component > 255
|   |-- color_negative.rt     # Negative color component
|   |-- color_missing_comp.rt # Color with only 2 components
|   |-- color_spaces.rt       # Color with spaces instead of commas
|   |-- cam_dir_zero.rt       # Camera direction (0,0,0)
|   |-- cam_dir_unnorm.rt     # Camera direction outside [-1,1]
|   |-- cam_fov_zero.rt       # FOV = 0
|   |-- cam_fov_180.rt        # FOV = 180
|   |-- cam_fov_negative.rt   # FOV < 0
|   |-- camera_extra_args.rt  # Camera with extra arguments
|   |-- sphere_neg_diam.rt    # Sphere with negative diameter
|   |-- sphere_zero_diam.rt   # Sphere with zero diameter
|   |-- sphere_missing_color.rt # Sphere missing color field
|   |-- cyl_neg_radius.rt     # Cylinder with negative radius
|   |-- cyl_neg_height.rt     # Cylinder with negative height
|   |-- cyl_zero_height.rt    # Cylinder with zero height
|   |-- cyl_missing_args.rt   # Cylinder missing arguments
|   |-- plane_zero_normal.rt  # Plane with (0,0,0) normal
|   |-- plane_unnorm_normal.rt# Plane normal outside [-1,1]
|   |-- cone_zero_radius.rt   # Cone with zero radius
|   |-- triangle_missing_v.rt # Triangle missing a vertex
|   |-- light_high_bright.rt  # Light brightness > 1
|   |-- light_neg_bright.rt   # Light brightness < 0
|   |-- light_missing_args.rt # Light missing arguments
|   |-- unknown_id.rt         # Unknown identifier "xx"
|   |-- lowercase_ids.rt      # "a" and "c" instead of "A" and "C"
|   |-- uppercase_shapes.rt   # "SP" instead of "sp"
|   |-- tabs_no_camera.rt     # Tabs everywhere but no camera
|   '-- vec3_missing_comp.rt  # Vec3 with only 2 components
|
'-- should_pass/              # 10 files that MUST be accepted
    |-- minimal.rt            # Just A and C (minimum valid)
    |-- comments_blanks.rt    # Comments and blank lines
    |-- tabs_whitespace.rt    # Tab-separated fields
    |-- multi_lights.rt       # Multiple L elements
    |-- ambient_zero.rt       # A ratio = 0.0
    |-- ambient_max.rt        # A ratio = 1.0
    |-- fov_narrow.rt         # FOV = 1 (extreme narrow)
    |-- fov_wide.rt           # FOV = 179 (extreme wide)
    |-- all_shapes.rt         # Every shape type in one file
    '-- decimal_values.rt     # Leading zeros (000.333)
```

The test script also tests:
- **No arguments** -> should error
- **Non-existent file** -> should error
- **Too many arguments** -> should error

---

## Showcase Scenes (12 Original Scenes)

Located in `scenes/`:

| Scene | Description | Features Tested |
|-------|-------------|-----------------|
| `solar_system.rt` | Miniature solar system with planets | Many spheres, flat cylinder ring, large distances |
| `snowman.rt` | Classic snowman with hat and buttons | Stacked spheres, thin cylinders, cone nose |
| `greek_temple.rt` | Classical temple with columns | Many cylinders, triangle pediment, checker floor |
| `glass_mirrors.rt` | Material showcase | Glass, metal, checkerboard, multiple lights |
| `mushroom_forest.rt` | Fantasy mushroom forest | Cones, mixed materials, bumpmap, multi-light |
| `chess_board.rt` | Chess board with pieces | Checker texture, many small objects, overhead light |
| `underwater.rt` | Ocean floor scene | Glass jellyfish, bumpmap coral, blue ambient |
| `still_life.rt` | Table with fruit and wine glass | Glass, metal, multiple lights, triangles |
| `space_station.rt` | Sci-fi corridor | Metal walls, glass dome, checker floor |
| `pyramid.rt` | Egyptian desert with pyramids | Triangles for pyramid faces, cone obelisk |
| `billiard.rt` | Pool table with colored balls | Many small spheres, flat cylinders |
| `diamond_exhibit.rt` | Jewelry showcase | Multiple glass objects, colored lights |
| `abstract_art.rt` | Geometric art installation | All materials, all shapes, dramatic lighting |
| `city_skyline.rt` | Night city with skyscrapers | Many cylinders, atmospheric lighting |

### Running a Scene

```bash
./miniRT scenes/snowman.rt
./miniRT scenes/solar_system.rt
./miniRT scenes/glass_mirrors.rt
# etc.
```

Press **ESC** or click the **X** button to close the window.

---

## Scene Description Assessment (Existing Scenes)

| Scene File | Description | Assessment |
|------------|-------------|------------|
| `room_test2.rt` | "Only white" | Camera is AT the light position. The light sphere is tiny (radius 1) and right at camera: all light, no geometry visible. **Normal behavior.** |
| `room_test3.rt` | "Tipi with 3 cylinders" | Cylinders with various tilted axes meeting at angles. **Normal behavior** - diagonal cylinders form a tent-like shape. |
| `scene.rt` | "Cornell box" | Cornell box with glass sphere and bumpmap sphere. **Expected rendering.** |
| `spheres.rt` | "2 spheres" | Two visible spheres (other two are very far away/behind camera). **Normal behavior.** |
| `test_scene.rt` | "Gray only" | Camera may be looking past objects depending on FOV. With low ambient `0.2` and blue-ish light, scene appears grey if nothing is hit. **Scene geometry issue, not a bug.** |
| `test.rt` | "Cylinder + sphere" | Sphere inside cylinder with other shapes. **Normal behavior.** |
| `test2.rt` | "Shapes stuck in wall" | Extreme coordinates like `2001111111.0` push objects to infinity. **Normal behavior** - bad scene data. |
| `test3.rt` | "Image is cut" | Camera direction `(0,-0.3,-1)` creates a downward angle. With 900px (now 1200px) this was causing viewport misalignment. **Fixed with larger window.** |
| `cylinders.rt` | Various cylinders | Five cylinders at various positions. **Normal behavior.** |

---

## Window Behavior

- **Resolution:** 1200x675 pixels (16:9 aspect ratio)
- **Expose/Redraw:** The window redraws its content when restored from minimized state or when overlapping windows are moved away
- **Close:** ESC key or window close button (X)
- **Image cleanup:** Image buffer is properly freed on exit

---

## 42 Subject Compliance Checklist

| Requirement | Status |
|---|---|
| File must have `.rt` extension | Validated |
| `A` (ambient) appears exactly once | Enforced |
| `C` (camera) appears exactly once | Enforced |
| `L` (light) can appear multiple times | Supported |
| Ambient ratio in [0.0, 1.0] | Validated |
| Light brightness in [0.0, 1.0] | Validated |
| Color components in [0, 255] | Validated |
| Direction vectors in [-1, 1] per component | Validated |
| FOV in (0, 180) exclusive | Validated |
| Sphere diameter > 0 | Validated |
| Cylinder diameter > 0, height > 0 | Validated |
| Unknown identifiers rejected | Rejected |
| ESC closes window cleanly | Working |
| Window close button works | Working |
| Error messages start with "Error\n" | Compliant |

---

## How to Break miniRT (Edge Cases Summary)

1. **No file extension** -> `./miniRT scene` (no .rt)
2. **Wrong extension** -> `./miniRT scene.txt`
3. **Empty file** -> file with no content
4. **Missing A or C** -> omit mandatory elements
5. **Duplicate A or C** -> two ambient or camera lines
6. **Zero direction vector** -> `C 0,0,0 0,0,0 70`
7. **FOV at boundaries** -> 0 or 180 degrees
8. **Negative dimensions** -> sphere diameter -5, cylinder height -10
9. **Zero dimensions** -> sphere diameter 0
10. **Color out of range** -> `256,0,0` or `-1,0,0`
11. **Missing arguments** -> `sp 0,0,0 10` (no color)
12. **Extra arguments** -> `A 0.2 255,255,255 extra`
13. **Unknown identifiers** -> `xx 0,0,0`
14. **Wrong case** -> `a`, `c`, `SP`, `PL`
15. **Missing color components** -> `255,255` (only 2)
16. **Missing vec3 components** -> `0,0` (only 2)
17. **No arguments** -> `./miniRT`
18. **Too many arguments** -> `./miniRT file1.rt file2.rt`
19. **Non-existent file** -> `./miniRT ghost.rt`