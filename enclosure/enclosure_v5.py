from build123d import *
from cadpy.assembly import AssemblyHelper

# Reconstructed from the shipped enclosure_v4_body.step / enclosure_v4_lid.step
# (the original generator was lost from disk). Dimensions below were measured
# back out of those STEP files with scripts/inspect, not guessed.
#
# Two real fixes vs. the lost original:
#   1. Lid friction fit was loose on a test print. The geometry was NOT
#      tapered (confirmed: the inner wall is a constant 54.0mm prism, the
#      lid lip a constant 53.7mm prism -- a uniform, deliberate 0.15mm/side
#      clearance the full 1.5mm depth). 0.15mm/side just prints loose on
#      this printer. Fix: wall 3.0 -> 3.1mm, tightening to 0.05mm/side.
#   2. USB-C cutout was a sharp rectangle; now a stadium (rounded-end) slot
#      matching the real connector outline.
#
# ponytail: skipped reproducing the cosmetic outer rim taper (the original
# body's outer shell pinched from 60mm down to 54mm right at the very top
# and bottom edges, purely for looks). It doesn't touch the inner cavity
# that mates with the lid, so it has zero effect on fit. This version keeps
# the outer shell a plain 60x60 prism top to bottom.

OUTER = 60.0            # overall footprint, X/Y
OUTER_FILLET = 10.0     # rounded "pebble" corner radius, outer
BODY_H = 24.0           # body height, front wall to open top
WALL = 3.1              # SIDE wall thickness (sets the cavity / lid fit)
FRONT_WALL = 4.0        # FRONT wall (floor) thickness -- thicker than the sides so
                        # the screen pocket sinks into it AND the screws bite deep.
CAVITY = OUTER - 2 * WALL    # inner cavity (53.8), driven by the SIDE walls only
CAVITY_FILLET = 6.0     # inner cavity corner radius

# Screen panel pocket: the OLED panel stands proud of its blue PCB. With no posts,
# the PCB rests flat on the cavity floor and the panel drops into this pocket cut
# into the thick front wall. Pocket depth >= panel stand-height so the PCB always
# seats flat (if the panel is shorter, it just floats a hair above the ledge --
# harmless, the screws hold the PCB). Tune POCKET_DEPTH up if the PCB won't sit
# flat (panel taller than assumed); the window frame is FRONT_WALL - POCKET_DEPTH.
POCKET_DEPTH = 2.5
POCKET_W = 35.0         # > panel glass (~33mm) so it drops in with clearance
POCKET_H = 34.0

# Lid friction fit: a TAPERED lip (loft), not a constant prism. It enters loose at
# the bottom (LIP_LEADIN per side under the cavity) and presses tight at the top
# (LIP_INTERFERENCE per side OVER the cavity) when fully seated. The taper makes
# the fit forgiving of printer play -- some height along it always grips. The old
# constant 0.05mm/side prism printed loose on this printer.
# If still loose: raise LIP_INTERFERENCE. If it won't close/splits: lower it.
LIP_H = 3.0             # lip depth (was 1.5; deeper = more grip, less wobble)
LIP_LEADIN = 0.4        # per-side gap at the lip BOTTOM (easy start)
LIP_INTERFERENCE = 0.12 # per-side INTERFERENCE at the lip TOP (press at full seat)

# The Waveshare 1.5" screen is NOT centered on its PCB: the lit area sits this
# far toward the top edge of the board, away from the ribbon connector. To get
# the screen window centered on the FRONT FACE while still framing the screen,
# the window stays at Y=0 and the screw-boss pattern is shifted DOWN by this
# amount (the display then sits 2.2mm low in the case, screen lands dead center).
SCREEN_PCB_OFFSET = 2.2
BOSS_CY = -SCREEN_PCB_OFFSET   # screw-pattern center, Y (shifted so screen centers)


