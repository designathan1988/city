"""What a ragdoll needs of a cooked person: read by cook_people.py."""
import numpy as np

# The ragdoll's joints and parts (the 4.0's ragdoll.ts): which bone each particle sits on, the parts in their order, which part each key bone follows.
RAGDOLL_JOINTS = ["Bip01_Pelvis", "Bip01_Spine2", "Bip01_Neck", "Bip01_Head", "Bip01_L_UpperArm", "Bip01_L_Forearm", "Bip01_L_Hand", "Bip01_R_UpperArm", "Bip01_R_Forearm",
                  "Bip01_R_Hand", "Bip01_L_Thigh", "Bip01_L_Calf", "Bip01_L_Foot", "Bip01_L_Toe0", "Bip01_R_Thigh", "Bip01_R_Calf", "Bip01_R_Foot", "Bip01_R_Toe0"]
RAGDOLL_PARTS = ["torso", "neck", "head", "lua", "lfa", "rua", "rfa", "lth", "lca", "lfo", "rth", "rca", "rfo"]
KEY_PART = {"Bip01": "torso", "Bip01_Pelvis": "torso", "Bip01_Spine": "torso", "Bip01_Spine1": "torso", "Bip01_Spine2": "torso", "Bip01_L_Clavicle": "torso", "Bip01_R_Clavicle": "torso",
            "Bip01_Neck": "neck", "Bip01_Head": "head", "Bip01_L_UpperArm": "lua", "Bip01_L_Forearm": "lfa", "Bip01_L_Hand": "lfa", "Bip01_R_UpperArm": "rua", "Bip01_R_Forearm": "rfa",
            "Bip01_R_Hand": "rfa", "Bip01_L_Thigh": "lth", "Bip01_L_Calf": "lca", "Bip01_L_Foot": "lfo", "Bip01_L_Toe0": "lfo", "Bip01_R_Thigh": "rth", "Bip01_R_Calf": "rca",
            "Bip01_R_Foot": "rfo", "Bip01_R_Toe0": "rfo"}


def ragdoll(head):
    """Which part each of the file's bones follows (a bone not named follows its nearest named ancestor) and where the ragdoll's joints stand at rest."""
    m = head["mesh"]
    nodes = head["nodes"]
    names = [nodes[i].get("name") for i in m["bones"]]

    def part_of(i):
        while i >= 0:
            if nodes[i].get("name") in KEY_PART:
                return RAGDOLL_PARTS.index(KEY_PART[nodes[i]["name"]])
            i = nodes[i].get("parent", -1)
        return 0

    inverses = np.array(m["inverses"], dtype=np.float64)
    rest = [np.linalg.inv(inverses[names.index(n) * 16:][:16].reshape(4, 4).T)[:3, 3] for n in RAGDOLL_JOINTS]
    return [part_of(i) for i in m["bones"]], [names.index(n) for n in RAGDOLL_JOINTS], np.array(rest, dtype="<f4")
