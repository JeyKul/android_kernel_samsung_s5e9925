SECTIONS {
	__patchable_function_entries : { *(__patchable_function_entries) }
	.text : {
		__set_recovery_mode$3435f46b26c05060f9e72a45c090fb5b.cfi_jt = . + 0x87c78;
		__set_recovery_state$3435f46b26c05060f9e72a45c090fb5b.cfi_jt = . + 0x87c88;
		__get_recovery_state$3435f46b26c05060f9e72a45c090fb5b.cfi_jt = . + 0x87cd0;
		__exynos_hdr_update$a8088351f22ad3b9006b532916af48ba.cfi_jt = . + 0x87cd8;
		__exynos_dqe_prepare$5f1a96dadbcce5447e3779ff7aedac2e.cfi_jt = . + 0x87ce0;
		__exynos_dqe_state$5f1a96dadbcce5447e3779ff7aedac2e.cfi_jt = . + 0x87de0;
		__exynos_hdr_disable$a8088351f22ad3b9006b532916af48ba.cfi_jt = . + 0x87e30;
		__drm_atomic_state_free.cfi_jt = . + 0x87e98;
		__exynos_hdr_prepare$a8088351f22ad3b9006b532916af48ba.cfi_jt = . + 0x87f28;
		__exynos_dqe_resume$5f1a96dadbcce5447e3779ff7aedac2e.cfi_jt = . + 0x88380;
	}
}
