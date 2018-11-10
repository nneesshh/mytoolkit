#pragma once

#define __CPU_PROFILER_ENABLED__
#define __CPU_PROFILER_FULL_TYPE_EXPANSION__

#undef noinline
#undef fastcall

#if defined(_MSC_VER)
#undef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCSIG__
#define CPUPROFILE_CONCAT( a, b ) a "/" b

#define noinline __declspec(noinline)
#define fastcall __fastcall
#else
#define CPUPROFILE_CONCAT( a, b ) b

#define noinline __attribute__ ((noinline))
#define fastcall
#endif

#if defined(__CPU_PROFILER_FULL_TYPE_EXPANSION__)
#define CPUPROFILE_FUNCTION() __PRETTY_FUNCTION__
#else
#define CPUPROFILE_FUNCTION() __FUNCTION__
#endif

#if defined(__CPU_PROFILER_ENABLED__)
// thread
#define CPUPROFILE_THREAD_START_RAW( text )   CpuProfiler::threadenter( text );
#define CPUPROFILE_THREAD_START()             CPUPROFILE_THREAD_START_RAW( CPUPROFILE_FUNCTION()  )
#define CPUPROFILE_THREAD_START_DESC( desc )  CPUPROFILE_THREAD_START_RAW( CPUPROFILE_CONCAT( CPUPROFILE_FUNCTION(), desc ) )

#define CPUPROFILE_THREAD_SCOPED_RAW( text )  CpuProfiler::ScopedThread profiler##__LINE__ ( text );
#define CPUPROFILE_THREAD_SCOPED()            CPUPROFILE_THREAD_SCOPED_RAW( CPUPROFILE_FUNCTION() )
#define CPUPROFILE_THREAD_SCOPED_DESC( desc ) CPUPROFILE_THREAD_SCOPED_RAW( CPUPROFILE_CONCAT( CPUPROFILE_FUNCTION(), desc ) )

#define CPUPROFILE_THREAD_STOP()              CpuProfiler::threadexit();

// function
#define CPUPROFILE_PAUSE()             CpuProfiler::pause();
#define CPUPROFILE_UNPAUSE()           CpuProfiler::unpause();
#define CPUPROFILE_PAUSE_SCOPED()      CpuProfiler::ScopedPause profilerpause##__LINE__;

#define CPUPROFILE_START_RAW( text )   CpuProfiler::enter( text );
#define CPUPROFILE_START()             CPUPROFILE_START_RAW( CPUPROFILE_FUNCTION()  )
#define CPUPROFILE_START_DESC( desc )  CPUPROFILE_START_RAW( CPUPROFILE_CONCAT( CPUPROFILE_FUNCTION(), desc ) )

#define CPUPROFILE_SCOPED_RAW( text )  CpuProfiler::Scoped profiler##__LINE__ ( text );
#define CPUPROFILE_SCOPED()            CPUPROFILE_SCOPED_RAW( CPUPROFILE_FUNCTION() )
#define CPUPROFILE_SCOPED_DESC( desc ) CPUPROFILE_SCOPED_RAW( CPUPROFILE_CONCAT( CPUPROFILE_FUNCTION(), desc ) )

#define CPUPROFILE_STOP()              CpuProfiler::exit();
#else
#define CPUPROFILE_THREAD_START_RAW( text )
#define CPUPROFILE_THREAD_START()
#define CPUPROFILE_THREAD_START_DESC( desc )

#define CPUPROFILE_THREAD_SCOPED_RAW( text )
#define CPUPROFILE_THREAD_SCOPED()
#define CPUPROFILE_THREAD_SCOPED_DESC( desc )

#define CPUPROFILE_THREAD_STOP()

#define CPUPROFILE_PAUSE()
#define CPUPROFILE_UNPAUSE()
#define CPUPROFILE_PAUSE_SCOPED()

#define CPUPROFILE_START_RAW( text )
#define CPUPROFILE_START()
#define CPUPROFILE_START_DESC( desc )

