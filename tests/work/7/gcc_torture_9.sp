import <"std/syscall">

func int main() {
	int ll_bitsize, ll_bitpos;
	int rl_bitsize, rl_bitpos;
	int end_bit;
	ll_bitpos = ll_bitsize = rl_bitsize = 32;
	rl_bitpos = 0;
	end_bit = ll_bitpos + ll_bitsize > rl_bitpos + rl_bitsize ?
		ll_bitpos + ll_bitsize : rl_bitpos + rl_bitsize;
	if(end_bit != 64) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	end_bit = rl_bitpos + rl_bitsize > ll_bitpos + ll_bitsize ?
		rl_bitpos + rl_bitsize : ll_bitpos + ll_bitsize;
	if(end_bit != 64) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	end_bit = ll_bitpos + ll_bitsize < rl_bitpos + rl_bitsize ?
		ll_bitpos + ll_bitsize : rl_bitpos + rl_bitsize;
	if(end_bit != 32) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	end_bit = (((rl_bitpos + rl_bitsize) < (ll_bitpos + ll_bitsize)) ?
		(rl_bitpos + rl_bitsize) : (ll_bitpos + ll_bitsize));
	if(end_bit != 32) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
