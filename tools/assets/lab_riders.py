# People sitting in and on vehicles, posed on the lab's cast by inverse kinematics (the lab's render/riderPoses.ts; the solve is lab_ik.py):
# the targets of each seat in the pelvis frame (+X left, +Y up, +Z forward, metres), a stopped rider's foot down, the seated idle that
# breathes and turns its head, and the look-asides (a head turned, a phone in the lap). Poses a lab_rig.Body.
import numpy as np

from lab_ik import BONES, apply, lowest_foot

SEATED_IDLE = 6.0  # riderPoses.ts: seconds of a seated person's idle loop
SOLE = 0.08  # the ankle over the floor where an average shoe's sole meets it
STEER_FULL = 0.3  # the largest steering angle a steered pose is baked at, radians
STOP_TILT_MOST = 0.4  # the most a stopped machine tilts onto its rider's foot, radians
LEG_REACH = 0.999  # of a leg's length, the most its foot-down target may stand from the hip
TWIST_OF_TURN = 0.15  # turned(): the shoulders take this much of the head's turn
SWAY_LEAN, SWAY_TURN, SWAY_LOOK = 0.03, 0.14, 0.05  # seated(): breathing in the trunk, the head turning and nodding
SWAY_TURN_PHASE, SWAY_LOOK_PHASE = 1.3, 0.4

DRIVER_WHEEL = dict(forward=0.46, up=0.36, tilt=0.45, radius=0.185)
CAB_WHEEL = dict(forward=0.42, up=0.13, tilt=1.0, radius=0.22)
MOTO_FIT = dict(pelvisY=0.96, saddleDrop=0.1, grip=dict(side=0.31, up=0.2, forward=0.52), steerAxis=0.8,
                peg=dict(side=0.24, up=-0.6, forward=0.1), stopTilt=0.14, footDown=dict(side=0.42, forward=0.12))
BIKE_FIT = dict(pelvisY=0.98, saddleDrop=0.08, grip=dict(side=0.27, up=0.1, forward=0.52), steerAxis=0.72,
                bracket=dict(up=-0.68, forward=0.22), crank=0.17, pedalSide=0.13, stopTilt=0.16, footDown=dict(side=0.34, forward=0.1))
WRIST_ON_GRIP = (0.02, 0.03, -0.06)
ANKLE_OVER_PEDAL = (0.0, 0.07, -0.08)
ANKLE_UP = 0.08  # footDownTarget: the ankle over the road



def rim_point(clock, wheel=DRIVER_WHEEL):
    across = np.sin(clock) * wheel['radius']
    along = np.cos(clock) * wheel['radius']
    return (across, wheel['up'] + along * np.cos(wheel['tilt']), wheel['forward'] - along * np.sin(wheel['tilt']))


def offset(p, d, side):
    return (p[0] + d[0] * side, p[1] + d[1], p[2] + d[2])


def grip_point(fit, side, steer=0.0):
    x = side * fit['grip']['side']
    z = fit['grip']['forward'] - fit['steerAxis']
    c, s = np.cos(steer), np.sin(steer)
    return (x * c + z * s, fit['grip']['up'], -x * s + z * c + fit['steerAxis'])


def pedal_point(fit, angle, side):
    b = fit['bracket']
    return (side * fit['pedalSide'], b['up'] + np.sin(angle) * fit['crank'], b['forward'] + np.cos(angle) * fit['crank'])


def foot_down_target(fit, tilt):
    x = fit['footDown']['side'] * np.cos(tilt) - ANKLE_UP * np.sin(tilt)
    h = fit['footDown']['side'] * np.sin(tilt) + ANKLE_UP * np.cos(tilt)
    return (x, h - fit['pelvisY'], fit['footDown']['forward'])


def hands(fit, steer=0.0):
    return dict(leftHand=offset(grip_point(fit, 1, steer), WRIST_ON_GRIP, -1), rightHand=offset(grip_point(fit, -1, steer), WRIST_ON_GRIP, 1))


CAR_DRIVE = dict(lean=-0.45, leftFoot=(0.17, -0.08, 0.62), rightFoot=(-0.14, -0.08, 0.64), leftHand=rim_point(1.05), rightHand=rim_point(-1.05),
                 kneePole=(0, 1, 1), elbowPole=(0, -1, 0.2), look=-0.12)
CAR_RIDE = dict(lean=-0.45, leftFoot=(0.16, -0.08, 0.61), rightFoot=(-0.16, -0.08, 0.61), leftHand=(0.13, 0.06, 0.3), rightHand=(-0.13, 0.06, 0.3),
                kneePole=(0, 1, 1), elbowPole=(0.3, -1, 0), look=-0.1)
