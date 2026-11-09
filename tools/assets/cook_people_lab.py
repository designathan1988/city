# Cooks the people's clips from the lab's own motion (the lab's render/citizenBake.ts, citizenWalk.ts, citizenGait.ts, citizenStride.ts,
# riderPoses.ts) onto the sixteen bodies of the cast, and writes project/assets/people/people.bin with them. The motion comes from the lab:
# its cooked bodies (public/models/citizens/people/<id>.bin, in the order of src/people/cast.json, which is people.bin's), its Rocketbox
# library and neutral walks (src/render/motion). What is not motion is kept from the people.bin given as the base (the bodies' meshes, card,
# face table, palms, helmets and ragdoll joints): the cook reads its own output back to the same bytes, so a run on the file it wrote changes
# nothing. Deterministic: the same inputs give the same file. Layout (people_asset.h reads it): version 9, the palette's bodies side by side
# in `columns` columns, each clip with its kind (standing, forward with a travel curve, turn with a yaw curve, cycle) and, per body, its stride
# and its curve over its frames.
# Usage: python tools/assets/cook_people_lab.py --lab <lab root> --base <people.bin> --out <people.bin> [--jobs N]
import argparse
import json
import sys
from multiprocessing import Pool
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lab_riders as R  # noqa: E402
import lab_rig as L  # noqa: E402
import lab_shaping as S  # noqa: E402
from people_bin import PALETTE_SPAN, read_base, write  # noqa: E402

FLAG_FEMALE = 1
KIND_STAND, KIND_FORWARD, KIND_TURN, KIND_CYCLE = 0, 1, 2, 3  # people_asset.h ClipKind

# citizenGait.ts / citizenBake.ts: the amplitudes, the arms' room by clip, the frame rates, the neutral walks' ground per cycle
ELDER = {'arms': 0.5, 'legs': 0.78, 'hips': 0.6}
SHUFFLE = {'arms': 0.4, 'legs': 0.5, 'hips': 0.55}
REST = {'arms': 0.0, 'legs': 0.0, 'hips': 0.0}
LIMB_ROOM = {'idle': (8, 0), 'look': (8, 0), 'listen': (8, 0), 'talk': (6, 0), 'phone': (5, 0), 'start': (6, 0), 'stop': (6, 0), 'run': (6, 0),
             'walkSlow': (6, 0), 'turnLeft': (7, 0), 'turnRight': (7, 0)}
LIMB_ROOM_DEFAULT = (6, 0)
FPS, SLOW_LOOP_S = 30, 6  # bakeFps: what walks and turns at the capture's 30 fps; a loop longer than 6 s is slow motion
# The lab bakes a long loop at 10 fps (the library's rate for those takes); here at 4, and a standing one-shot (a look, a photo, a crouch) at
# the library's 10 rather than 30: at the lab's rates the sixteen bodies' palette was 107 MB, over the 100 MB a file the repository's host
# takes; the shader blends between rows, and these are the slow takes.
LONG_LOOP_FPS, STANDING_ONCE_FPS = 4, 10
WALK_ADVANCE = {'m': 1.495864, 'f': 1.596625}  # citizenWalk.ts WALK_ADVANCE: metres a neutral cycle covers on its own avatar
SIDE_ANGLES = {'walkBack': np.pi, 'walkLeft': np.pi / 2, 'walkRight': -np.pi / 2}

