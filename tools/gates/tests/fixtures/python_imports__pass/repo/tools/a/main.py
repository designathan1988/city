"""Main."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "lib"))
import helper  # noqa: E402
from pkg import thing  # noqa: E402

try:
    import yaml_not_installed
except ImportError:
    yaml_not_installed = None

if __name__ == "__main__":
    helper.go(thing)
