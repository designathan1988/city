# The lab's inverse kinematics for a seated or riding body (render/riderPoses.ts apply, limb, aim, levelFoot, reachForBars, lowestFoot): the trunk
# leant and twisted over the spine, the head turned and nodded, the limbs reaching their targets in the pelvis frame (+X left, +Y up, +Z
# forward, metres), the feet levelled. Poses a lab_rig.Body at rest.
import numpy as np

from lab_math import unit

LIMB_REACH = 0.999  # limb(): the most of a limb's length it is stretched to
NECK_TURN, HEAD_TURN = 0.7, 0.3  # of a turn of the head, the neck's share and the head's
NECK_NOD, HEAD_NOD = 0.6, 0.4  # of a nod, the neck's share and the head's
LEAN_IN_NOD = 0.6  # the lean the head takes back out of its look
FOOT_LEVEL = (0.0, -0.35, 1.0)  # levelFoot: the sole forward and a little down
REACH_STEP, REACH_LEAN_MOST, REACH_TWIST_MOST = 0.01, 0.5, 0.5  # reachForBars: a step of the trunk's turn, and its most
BONES = dict(pelvis='Bip01_Pelvis', spine=['Bip01_Spine', 'Bip01_Spine1', 'Bip01_Spine2'], neck='Bip01_Neck', head='Bip01_Head',
             leftLeg=['Bip01_L_Thigh', 'Bip01_L_Calf', 'Bip01_L_Foot'], rightLeg=['Bip01_R_Thigh', 'Bip01_R_Calf', 'Bip01_R_Foot'],
             leftArm=['Bip01_L_UpperArm', 'Bip01_L_Forearm', 'Bip01_L_Hand'], rightArm=['Bip01_R_UpperArm', 'Bip01_R_Forearm', 'Bip01_R_Hand'])
X, Y = np.array([1.0, 0.0, 0.0]), np.array([0.0, 1.0, 0.0])


def mirror_pole(p, side):
    return (p[0] * side, p[1], p[2])


def limb(body, names, target, pole):
    """Two-bone IK: the root bone, the middle joint and the end effector, reaching for `target`."""
    root, mid, end = (body.node(n) for n in names)
    if min(root, mid, end) < 0:
        return
    a0, b0, c0 = body.position(root), body.position(mid), body.position(end)
    upper, lower = np.linalg.norm(b0 - a0), np.linalg.norm(c0 - b0)
    to_target = target - a0
    reach = min((upper + lower) * LIMB_REACH, max(abs(upper - lower) * 1.001 + 1e-4, np.linalg.norm(to_target)))
    d = unit(to_target)
    p = np.array(pole, dtype=float)
    p = p - d * np.dot(p, d)
    p = np.array([0.0, 0.0, 1.0]) if np.dot(p, p) < 1e-8 else unit(p)
    cos = min(1.0, max(-1.0, (upper * upper + reach * reach - lower * lower) / (2 * upper * reach)))
    sin = np.sqrt(1 - cos * cos)
    joint = a0 + d * upper * cos + p * upper * sin
    body.aim(root, b0, joint)
    body.aim(mid, body.position(end), a0 + d * reach)


def level_foot(body, foot):
    toes = [c for c in body.children[foot] if c in body.bones]
    if not toes:
        return
    at, tip = body.position(foot), body.position(toes[0])
    length = np.linalg.norm(tip - at)
    if length < 1e-4:
        return
    body.aim(foot, tip, at + unit(np.array(FOOT_LEVEL)) * length)


def reach_for_bars(body, spine, left, right):
    arms = []
    for names, target in ((BONES['leftArm'], left), (BONES['rightArm'], right)):
        root, mid, end = (body.node(n) for n in names)
        length = np.linalg.norm(body.position(mid) - body.position(root)) + np.linalg.norm(body.position(end) - body.position(mid))
        arms.append((root, length, target))

    def short():
        return max(max(0.0, np.linalg.norm(body.position(r) - t) - length * LIMB_REACH) for r, length, t in arms)

    def turn(axis, angle):
        for bone in spine:
            body.rotate_world(bone, axis, angle / len(spine))

    deficit, leant, twisted = short(), 0.0, 0.0
    while deficit > 0:
        best = None
        for axis, angle, room in ((X, REACH_STEP, REACH_LEAN_MOST - leant), (Y, REACH_STEP, REACH_TWIST_MOST - twisted),
                                  (Y, -REACH_STEP, REACH_TWIST_MOST + twisted)):
            if room < REACH_STEP:
                continue
            turn(axis, angle)
            nxt = short()
            turn(axis, -angle)
            if nxt < deficit and (best is None or nxt < best[2]):
                best = (axis, angle, nxt)
        if best is None:
            break
        turn(best[0], best[1])
        if best[0] is X:
            leant += best[1]
        else:
            twisted += best[1]
        deficit = best[2]


def is_ancestor(body, bone, of):
    p = body.parent[of] if of >= 0 else -1
    while p >= 0:
        if p == bone:
            return True
        p = body.parent[p]
    return False


def apply(body, t):
    """Poses the rig, at rest, so its limbs reach the targets (riderPoses.ts apply)."""
    pelvis = body.node(BONES['pelvis'])
    origin = body.position(pelvis)

    def world(v):
        return np.array(v, dtype=float) + origin

    thigh = body.node(BONES['leftLeg'][0])
    spine = [b for b in (body.node(n) for n in BONES['spine']) if b >= 0 and not is_ancestor(body, b, thigh)]
    for bone in spine:
        body.rotate_world(bone, X, t['lean'] / len(spine))
        if t.get('twist'):
            body.rotate_world(bone, Y, t['twist'] / len(spine))
    neck, head = body.node(BONES['neck']), body.node(BONES['head'])
    turn = t.get('turn', 0.0) - t.get('twist', 0.0)
    if turn:
        body.rotate_world(neck, Y, turn * NECK_TURN)
        body.rotate_world(head, Y, turn * HEAD_TURN)
    nod = t['look'] - t['lean'] * LEAN_IN_NOD if t.get('look') else 0.0
    if nod:
        body.rotate_world(neck, X, nod * NECK_NOD)
        body.rotate_world(head, X, nod * HEAD_NOD)
    if t.get('reach'):
        reach_for_bars(body, spine, world(t['leftHand']), world(t['rightHand']))
    knee = t.get('kneePole', (0, 0, 1))
    elbow = t.get('elbowPole', (0, -1, 0))
    limb(body, BONES['leftLeg'], world(t['leftFoot']), knee)
    limb(body, BONES['rightLeg'], world(t['rightFoot']), mirror_pole(knee, -1))
    limb(body, BONES['leftArm'], world(t['leftHand']), mirror_pole(elbow, 1))
    limb(body, BONES['rightArm'], world(t['rightHand']), mirror_pole(elbow, -1))
    for name in (BONES['leftLeg'][2], BONES['rightLeg'][2]):
        level_foot(body, body.node(name))


def lowest_foot(body):
    """The lowest point the feet carry (vertices half or more on a foot or toe bone), metres from the pelvis."""
    carried = set()
    for name in (BONES['leftLeg'][2], BONES['rightLeg'][2]):
        stack = [body.node(name)]
        while stack:
            n = stack.pop()
            carried.add(n)
            stack.extend(body.children[n])
    on = np.array([b in carried for b in body.bones])
    share = sum(np.where(on[body.skin_index[:, c]], body.skin_weight[:, c], 0.0) for c in range(4))
    keep = share >= 0.5
    if not keep.any():
        return None
    return float(body.skinned_vertices()[keep][:, 1].min() - body.position(body.node(BONES['pelvis']))[1])
