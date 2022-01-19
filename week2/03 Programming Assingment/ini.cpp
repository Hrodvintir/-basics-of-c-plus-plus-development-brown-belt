#include "ini.h"
#include <unordered_map>

std::pair<std::string, std::string> Split(std::string line, char by) {
    size_t pos = line.find(by);
    std::string left = line.substr(0, pos);
    return {left, line.substr(pos + 1)};
}

Ini::Section &Ini::Document::AddSection(std::string name) {
    return sections[name];
}

const Ini::Section &Ini::Document::GetSection(const std::string &name) const {
    return sections.at(name);
}

size_t Ini::Document::SectionCount() const {
    return sections.size();
}

Ini::Document Ini::Load(std::istream &input) {
    Ini::Document doc;
    std::string name;
    for (std::string line; std::getline(input, line);) {
        if (line == "") {
            continue;
        } else if (line[0] == '[') {
            name = line.substr(1, line.size() - 2);
            doc.AddSection(name);
        } else {
            doc.AddSection(name).insert(Split(line, '='));
        }

    }

    return doc;
}
