"""The poses of people in and on vehicles, solved on the cooked skeleton by inverse kinematics (the behaviour of Roadcraft
riderPoses.ts: trunk lean, two-bone limbs, feet levelled), for tools/assets/cook_people.py.

The cooked pack keeps no skeleton, only palettes of bind -> pose matrices, so the joints come out of them: a child stays
fixed on its parent, so its joint is the point both matrices carry to the same place, solved by least squares over every
frame and held near the middle of the skin the two bones share (a hinge leaves its axis free). The pelvis has no parent to
stay on: the pack's pedal puts it at PACK_PELVIS, so its joint is that less the frame's translation. The solver starts from
the bind pose. All the targets are metres from the pelvis, +X the body's left, +Y up, +Z forward.
"""
import numpy as np

STILL_FRAMES = 2  # a held pose: the shader mixes two rows
PEDAL_FRAMES = 30  # one turn of the cranks
PACK_PELVIS = (0.0, 0.93, -0.14)  # where roadcraft-novo tools/cook-people.mjs pedalFrame puts the pelvis
JOINT_HOLD = 1e-3  # how little the shared skin pulls a joint against what the frames say
SOLE = 0.08  # the ankle over the floor the seated targets stand on, where an average sole meets it
LOWEST_WEIGHT = 0.5  # a vertex of the foot is one at least this much on the foot and toe bones
FOOT_LEVEL = (0.0, -0.35, 1.0)  # the way a foot runs: forward and a little down
LEAN_STEP, LEAN_MORE = 0.03, 0.6  # how a rider's trunk goes further, and how far, until the hands are on the grips
REACH_SLACK = 0.01  # hands this close are on the grips
REST_STOOP = 0.2  # Roadcraft's leans count from a rest that stoops this far forward (riderPoses.ts); the bind pose stands straight

PELVIS = 1  # its index in the pack's bones
SPINE = ("spine_01", "spine_02", "spine_03")
LEFT_LEG, RIGHT_LEG = ("thigh_l", "calf_l", "foot_l"), ("thigh_r", "calf_r", "foot_r")
LEFT_ARM, RIGHT_ARM = ("upperarm_l", "lowerarm_l", "hand_l"), ("upperarm_r", "lowerarm_r", "hand_r")

# The machines the game draws (agents.json moto.rider, bicycle.rider and bicycle.bracket), in the model's metres.
MOTO_HIP, MOTO_HAND, MOTO_ANKLE = (0.0, 0.86, -0.24), (0.26, 0.94, 0.41), (0.17, 0.246, 0.12)
BIKE_HIP, BIKE_HAND = (0.0, 0.98, -0.22), (0.25, 1.0, 0.3)
BIKE_BRACKET_Y, BIKE_BRACKET_Z, PEDAL_SIDE_M = 0.3, -0.08, 0.12
CRANK_M = 0.17  # a city bicycle's crank, 170 mm
ANKLE_OVER_PEDAL_Y, ANKLE_OVER_PEDAL_Z = 0.07, -0.08  # the ankle over a pedal under the ball of the foot


def rim(clock, forward, up, tilt, radius):
    """A point on a steering wheel's rim."""
    along = np.cos(clock) * radius
    return (np.sin(clock) * radius, up + along * np.cos(tilt), forward - along * np.sin(tilt))


def car_wheel(clock):
    return rim(clock, 0.46, 0.36, 0.45, 0.185)


def cab_wheel(clock):
    return rim(clock, 0.42, 0.13, 1.0, 0.22)


def from_hip(hip, point, side=1):
    """A point of a machine, from the rider's hip; `side` -1 its mirror."""
    return (point[0] * side - hip[0], point[1] - hip[1], point[2] - hip[2])


def foot_down(hip_y, side, forward, tilt):
    """The ankle of a foot put down on the road beside a machine leaning `tilt` towards it."""
    return (side * np.cos(tilt) - SOLE * np.sin(tilt), side * np.sin(tilt) + SOLE * np.cos(tilt) - hip_y, forward)


