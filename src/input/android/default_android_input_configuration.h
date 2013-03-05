/*
 * Copyright © 2013 Canonical Ltd.
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
 * Authored by: Robert Carr <robert.carr@canonical.com>
 */

#ifndef MIR_INPUT_ANDROID_DEFAULT_ANDROID_INPUT_CONFIGURATION_H_
#define MIR_INPUT_ANDROID_DEFAULT_ANDROID_INPUT_CONFIGURATION_H_

#include "android_input_configuration.h"

#include <utils/RefBase.h>
#include <utils/StrongPointer.h>

#include <functional>

namespace droidinput = android;

namespace mir
{
namespace input
{
namespace android
{

class DefaultInputConfiguration : public InputConfiguration
{
public:
    DefaultInputConfiguration();
    virtual ~DefaultInputConfiguration();
    
    droidinput::sp<droidinput::EventHubInterface> the_event_hub();

protected:
    DefaultInputConfiguration(DefaultInputConfiguration const&) = delete;
    DefaultInputConfiguration& operator=(DefaultInputConfiguration const&) = delete;

private:
    template <typename Type>
    class CachedAndroidPtr
    {
        droidinput::wp<Type> cache;
        
        CachedAndroidPtr(CachedAndroidPtr const&) = delete;
        CachedAndroidPtr& operator=(CachedAndroidPtr const&) = delete;

    public:
        CachedAndroidPtr() = default;
        
        droidinput::sp<Type> operator()(std::function<droidinput::sp<Type>()> make)
        {
            auto result = cache.promote();
            if (!result.get())
            {
                cache = result = make();
            }
            return result;
        }
    };
    CachedAndroidPtr<droidinput::EventHubInterface> event_hub;
};

}
}
} // namespace mir

#endif // MIR_INPUT_ANDROID_DEFAULT_ANDROID_INPUT_CONFIGURATION_H_
