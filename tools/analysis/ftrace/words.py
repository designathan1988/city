"""Vocabulary shared by every stage: identifier splitting, token shapes and path parts."""
import re

WORD = re.compile(r'[A-Z]+(?=[A-Z][a-z])|[A-Z]?[a-z]+|[A-Z]+|[0-9]+')
IDENT = re.compile(r'[^\W\d]\w*')
PATH_SPLIT = re.compile(r'[\\/]')
EXTENSION = re.compile(r'[^.\s]+\.[A-Za-z0-9]+')


def split_words(text):
    """Lower-case words of an identifier or phrase: camelCase, PascalCase, snake_case and digits split apart."""
    return [w.lower() for w in WORD.findall(text)]


def is_identifier(text):
    return IDENT.fullmatch(text) is not None


def convention_key(name):
    """Naming-convention-free key of a compound name (store_note == storeNote); None for one-word names."""
    words = split_words(name)
    return '_'.join(words) if len(words) > 1 else None


def is_compound_identifier(text):
    """A name-shaped identifier: two or more words (place_road, placeRoad) or all capitals (ROAD); prose has neither."""
    if not is_identifier(text):
        return False
    letters = [c for c in text if c.isalpha()]
    return convention_key(text) is not None or (len(letters) > 1 and all(c.isupper() for c in letters))


def path_parts(text):
    """Parts of a path-shaped string (has a separator or a file extension, no spaces); None otherwise.

    A leading scheme or drive ('res:', 'C:') is dropped; '.' and '..' are kept for relative resolution."""
    if not text or any(c.isspace() for c in text) or is_identifier(text):
        return None
    if not PATH_SPLIT.search(text) and not EXTENSION.fullmatch(text):
        return None
    parts = [p for p in PATH_SPLIT.split(text) if p]
    if parts and parts[0].endswith(':'):
        parts = parts[1:]
    return parts or None


def is_absolute(text):
    return text.startswith(('/', '\\')) or (PATH_SPLIT.split(text, 1)[0].endswith(':'))
