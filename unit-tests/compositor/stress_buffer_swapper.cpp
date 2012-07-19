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
 * Authored by: Kevin DuBois <kevin.dubois@canonical.com>
 */

#include "mock_buffer.h"

#include "mir/compositor/buffer_swapper_double.h"
#include "multithread_harness.h"

#include <thread>
#include <memory>
#include <functional>

namespace mc = mir::compositor;
namespace mt = mir::testing;
namespace geom = mir::geometry;

struct ThreadFixture {
    public:
        ThreadFixture(
            std::function<void( mt::SynchronizedThread<mc::BufferSwapper, mc::Buffer*>*,
                            std::shared_ptr<mc::BufferSwapper>,
                            mc::Buffer** )> a, 
            std::function<void( mt::SynchronizedThread<mc::BufferSwapper, mc::Buffer*>*,
                            std::shared_ptr<mc::BufferSwapper>,
                            mc::Buffer** )> b)
        {
            geom::Width w {1024};
            geom::Height h {768};
            geom::Stride s {1024};
            mc::PixelFormat pf {mc::PixelFormat::rgba_8888};

            std::unique_ptr<mc::Buffer> buffer_a(new mc::MockBuffer(w, h, s, pf));
            std::unique_ptr<mc::Buffer> buffer_b(new mc::MockBuffer(w, h, s, pf));

            auto swapper = std::make_shared<mc::BufferSwapperDouble>(
                    std::move(buffer_a),
                    std::move(buffer_b));

            auto thread_start_time = std::chrono::system_clock::now();
            auto abs_timeout = thread_start_time + std::chrono::milliseconds(1000);
            t1 = new mt::SynchronizedThread<mc::BufferSwapper, mc::Buffer*>
                                                (abs_timeout, a, swapper, &buffer1 );
            t2 = new mt::SynchronizedThread<mc::BufferSwapper, mc::Buffer*>
                                                (abs_timeout, b, swapper, &buffer2 );
        };

        ~ThreadFixture()
        {
            t2->ensure_child_is_waiting();
            t2->kill_thread();
            t2->activate_waiting_child();

            t1->ensure_child_is_waiting();
            t1->kill_thread();
            t1->activate_waiting_child();

            delete t1;
            delete t2;
        }

        mt::SynchronizedThread<mc::BufferSwapper, mc::Buffer*> *t1;
        mt::SynchronizedThread<mc::BufferSwapper, mc::Buffer*> *t2;
        mc::Buffer *buffer1;
        mc::Buffer *buffer2;        
};


void client_request_loop( mt::SynchronizedThread<mc::BufferSwapper, mc::Buffer*>* synchronizer,
                            std::shared_ptr<mc::BufferSwapper> swapper,
                            mc::Buffer** buf )
{
    for(;;)
    {
        *buf = swapper->dequeue_free_buffer();
        if (synchronizer->child_enter_wait()) return;

        swapper->queue_finished_buffer();
        if (synchronizer->child_enter_wait()) return;
    }
}

void compositor_grab_loop( mt::SynchronizedThread<mc::BufferSwapper, mc::Buffer*>* synchronizer,
                            std::shared_ptr<mc::BufferSwapper> swapper,
                            mc::Buffer** buf )
{
    for(;;)
    {
        *buf = swapper->grab_last_posted();
        if (synchronizer->child_enter_wait()) return;

        swapper->ungrab();
        if (synchronizer->child_enter_wait()) return;

    }

}
/* test that the compositor and the client are never in ownership of the same
   buffer */ 
TEST(buffer_swapper_double_stress, distinct_buffers_in_client_and_compositor)
{
    const int num_iterations = 1000;
    ThreadFixture fix(compositor_grab_loop, client_request_loop);
    for(int i=0; i< num_iterations; i++)
    {
        fix.t1->ensure_child_is_waiting();
        fix.t2->ensure_child_is_waiting();

        EXPECT_NE(fix.buffer1, fix.buffer2);

        fix.t1->activate_waiting_child();
        fix.t2->activate_waiting_child();
    }

}

/* test that we never get an invalid buffer */ 
TEST(buffer_swapper_double_stress, ensure_valid_buffers)
{
    const int num_iterations = 1000;
    ThreadFixture fix(compositor_grab_loop, client_request_loop);

    for(int i=0; i< num_iterations; i++)
    {
        fix.t1->ensure_child_is_waiting();
        fix.t2->ensure_child_is_waiting();

        EXPECT_NE(fix.buffer1, nullptr);
        EXPECT_NE(fix.buffer2, nullptr);

        fix.t1->activate_waiting_child();
        fix.t2->activate_waiting_child();

    }

}

void client_work_timing0( mt::SynchronizedThread<mc::BufferSwapper, mc::Buffer*>* synchronizer,
                            std::shared_ptr<mc::BufferSwapper> swapper,
                            mc::Buffer** buf )
{
    for(;;)
    {
        *buf = swapper->dequeue_free_buffer();
        swapper->queue_finished_buffer();
        if(synchronizer->child_check()) break;
    }
}

void server_work_timing0( mt::SynchronizedThread<mc::BufferSwapper, mc::Buffer*>* synchronizer,
                            std::shared_ptr<mc::BufferSwapper> swapper,
                            mc::Buffer** buf )
{
    for (;;)
    {
        *buf = swapper->grab_last_posted();
        if(synchronizer->child_check()) break;
        swapper->ungrab();
    }
}

/* here we ensure that there is a DQ/Q, and wait for a grab before checking. since we 
   queued buffer A, we should grab buffer A on next grabs */
TEST(buffer_swapper_double_timing, ensure_compositor_gets_last_posted)
{
    ThreadFixture fix(server_work_timing0, client_work_timing0);
    const int num_it = 300;
    for(int i=0; i< num_it; i++)
    {
        fix.t2->ensure_child_is_waiting();
        fix.t1->ensure_child_is_waiting();

        EXPECT_EQ(fix.buffer1, fix.buffer2); 

        fix.t2->activate_waiting_child();
        fix.t1->activate_waiting_child();
    }
}
