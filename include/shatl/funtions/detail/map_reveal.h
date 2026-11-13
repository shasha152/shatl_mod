#pragma once

#include "register.h"
#include "shatl/funtions/detail/class/world.h"
#include "shatl/funtions/detail/class/world_map.h"
#include "shatl/funtions/detail/register.h"
#include "shatl/utils/log.h"
#include <algorithm>
#include <atomic>
#include <memory>
namespace tl {
namespace func {
class map_reveal : public std::enable_shared_from_this<map_reveal> {
    inline static std::atomic_bool is_running = false;

    int map_size_x = 0;
    int map_size_y = 0;
    int chunks_x = 0;
    int chunks_y = 0;
    int curr_chunk = 0;

    world_map *map = nullptr;

    inline static constexpr int chunk_size = 200;

  public:
    map_reveal() noexcept = default;

    bool is_run() noexcept {
        return is_running.load();
    }

    void run() {
        bool expected = false;
        if (!is_running.compare_exchange_strong(expected, true))
            return;

        auto initializer = [self = shared_from_this()]() {
            self->map = world_map::map();
            if (!self->map) {
                is_running.store(false);
                return;
            }

            self->init();
            int total = self->chunks_x * self->chunks_y;
            for (int i = 0; i < total; ++i) {
                register_once_function([self, total]() {
                    if (self->curr_chunk >= total) {
                        return;
                    }

                    int chunk_x = self->curr_chunk % self->chunks_x;
                    int chunk_y = self->curr_chunk / self->chunks_x;

                    int start_x = chunk_x * chunk_size;
                    int start_y = chunk_y * chunk_size;

                    int end_x =
                        std::min(start_x + chunk_size, self->map_size_x);
                    int end_y =
                        std::min(start_y + chunk_size, self->map_size_y);

                    for (int y = start_y; y < end_y; ++y)
                        for (int x = start_x; x < end_x; ++x)
                            self->map->update_lighting(x, y, 255);

                    self->curr_chunk++;

                    if (self->curr_chunk == total) {
                        self->map->save();

                        world::instance()->static_call<void>("set_refreshMap",
                                                             true);
                        is_running.store(false);
                        LOGI("开图完成");
                    }
                });
            }
        };

        register_once_function(initializer);
    }

  private:
    void init() noexcept {
        map_size_x = map->max_width();
        map_size_y = map->max_height();
        chunks_x = (map_size_x + chunk_size - 1) / chunk_size;
        chunks_y = (map_size_y + chunk_size - 1) / chunk_size;
        curr_chunk = 0;
    }
};
} // namespace func
} // namespace tl