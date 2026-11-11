"""The configuration that decides where a JavaScript/TypeScript module specifier leads (J3): a tsconfig/jsconfig's
`compilerOptions.paths`, `baseUrl` and `extends`; a package.json's `name`, `exports`, `module`, `main`, `types`,
`browser`. tsconfig is JSON with comments and trailing commas (JSONC): both are removed outside strings first.
"""
from __future__ import annotations

import json
import re

from .model import FileFacts

TRAILING_COMMA = re.compile(r",(\s*[}\]])")
PACKAGE_FIELDS = ("name", "exports", "module", "main", "types", "typings", "browser")
JSX_OPTIONS = ("jsx", "jsxFactory", "jsxImportSource")      # tsconfig: how JSX is transformed (TS handbook, JSX)


def strip_jsonc(text: str) -> str:
    """JSON with `//` and `/* */` comments and trailing commas, as plain JSON (strings left as written)."""
    out, i, n, quote = [], 0, len(text), ""
    while i < n:
        c = text[i]
        if quote:
            out.append(c)
            if c == "\\" and i + 1 < n:
                out.append(text[i + 1])
                i += 1
            elif c == quote:
                quote = ""
        elif c == '"':
            quote = c
            out.append(c)
        elif text.startswith("//", i):
            end = text.find("\n", i)
            i = n if end < 0 else end
            continue
        elif text.startswith("/*", i):
            end = text.find("*/", i + 2)
            i = n if end < 0 else end + 2
            continue
        else:
            out.append(c)
        i += 1
    return TRAILING_COMMA.sub(r"\1", "".join(out))


def _load(src) -> tuple[dict, int]:
    try:
        data = json.loads(strip_jsonc(src.data.decode("utf-8-sig", "replace")))
        return (data if isinstance(data, dict) else {}), 0
    except ValueError:
        return {}, 1


def read_tsconfig(src) -> FileFacts:
    facts = FileFacts(src.display, "tsconfig")
    data, facts.errors = _load(src)
    options = data.get("compilerOptions") or {}
    facts.config = {"paths": options.get("paths") or {}, "baseUrl": options.get("baseUrl"),
                    "extends": data.get("extends")}
    facts.config.update({k: options[k] for k in JSX_OPTIONS if isinstance(options.get(k), str)})
    return facts


def read_package(src) -> FileFacts:
    facts = FileFacts(src.display, "package")
    data, facts.errors = _load(src)
    facts.config = {k: data[k] for k in PACKAGE_FIELDS if k in data}
    return facts