# The clips the game plays, in the file's order, each (name, recipe): ('lib', clip, amplitude) a library clip, ('walk', amplitude) the
# neutral walk, ('rest',) the slow walk's mean stance, ('side', angle) a step aside or back, ('rider', index) a seated or riding pose.
CLIPS = [
    ('walk', ('walk', L.FULL)), ('idle', ('lib', 'idle', L.FULL)),
    ('carDrive', ('rider',)), ('carRide', ('rider',)), ('carRearRide', ('rider',)), ('cabDrive', ('rider',)), ('cabRide', ('rider',)),
    ('chairSit', ('rider',)), ('motoRide', ('rider',)), ('motoStop', ('rider',)), ('bikePedal', ('rider',)), ('bikeStop', ('rider',)),
    ('talk', ('lib', 'talk', L.FULL)), ('listen', ('lib', 'listen', L.FULL)), ('look', ('lib', 'look', L.FULL)), ('phone', ('lib', 'phone', L.FULL)),
    ('headphones', ('lib', 'headphones', L.FULL)), ('bag', ('lib', 'bag', L.FULL)), ('crouchDown', ('lib', 'crouchDown', L.FULL)),
    ('crouchIdle', ('lib', 'crouchIdle', L.FULL)), ('crouchUp', ('lib', 'crouchUp', L.FULL)), ('photo', ('lib', 'photo', L.FULL)),
    ('walkElder', ('walk', ELDER)), ('walkShuffle', ('lib', 'walkSlow', SHUFFLE)), ('walkSlow', ('lib', 'walkSlow', L.FULL)),
    ('walkN1', ('lib', 'walkN1', L.FULL)), ('walkN2', ('lib', 'walkN2', L.FULL)), ('walkN3', ('lib', 'walkN3', L.FULL)),
    ('walkStroll', ('lib', 'walkStroll', L.FULL)), ('walkCool', ('lib', 'walkCool', L.FULL)), ('walkFast', ('lib', 'walkFast', L.FULL)),
    ('run', ('lib', 'run', L.FULL)), ('start', ('lib', 'start', L.FULL)), ('stop', ('lib', 'stop', L.FULL)),
    ('turnLeft', ('lib', 'turnLeft', L.FULL)), ('turnRight', ('lib', 'turnRight', L.FULL)),
    ('walkRest', ('rest',)), ('walkBack', ('side',)), ('walkLeft', ('side',)), ('walkRight', ('side',)),
    ('laugh', ('lib', 'laugh', L.FULL)), ('angry', ('lib', 'angry', L.FULL)), ('argue', ('lib', 'argue', L.FULL)),
    ('carDriveMirror', ('rider',)), ('carDriveRight', ('rider',)), ('carRideLeft', ('rider',)), ('carRideRight', ('rider',)),
    ('carRearLeft', ('rider',)), ('carRearRight', ('rider',)), ('cabDriveMirror', ('rider',)), ('cabDriveRight', ('rider',)),
    ('cabRideLeft', ('rider',)), ('cabRideRight', ('rider',)), ('chairSitLeft', ('rider',)), ('chairSitRight', ('rider',)),
    ('chairSitPhone', ('rider',)),
]
RIDERS = {name: (pose, seconds, frames) for name, pose, seconds, frames in R.RIDER_CLIPS}


def frame_count(seconds, loop, kind):
    fps = LONG_LOOP_FPS if loop and seconds > SLOW_LOOP_S else STANDING_ONCE_FPS if not loop and kind == KIND_STAND else FPS
    return max(1, int(round(seconds * fps)))


def curve_at(curve, loop, fraction):
    """citizenGait.ts curveAt: a per-source-frame curve of a library clip at a fraction of it."""
    count = len(curve)
    x = fraction * count if loop else fraction * (count - 1)
    k0 = min(count - 1, max(0, int(np.floor(x))))
    k1 = (k0 + 1) % count if loop else min(count - 1, k0 + 1)
    f = x - np.floor(x)
    return curve[k0] * (1 - f) + curve[k1] * f


def plan(lab):
    """Each clip's frames, loop, kind and seconds by sex (shared by the bodies: the shaders' clip tables)."""
    walks = {sex: L.walk_source(f'{lab}/src/render/motion/walk{"Female" if sex == "f" else "Male"}.json') for sex in 'mf'}
    libs = {sex: L.library(f'{lab}/src/render/motion/rocketbox{"Female" if sex == "f" else "Male"}.json', walks[sex][0]) for sex in 'mf'}
    out = []
    for name, recipe in CLIPS:
        kind = recipe[0]
        if kind == 'walk':
            seconds, loop, clip_kind = [walks[s][1].duration for s in 'mf'], True, KIND_CYCLE
        elif kind == 'lib':
            c = {s: libs[s][recipe[1]] for s in 'mf'}
            seconds, loop = [c[s].duration for s in 'mf'], c['m'].loop
            clip_kind = {'forward': KIND_FORWARD, 'turn': KIND_TURN, 'cycle': KIND_CYCLE}.get(c['m'].kind, KIND_STAND)
        elif kind == 'rest':
            seconds, loop, clip_kind = [1.0, 1.0], True, KIND_CYCLE
        elif kind == 'side':
            seconds, loop, clip_kind = [libs[s]['walkSlow'].duration for s in 'mf'], True, KIND_CYCLE
        else:
            _, secs, frames = RIDERS[name]
            out.append(dict(name=name, recipe=recipe, seconds=[secs, secs], loop=True, kind=KIND_STAND, frames=frames))
            continue
        frames = 1 if kind == 'rest' else frame_count(max(seconds), loop, clip_kind)
        out.append(dict(name=name, recipe=recipe, seconds=seconds, loop=loop, kind=clip_kind, frames=frames))
    return out