CAR_REAR_RIDE = dict(CAR_RIDE, lean=-0.38, leftFoot=(0.15, -0.13, 0.43), rightFoot=(-0.15, -0.13, 0.44), leftHand=(0.14, 0.04, 0.23),
                     rightHand=(-0.14, 0.04, 0.23))
CAB_DRIVE = dict(lean=-0.42, leftFoot=(0.16, -0.37, 0.4), rightFoot=(-0.15, -0.37, 0.43), leftHand=rim_point(1.15, CAB_WHEEL),
                 rightHand=rim_point(-1.15, CAB_WHEEL), kneePole=(0, 1, 1), elbowPole=(0, -1, 0.1), look=-0.05)
CAB_RIDE = dict(lean=-0.42, leftFoot=(0.16, -0.37, 0.38), rightFoot=(-0.16, -0.37, 0.38), leftHand=(0.13, 0.02, 0.27), rightHand=(-0.13, 0.02, 0.27),
                kneePole=(0, 1, 1), elbowPole=(0.3, -1, 0), look=0.0)
CHAIR_SIT = dict(lean=-0.3, leftFoot=(0.12, -0.46, 0.4), rightFoot=(-0.12, -0.46, 0.4), leftHand=(0.12, 0.03, 0.27), rightHand=(-0.12, 0.03, 0.27),
                 kneePole=(0, 1, 1), elbowPole=(0.3, -1, 0), look=0.05)
MOTO = dict(lean=0.28, reach=True, **hands(MOTO_FIT),
            leftFoot=offset((MOTO_FIT['peg']['side'], MOTO_FIT['peg']['up'], MOTO_FIT['peg']['forward']), ANKLE_OVER_PEDAL, 1),
            rightFoot=offset((-MOTO_FIT['peg']['side'], MOTO_FIT['peg']['up'], MOTO_FIT['peg']['forward']), ANKLE_OVER_PEDAL, -1),
            kneePole=(0.5, 0.1, 1), elbowPole=(0.6, -1, 0), look=-0.12)
MOTO_STOP = dict(MOTO, lean=0.12)
BIKE = dict(lean=0.32, reach=True, **hands(BIKE_FIT), kneePole=(0.1, 0.2, 1), elbowPole=(0.4, -1, 0), look=-0.2)


def turned(t, turn, look=None):
    """A head turned, and the shoulders a little with it (riderPoses.ts turned)."""
    return dict(t, turn=turn, look=t.get('look', 0.0) if look is None else look, twist=t.get('twist', 0.0) + turn * TWIST_OF_TURN)


def pedal_foot(angle, side):
    return offset(pedal_point(BIKE_FIT, angle, side), ANKLE_OVER_PEDAL, side)


def stop_tilt_for(body, fit):
    body.reset()
    origin = body.position(body.node(BONES['pelvis']))
    thigh, calf, foot = (body.node(n) for n in BONES['leftLeg'])
    hip = body.position(thigh) - origin
    leg = np.linalg.norm(body.position(calf) - body.position(thigh)) + np.linalg.norm(body.position(foot) - body.position(calf))

    def reaches(tilt):
        return np.linalg.norm(hip - np.array(foot_down_target(fit, tilt))) <= leg * LEG_REACH

    if reaches(fit['stopTilt']):
        return fit['stopTilt']
    if not reaches(STOP_TILT_MOST):
        return STOP_TILT_MOST
    lo, hi = fit['stopTilt'], STOP_TILT_MOST
    while hi - lo > 1e-3:
        mid = (lo + hi) / 2
        if reaches(mid):
            hi = mid
        else:
            lo = mid
    return hi


def seated_pose(targets):
    """A seated pose that lives: breathing in the trunk, the head turning a little and nodding (riderPoses.ts seated)."""
    lift = {}  # by body: the lab keeps one a rig (soleLifts, a WeakMap on the rig); a worker bakes several bodies

    def pose(body, time):
        a = time / SEATED_IDLE * np.pi * 2
        floor = min(targets['leftFoot'][1], targets['rightFoot'][1]) - SOLE
        sway = dict(targets, lean=targets['lean'] + SWAY_LEAN * np.sin(a * 2), turn=targets.get('turn', 0.0) + SWAY_TURN * np.sin(a + SWAY_TURN_PHASE),
                    look=targets.get('look', 0.0) + SWAY_LOOK * np.sin(a * 3 + SWAY_LOOK_PHASE))
        if body.id not in lift:  # solved once as it is, measured, and put back (soleLift)
            body.reset()
            apply(body, sway)
            low = lowest_foot(body)
            lift[body.id] = 0.0 if low is None else floor - low
            body.reset()
        up = lift[body.id]
        if up:
            sway = dict(sway, leftFoot=(sway['leftFoot'][0], sway['leftFoot'][1] + up, sway['leftFoot'][2]),
                        rightFoot=(sway['rightFoot'][0], sway['rightFoot'][1] + up, sway['rightFoot'][2]))
        apply(body, sway)
    return pose


