#pragma once

#include "util.hpp"
#include "std.hpp"

struct Timer {
    static constexpr u64 NS_PER_SECOND = 1000000000;

    static constexpr u64 TICK_SPEED = 60;

    u32 ticks, frames = 0;
    u32 fps, tps = 0;
    u64 last_frame, last_second;
    u64 delta;
    u64 tick_remainder = 0;

    Timer() = default;

    void init() {
	this->last_frame = ns_now();
	this->last_second = ns_now();
    }

    inline void tick(std::function<void()> f) {
	const u64 now = Timer::ns_now();

	this->delta = now - this->last_frame;
	this->last_frame = now;

	const u64 ns_per_tick =
	    (NS_PER_SECOND / TICK_SPEED);
	u64 tick_time = this->delta + this->tick_remainder;

	while(tick_time > ns_per_tick) {
	    f();
	    this->ticks++;
	    tick_time -= ns_per_tick;
	}

	tick_remainder = std::max<u64>(tick_time, 0);

	if(now - this->last_second > NS_PER_SECOND) {
	    this->fps = this->frames;
	    this->tps = this->ticks;

	    this->frames = 0;
	    this->ticks = 0;

	    this->last_second = now;

	    log(
		"FPS: " + std::to_string(this->fps)
		+ " TPS: " + std::to_string(this->tps),
		LOG_LEVEL_INFO);
	}
    }

    inline static u64 ns_now() {
	timespec ts;
	timespec_get(&ts, TIME_UTC);
	return ((ts.tv_sec * NS_PER_SECOND) + ts.tv_nsec);
    }
};