def _body():
    with BuildPart() as body_part:
        with BuildSketch():
            Rectangle(OUTER, OUTER)
            fillet(vertices(), radius=OUTER_FILLET)
        extrude(amount=BODY_H)

        # Hollow cavity: solid front wall (Z 0..WALL), open top above it.
        # NOTE: BuildSketch() always defaults to Plane.XY at world Z=0 -- an
        # enclosing `Locations((0,0,WALL))` does NOT shift it (Locations only
        # positions primitives built directly inside it, like Box/Cylinder).
        # Pass the offset plane explicitly instead.
        with BuildSketch(Plane.XY.offset(FRONT_WALL)):
            Rectangle(CAVITY, CAVITY)
            fillet(vertices(), radius=CAVITY_FILLET)
        extrude(amount=BODY_H - FRONT_WALL, mode=Mode.SUBTRACT)

        # Screen window: centered on the front face (0,0), sized so ONLY the
        # screen shows, no blue PCB. Goes fully through the front wall.
        with BuildPart() as screen_cutout:
            with BuildSketch():
                Rectangle(30.0, 30.0)
                fillet(vertices(), radius=2.0)
            extrude(amount=FRONT_WALL + 2.0)
        add(screen_cutout.part, mode=Mode.SUBTRACT)

        # Screen panel pocket: deep pocket in the inner face of the front wall.
        # The panel drops in here; the PCB rests flat on the cavity floor around
        # it. Leaves a window-frame ledge (FRONT_WALL - POCKET_DEPTH thick) that
        # hides the panel border, since POCKET_W/H > window > panel-border.
        with Locations((0, 0, FRONT_WALL)):
            Box(POCKET_W, POCKET_H, POCKET_DEPTH, mode=Mode.SUBTRACT,
                align=(Align.CENTER, Align.CENTER, Align.MAX))

        # M2 SCREW HOLES -- no posts. The PCB lies flat on the floor and the
        # screws thread straight down into the solid 4mm front wall. 4-corner
        # pattern from the Waveshare datasheet (pitch 39.50 x 32.45mm), shifted
        # by BOSS_CY so the screen lands centered behind the centered window.
        # Plain Ø1.7 pilot: M2 machine screws self-tap into the plastic.
        bore_d = 1.7
        cap = 0.5                         # solid front-face cap left below the pilot
        for x in [-19.75, 19.75]:
            for y in [BOSS_CY - 16.225, BOSS_CY + 16.225]:
                with Locations((x, y, FRONT_WALL)):
                    Cylinder(radius=bore_d / 2, height=FRONT_WALL - cap,
                             mode=Mode.SUBTRACT, align=(Align.CENTER, Align.CENTER, Align.MAX))

        # XIAO CRADLE SHELF: Z = 12.0 to 13.0mm
        with Locations((16.0, 0.0, 12.0)):
            Box(22.0, 20.0, 1.0, align=(Align.CENTER, Align.CENTER, Align.MIN))

        # 0.5mm relief pocket in the shelf bed to clear bottom solder pads,
        # extending out toward the wall so it merges with the USB-C slot.
        with Locations((18.75, 0.0, 12.5)):
            Box(22.5, 14.0, 0.5, align=(Align.CENTER, Align.CENTER, Align.MIN))

        # Small retention lip that catches the XIAO board's edge above the shelf.
        with Locations((6.0, 0.0, 13.0)):
            Box(2.0, 20.0, 1.25, align=(Align.CENTER, Align.CENTER, Align.MIN))

        # USB-C cutout: a stadium (rounded-end) slot through the wall, not a
        # plain rectangle. Envelope 10.0mm(Y) x 4.5mm(Z); short ends fully
        # rounded (radius = height/2 = 2.25mm) to match the connector outline.
        # Confined to X [CAVITY/2, OUTER/2 + 0.5] so it only cuts the wall --
        # it must not reach back far enough to nick the retention lip or shelf.
        with BuildPart() as usb_cutout:
            with BuildSketch(Plane.YZ.offset(CAVITY / 2)):
                with Locations((0, 15.25)):
                    Rectangle(10.0, 4.5)
                    fillet(vertices(), radius=2.25)
            extrude(amount=OUTER / 2 - CAVITY / 2 + 0.5)
        add(usb_cutout.part, mode=Mode.SUBTRACT)

    return body_part.part


def _lid():
    # Tapered lip: bigger (press-fit) at the top where it meets the plate,
    # smaller (lead-in) at the bottom tip. Lofted between the two profiles.
    lip_top = CAVITY + 2 * LIP_INTERFERENCE
    lip_bot = CAVITY - 2 * LIP_LEADIN
    fil_top = CAVITY_FILLET + LIP_INTERFERENCE
    fil_bot = CAVITY_FILLET - LIP_LEADIN
    with BuildPart() as lid_part:
        with BuildSketch():
            Rectangle(OUTER, OUTER)
            fillet(vertices(), radius=OUTER_FILLET)
        extrude(amount=2.0)

        with BuildSketch(Plane.XY):                  # lip top (at plate underside)
            Rectangle(lip_top, lip_top)
            fillet(vertices(), radius=fil_top)
        with BuildSketch(Plane.XY.offset(-LIP_H)):   # lip bottom (tip)
            Rectangle(lip_bot, lip_bot)
            fillet(vertices(), radius=fil_bot)
        loft()

    return lid_part.part


def gen_step():
    """Primary STEP target for `scripts/step` / `scripts/snapshot`: body + lid
    as a labeled assembly, lid placed on top of the body for review."""
    asm = AssemblyHelper("weathergotchi_enclosure_v5")
    asm.add(_body(), "body")
    lid_occ = asm.add(_lid(), "lid")
    lid_occ.move(Location((0, 0, BODY_H)))
    return asm.build()


def generate_files():
    """Project export routine: writes the individual print-ready files this
    repo's slicer workflow expects (body/lid STEP+STL, combined plate STL)."""
    body = _body()
    lid = _lid()

    export_step(body, "enclosure_v5_body.step")
    export_stl(body, "enclosure_v5_body.stl")
    export_step(lid, "enclosure_v5_lid.step")
    export_stl(lid, "enclosure_v5_lid.stl")

    # Combined print-plate layout: body and lid side by side, each already
    # in its correct print orientation (body cavity-up, lid lip-up).
    lid_flipped = lid.rotate(Axis.X, 180)
    lid_bb = lid_flipped.bounding_box()
    lid_plate = lid_flipped.translate((OUTER + 5.0, 0, -lid_bb.min.Z))
    combined = Compound(label="combined", children=[body, lid_plate])
    export_stl(combined, "enclosure_v5_combined.stl")


if __name__ == "__main__":
    generate_files()