def bake_body(job):
    """One body's clips: its palette rows, its fits and strides, its curves."""
    lab, body_id, sex, clips = job
    body = L.Body(f'{lab}/public/models/citizens/people/{body_id}.bin')
    walk, wsrc = L.walk_source(f'{lab}/src/render/motion/walk{"Female" if sex == "f" else "Male"}.json')
    lib = L.library(f'{lab}/src/render/motion/rocketbox{"Female" if sex == "f" else "Male"}.json', walk)
    pelvis = body.node('Bip01_Pelvis')
    rows, fits, curves = [], [], []
    shuffle_stride = None
    for clip in clips:
        name, recipe, frames, loop = clip['name'], clip['recipe'], clip['frames'], clip['loop']
        seconds = clip['seconds'][1 if sex == 'f' else 0]
        body.reset()
        stride, curve = 0.0, np.zeros(frames + 1)
        if recipe[0] == 'walk':
            t = L.Transfer(body, wsrc, recipe[1])
            pose = t.pose
            share = 1.0 if recipe[1] is L.FULL else L.stride_share(wsrc, recipe[1])
            stride = WALK_ADVANCE[sex] * t.scale * share
        elif recipe[0] == 'lib':
            c = lib[recipe[1]]
            t = L.Transfer(body, c.source, recipe[2])
            room = LIMB_ROOM.get(name, LIMB_ROOM_DEFAULT)

            def pose(time, t=t, room=room):
                t.pose(time)
                S.clear_limbs(body, *room)
            share = 1.0 if recipe[2] is L.FULL else L.stride_share(c.source, recipe[2])
            stride = c.advance * t.scale * share
            if name == 'walkShuffle':
                shuffle_stride = stride
            for i in range(frames + 1):
                fraction = min(1.0, i / frames) if loop else min(1.0, i / max(1, frames - 1))
                if clip['kind'] == KIND_FORWARD:
                    curve[i] = curve_at(c.travel, c.loop, fraction % 1 if c.loop else fraction) * t.scale
                elif clip['kind'] == KIND_TURN:
                    curve[i] = abs(curve_at(c.yaw, c.loop, fraction))
        elif recipe[0] == 'rest':
            t = L.Transfer(body, lib['walkSlow'].source, REST)
            pose = lambda time, t=t: t.pose(0.0)  # noqa: E731
        elif recipe[0] == 'side':
            d = S.Directional(body, lib['walkSlow'], SHUFFLE, SIDE_ANGLES[name])
            pose = d.pose
            stride = shuffle_stride * d.stride_scale
        else:
            rider = RIDERS[name][0]

            def pose(time, rider=rider):
                body.reset()
                rider(body, time)
        for i in range(frames):
            time = (i * seconds / frames) if loop else (i * seconds / max(1, frames - 1))
            pose(time)
            body.update()
            if i == 0:
                at = body.position(pelvis)
                v = body.skinned_vertices()
                fits.append([at[0], at[1], at[2], v[:, 1].max() - at[1], at[1] - v[:, 1].min(), v[:, 2].max() - at[2],
                             np.abs(v[:, 0] - at[0]).max(), stride])
            rows.append(body.skin()[:, :3, :])
        curves.append(curve)
    return np.array(rows), np.array(fits, dtype=np.float32), curves


def main(argv):
    ap = argparse.ArgumentParser()
    ap.add_argument('--lab', required=True)
    ap.add_argument('--base', required=True)
    ap.add_argument('--out', required=True)
    ap.add_argument('--jobs', type=int, default=4)
    a = ap.parse_args(argv)
    base = read_base(a.base)
    cast = json.load(open(f'{a.lab}/src/people/cast.json'))
    if len(cast) != base['count']:
        raise ValueError(f'the lab casts {len(cast)} bodies, people.bin holds {base["count"]}')
    for row, b in zip(cast, base['bodies']):
        if (row['gender'] == 'f') != bool(b['flags'] & FLAG_FEMALE) or abs(row['mood'] - b['mood']) > 1e-4:
            raise ValueError(f'people.bin body is not the lab\'s {row["id"]} (sex or mood differ)')
    clips = plan(a.lab)
    jobs = [(a.lab, row['id'], row['gender'], clips) for row in cast]
    with Pool(max(1, a.jobs)) as pool:
        baked = pool.map(bake_body, jobs)
    if any(np.abs(rows).max() >= PALETTE_SPAN for rows, _, _ in baked):
        raise ValueError('a palette value outside the fixed point span')
    block_rows = write(a.out, base, clips, baked)
    print(f'people.bin: {len(clips)} clips, {block_rows} rows a body, {base["count"]} bodies')
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