def pedal_ankle(angle, side):
    """The ankle on a pedal `angle` round the bracket, from the hip."""
    y = BIKE_BRACKET_Y + np.sin(angle) * CRANK_M + ANKLE_OVER_PEDAL_Y
    z = BIKE_BRACKET_Z + np.cos(angle) * CRANK_M + ANKLE_OVER_PEDAL_Z
    return (side * PEDAL_SIDE_M - BIKE_HIP[0], y - BIKE_HIP[1], z - BIKE_HIP[2])


SEATED = {"seated": True, "knee": (0, 1, 1), "elbow": (0.3, -1, 0)}
MOTO = {"lean": 0.28, "left_hand": from_hip(MOTO_HIP, MOTO_HAND), "right_hand": from_hip(MOTO_HIP, MOTO_HAND, -1),
        "left_foot": from_hip(MOTO_HIP, MOTO_ANKLE), "right_foot": from_hip(MOTO_HIP, MOTO_ANKLE, -1),
        "knee": (0.5, 0.1, 1), "elbow": (0.6, -1, 0), "look": -0.12}
BIKE = {"lean": 0.32, "left_hand": from_hip(BIKE_HIP, BIKE_HAND), "right_hand": from_hip(BIKE_HIP, BIKE_HAND, -1),
        "knee": (0.1, 0.2, 1), "elbow": (0.4, -1, 0), "look": -0.2}

# Name -> what the pose asks; `seated` poses stand their soles on the floor their targets name.
POSES = {
    "chairSit": {**SEATED, "lean": -0.3, "left_foot": (0.12, -0.46, 0.4), "right_foot": (-0.12, -0.46, 0.4),
                 "left_hand": (0.12, 0.03, 0.27), "right_hand": (-0.12, 0.03, 0.27), "look": 0.05},
    "carDrive": {**SEATED, "lean": -0.45, "left_foot": (0.17, -0.08, 0.62), "right_foot": (-0.14, -0.08, 0.64),
                 "left_hand": car_wheel(1.05), "right_hand": car_wheel(-1.05), "elbow": (0, -1, 0.2), "look": -0.12},
    "carRide": {**SEATED, "lean": -0.45, "left_foot": (0.16, -0.08, 0.61), "right_foot": (-0.16, -0.08, 0.61),
                "left_hand": (0.13, 0.06, 0.3), "right_hand": (-0.13, 0.06, 0.3), "look": -0.1},
    "carRearRide": {**SEATED, "lean": -0.38, "left_foot": (0.15, -0.13, 0.43), "right_foot": (-0.15, -0.13, 0.44),
                    "left_hand": (0.14, 0.04, 0.23), "right_hand": (-0.14, 0.04, 0.23), "look": -0.1},
    "cabDrive": {**SEATED, "lean": -0.42, "left_foot": (0.16, -0.37, 0.4), "right_foot": (-0.15, -0.37, 0.43),
                 "left_hand": cab_wheel(1.15), "right_hand": cab_wheel(-1.15), "elbow": (0, -1, 0.1), "look": -0.05},
    "cabRide": {**SEATED, "lean": -0.42, "left_foot": (0.16, -0.37, 0.38), "right_foot": (-0.16, -0.37, 0.38),
                "left_hand": (0.13, 0.02, 0.27), "right_hand": (-0.13, 0.02, 0.27), "look": 0},
    "motoRide": MOTO,
    "motoStop": {**MOTO, "lean": 0.12, "left_foot": foot_down(MOTO_HIP[1], 0.42, 0.12, 0.14)},
    "bikeStop": {**BIKE, "lean": 0.18, "left_foot": foot_down(BIKE_HIP[1], 0.34, 0.1, 0.16), "right_foot": pedal_ankle(np.pi * 0.6, -1)},
}


def pedal_pose(frame):
    """The ask of frame `frame` of a turn of the cranks."""
    angle = frame / PEDAL_FRAMES * 2 * np.pi
    return {**BIKE, "left_foot": pedal_ankle(angle, 1), "right_foot": pedal_ankle(angle + np.pi, -1)}


def axis_angle(axis, angle):
    a = np.asarray(axis, dtype=np.float64)
    a = a / np.linalg.norm(a)
    k = np.array([[0, -a[2], a[1]], [a[2], 0, -a[0]], [-a[1], a[0], 0]])
    return np.eye(3) + np.sin(angle) * k + (1 - np.cos(angle)) * (k @ k)


