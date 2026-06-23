Version: 6
Closure: {
	C: {
		'mwasplund|libseccomp': { Version: 2.5.9, Digest: 'sha256:13d4dd00af9758bacffac95d987071ecbdfa994de2d86e331dd7f81318bed243', Build: '2', Tool: '0' }
		'mwasplund|wren': { Version: 1.2.0, Digest: 'sha256:6b867a0b48ad53165921bc734032dc245b045898683e744c4df74c2ee06faecc', Build: '2', Tool: '0' }
	}
	'C++': {
		'monitor-host': { Version: '../../monitor/host/', Build: '0', Tool: '0' }
		'monitor-shared': { Version: '../../monitor/shared/', Build: '0', Tool: '0' }
		'mwasplund|cryptopp': { Version: 1.2.9, Digest: 'sha256:53e593acbdfcaa81f285bcb64dabc3d38f36fd9bff7168d3c10a5d9ef752f446', Build: '0', Tool: '0' }
		'mwasplund|detours': { Version: 4.0.15, Digest: 'sha256:1d482c600a7ac1acc36ea43c3d847451aeb5bf146312f40d351ec530c8c04746', Build: '0', Tool: '0' }
		'mwasplund|json11': { Version: 1.1.6, Digest: 'sha256:c6b0981921f926b73e9512d068efde6eb2c5183f6b3a8442bddc67f847d2bfc4', Build: '0', Tool: '0' }
		'mwasplund|opal': { Version: 0.13.5, Digest: 'sha256:44d81d4f032879ac74e753db438765829d6f4b512e68eacb990fb037f12b6e2a', Build: '0', Tool: '0' }
		'mwasplund|reflex': { Version: 5.5.4, Digest: 'sha256:09e879b8c200c6415543686cf81c710712d07286753cdcd77cec08eab8e81bbe', Build: '0', Tool: '0' }
		'mwasplund|soup-test-assert': { Version: 0.5.0, Digest: 'sha256:ae069ae6775ed31b5eb3ba5c05f33fd8e786cb77884991ae642b887cf7e0dde0', Build: '0', Tool: '0' }
		sml: { Version: '../../sml/', Build: '0', Tool: '0' }
		'soup-core': { Version: '../core/', Build: '1', Tool: '0' }
		'soup-native': { Version: './', Build: '0', Tool: '0' }
	}
}
Builds: {
	'0': {
		Wren: {
			'soup|cpp': {
				Version: 0.20.0
				Digest: 'sha256:f4a6bd667a431c011d1a2eb298918db58e05a2d8d8186afea041d51ff1ecee26'
				Artifacts: {
					Linux: 'sha256:1ae4766a4350909e0ac138752f24304622dab75c6d1ab98e87cfc0e4855b405a'
					Windows: 'sha256:3c2596e63a092e5b70a6e34fc4d2aeb141b2a378e548e88523351a14f89e76ac'
				}
			}
		}
	}
	'1': {
		Wren: {
			'soup|cpp': {
				Version: 0.20.0
				Digest: 'sha256:f4a6bd667a431c011d1a2eb298918db58e05a2d8d8186afea041d51ff1ecee26'
				Artifacts: {
					Linux: 'sha256:1ae4766a4350909e0ac138752f24304622dab75c6d1ab98e87cfc0e4855b405a'
					Windows: 'sha256:3c2596e63a092e5b70a6e34fc4d2aeb141b2a378e548e88523351a14f89e76ac'
				}
			}
			'mwasplund|soup-test-cpp': {
				Version: 0.19.0
				Digest: 'sha256:f236684f38d0647cd3fc12c770a96ba103964921691d71adc9b428dbbd1bf947'
			}
		}
	}
	'2': {
		Wren: {
			'soup|c': {
				Version: 0.9.3
				Digest: 'sha256:023fc6b0a29e7e18b10cef32b35774c8d5b5c53a8fd58fda1879f36c73cfc0f1'
				Artifacts: {
					Linux: 'sha256:919b5abd04c906997f6506fa949cd5f5bfcaee189bf1832e5fefaedfbd1722fc'
					Windows: 'sha256:ee746878b7393bcaa789f619c8b120909244376e36f57cdfbb8498499ff7441d'
				}
			}
		}
	}
}
Tools: {
	'0': {
		'C++': {
			'mwasplund|copy': {
				Version: 1.2.0
				Digest: 'sha256:d493afdc0eba473a7f5a544cc196476a105556210bc18bd6c1ecfff81ba07290'
				Artifacts: {
					Linux: 'sha256:cd2e05f53f8e6515383c6b5b5dc6423bda03ee9d4efe7bd2fa74f447495471d2'
					Windows: 'sha256:c4dc68326a11a704d568052e1ed46bdb3865db8d12b7d6d3e8e8d8d6d3fad6c8'
				}
			}
			'mwasplund|mkdir': {
				Version: 1.2.0
				Digest: 'sha256:b423f7173bb4eb233143f6ca7588955a4c4915f84945db5fb06ba2eec3901352'
				Artifacts: {
					Linux: 'sha256:bbf3cd98e44319844de6e9f21de269adeb0dabf1429accad9be97f3bd6c56bbd'
					Windows: 'sha256:4d43a781ed25ae9a97fa6881da7c24425a3162703df19964d987fb2c7ae46ae3'
				}
			}
			'mwasplund|parse-modules': {
				Version: 4.0.0
				Digest: 'sha256:85318f523aac6635109a51f7c0bddcc44fff28da4b7613514db6cefb3ca8ceee'
				Artifacts: {
					Linux: 'sha256:a1d15fada2827293862377a381427c5e75cbca16afcf913e9fcef6346b087a75'
					Windows: 'sha256:ad30416a9a2dc0eebed727e16c8676ae35c3496a3cbe218885af868df4f55d50'
				}
			}
		}
	}
}