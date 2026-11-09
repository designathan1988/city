# The lab's cast as a rig to pose (the lab's render/people/cookedPerson.ts files, public/models/citizens/people/<id>.bin): its nodes, its
# skinned mesh's bones and bind inverses, its vertices and skin, and the pose math the lab's bakes run on three's scene graph
# (render/citizenWalk.ts transferOnto), here on arrays; the arms' room and the side steps are lab_shaping.py, the math lab_math.py.
# Quaternions are (x, y, z, w) as three's; matrices 4x4 row-major numpy arrays; positions metres in the model's frame (+Y up, +Z forward).
import base64
import json
import re
import struct

import numpy as np

from lab_math import compose, decompose_rotation, qinv, qmul, qnorm, qrot, qslerp, unit, qaxis, qfrom_units

VERSION = 3  # cookedPerson.ts COOKED_VERSION
KINDS = {'f32': np.float32, 'u32': np.uint32, 'u16': np.uint16, 'i16': np.int16, 'u8': np.uint8, 'i8': np.int8}
I16_MAX = 32767.0  # a normalised signed short
U8_MAX = 255.0  # a normalised byte


# ------------------------------------------------------------------ the cooked body

class Body:
    """One cooked person of the lab: its scene's nodes as arrays, its skin, and its bind."""

    def __init__(self, path):
        raw = open(path, 'rb').read()
        length, = struct.unpack_from('<I', raw, 0)
        header = json.loads(raw[4:4 + length])
        if header['version'] != VERSION:
            raise ValueError(f'{path}: cooked version {header["version"]}, {VERSION} expected')
        base = 4 + ((length + 3) & ~3)
        mesh = header['mesh']

        def array(ref):
            return np.frombuffer(raw, dtype=KINDS[ref['kind']], count=ref['length'], offset=base + ref['at'])

        self.id = header['id']
        nodes = header['nodes']
        self.names = [n['name'] for n in nodes]
        self.parent = [n['parent'] for n in nodes]
        for i, p in enumerate(self.parent):
            if p >= i:
                raise ValueError(f'{path}: node {i} before its parent {p}')
        self.rest_p = np.array([n['p'] for n in nodes], dtype=float)
        self.rest_q = np.array([n['q'] for n in nodes], dtype=float)
        self.rest_s = np.array([n['s'] for n in nodes], dtype=float)
        # assemblePerson: the mesh's node dequantises (its position the box's offset, its scale the box's)
        self.mesh_node = mesh['node']
        self.rest_p[self.mesh_node] = mesh['quant']['offset']
        self.rest_s[self.mesh_node] = [mesh['quant']['scale']] * 3
        self.bones = list(mesh['bones'])
        self.inverses = np.array(mesh['inverses'], dtype=float).reshape(-1, 4, 4).transpose(0, 2, 1)  # column-major in the file
        self.bind = np.array(mesh['bind'], dtype=float).reshape(4, 4).T
        attributes = [array(ref) for ref in mesh['attributes']]
        quant = attributes[0].astype(float).reshape(-1, 3) / I16_MAX
        self.vertices = quant * mesh['quant']['scale'] + np.array(mesh['quant']['offset'])
        self.skin_index = attributes[4].reshape(-1, 4).astype(int)
        self.skin_weight = attributes[5].astype(float).reshape(-1, 4) / U8_MAX
        self.by_name = {n: i for i, n in enumerate(self.names)}
        self.children = [[] for _ in nodes]
        for i, p in enumerate(self.parent):
            if p >= 0:
                self.children[p].append(i)
        self.reset()

    def reset(self):
        self.p = self.rest_p.copy()
        self.q = self.rest_q.copy()
        self.s = self.rest_s.copy()
        self.update()

    def update(self):
        n = len(self.names)
        self.world = np.zeros((n, 4, 4))
        self.wq = np.zeros((n, 4))
        for i in range(n):
            local = compose(self.p[i], self.q[i], self.s[i])
            par = self.parent[i]
            self.world[i] = local if par < 0 else self.world[par] @ local
            self.wq[i] = self.q[i] if par < 0 else qnorm(qmul(self.wq[par], self.q[i]))

    def node(self, name):
        return self.by_name.get(name, -1)

    def position(self, i):
        return self.world[i][:3, 3].copy()

    def set_world_rotation(self, i, q):
        """bone.quaternion so that its world rotation is q, its parent kept."""
        par = self.parent[i]
        self.q[i] = qnorm(q if par < 0 else qmul(qinv(self.wq[par]), q))
        self.update()

    def rotate_world(self, i, axis, angle):
        self.set_world_rotation(i, qmul(qaxis(axis, angle), self.wq[i]))

    def aim(self, i, child_at, target):
        """Turns node i so the direction to `child_at` points at `target` (riderPoses.ts aim)."""
        origin = self.position(i)
        a = child_at - origin
        b = target - origin
        if np.dot(a, a) < 1e-10 or np.dot(b, b) < 1e-10:
            return
        self.set_world_rotation(i, qmul(qfrom_units(unit(a), unit(b)), self.wq[i]))

    def place_world(self, i, at):
        """node.position so its world position is `at` (parent.worldToLocal)."""
        par = self.parent[i]
        self.p[i] = (np.linalg.inv(self.world[par]) @ np.append(at, 1.0))[:3]
        self.update()

    def skin(self):
        """three's skeleton.boneMatrices: each bone's world times its bind inverse, as the game's palette takes them."""
        return np.array([self.world[b] @ self.inverses[k] for k, b in enumerate(self.bones)])

    def skinned_vertices(self, matrices=None):
        m = self.skin() if matrices is None else matrices
        v = np.concatenate([self.vertices, np.ones((len(self.vertices), 1))], axis=1)
        out = np.zeros((len(v), 3))
        for c in range(4):
            out += self.skin_weight[:, c:c + 1] * np.einsum('nij,nj->ni', m[self.skin_index[:, c]], v)[:, :3]
        return out

    def bind_world(self, k):
        """A bone's world at bind (citizenWalk.ts bindWorld): mesh.matrixWorld x bindMatrixInverse x boneInverse^-1."""
        return self.world[self.mesh_node] @ np.linalg.inv(self.bind) @ np.linalg.inv(self.inverses[k])


