/*
 * Copyright © 2015-2016 Canonical Ltd.
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
 * Authored by: Christopher James Halse Rogers <christopher.halse.rogers@canonical.com>
 */

#ifndef MIR_COOKIE_COOKIE_AUTHORITY_H_
#define MIR_COOKIE_COOKIE_AUTHORITY_H_

#include <memory>
#include <stdexcept>
#include <vector>

#include "cookie.h"

struct MirCookie;
namespace mir
{
namespace cookie
{

using Secret = std::vector<uint8_t>;

struct SecurityCheckFailed : std::runtime_error
{
    SecurityCheckFailed();
};

/**
 * \brief A source of moderately-difficult-to-spoof cookies.
 *
 * The primary motivation for this is to provide event timestamps that clients find it difficult to spoof.
 * This is useful for focus grant and similar operations where shell behaviour should be dependent on
 * the timestamp of the client event that caused the request.
 *
 * Some spoofing protection is desirable; experience with X clients shows that they will go to some effort
 * to attempt to bypass focus stealing prevention.
 *
 */
class CookieAuthority
{
public:
    /**
     * Optimal size for the provided Secret.
     *
     * This is the maximum useful size of the secret key. Keys of greater size
     * will be reduced to this size internally, and keys of smaller size may be
     * internally extended to this size.
     */
    static size_t optimal_secret_size();

    /**
    *   Construction function used to create a CookieAuthority. The secret size must be
    *   no less then minimum_secret_size otherwise an exception will be thrown
    *
    *   \param [in] secret  A filled in secret used to set the key for the hash function
    *   \return             A unique_ptr CookieAuthority
    */
    static std::unique_ptr<CookieAuthority> create_from_secret(Secret const& secret);

    /**
    *   Construction function used to create a CookieAuthority as well as a secret.
    *
    *   \param [out] save_secret  The secret that was created.
    *   \return                   A unique_ptr CookieAuthority
    */
    static std::unique_ptr<CookieAuthority> create_saving_secret(Secret& save_secret);

    /**
    *   Construction function used to create a CookieAuthority and a secret which it keeps internally.
    *
    *   \return                   A unique_ptr CookieAuthority
    */
    static std::unique_ptr<CookieAuthority> create_keeping_secret();

    CookieAuthority(CookieAuthority const& factory) = delete;
    CookieAuthority& operator=(CookieAuthority const& factory) = delete;
    virtual ~CookieAuthority() noexcept = default;

    /**
    *   Creates a cookie attesting the timestamp.
    *
    *   \param [in]  Timestamp to be attested
    *   \return      A unique_ptr MirCookie
    */
    virtual std::unique_ptr<MirCookie> timestamp_to_cookie(uint64_t const& timestamp) = 0;

    /**
    *   Rebuilds a MirCookie from a stream of bytes and validates it
    *
    *   \param [in]  A stream of bytes to be marshalled into a MirCookie
    *   \return      A unique_ptr MirCookie
    */
    virtual std::unique_ptr<MirCookie> unmarshall_cookie(std::vector<uint8_t> const& raw_cookie) = 0;

    static unsigned const minimum_secret_size = 8;
protected:
    CookieAuthority() = default;
};

}
}
#endif // MIR_COOKIE_COOKIE_AUTHORITY_H_
