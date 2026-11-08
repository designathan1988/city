# photodiff: an empty set, a photo on one side only, a missing folder and a uniform shift of one level all fail; equal
# photos pass.
import sys
import tempfile
import unittest
from contextlib import redirect_stdout
from io import StringIO
from pathlib import Path

import numpy as np
from PIL import Image

sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent / "build"))
import photodiff  # noqa: E402

SIZE = 200  # taller than the masked HUD bands together


def photo(path: Path, level: int) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    rng = np.random.default_rng(7)
    pixels = rng.integers(10, 240, size=(SIZE, SIZE, 3)).astype(np.int16) + level
    Image.fromarray(pixels.astype(np.uint8)).save(path)


class PhotodiffTest(unittest.TestCase):
    def setUp(self):
        base = Path(tempfile.mkdtemp(prefix="tasks-photodiff-"))
        self.before, self.after, self.out = base / "before", base / "after", base / "out"
        self.before.mkdir()
        self.after.mkdir()

    def diff(self) -> tuple[int, str]:
        text = StringIO()
        with redirect_stdout(text):
            code = photodiff.main([str(self.before), str(self.after), str(self.out)])
        return code, text.getvalue()

    def test_equal_photos_pass(self):
        photo(self.before / "a.png", 0)
        photo(self.after / "a.png", 0)
        self.assertEqual(self.diff()[0], 0)

    def test_empty_set_fails(self):
        code, text = self.diff()
        self.assertEqual(code, 1)
        self.assertIn("empty set", text)

    def test_photo_on_one_side_fails(self):
        photo(self.before / "a.png", 0)
        photo(self.after / "a.png", 0)
        photo(self.after / "b.png", 0)
        code, text = self.diff()
        self.assertEqual(code, 1)
        self.assertIn("b.png", text)
        photo(self.before / "c.png", 0)
        self.assertEqual(self.diff()[0], 1)

    def test_missing_folder_fails(self):
        self.before.rmdir()
        self.assertEqual(self.diff()[0], 1)

    def test_uniform_shift_of_one_level_fails(self):
        photo(self.before / "a.png", 0)
        photo(self.after / "a.png", 1)
        code, text = self.diff()
        self.assertEqual(code, 1)
        self.assertIn("changed 100.000%", text)
        self.assertIn("shift R +1.000 G +1.000 B +1.000", text)

    def test_usage_error_exits_2(self):
        with redirect_stdout(StringIO()):
            self.assertEqual(photodiff.main([str(self.before)]), 2)


if __name__ == "__main__":
    unittest.main()
