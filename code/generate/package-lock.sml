Version: 6
Closure: {
	C: {
		'mwasplund|libseccomp': { Version: 2.5.9, Digest: 'sha256:13d4dd00af9758bacffac95d987071ecbdfa994de2d86e331dd7f81318bed243', Build: '2', Tool: '0' }
		'mwasplund|wren': { Version: 1.2.0, Digest: 'sha256:6b867a0b48ad53165921bc734032dc245b045898683e744c4df74c2ee06faecc', Build: '2', Tool: '0' }
	}
	'C++': {
		'monitor-host': { Version: '../monitor/host/', Build: '0', Tool: '0' }
		'monitor-shared': { Version: '../monitor/shared/', Build: '0', Tool: '0' }
		'mwasplund|cryptopp': { Version: 1.2.8, Digest: 'sha256:6d19806db25880437c805a616a3470ddb1b86cba2e6bfd83a378118e1f46e037', Build: '0', Tool: '0' }
		'mwasplund|detours': { Version: 4.0.14, Digest: 'sha256:042d256bb8aecc3c5b58ff5be26c3f21a3ba24de5885663fb7d6a928c18ebfc4', Build: '0', Tool: '0' }
		'mwasplund|opal': { Version: 0.12.6, Digest: 'sha256:eba7621f545cc3f679a72dd034b046e2d0307025342debd9a798f0bc7a45db5b', Build: '0', Tool: '0' }
		'mwasplund|reflex': { Version: 5.5.4, Digest: 'sha256:09e879b8c200c6415543686cf81c710712d07286753cdcd77cec08eab8e81bbe', Build: '0', Tool: '0' }
		'mwasplund|soup-test-assert': { Version: 0.5.0, Digest: 'sha256:ae069ae6775ed31b5eb3ba5c05f33fd8e786cb77884991ae642b887cf7e0dde0', Build: '0', Tool: '0' }
		'soup-core': { Version: '../client/core/', Build: '1', Tool: '0' }
		'soup-generate': { Version: './', Build: '0', Tool: '0' }
	}
}
Builds: {
	'0': {
		Wren: {
			'soup|cpp': {
				Version: 0.17.0
				Digest: 'sha256:157d4d471af98055222b09c3b425d42bbd2c27909c1be88186c50245bd6a2b8e'
				Artifacts: {
					Linux: 'sha256:ee52ea68b4d3b3a910d882127b7cb486151f5b79a09e1fba24cca1a8300568da'
					Windows: 'sha256:8cd8c940d941eb33fb5ed4985ed7a0597a5564595e5ef23fc58cb3648e1ebeb3'
				}
			}
		}
	}
	'1': {
		Wren: {
			'soup|cpp': {
				Version: 0.17.0
				Digest: 'sha256:157d4d471af98055222b09c3b425d42bbd2c27909c1be88186c50245bd6a2b8e'
				Artifacts: {
					Linux: 'sha256:ee52ea68b4d3b3a910d882127b7cb486151f5b79a09e1fba24cca1a8300568da'
					Windows: 'sha256:8cd8c940d941eb33fb5ed4985ed7a0597a5564595e5ef23fc58cb3648e1ebeb3'
				}
			}
			'mwasplund|soup-test-cpp': {
				Version: 0.16.0
				Digest: 'sha256:e2415551e273ee1e0becaf85d71c85a80ee9aa6b00a69c7f0495d9be26017350'
			}
		}
	}
	'2': {
		Wren: {
			'soup|c': {
				Version: 0.7.0
				Digest: 'sha256:c940077e1171eaecb60ca2138056622b084ea1d57ab4f28b73c24740f2443df7'
				Artifacts: {
					Linux: 'sha256:f4b05202011eac3129322a273f29e0643eb86ae921196cd0b28028d1ea3d2e8d'
					Windows: 'sha256:93f6c11fbb787cb9ad74300e6eafe379124b67608b45219cdf567dba9ed27570'
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
				Version: 2.0.0
				Digest: 'sha256:41454c7aad2c86fd0ae0a238e7add396764661b344116af97f16a5d663a9b441'
				Artifacts: {
					Linux: 'sha256:3e4731bb5f231c322b8d315977d966b695fcee0e2e17b0828ae975aa728235a7'
					Windows: 'sha256:aaca663e0c951c1e0cbca59f2251d8fa7f53066f97b6ab3c479eb8e0c92daf0f'
				}
			}
		}
	}
}