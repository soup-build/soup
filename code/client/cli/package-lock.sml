Version: 6
Closure: {
	C: {
		'mwasplund|libseccomp': { Version: 2.5.9, Digest: 'sha256:13d4dd00af9758bacffac95d987071ecbdfa994de2d86e331dd7f81318bed243', Build: '2', Tool: '0' }
		'mwasplund|wren': { Version: 1.2.0, Digest: 'sha256:6b867a0b48ad53165921bc734032dc245b045898683e744c4df74c2ee06faecc', Build: '2', Tool: '0' }
	}
	'C++': {
		ftxui: { Version: '../../../../FTXUI/', Build: '0', Tool: '0' }
		'monitor-host': { Version: '../../monitor/host/', Build: '0', Tool: '0' }
		'monitor-shared': { Version: '../../monitor/shared/', Build: '0', Tool: '0' }
		'mwasplund|cryptopp': { Version: 1.2.8, Digest: 'sha256:6d19806db25880437c805a616a3470ddb1b86cba2e6bfd83a378118e1f46e037', Build: '0', Tool: '0' }
		'mwasplund|detours': { Version: 4.0.14, Digest: 'sha256:042d256bb8aecc3c5b58ff5be26c3f21a3ba24de5885663fb7d6a928c18ebfc4', Build: '0', Tool: '0' }
		'mwasplund|opal': { Version: 0.12.6, Digest: 'sha256:eba7621f545cc3f679a72dd034b046e2d0307025342debd9a798f0bc7a45db5b', Build: '0', Tool: '0' }
		'mwasplund|reflex': { Version: 5.5.4, Digest: 'sha256:09e879b8c200c6415543686cf81c710712d07286753cdcd77cec08eab8e81bbe', Build: '0', Tool: '0' }
		'mwasplund|soup-test-assert': { Version: 0.5.0, Digest: 'sha256:ae069ae6775ed31b5eb3ba5c05f33fd8e786cb77884991ae642b887cf7e0dde0', Build: '0', Tool: '0' }
		sml: { Version: '../../sml/', Build: '0', Tool: '0' }
		soup: { Version: './', Build: '0', Tool: '0' }
		'soup-core': { Version: '../core/', Build: '1', Tool: '0' }
		'soup-generate': { Version: '../../generate/', Build: '0', Tool: '0' }
	}
}
Builds: {
	'0': {
		Wren: {
			'soup|cpp': {
				Version: 0.18.2
				Digest: 'sha256:ede3779beb101896e60039f1f3949a736e75bdcf690a01fe367f1ee6027f7da9'
				Artifacts: {
					Linux: 'sha256:acf24733c1b129417414fce37485e3ff22b5adb5c00af91de969feb4aecfc316'
					Windows: 'sha256:902fe0658d6ce048daf2bb9952991dabeb83909197be73f1e0f29598184cbe1d'
				}
			}
		}
	}
	'1': {
		Wren: {
			'soup|cpp': {
				Version: 0.18.2
				Digest: 'sha256:ede3779beb101896e60039f1f3949a736e75bdcf690a01fe367f1ee6027f7da9'
				Artifacts: {
					Linux: 'sha256:acf24733c1b129417414fce37485e3ff22b5adb5c00af91de969feb4aecfc316'
					Windows: 'sha256:902fe0658d6ce048daf2bb9952991dabeb83909197be73f1e0f29598184cbe1d'
				}
			}
			'mwasplund|soup-test-cpp': {
				Version: 0.18.1
				Digest: 'sha256:abeba7430c29089dbb24b932779f231b5f443203772579807067437e34cc4e55'
			}
		}
	}
	'2': {
		Wren: {
			'soup|c': {
				Version: 0.8.0
				Digest: 'sha256:8e816fc36d390ea692f29d799dd2f803e8ef614e685ef33cd9c869019a80f9f2'
				Artifacts: {
					Linux: 'sha256:7625f2e86e62c08279a7daf63ccc0638040f2b0eda3b508d375da6f974c509a0'
					Windows: 'sha256:ab62a89fe58c38b9805d6dc65a107e16aa16adb5033604800ff58a37623c77f9'
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
				Version: 3.0.0
				Digest: 'sha256:c7d7b9e7de3b304c936430be8d94b406b40939550503c0d8b5d3c1a5d10815e2'
				Artifacts: {
					Linux: 'sha256:8a54de2c0495c08349c7f9e073536c30c071cca9c311639c59b10bf11c3ebe79'
					Windows: 'sha256:ce5ab795beffde62d46d00a24a20b5206fa6e560a05c8a5058141aa8bd7e64ad'
				}
			}
		}
	}
}