"""The list of gate rules: each per-file rule says which files it judges and what context its result depends on; each
whole-repo rule says which files are its inputs (a --file run triggers it when a requested file is one of them)."""
import os
import sys

import gate_clones
import gate_code
import gate_data
import gate_defs
import gate_docs
import gate_files
import gate_forbidden
import gate_generated
import gate_keys
import gate_lab
import gate_language
import gate_materials
import gate_patch
import gate_python
import gate_states
import gate_text
import gate_tuning
import gate_ui
from gate_files import SIDECARS


class PerFile:
    def __init__(self, name, ids, applies, check, context=None):
        self.name, self.ids, self.applies, self.check, self.context = name, tuple(ids), applies, check, context


class Whole:
    def __init__(self, name, ids, inputs, check, extra=None):
        self.name, self.ids, self.inputs, self.check, self.extra = name, tuple(ids), inputs, check, extra


def _in_scope(predicate):
    """A whole-repo rule that reads only the gates' scope has no input outside it."""
    return lambda ctx, path: path in ctx.file_set and predicate(ctx, path)


def _everything(ctx, path):
    return True


def _ext_in(key):
    return lambda ctx, path: os.path.splitext(path)[1] in ctx.rules[key]


def _function_files(ctx, path):
    return gate_files.function_limit(ctx.rules, path) is not None and path.endswith((".gd", ".cpp", ".h", ".hpp"))


def _forbidden(entry):
    """The per-file rule of one rules.json forbidden entry, judged by its own check (gate_forbidden.CHECKS)."""
    check = gate_forbidden.CHECKS.get(entry["id"])
    if check is None:
        raise ValueError(f"rules.json forbidden {entry['id']}: no check judges it in gate_forbidden.CHECKS")
    context = None
    if entry["id"] == "JOB_WITHOUT_KEY":
        context = lambda ctx: sorted(gate_forbidden.job_pool_members(ctx))  # noqa: E731
    elif check is gate_text.untranslated:  # its verdict reads the owner of shown values' class
        context = lambda ctx: [gate_text.shown_owner(ctx)]  # noqa: E731
    return PerFile(entry["id"], [entry["id"]], lambda ctx, path: gate_forbidden.applies(entry, path),
                   lambda ctx, src, err: check(ctx, src, entry, err), context)


def _data_files(ctx):
    return sorted(ctx.rules["schemas"]) + sorted(set(ctx.rules["schemas"].values()))


def per_file_rules(rules):
    return [
        PerFile("junk", ["NO_JUNK"], _everything, gate_files.check_junk),
        PerFile("single_doc", ["SINGLE_DOC"], _everything, gate_files.check_single_doc),
        PerFile("sizes", ["FILE_SIZE", "EMPTY_FILE"], _ext_in("max_lines"), gate_files.check_sizes),
        PerFile("function_sizes", ["FUNCTION_SIZE"], _function_files, gate_files.check_function_sizes),
        PerFile("headers", ["HEADER"], _ext_in("header_comment_extensions"), gate_files.check_headers),
        PerFile("layers", ["LAYER", "UNKNOWN_LAYER"], gate_code.applies_layers, gate_code.check_layers),
        *[_forbidden(e) for e in rules["forbidden"]],
        PerFile("magic_numbers", ["MAGIC_NUMBER"], gate_defs.applies_magic, gate_defs.check_magic_numbers),
        PerFile("ui_state", ["STATE_IN_UI"], gate_ui.applies, gate_ui.check_ui_state, gate_ui.context_inputs),
        PerFile("single_owner", ["SINGLE_OWNER"], gate_defs.applies_owners, gate_defs.check_single_owner),
        PerFile("data_copies", ["SECOND_OWNER", "DUPLICATED_VALUE"], gate_defs.applies_data_copies,
                gate_defs.check_data_copies, lambda ctx: ctx.cache.key_of(_data_files(ctx))),
        PerFile("english", ["NON_ENGLISH_TEXT"], gate_language.applies, gate_language.check_english,
                lambda ctx: ctx.cache.key_of(gate_language.inputs(ctx))),
        PerFile("lab_copy", ["LAB_COPY"], gate_lab.applies, gate_lab.check_lab_copy, gate_lab.store_key),
        PerFile("states", [gate_states.ID], gate_states.applies, gate_states.check_states, gate_states.context),
        PerFile("patch", [gate_patch.ID], gate_patch.applies, gate_patch.check_patch, gate_patch.context),
    ]


def _site_listing():
    import site
    names = []
    for d in site.getsitepackages() + [site.getusersitepackages()]:
        if os.path.isdir(d):
            names += sorted(os.listdir(d))
    return [sys.version, names]


def whole_rules(rules):
    return [
        Whole("root", ["CLEAN_ROOT"], lambda ctx, p: "/" not in p, gate_files.check_root,
              lambda ctx: sorted(os.listdir(ctx.root))),
        Whole("sidecars", ["SIDECAR_WITHOUT_SOURCE"], _in_scope(lambda ctx, p: p.endswith(SIDECARS)), gate_files.check_sidecars,
              lambda ctx: sorted(ctx.listed)),
        Whole("orphans", ["ORPHAN"], _in_scope(gate_code.orphan_inputs), gate_code.check_orphans),
        Whole("map", ["CLAUDE_MD", "MAP_OUTDATED"], gate_docs.inputs, gate_docs.check_claude_md,
              lambda ctx: sorted(ctx.listed)),
        Whole("data", ["DATA", "DATA_WITHOUT_SCHEMA"], _in_scope(gate_data.inputs), gate_data.check_data),
        Whole("register", ["PROBLEM_REGISTER"], gate_data.register_inputs, gate_data.check_register, gate_data.register_extra),
        Whole("materials", ["SHADER_PARAM"], _in_scope(gate_materials.inputs), gate_materials.check_materials),
        Whole("owner_symbols", ["OWNER_MISSING"], gate_defs.owner_inputs, gate_defs.check_owner_symbols,
              lambda ctx: sorted(c["owner"] in ctx.listed_set for c in ctx.owners["concepts"])),
        Whole("clones", ["DUPLICATED_CODE"], _in_scope(gate_clones.applies), gate_clones.check_clones),
        Whole("tuning", ["TUNING", "TUNING_UNUSED"], _in_scope(gate_tuning.inputs), gate_tuning.check_tuning),
        Whole("keys", [gate_keys.ID], _in_scope(gate_keys.inputs), gate_keys.check_keys),
        Whole("mirrors", ["MIRROR"], gate_tuning.mirror_inputs, gate_tuning.check_mirrors),
        Whole("generated", ["GENERATED_DRIFT"], gate_generated.inputs, gate_generated.check_generated,
              lambda ctx: ctx.rules.get("generated", [])),
        Whole("python_imports", ["PYTHON_IMPORT_UNRESOLVED"], gate_python.applies, gate_python.check_imports,
              lambda ctx: _site_listing()),
        Whole("line_endings", ["LINE_ENDINGS"], gate_files.line_ending_inputs, gate_files.check_line_endings,
              lambda ctx: sorted(ctx.changed)),
        Whole("instructions", ["PRECEDENCE_CLAIM", "NESTED_CLAUDE"], gate_docs.instruction_inputs, gate_docs.check_instructions),
        Whole("problems_elsewhere", ["PROBLEMS_ELSEWHERE"], gate_docs.problems_inputs, gate_docs.check_problems_elsewhere,
              lambda ctx: sorted(ctx.listed)),
    ]


def all_rules(rules):
    return per_file_rules(rules) + whole_rules(rules)
