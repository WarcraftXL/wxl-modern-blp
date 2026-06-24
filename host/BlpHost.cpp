// Host face for wxl-modern-blp: serves an uncompressed-BGRA texture as a DXT5 texture the Client reads.
// Copyright (C) 2026 WarcraftXL
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include "Host.hpp"

#include "core/Logger.hpp"

#include "../shared/BlpTranscode.hpp"

#include <cctype>
#include <cstdint>
#include <span>
#include <string_view>
#include <vector>

// Fires on every .blp open. A texture in the uncompressed-BGRA encoding (3) the Client cannot decode is
// re-encoded to DXT5; every other texture is served raw (the transcode declines fast after the header).
namespace
{
    bool EndsWithCI(std::string_view s, std::string_view suffix)
    {
        if (suffix.size() > s.size()) return false;
        const size_t off = s.size() - suffix.size();
        for (size_t i = 0; i < suffix.size(); ++i)
            if (std::tolower(static_cast<unsigned char>(s[off + i])) !=
                std::tolower(static_cast<unsigned char>(suffix[i]))) return false;
        return true;
    }

    bool TransformBlp(std::string_view name, std::span<const uint8_t> raw, std::vector<uint8_t>& out)
    {
        if (!EndsWithCI(name, ".blp")) return false;
        const bool ok = wxl::modern::blp::TranscodeBlp(raw, out);
        if (ok)
            wxl::core::log::Printf("modern-blp: %.*s BGRA->DXT5 (%u -> %u bytes)",
                int(name.size()), name.data(), uint32_t(raw.size()), uint32_t(out.size()));
        return ok;
    }

    struct Registrar
    {
        Registrar() { wxl::host::RegisterTransform("modern-blp", &TransformBlp); }
    } g_registrar;
}
