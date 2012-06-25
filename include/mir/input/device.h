/*
 * Copyright © 2012 Canonical Ltd.
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
 * Authored by: Thomas Voss <thomas.voss@canonical.com>
 */

#ifndef DEVICE_H_
#define DEVICE_H_

namespace mir
{
namespace input
{

class EventHandler;

// Abstracts an input device that feeds events
// into the system via an event handler.
class Device
{
 public:

    explicit Device (EventHandler * handler)
            : handler(handler)
    {
    }

    virtual ~Device() {}

 protected:
    EventHandler * handler;
};

}
}

#endif /* DEVICE_H_ */
