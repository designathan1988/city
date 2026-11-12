# The base script a shop extends by its path: `visit` calls `greet`, which the shop overrides.
func visit():
	greet()

func greet():
	return 0
