"""Guards against runtime APIs known to corrupt memory: py-tree-sitter 0.26.0 reading Point.row (issue #505)."""
import os
import re
import unittest

PACKAGE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
POINT_ACCESS = re.compile(r'\.(start_point|end_point|range)\b')


class RuntimeSafety(unittest.TestCase):
    def test_no_code_reads_tree_sitter_points(self):
        offenders = []
        for name in sorted(os.listdir(PACKAGE)):
            if name.endswith('.py'):
                with open(os.path.join(PACKAGE, name), encoding='utf-8') as handle:
                    for number, line in enumerate(handle, 1):
                        if POINT_ACCESS.search(line):
                            offenders.append(f'{name}:{number}: {line.strip()}')
        self.assertEqual(offenders, [])


if __name__ == '__main__':
    unittest.main()
