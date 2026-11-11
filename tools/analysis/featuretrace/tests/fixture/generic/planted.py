"""A module the gate must refuse (tests/test_generic.py): a table of names and names written in code."""
CALLS = ("push", "pop")


def f(x, ix):
    return x == "forEach" or ix.lookup(x,
                                       "constructor")