def between(a, b):
    """The shortest rotation taking direction `a` to `b`."""
    a, b = a / np.linalg.norm(a), b / np.linalg.norm(b)
    v, c = np.cross(a, b), float(a @ b)
    s = np.linalg.norm(v)
    if s < 1e-9:
        return np.eye(3) if c > 0 else axis_angle(np.cross(a, [1, 0, 0]) if abs(a[0]) < 0.9 else np.cross(a, [0, 1, 0]), np.pi)
    return axis_angle(v, np.arctan2(s, c))


def joints_of(frames, pedal, parents, positions, bones, weights):
    """Each bone's joint in the bind pose. `frames`: F x bones x 3 x 4 of one body; `pedal`: a frame of the pack's pedal;
    `positions`, `bones`, `weights`: its skin."""
    count = len(parents)
    out = np.zeros((count, 3))
    out[PELVIS] = np.asarray(PACK_PELVIS) - pedal[PELVIS, :, 3]
    for child in range(PELVIS + 1, count):
        parent = parents[child]
        on_child, on_parent = (weights * (bones == child)).sum(axis=1), (weights * (bones == parent)).sum(axis=1)
        share = np.minimum(on_child, on_parent)
        if share.sum() < 1e-6:
            share = on_child + on_parent
        guess = (positions * share[:, None]).sum(axis=0) / max(share.sum(), 1e-6)
        rot = (frames[:, parent, :, :3] - frames[:, child, :, :3]).reshape(-1, 3)
        move = (frames[:, child, :, 3] - frames[:, parent, :, 3]).reshape(-1)
        hold = JOINT_HOLD * len(frames) ** 0.5
        out[child] = np.linalg.lstsq(np.vstack([rot, np.eye(3) * hold]), np.concatenate([move, guess * hold]), rcond=None)[0]
    return out


class Rig:
    """A body's bones as bind -> pose matrices, with what a solver turns: a bone and all it carries, about its joint."""

    def __init__(self, names, parents, joints, start):
        self.parents, self.joints = list(parents), joints
        self.index = {n: i for i, n in enumerate(names)}
        self.pose = start.astype(np.float64).copy()  # bones x 3 x 4
        self.kids = [[c for c, p in enumerate(parents) if p == b] for b in range(len(parents))]

    def at(self, name):
        i = self.index[name]
        return self.pose[i, :, :3] @ self.joints[i] + self.pose[i, :, 3]

    def carried(self, bone):
        out, todo = [], [bone]
        while todo:
            b = todo.pop()
            out.append(b)
            todo.extend(self.kids[b])
        return out

    def turn(self, name, rotation):
        pivot = self.at(name)
        for b in self.carried(self.index[name]):
            self.pose[b, :, :3] = rotation @ self.pose[b, :, :3]
            self.pose[b, :, 3] = rotation @ (self.pose[b, :, 3] - pivot) + pivot

    def aim(self, name, tip, target):
        at = self.at(name)
        self.turn(name, between(tip - at, target - at))

    def limb(self, chain, target, pole):
        root, mid, end = chain
        a, b, c = self.at(root), self.at(mid), self.at(end)
        upper, lower = np.linalg.norm(b - a), np.linalg.norm(c - b)
        to = target - a
        reach = min((upper + lower) * 0.999, max(abs(upper - lower) * 1.001 + 1e-4, np.linalg.norm(to)))
        d = to / np.linalg.norm(to)
        p = np.asarray(pole, dtype=np.float64)
        p = p - d * (p @ d)
        p = p / np.linalg.norm(p) if p @ p > 1e-8 else np.array([0.0, 0.0, 1.0])
        cos = np.clip((upper * upper + reach * reach - lower * lower) / (2 * upper * reach), -1, 1)
        self.aim(root, b, a + d * upper * cos + p * upper * np.sqrt(1 - cos * cos))
        self.aim(mid, self.at(end), a + d * reach)

    def level_foot(self, foot, ball):
        at, tip = self.at(foot), self.at(ball)
        way = np.asarray(FOOT_LEVEL) / np.linalg.norm(FOOT_LEVEL)
        self.aim(foot, tip, at + way * np.linalg.norm(tip - at))


def rest_of(count):
    """The bind pose."""
    rest = np.zeros((count, 3, 4))
    rest[:, :, :3] = np.eye(3)
    return rest


