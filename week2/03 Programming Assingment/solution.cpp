#include "ini.h"

namespace Ini {

    Document Load(istream &input) {
        Document result;

        Section *current_section = nullptr;
        for (string line; getline(input, line);) {
            if (!line.empty()) {
                if (line[0] == '[') {
                    current_section = &result.AddSection(line.substr(1, line.size() - 2));
                } else {
                    size_t eq_pos = line.find('=');
                    current_section->insert({line.substr(0, eq_pos), line.substr(eq_pos + 1)});
                }
            }
        }

        return result;
    }

    Section &Document::AddSection(string name) {
        return sections[name];
    }

    const Section &Document::GetSection(const string &name) const {
        return sections.at(name);
    }

    size_t Document::SectionCount() const {
        return sections.size();
    }

} /* namespace Ini */
