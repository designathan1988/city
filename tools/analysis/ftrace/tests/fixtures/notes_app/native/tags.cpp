// Tag set used by the search feature; unrelated to storing notes.
#include <string>
#include <vector>

class TagSet {
public:
    void add(const std::string& tag) { tags.push_back(tag); }
    bool has(const std::string& tag) const;
private:
    std::vector<std::string> tags;
};
