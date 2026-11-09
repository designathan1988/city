# The quaternion and matrix arithmetic of three.js the lab's bakes rely on (Quaternion.slerp, setFromUnitVectors, setFromRotationMatrix,
# Matrix4.compose), on numpy arrays: quaternions (x, y, z, w), matrices 4x4 row-major. Read by lab_rig.py, lab_shaping.py, lab_ik.py.
import numpy as np

EPS = 1e-9  # a length or a turn under this is none


# ------------------------------------------------------------------ quaternions

def qmul(a, b):
    ax, ay, az, aw = a
    bx, by, bz, bw = b
    return np.array([aw * bx + ax * bw + ay * bz - az * by, aw * by - ax * bz + ay * bw + az * bx,
                     aw * bz + ax * by - ay * bx + az * bw, aw * bw - ax * bx - ay * by - az * bz])


def qinv(q):
    return np.array([-q[0], -q[1], -q[2], q[3]]) / max(EPS, float(np.dot(q, q)))


def qnorm(q):
    return q / max(EPS, float(np.linalg.norm(q)))


def qrot(q, v):
    """v turned by q."""
    u = np.array(q[:3])
    w = q[3]
    t = 2.0 * np.cross(u, v)
    return v + w * t + np.cross(u, t)


def qslerp(a, b, t):
    """three's Quaternion.slerp: the shorter way, a normalised lerp where the two nearly agree."""
    if t == 0:
        return np.array(a, dtype=float)
    if t == 1:
        return np.array(b, dtype=float)
    cos_half = float(np.dot(a, b))
    bb = np.array(b, dtype=float)
    if cos_half < 0:
        bb = -bb
        cos_half = -cos_half
    if cos_half >= 1.0:
        return np.array(a, dtype=float)
    sqr_sin = 1.0 - cos_half * cos_half
    if sqr_sin <= np.finfo(float).eps:
        return qnorm((1 - t) * np.array(a) + t * bb)
    sin_half = np.sqrt(sqr_sin)
    half = np.arctan2(sin_half, cos_half)
    return np.sin((1 - t) * half) / sin_half * np.array(a) + np.sin(t * half) / sin_half * bb


def qaxis(axis, angle):
    axis = np.asarray(axis, dtype=float)
    s = np.sin(angle / 2)
    return np.array([axis[0] * s, axis[1] * s, axis[2] * s, np.cos(angle / 2)])


def qfrom_units(a, b):
    """three's setFromUnitVectors."""
    r = float(np.dot(a, b)) + 1.0
    if r < EPS:
        q = np.array([-a[1], a[0], 0.0, 0.0]) if abs(a[0]) > abs(a[2]) else np.array([0.0, -a[2], a[1], 0.0])
    else:
        c = np.cross(a, b)
        q = np.array([c[0], c[1], c[2], r])
    return qnorm(q)


def qmatrix(q):
    x, y, z, w = q
    return np.array([[1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)],
                     [2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)],
                     [2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)]])


def mat_quat(m):
    """The rotation of a 3x3 (three's setFromRotationMatrix)."""
    m11, m12, m13 = m[0]
    m21, m22, m23 = m[1]
    m31, m32, m33 = m[2]
    trace = m11 + m22 + m33
    if trace > 0:
        s = 0.5 / np.sqrt(trace + 1.0)
        return np.array([(m32 - m23) * s, (m13 - m31) * s, (m21 - m12) * s, 0.25 / s])
    if m11 > m22 and m11 > m33:
        s = 2.0 * np.sqrt(1.0 + m11 - m22 - m33)
        return np.array([0.25 * s, (m12 + m21) / s, (m13 + m31) / s, (m32 - m23) / s])
    if m22 > m33:
        s = 2.0 * np.sqrt(1.0 + m22 - m11 - m33)
        return np.array([(m12 + m21) / s, 0.25 * s, (m23 + m32) / s, (m13 - m31) / s])
    s = 2.0 * np.sqrt(1.0 + m33 - m11 - m22)
    return np.array([(m13 + m31) / s, (m23 + m32) / s, 0.25 * s, (m21 - m12) / s])


def compose(p, q, s):
    m = np.eye(4)
    m[:3, :3] = qmatrix(q) * np.asarray(s, dtype=float)
    m[:3, 3] = p
    return m


def decompose_rotation(m):
    r = m[:3, :3] / np.linalg.norm(m[:3, :3], axis=0)
    return qnorm(mat_quat(r))


def unit(v):
    return v / max(EPS, float(np.linalg.norm(v)))
