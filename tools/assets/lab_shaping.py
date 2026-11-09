# What the lab's bakes pose over a transfer (lab_rig.Transfer): the arms carried clear of the body (render/citizenBake.ts clearLimbs) and a
# walk turned into steps aside or back by moving the feet's stride about their mean, the legs reaching for them (render/citizenStride.ts
# directionalWalkFor).
import re

import numpy as np

from lab_math import unit
from lab_rig import Transfer

SIDE_SQ_LEAST = 1e-8  # citizenBake.ts clearLimbs: a direction this short (squared) has none


# ------------------------------------------------------------------ the arms' room (citizenBake.ts clearLimbs)

def clear_limbs(body, abduct, lift):
    if abduct <= 0 and lift <= 0:
        return
    chest = body.node('Bip01_Spine2')
    if chest < 0:
        return
    for side in 'LR':
        upper = body.node(f'Bip01_{side}_UpperArm')
        fore = body.node(f'Bip01_{side}_Forearm')
        hand = body.node(f'Bip01_{side}_Hand')
        if upper < 0 or fore < 0:
            continue
        if abduct > 0:
            out = body.position(upper) - body.position(chest)
            out[1] = 0
            if np.dot(out, out) > SIDE_SQ_LEAST:
                axis = unit(np.cross(np.array([0.0, -1.0, 0.0]), unit(out)))
                body.rotate_world(upper, axis, np.radians(abduct))
        if lift > 0 and hand >= 0:
            along = body.position(hand) - body.position(fore)
            if np.dot(along, along) > SIDE_SQ_LEAST:
                axis = np.cross(unit(along), np.array([0.0, 1.0, 0.0]))
                if np.dot(axis, axis) > SIDE_SQ_LEAST:
                    body.rotate_world(fore, unit(axis), -np.radians(lift))


# ------------------------------------------------------------------ steps aside and back (citizenStride.ts directionalWalkFor)

def foot_sides(body):
    """The drawn feet's vertices: each weighted more than half to a foot or its toes, and its side (0 left, 1 right)."""
    sides = np.array([0 if re.match(r'Bip01_L_(Foot|Toe)', body.names[b]) else 1 if re.match(r'Bip01_R_(Foot|Toe)', body.names[b]) else -1
                      for b in body.bones])
    share = np.zeros((len(body.vertices), 2))
    for c in range(4):
        s = sides[body.skin_index[:, c]]
        for side in (0, 1):
            share[:, side] += np.where(s == side, body.skin_weight[:, c], 0.0)
    which = np.where(share[:, 0] > 0.5, 0, np.where(share[:, 1] > 0.5, 1, -1))
    keep = which >= 0
    return np.nonzero(keep)[0], which[keep]


def reach_leg(body, thigh, calf, foot, target):
    """citizenStride.ts reach: the two leg bones bent forward so the ankle meets `target`."""
    hip, knee, ankle = body.position(thigh), body.position(calf), body.position(foot)
    upper, lower = np.linalg.norm(knee - hip), np.linalg.norm(ankle - knee)
    direction = target - hip
    distance = min(upper + lower - 1e-6, max(abs(upper - lower) + 1e-6, np.linalg.norm(direction)))
    direction = unit(direction)
    bend = unit(np.array([0.0, 0.0, 1.0]) - direction * direction[2])
    along = (upper * upper + distance * distance - lower * lower) / (2 * distance)
    across = np.sqrt(max(0.0, upper * upper - along * along))
    body.aim(thigh, knee, hip + direction * along + bend * across)
    body.aim(calf, body.position(foot), hip + direction * distance)


class Directional:
    """A capture walked aside or back by turning the feet's stride about their mean, the legs reaching for them."""

    def __init__(self, body, clip, amplitude, angle):
        self.walk = Transfer(body, clip.source, amplitude)
        self.body = body
        self.legs = [[body.node(f'Bip01_{side}_{p}') for p in ('Thigh', 'Calf', 'Foot')] for side in 'LR']
        samples = len(clip.source.frames_q)
        soles, sides = foot_sides(body)
        means = [np.zeros(3), np.zeros(3)]
        feet = []
        inward = [0.0, 0.0]
        for i in range(samples):
            self.walk.pose(i * clip.duration / samples)
            positions = [body.position(leg[2]) for leg in self.legs]
            means[0] += positions[0]
            means[1] += positions[1]
            feet.append(positions)
            points = body.skinned_vertices()[soles]
            left, right = points[sides == 0], points[sides == 1]
            inward[0] = max(inward[0], float(np.max(positions[0][0] - left[:, 0])))
            inward[1] = max(inward[1], float(np.max(right[:, 0] - positions[1][0])))
        self.origins = [m / samples for m in means]
        self.means = [m.copy() for m in self.origins]
        self.sin, self.cos = np.sin(angle), np.cos(angle)
        self.stride_scale = 1.0
        if abs(self.sin) > abs(self.cos):
            clearance = inward[0] + inward[1]
            self.walk.pose(0)
            hips = abs(body.position(self.legs[0][0])[0] - body.position(self.legs[1][0])[0])
            centre = (self.origins[0][0] + self.origins[1][0]) / 2
            stance = max(self.origins[0][0] - self.origins[1][0], hips + clearance)
            self.means[0][0] = centre + stance / 2
            self.means[1][0] = centre - stance / 2
            closing = 0.0
            for pair in feet:
                dx = (pair[1][0] - self.origins[1][0]) - (pair[0][0] - self.origins[0][0])
                dz = (pair[1][2] - self.origins[1][2]) - (pair[0][2] - self.origins[0][2])
                closing = max(closing, dx * self.cos + dz * self.sin)
            if closing > 0:
                self.stride_scale = min(1.0, max(0.0, (stance - clearance) / closing))
        self.scale = self.walk.scale

    def pose(self, time):
        body = self.body
        self.walk.pose(time)
        for leg, origin, mean in zip(self.legs, self.origins, self.means):
            at = body.position(leg[2])
            orientation = body.wq[leg[2]].copy()
            dx, dz = at[0] - origin[0], at[2] - origin[2]
            target = np.array([mean[0] + (dx * self.cos + dz * self.sin) * self.stride_scale, at[1],
                               mean[2] + (-dx * self.sin + dz * self.cos) * self.stride_scale])
            reach_leg(body, leg[0], leg[1], leg[2], target)
            body.set_world_rotation(leg[2], orientation)
