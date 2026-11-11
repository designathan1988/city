"""The list of gate rules: each per-file rule says which files it judges and what context its result depends on; each
whole-repo rule says which files are its inputs (a --file run triggers it when a requested file is one of them)."""
import os
import sys

import gate_clones
import gate_code
import gate_contracts
import gate_data
import gate_defs
import gate_docs
import gate_files
import gate_forbidden
import gate_generated
import gate_lab
import gate_language
import gate_materials
import gate_python
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
    check = gate_forbidden.CHECKS.get(entry["id"], gate_forbidden.pattern_on_code)
    context = None
    if entry["id"] == "JOB_SEM_CHAVE":
        context = lambda ctx: sorted(gate_forbidden.job_pool_members(ctx))  # noqa: E731
    elif check is gate_text.untranslated:  # its verdict reads the owner of shown values' class
        context = lambda ctx: [gate_text.shown_owner(ctx)]  # noqa: E731
    return PerFile(entry["id"], [entry["id"]], lambda ctx, path: gate_forbidden.applies(entry, path),
                   lambda ctx, src, err: check(ctx, src, entry, err), context)


def _data_files(ctx):
    return sorted(ctx.rules["schemas"]) + sorted(set(ctx.rules["schemas"].values()))


def per_file_rules(rules):
    return [
        PerFile("junk", ["SEM_LIXO"], _everything, gate_files.check_junk),
        PerFile("single_doc", ["DOC_UNICO"], _everything, gate_files.check_single_doc),
        PerFile("sizes", ["TAMANHO_ARQUIVO", "ARQUIVO_VAZIO"], _ext_in("max_lines"), gate_files.check_sizes),
        PerFile("function_sizes", ["TAMANHO_FUNCAO"], _function_files, gate_files.check_function_sizes),
        PerFile("headers", ["CABECALHO"], _ext_in("header_comment_extensions"), gate_files.check_headers),
        PerFile("layers", ["CAMADA", "CAMADA_DESCONHECIDA"], gate_code.applies_layers, gate_code.check_layers),
        *[_forbidden(e) for e in rules["forbidden"]],
        PerFile("magic_numbers", ["NUMERO_MAGICO"], gate_defs.applies_magic, gate_defs.check_magic_numbers),
        PerFile("ui_state", ["ESTADO_NA_UI"], gate_ui.applies, gate_ui.check_ui_state, gate_ui.context_inputs),
        PerFile("single_owner", ["DONO_UNICO"], gate_defs.applies_owners, gate_defs.check_single_owner),
        PerFile("data_copies", ["SEGUNDO_DONO", "VALOR_DUPLICADO"], gate_defs.applies_data_copies,
                gate_defs.check_data_copies, lambda ctx: ctx.cache.key_of(_data_files(ctx))),
        PerFile("english", ["NON_ENGLISH_TEXT"], gate_language.applies, gate_language.check_english,
                lambda ctx: ctx.cache.key_of(gate_language.inputs(ctx))),
        PerFile("lab_copy", ["LAB_COPY"], gate_lab.applies, gate_lab.check_lab_copy, gate_lab.store_key),
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
        Whole("root", ["RAIZ_LIMPA"], lambda ctx, p: "/" not in p, gate_files.check_root,
              lambda ctx: sorted(os.listdir(ctx.root))),
        Whole("sidecars", ["SIDECAR_WITHOUT_SOURCE"], _in_scope(lambda ctx, p: p.endswith(SIDECARS)), gate_files.check_sidecars,
              lambda ctx: sorted(ctx.listed)),
        Whole("orphans", ["ORFAO"], _in_scope(gate_code.orphan_inputs), gate_code.check_orphans),
        Whole("map", ["CLAUDE_MD", "MAPA_DESATUALIZADO"], gate_docs.inputs, gate_docs.check_claude_md,
              lambda ctx: sorted(ctx.listed)),
        Whole("data", ["DADOS", "DADOS_SEM_SCHEMA"], _in_scope(gate_data.inputs), gate_data.check_data),
        Whole("materials", ["SHADER_PARAM"], _in_scope(gate_materials.inputs), gate_materials.check_materials),
        Whole("owner_symbols", ["DONO_AUSENTE"], gate_defs.owner_inputs, gate_defs.check_owner_symbols,
              lambda ctx: sorted(c["owner"] in ctx.listed_set for c in ctx.owners["concepts"])),
        Whole("clones", ["CODIGO_DUPLICADO"], _in_scope(gate_clones.applies), gate_clones.check_clones),
        Whole("tuning", ["TUNING", "TUNING_SEM_USO"], _in_scope(gate_tuning.inputs), gate_tuning.check_tuning),
        Whole("mirrors", ["ESPELHO"], gate_tuning.mirror_inputs, gate_tuning.check_mirrors),
        Whole("generated", ["GENERATED_DRIFT"], gate_generated.inputs, gate_generated.check_generated,
              lambda ctx: ctx.rules.get("generated", [])),
        Whole("python_imports", ["PYTHON_IMPORT_UNRESOLVED"], gate_python.applies, gate_python.check_imports,
              lambda ctx: _site_listing()),
        Whole("contracts", ["CONTRACT_UNGUARDED"], gate_contracts.inputs, gate_contracts.check_contracts, gate_contracts.extra),
    ]


def all_rules(rules):
    return per_file_rules(rules) + whole_rules(rules)