def still_pose(targets):
    return lambda body, time: apply(body, targets)


def stopped_pose(fit, targets):
    def pose(body, time):
        tilt = stop_tilt_for(body, fit)
        body.reset()
        apply(body, dict(targets, leftFoot=foot_down_target(fit, tilt)))
    return pose


def bike_pedal(body, time):
    angle = time * np.pi * 2
    apply(body, dict(BIKE, leftFoot=pedal_foot(angle, 1), rightFoot=pedal_foot(angle + np.pi, -1)))


SEATED_FPS, PEDAL_FPS, STILL_FRAMES = 6, 30, 2  # citizenBake.ts bakeRiderClip: a seated loop at 6 fps, the pedalling at 30, a still pose two frames
CHAIR_PHONE = dict(CHAIR_SIT, leftHand=(0.05, 0.14, 0.3), rightHand=(-0.05, 0.14, 0.3), elbowPole=(0.6, -1, -0.2), look=0.55)

# The game's rider clips: (name, pose(body, time), seconds, frames). The look-asides are the lab's glances (render/occupants.ts plays them).
RIDER_CLIPS = [
    ('carDrive', seated_pose(CAR_DRIVE), SEATED_IDLE, int(SEATED_IDLE * SEATED_FPS)),
    ('carDriveMirror', seated_pose(turned(CAR_DRIVE, 0.45, -0.05)), SEATED_IDLE, int(SEATED_IDLE * SEATED_FPS)),
    ('carDriveRight', seated_pose(turned(CAR_DRIVE, -0.45)), SEATED_IDLE, int(SEATED_IDLE * SEATED_FPS)),
    ('carRide', seated_pose(CAR_RIDE), SEATED_IDLE, int(SEATED_IDLE * SEATED_FPS)),
    ('carRideLeft', seated_pose(turned(CAR_RIDE, 0.45)), SEATED_IDLE, int(SEATED_IDLE * SEATED_FPS)),
    ('carRideRight', seated_pose(turned(CAR_RIDE, -0.45)), SEATED_IDLE, int(SEATED_IDLE * SEATED_FPS)),
    ('carRearRide', seated_pose(CAR_REAR_RIDE), SEATED_IDLE, int(SEATED_IDLE * SEATED_FPS)),
    ('carRearLeft', seated_pose(turned(CAR_REAR_RIDE, 0.45)), SEATED_IDLE, int(SEATED_IDLE * SEATED_FPS)),
    ('carRearRight', seated_pose(turned(CAR_REAR_RIDE, -0.45)), SEATED_IDLE, int(SEATED_IDLE * SEATED_FPS)),
    ('cabDrive', seated_pose(CAB_DRIVE), SEATED_IDLE, int(SEATED_IDLE * SEATED_FPS)),
    ('cabDriveMirror', seated_pose(turned(CAB_DRIVE, 0.6, 0.0)), SEATED_IDLE, int(SEATED_IDLE * SEATED_FPS)),
    ('cabDriveRight', seated_pose(turned(CAB_DRIVE, -0.5, 0.0)), SEATED_IDLE, int(SEATED_IDLE * SEATED_FPS)),
    ('cabRide', seated_pose(CAB_RIDE), SEATED_IDLE, int(SEATED_IDLE * SEATED_FPS)),
    ('cabRideLeft', seated_pose(turned(CAB_RIDE, 0.55)), SEATED_IDLE, int(SEATED_IDLE * SEATED_FPS)),
    ('cabRideRight', seated_pose(turned(CAB_RIDE, -0.6)), SEATED_IDLE, int(SEATED_IDLE * SEATED_FPS)),
    ('chairSit', seated_pose(CHAIR_SIT), SEATED_IDLE, int(SEATED_IDLE * SEATED_FPS)),
    ('chairSitLeft', seated_pose(turned(CHAIR_SIT, 0.65)), SEATED_IDLE, int(SEATED_IDLE * SEATED_FPS)),
    ('chairSitRight', seated_pose(turned(CHAIR_SIT, -0.65)), SEATED_IDLE, int(SEATED_IDLE * SEATED_FPS)),
    ('chairSitPhone', seated_pose(CHAIR_PHONE), SEATED_IDLE, int(SEATED_IDLE * SEATED_FPS)),
    ('motoRide', still_pose(MOTO), 1.0, STILL_FRAMES),
    ('motoStop', stopped_pose(MOTO_FIT, MOTO_STOP), 1.0, STILL_FRAMES),
    ('bikePedal', bike_pedal, 1.0, PEDAL_FPS),
    ('bikeStop', stopped_pose(BIKE_FIT, dict(BIKE, lean=0.18, rightFoot=pedal_foot(np.pi * 0.6, -1))), 1.0, STILL_FRAMES),
]
