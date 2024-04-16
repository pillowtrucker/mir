/*
 * Copyright © Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 or 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MIR_TEST_DOUBLES_MOCK_CURSOR_LISTENER_H
#define MIR_TEST_DOUBLES_MOCK_CURSOR_LISTENER_H

#include "mir/input/cursor_listener.h"

#include <gmock/gmock.h>

namespace mir
{
namespace test
{
namespace doubles
{

struct MockCursorListener : public input::CursorListener
{
    MOCK_METHOD(void, cursor_moved_to, (float, float), (override));
    ~MockCursorListener() noexcept {}
    void pointer_usable() {}
    void pointer_unusable() {}
};

}
}
}

#endif
