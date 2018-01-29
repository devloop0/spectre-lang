import <"std/syscall">

namespace std {
	namespace syscall {

		func void bytes_to_stat(byte* stream, type stat* statbuf) {
			statbuf->st_dev = stream[STAT_st_dev_OFFSET]$ as unsigned long* @;

			long* pad1 = stream[STAT_st_pad1_OFFSET]$ as long*;
			statbuf->st_pad1_0 = pad1[0], statbuf->st_pad1_1 = pad1[1],
				statbuf->st_pad1_2 = pad1[2];

			statbuf->st_ino = stream[STAT_st_ino_OFFSET]$ as unsigned long* @;
			statbuf->st_mode = stream[STAT_st_mode_OFFSET]$ as unsigned int* @;
			statbuf->st_nlink = stream[STAT_st_nlink_OFFSET]$ as unsigned long* @;
			statbuf->st_uid = stream[STAT_st_uid_OFFSET]$ as unsigned int* @;
			statbuf->st_gid = stream[STAT_st_gid_OFFSET]$ as unsigned int* @;
			statbuf->st_rdev = stream[STAT_st_rdev_OFFSET]$ as unsigned long* @;

			long* pad2 = stream[STAT_st_pad2_OFFSET]$ as long*;
			statbuf->st_pad2_0 = pad2[0], statbuf->st_pad2_1 = pad2[1];

			statbuf->st_size = stream[STAT_st_size_OFFSET]$ as unsigned long* @;

			long* pad3 = stream[STAT_st_pad3_OFFSET]$ as long*;
			statbuf->st_pad3_0 = pad3[0];

			statbuf->st_atime_tv_sec = stream[STAT_st_atim_tv_sec_OFFSET]$ as unsigned long* @;
			statbuf->st_atime_tv_nsec = stream[STAT_st_atim_tv_nsec_OFFSET]$ as unsigned long* @;
			statbuf->st_mtime_tv_sec = stream[STAT_st_mtim_tv_sec_OFFSET]$ as unsigned long* @;
			statbuf->st_mtime_tv_nsec = stream[STAT_st_mtim_tv_nsec_OFFSET]$ as unsigned long* @;
			statbuf->st_ctime_tv_sec = stream[STAT_st_ctim_tv_sec_OFFSET]$ as unsigned long* @;
			statbuf->st_ctime_tv_nsec = stream[STAT_st_ctim_tv_nsec_OFFSET]$ as unsigned long* @;
			statbuf->st_blksize = stream[STAT_st_blksize_OFFSET]$ as unsigned long* @;
			statbuf->st_blocks = stream[STAT_st_blocks_OFFSET]$ as unsigned long* @;

			long* pad5 = stream[STAT_st_pad5_OFFSET]$ as long*;
			statbuf->st_pad5_0 = pad5[0], statbuf->st_pad5_1 = pad5[1], statbuf->st_pad5_2 = pad5[2],
				statbuf->st_pad5_3 = pad5[3], statbuf->st_pad5_4 = pad5[4], statbuf->st_pad5_5 = pad5[5],
				statbuf->st_pad5_6 = pad5[6], statbuf->st_pad5_6 = pad5[6], statbuf->st_pad5_7 = pad5[7],
				statbuf->st_pad5_8 = pad5[8], statbuf->st_pad5_9 = pad5[9], statbuf->st_pad5_10 = pad5[10],
				statbuf->st_pad5_11 = pad5[11], statbuf->st_pad5_12 = pad5[12], statbuf->st_pad5_12 = pad5[12];
		}
	}
}
