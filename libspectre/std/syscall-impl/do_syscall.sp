import <"std/syscall">

namespace std {
	namespace syscall {

		func int do_syscall_return(int ret) {
			if (ret >= ((-4095) as unsigned int))
				ret = -1;
			return ret;
		}

		func int do_syscall4(int sysnum, int a1, int a2, int a3, int a4) {
			int ret;
			__asm__ (
				"push { r7, r8 }" :
				"mov r7, r0" :
				"mov r0, r1" :
				"mov r1, r2" :
				"mov r2, r3" :
				a4 "r12" :
				"ldr r4, [r12]" :
				"svc #0" :
				ret "r1" :
				"str r0, [r1]" :
				"pop { r7, r8 }"
			);
			return do_syscall_return(ret);
		}

		func int do_syscall3(int sysnum, int a1, int a2, int a3) {
			int ret;
			__asm__ (
				"push { r7, r8 }" :
				"mov r7, r0" :
				"mov r0, r1" :
				"mov r1, r2" :
				"mov r2, r3" :
				"svc #0" :
				ret "r1" :
				"str r0, [r1]" :
				"pop { r7, r8 }"
			);
			return do_syscall_return(ret);
		}

		func int do_syscall2(int sysnum, int a1, int a2) {
			int ret;
			__asm__ (
				"push { r7, r8 }" :
				"mov r7, r0" :
				"mov r0, r1" :
				"mov r1, r2" :
				"svc #0" :
				ret "r1" :
				"str r0, [r1]" :
				"pop { r7, r8 }"
			);
			return do_syscall_return(ret);
		}

		func int do_syscall1(int sysnum, int a1) {
			int ret;
			__asm__ (
				"push { r7, r8 }" :
				"mov r7, r0" :
				"mov r0, r1" :
				"svc #0" :
				ret "r1" :
				"str r0, [r1]" :
				"pop { r7, r8 }"
			);
			return do_syscall_return(ret);
		}

		func int do_syscall0(int sysnum) {
			int ret;
			__asm__ (
				"push { r7, r8 }" :
				"mov r7, r0" :
				"svc #0" :
				ret "r1" :
				"str r0, [r1]" :
				"pop { r7, r8 }"
			);
			if (ret != 0) return -1;
			return do_syscall_return(ret);
		}
	}
}