def apply(names, parents, joints, ask, lift=0.0):
    """The pose `ask` from the bind pose, as bones x 3 x 4; `lift` raises the feet."""
    rig = Rig(names, parents, joints, rest_of(len(names)))
    origin = joints[PELVIS]
    lean = ask["lean"] + REST_STOOP
    for name in SPINE:
        rig.turn(name, axis_angle((1, 0, 0), lean / len(SPINE)))
    nod = ask.get("look", 0) - lean * 0.6
    if nod:
        rig.turn("neck_01", axis_angle((1, 0, 0), nod * 0.6))
        rig.turn("head", axis_angle((1, 0, 0), nod * 0.4))
    knee, elbow = np.asarray(ask["knee"], dtype=float), np.asarray(ask["elbow"], dtype=float)
    up = np.array([0.0, lift, 0.0])
    rig.limb(LEFT_LEG, origin + np.asarray(ask["left_foot"]) + up, knee)
    rig.limb(RIGHT_LEG, origin + np.asarray(ask["right_foot"]) + up, knee * (-1, 1, 1))
    rig.limb(LEFT_ARM, origin + np.asarray(ask["left_hand"]), elbow)
    rig.limb(RIGHT_ARM, origin + np.asarray(ask["right_hand"]), elbow * (-1, 1, 1))
    rig.level_foot("foot_l", "ball_l")
    rig.level_foot("foot_r", "ball_r")
    return rig.pose


def skinned(pose, positions, bones, weights):
    """The skin in a pose, as people.gdshader skins it."""
    out = np.zeros_like(positions)
    for k in range(bones.shape[1]):
        m = pose[bones[:, k]]
        out += weights[:, k:k + 1] * (np.einsum("nij,nj->ni", m[:, :, :3], positions) + m[:, :, 3])
    return out


def hands_miss(names, parents, joints, pose, ask):
    """How far the worse hand stops from its target."""
    rig = Rig(names, parents, joints, pose)
    origin = joints[PELVIS]
    return max(np.linalg.norm(rig.at("hand_l") - origin - ask["left_hand"]), np.linalg.norm(rig.at("hand_r") - origin - ask["right_hand"]))


def reaching(names, parents, joints, ask):
    """The ask with the trunk leaning as far as this body needs to hold the bars; a body too small stops at LEAN_MORE."""
    lean = ask["lean"]
    while lean < ask["lean"] + LEAN_MORE and hands_miss(names, parents, joints, apply(names, parents, joints, {**ask, "lean": lean}), ask) > REACH_SLACK:
        lean += LEAN_STEP
    return {**ask, "lean": lean}


def solve(names, parents, joints, ask, positions, bones, weights):
    """A pose: a rider reaching the bars; a seated one with the lowest point of its feet on the floor its targets name,
    whatever the shoe."""
    if not ask.get("seated"):
        return apply(names, parents, joints, reaching(names, parents, joints, ask))
    pose = apply(names, parents, joints, ask)
    foot = [names.index(n) for n in ("foot_l", "ball_l", "foot_r", "ball_r")]
    on_foot = (weights * np.isin(bones, foot)).sum(axis=1) >= LOWEST_WEIGHT
    floor = min(ask["left_foot"][1], ask["right_foot"][1]) - SOLE
    lowest = skinned(pose, positions[on_foot], bones[on_foot], weights[on_foot])[:, 1].min() - joints[PELVIS][1]
    return apply(names, parents, joints, ask, floor - lowest)


def pedalling(names, parents, joints):
    """A turn of the cranks, frames x bones x 3 x 4: one lean for the turn, for the hands stay where they are."""
    lean = reaching(names, parents, joints, pedal_pose(0))["lean"]
    return np.stack([apply(names, parents, joints, {**pedal_pose(f), "lean": lean}) for f in range(PEDAL_FRAMES)])


def reach_of(pose, pelvis, positions, bones, weights):
    """How far the body runs from the pelvis in a pose: above, below, ahead, to the side (metres)."""
    p = skinned(pose, positions, bones, weights) - pelvis
    return (float(p[:, 1].max()), float(-p[:, 1].min()), float(p[:, 2].max()), float(np.abs(p[:, 0]).max()))
