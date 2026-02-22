Version: 6
Closure: {
	C: {
		'Samples.C.DynamicLibrary.Application': { Version: './', Build: '0', Tool: '0' }
		'Samples.C.DynamicLibrary.Library': { Version: '../library/', Build: '0', Tool: '0' }
	}
}
Builds: {
	'0': {
		Wren: {
			'Soup|C': {
				Version: 0.5.0
				Digest: 'sha256:1e9683087b84ed998a5ea4a96ee70561e4af5575f46c68d4cdf8a154d405d52b'
				Artifacts: {
					Linux: 'sha256:78475282211521684dd619b99b2cce6edb3034192209aa86317aec2b26a0f207'
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
		}
	}
}