# ------------------------------------------------------------------ the motion library

class Source:
    """A capture as a transfer source (citizenWalk.ts sourceOf): its frames, the bind inverted, the swing's centres."""

    def __init__(self, bones, duration, loop, frames_q, pelvis, lowest, bind, bind_lowest):
        self.bones = bones
        self.duration = duration
        self.loop = loop
        self.frames_q = frames_q  # (frames, bones, 4)
        self.pelvis_frames = pelvis  # (frames, 3)
        self.lowest = lowest  # (frames,)
        self.bind_lowest = bind_lowest
        pi = bones.index('Bip01_Pelvis')
        self.bind_inverse = [qinv(np.array(b['q'], dtype=float)) for b in bind]
        self.pelvis_bind = np.array(bind[pi]['p'], dtype=float)
        self.bind_p = [np.array(b['p'], dtype=float) for b in bind]
        first = frames_q[0]
        self.mean = []
        for k in range(len(bones)):
            signs = np.where(np.einsum('fi,i->f', frames_q[:, k], first[k]) < 0, -1.0, 1.0)
            self.mean.append(qnorm((frames_q[:, k] * signs[:, None]).sum(axis=0)))
        self.pelvis_mean = pelvis.mean(axis=0)


def walk_source(path):
    w = json.load(open(path))
    frames_q = np.array([f['q'] for f in w['frames']], dtype=float)
    pelvis = np.array([f['pelvis'] for f in w['frames']], dtype=float)
    lowest = np.array([f['lowest'] for f in w['frames']], dtype=float)
    return w, Source(w['bones'], w['duration'], True, frames_q, pelvis, lowest, w['bind'], w['bindLowest'])


class LibraryClip:
    """One clip of the Rocketbox library as citizenWalk.ts decode reads it."""

    def __init__(self, name, c, bones, walk):
        self.name = name
        self.kind = c['kind']
        self.loop = c['loop']
        self.duration = c['duration']
        self.advance = c['advance']
        n = c['frames']
        q = np.frombuffer(base64.b64decode(c['q']), dtype=np.int16).astype(float).reshape(n, len(bones), 4) / I16_MAX
        pelvis = np.frombuffer(base64.b64decode(c['pelvis']), dtype=np.float32).astype(float).reshape(n, 3)
        lowest = np.frombuffer(base64.b64decode(c['lowest']), dtype=np.float32).astype(float)
        self.travel = np.frombuffer(base64.b64decode(c['travel']), dtype=np.float32).astype(float)
        self.yaw = np.frombuffer(base64.b64decode(c['yaw']), dtype=np.float32).astype(float)
        self.source = Source(bones, c['duration'], c['loop'], q, pelvis, lowest, walk['bind'], walk['bindLowest'])


def library(path, walk):
    lib = json.load(open(path))
    return {name: LibraryClip(name, c, lib['bones'], walk) for name, c in lib['clips'].items()}


# ------------------------------------------------------------------ transfer (citizenWalk.ts transferOnto)

