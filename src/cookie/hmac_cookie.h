/*
 * Copyright © 2016 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Brandon Schaefer <brandon.schaefer@canonical.com>
 */

#ifndef MIR_COOKIE_HMAC_COOKIE_H_
#define MIR_COOKIE_HMAC_COOKIE_H_

#include "mir/cookie.h"
#include "cookie_format.h"

namespace mir
{
namespace cookie
{

class HMACMirCookie : public mir::cookie::MirCookie
{
public:
    HMACMirCookie() = delete;

    explicit HMACMirCookie(uint64_t const& timestamp,
                       std::vector<uint8_t> const& mac,
                       mir::cookie::Format const& format);

    uint64_t timestamp() const override;
    std::vector<uint8_t> marshall() const override;

    bool operator==(MirCookie const& cookie) const override;
    bool operator!=(MirCookie const& cookie) const override;

private:
    uint64_t timestamp_;
    std::vector<uint8_t> mac_;
    mir::cookie::Format format_;
};

}
}

#endif // MIR_COOKIE_HMAC_COOKIE_H_
