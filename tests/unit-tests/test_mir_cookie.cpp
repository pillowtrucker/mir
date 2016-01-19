/*
 * Copyright © 2015 Canonical Ltd.
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

#include "mir/cookie_authority.h"
#include "mir/cookie.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(MirCookieAuthority, attests_real_timestamp)
{
    std::vector<uint8_t> secret{ 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0xde, 0x01 };
    auto factory = mir::cookie::CookieAuthority::create_from_secret(secret);

    uint64_t mock_timestamp{0x322322322332};

    auto cookie = factory->timestamp_to_cookie(mock_timestamp);
    EXPECT_NO_THROW({
        factory->unmarshall_cookie(cookie->marshall());
    });
}

TEST(MirCookieAuthority, doesnt_attest_faked_mac)
{
    std::vector<uint8_t> secret{ 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0xde, 0x01 };
    auto factory = mir::cookie::CookieAuthority::create_from_secret(secret);

    std::vector<uint8_t> mac{ 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0xde, 0x01 };

    EXPECT_THROW({
        factory->unmarshall_cookie(mac);
    }, mir::cookie::SecurityCheckFailed);
}

TEST(MirCookieAuthority, timestamp_trusted_with_different_secret_doesnt_attest)
{
    std::vector<uint8_t> alice{ 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0xde, 0x01 };
    std::vector<uint8_t> bob{ 0x01, 0x02, 0x44, 0xd8, 0xee, 0x0f, 0xde, 0x01 };

    auto alices_factory = mir::cookie::CookieAuthority::create_from_secret(alice);
    auto bobs_factory   = mir::cookie::CookieAuthority::create_from_secret(bob);

    uint64_t mock_timestamp{0x01020304};

    EXPECT_THROW({
        auto alices_cookie = alices_factory->timestamp_to_cookie(mock_timestamp);
        auto bobs_cookie   = bobs_factory->timestamp_to_cookie(mock_timestamp);

        alices_factory->unmarshall_cookie(bobs_cookie->marshall());
        bobs_factory->unmarshall_cookie(alices_cookie->marshall());
    }, mir::cookie::SecurityCheckFailed);
}

TEST(MirCookieAuthority, throw_when_secret_size_to_small)
{
    std::vector<uint8_t> bob(mir::cookie::CookieAuthority::minimum_secret_size - 1);
    EXPECT_THROW({
        auto factory = mir::cookie::CookieAuthority::create_from_secret(bob);
    }, std::logic_error);
}

TEST(MirCookieAuthority, saves_a_secret)
{
    using namespace testing;
    std::vector<uint8_t> secret;

    mir::cookie::CookieAuthority::create_saving_secret(secret);

    EXPECT_THAT(secret.size(), Ge(mir::cookie::CookieAuthority::minimum_secret_size));
}

TEST(MirCookieAuthority, timestamp_trusted_with_saved_secret_does_attest)
{
    uint64_t timestamp   = 23;
    std::vector<uint8_t> secret;

    auto source_factory = mir::cookie::CookieAuthority::create_saving_secret(secret);
    auto sink_factory   = mir::cookie::CookieAuthority::create_from_secret(secret);
    auto cookie = source_factory->timestamp_to_cookie(timestamp);

    EXPECT_NO_THROW({
        sink_factory->unmarshall_cookie(cookie->marshall());
    });
}

TEST(MirCookieAuthority, internally_generated_secret_has_optimum_size)
{
    using namespace testing;
    std::vector<uint8_t> secret;

    mir::cookie::CookieAuthority::create_saving_secret(secret);

    EXPECT_THAT(secret.size(), Eq(mir::cookie::CookieAuthority::optimal_secret_size()));
}

TEST(MirCookieAuthority, optimal_secret_size_is_larger_than_minimum_size)
{
    using namespace testing;

    EXPECT_THAT(mir::cookie::CookieAuthority::optimal_secret_size(),
        Ge(mir::cookie::CookieAuthority::minimum_secret_size));
}