ARM = re.compile(r'Clavicle|UpperArm|Forearm|Hand|Finger')
LEG = re.compile(r'Thigh|Calf|Foot|Toe')
FULL = {'arms': 1.0, 'legs': 1.0, 'hips': 1.0}
FEET = ['Bip01_L_Foot', 'Bip01_R_Foot', 'Bip01_L_Toe0', 'Bip01_R_Toe0']
LEAST_HIP = 0.1  # transferOnto: a source's hip over its lowest foot is taken as at least this, metres


class Transfer:
    """A capture played onto one body: the source's world rotations made the body's by the two binds, the pelvis carried along."""

    def __init__(self, body, src, amplitude=FULL):
        self.body = body
        self.src = src
        self.amp = amplitude
        self.untouched = all(amplitude[k] == 1 for k in amplitude)
        body.update()
        self.links = []
        for si, name in enumerate(src.bones):
            i = body.node(name)
            if i < 0 or i not in body.bones:
                continue
            bind = decompose_rotation(body.bind_world(body.bones.index(i)))
            depth = 0
            p = body.parent[i]
            while p >= 0:
                depth += 1
                p = body.parent[p]
            swing = amplitude['arms'] if ARM.search(name) else amplitude['legs'] if LEG.search(name) else amplitude['hips'] if name == 'Bip01_Pelvis' else 1.0
            self.links.append((depth, i, si, bind, swing))
        self.links.sort(key=lambda link: link[0])
        self.pelvis = body.node('Bip01_Pelvis')
        self.pelvis_bind = body.bind_world(body.bones.index(self.pelvis))[:3, 3].copy()
        self.feet = [body.node(n) for n in FEET if body.node(n) >= 0]
        foot_bind = [body.bind_world(body.bones.index(i))[1, 3] for i in self.feet]
        self.lowest_bind = min(foot_bind) if foot_bind else 0.0
        self.scale = (self.pelvis_bind[1] - self.lowest_bind) / max(LEAST_HIP, src.pelvis_bind[1] - src.bind_lowest)

    def at(self, time):
        n = len(self.src.frames_q)
        if not self.src.loop:
            x = min(1.0, max(0.0, time / self.src.duration)) * (n - 1)
            k0 = min(n - 1, int(np.floor(x)))
            return k0, min(n - 1, k0 + 1), x - k0
        x = ((time / self.src.duration) % 1 + 1) % 1 * n
        k0 = int(np.floor(x)) % n
        return k0, (k0 + 1) % n, x - np.floor(x)

    def lowest_at(self, time):
        k0, k1, f = self.at(time)
        lowest = self.src.lowest[k0] * (1 - f) + self.src.lowest[k1] * f
        return self.lowest_bind + (lowest - self.src.bind_lowest) * self.scale

    def pose(self, time):
        body, src = self.body, self.src
        k0, k1, f = self.at(time)
        travel = (src.pelvis_frames[k0] * (1 - f) + src.pelvis_frames[k1] * f - src.pelvis_mean) * self.amp['hips']
        travel = (travel + src.pelvis_mean - src.pelvis_bind) * self.scale
        body.place_world(self.pelvis, self.pelvis_bind + travel)
        for _, i, si, bind, swing in self.links:
            q0 = qslerp(src.frames_q[k0, si], src.frames_q[k1, si], f)
            if swing != 1:
                q0 = qslerp(src.mean[si], q0, swing)
            want = qmul(qmul(q0, src.bind_inverse[si]), bind)
            par = body.parent[i]
            body.q[i] = qnorm(want if par < 0 else qmul(qinv(body.wq[par]), want))
            body.wq[i] = qnorm(want)  # top-down: a later link reads its parent's world rotation as just set
        body.update()
        if self.untouched or not self.feet:
            return
        # A shorter swing leaves the legs straighter under the hips, which would sink the feet into the ground
        lowest = min(body.position(i)[1] for i in self.feet)
        travel[1] += self.lowest_at(time) - lowest
        body.place_world(self.pelvis, self.pelvis_bind + travel)


def stride_share(src, amplitude):
    """citizenWalk.ts strideShare: of a cycle's stride, what is left once the legs' swing is shrunk."""
    def reach(share):
        low, high = np.inf, -np.inf
        for side in 'LR':
            names = [f'Bip01_{side}_{part}' for part in ('Thigh', 'Calf', 'Foot')]
            if any(n not in src.bones for n in names):
                continue
            chain = [src.bones.index(n) for n in names]
            for frame in src.frames_q:
                foot = np.zeros(3)
                for k in range(len(chain) - 1):
                    bone = chain[k]
                    q = frame[bone] if share == 1 else qslerp(src.mean[bone], frame[bone], share)
                    foot += qrot(qmul(q, src.bind_inverse[bone]), src.bind_p[chain[k + 1]] - src.bind_p[bone])
                low = min(low, foot[2])
                high = max(high, foot[2])
        return high - low
    return reach(amplitude['legs']) / max(1e-6, reach(1.0))