#define CPUPROFILE_SCOPED_RAW( text )
#define CPUPROFILE_SCOPED()
#define CPUPROFILE_SCOPED_DESC( desc )

#define CPUPROFILE_STOP()
#endif

#include <stdint.h>

namespace CpuProfiler {
	/*
	=============
	Types that won't conflict with the rest of the system
	=============
	*/
	typedef float f32;
	typedef double f64;
	typedef uint8_t u8;
	typedef uint16_t u16;
	typedef uint32_t u32;
	typedef uint64_t u64;

	template< class type1, class type2 >
	f64 average(type1 sum, type2 count) {
		return (count) ? f64(sum) / f64(count) : 0;
	}

	/*
	=============
	Timer
	=============
	*/
#pragma pack(push,1)
	struct Timer {
		Timer() { Reset(); }

		inline bool IsEmpty() const { return ticks == 0; }
		inline bool IsPaused() const { return paused; }
		inline void Unpause(u64 curticks) { started = curticks; paused = false; }
		inline void Unpause() { Unpause(getticks()); }
		inline void Pause(u64 curticks) { ticks += (curticks - started); paused = true; }
		inline void Pause() { Pause(getticks()); }
		inline void Start() { ++calls; started = getticks(); }
		inline void Stop() { ticks += (getticks() - started); }
		inline void Reset() { ticks = started = calls = 0; paused = false; }
		inline void SoftStop() { if (!paused) { u64 t = getticks(); ticks += (t - started); started = t; } }
		inline void SoftReset() { ticks = 0; calls = 1; started = getticks(); }

		template< class type > static f64 ms(const type &t) { return f64(t) / 1000000.0; }
		f64 millicycles() { return ms(ticks); }
		f64 currentmillicycles() { return ms(ticks + (getticks() - started)); }
		f64 avg() { return average(ticks, calls); }
		f64 avgms() { return ms(average(ticks, calls)); }

		void operator+= (const Timer &b) {
			ticks += b.ticks;
			calls += b.calls;
		}

		static inline u64 getticks_serial() {
#if defined(__GNUC__)
			asm volatile("cpuid" : : : "%eax", "%ebx", "%ecx", "%edx");
#else
			__asm cpuid;
#endif
			return getticks();
		}

#if defined(__GNUC__)
		static inline u64 getticks() {
			u32 __a, __d;
			asm volatile("rdtsc" : "=a" (__a), "=d" (__d));
			return (u64(__a) | u64(__d) << 32);
		}
#elif defined(__ICC) || defined(__ICL)
		static inline u64 getticks() { return _rdtsc(); }
#else
		static inline u64 getticks() { __asm { rdtsc }; }
#endif

		u64 ticks, started;
		u32 calls;
		bool paused;
	};
#pragma pack(pop)


	/*
	=============
	Interface functions
	=============
	*/

	void detect(int argc, const char *argv[]);
	void detect(const char *commandLine);
	void dump();
	void dumphtml();
	void fastcall enter(const char *name);
	void fastcall exit();
	void fastcall pause();
	void fastcall unpause();
	void threadenter(const char *name);
	void threadexit();
	void reset();

	struct Scoped {
		Scoped(const char *name) { CPUPROFILE_START_RAW(name) }
		~Scoped() { CPUPROFILE_STOP() }
	};

	struct ScopedPause {
		ScopedPause() { CPUPROFILE_PAUSE() }
		~ScopedPause() { CPUPROFILE_UNPAUSE() }
	};

	struct ScopedThread {
		ScopedThread(const char *name) { CPUPROFILE_THREAD_START_RAW(name); }
		~ScopedThread() { CPUPROFILE_THREAD_STOP() }
	};
}; // namespace Profiler


struct ScopedTimer {
	ScopedTimer(CpuProfiler::Timer &t) : mTimer(t) { mTimer.Start(); }
	~ScopedTimer() { mTimer.Stop(); }
protected:
	CpuProfiler::Timer &mTimer;
};